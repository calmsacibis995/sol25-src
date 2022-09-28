/*
 * Copyright 1990 Sun Microsystems, Inc.
 */

#ident	"@(#)map.s	1.16	93/05/10 SMI"

/*
 * Sun-4 MMU and Virtual Address Cache routines.
 *
 * Notes:
 *
 * - Hardware cache flush must work in page size chunks.
 * - Cache size <= 1 MB, line size <= 256 bytes.
 * - Assumes vac_flush() performance is not critical.
 */

#include <sys/param.h>
#include <sys/asm_linkage.h>
#include <sys/mmu.h>
#include <sys/pte.h>
#include <sys/enable.h>
#include <sys/cpu.h>

#if defined(lint)
#include <vm/hat_sunm.h>
#endif /* lint */

#if !defined(lint)
#include "assym.s"
#endif /* !lint */

/*
 * Read the page map entry for the given address v
 * and return it in a form suitable for software use.
 *
 * u_int map_getpgmap(caddr_t v);
 */

#if defined(lint)

/*ARGSUSED*/
u_int
map_getpgmap(caddr_t v)
{ return (0); }

#else	/* lint */

	ENTRY(map_getpgmap)
	andn	%o0, 0x3, %o1		! align to word boundary
	retl
	lda	[%o1]ASI_PM, %o0	! read page map entry
	SET_SIZE(map_getpgmap)

#endif	/* lint */

/*
 * Set the pme for address v using the software pte given.
 *
 * void map_setpgmap(caddr_t v, u_int pte);
 */

#if defined(lint)

/*ARGSUSED*/
void
map_setpgmap(caddr_t v, u_int pte)
{}

#else	/* lint */

	ENTRY(map_setpgmap)
	andn	%o0, 0x3, %o2		! align to word boundary
	retl
	sta	%o1, [%o2]ASI_PM	! write page map entry
	SET_SIZE(map_setpgmap)

#endif	/* lint */

/*
 * Return the 16 bit segment map entry for the given segment number.
 *
 * u_int map_getsgmap(caddr_t v);
 */

#if defined(lint)

/*ARGSUSED*/
u_int
map_getsgmap(caddr_t v)
{ return (0); }

#else	/* lint */

	ENTRY(map_getsgmap)
#ifdef	sun4
	andn	%o0, 0x3, %o0		! align to word boundary
	lduha	[%o0]ASI_SM, %o0	! read segment number
#else
	lduba	[%o0]ASI_SM, %o0	! read segment number
#endif
	sethi	%hi(segmask), %o2	! need to mask bits due to bug in cobra
	ld	[%o2 + %lo(segmask)], %o2
	retl
	and	%o0, %o2, %o0		! A HACK BECAUSE COBRA IS BROKEN
	SET_SIZE(map_getsgmap)

#endif	/* lint */

/*
 * Set the segment map entry for segno to pm.
 *
 * void map_setsgmap(caddr_t v, u_int pm);
 */

#if defined(lint)

/*ARGSUSED*/
void
map_setsgmap(caddr_t v, u_int pm)
{}

#else	/* lint */

	ENTRY(map_setsgmap)
#ifdef sun4c
	retl
	stba	%o1, [%o0]ASI_SM	! write segment entry
#else
	andn	%o0, 0x3, %o0		! align to word boundary
	retl
	stha	%o1, [%o0]ASI_SM	! write segment entry
#endif
	SET_SIZE(map_setsgmap)

#endif	/* lint */

#ifdef MMU_3LEVEL
/*
 * Return the 16 bit region map entry for the given region number.
 */

#if	defined(lint)

/*ARGSUSED*/
u_int
map_getrgnmap(caddr_t v)
{ return (0); }

#else	/* lint */
	ENTRY(map_getrgnmap)
	andn	%o0, 0x1, %o0		! align to halfword boundary
	or	%o0, 0x2, %o0
	lduha	[%o0]ASI_RM, %o0	! read region number
	retl
	srl	%o0, 0x8, %o0
	SET_SIZE(map_getrgnmap)

#endif	/* lint */

/*
 * Set the segment map entry for segno to pm.
 */

#if	defined(lint)

/*ARGSUSED*/
void
map_setrgnmap(caddr_t v, u_int pm)
{}

#else	/* lint */

	ENTRY(map_setrgnmap)
	andn	%o0, 0x1, %o0		! align to halfword boundary
	or	%o0, 0x2, %o0
	sll	%o1, 0x8, %o1
	retl
	stha	%o1, [%o0]ASI_RM	! write region entry
	SET_SIZE(map_setrgnmap)

#endif	/* lint */
#endif /* MMU_3LEVEL */

/*
 * Return the current context number.
 *
 * u_int map_getctx(void);
 */

#if defined(lint)

u_int
map_getctx(void)
{ return (0); }

#else	/* lint */

	ENTRY(map_getctx)
	set	CONTEXT_REG, %o1
	retl
	lduba	[%o1]ASI_CTL, %o0	! read the context register
	SET_SIZE(map_getctx)

#endif	/* lint */

/*
 * Set the current context number.
 *
 * void map_setctx(u_int c);
 */

#if defined(lint)

/*ARGSUSED*/
void
map_setctx(u_int c)
{}

#else	/* lint */

	ENTRY(map_setctx)
	set	CONTEXT_REG, %o1
	retl
	stba	%o0, [%o1]ASI_CTL	! write the context register
	SET_SIZE(map_setctx)

#endif	/* lint */

#ifdef	sun4c
/*
 * cache config word: _vac_info
 */
#if 0
struct {
	vi_size : 21;		/* cache size, bytes */
	vi_vac : 1;		/* vac enabled */
	vi_hw : 1;		/* HW flush */
	vi_linesize : 9;	/* line size, bytes */
} vac_info;

#endif /* 0 */

#if !defined(lint)

	.reserve vac_info, 4, ".bss", 4

#define	VAC_INFO_VSS		11		/* VAC size shift (32 - 21) */
#define	VAC_INFO_VAC		(0x400)		/* VAC bit */
#define	VAC_INFO_HW		(0x200)		/* HW flush bit */
#define	VAC_INFO_LSM		(0x1ff)		/* line size mask */

/* note: safe in .empty slot */
/* NOT WHEN COMPILED WITH PROFILING, A SAVE GETS SLAMMED INTO THE DELAY */
/* SLOT OF THE RET ! ! */

#define	GET(val, p, d) \
	sethi %hi(val), p; \
	ld [p + %lo(val)], d;

#define	GET_INFO(p, d)	GET(vac_info, p, d)

#endif	/* lint */

/*
 * Set up VAC config word from imported variables, set vac, clear tags,
 * enable/disable cache.
 *
 * void vac_control(int on);
 */

#if defined(lint)

/*ARGSUSED*/
void
vac_control(int on)
{}

