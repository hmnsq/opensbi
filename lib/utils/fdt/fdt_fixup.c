
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

पूर्णांक fdt_add_cpu_idle_states(शून्य *fdt, स्थिरांक संरचना sbi_cpu_idle_state *state)
{
	पूर्णांक cpu_गाठ, cpus_गाठ, err, idle_states_गाठ;
	uint32_t मोजणी, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	जर (err < 0)
		परत err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	जर (err < 0)
		परत err;

	cpus_गाठ = fdt_path_offset(fdt, "/cpus");
	जर (cpus_गाठ < 0)
		परत cpus_गाठ;

	/* Do nothing if the idle-states node already exists. */
	idle_states_गाठ = fdt_subnode_offset(fdt, cpus_गाठ, "idle-states");
	जर (idle_states_गाठ >= 0)
		परत 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_गाठ = fdt_add_subnode(fdt, cpus_गाठ, "idle-states");
	जर (idle_states_गाठ < 0)
		परत idle_states_गाठ;

	साठी (मोजणी = 0; state->name; मोजणी++, phandle++, state++) {
		पूर्णांक idle_state_गाठ;

		idle_state_गाठ = fdt_add_subnode(fdt, idle_states_गाठ,
						  state->name);
		जर (idle_state_गाठ < 0)
			परत idle_state_गाठ;

		fdt_setprop_string(fdt, idle_state_गाठ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_गाठ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		जर (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_गाठ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_गाठ, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाठ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाठ, "min-residency-us",
				state->min_residency_us);
		जर (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_गाठ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाठ, "phandle", phandle);
	}

	जर (मोजणी == 0)
		परत 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_गाठ, fdt, cpus_गाठ) {
		स्थिरांक अक्षर *device_type;
		fdt32_t *मूल्य;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_गाठ, "device_type", रिक्त);
		जर (!device_type || म_भेद(device_type, "cpu"))
			सुरूठेवा;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_गाठ, "cpu-idle-states",
					      मोजणी * आकार(phandle),
					      (शून्य **)&मूल्य);
		जर (err < 0)
			परत err;

		/* Fill in the phandles of the idle state nodes. */
		साठी (uint32_t i = 0; i < मोजणी; ++i)
			मूल्य[i] = cpu_to_fdt32(phandle - मोजणी + i);
	}

	परत 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
