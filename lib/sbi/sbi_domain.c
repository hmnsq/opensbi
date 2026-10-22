/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

SBI_LIST_HEAD(domain_list);

static u32 domain_count = 0;
static bool domain_finalized = false;

#define ROOT_REGION_MAX	32

struct sbi_domain root = {
	.name = "root",
	.init_order = -1U,
	.possible_harts = NULL,
	.regions = NULL,
	.system_reset_allowed = true,
	.system_suspend_allowed = true,
	.fw_region_inited = false,
};

static unsigned long domain_hart_ptr_offset;

struct sbi_domain *sbi_hartindex_to_domain(u32 hartindex)
{
	struct sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	nếu (!scratch || !domain_hart_ptr_offset)
		trả_về NULL;

	trả_về sbi_scratch_read_type(scratch, void *, domain_hart_ptr_offset);
}

void sbi_update_hartindex_to_domain(u32 hartindex, struct sbi_domain *dom)
{
	struct sbi_scratch *scratch;

	scratch = sbi_hartindex_to_scratch(hartindex);
	nếu (!scratch)
		trả_về;

	sbi_scratch_write_type(scratch, void *, domain_hart_ptr_offset, dom);
}

bool sbi_domain_is_assigned_hart(const struct sbi_domain *dom, u32 hartindex)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	bool ret;
	struct sbi_domain *tdom = (struct sbi_domain *)dom;

	nếu (!dom)
		trả_về false;

	spin_lock(&tdom->assigned_harts_lock);
	ret = sbi_hartmask_test_hartindex(hartindex, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	trả_về ret;
}

số_nguyên sbi_domain_get_assigned_hartmask(const struct sbi_domain *dom,
				     struct sbi_hartmask *mask)
	MUST_NOT_HOLD(&dom->assigned_harts_lock)
{
	ulong ret = 0;
	struct sbi_domain *tdom = (struct sbi_domain *)dom;

	nếu (!dom) {
		sbi_hartmask_clear_all(mask);
		trả_về 0;
	}

	spin_lock(&tdom->assigned_harts_lock);
	sbi_hartmask_copy(mask, &tdom->assigned_harts);
	spin_unlock(&tdom->assigned_harts_lock);

	trả_về ret;
}

void sbi_domain_memregion_init(unsigned long addr,
				unsigned long size,
				unsigned long flags,
				struct sbi_domain_memregion *reg)
{
	unsigned long base = 0, order;

	cho (order = log2roundup(size) ; order <= __riscv_xlen; order++) {
		nếu (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			nếu ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				break;
		} không_thì {
			base = 0;
			break;
		}

	}

	nếu (reg) {
		reg->base = base;
		reg->order = order;
		reg->flags = flags;
	}
}

unsigned số_nguyên sbi_domain_get_oldpmp_flags(struct sbi_domain_memregion *reg)
{

	unsigned số_nguyên pmp_flags = 0;

	/*
	 * If permissions are to be enforced for all modes on
	 * this region, the lock bit should be set.
	 */
	nếu (reg->flags & SBI_DOMAIN_MEMREGION_ENF_PERMISSIONS)
		pmp_flags |= PMP_L;

	nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
		pmp_flags |= PMP_R;
	nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
		pmp_flags |= PMP_W;
	nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
		pmp_flags |= PMP_X;

	trả_về pmp_flags;
}

