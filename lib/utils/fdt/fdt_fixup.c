
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_math.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_fixup.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_pmu.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>

पूर्णाङ्क fdt_add_cpu_idle_states(शून्य *fdt, ध्रुव संरचना sbi_cpu_idle_state *state)
{
	पूर्णाङ्क cpu_ग्रन्थिः, cpus_ग्रन्थिः, err, idle_states_ग्रन्थिः;
	uint32_t गणनम्, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	जइ (err < 0)
		प्रत्यागम err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	जइ (err < 0)
		प्रत्यागम err;

	cpus_ग्रन्थिः = fdt_path_offset(fdt, "/cpus");
	जइ (cpus_ग्रन्थिः < 0)
		प्रत्यागम cpus_ग्रन्थिः;

	/* Do nothing if the idle-states node already exists. */
	idle_states_ग्रन्थिः = fdt_subnode_offset(fdt, cpus_ग्रन्थिः, "idle-states");
	जइ (idle_states_ग्रन्थिः >= 0)
		प्रत्यागम 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_ग्रन्थिः = fdt_add_subnode(fdt, cpus_ग्रन्थिः, "idle-states");
	जइ (idle_states_ग्रन्थिः < 0)
		प्रत्यागम idle_states_ग्रन्थिः;

	प्रति (गणनम् = 0; state->name; गणनम्++, phandle++, state++) {
		पूर्णाङ्क idle_state_ग्रन्थिः;

		idle_state_ग्रन्थिः = fdt_add_subnode(fdt, idle_states_ग्रन्थिः,
						  state->name);
		जइ (idle_state_ग्रन्थिः < 0)
			प्रत्यागम idle_state_ग्रन्थिः;

		fdt_setprop_string(fdt, idle_state_ग्रन्थिः, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		जइ (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_ग्रन्थिः,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "min-residency-us",
				state->min_residency_us);
		जइ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ग्रन्थिः,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "phandle", phandle);
	}

	जइ (गणनम् == 0)
		प्रत्यागम 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ग्रन्थिः, fdt, cpus_ग्रन्थिः) {
		ध्रुव वर्ण *device_type;
		fdt32_t *मानम्;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ग्रन्थिः, "device_type", रिक्त);
		जइ (!device_type || strcmp(device_type, "cpu"))
			अनुवर्त;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ग्रन्थिः, "cpu-idle-states",
					      गणनम् * परिमाण(phandle),
					      (शून्य **)&मानम्);
		जइ (err < 0)
			प्रत्यागम err;

		/* Fill in the phandles of the idle state nodes. */
		प्रति (uint32_t i = 0; i < गणनम्; ++i)
			मानम्[i] = cpu_to_fdt32(phandle - गणनम् + i);
	}

	प्रत्यागम 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
स्थिर शून्य fdt_cpu_fixup_needed(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	ध्रुव वर्ण *mmu_type, *extensions;
	u32 hartid, hartindex;
	पूर्णाङ्क len;

	*disable = false;
	*add_zicntr = false;

	जइ (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		प्रत्यागम;

	जइ (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_offset))
		प्रत्यागम;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	जइ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	जइ (!emulated_zicntr)
		प्रत्यागम;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	जइ (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

शून्य fdt_cpu_fixup(शून्य *fdt)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	पूर्णाङ्क err, cpu_offset, cpus_offset;
	पूर्णाङ्क reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ध्रुव पूर्णाङ्क status_size = परिमाण(संरचना fdt_property) +
				ROUNDUP(परिमाण("disabled"), FDT_TAGSIZE) +
				परिमाण("status");
	ध्रुव पूर्णाङ्क zicntr_size = ROUNDUP(परिमाण("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != रिक्त &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जइ (cpus_offset < 0)
		प्रत्यागम;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		जइ (disable)
			reserve += status_size;
		जइ (add_zicntr)
			reserve += zicntr_size;
	}

	जइ (!reserve)
		प्रत्यागम;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	जइ (err < 0)
		प्रत्यागम;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जइ (cpus_offset < 0)
		प्रत्यागम;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		जइ (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			जइ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, रिक्त),
					   err);
		}

		जइ (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			जइ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, रिक्त),
					   err);
		}
	}
}

