#include <tommath_private.h>
#ifdef BN_MP_MOD_D_C
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
 * Tom St Denis, tstdenis82@gmail.com, http://libtom.org
 */

int
mp_mod_d (mp_int * a, mp_digit b, mp_digit * c)
{
  return mp_div_d(a, b, NULL, c);
}
#endif

/* ref:         HEAD -> release/1.0.1, tag: v1.0.1-rc2 */
/* git commit:  e8c27ba7df0efb90708029115c94d681dfa7812f */
/* commit time: 2017-08-29 10:48:46 +0200 */
