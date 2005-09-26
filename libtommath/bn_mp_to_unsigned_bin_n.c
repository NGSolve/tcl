#include <tommath.h>
#ifdef BN_MP_TO_UNSIGNED_BIN_N_C
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
 *
 * Tom St Denis, tomstdenis@iahu.ca, http://math.libtomcrypt.org
 */

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin_n (mp_int * a, unsigned char *b, unsigned long *outlen)
{
   if (*outlen < (unsigned long)mp_unsigned_bin_size(a)) {
      return MP_VAL;
   }
   *outlen = mp_unsigned_bin_size(a);
   return mp_to_unsigned_bin(a, b);
}
#endif

/* $Source: /root/tcl/repos-to-convert/tcl/libtommath/bn_mp_to_unsigned_bin_n.c,v $ */
/* $Revision: 1.1.1.1.2.2 $ */
/* $Date: 2005/09/26 20:16:54 $ */
