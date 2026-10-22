
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_math.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_fixup.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_pmu.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_cpu_idle_states(ଶୂନ୍ୟ *fdt, ଅଚଳ ସଂରଚନା sbi_cpu_idle_state *state)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_ଗଣ୍ଠି, cpus_ଗଣ୍ଠି, err, idle_states_ଗଣ୍ଠି;
	uint32_t ଗଣନା_2, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	cpus_ଗଣ୍ଠି = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_ଗଣ୍ଠି < 0)
		ଫେରନ୍ତୁ cpus_ଗଣ୍ଠି;

	/* Do nothing if the idle-states node already exists. */
	idle_states_ଗଣ୍ଠି = fdt_subnode_offset(fdt, cpus_ଗଣ୍ଠି, "idle-states");
	ଯଦି (idle_states_ଗଣ୍ଠି >= 0)
		ଫେରନ୍ତୁ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_ଗଣ୍ଠି = fdt_add_subnode(fdt, cpus_ଗଣ୍ଠି, "idle-states");
	ଯଦି (idle_states_ଗଣ୍ଠି < 0)
		ଫେରନ୍ତୁ idle_states_ଗଣ୍ଠି;

	ପାଇଁ (ଗଣନା_2 = 0; state->name; ଗଣନା_2++, phandle++, state++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା idle_state_ଗଣ୍ଠି;

		idle_state_ଗଣ୍ଠି = fdt_add_subnode(fdt, idle_states_ଗଣ୍ଠି,
						  state->name);
		ଯଦି (idle_state_ଗଣ୍ଠି < 0)
			ଫେରନ୍ତୁ idle_state_ଗଣ୍ଠି;

		fdt_setprop_string(fdt, idle_state_ଗଣ୍ଠି, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ଯଦି (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_ଗଣ୍ଠି,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି, "min-residency-us",
				state->min_residency_us);
		ଯଦି (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ଗଣ୍ଠି, "phandle", phandle);
	}

	ଯଦି (ଗଣନା_2 == 0)
		ଫେରନ୍ତୁ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ଗଣ୍ଠି, fdt, cpus_ଗଣ୍ଠି) {
		ଅଚଳ ଅକ୍ଷର *device_type;
		fdt32_t *ମୂଲ୍ୟ;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ଗଣ୍ଠି, "device_type", ଖାଲି);
		ଯଦି (!device_type || strcmp(device_type, "cpu"))
			ଜାରି;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ଗଣ୍ଠି, "cpu-idle-states",
					      ଗଣନା_2 * ଆକାର(phandle),
					      (ଶୂନ୍ୟ **)&ମୂଲ୍ୟ);
		ଯଦି (err < 0)
			ଫେରନ୍ତୁ err;

		/* Fill in the phandles of the idle state nodes. */
		ପାଇଁ (uint32_t i = 0; i < ଗଣନା_2; ++i)
			ମୂଲ୍ୟ[i] = cpu_to_fdt32(phandle - ଗଣନା_2 + i);
	}

	ଫେରନ୍ତୁ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ସ୍ଥିର ଶୂନ୍ୟ fdt_cpu_fixup_needed(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	ଅଚଳ ଅକ୍ଷର *mmu_type, *extensions;
	u32 hartid, hartindex;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	*disable = false;
	*add_zicntr = false;

	ଯଦି (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		ଫେରନ୍ତୁ;

	ଯଦି (!fdt_ଗଣ୍ଠି_is_enabled(fdt, cpu_offset))
		ଫେରନ୍ତୁ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	ଯଦି (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ଯଦି (!emulated_zicntr)
		ଫେରନ୍ତୁ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ଯଦି (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

ଶୂନ୍ୟ fdt_cpu_fixup(ଶୂନ୍ୟ *fdt)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, cpu_offset, cpus_offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା status_size = ଆକାର(ସଂରଚନା fdt_property) +
				ROUNDUP(ଆକାର("disabled"), FDT_TAGSIZE) +
				ଆକାର("status");
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା zicntr_size = ROUNDUP(ଆକାର("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ଖାଲି &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		ଯଦି (disable)
			reserve += status_size;
		ଯଦି (add_zicntr)
			reserve += zicntr_size;
	}

	ଯଦି (!reserve)
		ଫେରନ୍ତୁ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ଯଦି (cpus_offset < 0)
		ଫେରନ୍ତୁ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		ଯଦି (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			ଯଦି (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ଖାଲି),
					   err);
		}

		ଯଦି (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			ଯଦି (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ଖାଲି),
					   err);
		}
	}
}

ସ୍ଥିର ଶୂନ୍ୟ fdt_domain_based_fixup_one(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	uint64_t reg_addr, reg_size;
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ଯଦି (rc < 0 || !reg_addr || !reg_size)
		ଫେରନ୍ତୁ;

	ଯଦି (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ଯଦି (rc < 0)
			ଫେରନ୍ତୁ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ସ୍ଥିର ଶୂନ୍ୟ fdt_fixup_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *compatible)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା noff = 0;

	ଯେତେବେଳେ ((noff = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

ଶୂନ୍ୟ fdt_aplic_fixup(ଶୂନ୍ୟ *fdt)
{
	fdt_fixup_ଗଣ୍ଠି(fdt, "riscv,aplic");
}

ଶୂନ୍ୟ fdt_imsic_fixup(ଶୂନ୍ୟ *fdt)
{
	fdt_fixup_ଗଣ୍ଠି(fdt, "riscv,imsics");
}

ଶୂନ୍ୟ fdt_plic_fixup(ଶୂନ୍ୟ *fdt)
{
	u32 *cells;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cells_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା plic_off;

	plic_off = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ଯଦି (plic_off < 0) {
		plic_off = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, 0, "riscv,plic0");
		ଯଦି (plic_off < 0)
			ଫେରନ୍ତୁ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ଗଣନା);
	ଯଦି (!cells)
		ଫେରନ୍ତୁ;

	cells_ଗଣନା = cells_ଗଣନା / ଆକାର(u32);
	ଯଦି (!cells_ଗଣନା)
		ଫେରନ୍ତୁ;

	ପାଇଁ (i = 0; i < (cells_ଗଣନା / 2); i++) {
		ଯଦି (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_resv_memory_update_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, unsigned long addr,
				       unsigned long size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
				       ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା na = fdt_address_cells(fdt, 0);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_ଉପର, addr_ତଳ;
	fdt32_t size_ଉପର, size_ତଳ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	ଅକ୍ଷର name[32];

	addr_ଉପର = (u64)addr >> 32;
	addr_ତଳ = addr;
	size_ଉପର = (u64)size >> 32;
	size_ତଳ = size;

	ଯଦି (na > 1 && addr_ଉପର)
		sbi_snprintf(name, ଆକାର(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_ଉପର, addr_ତଳ);
	ଅନ୍ୟଥା
		sbi_snprintf(name, ଆକାର(name),
			     "mmode_resv%d@%x", index,
			     addr_ତଳ);

	subnode = fdt_add_subnode(fdt, parent, name);
	ଯଦି (subnode < 0)
		ଫେରନ୍ତୁ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	/* encode the <reg> property value */
	val = reg;
	ଯଦି (na > 1)
		*val++ = cpu_to_fdt32(addr_ଉପର);
	*val++ = cpu_to_fdt32(addr_ତଳ);
	ଯଦି (ns > 1)
		*val++ = cpu_to_fdt32(size_ଉପର);
	*val++ = cpu_to_fdt32(size_ତଳ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * ଆକାର(fdt32_t));
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	ଫେରନ୍ତୁ 0;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_reserved_memory_fixup(ଶୂନ୍ୟ *fdt)
{
	ସଂରଚନା sbi_domain_memregion *reg;
	ସଂରଚନା sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_ଗଣନା] = { 0 };
	unsigned ଅକ୍ଷର filtered_order[PMP_ଗଣନା] = { 0 };
	unsigned long addr, size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, parent, i, j;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା na = fdt_address_cells(fdt, 0);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ଯଦି (err < 0)
		ଫେରନ୍ତୁ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	ଯଦି (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		ଯଦି (parent < 0)
			ଫେରନ୍ତୁ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		ଯଦି (err < 0)
			ଫେରନ୍ତୁ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		ଯଦି (err < 0)
			ଫେରନ୍ତୁ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		ଯଦି (err < 0)
			ଫେରନ୍ତୁ err;
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
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ଜାରି;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ଜାରି;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ଜାରି;
		ଯଦି (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ଜାରି;

		ଯଦି (i >= PMP_ଗଣନା) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ଫେରନ୍ତୁ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		ପାଇଁ (j = 0; j < i; j++) {
			ଯଦି (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				ଭାଙ୍ଗ;
			}
		}

		ଯଦି (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	ପାଇଁ (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_ଗଣ୍ଠି(fdt, addr, size, j, parent);
	}

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ fdt_config_fixup(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	ଯଦି (chosen_offset < 0)
		ଫେରନ୍ତୁ;

	config_offset = fdt_ଗଣ୍ଠି_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	ଯଦି (config_offset < 0)
		ଫେରନ୍ତୁ;

	fdt_nop_ଗଣ୍ଠି(fdt, config_offset);
}

ସ୍ଥିର SBI_LIST_HEAD(fixup_list);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_register_general_fixup(ସଂରଚନା fdt_general_fixup *fixup)
{
	ସଂରଚନା fdt_general_fixup *f;

	ଯଦି (!fixup || !fixup->name || !fixup->do_fixup)
		ଫେରନ୍ତୁ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		ଯଦି (f == fixup)
			ଫେରନ୍ତୁ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ fdt_unregister_general_fixup(ସଂରଚନା fdt_general_fixup *fixup)
{
	ଯଦି (!fixup)
		ଫେରନ୍ତୁ;

	sbi_list_del(&fixup->head);
}

ଶୂନ୍ୟ fdt_fixups(ଶୂନ୍ୟ *fdt)
{
	ସଂରଚନା fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_ଗଣ୍ଠି
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
