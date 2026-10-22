
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_math.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_fixup.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_pmu.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>

പൂർണ്ണസംഖ്യ fdt_add_cpu_idle_states(ശൂന്യം *fdt, സ്ഥിരം ഘടന sbi_cpu_idle_state *state)
{
	പൂർണ്ണസംഖ്യ cpu_കണ്ണി, cpus_കണ്ണി, err, idle_states_കണ്ണി;
	uint32_t എണ്ണം, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	എങ്കിൽ (err < 0)
		തിരികെ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	എങ്കിൽ (err < 0)
		തിരികെ err;

	cpus_കണ്ണി = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_കണ്ണി < 0)
		തിരികെ cpus_കണ്ണി;

	/* Do nothing if the idle-states node already exists. */
	idle_states_കണ്ണി = fdt_subnode_offset(fdt, cpus_കണ്ണി, "idle-states");
	എങ്കിൽ (idle_states_കണ്ണി >= 0)
		തിരികെ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_കണ്ണി = fdt_add_subnode(fdt, cpus_കണ്ണി, "idle-states");
	എങ്കിൽ (idle_states_കണ്ണി < 0)
		തിരികെ idle_states_കണ്ണി;

	വേണ്ടി (എണ്ണം = 0; state->name; എണ്ണം++, phandle++, state++) {
		പൂർണ്ണസംഖ്യ idle_state_കണ്ണി;

		idle_state_കണ്ണി = fdt_add_subnode(fdt, idle_states_കണ്ണി,
						  state->name);
		എങ്കിൽ (idle_state_കണ്ണി < 0)
			തിരികെ idle_state_കണ്ണി;

		fdt_setprop_string(fdt, idle_state_കണ്ണി, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_കണ്ണി,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		എങ്കിൽ (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_കണ്ണി,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_കണ്ണി, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_കണ്ണി, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_കണ്ണി, "min-residency-us",
				state->min_residency_us);
		എങ്കിൽ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_കണ്ണി,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_കണ്ണി, "phandle", phandle);
	}

	എങ്കിൽ (എണ്ണം == 0)
		തിരികെ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_കണ്ണി, fdt, cpus_കണ്ണി) {
		സ്ഥിരം അക്ഷരം *device_type;
		fdt32_t *മൂല്യം;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_കണ്ണി, "device_type", ശൂന്യ);
		എങ്കിൽ (!device_type || strcmp(device_type, "cpu"))
			തുടരുക;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_കണ്ണി, "cpu-idle-states",
					      എണ്ണം * വലുപ്പം(phandle),
					      (ശൂന്യം **)&മൂല്യം);
		എങ്കിൽ (err < 0)
			തിരികെ err;

		/* Fill in the phandles of the idle state nodes. */
		വേണ്ടി (uint32_t i = 0; i < എണ്ണം; ++i)
			മൂല്യം[i] = cpu_to_fdt32(phandle - എണ്ണം + i);
	}

	തിരികെ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
സ്ഥിതം ശൂന്യം fdt_cpu_fixup_needed(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	സ്ഥിരം അക്ഷരം *mmu_type, *extensions;
	u32 hartid, hartindex;
	പൂർണ്ണസംഖ്യ len;

	*disable = false;
	*add_zicntr = false;

	എങ്കിൽ (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		തിരികെ;

	എങ്കിൽ (!fdt_കണ്ണി_is_enabled(fdt, cpu_offset))
		തിരികെ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	എങ്കിൽ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	എങ്കിൽ (!emulated_zicntr)
		തിരികെ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	എങ്കിൽ (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

ശൂന്യം fdt_cpu_fixup(ശൂന്യം *fdt)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	പൂർണ്ണസംഖ്യ err, cpu_offset, cpus_offset;
	പൂർണ്ണസംഖ്യ reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	സ്ഥിരം പൂർണ്ണസംഖ്യ status_size = വലുപ്പം(ഘടന fdt_property) +
				ROUNDUP(വലുപ്പം("disabled"), FDT_TAGSIZE) +
				വലുപ്പം("status");
	സ്ഥിരം പൂർണ്ണസംഖ്യ zicntr_size = ROUNDUP(വലുപ്പം("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ശൂന്യ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		എങ്കിൽ (disable)
			reserve += status_size;
		എങ്കിൽ (add_zicntr)
			reserve += zicntr_size;
	}

	എങ്കിൽ (!reserve)
		തിരികെ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	എങ്കിൽ (err < 0)
		തിരികെ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		എങ്കിൽ (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			എങ്കിൽ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ശൂന്യ),
					   err);
		}

		എങ്കിൽ (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			എങ്കിൽ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ശൂന്യ),
					   err);
		}
	}
}

