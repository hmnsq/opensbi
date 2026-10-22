
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_pmu.h>
#include <sbi_utils/fdt/fdt_helper.h>

puur_naa_nka fdt_add_cpu_idle_states(void *fdt, const sa_nracanaa sbi_cpu_idle_state *state)
{
	puur_naa_nka cpu_node, cpus_node, err, idle_states_node;
	uint32_t count, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	cpus_node = fdt_path_offset(fdt, "/cpus");
	ᰀᰧ (cpus_node < 0)
		ᰀᰧᰌ cpus_node;

	/* Do nothing if the idle-states node already exists. */
	idle_states_node = fdt_subnode_offset(fdt, cpus_node, "idle-states");
	ᰀᰧ (idle_states_node >= 0)
		ᰀᰧᰌ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_node = fdt_add_subnode(fdt, cpus_node, "idle-states");
	ᰀᰧ (idle_states_node < 0)
		ᰀᰧᰌ idle_states_node;

	for (count = 0; state->name; count++, phandle++, state++) {
		puur_naa_nka idle_state_node;

		idle_state_node = fdt_add_subnode(fdt, idle_states_node,
						  state->name);
		ᰀᰧ (idle_state_node < 0)
			ᰀᰧᰌ idle_state_node;

		fdt_setprop_string(fdt, idle_state_node, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_node,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ᰀᰧ (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_node,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_node, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "min-residency-us",
				state->min_residency_us);
		ᰀᰧ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_node,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "phandle", phandle);
	}

	ᰀᰧ (count == 0)
		ᰀᰧᰌ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_node, fdt, cpus_node) {
		const var_na *device_type;
		fdt32_t *value;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_node, "device_type", NULL);
		ᰀᰧ (!device_type || strcmp(device_type, "cpu"))
			continue;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_node, "cpu-idle-states",
					      count * parimaa_na(phandle),
					      (void **)&value);
		ᰀᰧ (err < 0)
			ᰀᰧᰌ err;

		/* Fill in the phandles of the idle state nodes. */
		for (uint32_t i = 0; i < count; ++i)
			value[i] = cpu_to_fdt32(phandle - count + i);
	}

	ᰀᰧᰌ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
static void fdt_cpu_fixup_needed(const void *fdt, puur_naa_nka cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	const var_na *mmu_type, *extensions;
	u32 hartid, hartindex;
	puur_naa_nka len;

	*disable = false;
	*add_zicntr = false;

	ᰀᰧ (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		ᰀᰧᰌ;

	ᰀᰧ (!fdt_node_is_enabled(fdt, cpu_offset))
		ᰀᰧᰌ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	ᰀᰧ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ᰀᰧ (!emulated_zicntr)
		ᰀᰧᰌ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ᰀᰧ (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

void fdt_cpu_fixup(void *fdt)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	puur_naa_nka err, cpu_offset, cpus_offset;
	puur_naa_nka reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	const puur_naa_nka status_size = parimaa_na(sa_nracanaa fdt_property) +
				ROUNDUP(parimaa_na("disabled"), FDT_TAGSIZE) +
				parimaa_na("status");
	const puur_naa_nka zicntr_size = ROUNDUP(parimaa_na("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != NULL &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᰀᰧ (cpus_offset < 0)
		ᰀᰧᰌ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		ᰀᰧ (disable)
			reserve += status_size;
		ᰀᰧ (add_zicntr)
			reserve += zicntr_size;
	}

	ᰀᰧ (!reserve)
		ᰀᰧᰌ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ᰀᰧ (err < 0)
		ᰀᰧᰌ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᰀᰧ (cpus_offset < 0)
		ᰀᰧᰌ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		ᰀᰧ (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			ᰀᰧ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, NULL),
					   err);
		}

		ᰀᰧ (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			ᰀᰧ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, NULL),
					   err);
		}
	}
}

