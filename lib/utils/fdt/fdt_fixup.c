
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

पूर्णाङ्क fdt_add_cpu_idle_states(शून्य *fdt, अचर संरचना sbi_cpu_idle_state *state)
{
	पूर्णाङ्क cpu_गाँठो, cpus_गाँठो, err, idle_states_गाँठो;
	uint32_t गन्ती, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	यदि (err < 0)
		फर्काऊ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	यदि (err < 0)
		फर्काऊ err;

	cpus_गाँठो = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_गाँठो < 0)
		फर्काऊ cpus_गाँठो;

	/* Do nothing if the idle-states node already exists. */
	idle_states_गाँठो = fdt_subnode_offset(fdt, cpus_गाँठो, "idle-states");
	यदि (idle_states_गाँठो >= 0)
		फर्काऊ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_गाँठो = fdt_add_subnode(fdt, cpus_गाँठो, "idle-states");
	यदि (idle_states_गाँठो < 0)
		फर्काऊ idle_states_गाँठो;

	लागि (गन्ती = 0; state->name; गन्ती++, phandle++, state++) {
		पूर्णाङ्क idle_state_गाँठो;

		idle_state_गाँठो = fdt_add_subnode(fdt, idle_states_गाँठो,
						  state->name);
		यदि (idle_state_गाँठो < 0)
			फर्काऊ idle_state_गाँठो;

		fdt_setprop_string(fdt, idle_state_गाँठो, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_गाँठो,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		यदि (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_गाँठो,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_गाँठो, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठो, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठो, "min-residency-us",
				state->min_residency_us);
		यदि (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_गाँठो,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठो, "phandle", phandle);
	}

	यदि (गन्ती == 0)
		फर्काऊ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_गाँठो, fdt, cpus_गाँठो) {
		अचर वर्ण *device_type;
		fdt32_t *मान;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_गाँठो, "device_type", रिक्त);
		यदि (!device_type || म_भेद(device_type, "cpu"))
			जारी;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_गाँठो, "cpu-idle-states",
					      गन्ती * परिमाण(phandle),
					      (शून्य **)&मान);
		यदि (err < 0)
			फर्काऊ err;

		/* Fill in the phandles of the idle state nodes. */
		लागि (uint32_t i = 0; i < गन्ती; ++i)
			मान[i] = cpu_to_fdt32(phandle - गन्ती + i);
	}

	फर्काऊ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