unsigned số_nguyên sbi_domain_get_smepmp_flags(struct sbi_domain_memregion *reg)
{
	unsigned số_nguyên pmp_flags = 0;
	unsigned long rstart, rend;

	nếu ((reg->flags & SBI_DOMAIN_MEMREGION_ACCESS_MASK) == 0) {
		/*
		 * Region is inaccessible in all privilege modes.
		 *
		 * SmePMP allows two encodings for an inaccessible region:
		 *   - pmpcfg.LRWX = 0000 (Inaccessible region)
		 *   - pmpcfg.LRWX = 1000 (Locked inaccessible region)
		 * We use the first encoding here.
		 */
		trả_về 0;
	} không_thì nếu (SBI_DOMAIN_MEMREGION_IS_SHARED(reg->flags)) {
		/* Read only for both M and SU modes */
		nếu (SBI_DOMAIN_MEMREGION_IS_SUR_MR(reg->flags))
			pmp_flags = (PMP_L | PMP_R | PMP_W | PMP_X);

		/* Execute for SU but Read/Execute for M mode */
		không_thì nếu (SBI_DOMAIN_MEMREGION_IS_SUX_MRX(reg->flags))
			/* locked region */
			pmp_flags = (PMP_L | PMP_W | PMP_X);

		/* Execute only for both M and SU modes */
		không_thì nếu (SBI_DOMAIN_MEMREGION_IS_SUX_MX(reg->flags))
			pmp_flags = (PMP_L | PMP_W);

		/* Read/Write for both M and SU modes */
		không_thì nếu (SBI_DOMAIN_MEMREGION_IS_SURW_MRW(reg->flags))
			pmp_flags = (PMP_W | PMP_X);

		/* Read only for SU mode but Read/Write for M mode */
		không_thì nếu (SBI_DOMAIN_MEMREGION_IS_SUR_MRW(reg->flags))
			pmp_flags = (PMP_W);
	} không_thì nếu (SBI_DOMAIN_MEMREGION_M_ONLY_ACCESS(reg->flags)) {
		/*
		 * When smepmp is supported and used, M region cannot have RWX
		 * permissions on any region.
		 */
		nếu ((reg->flags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK)
		    == SBI_DOMAIN_MEMREGION_M_RWX) {
			sbi_printf("%s: M-mode only regions cannot have"
				   "RWX permissions\n", __func__);
			trả_về 0;
		}

		/* M-mode only access regions are always locked */
		pmp_flags |= PMP_L;

		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			pmp_flags |= PMP_R;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			pmp_flags |= PMP_W;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			pmp_flags |= PMP_X;
	} không_thì nếu (SBI_DOMAIN_MEMREGION_SU_ONLY_ACCESS(reg->flags)) {
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			pmp_flags |= PMP_R;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			pmp_flags |= PMP_W;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			pmp_flags |= PMP_X;
	} không_thì {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ? rstart + ((1UL << reg->order) - 1) : -1UL;
		sbi_printf("%s: Unsupported Smepmp permissions on region 0x%"PRILX"-0x%"PRILX"\n",
			   __func__, rstart, rend);
	}

	trả_về pmp_flags;
}

bool sbi_domain_check_addr(const struct sbi_domain *dom,
			   unsigned long addr, unsigned long mode,
			   unsigned long access_flags)
{
	bool rmmio, mmio = false;
	struct sbi_domain_memregion *reg;
	unsigned long rstart, rend, rflags, rwx = 0, rrwx = 0;

	nếu (!dom)
		trả_về false;

	/*
	 * Use M_{R/W/X} bits because the SU-bits are at the
	 * same relative offsets. If the mode is not M, the SU
	 * bits will fall at same offsets after the shift.
	 */
	nếu (access_flags & SBI_DOMAIN_READ)
		rwx |= SBI_DOMAIN_MEMREGION_M_READABLE;

	nếu (access_flags & SBI_DOMAIN_WRITE)
		rwx |= SBI_DOMAIN_MEMREGION_M_WRITABLE;

	nếu (access_flags & SBI_DOMAIN_EXECUTE)
		rwx |= SBI_DOMAIN_MEMREGION_M_EXECUTABLE;

	nếu (access_flags & SBI_DOMAIN_MMIO)
		mmio = true;

	sbi_domain_for_each_memregion(dom, reg) {
		rflags = reg->flags;
		rrwx = (mode == PRV_M ?
			(rflags & SBI_DOMAIN_MEMREGION_M_ACCESS_MASK) :
			(rflags & SBI_DOMAIN_MEMREGION_SU_ACCESS_MASK)
			>> SBI_DOMAIN_MEMREGION_SU_ACCESS_SHIFT);

		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		nếu (rstart <= addr && addr <= rend) {
			rmmio = (rflags & SBI_DOMAIN_MEMREGION_MMIO) ? true : false;
			/*
			 * MMIO devices may appear in regions without the flag set (such as the
			 * default region), but MMIO device regions should not be used as memory.
			 */
			nếu (!mmio && rmmio)
				trả_về false;
			trả_về ((rrwx & rwx) == rwx) ? true : false;
		}
	}

	trả_về (mode == PRV_M) ? true : false;
}

