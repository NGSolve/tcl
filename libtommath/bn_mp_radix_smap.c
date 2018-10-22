#include "tommath_private.h"
#ifdef BN_MP_RADIX_SMAP_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

/* chars used in radix conversions */
const char *const mp_s_rmap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
const uint8_t mp_s_rmap_reverse[] = {
   0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f, /* ()*+,-./ */
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 01234567 */
   0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 89:;<=>? */
   0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, /* @ABCDEFG */
   0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, /* HIJKLMNO */
   0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, /* PQRSTUVW */
   0x21, 0x22, 0x23, 0xff, 0xff, 0xff, 0xff, 0xff, /* XYZ[\]^_ */
   0xff, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, /* `abcdefg */
   0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, /* hijklmno */
   0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, /* pqrstuvw */
   0x3b, 0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, /* xyz{|}~. */
};
const size_t mp_s_rmap_reverse_sz = sizeof(mp_s_rmap_reverse);
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