स्थिर शून्य fdt_domain_based_fixup_one(शून्य *fdt, पूर्णाङ्क nodeoff)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_size;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	जइ (rc < 0 || !reg_addr || !reg_size)
		प्रत्यागम;

	जइ (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		जइ (rc < 0)
			प्रत्यागम;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

स्थिर शून्य fdt_fixup_ग्रन्थिः(शून्य *fdt, ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क noff = 0;

	जाव ((noff = fdt_ग्रन्थिः_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

शून्य fdt_aplic_fixup(शून्य *fdt)
{
	fdt_fixup_ग्रन्थिः(fdt, "riscv,aplic");
}

शून्य fdt_imsic_fixup(शून्य *fdt)
{
	fdt_fixup_ग्रन्थिः(fdt, "riscv,imsics");
}

शून्य fdt_plic_fixup(शून्य *fdt)
{
	u32 *cells;
	पूर्णाङ्क i, cells_गणनम्;
	पूर्णाङ्क plic_off;

	plic_off = fdt_ग्रन्थिः_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	जइ (plic_off < 0) {
		plic_off = fdt_ग्रन्थिः_offset_by_compatible(fdt, 0, "riscv,plic0");
		जइ (plic_off < 0)
			प्रत्यागम;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_गणनम्);
	जइ (!cells)
		प्रत्यागम;

	cells_गणनम् = cells_गणनम् / परिमाण(u32);
	जइ (!cells_गणनम्)
		प्रत्यागम;

	प्रति (i = 0; i < (cells_गणनम् / 2); i++) {
		जइ (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

स्थिर पूर्णाङ्क fdt_resv_memory_update_ग्रन्थिः(शून्य *fdt, unsigned long addr,
				       unsigned long size, पूर्णाङ्क index,
				       पूर्णाङ्क parent)
{
	पूर्णाङ्क na = fdt_address_cells(fdt, 0);
	पूर्णाङ्क ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_उच्चः, addr_नीचः;
	fdt32_t size_उच्चः, size_नीचः;
	पूर्णाङ्क subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	वर्ण name[32];

	addr_उच्चः = (u64)addr >> 32;
	addr_नीचः = addr;
	size_उच्चः = (u64)size >> 32;
	size_नीचः = size;

	जइ (na > 1 && addr_उच्चः)
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_उच्चः, addr_नीचः);
	अण्णहा
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x", index,
			     addr_नीचः);

	subnode = fdt_add_subnode(fdt, parent, name);
	जइ (subnode < 0)
		प्रत्यागम subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	जइ (err < 0)
		प्रत्यागम err;

	/* encode the <reg> property value */
	val = reg;
	जइ (na > 1)
		*val++ = cpu_to_fdt32(addr_उच्चः);
	*val++ = cpu_to_fdt32(addr_नीचः);
	जइ (ns > 1)
		*val++ = cpu_to_fdt32(size_उच्चः);
	*val++ = cpu_to_fdt32(size_नीचः);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * परिमाण(fdt32_t));
	जइ (err < 0)
		प्रत्यागम err;

	प्रत्यागम 0;
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
पूर्णाङ्क fdt_reserved_memory_fixup(शून्य *fdt)
{
	संरचना sbi_domain_memregion *reg;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_गणनम्] = { 0 };
	unsigned वर्ण filtered_order[PMP_गणनम्] = { 0 };
	unsigned long addr, size;
	पूर्णाङ्क err, parent, i, j;
	पूर्णाङ्क na = fdt_address_cells(fdt, 0);
	पूर्णाङ्क ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	जइ (err < 0)
		प्रत्यागम err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	जइ (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		जइ (parent < 0)
			प्रत्यागम parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		जइ (err < 0)
			प्रत्यागम err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		जइ (err < 0)
			प्रत्यागम err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		जइ (err < 0)
			प्रत्यागम err;
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
		जइ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			अनुवर्त;
		जइ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			अनुवर्त;
		जइ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			अनुवर्त;
		जइ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			अनुवर्त;

		जइ (i >= PMP_गणनम्) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			प्रत्यागम SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		प्रति (j = 0; j < i; j++) {
			जइ (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				भङ्ग;
			}
		}

		जइ (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	प्रति (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_ग्रन्थिः(fdt, addr, size, j, parent);
	}

	प्रत्यागम 0;
}

शून्य fdt_config_fixup(शून्य *fdt)
{
	पूर्णाङ्क chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	जइ (chosen_offset < 0)
		प्रत्यागम;

	config_offset = fdt_ग्रन्थिः_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	जइ (config_offset < 0)
		प्रत्यागम;

	fdt_nop_ग्रन्थिः(fdt, config_offset);
}

स्थिर SBI_LIST_HEAD(fixup_list);

पूर्णाङ्क fdt_register_general_fixup(संरचना fdt_general_fixup *fixup)
{
	संरचना fdt_general_fixup *f;

	जइ (!fixup || !fixup->name || !fixup->do_fixup)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		जइ (f == fixup)
			प्रत्यागम SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	प्रत्यागम 0;
}

शून्य fdt_unregister_general_fixup(संरचना fdt_general_fixup *fixup)
{
	जइ (!fixup)
		प्रत्यागम;

	sbi_list_del(&fixup->head);
}

शून्य fdt_fixups(शून्य *fdt)
{
	संरचना fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_ग्रन्थिः
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
