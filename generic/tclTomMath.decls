# tclTomMath.decls --
#
#	This file contains the declarations for the functions in 'libtommath'
#	that are contained within the Tcl library.  This file is used to
#	generate the 'tclTomMathDecls.h' and 'tclStubInit.c' files.
#
# If you edit this file, advance the revision number (and the epoch
# if the new stubs are not backward compatible) in tclTomMathDecls.h
#
# Copyright (c) 2005 by Kevin B. Kenny.  All rights reserved.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.

library tcl

# Define the unsupported generic interfaces.

interface tclTomMath
# hooks {tclTomMathInt}
scspec EXTERN

# Declare each of the functions in the Tcl tommath interface

declare 0 {
    int TclBN_epoch(void)
}
declare 1 {
    int TclBN_revision(void)
}

declare 2 {
    int TclBN_mp_add(const mp_int *a, const mp_int *b, mp_int *c)
}
declare 3 {
    int TclBN_mp_add_d(const mp_int *a, mp_digit b, mp_int *c)
}
declare 4 {
    int TclBN_mp_and(const mp_int *a, const mp_int *b, mp_int *c)
}
declare 5 {
    void TclBN_mp_clamp(mp_int *a)
}
declare 6 {
    void TclBN_mp_clear(mp_int *a)
}
declare 7 {
    void TclBN_mp_clear_multi(mp_int *a, ...)
}
declare 8 {
    int TclBN_mp_cmp(const mp_int *a, const mp_int *b)
}
declare 9 {
    int TclBN_mp_cmp_d(const mp_int *a, mp_digit b)
}
declare 10 {
    int TclBN_mp_cmp_mag(const mp_int *a, const mp_int *b)
}
declare 11 {
    int TclBN_mp_copy(const mp_int *a, mp_int *b)
}
declare 12 {
    int TclBN_mp_count_bits(const mp_int *a)
}
declare 13 {
    int TclBN_mp_div(const mp_int *a, const mp_int *b, mp_int *q, mp_int *r)
}
declare 14 {
    int TclBN_mp_div_d(const mp_int *a, mp_digit b, mp_int *q, mp_digit *r)
}
declare 15 {
    int TclBN_mp_div_2(const mp_int *a, mp_int *q)
}
declare 16 {
    int TclBN_mp_div_2d(const mp_int *a, int b, mp_int *q, mp_int *r)
}
declare 17 {
    int TclBN_mp_div_3(const mp_int *a, mp_int *q, mp_digit *r)
}
declare 18 {
    void TclBN_mp_exch(mp_int *a, mp_int *b)
}
declare 19 {
    int TclBN_mp_expt_d(const mp_int *a, mp_digit b, mp_int *c)
}
declare 20 {
    int TclBN_mp_grow(mp_int *a, int size)
}
declare 21 {
    int TclBN_mp_init(mp_int *a)
}
declare 22 {
    int TclBN_mp_init_copy(mp_int *a, const mp_int *b)
}
declare 23 {
    int TclBN_mp_init_multi(mp_int *a, ...)
}
declare 24 {
    int TclBN_mp_init_set(mp_int *a, mp_digit b)
}
declare 25 {
    int TclBN_mp_init_size(mp_int *a, int size)
}
declare 26 {
    int TclBN_mp_lshd(mp_int *a, int shift)
}
declare 27 {
    int TclBN_mp_mod(const mp_int *a, const mp_int *b, mp_int *r)
}
declare 28 {
    int TclBN_mp_mod_2d(const mp_int *a, int b, mp_int *r)
}
declare 29 {
    int TclBN_mp_mul(const mp_int *a, const mp_int *b, mp_int *p)
}
declare 30 {
    int TclBN_mp_mul_d(const mp_int *a, mp_digit b, mp_int *p)
}
declare 31 {
    int TclBN_mp_mul_2(const mp_int *a, mp_int *p)
}
declare 32 {
    int TclBN_mp_mul_2d(const mp_int *a, int d, mp_int *p)
}
declare 33 {
    int TclBN_mp_neg(const mp_int *a, mp_int *b)
}
declare 34 {
    int TclBN_mp_or(const mp_int *a, const mp_int *b, mp_int *c)
}
declare 35 {
    int TclBN_mp_radix_size(const mp_int *a, int radix, int *size)
}
declare 36 {
    int TclBN_mp_read_radix(mp_int *a, const char *str, int radix)
}
declare 37 {
    void TclBN_mp_rshd(mp_int *a, int shift)
}
declare 38 {
    int TclBN_mp_shrink(mp_int *a)
}
declare 39 {
    void TclBN_mp_set(mp_int *a, mp_digit b)
}
declare 40 {
    int TclBN_mp_sqr(const mp_int *a, mp_int *b)
}
declare 41 {
    int TclBN_mp_sqrt(const mp_int *a, mp_int *b)
}
declare 42 {
    int TclBN_mp_sub(const mp_int *a, const mp_int *b, mp_int *c)
}
declare 43 {
    int TclBN_mp_sub_d(const mp_int *a, mp_digit b, mp_int *c)
}
declare 44 {
    int TclBN_mp_to_unsigned_bin(const mp_int *a, unsigned char *b)
}
declare 45 {
    int TclBN_mp_to_unsigned_bin_n(const mp_int *a, unsigned char *b,
	    unsigned long *outlen)
}
declare 46 {
    int TclBN_mp_toradix_n(const mp_int *a, char *str, int radix, int maxlen)
}
declare 47 {
    int TclBN_mp_unsigned_bin_size(const mp_int *a)
}
declare 48 {
    int TclBN_mp_xor(const mp_int *a, const mp_int *b, mp_int *c)
}
declare 49 {
    void TclBN_mp_zero(mp_int *a)
}
declare 61 {
    int TclBN_mp_init_set_int(mp_int *a, unsigned long i)
}
declare 62 {
    int TclBN_mp_set_int(mp_int *a, unsigned long i)
}
declare 63 {
    int TclBN_mp_cnt_lsb(const mp_int *a)
}

