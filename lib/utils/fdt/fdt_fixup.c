
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_math.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/fdt/fdt_fixup.h>
#ضمن <sbi_utils/fdt/fdt_pmu.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>

صحيح fdt_add_cpu_idle_states(فراغ *fdt, ثابت بنية sbi_cpu_idle_state *state)
{
	صحيح cpu_node, cpus_node, err, idle_states_node;
	uint32_t count, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ܐܢ (err < 0)
		ܦܢܐ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	ܐܢ (err < 0)
		ܦܢܐ err;

	cpus_node = fdt_path_offset(fdt, "/cpus");
	ܐܢ (cpus_node < 0)
		ܦܢܐ cpus_node;

	/* Do nothing if the idle-states node already exists. */
	idle_states_node = fdt_subnode_offset(fdt, cpus_node, "idle-states");
	ܐܢ (idle_states_node >= 0)
		ܦܢܐ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_node = fdt_add_subnode(fdt, cpus_node, "idle-states");
	ܐܢ (idle_states_node < 0)
		ܦܢܐ idle_states_node;

	لكل (count = 0; state->name; count++, phandle++, state++) {
		صحيح idle_state_node;

		idle_state_node = fdt_add_subnode(fdt, idle_states_node,
						  state->name);
		ܐܢ (idle_state_node < 0)
			ܦܢܐ idle_state_node;

		fdt_setprop_string(fdt, idle_state_node, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_node,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ܐܢ (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_node,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_node, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "min-residency-us",
				state->min_residency_us);
		ܐܢ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_node,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "phandle", phandle);
	}

	ܐܢ (count == 0)
		ܦܢܐ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_node, fdt, cpus_node) {
		ثابت حرف *device_type;
		fdt32_t *value;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_node, "device_type", عدم);
		ܐܢ (!device_type || strcmp(device_type, "cpu"))
			استمر;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_node, "cpu-idle-states",
					      count * حجم(phandle),
					      (فراغ **)&value);
		ܐܢ (err < 0)
			ܦܢܐ err;

		/* Fill in the phandles of the idle state nodes. */
		لكل (uint32_t i = 0; i < count; ++i)
			value[i] = cpu_to_fdt32(phandle - count + i);
	}

	ܦܢܐ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساكن فراغ fdt_cpu_fixup_needed(ثابت فراغ *fdt, صحيح cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحيح len;

	*disable = false;
	*add_zicntr = false;

	ܐܢ (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		ܦܢܐ;

	ܐܢ (!fdt_node_is_enabled(fdt, cpu_offset))
		ܦܢܐ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	ܐܢ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ܐܢ (!emulated_zicntr)
		ܦܢܐ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ܐܢ (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

فراغ fdt_cpu_fixup(فراغ *fdt)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	صحيح err, cpu_offset, cpus_offset;
	صحيح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحيح status_size = حجم(بنية fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحيح zicntr_size = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != عدم &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ܐܢ (cpus_offset < 0)
		ܦܢܐ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		ܐܢ (disable)
			reserve += status_size;
		ܐܢ (add_zicntr)
			reserve += zicntr_size;
	}

	ܐܢ (!reserve)
		ܦܢܐ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ܐܢ (err < 0)
		ܦܢܐ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ܐܢ (cpus_offset < 0)
		ܦܢܐ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		ܐܢ (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			ܐܢ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, عدم),
					   err);
		}

		ܐܢ (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			ܐܢ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, عدم),
					   err);
		}
	}
}