#else	/* lint */

	ENTRY(vac_control)

! encode VAC params
	GET(vac_size, %g1, %o1)			! get VAC size
	GET(vac_hwflush, %g2, %o2)		! get HW flush flag
	GET(vac_linesize, %g1, %o3)		! get line size
	sll	%o1, VAC_INFO_VSS, %o1		! encode VAC size
	sethi	%hi(vac), %g2			! prepare to write "vac"
	tst	%o2				! HW flush?
	bz	1f
	st	%o0, [%g2 + %lo(vac)]		! write "vac"
	bset	VAC_INFO_HW, %o1		! encode HW flush
1:	tst	%o0				! enabling VAC?
	bz	2f
	bset	%o3, %o1			! merge line size
	bset	VAC_INFO_VAC, %o1		! encode VAC enabled
2:	sethi	%hi(vac_info), %g1		! prepare to write vac_info
	set	off_enablereg, %g2		! prepare to disable
	bz	3f
	st	%o1, [%g1 + %lo(vac_info)]	! write vac_info

! enabling cache
	mov	%o7, %g1			! save ret addr
	call	vac_tagsinit			! wipe cache
	nop   
	set	on_enablereg, %g2		! prepare to enable
	mov	%g1, %o7			! restore ret addr

! do it
3:	jmp	%g2
	mov	ENA_CACHE, %o0
	SET_SIZE(vac_control)

#endif	/* lint */
#else	/* sun4c */

/*
 * Function to mark a page as noncachable
 */

#if defined(lint)

/*ARGSUSED*/
void
vac_dontcache(caddr_t v)
{}

#else	/* lint */

	ENTRY(vac_dontcache)
	andn    %o0, 0x3, %g1		! align to word boundary
	lda	[%g1]ASI_PM, %o0	! read old page map entry
	set	PG_NC, %g2
	or	%o0, %g2, %o0		! turn on NOCACHE bit
	retl
	sta	%o0, [%g1]ASI_PM	! and write it back out
	SET_SIZE(vac_dontcache)
#endif	/* lint */
#endif	/* sun4c */


/*
 * Initialize the cache by invalidating all the cache tags.
 * DOES NOT turn on cache enable bit in the enable register.
 *
 * This is also vac_flushall for write-through caches.
 *
 * void vac_tagsinit(void);
 */

#if defined(lint)

void
vac_tagsinit(void)
{}

#else	/* lint */

	ENTRY(vac_tagsinit)
#ifdef	sun4c
	GET_INFO(%o5, %o2)		! get VAC info
	set	CACHE_TAGS, %o3		! address of cache tags in CTL space
	srl	%o2, VAC_INFO_VSS, %o1	! cache size
	and	%o2, VAC_INFO_LSM, %o2	! line size
#else
	sethi	%hi(vac_linesize), %o0	!
	ld	[%o0 + %lo(vac_linesize)], %o2	! line size
	sethi	%hi(vac_size), %o1	!
	ld	[%o1 + %lo(vac_size)], %o1	! cache size
	set	CACHE_TAGS, %o3		! address of cache tags in CTL space
#endif

#ifdef SIMUL
	/*
	 * Don't clear entire cache in the hardware simulator,
	 * it takes too long and the simulator has already cleared it
	 * for us.
	 */
	set	256, %o1		! initialize only 256 bytes worth
#endif SIMUL

1:	subcc	%o1, %o2, %o1		! done yet?
	bg	1b
	sta	%g0, [%o3+%o1]ASI_CTL	! clear tag

	retl
	nop   
	SET_SIZE(vac_tagsinit)

#endif	/* lint */

/*
 * Flush a context from the cache.
 * To flush a context we must cycle through all lines of the
 * cache issuing a store into alternate space command for each
 * line whilst the context register remains constant.
 *
 * void vac_ctxflush(void);
 */

#if defined(lint)

void
vac_ctxflush(void)
{}

#else	/* lint */

	ENTRY(vac_ctxflush)
#ifdef	sun4c
	GET_INFO(%o5, %o2)		! get VAC info
	sethi	%hi(flush_cnt+FM_CTX), %g1 ! get flush count
	btst	VAC_INFO_VAC, %o2	! check if cache is turned on
	bz	9f			! cache off, return
	srl	%o2, VAC_INFO_VSS, %o1	! cache size
	ld	[%g1 + %lo(flush_cnt+FM_CTX)], %g2 ! get flush count
	btst	VAC_INFO_HW, %o2	! HW flush?
	inc	%g2			! increment flush count
	bz	2f			! use SW flush
	st	%g2, [%g1 + %lo(flush_cnt+FM_CTX)] ! store flush count

! hardware flush

	set	NBPG, %o2

1:	subcc	%o1, %o2, %o1
	bg	1b
	sta	%g0, [%o1]ASI_FCC_HW

	retl
	nop   

! software flush

2:	and	%o2, VAC_INFO_LSM, %o2	! line size
	add	%o2, %o2, %o3		! LS * 2
	add	%o2, %o3, %o4		! LS * 3
	add	%o2, %o4, %o5		! LS * 4
	add	%o2, %o5, %g1		! LS * 5
	add	%o2, %g1, %g2		! LS * 6
	add	%o2, %g2, %g3		! LS * 7
	add	%o2, %g3, %g4		! LS * 8

3:	subcc	%o1, %g4, %o1
	sta	%g0, [%o1 + %g0]ASI_FCC
	sta	%g0, [%o1 + %o2]ASI_FCC
	sta	%g0, [%o1 + %o3]ASI_FCC
	sta	%g0, [%o1 + %o4]ASI_FCC
	sta	%g0, [%o1 + %o5]ASI_FCC
	sta	%g0, [%o1 + %g1]ASI_FCC
	sta	%g0, [%o1 + %g2]ASI_FCC
	bg	3b
	sta	%g0, [%o1 + %g3]ASI_FCC

9:	retl
	nop    
#else	/* sun4c */
	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp	! get a new window

	retl
	nop
1:
	sethi	%hi(vac_size), %l0	! cachesize / number of steps in loop
	ld	[%l0 + %lo(vac_size)], %l0
	srl	%l0, 4, %l0
	sethi	%hi(vac_linesize), %i5
	ld	[%i5 + %lo(vac_linesize)], %i5
	sethi	%hi(flush_cnt+FM_CTX), %i2 ! increment flush count
	ld	[%i2 + %lo(flush_cnt+FM_CTX)], %i3
	inc	%i3
	!
	! MJ: sun4c doesn't call flush_user_windows. Bug in sun4c?
	!
	call	flush_user_windows	! make sure no windows are hanging out
	st	%i3, [%i2 + %lo(flush_cnt+FM_CTX)]

	!
	! preload a bunch of offsets
	! Avoid going through the cache sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	sub	%l0, %i5, %i0		! cachesize/16 - linesize
	add	%l0, %l0, %l1		! cachesize/16*2
	add	%l1, %l0, %l2		! cachesize/16*3
	add	%l2, %l0, %l3		! ...
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4

	sta	%g0, [%i0]ASI_FCC