/* Check if region complies with constraints */
static bool is_region_valid(const struct sbi_domain_memregion *reg)
{
	nếu (reg->order < 3 || __riscv_xlen < reg->order)
		trả_về false;

	nếu (reg->order == __riscv_xlen && reg->base != 0)
		trả_về false;

	nếu (reg->order < __riscv_xlen && (reg->base & (BIT(reg->order) - 1)))
		trả_về false;

	trả_về true;
}

/** Check if regionA can be replaced by regionB */
static bool is_region_compatible(const struct sbi_domain_memregion *regA,
				 const struct sbi_domain_memregion *regB)
{
	nếu (sbi_domain_memregion_is_subset(regA, regB) &&
	    regA->flags == regB->flags)
		trả_về true;

	trả_về false;
}

/** Check if regionA should be placed before regionB */
static bool is_region_before(const struct sbi_domain_memregion *regA,
			     const struct sbi_domain_memregion *regB)
{
	/*
	 * Enforce firmware region ordering for memory access
	 * under SmePMP.
	 * Place firmware regions first to ensure consistent
	 * PMP entries during domain context switches.
	 */
	nếu (SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	   !SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		trả_về true;
	nếu (!SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regA->flags) &&
	    SBI_DOMAIN_MEMREGION_IS_FIRMWARE(regB->flags))
		trả_về false;

	nếu (regA->order < regB->order)
		trả_về true;

	nếu ((regA->order == regB->order) &&
	    (regA->base < regB->base))
		trả_về true;

	trả_về false;
}

static const struct sbi_domain_memregion *find_region(
						const struct sbi_domain *dom,
						unsigned long addr)
{
	unsigned long rstart, rend;
	struct sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;
		nếu (rstart <= addr && addr <= rend)
			trả_về reg;
	}

	trả_về NULL;
}

static const struct sbi_domain_memregion *find_next_subset_region(
				const struct sbi_domain *dom,
				const struct sbi_domain_memregion *reg,
				unsigned long addr)
{
	struct sbi_domain_memregion *sreg, *ret = NULL;

	sbi_domain_for_each_memregion(dom, sreg) {
		nếu (sreg == reg || (sreg->base <= addr) ||
		    !sbi_domain_memregion_is_subset(sreg, reg))
			continue;

		nếu (!ret || (sreg->base < ret->base) ||
		    ((sreg->base == ret->base) && (sreg->order < ret->order)))
			ret = sreg;
	}

	trả_về ret;
}

static void swap_region(struct sbi_domain_memregion* reg1,
			struct sbi_domain_memregion* reg2)
{
	struct sbi_domain_memregion treg;

	sbi_memcpy(&treg, reg1, sizeof(treg));
	sbi_memcpy(reg1, reg2, sizeof(treg));
	sbi_memcpy(reg2, &treg, sizeof(treg));
}

static số_nguyên sbi_domain_used_memregions(const struct sbi_domain *dom)
{
	số_nguyên count = 0;
	struct sbi_domain_memregion *reg;

	sbi_domain_for_each_memregion(dom, reg)
		count++;

	trả_về count;
}

