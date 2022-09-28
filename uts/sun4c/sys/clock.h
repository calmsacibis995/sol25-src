/*
 * Copyright (c) 1989, 1993 by Sun Microsystems, Inc.
 */

#ifndef _SYS_CLOCK_H
#define	_SYS_CLOCK_H

#pragma ident	"@(#)clock.h	1.21	94/11/21 SMI"

#include <sys/spl.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Definitions and structures for the hi resolution counters
 * on sun4c machines. These counters are normally used to generate
 * 100hz clock interrupts, but are fully programmable for whatever
 * purpose desired.
 */
#define	OBIO_COUNTER_ADDR	0xF3000000	/* addr in obio space */
#define	COUNTER_ADDR		0xFFFF7000	/* virtual addr we map to */
#ifndef _ASM
struct counterregs {
	u_int	counter10;
	u_int	limit10;
	u_int	counter14;
	u_int	limit14;
};
#define	COUNTER	((volatile struct counterregs *)(COUNTER_ADDR))

#ifdef	_KERNEL

extern void	set_clk_mode(u_char, u_char);

#endif	/* _KERNEL */

#endif	/* _ASM */

#define	CTR_LIMIT_BIT		0x80000000	/* limit bit mask */
#define	CTR_USEC_MASK		0x7FFFFC00	/* counter/limit mask */
#define	CTR_USEC_SHIFT		10		/* counter/limit shift */

/*
 * Definitions for the Mostek 48T02 clock chip. We use this chip as
 * our TOD clock. Clock interrupts are generated by a separate timer
 * circuit.
 */

#define	YRBASE		68	/* 1968 - what year 0 in chip represents */

#define	NANOSEC	1000000000
#define	ADJ_SHIFT 4		/* used in get_hrestime and _level10 */

#define	CLOCK_ADDR 0xFFFF87F8	/* virtual address we map clock to be at */

#ifndef _ASM
struct mostek48T02 {
	volatile u_char	clk_ctrl;	/* ctrl register */
	volatile u_char	clk_sec;	/* counter - seconds 0-59 */
	volatile u_char	clk_min;	/* counter - minutes 0-59 */
	volatile u_char	clk_hour;	/* counter - hours 0-23 */
	volatile u_char	clk_weekday;	/* counter - weekday 1-7 */
	volatile u_char	clk_day;	/* counter - day 1-31 */
	volatile u_char	clk_month;	/* counter - month 1-12 */
	volatile u_char	clk_year;	/* counter - year 0-99 */
};
#define	CLOCK ((struct mostek48T02 *)(CLOCK_ADDR))
#endif	/* _ASM */

/*
 * Bit masks for various operations and register limits.
 */
#define	CLK_CTRL_WRITE		0x80
#define	CLK_CTRL_READ		0x40
#define	CLK_CTRL_SIGN		0x20

#define	CLK_STOP		0x80
#define	CLK_KICK		0x80
#define	CLK_FREQT		0x40

/*
 * CLOCK_LOCK() puts a "ff" in the lowest byte of the hres_lock. The
 * higher three bytes are used as a counter. This lock is acquired
 * around "hrestime" and "timedelta". This lock is acquired to make
 * sure that level10 accounts for changes to this variable in that
 * interrupt itself. The level10 interrupt code also acquires this
 * lock.
 *
 * CLOCK_UNLOCK() increments the lower bytes straight, thus clearing the
 * lock and also incrementing the 3 byte counter. This way GET_HRESTIME()
 * can figure out if the value in the lock got changed or not.
 */
#define	HRES_LOCK_OFFSET 3

#define	CLOCK_LOCK()	\
	lock_set_spl(((lock_t *)&hres_lock) + HRES_LOCK_OFFSET, 	\
						ipltospl(LOCK_LEVEL))

#define	CLOCK_UNLOCK(spl)	\
	hres_lock++;		\
	(void) splx(spl)

/*
 * NOTE: the macros below assume that the various time-related variables
 * (hrtime_base, vtrace_time_base, hrestime, timedelta, etc) are all
 * stored together at a 64-byte boundary.  The real motivation is cache
 * performance, but here we take advantage of the side effect that all
 * these variables have the same high 22 address bits -- thus, only one
 * sethi is required.
 */

/*
 * macro to get high res time in nanoseconds since boot to the register
 * pair outh/outl, using register pair scrh/scrl and nslt for scratch.
 * These must be specified as five distinct registers!
 *
 * WARNING: branches are hand-computed to prevent hidden conflicts with
 * local labels in the caller.  If you ever change these macros, make
 * sure you recompute the branch targets.
 */