2:
	sta	%g0, [%i0 + %l0]ASI_FCC
	sta	%g0, [%i0 + %l1]ASI_FCC
	sta	%g0, [%i0 + %l2]ASI_FCC
	sta	%g0, [%i0 + %l3]ASI_FCC
	sta	%g0, [%i0 + %l4]ASI_FCC
	sta	%g0, [%i0 + %l5]ASI_FCC
	sta	%g0, [%i0 + %l6]ASI_FCC
	sta	%g0, [%i0 + %l7]ASI_FCC
	sta	%g0, [%i0 + %o0]ASI_FCC
	sta	%g0, [%i0 + %o1]ASI_FCC
	sta	%g0, [%i0 + %o2]ASI_FCC
	sta	%g0, [%i0 + %o3]ASI_FCC
	sta	%g0, [%i0 + %o4]ASI_FCC
	sta	%g0, [%i0 + %o5]ASI_FCC
	sta	%g0, [%i0 + %i4]ASI_FCC
	subcc	%i0, %i5, %i0			! generate next address
	bge,a	2b				! are we done yet?
	sta	%g0, [%i0]ASI_FCC

	ret
	restore
#endif	/* sun4c */
	SET_SIZE(vac_ctxflush)

#endif	/* lint */

#ifndef	sun4c
/*
 * Flush all non-supervisor lines from the cache.
 * Cycle through all lines of the cache issuing a store into
 * alternate space command for each line.
 * We'll start at the end and work backwards to use only
 * one variable for the loop and test.
 */

#if	defined(lint)

void vac_usrflush(void) {}

#else	/* lint */

	ENTRY(vac_usrflush)
	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp ! get a new window

	retl
	nop