static số_nguyên sanitize_domain(struct sbi_domain *dom)
{
	u32 i, j, count;
	bool is_covered;
	struct sbi_domain_memregion *reg, *reg1;

	/* Check possible HARTs */
	nếu (!dom->possible_harts) {
		sbi_printf("%s: %s possible HART mask is NULL\n",
			   __func__, dom->name);
		trả_về SBI_EINVAL;
	}
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		nếu (!sbi_hartindex_valid(i)) {
			sbi_printf("%s: %s possible HART mask has invalid "
				   "hart %d\n", __func__,
				   dom->name, sbi_hartindex_to_hartid(i));
			trả_về SBI_EINVAL;
		}
	}

	/* Check memory regions */
	nếu (!dom->regions) {
		sbi_printf("%s: %s regions is NULL\n",
			   __func__, dom->name);
		trả_về SBI_EINVAL;
	}
	sbi_domain_for_each_memregion(dom, reg) {
		nếu (!is_region_valid(reg)) {
			sbi_printf("%s: %s has invalid region base=0x%lx "
				   "order=%lu flags=0x%lx\n", __func__,
				   dom->name, reg->base, reg->order,
				   reg->flags);
			trả_về SBI_EINVAL;
		}
	}

	/* Count memory regions */
	count = sbi_domain_used_memregions(dom);

	/* Check presence of firmware regions */
	nếu (!dom->fw_region_inited) {
		sbi_printf("%s: %s does not have firmware region\n",
			   __func__, dom->name);
		trả_về SBI_EINVAL;
	}

	/* Sort the memory regions */
	cho (i = 0; i < (count - 1); i++) {
		reg = &dom->regions[i];
		cho (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			nếu (!is_region_before(reg1, reg))
				continue;

			swap_region(reg, reg1);
		}
	}

	/* Remove covered regions */
	cho (i = 0; i < (count - 1);) {
		is_covered = false;
		reg = &dom->regions[i];

		cho (j = i + 1; j < count; j++) {
			reg1 = &dom->regions[j];

			nếu (is_region_compatible(reg, reg1)) {
				is_covered = true;
				break;
			}
		}

		/* find a region is superset of reg, remove reg */
		nếu (is_covered) {
			sbi_memmove(reg, reg + 1, sizeof(*reg) * (count - i));
			count--;
		} không_thì
			i++;
	}

	/*
	 * We don't need to check boot HART id of domain because if boot
	 * HART id is not possible/assigned to this domain then it won't
	 * be started at boot-time by sbi_domain_finalize().
	 */

	/*
	 * Check next mode
	 *
	 * We only allow next mode to be S-mode or U-mode, so that we can
	 * protect M-mode context and enforce checks on memory accesses.
	 */
	nếu (dom->next_mode != PRV_S &&
	    dom->next_mode != PRV_U) {
		sbi_printf("%s: %s invalid next booting stage mode 0x%lx\n",
			   __func__, dom->name, dom->next_mode);
		trả_về SBI_EINVAL;
	}

	/* Check next address and next mode */
	nếu (!sbi_domain_check_addr(dom, dom->next_addr, dom->next_mode,
				   SBI_DOMAIN_EXECUTE)) {
		sbi_printf("%s: %s next booting stage address 0x%lx can't "
			   "execute\n", __func__, dom->name, dom->next_addr);
		trả_về SBI_EINVAL;
	}

	trả_về 0;
}

bool sbi_domain_check_addr_range(const struct sbi_domain *dom,
				 unsigned long addr, unsigned long size,
				 unsigned long mode,
				 unsigned long access_flags)
{
	unsigned long max = addr + size;
	const struct sbi_domain_memregion *reg, *sreg;

	nếu (!dom)
		trả_về false;

	nếu (size && max <= addr)
		trả_về false;

	trong_khi (addr < max) {
		reg = find_region(dom, addr);
		nếu (!reg)
			trả_về false;

		nếu (!sbi_domain_check_addr(dom, addr, mode, access_flags))
			trả_về false;

		sreg = find_next_subset_region(dom, reg, addr);
		nếu (sreg)
			addr = sreg->base;
		không_thì nếu (reg->order < __riscv_xlen)
			addr = reg->base + (1UL << reg->order);
		không_thì
			break;
	}

	trả_về true;
}

