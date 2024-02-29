#ifndef __AIEV2_VLD_SPARSE_H
#define __AIEV2_VLD_SPARSE_H

typedef char v64c __attribute__((__vector_size__(64)));
typedef short v64s __attribute__((__vector_size__(64)));

/*The intrinsics which return a sparse type are implemented using builtins
that take a local variable passed by reference and inserting the result into it.
This is to overcome the limitation of the builtin type signature being able to
return one type only and we have to return 2 (the vector and the mask)*/

/*v64int16_sparse_compress*/
INTRINSIC(v64int16_sparse)
sparse_pop(v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVecTmp;
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_set_lo(
      sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  __builtin_aiev2_sparse_pop_16_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, sparseVecTmp.data,
      (__int128_t &)sparseVecTmp.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_pop_set_lo(v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_set_lo(sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_pop_insert_hi(v64int16_sparse vec, v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_pop_and_get_pointer(v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_and_get_pointer(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_peek_set_lo(v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_set_lo(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_peek_insert_hi(v64int16_sparse vec, v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64int16_sparse)
sparse_peek_and_get_pointer(v64int16_sparse_compress *&P) {
  v64int16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_and_get_pointer(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v64int16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v64int16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v64int16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v64int16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16((int *&)P);
}

/*v64uint16_sparse_compress*/
INTRINSIC(v64uint16_sparse)
sparse_pop(v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVecTmp;
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_set_lo(
      (v64s &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  __builtin_aiev2_sparse_pop_16_insert_hi(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v64s &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_pop_set_lo(v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_set_lo((v64s &)sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_pop_insert_hi(v64uint16_sparse vec, v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_insert_hi(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64s &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_pop_and_get_pointer(v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_and_get_pointer(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_peek_set_lo(v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_set_lo(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_peek_insert_hi(v64uint16_sparse vec, v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_insert_hi(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64s &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64uint16_sparse)
sparse_peek_and_get_pointer(v64uint16_sparse_compress *&P) {
  v64uint16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_and_get_pointer(
      (v64s &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v64uint16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v64uint16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v64uint16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v64uint16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16((int *&)P);
}

/*v128int8_sparse_compress*/

INTRINSIC(v128int8_sparse)
sparse_pop(v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVecTmp;
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_set_lo(
      (v64c &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  __builtin_aiev2_sparse_pop_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v64c &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_pop_set_lo(v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_set_lo((v64c &)sparseVec.data,
                                      (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_pop_insert_hi(v128int8_sparse vec, v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64c &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_pop_and_get_pointer(v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_and_get_pointer(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_peek_set_lo(v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_set_lo((v64c &)sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_peek_insert_hi(v128int8_sparse vec, v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64c &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128int8_sparse)
sparse_peek_and_get_pointer(v128int8_sparse_compress *&P) {
  v128int8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_and_get_pointer(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v128int8_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_8_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v128int8_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_8((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v128int8_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_8_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v128int8_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_8((int *&)P);
}

/*v128uint8_sparse_compress*/

INTRINSIC(v128uint8_sparse)
sparse_pop(v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVecTmp;
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_set_lo(
      (v64c &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  __builtin_aiev2_sparse_pop_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v64c &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_pop_set_lo(v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_set_lo((v64c &)sparseVec.data,
                                      (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_pop_insert_hi(v128uint8_sparse vec, v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64c &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_pop_and_get_pointer(v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_pop_8_and_get_pointer(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_peek_set_lo(v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_set_lo((v64c &)sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_peek_insert_hi(v128uint8_sparse vec, v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_insert_hi(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (v64c &)vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v128uint8_sparse)
sparse_peek_and_get_pointer(v128uint8_sparse_compress *&P) {
  v128uint8_sparse sparseVec;
  __builtin_aiev2_sparse_peek_8_and_get_pointer(
      (v64c &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v128uint8_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_8_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v128uint8_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_8((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v128uint8_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_8_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v128uint8_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_8((int *&)P);
}

/*v256int4_sparse_compress*/

INTRINSIC(v256int4_sparse)
sparse_pop(v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVecTmp;
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_set_lo((v16int32 &)sparseVecTmp.data,
                                      (__int128_t &)sparseVecTmp.mask,
                                      (int *&)P);
  __builtin_aiev2_sparse_pop_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask,
      (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_pop_set_lo(v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_set_lo((v16int32 &)sparseVec.data,
                                      (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_pop_insert_hi(v256int4_sparse vec, v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)vec.data, (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_pop_and_get_pointer(v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_and_get_pointer(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_peek_set_lo(v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_set_lo((v16int32 &)sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_peek_insert_hi(v256int4_sparse vec, v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)vec.data, (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256int4_sparse)
sparse_peek_and_get_pointer(v256int4_sparse_compress *&P) {
  v256int4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_and_get_pointer(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v256int4_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_4_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v256int4_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_4((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v256int4_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_4_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v256int4_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_4((int *&)P);
}

/*v256uint4_sparse_compress*/

INTRINSIC(v256uint4_sparse)
sparse_pop(v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVecTmp;
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_set_lo((v16int32 &)sparseVecTmp.data,
                                      (__int128_t &)sparseVecTmp.mask,
                                      (int *&)P);
  __builtin_aiev2_sparse_pop_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask,
      (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_pop_set_lo(v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_set_lo((v16int32 &)sparseVec.data,
                                      (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_pop_insert_hi(v256uint4_sparse vec, v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)vec.data, (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_pop_and_get_pointer(v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_pop_4_and_get_pointer(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_peek_set_lo(v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_set_lo((v16int32 &)sparseVec.data,
                                       (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_peek_insert_hi(v256uint4_sparse vec, v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_insert_hi(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask,
      (v16int32 &)vec.data, (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v256uint4_sparse)
sparse_peek_and_get_pointer(v256uint4_sparse_compress *&P) {
  v256uint4_sparse sparseVec;
  __builtin_aiev2_sparse_peek_4_and_get_pointer(
      (v16int32 &)sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v256uint4_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_4_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v256uint4_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_4((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v256uint4_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_4_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v256uint4_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_4((int *&)P);
}

/*v64bfloat16_sparse_compress*/
INTRINSIC(v64bfloat16_sparse)
sparse_pop(v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVecTmp;
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_bfloat_set_lo(
      sparseVecTmp.data, (__int128_t &)sparseVecTmp.mask, (int *&)P);
  __builtin_aiev2_sparse_pop_16_bfloat_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, sparseVecTmp.data,
      (__int128_t &)sparseVecTmp.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_pop_set_lo(v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_bfloat_set_lo(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_pop_insert_hi(v64bfloat16_sparse vec, v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_bfloat_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_pop_and_get_pointer(v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_pop_16_bfloat_and_get_pointer(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_peek_set_lo(v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_bfloat_set_lo(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_peek_insert_hi(v64bfloat16_sparse vec, v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_bfloat_insert_hi(
      sparseVec.data, (__int128_t &)sparseVec.mask, vec.data,
      (__int128_t &)vec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(v64bfloat16_sparse)
sparse_peek_and_get_pointer(v64bfloat16_sparse_compress *&P) {
  v64bfloat16_sparse sparseVec;
  __builtin_aiev2_sparse_peek_16_bfloat_and_get_pointer(
      sparseVec.data, (__int128_t &)sparseVec.mask, (int *&)P);
  return sparseVec;
}

INTRINSIC(void)
sparse_reset_and_get_pointer(v64bfloat16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_reset(v64bfloat16_sparse_compress *&P) {
  __builtin_aiev2_sparse_reset_16((int *&)P);
}

INTRINSIC(void)
sparse_fill_and_get_pointer(v64bfloat16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16_and_get_pointer((int *&)P);
}

INTRINSIC(void)
sparse_fill(v64bfloat16_sparse_compress *&P) {
  __builtin_aiev2_sparse_fill_16((int *&)P);
}

#endif /* __AIEV2_VLD_SPARSE_H */
