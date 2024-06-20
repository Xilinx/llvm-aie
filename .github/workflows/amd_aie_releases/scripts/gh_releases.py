import os
import time

from github import Github
import datetime

# Authentication is defined via github.Auth
from github import Auth

# using an access token
auth = Auth.Token(os.environ["GITHUB_TOKEN"])

twomonthsago = datetime.date.today() - datetime.timedelta(days=30)

# should be an env variable...
REPO_NAME = "Xilinx/llvm-aie"
# get like this https://stackoverflow.com/a/72132792/9045206
RELEASE_ID = 153575086

# First create a Github instance:

# Public Web Github
g = Github(auth=auth)

n_deleted = 0
for _ in range(100):
    n_deleted = 0
    repo = g.get_repo(REPO_NAME)
    release = repo.get_release(RELEASE_ID)
    assets = release.get_assets()
    for ass in assets:
        if ass.created_at.date() < twomonthsago:
            print(ass.name)
            assert ass.delete_asset()
            n_deleted += 1

    if n_deleted == 0:
        break
    time.sleep(5)

if n_deleted > 0:
    raise Exception("missed some")