void sbi_domain_dump(const struct sbi_domain *dom, const char *suffix)
{
	u32 i, j, k;
	unsigned long rstart, rend;
	struct sbi_domain_memregion *reg;

	sbi_printf("Domain%d Name        %s: %s\n",
		   dom->index, suffix, dom->name);

	sbi_printf("Domain%d Init Order  %s: 0x%x\n",
		   dom->index, suffix, dom->init_order);

	sbi_printf("Domain%d Boot HART   %s: %d\n",
		   dom->index, suffix, dom->boot_hartid);

	k = 0;
	sbi_printf("Domain%d HARTs       %s: ", dom->index, suffix);
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		j = sbi_hartindex_to_hartid(i);
		sbi_printf("%s0x%x%s", (k++) ? "," : "",
			   j, sbi_domain_is_assigned_hart(dom, i) ? "*" : "");
	}
	sbi_printf("\n");

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		rstart = reg->base;
		rend = (reg->order < __riscv_xlen) ?
			rstart + ((1UL << reg->order) - 1) : -1UL;

		sbi_printf("Domain%d Region%02d    %s: 0x%" PRILX "-0x%" PRILX " ",
			   dom->index, i, suffix, rstart, rend);

		k = 0;

		sbi_printf("M: ");
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			sbi_printf("%cI", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_FW)
			sbi_printf("%cF", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_M_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s ", (k++) ? ")" : "()");

		k = 0;
		sbi_printf("S/U: ");
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			sbi_printf("%cR", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			sbi_printf("%cW", (k++) ? ',' : '(');
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			sbi_printf("%cX", (k++) ? ',' : '(');
		sbi_printf("%s\n", (k++) ? ")" : "()");

		i++;
	}

	sbi_printf("Domain%d Next Address%s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_addr);

	sbi_printf("Domain%d Next Arg1   %s: 0x%" PRILX "\n",
		   dom->index, suffix, dom->next_arg1);

	sbi_printf("Domain%d Next Mode   %s: ", dom->index, suffix);
	switch (dom->next_mode) {
	case PRV_M:
		sbi_printf("M-mode\n");
		break;
	case PRV_S:
		sbi_printf("S-mode\n");
		break;
	case PRV_U:
		sbi_printf("U-mode\n");
		break;
	default:
		sbi_printf("Unknown\n");
		break;
	}

	sbi_printf("Domain%d SysReset    %s: %s\n",
		   dom->index, suffix, (dom->system_reset_allowed) ? "yes" : "no");

	sbi_printf("Domain%d SysSuspend  %s: %s\n",
		   dom->index, suffix, (dom->system_suspend_allowed) ? "yes" : "no");
}

void sbi_domain_dump_all(const char *suffix)
{
	const struct sbi_domain *dom;

	sbi_domain_for_each(dom) {
		sbi_domain_dump(dom, suffix);
		sbi_printf("\n");
	}
}