स्थिर शून्य fdt_cpu_fixup_needed(स्थिरांक शून्य *fdt, पूर्णांक cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	स्थिरांक अक्षर *mmu_type, *extensions;
	u32 hartid, hartindex;
	पूर्णांक len;

	*disable = false;
	*add_zicntr = false;

	जर (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		परत;

	जर (!fdt_गाठ_is_enabled(fdt, cpu_offset))
		परत;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	जर (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	जर (!emulated_zicntr)
		परत;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	जर (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

शून्य fdt_cpu_fixup(शून्य *fdt)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	पूर्णांक err, cpu_offset, cpus_offset;
	पूर्णांक reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	स्थिरांक पूर्णांक status_size = आकार(संरचना fdt_property) +
				ROUNDUP(आकार("disabled"), FDT_TAGSIZE) +
				आकार("status");
	स्थिरांक पूर्णांक zicntr_size = ROUNDUP(आकार("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != रिक्त &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		जर (disable)
			reserve += status_size;
		जर (add_zicntr)
			reserve += zicntr_size;
	}

	जर (!reserve)
		परत;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	जर (err < 0)
		परत;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		जर (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			जर (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, रिक्त),
					   err);
		}

		जर (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			जर (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, रिक्त),
					   err);
		}
	}
}

स्थिर शून्य fdt_domain_based_fixup_one(शून्य *fdt, पूर्णांक nodeoff)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_size;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	जर (rc < 0 || !reg_addr || !reg_size)
		परत;

	जर (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		जर (rc < 0)
			परत;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

स्थिर शून्य fdt_fixup_गाठ(शून्य *fdt, स्थिरांक अक्षर *compatible)
{
	पूर्णांक noff = 0;

	पर्यंत ((noff = fdt_गाठ_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

शून्य fdt_aplic_fixup(शून्य *fdt)
{
	fdt_fixup_गाठ(fdt, "riscv,aplic");
}

शून्य fdt_imsic_fixup(शून्य *fdt)
{
	fdt_fixup_गाठ(fdt, "riscv,imsics");
}

शून्य fdt_plic_fixup(शून्य *fdt)
{
	u32 *cells;
	पूर्णांक i, cells_मोजणी;
	पूर्णांक plic_off;

	plic_off = fdt_गाठ_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	जर (plic_off < 0) {
		plic_off = fdt_गाठ_offset_by_compatible(fdt, 0, "riscv,plic0");
		जर (plic_off < 0)
			परत;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_मोजणी);
	जर (!cells)
		परत;

	cells_मोजणी = cells_मोजणी / आकार(u32);
	जर (!cells_मोजणी)
		परत;

	साठी (i = 0; i < (cells_मोजणी / 2); i++) {
		जर (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

स्थिर पूर्णांक fdt_resv_memory_update_गाठ(शून्य *fdt, अचिन्हित दीर्घ addr,
				       अचिन्हित दीर्घ size, पूर्णांक index,
				       पूर्णांक parent)
{
	पूर्णांक na = fdt_address_cells(fdt, 0);
	पूर्णांक ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_वरचा, addr_खालचा;
	fdt32_t size_वरचा, size_खालचा;
	पूर्णांक subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	अक्षर name[32];

	addr_वरचा = (u64)addr >> 32;
	addr_खालचा = addr;
	size_वरचा = (u64)size >> 32;
	size_खालचा = size;

	जर (na > 1 && addr_वरचा)
		sbi_snprintf(name, आकार(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_वरचा, addr_खालचा);
	अन्यथा
		sbi_snprintf(name, आकार(name),
			     "mmode_resv%d@%x", index,
			     addr_खालचा);

	subnode = fdt_add_subnode(fdt, parent, name);
	जर (subnode < 0)
		परत subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	जर (err < 0)
		परत err;

	/* encode the <reg> property value */
	val = reg;
	जर (na > 1)
		*val++ = cpu_to_fdt32(addr_वरचा);
	*val++ = cpu_to_fdt32(addr_खालचा);
	जर (ns > 1)
		*val++ = cpu_to_fdt32(size_वरचा);
	*val++ = cpu_to_fdt32(size_खालचा);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * आकार(fdt32_t));
	जर (err < 0)
		परत err;

	परत 0;
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
पूर्णांक fdt_reserved_memory_fixup(शून्य *fdt)
{
	संरचना sbi_domain_memregion *reg;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	अचिन्हित दीर्घ filtered_base[PMP_मोजणी] = { 0 };
	अचिन्हित अक्षर filtered_order[PMP_मोजणी] = { 0 };
	अचिन्हित दीर्घ addr, size;
	पूर्णांक err, parent, i, j;
	पूर्णांक na = fdt_address_cells(fdt, 0);
	पूर्णांक ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	जर (err < 0)
		परत err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	जर (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		जर (parent < 0)
			परत parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		जर (err < 0)
			परत err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		जर (err < 0)
			परत err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		जर (err < 0)
			परत err;
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
		जर (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			सुरूठेवा;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			सुरूठेवा;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			सुरूठेवा;
		जर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			सुरूठेवा;

		जर (i >= PMP_मोजणी) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			परत SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		साठी (j = 0; j < i; j++) {
			जर (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				थांबा;
			}
		}

		जर (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	साठी (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_गाठ(fdt, addr, size, j, parent);
	}

	परत 0;
}

शून्य fdt_config_fixup(शून्य *fdt)
{
	पूर्णांक chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	जर (chosen_offset < 0)
		परत;

	config_offset = fdt_गाठ_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	जर (config_offset < 0)
		परत;

	fdt_nop_गाठ(fdt, config_offset);
}

स्थिर SBI_LIST_HEAD(fixup_list);

पूर्णांक fdt_register_general_fixup(संरचना fdt_general_fixup *fixup)
{
	संरचना fdt_general_fixup *f;

	जर (!fixup || !fixup->name || !fixup->do_fixup)
		परत SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		जर (f == fixup)
			परत SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	परत 0;
}

शून्य fdt_unregister_general_fixup(संरचना fdt_general_fixup *fixup)
{
	जर (!fixup)
		परत;

	sbi_list_del(&fixup->head);
}

शून्य fdt_fixups(शून्य *fdt)
{
	संरचना fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_गाठ
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
