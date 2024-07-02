import shutil
from datetime import datetime
import os
import platform
import re
import subprocess
import sys
from pathlib import Path, PureWindowsPath
from pprint import pprint

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


def check_env(build):
    return os.environ.get(build, 0) in {"1", "true", "True", "ON", "YES"}


class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


def get_exe_suffix():
    if platform.system() == "Windows":
        suffix = ".exe"
    else:
        suffix = ""
    return suffix


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()
        install_dir = extdir / "llvm-aie"
        cfg = "Release"
        src_dir = Path(ext.sourcedir)

        cmake_generator = os.environ.get("CMAKE_GENERATOR", "Ninja")

        RUN_TESTS = 1 if check_env("RUN_TESTS") else 0
        # make windows happy
        PYTHON_EXECUTABLE = str(Path(sys.executable))
        if platform.system() == "Windows":
            PYTHON_EXECUTABLE = PYTHON_EXECUTABLE.replace("\\", "\\\\")

        cmake_args = [
            f"-B{build_temp}",
            f"-G {cmake_generator}",
            # load defaults from cache
            "-DLLVM_BUILD_LLVM_DYLIB=ON",
            "-DLLVM_LINK_LLVM_DYLIB=ON",
            "-DLLVM_BUILD_BENCHMARKS=OFF",
            "-DLLVM_BUILD_EXAMPLES=OFF",
            f"-DLLVM_BUILD_TESTS={RUN_TESTS}",
            "-DLLVM_BUILD_TOOLS=ON",
            "-DLLVM_BUILD_UTILS=ON",
            "-DLLVM_CCACHE_BUILD=ON",
            "-DLLVM_ENABLE_ASSERTIONS=ON",
            "-DLLVM_ENABLE_RTTI=ON",
            "-DLLVM_ENABLE_ZSTD=OFF",
            "-DLLVM_INCLUDE_BENCHMARKS=OFF",
            "-DLLVM_INCLUDE_EXAMPLES=OFF",
            f"-DLLVM_INCLUDE_TESTS={RUN_TESTS}",
            "-DLLVM_ENABLE_WARNINGS=ON",
            "-DLLVM_INCLUDE_TOOLS=ON",
            "-DLLVM_INCLUDE_UTILS=ON",
            "-DLLVM_INSTALL_UTILS=ON",
            # get rid of that annoying af git on the end of .17git
            "-DLLVM_VERSION_SUFFIX=",
            # Disables generation of "version soname" (i.e. libFoo.so.<version>), which
            # causes pure duplication of various shlibs for Python wheels.
            "-DCMAKE_PLATFORM_NO_VERSIONED_SONAME=ON",
            f"-DCMAKE_INSTALL_PREFIX={install_dir}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}",
            f"-DPython3_EXECUTABLE={PYTHON_EXECUTABLE}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
            f'-C {LLVM_AIE_SRC_ROOT / "clang" / "cmake" / "caches" / "Peano-AIE.cmake"}',
        ]

        if platform.system() == "Windows":
            cmake_args += [
                "-DCMAKE_C_COMPILER=cl",
                "-DCMAKE_CXX_COMPILER=cl",
                "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded",
                "-DCMAKE_C_FLAGS=/MT",
                "-DCMAKE_CXX_FLAGS=/MT",
                "-DLLVM_USE_CRT_MINSIZEREL=MT",
                "-DLLVM_USE_CRT_RELEASE=MT",
            ]

        if "CMAKE_ARGS" in os.environ:
            cmake_args += [item for item in os.environ["CMAKE_ARGS"].split(" ") if item]

        build_args = []
        if self.compiler.compiler_type != "msvc":
            if not cmake_generator or cmake_generator == "Ninja":
                try:
                    import ninja

                    ninja_executable_path = Path(ninja.BIN_DIR) / "ninja"
                    cmake_args += [
                        "-GNinja",
                        f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
                    ]
                except ImportError:
                    pass

        else:
            single_config = any(x in cmake_generator for x in {"NMake", "Ninja"})
            contains_arch = any(x in cmake_generator for x in {"ARM", "Win64"})
            if not single_config and not contains_arch:
                PLAT_TO_CMAKE = {
                    "win32": "Win32",
                    "win-amd64": "x64",
                    "win-arm32": "ARM",
                    "win-arm64": "ARM64",
                }
                cmake_args += ["-A", PLAT_TO_CMAKE[self.plat_name]]
            if not single_config:
                cmake_args += [
                    f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}"
                ]
                build_args += ["--config", cfg]

        if sys.platform.startswith("darwin"):
            osx_version = os.getenv("OSX_VERSION", "11.6")
            cmake_args += [f"-DCMAKE_OSX_DEPLOYMENT_TARGET={osx_version}"]
            # Cross-compile support for macOS - respect ARCHFLAGS if set
            archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
            if archs:
                cmake_args += ["-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))]

        if "PARALLEL_LEVEL" not in os.environ:
            build_args += [f"-j{str(2 * os.cpu_count())}"]
        else:
            build_args += [f"-j{os.environ.get('PARALLEL_LEVEL')}"]

        print("ENV", pprint(os.environ), file=sys.stderr)
        print("CMAKE_ARGS", cmake_args, file=sys.stderr)

        subprocess.run(
            ["cmake", ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        if check_env("DEBUG_CI_FAST_BUILD"):
            subprocess.run(
                ["cmake", "--build", ".", "--target", "llvm-tblgen", *build_args],
                cwd=build_temp,
                check=True,
            )
            shutil.rmtree(install_dir / "bin", ignore_errors=True)
            shutil.copytree(build_temp / "bin", install_dir / "bin")
        else:
            subprocess.run(
                [
                    "cmake",
                    "--build",
                    ".",
                    "--target",
                    "install-distribution",
                    *build_args,
                ],
                cwd=build_temp,
                check=True,
            )
            if RUN_TESTS:
                env = os.environ.copy()
                # PYTHONPATH needs to be set to find build deps like numpy
                # https://github.com/llvm/llvm-project/pull/89296
                env["MLIR_LIT_PYTHONPATH"] = os.pathsep.join(sys.path)
                subprocess.run(
                    ["cmake", "--build", ".", "--target", "check-all", *build_args],
                    cwd=build_temp,
                    env=env,
                    check=False,
                )
            shutil.rmtree(install_dir / "python_packages", ignore_errors=True)

        subprocess.run(
            [
                "find",
                ".",
                "-exec",
                "touch",
                "-a",
                "-m",
                "-t",
                "197001010000",
                "{}",
                ";",
            ],
            cwd=install_dir,
            check=False,
        )


LLVM_AIE_SRC_ROOT = Path(
    os.getenv("LLVM_AIE_SRC_ROOT", Path.cwd() / "llvm-aie")
).absolute()

cmake_txt = open(LLVM_AIE_SRC_ROOT / "llvm" / "CMakeLists.txt").read()
llvm_version = []
for v in ["LLVM_VERSION_MAJOR", "LLVM_VERSION_MINOR", "LLVM_VERSION_PATCH"]:
    vn = re.findall(rf"set\({v} (\d+)\)", cmake_txt)
    assert vn, f"couldn't find {v} in cmake txt"
    llvm_version.append(vn[0])

commit_hash = os.getenv("LLVM_AIE_PROJECT_COMMIT", "DEADBEEF")
if not commit_hash:
    raise ValueError("commit_hash must not be empty")

now = datetime.now()
llvm_datetime = os.getenv(
    "DATETIME", f"{now.year}{now.month:02}{now.day:02}{now.hour:02}"
)

version = f"{llvm_version[0]}.{llvm_version[1]}.{llvm_version[2]}.{llvm_datetime}+{commit_hash}"
llvm_url = f"https://github.com/Xilinx/llvm-aie/commit/{commit_hash}"

build_temp = Path.cwd() / "build" / "temp"
if not build_temp.exists():
    build_temp.mkdir(parents=True)

setup(
    name="llvm-aie",
    version=version,
    description=f"LLVM-AIE distribution as wheel. Created at {now} build of {llvm_url}",
    long_description=f"LLVM-AIE distribution as wheel. Created at {now} build of [Xilinx/llvm-aie/{commit_hash}]({llvm_url})",
    long_description_content_type="text/markdown",
    ext_modules=[CMakeExtension("llvm-aie", sourcedir="llvm-aie/llvm")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    download_url=llvm_url,
)