số_nguyên sbi_domain_register(struct sbi_domain *dom)
	NO_THREAD_SAFETY_ANALYSIS
{
	u32 i, cold_hartid = current_hartid();
	struct sbi_domain *tdom;
	số_nguyên rc;

	/* Sanity checks */
	nếu (!dom || domain_finalized)
		trả_về SBI_EINVAL;

	/*
	 * Ensure that:
	 *  1) Domain not already registered
	 *  2) Initialization order is unique
	 */
	sbi_domain_for_each(tdom) {
		nếu (tdom == dom ||
		    tdom->init_order == dom->init_order)
			trả_về SBI_EALREADY;
	}

	/* Sanitize discovered domain */
	rc = sanitize_domain(dom);
	nếu (rc) {
		sbi_printf("%s: sanity checks failed for"
			   " %s (error %d)\n", __func__,
			   dom->name, rc);
		trả_về rc;
	}

	sbi_list_add_tail(&dom->node, &domain_list);

	/* Assign index to domain */
	dom->index = domain_count++;

	/* Initialize spinlock for dom->assigned_harts */
	SPIN_LOCK_INIT(dom->assigned_harts_lock);

	/* Clear assigned HARTs of domain */
	sbi_hartmask_clear_all(&dom->assigned_harts);

	/*
	 * Assign HART to a domain with the least initialization order
	 * where the HART is listed as a possible HART of the domain.
	 */
	sbi_hartmask_for_each_hartindex(i, dom->possible_harts) {
		tdom = sbi_hartindex_to_domain(i);
		nếu (tdom) {
			nếu (tdom->init_order > dom->init_order)
				sbi_hartmask_clear_hartindex(i, &tdom->assigned_harts);
			không_thì
				continue;
		}

		sbi_update_hartindex_to_domain(i, dom);
		sbi_hartmask_set_hartindex(i, &dom->assigned_harts);

		/*
		 * If cold boot HART is assigned to this domain then
		 * override boot HART of this domain.
		 */
		nếu (sbi_hartindex_to_hartid(i) == cold_hartid &&
		    dom->boot_hartid != cold_hartid) {
			sbi_printf("Domain%d Boot HARTID forced to"
				   " %d\n", dom->index, cold_hartid);
			dom->boot_hartid = cold_hartid;
		}
	}

	/* Setup state for the discovered domain */
	rc = sbi_domain_setup_state(dom);
	nếu (rc) {
		sbi_printf("%s: domain state setup failed for %s (error %d)\n",
			   __func__, dom->name, rc);
		sbi_list_del(&dom->node);
		trả_về rc;
	}

	trả_về 0;
}

static số_nguyên root_add_memregion(const struct sbi_domain_memregion *reg)
{
	số_nguyên rc;
	bool reg_merged;
	struct sbi_domain_memregion *nreg, *nreg1;
	số_nguyên root_memregs_count = sbi_domain_used_memregions(&root);

	/* Sanity checks */
	nếu (!reg || domain_finalized || !root.regions ||
	    (ROOT_REGION_MAX <= root_memregs_count))
		trả_về SBI_EINVAL;

	/* Check whether compatible region exists for the new one */
	sbi_domain_for_each_memregion(&root, nreg) {
		nếu (is_region_compatible(reg, nreg))
			trả_về 0;
	}

	/* Append the memregion to root memregions */
	nreg = &root.regions[root_memregs_count];
	sbi_memcpy(nreg, reg, sizeof(*reg));
	root_memregs_count++;
	root.regions[root_memregs_count].order = 0;

	/* Sort and optimize root regions */
	do {
		/* Sanitize the root domain so that memregions are sorted */
		rc = sanitize_domain(&root);
		nếu (rc) {
			sbi_printf("%s: sanity checks failed for"
				   " %s (error %d)\n", __func__,
				   root.name, rc);
			trả_về rc;
		}

		/* Merge consecutive memregions with same order and flags */
		reg_merged = false;
		sbi_domain_for_each_memregion(&root, nreg) {
			nreg1 = nreg + 1;
			nếu (!nreg1->order)
				continue;

			nếu (!(nreg->base & (BIT(nreg->order + 1) - 1)) &&
			    (nreg->base + BIT(nreg->order)) == nreg1->base &&
			    nreg->order == nreg1->order &&
			    nreg->flags == nreg1->flags) {
				số_nguyên i1 = nreg1 - root.regions;
				nreg->order++;
				sbi_memmove(nreg1, nreg1 + 1,
					    sizeof(*nreg1) * (root_memregs_count - i1));
				reg_merged = true;
				root_memregs_count--;
			}
		}
	} trong_khi (reg_merged);

	trả_về 0;
}

