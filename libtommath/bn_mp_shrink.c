#include <tommath_private.h>
#ifdef BN_MP_SHRINK_C
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

/* shrink a bignum */
int mp_shrink (mp_int * a)
{
  mp_digit *tmp;
  int used = 1;
  
  if(a->used > 0) {
    used = a->used;
  }
  
  if (a->alloc != used) {
    if ((tmp = OPT_CAST(mp_digit) XREALLOC (a->dp, sizeof (mp_digit) * used)) == NULL) {
      return MP_MEM;
    }
    a->dp    = tmp;
    a->alloc = used;
  }
  return MP_OKAY;
}
#endif

/* ref:         HEAD -> release/1.0.1, tag: v1.0.1-rc2 */
/* git commit:  e8c27ba7df0efb90708029115c94d681dfa7812f */
/* commit time: 2017-08-29 10:48:46 +0200 */