#define	GET_HRTIME(outh, outl, scrh, scrl, nslt)			\
/* 1 */	sethi	%hi(hrtime_base), scrh;		/* time base addr */	\
	ldd	[scrh + %lo(hrtime_base)], outh; /* read time base */	\
	sethi	%hi(COUNTER_ADDR + CTR_COUNT10), scrl;			\
	ld	[scrl + %lo(COUNTER_ADDR + CTR_COUNT10)], nslt; /* read ctr */\
	ldd	[scrh + %lo(hrtime_base)], scrh; /* re-read time base */\
	sub	scrl, outl, scrl;		/* low bit diff */	\
	sub	scrh, outh, scrh;		/* high bit diff */	\
	orcc	scrl, scrh, %g0;		/* time base changed? */\
	bne	. - 8*4;	/* 1b */	/* yes, try again */	\
	addcc	nslt, nslt, nslt;		/* test & clear limit bit 31 */\
	srl	nslt, 7, scrl;			/* 2048u / 128 = 16u */	\
	sub	nslt, scrl, nslt;		/* 2048u - 16u = 2032u */\
	sub	nslt, scrl, nslt;		/* 2032u - 16u = 2016u */\
	sub	nslt, scrl, nslt;		/* 2016u - 16u = 2000u */\
	bcc	. + 5*4;	/* 2f */	/* limit bit not set */ \
	srl	nslt, 1, nslt;			/* delay: 2000u / 2 = nsec */\
	sethi	%hi(nsec_per_tick), scrh;				\
	ld	[scrh + %lo(nsec_per_tick)], scrh;			\
	add	nslt, scrh, nslt;		/* add 1 tick for limit bit */\
/* 2 */	addcc	outl, nslt, outl;		/* add nsec since last tick */\
	addx	outh, %g0, outh;		/* to hrtime_base */

/*
 * This macro returns the value of hrestime, hrestime_adj and the counter.
 * It assumes that the adj and hrest are register pairs. This macro
 * is called from trap (0x27) in sparc_subr.s.
 */
#define	GET_HRESTIME(out, scr, scr1, adj, hrest)			\
	sethi	%hi(COUNTER_ADDR + CTR_COUNT10), out;			\
	ld	[out + %lo(COUNTER_ADDR + CTR_COUNT10)], out; /* read ctr */\
	addcc	out, out, out;			/* test & clear limit bit 31 */\
	srl	out, 7, scr;			/* 2048u / 128 = 16u */	\
	sub	out, scr, out;			/* 2048u - 16u = 2032u */\
	sub	out, scr, out;			/* 2032u - 16u = 2016u */\
	sub	out, scr, out;			/* 2016u - 16u = 2000u */\
	bcc	. + 5*4;	/* 1f */	/* limit bit not set */ \
	srl	out, 1, out;			/* delay: 2000u / 2 = nsec */\
	sethi	%hi(nsec_per_tick), scr;				\
	ld	[scr + %lo(nsec_per_tick)], scr;			\
	add	out, scr, out;		/* add 1 tick for limit bit */	\
/* 1 */	sethi	%hi(hrtime_base), scr;		/* scr = hi bits for all */\
	ld	[scr + %lo(hres_last_tick)], adj;			\
	sub	out, adj, out;						\
	ldd	[scr + %lo(hrestime)], hrest;	/* load hrestime */	\
	ldd	[scr + %lo(hrestime_adj)], adj; /* load hrestime_adj */

/*
 * This macro is here to support vtrace 3.x, which is microsecond-based.
 * This will go away with vtrace 4.0.0, which will be nanosecond-based.
 */
#define	GET_VTRACE_TIME(outl, scr1, scr2)				\
/* 1 */	sethi	%hi(vtrace_time_base), scr1;	/* time base addr */	\
	ld	[scr1 + %lo(vtrace_time_base)], outl; /* read time base */\
	sethi	%hi(COUNTER_ADDR + CTR_COUNT10), scr2;			\
	ld	[scr2 + %lo(COUNTER_ADDR + CTR_COUNT10)], scr2; /* read ctr */\
	addcc	scr2, scr2, scr2;		/* test & clear limit bit 31 */\
	bcc	. + 5*4;	/* 2f */	/* limit bit not set */ \
	srl	scr2, CTR_USEC_SHIFT + 1, scr2;	/* delay: convert to usec */\
	sethi	%hi(usec_per_tick), scr1;				\
	ld	[scr1 + %lo(usec_per_tick)], scr1;			\
	add	scr2, scr1, scr2;		/* add 1 tick for limit bit */\
/* 2 */	add	outl, scr2, outl;		/* add counter value */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_CLOCK_H */
