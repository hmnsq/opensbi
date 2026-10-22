
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_math.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_fixup.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_pmu.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>

পূর্ণসংখ্যা fdt_add_cpu_idle_states(শূন্য *fdt, ধ্রুবক কাঠামো sbi_cpu_idle_state *state)
{
	পূর্ণসংখ্যা cpu_গ্রন্থি, cpus_গ্রন্থি, err, idle_states_গ্রন্থি;
	uint32_t গুনতি, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	যদি (err < 0)
		ফেরত err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	যদি (err < 0)
		ফেরত err;

	cpus_গ্রন্থি = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_গ্রন্থি < 0)
		ফেরত cpus_গ্রন্থি;

	/* Do nothing if the idle-states node already exists. */
	idle_states_গ্রন্থি = fdt_subnode_offset(fdt, cpus_গ্রন্থি, "idle-states");
	যদি (idle_states_গ্রন্থি >= 0)
		ফেরত 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_গ্রন্থি = fdt_add_subnode(fdt, cpus_গ্রন্থি, "idle-states");
	যদি (idle_states_গ্রন্থি < 0)
		ফেরত idle_states_গ্রন্থি;

	জন্য (গুনতি = 0; state->name; গুনতি++, phandle++, state++) {
		পূর্ণসংখ্যা idle_state_গ্রন্থি;

		idle_state_গ্রন্থি = fdt_add_subnode(fdt, idle_states_গ্রন্থি,
						  state->name);
		যদি (idle_state_গ্রন্থি < 0)
			ফেরত idle_state_গ্রন্থি;

		fdt_setprop_string(fdt, idle_state_গ্রন্থি, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_গ্রন্থি,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		যদি (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_গ্রন্থি,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_গ্রন্থি, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_গ্রন্থি, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_গ্রন্থি, "min-residency-us",
				state->min_residency_us);
		যদি (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_গ্রন্থি,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_গ্রন্থি, "phandle", phandle);
	}

	যদি (গুনতি == 0)
		ফেরত 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_গ্রন্থি, fdt, cpus_গ্রন্থি) {
		ধ্রুবক অক্ষর *device_type;
		fdt32_t *মান;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_গ্রন্থি, "device_type", ফাঁকা);
		যদি (!device_type || strcmp(device_type, "cpu"))
			চালু;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_গ্রন্থি, "cpu-idle-states",
					      গুনতি * আকার(phandle),
					      (শূন্য **)&মান);
		যদি (err < 0)
			ফেরত err;

		/* Fill in the phandles of the idle state nodes. */
		জন্য (uint32_t i = 0; i < গুনতি; ++i)
			মান[i] = cpu_to_fdt32(phandle - গুনতি + i);
	}

	ফেরত 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
