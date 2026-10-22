
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_math.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_fixup.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_pmu.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>

முழு fdt_add_cpu_idle_states(வெறுமை *fdt, மாறிலி கட்டமைப்பு sbi_cpu_idle_state *state)
{
	முழு cpu_முனை, cpus_முனை, err, idle_states_முனை;
	uint32_t மொத்தம், phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	எனில் (err < 0)
		திரும்பு err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	எனில் (err < 0)
		திரும்பு err;

	cpus_முனை = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_முனை < 0)
		திரும்பு cpus_முனை;

	/* Do nothing if the idle-states node already exists. */
	idle_states_முனை = fdt_subnode_offset(fdt, cpus_முனை, "idle-states");
	எனில் (idle_states_முனை >= 0)
		திரும்பு 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_முனை = fdt_add_subnode(fdt, cpus_முனை, "idle-states");
	எனில் (idle_states_முனை < 0)
		திரும்பு idle_states_முனை;

	க்கு (மொத்தம் = 0; state->name; மொத்தம்++, phandle++, state++) {
		முழு idle_state_முனை;

		idle_state_முனை = fdt_add_subnode(fdt, idle_states_முனை,
						  state->name);
		எனில் (idle_state_முனை < 0)
			திரும்பு idle_state_முனை;

		fdt_setprop_string(fdt, idle_state_முனை, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_முனை,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		எனில் (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_முனை,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_முனை, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_முனை, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_முனை, "min-residency-us",
				state->min_residency_us);
		எனில் (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_முனை,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_முனை, "phandle", phandle);
	}

	எனில் (மொத்தம் == 0)
		திரும்பு 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_முனை, fdt, cpus_முனை) {
		மாறிலி எழுத்து *device_type;
		fdt32_t *மதிப்பு;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_முனை, "device_type", வெற்று);
		எனில் (!device_type || strcmp(device_type, "cpu"))
			தொடர்;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_முனை, "cpu-idle-states",
					      மொத்தம் * அளவு(phandle),
					      (வெறுமை **)&மதிப்பு);
		எனில் (err < 0)
			திரும்பு err;

		/* Fill in the phandles of the idle state nodes. */
		க்கு (uint32_t i = 0; i < மொத்தம்; ++i)
			மதிப்பு[i] = cpu_to_fdt32(phandle - மொத்தம் + i);
	}

	திரும்பு 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
