#include <tommath.h>
#ifdef BN_MP_SQRMOD_C
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

/* c = a * a (mod b) */
int
mp_sqrmod (mp_int * a, mp_int * b, mp_int * c)
{
  int     res;
  mp_int  t;

  if ((res = mp_init (&t)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_sqr (a, &t)) != MP_OKAY) {
    mp_clear (&t);
    return res;
  }
  res = mp_mod (&t, b, c);
  mp_clear (&t);
  return res;
}
#endif

/* $Source: /root/tcl/repos-to-convert/tcl/libtommath/bn_mp_sqrmod.c,v $ */
/* $Revision: 1.1.1.1.2.1 $ */
/* $Date: 2005/09/26 20:16:53 $ */