số_nguyên sbi_domain_root_add_memrange(unsigned long addr, unsigned long size,
			   unsigned long align, unsigned long region_flags)
{
	số_nguyên rc;
	unsigned long pos, end, rsize;
	struct sbi_domain_memregion reg;

	pos = addr;
	end = addr + size;
	trong_khi (pos < end) {
		rsize = pos & (align - 1);
		nếu (rsize)
			rsize = 1UL << sbi_ffs(pos);
		không_thì
			rsize = ((end - pos) < align) ?
				(end - pos) : align;

		sbi_domain_memregion_init(pos, rsize, region_flags, &reg);
		rc = root_add_memregion(&reg);
		nếu (rc)
			trả_về rc;
		pos += rsize;
	}

	trả_về 0;
}

số_nguyên sbi_domain_startup(struct sbi_scratch *scratch, u32 cold_hartid)
{
	số_nguyên rc;
	u32 dhart;
	struct sbi_domain *dom;

	/* Sanity checks */
	nếu (!domain_finalized)
		trả_về SBI_EINVAL;

	/* Startup boot HART of domains */
	sbi_domain_for_each(dom) {
		/* Domain boot HART index */
		dhart = sbi_hartid_to_hartindex(dom->boot_hartid);

		/* Ignore of boot HART is off limits */
		nếu (!sbi_hartindex_valid(dhart))
			continue;

		/* Ignore if boot HART not possible for this domain */
		nếu (!sbi_hartmask_test_hartindex(dhart, dom->possible_harts))
			continue;

		/* Ignore if boot HART assigned different domain */
		nếu (sbi_hartindex_to_domain(dhart) != dom)
			continue;

		/* Ignore if boot HART is not part of the assigned HARTs */
		spin_lock(&dom->assigned_harts_lock);
		rc = sbi_hartmask_test_hartindex(dhart, &dom->assigned_harts);
		spin_unlock(&dom->assigned_harts_lock);
		nếu (!rc)
			continue;

		/* Startup boot HART of domain */
		nếu (dom->boot_hartid == cold_hartid) {
			scratch->next_addr = dom->next_addr;
			scratch->next_mode = dom->next_mode;
			scratch->next_arg1 = dom->next_arg1;
		} không_thì {
			rc = sbi_hsm_hart_start(scratch, NULL,
						dom->boot_hartid,
						dom->next_addr,
						dom->next_mode,
						dom->next_arg1);
			nếu (rc) {
				sbi_printf("%s: failed to start boot HART %d"
					   " for %s (error %d)\n", __func__,
					   dom->boot_hartid, dom->name, rc);
				trả_về rc;
			}
		}
	}

	trả_về 0;
}

số_nguyên sbi_domain_finalize(struct sbi_scratch *scratch)
{
	số_nguyên rc;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Sanity checks */
	nếu (domain_finalized)
		trả_về SBI_EINVAL;

	/* Initialize and populate domains for the platform */
	rc = sbi_platform_domains_init(plat);
	nếu (rc) {
		sbi_printf("%s: platform domains_init() failed (error %d)\n",
			   __func__, rc);
		trả_về rc;
	}

	/*
	 * Set the finalized flag so that the root domain
	 * regions can't be changed.
	 */
	domain_finalized = true;

	trả_về 0;
}