स्थिर शून्य fdt_cpu_fixup_needed(अचर शून्य *fdt, पूर्णाङ्क cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	अचर वर्ण *mmu_type, *extensions;
	u32 hartid, hartindex;
	पूर्णाङ्क len;

	*disable = false;
	*add_zicntr = false;

	यदि (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		फर्काऊ;

	यदि (!fdt_गाँठो_is_enabled(fdt, cpu_offset))
		फर्काऊ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	यदि (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	यदि (!emulated_zicntr)
		फर्काऊ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	यदि (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
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
	अचर पूर्णाङ्क status_size = परिमाण(संरचना fdt_property) +
				ROUNDUP(परिमाण("disabled"), FDT_TAGSIZE) +
				परिमाण("status");
	अचर पूर्णाङ्क zicntr_size = ROUNDUP(परिमाण("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != रिक्त &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_offset < 0)
		फर्काऊ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		यदि (disable)
			reserve += status_size;
		यदि (add_zicntr)
			reserve += zicntr_size;
	}

	यदि (!reserve)
		फर्काऊ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	यदि (err < 0)
		फर्काऊ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_offset < 0)
		फर्काऊ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		यदि (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			यदि (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, रिक्त),
					   err);
		}

		यदि (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			यदि (err)
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

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_addr || !reg_size)
		फर्काऊ;

	यदि (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		यदि (rc < 0)
			फर्काऊ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

स्थिर शून्य fdt_fixup_गाँठो(शून्य *fdt, अचर वर्ण *compatible)
{
	पूर्णाङ्क noff = 0;

	जबसम्म ((noff = fdt_गाँठो_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

शून्य fdt_aplic_fixup(शून्य *fdt)
{
	fdt_fixup_गाँठो(fdt, "riscv,aplic");
}

शून्य fdt_imsic_fixup(शून्य *fdt)
{
	fdt_fixup_गाँठो(fdt, "riscv,imsics");
}

शून्य fdt_plic_fixup(शून्य *fdt)
{
	u32 *cells;
	पूर्णाङ्क i, cells_गन्ती;
	पूर्णाङ्क plic_off;

	plic_off = fdt_गाँठो_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	यदि (plic_off < 0) {
		plic_off = fdt_गाँठो_offset_by_compatible(fdt, 0, "riscv,plic0");
		यदि (plic_off < 0)
			फर्काऊ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_गन्ती);
	यदि (!cells)
		फर्काऊ;

	cells_गन्ती = cells_गन्ती / परिमाण(u32);
	यदि (!cells_गन्ती)
		फर्काऊ;

	लागि (i = 0; i < (cells_गन्ती / 2); i++) {
		यदि (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

स्थिर पूर्णाङ्क fdt_resv_memory_update_गाँठो(शून्य *fdt, अचिन्हित दीर्घ addr,
				       अचिन्हित दीर्घ size, पूर्णाङ्क index,
				       पूर्णाङ्क parent)
{
	पूर्णाङ्क na = fdt_address_cells(fdt, 0);
	पूर्णाङ्क ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_माथिल्लो, addr_तल्लो;
	fdt32_t size_माथिल्लो, size_तल्लो;
	पूर्णाङ्क subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	वर्ण name[32];

	addr_माथिल्लो = (u64)addr >> 32;
	addr_तल्लो = addr;
	size_माथिल्लो = (u64)size >> 32;
	size_तल्लो = size;

	यदि (na > 1 && addr_माथिल्लो)
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_माथिल्लो, addr_तल्लो);
	अन्यथा
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x", index,
			     addr_तल्लो);

	subnode = fdt_add_subnode(fdt, parent, name);
	यदि (subnode < 0)
		फर्काऊ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	यदि (err < 0)
		फर्काऊ err;

	/* encode the <reg> property value */
	val = reg;
	यदि (na > 1)
		*val++ = cpu_to_fdt32(addr_माथिल्लो);
	*val++ = cpu_to_fdt32(addr_तल्लो);
	यदि (ns > 1)
		*val++ = cpu_to_fdt32(size_माथिल्लो);
	*val++ = cpu_to_fdt32(size_तल्लो);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * परिमाण(fdt32_t));
	यदि (err < 0)
		फर्काऊ err;

	फर्काऊ 0;
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
	अचिन्हित दीर्घ filtered_base[PMP_गन्ती] = { 0 };
	अचिन्हित वर्ण filtered_order[PMP_गन्ती] = { 0 };
	अचिन्हित दीर्घ addr, size;
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
	यदि (err < 0)
		फर्काऊ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	यदि (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		यदि (parent < 0)
			फर्काऊ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		यदि (err < 0)
			फर्काऊ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		यदि (err < 0)
			फर्काऊ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		यदि (err < 0)
			फर्काऊ err;
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
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			जारी;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			जारी;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			जारी;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			जारी;

		यदि (i >= PMP_गन्ती) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			फर्काऊ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		लागि (j = 0; j < i; j++) {
			यदि (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				रोक;
			}
		}

		यदि (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	लागि (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_गाँठो(fdt, addr, size, j, parent);
	}

	फर्काऊ 0;
}

शून्य fdt_config_fixup(शून्य *fdt)
{
	पूर्णाङ्क chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	यदि (chosen_offset < 0)
		फर्काऊ;

	config_offset = fdt_गाँठो_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	यदि (config_offset < 0)
		फर्काऊ;

	fdt_nop_गाँठो(fdt, config_offset);
}

स्थिर SBI_LIST_HEAD(fixup_list);

पूर्णाङ्क fdt_register_general_fixup(संरचना fdt_general_fixup *fixup)
{
	संरचना fdt_general_fixup *f;

	यदि (!fixup || !fixup->name || !fixup->do_fixup)
		फर्काऊ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		यदि (f == fixup)
			फर्काऊ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	फर्काऊ 0;
}

शून्य fdt_unregister_general_fixup(संरचना fdt_general_fixup *fixup)
{
	यदि (!fixup)
		फर्काऊ;

	sbi_list_del(&fixup->head);
}

शून्य fdt_fixups(शून्य *fdt)
{
	संरचना fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_गाँठो
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