സ്ഥിതം ശൂന്യം fdt_domain_based_fixup_one(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff)
{
	പൂർണ്ണസംഖ്യ rc;
	uint64_t reg_addr, reg_size;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_addr || !reg_size)
		തിരികെ;

	എങ്കിൽ (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		എങ്കിൽ (rc < 0)
			തിരികെ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

സ്ഥിതം ശൂന്യം fdt_fixup_കണ്ണി(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *compatible)
{
	പൂർണ്ണസംഖ്യ noff = 0;

	ആകുമ്പോൾ ((noff = fdt_കണ്ണി_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

ശൂന്യം fdt_aplic_fixup(ശൂന്യം *fdt)
{
	fdt_fixup_കണ്ണി(fdt, "riscv,aplic");
}

ശൂന്യം fdt_imsic_fixup(ശൂന്യം *fdt)
{
	fdt_fixup_കണ്ണി(fdt, "riscv,imsics");
}

ശൂന്യം fdt_plic_fixup(ശൂന്യം *fdt)
{
	u32 *cells;
	പൂർണ്ണസംഖ്യ i, cells_എണ്ണം;
	പൂർണ്ണസംഖ്യ plic_off;

	plic_off = fdt_കണ്ണി_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	എങ്കിൽ (plic_off < 0) {
		plic_off = fdt_കണ്ണി_offset_by_compatible(fdt, 0, "riscv,plic0");
		എങ്കിൽ (plic_off < 0)
			തിരികെ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_എണ്ണം);
	എങ്കിൽ (!cells)
		തിരികെ;

	cells_എണ്ണം = cells_എണ്ണം / വലുപ്പം(u32);
	എങ്കിൽ (!cells_എണ്ണം)
		തിരികെ;

	വേണ്ടി (i = 0; i < (cells_എണ്ണം / 2); i++) {
		എങ്കിൽ (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_resv_memory_update_കണ്ണി(ശൂന്യം *fdt, unsigned long addr,
				       unsigned long size, പൂർണ്ണസംഖ്യ index,
				       പൂർണ്ണസംഖ്യ parent)
{
	പൂർണ്ണസംഖ്യ na = fdt_address_cells(fdt, 0);
	പൂർണ്ണസംഖ്യ ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_മുകളിലെ, addr_താഴെയുള്ള;
	fdt32_t size_മുകളിലെ, size_താഴെയുള്ള;
	പൂർണ്ണസംഖ്യ subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	അക്ഷരം name[32];

	addr_മുകളിലെ = (u64)addr >> 32;
	addr_താഴെയുള്ള = addr;
	size_മുകളിലെ = (u64)size >> 32;
	size_താഴെയുള്ള = size;

	എങ്കിൽ (na > 1 && addr_മുകളിലെ)
		sbi_snprintf(name, വലുപ്പം(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_മുകളിലെ, addr_താഴെയുള്ള);
	അല്ലെങ്കിൽ
		sbi_snprintf(name, വലുപ്പം(name),
			     "mmode_resv%d@%x", index,
			     addr_താഴെയുള്ള);

	subnode = fdt_add_subnode(fdt, parent, name);
	എങ്കിൽ (subnode < 0)
		തിരികെ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	എങ്കിൽ (err < 0)
		തിരികെ err;

	/* encode the <reg> property value */
	val = reg;
	എങ്കിൽ (na > 1)
		*val++ = cpu_to_fdt32(addr_മുകളിലെ);
	*val++ = cpu_to_fdt32(addr_താഴെയുള്ള);
	എങ്കിൽ (ns > 1)
		*val++ = cpu_to_fdt32(size_മുകളിലെ);
	*val++ = cpu_to_fdt32(size_താഴെയുള്ള);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * വലുപ്പം(fdt32_t));
	എങ്കിൽ (err < 0)
		തിരികെ err;

	തിരികെ 0;
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
പൂർണ്ണസംഖ്യ fdt_reserved_memory_fixup(ശൂന്യം *fdt)
{
	ഘടന sbi_domain_memregion *reg;
	ഘടന sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_എണ്ണം] = { 0 };
	unsigned അക്ഷരം filtered_order[PMP_എണ്ണം] = { 0 };
	unsigned long addr, size;
	പൂർണ്ണസംഖ്യ err, parent, i, j;
	പൂർണ്ണസംഖ്യ na = fdt_address_cells(fdt, 0);
	പൂർണ്ണസംഖ്യ ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	എങ്കിൽ (err < 0)
		തിരികെ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	എങ്കിൽ (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		എങ്കിൽ (parent < 0)
			തിരികെ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		എങ്കിൽ (err < 0)
			തിരികെ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		എങ്കിൽ (err < 0)
			തിരികെ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		എങ്കിൽ (err < 0)
			തിരികെ err;
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
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			തുടരുക;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			തുടരുക;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			തുടരുക;
		എങ്കിൽ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			തുടരുക;

		എങ്കിൽ (i >= PMP_എണ്ണം) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			തിരികെ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		വേണ്ടി (j = 0; j < i; j++) {
			എങ്കിൽ (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				നിർത്തുക;
			}
		}

		എങ്കിൽ (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	വേണ്ടി (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_കണ്ണി(fdt, addr, size, j, parent);
	}

	തിരികെ 0;
}

ശൂന്യം fdt_config_fixup(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	എങ്കിൽ (chosen_offset < 0)
		തിരികെ;

	config_offset = fdt_കണ്ണി_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	എങ്കിൽ (config_offset < 0)
		തിരികെ;

	fdt_nop_കണ്ണി(fdt, config_offset);
}

സ്ഥിതം SBI_LIST_HEAD(fixup_list);

പൂർണ്ണസംഖ്യ fdt_register_general_fixup(ഘടന fdt_general_fixup *fixup)
{
	ഘടന fdt_general_fixup *f;

	എങ്കിൽ (!fixup || !fixup->name || !fixup->do_fixup)
		തിരികെ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		എങ്കിൽ (f == fixup)
			തിരികെ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	തിരികെ 0;
}

ശൂന്യം fdt_unregister_general_fixup(ഘടന fdt_general_fixup *fixup)
{
	എങ്കിൽ (!fixup)
		തിരികെ;

	sbi_list_del(&fixup->head);
}

ശൂന്യം fdt_fixups(ശൂന്യം *fdt)
{
	ഘടന fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_കണ്ണി
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