ساكن فراغ fdt_domain_based_fixup_one(فراغ *fdt, صحيح nodeoff)
{
	صحيح rc;
	uint64_t reg_addr, reg_size;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ܐܢ (rc < 0 || !reg_addr || !reg_size)
		ܦܢܐ;

	ܐܢ (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ܐܢ (rc < 0)
			ܦܢܐ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساكن فراغ fdt_fixup_node(فراغ *fdt, ثابت حرف *compatible)
{
	صحيح noff = 0;

	ܟܡܐ ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

فراغ fdt_aplic_fixup(فراغ *fdt)
{
	fdt_fixup_node(fdt, "riscv,aplic");
}

فراغ fdt_imsic_fixup(فراغ *fdt)
{
	fdt_fixup_node(fdt, "riscv,imsics");
}

فراغ fdt_plic_fixup(فراغ *fdt)
{
	u32 *cells;
	صحيح i, cells_count;
	صحيح plic_off;

	plic_off = fdt_node_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ܐܢ (plic_off < 0) {
		plic_off = fdt_node_offset_by_compatible(fdt, 0, "riscv,plic0");
		ܐܢ (plic_off < 0)
			ܦܢܐ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_count);
	ܐܢ (!cells)
		ܦܢܐ;

	cells_count = cells_count / حجم(u32);
	ܐܢ (!cells_count)
		ܦܢܐ;

	لكل (i = 0; i < (cells_count / 2); i++) {
		ܐܢ (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

ساكن صحيح fdt_resv_memory_update_node(فراغ *fdt, unsigned long addr,
				       unsigned long size, صحيح index,
				       صحيح parent)
{
	صحيح na = fdt_address_cells(fdt, 0);
	صحيح ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_high, addr_low;
	fdt32_t size_high, size_low;
	صحيح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_high = (u64)addr >> 32;
	addr_low = addr;
	size_high = (u64)size >> 32;
	size_low = size;

	ܐܢ (na > 1 && addr_high)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_high, addr_low);
	ܐܚܪܢܐ
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", index,
			     addr_low);

	subnode = fdt_add_subnode(fdt, parent, name);
	ܐܢ (subnode < 0)
		ܦܢܐ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	ܐܢ (err < 0)
		ܦܢܐ err;

	/* encode the <reg> property value */
	val = reg;
	ܐܢ (na > 1)
		*val++ = cpu_to_fdt32(addr_high);
	*val++ = cpu_to_fdt32(addr_low);
	ܐܢ (ns > 1)
		*val++ = cpu_to_fdt32(size_high);
	*val++ = cpu_to_fdt32(size_low);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * حجم(fdt32_t));
	ܐܢ (err < 0)
		ܦܢܐ err;

	ܦܢܐ 0;
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
صحيح fdt_reserved_memory_fixup(فراغ *fdt)
{
	بنية sbi_domain_memregion *reg;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_COUNT] = { 0 };
	unsigned حرف filtered_order[PMP_COUNT] = { 0 };
	unsigned long addr, size;
	صحيح err, parent, i, j;
	صحيح na = fdt_address_cells(fdt, 0);
	صحيح ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ܐܢ (err < 0)
		ܦܢܐ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	ܐܢ (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		ܐܢ (parent < 0)
			ܦܢܐ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		ܐܢ (err < 0)
			ܦܢܐ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		ܐܢ (err < 0)
			ܦܢܐ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		ܐܢ (err < 0)
			ܦܢܐ err;
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
		ܐܢ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			استمر;
		ܐܢ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			استمر;
		ܐܢ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			استمر;
		ܐܢ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			استمر;

		ܐܢ (i >= PMP_COUNT) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ܦܢܐ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		لكل (j = 0; j < i; j++) {
			ܐܢ (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				اكسر;
			}
		}

		ܐܢ (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	لكل (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_node(fdt, addr, size, j, parent);
	}

	ܦܢܐ 0;
}

فراغ fdt_config_fixup(فراغ *fdt)
{
	صحيح chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	ܐܢ (chosen_offset < 0)
		ܦܢܐ;

	config_offset = fdt_node_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	ܐܢ (config_offset < 0)
		ܦܢܐ;

	fdt_nop_node(fdt, config_offset);
}

ساكن SBI_LIST_HEAD(fixup_list);

صحيح fdt_register_general_fixup(بنية fdt_general_fixup *fixup)
{
	بنية fdt_general_fixup *f;

	ܐܢ (!fixup || !fixup->name || !fixup->do_fixup)
		ܦܢܐ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		ܐܢ (f == fixup)
			ܦܢܐ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	ܦܢܐ 0;
}

فراغ fdt_unregister_general_fixup(بنية fdt_general_fixup *fixup)
{
	ܐܢ (!fixup)
		ܦܢܐ;

	sbi_list_del(&fixup->head);
}

فراغ fdt_fixups(فراغ *fdt)
{
	بنية fdt_general_fixup *f;

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
