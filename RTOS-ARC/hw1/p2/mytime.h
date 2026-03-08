/*
 * from tex/ncsu/classes/os/spring10/hw/hw1/pthreads/threads/src/signal_internals.h
 */

#ifndef MACRO_BEGIN

#define MACRO_BEGIN     do {

#ifndef lint
#define MACRO_END       } while (0)
#else /* lint */
extern int _NEVER_;
#define MACRO_END       } while (_NEVER_)
#endif /* lint */

#endif /* !MACRO_BEGIN */

/*
 * MINUS_NTIME only works if src1 > src2
 */
#define MINUS_NTIME(dst, src1, src2) \
  MACRO_BEGIN \
    if ((src2).tv_nsec > (src1).tv_nsec) { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec - 1; \
      (dst).tv_nsec = ((src1).tv_nsec - (src2).tv_nsec) + 1000000000; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec; \
      (dst).tv_nsec = (src1).tv_nsec - (src2).tv_nsec; \
    } \
  MACRO_END


  #define PLUS_NTIME(dst, src1, src2) \
  MACRO_BEGIN \
    if (1000000000 - (src2).tv_nsec <= (src1).tv_nsec) { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec + 1; \
      (dst).tv_nsec = -1000000000 + (src1).tv_nsec + (src2).tv_nsec; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec; \
      (dst).tv_nsec = (src1).tv_nsec + (src2).tv_nsec; \
    } \
  MACRO_END

#define GT_NTIME(t1, t2) \
      (t1.tv_sec > t2.tv_sec || \
       (t1.tv_sec == t2.tv_sec && \
        t1.tv_nsec > t2.tv_nsec))

#define GTEQ_NTIME(t1, t2) \
      ((t1).tv_sec > (t2).tv_sec || \
       ((t1).tv_sec == (t2).tv_sec && \
        (t1).tv_nsec >= (t2).tv_nsec))

#define LE0_NTIME(t1) \
      ((t1).tv_sec < 0 || \
       ((t1).tv_sec == 0 && \
        (t1).tv_nsec <= 0))

/*
 * MINUS_TIME only works if src1 > src2
 */
#define MINUS_TIME(dst, src1, src2) \
  MACRO_BEGIN \
    if ((src2).tv_usec > (src1).tv_usec) { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec - 1; \
      (dst).tv_usec = ((src1).tv_usec - (src2).tv_usec) + 1000000; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec - (src2).tv_sec; \
      (dst).tv_usec = (src1).tv_usec - (src2).tv_usec; \
    } \
  MACRO_END

#define PLUS_TIME(dst, src1, src2) \
  MACRO_BEGIN \
    if (1000000 - (src2).tv_usec <= (src1).tv_usec) { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec + 1; \
      (dst).tv_usec = -1000000 + (src1).tv_usec + (src2).tv_usec; \
    } \
    else { \
      (dst).tv_sec = (src1).tv_sec + (src2).tv_sec; \
      (dst).tv_usec = (src1).tv_usec + (src2).tv_usec; \
    } \
  MACRO_END

#define GT_TIME(t1, t2) \
      ((t1).tv_sec > (t2).tv_sec || \
       ((t1).tv_sec == (t2).tv_sec && \
        (t1).tv_usec > (t2).tv_usec))

#define GTEQ_TIME(t1, t2) \
      ((t1).tv_sec > (t2).tv_sec || \
       ((t1).tv_sec == (t2).tv_sec && \
        (t1).tv_usec >= (t2).tv_usec))

#define LE0_TIME(t1) \
      ((t1).tv_sec < 0 || \
       ((t1).tv_sec == 0 && \
        (t1).tv_usec <= 0))

#define P2U_TIME(dst, src) \
  MACRO_BEGIN \
  (dst).tv_sec = (src).tv_sec; \
  (dst).tv_usec = (src).tv_nsec / 1000; \
  MACRO_END

#define U2P_TIME(dst, src) \
  MACRO_BEGIN \
  (dst).tv_sec = (src).tv_sec; \
  (dst).tv_nsec = (src).tv_usec * 1000; \
  MACRO_END

#define ISNTIMERSET(tp) ((tp).tv_sec || (tp).tv_nsec)
#define NTIMERCLEAR(tp) (tp).tv_sec = (tp).tv_nsec = 0