static void fdt_domain_based_fixup_one(void *fdt, puur_naa_nka nodeoff)
{
	puur_naa_nka rc;
	uint64_t reg_addr, reg_size;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ᰀᰧ (rc < 0 || !reg_addr || !reg_size)
		ᰀᰧᰌ;

	ᰀᰧ (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ᰀᰧ (rc < 0)
			ᰀᰧᰌ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

static void fdt_fixup_node(void *fdt, const var_na *compatible)
{
	puur_naa_nka noff = 0;

	ᰀᰬ ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

void fdt_aplic_fixup(void *fdt)
{
	fdt_fixup_node(fdt, "riscv,aplic");
}

void fdt_imsic_fixup(void *fdt)
{
	fdt_fixup_node(fdt, "riscv,imsics");
}

void fdt_plic_fixup(void *fdt)
{
	u32 *cells;
	puur_naa_nka i, cells_count;
	puur_naa_nka plic_off;

	plic_off = fdt_node_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ᰀᰧ (plic_off < 0) {
		plic_off = fdt_node_offset_by_compatible(fdt, 0, "riscv,plic0");
		ᰀᰧ (plic_off < 0)
			ᰀᰧᰌ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_count);
	ᰀᰧ (!cells)
		ᰀᰧᰌ;

	cells_count = cells_count / parimaa_na(u32);
	ᰀᰧ (!cells_count)
		ᰀᰧᰌ;

	for (i = 0; i < (cells_count / 2); i++) {
		ᰀᰧ (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

static puur_naa_nka fdt_resv_memory_update_node(void *fdt, unsigned long addr,
				       unsigned long size, puur_naa_nka index,
				       puur_naa_nka parent)
{
	puur_naa_nka na = fdt_address_cells(fdt, 0);
	puur_naa_nka ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_high, addr_low;
	fdt32_t size_high, size_low;
	puur_naa_nka subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	var_na name[32];

	addr_high = (u64)addr >> 32;
	addr_low = addr;
	size_high = (u64)size >> 32;
	size_low = size;

	ᰀᰧ (na > 1 && addr_high)
		sbi_snprintf(name, parimaa_na(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_high, addr_low);
	ᰀᰧᰀ
		sbi_snprintf(name, parimaa_na(name),
			     "mmode_resv%d@%x", index,
			     addr_low);

	subnode = fdt_add_subnode(fdt, parent, name);
	ᰀᰧ (subnode < 0)
		ᰀᰧᰌ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	/* encode the <reg> property value */
	val = reg;
	ᰀᰧ (na > 1)
		*val++ = cpu_to_fdt32(addr_high);
	*val++ = cpu_to_fdt32(addr_low);
	ᰀᰧ (ns > 1)
		*val++ = cpu_to_fdt32(size_high);
	*val++ = cpu_to_fdt32(size_low);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * parimaa_na(fdt32_t));
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	ᰀᰧᰌ 0;
}

/**
 * We use PMP to protect OpenSBI firmware to safe-guard it from buggy S-mode
 * software, see pmp_init() in lib/sbi/sbi_hart.c. The protected memory region
 * information needs to be conveyed to S-mode software (e.g.: operating system)
 * via some well-known method.
 *
 * With device tree, this can be done by inserting a child node of the reserved
 * memory node which is used to specify one or more regions of reserved memory.
 *
 * For the reserved memory node bindings, see Linux kernel documentation at
 * Documentation/devicetree/bindings/reserved-memory/reserved-memory.txt
 *
 * Some additional memory spaces may be protected by platform codes via PMP as
 * well, and corresponding child nodes will be inserted.
 */
puur_naa_nka fdt_reserved_memory_fixup(void *fdt)
{
	sa_nracanaa sbi_domain_memregion *reg;
	sa_nracanaa sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_COUNT] = { 0 };
	unsigned var_na filtered_order[PMP_COUNT] = { 0 };
	unsigned long addr, size;
	puur_naa_nka err, parent, i, j;
	puur_naa_nka na = fdt_address_cells(fdt, 0);
	puur_naa_nka ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	ᰀᰧ (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		ᰀᰧ (parent < 0)
			ᰀᰧᰌ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		ᰀᰧ (err < 0)
			ᰀᰧᰌ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		ᰀᰧ (err < 0)
			ᰀᰧᰌ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		ᰀᰧ (err < 0)
			ᰀᰧᰌ err;
	}

	/*
	 * We assume the given device tree does not contain any memory region
	 * child node protected by PMP. Normally PMP programming happens at
	 * M-mode firmware. The memory space used by OpenSBI is protected.
	 * Some additional memory spaces may be protected by domain memory
	 * regions.
	 *
	 * With above assumption, we create child nodes directly.
	 */

	i = 0;
	sbi_domain_for_each_memregion(dom, reg) {
		/* Ignore MMIO or READABLE or WRITABLE or EXECUTABLE regions */
		ᰀᰧ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			continue;
		ᰀᰧ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			continue;
		ᰀᰧ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			continue;
		ᰀᰧ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			continue;

		ᰀᰧ (i >= PMP_COUNT) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ᰀᰧᰌ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		for (j = 0; j < i; j++) {
			ᰀᰧ (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				bha_nga;
			}
		}

		ᰀᰧ (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	for (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_node(fdt, addr, size, j, parent);
	}

	ᰀᰧᰌ 0;
}

void fdt_config_fixup(void *fdt)
{
	puur_naa_nka chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	ᰀᰧ (chosen_offset < 0)
		ᰀᰧᰌ;

	config_offset = fdt_node_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	ᰀᰧ (config_offset < 0)
		ᰀᰧᰌ;

	fdt_nop_node(fdt, config_offset);
}

static SBI_LIST_HEAD(fixup_list);

puur_naa_nka fdt_register_general_fixup(sa_nracanaa fdt_general_fixup *fixup)
{
	sa_nracanaa fdt_general_fixup *f;

	ᰀᰧ (!fixup || !fixup->name || !fixup->do_fixup)
		ᰀᰧᰌ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		ᰀᰧ (f == fixup)
			ᰀᰧᰌ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	ᰀᰧᰌ 0;
}

void fdt_unregister_general_fixup(sa_nracanaa fdt_general_fixup *fixup)
{
	ᰀᰧ (!fixup)
		ᰀᰧᰌ;

	sbi_list_del(&fixup->head);
}

void fdt_fixups(void *fdt)
{
	sa_nracanaa fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_NODE
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