số_nguyên sbi_domain_init(struct sbi_scratch *scratch, u32 cold_hartid)
{
	số_nguyên rc;
	struct sbi_hartmask *root_hmask;
	struct sbi_domain_memregion *root_memregs;
	số_nguyên root_memregs_count = 0;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	bool fw_single_region = sbi_platform_single_fw_region(plat);

	SBI_INIT_LIST_HEAD(&domain_list);

	nếu (!fw_single_region) {
		nếu (scratch->fw_rw_offset == 0 ||
		    (scratch->fw_rw_offset & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_rw_offset is not a power of 2 (0x%lx)\n",
				   __func__, scratch->fw_rw_offset);
			trả_về SBI_EINVAL;
		}
		nếu ((scratch->fw_start & (scratch->fw_rw_offset - 1)) != 0) {
			sbi_printf("%s: fw_start and fw_rw_offset not aligned\n",
				   __func__);
			trả_về SBI_EINVAL;
		}
	}

	domain_hart_ptr_offset = sbi_scratch_alloc_type_offset(void *);
	nếu (!domain_hart_ptr_offset)
		trả_về SBI_ENOMEM;

	/* Initialize domain context support */
	rc = sbi_domain_context_init();
	nếu (rc)
		goto fail_free_domain_hart_ptr_offset;

	root_memregs = sbi_calloc(sizeof(*root_memregs), ROOT_REGION_MAX + 1);
	nếu (!root_memregs) {
		sbi_printf("%s: no memory for root regions\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_deinit_context;
	}
	root.regions = root_memregs;

	root_hmask = sbi_zalloc(sizeof(*root_hmask));
	nếu (!root_hmask) {
		sbi_printf("%s: no memory for root hartmask\n", __func__);
		rc = SBI_ENOMEM;
		goto fail_free_root_memregs;
	}
	root.possible_harts = root_hmask;

	/* Root domain firmware memory region */
	nếu (fw_single_region) {
		sbi_domain_memregion_init(scratch->fw_start, scratch->fw_size,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	} không_thì {
		sbi_domain_memregion_init(scratch->fw_start,
					  scratch->fw_rw_offset,
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_EXECUTABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);

		sbi_domain_memregion_init((scratch->fw_start +
					   scratch->fw_rw_offset),
					  (scratch->fw_size -
					   scratch->fw_rw_offset),
					  (SBI_DOMAIN_MEMREGION_M_READABLE |
					   SBI_DOMAIN_MEMREGION_M_WRITABLE |
					   SBI_DOMAIN_MEMREGION_FW),
					  &root_memregs[root_memregs_count++]);
	}

	root.fw_region_inited = true;

	/*
	 * Allow SU RWX on rest of the memory region. Since pmp entries
	 * have implicit priority on index, previous entries will
	 * deny access to SU on M-mode region. Also, M-mode will not
	 * have access to SU region while previous entries will allow
	 * access to M-mode regions.
	 */
	sbi_domain_memregion_init(0, ~0UL,
				  (SBI_DOMAIN_MEMREGION_SU_READABLE |
				   SBI_DOMAIN_MEMREGION_SU_WRITABLE |
				   SBI_DOMAIN_MEMREGION_SU_EXECUTABLE),
				  &root_memregs[root_memregs_count++]);

	/* Root domain memory region end */
	root_memregs[root_memregs_count].order = 0;

	/* Root domain boot HART id is same as coldboot HART id */
	root.boot_hartid = cold_hartid;

	/* Root domain next booting stage details */
	root.next_arg1 = scratch->next_arg1;
	root.next_addr = scratch->next_addr;
	root.next_mode = scratch->next_mode;

	/* Root domain possible and assigned HARTs */
	sbi_for_each_hartindex(i)
		sbi_hartmask_set_hartindex(i, root_hmask);

	/* Finally register the root domain */
	rc = sbi_domain_register(&root);
	nếu (rc)
		goto fail_free_root_hmask;

	trả_về 0;

fail_free_root_hmask:
	sbi_free(root_hmask);
fail_free_root_memregs:
	sbi_free(root_memregs);
fail_deinit_context:
	sbi_domain_context_deinit();
fail_free_domain_hart_ptr_offset:
	sbi_scratch_free_offset(domain_hart_ptr_offset);
	trả_về rc;
}