1:
	sethi	%hi(vac_size), %l0
	ld	[%l0 + %lo(vac_size)], %l0
	sethi	%hi(vac_linesize), %i5
	ld	[%i5 + %lo(vac_linesize)], %i5
	sethi	%hi(flush_cnt+FM_USR), %i2 ! increment flush count
	ld	[%i2 + %lo(flush_cnt+FM_USR)], %i3
	srl	%l0, 4, %l0		! cachesize / 16 (# steps in loop)
	inc	%i3
	call	flush_user_windows	! make sure no windows are hanging out
	st	%i3, [%i2 + %lo(flush_cnt+FM_USR)]

	!
	!
	!
	sethi	%hi(vac_hashwusrflush), %g1
	ld	[%g1 + %lo(vac_hashwusrflush)], %g1
	tst	%g1
	bz,a	.vac_fake_usrflush
	mov	%i5, %i2

	!
	! preload a bunch of offsets
	! Avoid going through the cache sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	sub	%l0, %i5, %i0		! cachesize/16 - 16
	add	%l0, %l0, %l1		! cahcesize/16*2
	add	%l1, %l0, %l2		! cahcesize/16*3
	add	%l2, %l0, %l3		! ...
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4

	sta	%g0, [%i0]ASI_FCU
2:
	sta	%g0, [%i0 + %l0]ASI_FCU
	sta	%g0, [%i0 + %l1]ASI_FCU
	sta	%g0, [%i0 + %l2]ASI_FCU
	sta	%g0, [%i0 + %l3]ASI_FCU
	sta	%g0, [%i0 + %l4]ASI_FCU
	sta	%g0, [%i0 + %l5]ASI_FCU
	sta	%g0, [%i0 + %l6]ASI_FCU
	sta	%g0, [%i0 + %l7]ASI_FCU
	sta	%g0, [%i0 + %o0]ASI_FCU
	sta	%g0, [%i0 + %o1]ASI_FCU
	sta	%g0, [%i0 + %o2]ASI_FCU
	sta	%g0, [%i0 + %o3]ASI_FCU
	sta	%g0, [%i0 + %o4]ASI_FCU
	sta	%g0, [%i0 + %o5]ASI_FCU
	sta	%g0, [%i0 + %i4]ASI_FCU
	subcc	%i0, %i5, %i0			! generate next address
	bge,a	2b				! are we done yet?
	sta	%g0, [%i0]ASI_FCU
	ret
	restore

!
! Flush all user lines from the cache.  We do this by reading a portion
! of the kernel text starting at sys_trap. The size of the portion is
! equal to the VAC size. We read a word from each line. sys_trap was chosen
! as the start address because it is the start of the locore code
! that we assume will be very likely executed in near future.
!
! We branch here from vac_usrflush if VAC does not support 'user flush' in HW.
! We have a new register window.
!

.vac_fake_usrflush:
	!
	! Here we have:
	!  	i2	linesize
	!	l0	cachesize / 16
	!

	!
	! Due to a bug in HW, some processors must map the trap vectors
	! non cacheable. Software (locore.s) must guarantee that the
	! code that follows the trap vectors starts in the next page.
	! We are paranoid about it and check that sys_trap is actually
	! in a cacheable page. We panic otherwise.
	!
	cmp	%i3, 1
	set	sys_trap, %i0		! start reading text seg. from sys_trap
	bne	2f
	mov	%l0, %i3		! loop counter: vac_size / 16

	! check pte only the first time vac_fake_usrflush is called
	lda	[%i0]ASI_PM, %l2	! read page map entry
	set	PG_NC, %l1
	btst	%l1, %l2
	bz	2f
	nop

	sethi	%hi(6f), %o0
	call	panic
	or	%o0, %lo(6f), %o0

2:
	!
	! A flush that causes a writeback will happen in parallel
	! with other instructions.  Back to back flushes which cause
	! writebacks cause the processor to wait until the first writeback
	! is finished and the second is initiated before proceeding.
	! Avoid going through the cache sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	!  i0 	start address
	!  i2 	linesize
	!  i3   vac_size / 16
	!  l0	vac_size / 16
	!
	add	%l0, %l0, %l1		! 2 * (vac_size / 16)
	add	%l1, %l0, %l2	! ...
	add	%l2, %l0, %l3
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4		! 15 * (vac_size / 16)

3:
	ld	[%i0      ], %i1
	ld	[%i0 + %l0], %i1
	ld	[%i0 + %l1], %i1
	ld	[%i0 + %l2], %i1
	ld	[%i0 + %l3], %i1
	ld	[%i0 + %l4], %i1
	ld	[%i0 + %l5], %i1
	ld	[%i0 + %l6], %i1
	ld	[%i0 + %l7], %i1
	ld	[%i0 + %o0], %i1
	ld	[%i0 + %o1], %i1
	ld	[%i0 + %o2], %i1
	ld	[%i0 + %o3], %i1
	ld	[%i0 + %o4], %i1
	ld	[%i0 + %o5], %i1
	ld	[%i0 + %i4], %i1

	subcc	%i3, %i2, %i3		! decrement loop count
	bg	3b			! are we done yet?
	add	%i0, %i2, %i0		! generate next addr
	ret
	restore
	SET_SIZE(vac_usrflush)

	.section ".data"
6:	.asciz	"vac_usrflush: sys_trap is not in cacheable page"
	.align	4
	.section ".text"

#endif	/* lint */
#endif	/* !sun4c */

#ifdef MMU_3LEVEL

/*
 * Flush a region from the cache.
 * To flush the argument region from the cache we hold the bits that
 * specify the region in the address constant and issue a store into
 * alternate space command for each line of the cache by incrementing
 * the lower bits of the address by VAC_LINESIZE (cache line size).
 */

#if	defined(lint)
/* ARGSUSED */
void
vac_rgnflush(caddr_t v) {}

#else	/* lint */

	ENTRY(vac_rgnflush)
	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp

	retl
	nop
1:
	sethi	%hi(vac_size), %l0
	ld	[%l0 + %lo(vac_size)], %l0
	sethi	%hi(vac_linesize), %i5
	ld	[%i5 + %lo(vac_linesize)], %i5
	sethi	%hi(vac_nlines), %i1	! nlines to flush / # steps in loop
	ld	[%i1 + %lo(vac_nlines)], %i1
	sethi	%hi(flush_cnt+FM_REGION), %g1 ! increment flush count
	ld	[%g1 + %lo(flush_cnt+FM_REGION)], %g2
	srl	%l0, 4, %l0		! cachesize / 16 (# of steps in loop)
	inc	%g2
	call	flush_user_windows	! make sure no windows are hanging out
	st	%g2, [%g1 + %lo(flush_cnt+FM_SEGMENT)]
	srl	%i0, SMGRPSHIFT, %i0	! mask off lo bits
	sll	%i0, SMGRPSHIFT, %i0

	!
	! preload a bunch of offsets
	! Avoid going through sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	add	%i0, %l0, %i0
	sub	%i0, %i5, %i0		! base address + cachesize - linesize
	add	%l0, %l0, %l1		! cachesize*2
	add	%l1, %l0, %l2		! cachesize*3
	add	%l2, %l0, %l3		! ...
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4
2:
	sta	%g0, [%i0]ASI_FCR
	sta	%g0, [%i0 + %l0]ASI_FCR
	sta	%g0, [%i0 + %l1]ASI_FCR
	sta	%g0, [%i0 + %l2]ASI_FCR
	sta	%g0, [%i0 + %l3]ASI_FCR
	sta	%g0, [%i0 + %l4]ASI_FCR
	sta	%g0, [%i0 + %l5]ASI_FCR
	sta	%g0, [%i0 + %l6]ASI_FCR
	sta	%g0, [%i0 + %l7]ASI_FCR
	sta	%g0, [%i0 + %o0]ASI_FCR
	sta	%g0, [%i0 + %o1]ASI_FCR
	sta	%g0, [%i0 + %o2]ASI_FCR
	sta	%g0, [%i0 + %o3]ASI_FCR
	sta	%g0, [%i0 + %o4]ASI_FCR
	sta	%g0, [%i0 + %o5]ASI_FCR
	sta	%g0, [%i0 + %i4]ASI_FCR
	subcc	%i1, 16, %i1		! decrement loop count
	bg	2b			! are we done yet?
	sub	%i0, %i5, %i0		! generate next address
	ret
	restore
	SET_SIZE(vac_rgnflush)

#endif	/* lint */
#endif /* MMU_3LEVEL */


/*
 *
 * Flush a segment from the cache.
 * To flush the argument segment from the cache we hold the bits that
 * specify the segment in the address constant and issue a store into
 * alternate space command for each line.
 *
 * void vac_segflush(caddr_t v);
 */

#if defined(lint)

/*ARGSUSED*/
void
vac_segflush(caddr_t v)
{}

#else	/* lint */

	ENTRY(vac_segflush)
#ifdef	sun4c
	GET_INFO(%o5, %o2)		! get VAC info
	sethi	%hi(flush_cnt+FM_SEGMENT), %g1 ! get flush count
	btst	VAC_INFO_VAC, %o2	! check if cache is turned on
	bz	9f			! cache off, return
	srl	%o2, VAC_INFO_VSS, %o1	! cache size
	ld	[%g1 + %lo(flush_cnt+FM_SEGMENT)], %g2 ! get flush count
	btst	VAC_INFO_HW, %o2	! HW flush?
	inc	%g2			! increment flush count
	srl	%o0, PMGRPSHIFT, %o0	! get segment part of address
	sll	%o0, PMGRPSHIFT, %o0
	bz	2f			! use SW flush
	st	%g2, [%g1 + %lo(flush_cnt+FM_SEGMENT)] ! store flush count

! hardware flush

	set	NBPG, %o2

1:	subcc	%o1, %o2, %o1
	bg	1b
	sta	%g0, [%o0 + %o1]ASI_FCS_HW

	retl
	nop    

! software flush

2:	and	%o2, VAC_INFO_LSM, %o2	! line size
	add	%o2, %o2, %o3		! LS * 2
	add	%o2, %o3, %o4		! LS * 3
	add	%o2, %o4, %o5		! LS * 4
	add	%o2, %o5, %g1		! LS * 5
	add	%o2, %g1, %g2		! LS * 6
	add	%o2, %g2, %g3		! LS * 7
	add	%o2, %g3, %g4		! LS * 8

3:	subcc	%o1, %g4, %o1
	sta	%g0, [%o0 + %g0]ASI_FCS
	sta	%g0, [%o0 + %o2]ASI_FCS
	sta	%g0, [%o0 + %o3]ASI_FCS
	sta	%g0, [%o0 + %o4]ASI_FCS
	sta	%g0, [%o0 + %o5]ASI_FCS
	sta	%g0, [%o0 + %g1]ASI_FCS
	sta	%g0, [%o0 + %g2]ASI_FCS
	sta	%g0, [%o0 + %g3]ASI_FCS
	bg	3b
	add	%o0, %g4, %o0

9:	retl
	nop    
	SET_SIZE(vac_segflush)

#else	sun4c
	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp

	retl
	nop
1:
	sethi	%hi(vac_size), %l0
	ld	[%l0 + %lo(vac_size)], %l0
	srl	%l0, 4, %l0		! cachesize / number of steps in loop
	sethi	%hi(vac_linesize), %i5
	ld	[%i5 + %lo(vac_linesize)], %i5
	sethi	%hi(vac_nlines), %i1	! nlines to flush / # steps in loop
	ld	[%i1 + %lo(vac_nlines)], %i1
	sethi	%hi(flush_cnt+FM_SEGMENT), %g1 ! increment flush count
	ld	[%g1 + %lo(flush_cnt+FM_SEGMENT)], %g2
	srl	%i0, PMGRPSHIFT, %i0	! mask off lo bits
	sll	%i0, PMGRPSHIFT, %i0
	inc	%g2
	call	flush_user_windows	! make sure no windows are hanging out
	st	%g2, [%g1 + %lo(flush_cnt+FM_SEGMENT)]

	!
	! preload a bunch of offsets
	! Avoid going through sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	add	%i0, %l0, %i0
	sub	%i0, %i5, %i0		! base address + cachesize - linesize
	add	%l0, %l0, %l1		! cachesize/16*2
	add	%l1, %l0, %l2		! cachesize/16*3
	add	%l2, %l0, %l3		! ...
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4
2:
	sta	%g0, [%i0]ASI_FCS
	sta	%g0, [%i0 + %l0]ASI_FCS
	sta	%g0, [%i0 + %l1]ASI_FCS
	sta	%g0, [%i0 + %l2]ASI_FCS
	sta	%g0, [%i0 + %l3]ASI_FCS
	sta	%g0, [%i0 + %l4]ASI_FCS
	sta	%g0, [%i0 + %l5]ASI_FCS
	sta	%g0, [%i0 + %l6]ASI_FCS
	sta	%g0, [%i0 + %l7]ASI_FCS
	sta	%g0, [%i0 + %o0]ASI_FCS
	sta	%g0, [%i0 + %o1]ASI_FCS
	sta	%g0, [%i0 + %o2]ASI_FCS
	sta	%g0, [%i0 + %o3]ASI_FCS
	sta	%g0, [%i0 + %o4]ASI_FCS
	sta	%g0, [%i0 + %o5]ASI_FCS
	sta	%g0, [%i0 + %i4]ASI_FCS
	subcc	%i1, 16, %i1		! decrement loop count
	bg	2b			! are we done yet?
	sub	%i0, %i5, %i0		! generate next address
	ret
	restore

#endif	/* sun4c */
#endif	/* lint */

/*
 * Flush a page from the cache.
 * To flush the page containing the argument virtual address from
 * the cache we hold the bits that specify the page constant and
 * issue a store into alternate space command for each line.
 *
 * void vac_pageflush(caddr_t v);
 */

#if defined(lint)

/*ARGSUSED*/
void
vac_pageflush(caddr_t v)
{}

#else	/* lint */

	ENTRY(vac_pageflush)
#ifdef	sun4c
	GET_INFO(%o5, %o2)		! get VAC info
	sethi	%hi(flush_cnt+FM_PAGE), %g1 ! get flush count
	btst	VAC_INFO_VAC, %o2	! check if cache is turned on
	bz	9f			! cache off, return
	set	NBPG, %o1		! page size
	ld	[%g1 + %lo(flush_cnt+FM_PAGE)], %g2 ! get flush count
	btst	VAC_INFO_HW, %o2	! HW flush?
	inc	%g2			! increment flush count
	bz	2f			! use SW flush
	st	%g2, [%g1 + %lo(flush_cnt+FM_PAGE)] ! store flush count

! hardware flush

	bclr	3, %o0			! force word alignment
	retl
	sta	%g0, [%o0]ASI_FCP_HW

! software flush

2:
#if PGSHIFT <= 13
	bclr	(NBPG - 1), %o0		! get page part of address
#else
	srl	%o0, PGSHIFT, %o0	! get page part of address
	sll	%o0, PGSHIFT, %o0
#endif
	and	%o2, VAC_INFO_LSM, %o2	! line size
	add	%o2, %o2, %o3		! LS * 2
	add	%o2, %o3, %o4		! LS * 3
	add	%o2, %o4, %o5		! LS * 4
	add	%o2, %o5, %g1		! LS * 5
	add	%o2, %g1, %g2		! LS * 6
	add	%o2, %g2, %g3		! LS * 7
	add	%o2, %g3, %g4		! LS * 8

3:	subcc	%o1, %g4, %o1
	sta	%g0, [%o0 + %g0]ASI_FCP
	sta	%g0, [%o0 + %o2]ASI_FCP
	sta	%g0, [%o0 + %o3]ASI_FCP
	sta	%g0, [%o0 + %o4]ASI_FCP
	sta	%g0, [%o0 + %o5]ASI_FCP
	sta	%g0, [%o0 + %g1]ASI_FCP
	sta	%g0, [%o0 + %g2]ASI_FCP
	sta	%g0, [%o0 + %g3]ASI_FCP
	bg	3b
	add	%o0, %g4, %o0

9:	retl
	nop    
#else	/* sun4c */

	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp

	retl
	nop
1:
	sethi	%hi(flush_cnt+FM_PAGE), %g1 ! increment flush count
	ld	[%g1 + %lo(flush_cnt+FM_PAGE)], %g2
	sethi	%hi(vac_linesize), %i5
	inc	%g2
	st	%g2, [%g1 + %lo(flush_cnt+FM_PAGE)]
	ld	[%i5 + %lo(vac_linesize)], %i5
	sethi	%hi(vac_pglines), %i1
	ld	[%i1 + %lo(vac_pglines)], %i1
	srl	%i0, PGSHIFT, %i0	! mask off lo bits
	sll	%i0, PGSHIFT, %i0
	set	KERNELBASE, %l0
	cmp	%i0, %l0		! if user addr, flush windows
	bgeu	0f
	nop

	call	flush_user_windows	! make sure no windows are hanging out
	nop

	!
	! A flush that causes a writeback will happen in parallel
	! with other instructions.  Back to back flushes which cause
	! writebacks cause the processor to wait until the first writeback
	! is finished and the second is initiated before proceeding.
	! Avoid going through a page sequentially by flushing
	! 16 lines spread evenly through the page.
	!
0:
	mov	NBPG/16, %l0		! 512
	add	%l0, NBPG/16, %l1	! 1024
	add	%l1, NBPG/16, %l2	! ...
	add	%l2, NBPG/16, %l3
	add	%l3, NBPG/16, %l4
	add	%l4, NBPG/16, %l5
	add	%l5, NBPG/16, %l6
	add	%l6, NBPG/16, %l7
	add	%l7, NBPG/16, %o0
	add	%o0, NBPG/16, %o1
	add	%o1, NBPG/16, %o2
	add	%o2, NBPG/16, %o3
	add	%o3, NBPG/16, %o4
	add	%o4, NBPG/16, %o5
	add	%o5, NBPG/16, %i4
	sub	%l0, %i5, %i3		! NBPG/16 - linesize
	add	%i0, %i3, %i0 		! page addr + 512 - 16
2:
	sta	%g0, [%i0]ASI_FCP
	sta	%g0, [%i0 + %l0]ASI_FCP
	sta	%g0, [%i0 + %l1]ASI_FCP
	sta	%g0, [%i0 + %l2]ASI_FCP
	sta	%g0, [%i0 + %l3]ASI_FCP
	sta	%g0, [%i0 + %l4]ASI_FCP
	sta	%g0, [%i0 + %l5]ASI_FCP
	sta	%g0, [%i0 + %l6]ASI_FCP
	sta	%g0, [%i0 + %l7]ASI_FCP
	sta	%g0, [%i0 + %o0]ASI_FCP
	sta	%g0, [%i0 + %o1]ASI_FCP
	sta	%g0, [%i0 + %o2]ASI_FCP
	sta	%g0, [%i0 + %o3]ASI_FCP
	sta	%g0, [%i0 + %o4]ASI_FCP
	sta	%g0, [%i0 + %o5]ASI_FCP
	sta	%g0, [%i0 + %i4]ASI_FCP
	subcc	%i1, 16, %i1		! decrement loop count
	bg	2b			! are we done yet?
	sub	%i0, %i5, %i0		! generate next match address
	ret
	restore

#endif	/* sun4c */

	SET_SIZE(vac_pageflush)

#endif	/* lint */

/*
 *
 * Flush a range of addresses.
 *
 * void vac_flush(caddr_t v, u_int nbytes);
 */

#if defined(lint)

/*ARGSUSED*/
void
vac_flush(caddr_t v, u_int nbytes)
{}

#else	/* lint */

	ENTRY(vac_flush)
#ifdef	sun4c
	GET_INFO(%o5, %o2)		! get VAC info
	sethi	%hi(flush_cnt+FM_PARTIAL), %g1 ! get flush count
	btst	VAC_INFO_VAC, %o2	! check if cache is turned on
	bz	9f			! cache off, return
	srl	%o2, VAC_INFO_VSS, %o3	! cache size
	ld	[%g1 + %lo(flush_cnt+FM_PARTIAL)], %g2 ! get flush count
	and	%o2, VAC_INFO_LSM, %o2	! line size
	sub	%o2, 1, %o4		! convert to mask (assumes power of 2)
	inc	%g2			! increment flush count
	st	%g2, [%g1 + %lo(flush_cnt+FM_PARTIAL)] ! store flush count
	add	%o0, %o1, %o1		! add start to length
	andn	%o0, %o4, %o0		! round down start
	add	%o4, %o1, %o1		! round up end
	andn	%o1, %o4, %o1		! and mask off
	sub	%o1, %o0, %o1		! and subtract start
	cmp	%o1, %o3		! if (nbytes > vac_size)
	bgu,a	1f			! ...
	mov	%o3, %o1		!	nbytes = vac_size
1:	! nop

2:	subcc	%o1, %o2, %o1
	bg	2b
	sta	%g0, [%o0 + %o1]ASI_FCP

9:	retl
	nop
#else	/* sun4c */
	sethi	%hi(vac), %g1		! check if cache is turned on
	ld	[%g1 + %lo(vac)], %g1
	tst	%g1
	bnz,a	1f			! if cache not enabled, return
	save	%sp, -SA(MINFRAME), %sp

	retl
	nop
1:
	sethi	%hi(vac_linesize), %i5
	ld	[%i5 + %lo(vac_linesize)], %i5

	sethi	%hi(flush_cnt+FM_PARTIAL), %g1 ! increment flush count
	ld	[%g1 + %lo(flush_cnt+FM_PARTIAL)], %g2
	sub	%i5, 1, %l0		! build linesize mask
	andn	%i0, %l0, %i0		! mask off line offset bits
	inc	%g2
	st	%g2, [%g1 + %lo(flush_cnt+FM_PARTIAL)]

	mov	%i5, %l1		! preload a bunch of offsets
	add	%i5, %l1, %l2		! linesize*2
	add	%i5, %l2, %l3		! linesize*3
	add	%i5, %l3, %l4		! linesize*4
	add	%i5, %l4, %l5		! linesize*5
	add	%i5, %l5, %l6		! linesize*6
	add	%i5, %l6, %l7		! linesize*7
	sll	%i5, 3, %i5		! linesize * 8 (loop flush amount)
	add	%i1, %l1, %i1		! round up size
2:
	sta	%g0, [%i0]ASI_FCP
	sta	%g0, [%i0 + %l1]ASI_FCP
	sta	%g0, [%i0 + %l2]ASI_FCP
	sta	%g0, [%i0 + %l3]ASI_FCP
	sta	%g0, [%i0 + %l4]ASI_FCP
	sta	%g0, [%i0 + %l5]ASI_FCP
	sta	%g0, [%i0 + %l6]ASI_FCP
	sta	%g0, [%i0 + %l7]ASI_FCP
	subcc	%i1, %i5, %i1			! decrement count
	bg	2b				! are we done yet?
	add	%i0, %i5, %i0			! generate next chunk address

	ret
	restore
#endif	/* sun4c */
	SET_SIZE(vac_flush)

#endif	/* lint */

#ifdef	sun4c

/*
 * Flush all user lines from the cache.  We accomplish it by reading a portion
 * of the kernel text starting at sys_trap. The size of the portion is
 * equal to the VAC size. We read a word from each line. sys_trap was chosen
 * as the start address because it is the start of the locore code
 * that we assume will be very likely executed in near future.
 *
 * XXX - use a HW feature if the cache supports it (e.g. SunRay).
 */

#if defined(lint)

void
vac_usrflush(void)
{}

#else	/* lint */
	ENTRY(vac_usrflush)
	GET_INFO(%o5, %o2)		! get VAC info
	btst	VAC_INFO_VAC, %o2	! check if cache is turned on
	bnz,a	1f			! cache on
	save	%sp, -SA(MINFRAME), %sp
	retl
	nop

1:
	call	flush_user_windows	! make sure no windows are hanging out
	nop

	sethi	%hi(flush_cnt+FM_USR), %g1 ! get flush count
	ld	[%g1 + %lo(flush_cnt+FM_USR)], %g2 ! get flush count
	!
	! Due to a bug in HW, some processor must map the trap vectors
	! non cacheable. Software (locore.s) must guarantee that the
	! code that follows the trap vectors starts in next page.
	! We are paranoid about it and check that sys_trap is actually
	! in a cacheable page. We panic otherwise.
	!
	tst	%g2
	set	sys_trap, %i0		! start reading text seg. from sys_trap
	bnz	2f
	inc	%g2			! increment flush count

	! check pte only the first time we vac_usrflush is called
	lda	[%i0]ASI_PM, %l0	! read page map entry
	set	PG_NC, %l1
	btst	%l1, %l0
	bz	2f
	nop

	sethi	%hi(6f), %o0
	call	panic
	or	%o0, %lo(6f), %o0

	.seg	".data"
6:	.asciz	"vac_usrflush: sys_trap is not in cacheable page"
	.seg	".text"

2:
	st	%g2, [%g1 + %lo(flush_cnt+FM_USR)] ! store flush count
	GET(vac_size, %l1, %i1)	! cache size
	and	%i2, VAC_INFO_LSM, %i2	! line size

	!
	! A flush that causes a writeback will happen in parallel
	! with other instructions.  Back to back flushes which cause
	! writebacks cause the processor to wait until the first writeback
	! is finished and the second is initiated before proceeding.
	! Avoid going through the cache sequentially by flushing
	! 16 lines spread evenly through the cache.
	!
	!  i0 start address
	!  i1 vac_size
	!  i2 linesize

	srl	%i1, 4, %l0		! vac_size / 16
	add	%l0, %l0, %l1		! 2 * (vac_size / 16)
	add	%l1, %l0, %l2	! ...
	add	%l2, %l0, %l3
	add	%l3, %l0, %l4
	add	%l4, %l0, %l5
	add	%l5, %l0, %l6
	add	%l6, %l0, %l7
	add	%l7, %l0, %o0
	add	%o0, %l0, %o1
	add	%o1, %l0, %o2
	add	%o2, %l0, %o3
	add	%o3, %l0, %o4
	add	%o4, %l0, %o5
	add	%o5, %l0, %i4		! 15 * (vac_size / 16)

	mov	%l0, %i3		! loop counter: vac_size / 16

3:
	ld	[%i0      ], %i1
	ld	[%i0 + %l0], %i1
	ld	[%i0 + %l1], %i1
	ld	[%i0 + %l2], %i1
	ld	[%i0 + %l3], %i1
	ld	[%i0 + %l4], %i1
	ld	[%i0 + %l5], %i1
	ld	[%i0 + %l6], %i1
	ld	[%i0 + %l7], %i1
	ld	[%i0 + %o0], %i1
	ld	[%i0 + %o1], %i1
	ld	[%i0 + %o2], %i1
	ld	[%i0 + %o3], %i1
	ld	[%i0 + %o4], %i1
	ld	[%i0 + %o5], %i1
	ld	[%i0 + %i4], %i1

	subcc	%i3, %i2, %i3		! decrement loop count
	bg	3b			! are we done yet?
	add	%i0, %i2, %i0		! generate next addr
	ret
	restore
	SET_SIZE(vac_usrflush)

#endif	/* lint */
#endif	/* sun4c */

/*
 * Flush a single cache line
 */

#if	defined(lint)

/* ARGSUSED */
void
vac_flushone(caddr_t vaddr) { }

#else	/* lint */
	ENTRY(vac_flushone)
	set	vac_linesize, %o1
	ld	[%o1], %o1
	sub	%o1, 1, %o1
	andn	%o0, %o1, %o0
	retl
	sta	%g0, [%o0]ASI_FCP
	SET_SIZE(vac_flushone)

#endif	/* lint */

/*
 * Load a SW page table to HW pmg
 *
 * void sunm_pmgloadptes(caddr_t a, struct pte *ppte);
 *
 *	o0	running page address
 *	o1	running ppte
 *	o3	end address
 *	o4	pte
 *	o5	pagesize (machine dependent value 4k, or 8k)
 *	g1	PG_V
 */

#if defined(lint)

/*ARGSUSED*/
void
sunm_pmgloadptes(caddr_t a, struct pte *ppte)
{}

#else	/* lint */

	ENTRY(sunm_pmgloadptes)
	set	PAGESIZE, %o5
	set	(NPMENTPERPMGRP * 4), %o3 ! o3 = nptes * (sizeof pte)
	set	PG_V, %g1		! pte valid bit
	add	%o1, %o3, %o3		! o3 = ppte + nptes * (sizeof pte)
0:
	ld	[%o1], %o4		! o4 has SW pte
	add	%o1, 4, %o1		! o1 += sizeof (pte)
	andcc	%o4, %g1, %g0		! valid page?
	bnz,a	1f
	sta	%o4, [%o0]ASI_PM	! write page map entry
1:
	cmp	%o1, %o3		! (ppte < last pte) ?
	blu	0b
	add	%o0, %o5, %o0		! a += pagesize

	retl
	nop
	SET_SIZE(sunm_pmgloadptes)

#endif	/* lint */

/*
 * Unload a SW page table from HW pmg.
 *
 * void sunm_pmgunloadptes(caddr_t a, struct pte *ppte);
 *
 *	o0	running page address
 *	o1	running ppte
 *	o3	end address
 *	o4	pte
 *	g1	PG_V
 *	g2	pte_invalid
 *	g3	pagesize (machine dependent value 4k, or 8k)
 */

#if defined(lint)

/*ARGSUSED*/
void
sunm_pmgunloadptes(caddr_t a, struct pte *ppte)
{}

#else	/* lint */

	ENTRY(sunm_pmgunloadptes)
	sethi 	%hi(mmu_pteinvalid), %g1  ! g2 = *mmu_pteinvalid
	ld	[%g1 + %lo(mmu_pteinvalid)], %g2
	set	PG_V, %g1		! g1 PG_V

	set	PAGESIZE, %g3		! g3 = PAGESIZE
	set	PMGRPSIZE, %o3
	add	%o0, %o3, %o3		! o3 = a + PMGRPSIZE

0:
	ld	[%o1], %o4		! o4 has SW pte
	andcc	%o4, %g1, %g0		! valid page?
	bnz,a	1f
	lda	[%o0]ASI_PM, %o4	! (delay slot) read page map entry

	ba	2f
	add	%o0, %g3, %o0		! (delay slot) a += pagesize
1:
	st	%o4, [%o1]		! store pte in SW page table
	sta	%g2, [%o0]ASI_PM	! write invalid pte
	add	%o0, %g3, %o0		! a += pagesize
2:
	cmp	%o0, %o3		! (a < end address) ?
	blu	0b
	add	%o1, 4, %o1		! o1 += sizeof (pte)

	retl
	nop    
	SET_SIZE(sunm_pmgunloadptes)

#endif	/* lint */


/*
 * Unload old SW page table from HW pmg and load a new SW page table
 * into this HW pmg at the same time.
 *
 * void sunm_pmgswapptes(caddr_t a, struct pte *pnew, struct pte *pold);
 *
 *	o0	running page address
 *	o1	running new ppte
 *	o2	running old ppte
 *	o3	end address
 *	o4	old SW pte1
 * 	o5	old SW pte2
 *	g1	PG_V
 *	g2	new SW pte1
 *	g3	new SW pte1
 *	g4	pagesize (machine dependent value 4k, or 8k)
 */

#if defined(lint)

/*ARGSUSED*/
void
sunm_pmgswapptes(caddr_t a, struct pte *pnew, struct pte *pold)
{}

#else	/* lint */

	ENTRY(sunm_pmgswapptes)
	set	PMGRPSIZE - PAGESIZE, %g2
	set	PG_V, %g1		! g1 = PG_V
	set	PAGESIZE, %g4		! g4 = PAGESIZE
	set	(PMGRPSIZE / PAGESIZE - 2) * 4, %o3	! o3 = last ptes offset
	b	1f
	add	%o0, %g2, %o0		! o0 = last page

7:
	subcc	%o3, 8, %o3		! advance to next SW ptes
8:
	blt	9f
	sub	%o0, %g4, %o0		! (delay) advance to next page
1:
	ldd	[%o2+%o3], %o4		! old SW ptes in o4, o5
	ldd	[%o1+%o3], %g2		! new SW ptes in g2, g3
	andcc	%o5, %g1, %g0		! valid 1st old SW pte?
 	be	3f
 	andcc	%o4, %g1, %g0		! (delay slot) valid 2nd old SW pte?
 
 	! 1st old pte valid
 	lda	[%o0]ASI_PM, %o5	! read 1st old HW pte
 	sta	%g3, [%o0]ASI_PM	! write 1st new SW pte into HW
 	sub	%o0, %g4, %o0		! advance to next page
  	be,a	2f
 	std	%o4, [%o2+%o3]		! (delay) store old HW ptes in SW copy
  
 	! 2nd old pte valid, 1st old pte already read
 	lda	[%o0]ASI_PM, %o4	! read 2nd old HW pte
 	sta	%g2, [%o0]ASI_PM	! write 2nd new SW pte into HW
 	ba	7b
	std	%o4, [%o2+%o3]		! (delay) store old HW ptes in SW copy
  
2:	! 2nd old pte invalid, 1st pte already swapped
	andcc	%g2, %g1, %g0		! valid 2nd new SW pte?
	be	8b
	subcc	%o3, 8, %o3		! (delay)
	ba	8b
	sta	%g2, [%o0]ASI_PM	! (delay) write 2nd new SW pte into HW
  
3:	! 1st old pte invalid
	be	5f
	andcc	%g3, %g1, %g0		! (delay) valid 1st new pte?
  
	! 1st old pte invalid, 2nd old pte valid
	bne,a	4f
	sta	%g3, [%o0]ASI_PM	! (delay) write 1st new SW pte into HW
4:	sub	%o0, %g4, %o0		! advance to next page
	lda	[%o0]ASI_PM, %o4	! read 2nd old HW pte
	sta	%g2, [%o0]ASI_PM	! write 2nd new SW pte into HW
	ba	7b 
	st	%o4, [%o2+%o3]		! (delay) store old HW ptes in SW copy
  
5:	! 1st old pte invalid, 2nd old pte invalid
	be	6f
	andcc	%g2, %g1, %g0		! (delay) valid 2nd new pte?

	! 1st new pte valid
	sta	%g3, [%o0]ASI_PM	! write 1st new SW pte into HW
6:
	sub	%o0, %g4, %o0		! advance to next page
	be	8b
	subcc	%o3, 8, %o3		! (delay)
	ba	8b
	sta	%g2, [%o0]ASI_PM	! (delay) write 2nd new SW pte into HW

9:
  	retl
 	nop
	SET_SIZE(sunm_pmgswapptes)

#endif	/* lint */


/*
 * Load a SW page table from HW pmg.
 *
 * void sunm_pmgloadswptes(caddr_t a, struct pte *ppte)
 *
 *	o0	running page address
 *	o1	running ppte
 *	o3	end address
 *	o4	current hw pte value
 *	o5	pagesize (machine dependent value 4k, or 8k)
 */

#if defined(lint)

/*ARGSUSED*/
void
sunm_pmgloadswptes(caddr_t a, struct pte *ppte)
{}

#else	/* lint */

	ENTRY(sunm_pmgloadswptes)
	set	PAGESIZE, %o5		! o5 = PAGESIZE
	set	(NPMENTPERPMGRP * 4), %o3 ! o3 = nptes * (sizeof pte)
	add	%o1, %o3, %o3		! o3 = ppte + nptes * (sizeof pte)
1:
	lda	[%o0]ASI_PM, %o4	! read hw page map entry
	st	%o4, [%o1]		! store into SW page table
	add	%o1, 4, %o1		! o1 += sizeof (pte)
	cmp	%o1, %o3		! (ppte < last pte) ?
	blu	1b
	add	%o0, %o5, %o0		! a += pagesize

	retl
	nop
	SET_SIZE(sunm_pmgloadswptes)

#endif	/* lint */

#ifndef MMU_3LEVEL
/*
 * Stub routines for 3-level MMU support on machines
 * that don't have any.. the first three would be
 * in this file if we had them c.f. sun4/ml/map.s
 */

#if defined(lint)

/*ARGSUSED*/
u_int
map_getrgnmap(caddr_t v)
{ return (0); }

#else	/* lint */

	ENTRY(map_getrgnmap)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(map_getrgnmap)

#endif	/* lint */

#if defined(lint)

/*ARGSUSED*/
void
map_setrgnmap(caddr_t v, u_int pm)
{}

#else	/* lint */

	ENTRY(map_setrgnmap)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(map_setrgnmap)

#endif	/* lint */

#if defined(lint)

/*ARGSUSED*/
void
vac_rgnflush(caddr_t v)
{}

#else	/* lint */

	ENTRY(vac_rgnflush)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(vac_rgnflush)

#endif	/* lint */

/*
 * Routines that would be in sun4c/vm/mmu.c if they
 * were to do anything useful.
 *
 * XXX maybe we should move them there?
 */

#if defined(lint)

/*ARGSUSED*/
void
mmu_setsmg(caddr_t base, struct smgrp *smg)
{}

#else	/* lint */

	ENTRY(mmu_setsmg)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(mmu_setsmg)

#endif	/* lint */

#if defined(lint)

/*ARGSUSED*/
void
mmu_settsmg(caddr_t base, struct smgrp *smg)
{}

#else	/* lint */

	ENTRY(mmu_settsmg)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(mmu_settsmg)

#endif	/* lint */

#if defined(lint)

/*ARGSUSED*/
struct smgrp *
mmu_getsmg(caddr_t base)
{ return ((struct smgrp *)0); }

#else	/* lint */

	ENTRY(mmu_getsmg)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(mmu_getsmg)

#endif	/* lint */

#if defined(lint)

/*ARGSUSED*/
void
mmu_smginval(caddr_t base)
{}

#else	/* lint */

	ENTRY(mmu_smginval)
	sethi	%hi(.no_3_level_msg), %o0
	call	panic
	or	%o0, %lo(.no_3_level_msg), %o0
	SET_SIZE(mmu_smginval)

.no_3_level_msg:
	.asciz	"can't happen: not a 3-level mmu"
	.align	4

#endif	/* lint */

#endif /* !MMU_3LEVEL */
