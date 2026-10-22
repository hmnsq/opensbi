/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LIBFDT_ਅੰਦਰੂਨੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIBFDT_ਅੰਦਰੂਨੀ_H
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ALIGN(x, a)		(((x) + (a) - 1) & ~((a) - 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_TAGALIGN(x)		(FDT_ALIGN((x), FDT_TAGSIZE))

int32_t fdt_ro_probe_(ਅਚਲ ਖਾਲੀ *fdt);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_RO_PROBE(fdt)					\
	{							\
		int32_t totalsize_;				\
		ਜੇ ((totalsize_ = fdt_ro_probe_(fdt)) < 0)	\
			ਵਾਪਸ totalsize_;			\
	}

ਪੂਰਨ fdt_check_ਗੰਢ_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ);
ਪੂਰਨ fdt_check_prop_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ);
ਅਚਲ ਅੱਖਰ *fdt_ਲੱਭੋ_string(ਅਚਲ ਅੱਖਰ *strtab, ਪੂਰਨ tabsize, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ fdt_ਗੰਢ_ਸਿਰਾ_ਵਿੱਥ(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਖਾਲੀ *fdt_ਵਿੱਥ_ptr_2(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਵਾਪਸ (ਅਚਲ ਅੱਖਰ *)fdt + fdt_off_dt_struct(fdt) + ਵਿੱਥ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *fdt_ਵਿੱਥ_ptr_w(ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਵਾਪਸ (ਖਾਲੀ *)(uintptr_t)fdt_ਵਿੱਥ_ptr_2(fdt, ਵਿੱਥ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *fdt_mem_rsv_(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ n)
{
	ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *rsv_ਸਾਰਣੀ =
		(ਅਚਲ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *)
		((ਅਚਲ ਅੱਖਰ *)fdt + fdt_off_mem_rsvmap(fdt));

	ਵਾਪਸ rsv_ਸਾਰਣੀ + n;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *fdt_mem_rsv_w_(ਖਾਲੀ *fdt, ਪੂਰਨ n)
{
	ਵਾਪਸ (ਖਾਲੀ *)(uintptr_t)fdt_mem_rsv_(fdt, n);
}

/*
 * Internal helpers to access tructural elements of the device tree
 * blob (rather than for exaple reading integers from within property
 * values).  We assume that we are either given a naturally aligned
 * address for the platform or if we are not, we are on a platform
 * where unaligned memory reads will be handled in a graceful manner.
 * If not the external helpers fdtXX_ld() from libfdt.h can be used
 * instead.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t fdt32_ld_(ਅਚਲ fdt32_t *p)
{
	ਵਾਪਸ fdt32_ਮੰਜ਼ਿਲ_cpu(*p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t fdt64_ld_(ਅਚਲ fdt64_t *p)
{
	ਵਾਪਸ fdt64_ਮੰਜ਼ਿਲ_cpu(*p);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_SW_MAGIC		(~FDT_MAGIC)

/**********************************************************************/
/* Checking controls                                                  */
/**********************************************************************/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FDT_ASSUME_ਨਕਾਬ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ASSUME_ਨਕਾਬ 0
#ਜੇ_ਅੰਤ

/*
 * Defines assumptions which can be enabled. Each of these can be enabled
 * individually. For maximum safety, don't enable any assumptions!
 *
 * For minimal code size and no safety, use ASSUME_PERFECT at your own risk.
 * You should have another method of validating the device tree, such as a
 * signature or hash check before using libfdt.
 *
 * For situations where security is not a concern it may be safe to enable
 * ASSUME_SANE.
 */
ਗਿਣਤੀ {
	/*
	 * This does essentially no checks. Only the latest device-tree
	 * version is correctly handled. Inconsistencies or errors in the device
	 * tree may cause undefined behaviour or crashes. Invalid parameters
	 * passed to libfdt may do the same.
	 *
	 * If an error occurs when modifying the tree it may leave the tree in
	 * an intermediate (but valid) state. As an example, adding a property
	 * where there is insufficient space may result in the property name
	 * being added to the string table even though the property itself is
	 * not added to the struct section.
	 *
	 * Only use this if you have a fully validated device tree with
	 * the latest supported version and wish to minimise code size.
	 */
	ASSUME_PERFECT		= 0xff,

	/*
	 * This assumes that the device tree is sane. i.e. header metadata
	 * and basic hierarchy are correct.
	 *
	 * With this assumption enabled, normal device trees produced by libfdt
	 * and the compiler should be handled safely. Malicious device trees and
	 * complete garbage may cause libfdt to behave badly or crash. Truncated
	 * device trees (e.g. those only partially loaded) can also cause
	 * problems.
	 *
	 * Note: Only checks that relate exclusively to the device tree itself
	 * (not the parameters passed to libfdt) are disabled by this
	 * assumption. This includes checking headers, tags and the like.
	 */
	ASSUME_VALID_DTB	= 1 << 0,

	/*
	 * This builds on ASSUME_VALID_DTB and further assumes that libfdt
	 * functions are called with valid parameters, i.e. not trigger
	 * FDT_ERR_BADOFFSET or offsets that are out of bounds. It disables any
	 * extensive checking of parameters and the device tree, making various
	 * assumptions about correctness.
	 *
	 * It doesn't make sense to enable this assumption unless
	 * ASSUME_VALID_DTB is also enabled.
	 */
	ASSUME_VALID_ਨਿਵੇਸ਼	= 1 << 1,

	/*
	 * This disables checks for device-tree version and removes all code
	 * which handles older versions.
	 *
	 * Only enable this if you know you have a device tree with the latest
	 * version.
	 */
	ASSUME_LATEST		= 1 << 2,

	/*
	 * This assumes that it is OK for a failed addition to the device tree,
	 * due to lack of space or some other problem, to skip any rollback
	 * steps (such as dropping the property name from the string table).
	 * This is safe to enable in most circumstances, even though it may
	 * leave the tree in a sub-optimal state.
	 */
	ASSUME_NO_ROLLBACK	= 1 << 3,

	/*
	 * This assumes that the device tree components appear in a 'convenient'
	 * order, i.e. the memory reservation block first, then the structure
	 * block and finally the string block.
	 *
	 * This order is not specified by the device-tree specification,
	 * but is expected by libfdt. The device-tree compiler always created
	 * device trees with this order.
	 *
	 * This assumption disables a check in fdt_open_into() and removes the
	 * ability to fix the problem there. This is safe if you know that the
	 * device tree is correctly ordered. See fdt_blocks_misordered_().
	 */
	ASSUME_LIBFDT_ਕ੍ਰਮ	= 1 << 4,

	/*
	 * This assumes that libfdt itself does not have any internal bugs. It
	 * drops certain checks that should never be needed unless libfdt has an
	 * undiscovered bug.
	 *
	 * This can generally be considered safe to enable.
	 */
	ASSUME_LIBFDT_FLAWLESS	= 1 << 5,
};

/**
 * can_assume_() - check if a particular assumption is enabled
 *
 * @mask: Mask to check (ASSUME_...)
 * @return true if that assumption is enabled, else false
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool can_assume_(ਪੂਰਨ ਨਕਾਬ)
{
	ਵਾਪਸ FDT_ASSUME_ਨਕਾਬ & ਨਕਾਬ;
}

/** helper macros for checking assumptions */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ can_assume(_assume)	can_assume_(ASSUME_ ## _assume)

#ਜੇ_ਅੰਤ /* LIBFDT_INTERNAL_H */