স্থির শূন্য fdt_cpu_fixup_needed(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	ধ্রুবক অক্ষর *mmu_type, *extensions;
	u32 hartid, hartindex;
	পূর্ণসংখ্যা len;

	*disable = false;
	*add_zicntr = false;

	যদি (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		ফেরত;

	যদি (!fdt_গ্রন্থি_is_enabled(fdt, cpu_offset))
		ফেরত;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	যদি (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	যদি (!emulated_zicntr)
		ফেরত;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	যদি (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

শূন্য fdt_cpu_fixup(শূন্য *fdt)
{
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	পূর্ণসংখ্যা err, cpu_offset, cpus_offset;
	পূর্ণসংখ্যা reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ধ্রুবক পূর্ণসংখ্যা status_আকার = আকার(কাঠামো fdt_property) +
				ROUNDUP(আকার("disabled"), FDT_TAGSIZE) +
				আকার("status");
	ধ্রুবক পূর্ণসংখ্যা zicntr_আকার = ROUNDUP(আকার("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ফাঁকা &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		যদি (disable)
			reserve += status_আকার;
		যদি (add_zicntr)
			reserve += zicntr_আকার;
	}

	যদি (!reserve)
		ফেরত;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	যদি (err < 0)
		ফেরত;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		যদি (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			যদি (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ফাঁকা),
					   err);
		}

		যদি (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			যদি (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ফাঁকা),
					   err);
		}
	}
}

স্থির শূন্য fdt_domain_based_fixup_one(শূন্য *fdt, পূর্ণসংখ্যা nodeoff)
{
	পূর্ণসংখ্যা rc;
	uint64_t reg_addr, reg_আকার;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_addr || !reg_আকার)
		ফেরত;

	যদি (!sbi_domain_check_addr(dom, reg_addr, dom->পরবর্তী_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		যদি (rc < 0)
			ফেরত;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

স্থির শূন্য fdt_fixup_গ্রন্থি(শূন্য *fdt, ধ্রুবক অক্ষর *compatible)
{
	পূর্ণসংখ্যা noff = 0;

	যখন ((noff = fdt_গ্রন্থি_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

শূন্য fdt_aplic_fixup(শূন্য *fdt)
{
	fdt_fixup_গ্রন্থি(fdt, "riscv,aplic");
}

শূন্য fdt_imsic_fixup(শূন্য *fdt)
{
	fdt_fixup_গ্রন্থি(fdt, "riscv,imsics");
}

শূন্য fdt_plic_fixup(শূন্য *fdt)
{
	u32 *cells;
	পূর্ণসংখ্যা i, cells_গুনতি;
	পূর্ণসংখ্যা plic_off;

	plic_off = fdt_গ্রন্থি_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	যদি (plic_off < 0) {
		plic_off = fdt_গ্রন্থি_offset_by_compatible(fdt, 0, "riscv,plic0");
		যদি (plic_off < 0)
			ফেরত;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_গুনতি);
	যদি (!cells)
		ফেরত;

	cells_গুনতি = cells_গুনতি / আকার(u32);
	যদি (!cells_গুনতি)
		ফেরত;

	জন্য (i = 0; i < (cells_গুনতি / 2); i++) {
		যদি (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

স্থির পূর্ণসংখ্যা fdt_resv_memory_update_গ্রন্থি(শূন্য *fdt, unsigned long addr,
				       unsigned long আকার_2, পূর্ণসংখ্যা সূচক_2,
				       পূর্ণসংখ্যা parent)
{
	পূর্ণসংখ্যা na = fdt_address_cells(fdt, 0);
	পূর্ণসংখ্যা ns = fdt_আকার_cells(fdt, 0);
	fdt32_t addr_উপরের, addr_নিচের;
	fdt32_t আকার_উপরের, আকার_নিচের;
	পূর্ণসংখ্যা subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	অক্ষর name[32];

	addr_উপরের = (u64)addr >> 32;
	addr_নিচের = addr;
	আকার_উপরের = (u64)আকার_2 >> 32;
	আকার_নিচের = আকার_2;

	যদি (na > 1 && addr_উপরের)
		sbi_snprintf(name, আকার(name),
			     "mmode_resv%d@%x,%x", সূচক_2,
			     addr_উপরের, addr_নিচের);
	অন্যথা
		sbi_snprintf(name, আকার(name),
			     "mmode_resv%d@%x", সূচক_2,
			     addr_নিচের);

	subnode = fdt_add_subnode(fdt, parent, name);
	যদি (subnode < 0)
		ফেরত subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	যদি (err < 0)
		ফেরত err;

	/* encode the <reg> property value */
	val = reg;
	যদি (na > 1)
		*val++ = cpu_to_fdt32(addr_উপরের);
	*val++ = cpu_to_fdt32(addr_নিচের);
	যদি (ns > 1)
		*val++ = cpu_to_fdt32(আকার_উপরের);
	*val++ = cpu_to_fdt32(আকার_নিচের);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * আকার(fdt32_t));
	যদি (err < 0)
		ফেরত err;

	ফেরত 0;
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
পূর্ণসংখ্যা fdt_reserved_memory_fixup(শূন্য *fdt)
{
	কাঠামো sbi_domain_memregion *reg;
	কাঠামো sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_গুনতি] = { 0 };
	unsigned অক্ষর filtered_order[PMP_গুনতি] = { 0 };
	unsigned long addr, আকার_2;
	পূর্ণসংখ্যা err, parent, i, j;
	পূর্ণসংখ্যা na = fdt_address_cells(fdt, 0);
	পূর্ণসংখ্যা ns = fdt_আকার_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	যদি (err < 0)
		ফেরত err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	যদি (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		যদি (parent < 0)
			ফেরত parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		যদি (err < 0)
			ফেরত err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		যদি (err < 0)
			ফেরত err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		যদি (err < 0)
			ফেরত err;
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
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			চালু;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			চালু;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			চালু;
		যদি (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			চালু;

		যদি (i >= PMP_গুনতি) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ফেরত SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		জন্য (j = 0; j < i; j++) {
			যদি (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				বিরতি;
			}
		}

		যদি (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	জন্য (j = 0; j < i; j++) {
		addr = filtered_base[j];
		আকার_2 = 1UL << filtered_order[j];
		fdt_resv_memory_update_গ্রন্থি(fdt, addr, আকার_2, j, parent);
	}

	ফেরত 0;
}

শূন্য fdt_config_fixup(শূন্য *fdt)
{
	পূর্ণসংখ্যা chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	যদি (chosen_offset < 0)
		ফেরত;

	config_offset = fdt_গ্রন্থি_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	যদি (config_offset < 0)
		ফেরত;

	fdt_nop_গ্রন্থি(fdt, config_offset);
}

স্থির SBI_LIST_শীর্ষ(fixup_list);

পূর্ণসংখ্যা fdt_register_general_fixup(কাঠামো fdt_general_fixup *fixup)
{
	কাঠামো fdt_general_fixup *f;

	যদি (!fixup || !fixup->name || !fixup->do_fixup)
		ফেরত SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, শীর্ষ) {
		যদি (f == fixup)
			ফেরত SBI_EALREADY;
	}

	sbi_list_add_লেজ(&fixup->শীর্ষ, &fixup_list);

	ফেরত 0;
}

শূন্য fdt_unregister_general_fixup(কাঠামো fdt_general_fixup *fixup)
{
	যদি (!fixup)
		ফেরত;

	sbi_list_del(&fixup->শীর্ষ);
}

শূন্য fdt_fixups(শূন্য *fdt)
{
	কাঠামো fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_গ্রন্থি
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, শীর্ষ)
		f->do_fixup(f, fdt);
}