நிலையான வெறுமை fdt_cpu_fixup_needed(மாறிலி வெறுமை *fdt, முழு cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	மாறிலி எழுத்து *mmu_type, *extensions;
	u32 hartid, hartindex;
	முழு len;

	*disable = false;
	*add_zicntr = false;

	எனில் (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		திரும்பு;

	எனில் (!fdt_முனை_is_enabled(fdt, cpu_offset))
		திரும்பு;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	எனில் (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	எனில் (!emulated_zicntr)
		திரும்பு;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	எனில் (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

வெறுமை fdt_cpu_fixup(வெறுமை *fdt)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	முழு err, cpu_offset, cpus_offset;
	முழு reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	மாறிலி முழு status_size = அளவு(கட்டமைப்பு fdt_property) +
				ROUNDUP(அளவு("disabled"), FDT_TAGSIZE) +
				அளவு("status");
	மாறிலி முழு zicntr_size = ROUNDUP(அளவு("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != வெற்று &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		எனில் (disable)
			reserve += status_size;
		எனில் (add_zicntr)
			reserve += zicntr_size;
	}

	எனில் (!reserve)
		திரும்பு;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	எனில் (err < 0)
		திரும்பு;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		எனில் (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			எனில் (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, வெற்று),
					   err);
		}

		எனில் (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			எனில் (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, வெற்று),
					   err);
		}
	}
}

நிலையான வெறுமை fdt_domain_based_fixup_one(வெறுமை *fdt, முழு nodeoff)
{
	முழு rc;
	uint64_t reg_addr, reg_size;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_addr || !reg_size)
		திரும்பு;

	எனில் (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		எனில் (rc < 0)
			திரும்பு;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

நிலையான வெறுமை fdt_fixup_முனை(வெறுமை *fdt, மாறிலி எழுத்து *compatible)
{
	முழு noff = 0;

	போது ((noff = fdt_முனை_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

வெறுமை fdt_aplic_fixup(வெறுமை *fdt)
{
	fdt_fixup_முனை(fdt, "riscv,aplic");
}

வெறுமை fdt_imsic_fixup(வெறுமை *fdt)
{
	fdt_fixup_முனை(fdt, "riscv,imsics");
}

வெறுமை fdt_plic_fixup(வெறுமை *fdt)
{
	u32 *cells;
	முழு i, cells_மொத்தம்;
	முழு plic_off;

	plic_off = fdt_முனை_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	எனில் (plic_off < 0) {
		plic_off = fdt_முனை_offset_by_compatible(fdt, 0, "riscv,plic0");
		எனில் (plic_off < 0)
			திரும்பு;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_மொத்தம்);
	எனில் (!cells)
		திரும்பு;

	cells_மொத்தம் = cells_மொத்தம் / அளவு(u32);
	எனில் (!cells_மொத்தம்)
		திரும்பு;

	க்கு (i = 0; i < (cells_மொத்தம் / 2); i++) {
		எனில் (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

நிலையான முழு fdt_resv_memory_update_முனை(வெறுமை *fdt, unsigned long addr,
				       unsigned long size, முழு index,
				       முழு parent)
{
	முழு na = fdt_address_cells(fdt, 0);
	முழு ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_உயர்வு, addr_கீழ்;
	fdt32_t size_உயர்வு, size_கீழ்;
	முழு subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	எழுத்து name[32];

	addr_உயர்வு = (u64)addr >> 32;
	addr_கீழ் = addr;
	size_உயர்வு = (u64)size >> 32;
	size_கீழ் = size;

	எனில் (na > 1 && addr_உயர்வு)
		sbi_snprintf(name, அளவு(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_உயர்வு, addr_கீழ்);
	இல்லையெனில்
		sbi_snprintf(name, அளவு(name),
			     "mmode_resv%d@%x", index,
			     addr_கீழ்);

	subnode = fdt_add_subnode(fdt, parent, name);
	எனில் (subnode < 0)
		திரும்பு subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	எனில் (err < 0)
		திரும்பு err;

	/* encode the <reg> property value */
	val = reg;
	எனில் (na > 1)
		*val++ = cpu_to_fdt32(addr_உயர்வு);
	*val++ = cpu_to_fdt32(addr_கீழ்);
	எனில் (ns > 1)
		*val++ = cpu_to_fdt32(size_உயர்வு);
	*val++ = cpu_to_fdt32(size_கீழ்);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * அளவு(fdt32_t));
	எனில் (err < 0)
		திரும்பு err;

	திரும்பு 0;
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
முழு fdt_reserved_memory_fixup(வெறுமை *fdt)
{
	கட்டமைப்பு sbi_domain_memregion *reg;
	கட்டமைப்பு sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_மொத்தம்] = { 0 };
	unsigned எழுத்து filtered_order[PMP_மொத்தம்] = { 0 };
	unsigned long addr, size;
	முழு err, parent, i, j;
	முழு na = fdt_address_cells(fdt, 0);
	முழு ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	எனில் (err < 0)
		திரும்பு err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	எனில் (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		எனில் (parent < 0)
			திரும்பு parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		எனில் (err < 0)
			திரும்பு err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		எனில் (err < 0)
			திரும்பு err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		எனில் (err < 0)
			திரும்பு err;
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
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			தொடர்;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			தொடர்;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			தொடர்;
		எனில் (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			தொடர்;

		எனில் (i >= PMP_மொத்தம்) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			திரும்பு SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		க்கு (j = 0; j < i; j++) {
			எனில் (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				நிறுத்து;
			}
		}

		எனில் (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	க்கு (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_முனை(fdt, addr, size, j, parent);
	}

	திரும்பு 0;
}

வெறுமை fdt_config_fixup(வெறுமை *fdt)
{
	முழு chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	எனில் (chosen_offset < 0)
		திரும்பு;

	config_offset = fdt_முனை_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	எனில் (config_offset < 0)
		திரும்பு;

	fdt_nop_முனை(fdt, config_offset);
}

நிலையான SBI_LIST_HEAD(fixup_list);

முழு fdt_register_general_fixup(கட்டமைப்பு fdt_general_fixup *fixup)
{
	கட்டமைப்பு fdt_general_fixup *f;

	எனில் (!fixup || !fixup->name || !fixup->do_fixup)
		திரும்பு SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		எனில் (f == fixup)
			திரும்பு SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	திரும்பு 0;
}

வெறுமை fdt_unregister_general_fixup(கட்டமைப்பு fdt_general_fixup *fixup)
{
	எனில் (!fixup)
		திரும்பு;

	sbi_list_del(&fixup->head);
}

வெறுமை fdt_fixups(வெறுமை *fdt)
{
	கட்டமைப்பு fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_முனை
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