# Formerly internal API to allow initialisation of bignums without knowing the
# typedefs of how a bignum works internally.
# Removed in 9.0
#declare 64 {
#    void TclBNInitBignumFromLong(mp_int *bignum, long initVal)
#}
# Removed in 9.0
#declare 65 {
#    void TclBNInitBignumFromWideInt(mp_int *bignum, Tcl_WideInt initVal)
#}
# Removed in 9.0
#declare 66 {
#    void TclBNInitBignumFromWideUInt(mp_int *bignum, Tcl_WideUInt initVal)
#}

# Added in libtommath 1.0
declare 67 {
    int TclBN_mp_expt_d_ex(const mp_int *a, mp_digit b, mp_int *c, int fast)
}
# Added in libtommath 1.0.1
declare 68 {
    int TclBN_mp_set_long_long(mp_int *a, Tcl_WideUInt i)
}
declare 69 {
    Tcl_WideUInt TclBN_mp_get_long_long(const mp_int *a)
}
declare 70 {
    int TclBN_mp_set_long(mp_int *a, unsigned long i)
}
declare 71 {
    unsigned long TclBN_mp_get_long(const mp_int *a)
}
declare 72 {
    unsigned long TclBN_mp_get_int(const mp_int *a)
}

# Added in libtommath 1.1.0
# No longer in use: replaced by mp_and()
#declare 73 {
#    int TclBN_mp_tc_and(const mp_int *a, const mp_int *b, mp_int *c)
#}
# No longer in use: replaced by mp_or()
#declare 74 {
#    int TclBN_mp_tc_or(const mp_int *a, const mp_int *b, mp_int *c)
#}
# No longer in use: replaced by mp_xor()
#declare 75 {
#    int TclBN_mp_tc_xor(const mp_int *a, const mp_int *b, mp_int *c)
#}
declare 76 {
    int TclBN_mp_signed_rsh(const mp_int *a, int b, mp_int *c)
}
declare 77 {
    int TclBN_mp_get_bit(const mp_int *a, int b)
}


# Local Variables:
# mode: tcl
# End:
