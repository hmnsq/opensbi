
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_math.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/fdt/fdt_fixup.h>
#شامل <sbi_utils/fdt/fdt_pmu.h>
#شامل <sbi_utils/fdt/fdt_helper.h>

صحیح fdt_add_cpu_idle_states(تهی *fdt, ثابت ساختار sbi_cpu_idle_state *state)
{
	صحیح cpu_گره, cpus_گره, err, idle_states_گره;
	uint32_t شمار, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	اگر (err < 0)
		بازگشت err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	اگر (err < 0)
		بازگشت err;

	cpus_گره = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_گره < 0)
		بازگشت cpus_گره;

	/* Do nothing if the idle-states node already exists. */
	idle_states_گره = fdt_subnode_offset(fdt, cpus_گره, "idle-states");
	اگر (idle_states_گره >= 0)
		بازگشت 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_گره = fdt_add_subnode(fdt, cpus_گره, "idle-states");
	اگر (idle_states_گره < 0)
		بازگشت idle_states_گره;

	برای (شمار = 0; state->name; شمار++, phandle++, state++) {
		صحیح idle_state_گره;

		idle_state_گره = fdt_add_subnode(fdt, idle_states_گره,
						  state->name);
		اگر (idle_state_گره < 0)
			بازگشت idle_state_گره;

		fdt_setprop_string(fdt, idle_state_گره, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_گره,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		اگر (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_گره,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_گره, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "min-residency-us",
				state->min_residency_us);
		اگر (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_گره,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "phandle", phandle);
	}

	اگر (شمار == 0)
		بازگشت 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_گره, fdt, cpus_گره) {
		ثابت نویسه *device_type;
		fdt32_t *مقدار;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_گره, "device_type", پوچ);
		اگر (!device_type || strcmp(device_type, "cpu"))
			ادامه;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_گره, "cpu-idle-states",
					      شمار * اندازه(phandle),
					      (تهی **)&مقدار);
		اگر (err < 0)
			بازگشت err;

		/* Fill in the phandles of the idle state nodes. */
		برای (uint32_t i = 0; i < شمار; ++i)
			مقدار[i] = cpu_to_fdt32(phandle - شمار + i);
	}

	بازگشت 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ایستا تهی fdt_cpu_fixup_needed(ثابت تهی *fdt, صحیح cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت نویسه *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحیح len;

	*disable = false;
	*add_zicntr = false;

	اگر (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		بازگشت;

	اگر (!fdt_گره_is_enabled(fdt, cpu_offset))
		بازگشت;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	اگر (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	اگر (!emulated_zicntr)
		بازگشت;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	اگر (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

تهی fdt_cpu_fixup(تهی *fdt)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	صحیح err, cpu_offset, cpus_offset;
	صحیح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحیح status_size = اندازه(ساختار fdt_property) +
				ROUNDUP(اندازه("disabled"), FDT_TAGSIZE) +
				اندازه("status");
	ثابت صحیح zicntr_size = ROUNDUP(اندازه("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != پوچ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		اگر (disable)
			reserve += status_size;
		اگر (add_zicntr)
			reserve += zicntr_size;
	}

	اگر (!reserve)
		بازگشت;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	اگر (err < 0)
		بازگشت;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	اگر (cpus_offset < 0)
		بازگشت;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		اگر (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			اگر (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, پوچ),
					   err);
		}

		اگر (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			اگر (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, پوچ),
					   err);
		}
	}
}

ایستا تهی fdt_domain_based_fixup_one(تهی *fdt, صحیح nodeoff)
{
	صحیح rc;
	uint64_t reg_addr, reg_size;
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	اگر (rc < 0 || !reg_addr || !reg_size)
		بازگشت;

	اگر (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		اگر (rc < 0)
			بازگشت;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ایستا تهی fdt_fixup_گره(تهی *fdt, ثابت نویسه *compatible)
{
	صحیح noff = 0;

	وقتی ((noff = fdt_گره_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

تهی fdt_aplic_fixup(تهی *fdt)
{
	fdt_fixup_گره(fdt, "riscv,aplic");
}

تهی fdt_imsic_fixup(تهی *fdt)
{
	fdt_fixup_گره(fdt, "riscv,imsics");
}

تهی fdt_plic_fixup(تهی *fdt)
{
	u32 *cells;
	صحیح i, cells_شمار;
	صحیح plic_off;

	plic_off = fdt_گره_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	اگر (plic_off < 0) {
		plic_off = fdt_گره_offset_by_compatible(fdt, 0, "riscv,plic0");
		اگر (plic_off < 0)
			بازگشت;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_شمار);
	اگر (!cells)
		بازگشت;

	cells_شمار = cells_شمار / اندازه(u32);
	اگر (!cells_شمار)
		بازگشت;

	برای (i = 0; i < (cells_شمار / 2); i++) {
		اگر (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

ایستا صحیح fdt_resv_memory_update_گره(تهی *fdt, unsigned long addr,
				       unsigned long size, صحیح index,
				       صحیح parent)
{
	صحیح na = fdt_address_cells(fdt, 0);
	صحیح ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_بالا, addr_کم;
	fdt32_t size_بالا, size_کم;
	صحیح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	نویسه name[32];

	addr_بالا = (u64)addr >> 32;
	addr_کم = addr;
	size_بالا = (u64)size >> 32;
	size_کم = size;

	اگر (na > 1 && addr_بالا)
		sbi_snprintf(name, اندازه(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_بالا, addr_کم);
	وگرنه
		sbi_snprintf(name, اندازه(name),
			     "mmode_resv%d@%x", index,
			     addr_کم);

	subnode = fdt_add_subnode(fdt, parent, name);
	اگر (subnode < 0)
		بازگشت subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	اگر (err < 0)
		بازگشت err;

	/* encode the <reg> property value */
	val = reg;
	اگر (na > 1)
		*val++ = cpu_to_fdt32(addr_بالا);
	*val++ = cpu_to_fdt32(addr_کم);
	اگر (ns > 1)
		*val++ = cpu_to_fdt32(size_بالا);
	*val++ = cpu_to_fdt32(size_کم);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * اندازه(fdt32_t));
	اگر (err < 0)
		بازگشت err;

	بازگشت 0;
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
صحیح fdt_reserved_memory_fixup(تهی *fdt)
{
	ساختار sbi_domain_memregion *reg;
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_شمار] = { 0 };
	unsigned نویسه filtered_order[PMP_شمار] = { 0 };
	unsigned long addr, size;
	صحیح err, parent, i, j;
	صحیح na = fdt_address_cells(fdt, 0);
	صحیح ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	اگر (err < 0)
		بازگشت err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	اگر (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		اگر (parent < 0)
			بازگشت parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		اگر (err < 0)
			بازگشت err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		اگر (err < 0)
			بازگشت err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		اگر (err < 0)
			بازگشت err;
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
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ادامه;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ادامه;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ادامه;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ادامه;

		اگر (i >= PMP_شمار) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			بازگشت SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		برای (j = 0; j < i; j++) {
			اگر (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				بشکن;
			}
		}

		اگر (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	برای (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_گره(fdt, addr, size, j, parent);
	}

	بازگشت 0;
}

تهی fdt_config_fixup(تهی *fdt)
{
	صحیح chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	اگر (chosen_offset < 0)
		بازگشت;

	config_offset = fdt_گره_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	اگر (config_offset < 0)
		بازگشت;

	fdt_nop_گره(fdt, config_offset);
}

ایستا SBI_LIST_HEAD(fixup_list);

صحیح fdt_register_general_fixup(ساختار fdt_general_fixup *fixup)
{
	ساختار fdt_general_fixup *f;

	اگر (!fixup || !fixup->name || !fixup->do_fixup)
		بازگشت SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		اگر (f == fixup)
			بازگشت SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	بازگشت 0;
}

تهی fdt_unregister_general_fixup(ساختار fdt_general_fixup *fixup)
{
	اگر (!fixup)
		بازگشت;

	sbi_list_del(&fixup->head);
}

تهی fdt_fixups(تهی *fdt)
{
	ساختار fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_گره
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
