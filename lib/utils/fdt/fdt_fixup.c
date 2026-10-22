
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_math.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/fdt/fdt_fixup.h>
#समावेश <sbi_utils/fdt/fdt_pmu.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>

पूर्णांक fdt_add_cpu_idle_states(व्योम *fdt, स्थिर काष्ठा sbi_cpu_idle_state *state)
{
	पूर्णांक cpu_गाँठ, cpus_गाँठ, err, idle_states_गाँठ;
	uint32_t गिनती, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	अगर (err < 0)
		वापस err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	अगर (err < 0)
		वापस err;

	cpus_गाँठ = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_गाँठ < 0)
		वापस cpus_गाँठ;

	/* Do nothing if the idle-states node already exists. */
	idle_states_गाँठ = fdt_subnode_offset(fdt, cpus_गाँठ, "idle-states");
	अगर (idle_states_गाँठ >= 0)
		वापस 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_गाँठ = fdt_add_subnode(fdt, cpus_गाँठ, "idle-states");
	अगर (idle_states_गाँठ < 0)
		वापस idle_states_गाँठ;

	क्रम (गिनती = 0; state->name; गिनती++, phandle++, state++) {
		पूर्णांक idle_state_गाँठ;

		idle_state_गाँठ = fdt_add_subnode(fdt, idle_states_गाँठ,
						  state->name);
		अगर (idle_state_गाँठ < 0)
			वापस idle_state_गाँठ;

		fdt_setprop_string(fdt, idle_state_गाँठ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_गाँठ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		अगर (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_गाँठ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_गाँठ, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठ, "min-residency-us",
				state->min_residency_us);
		अगर (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_गाँठ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_गाँठ, "phandle", phandle);
	}

	अगर (गिनती == 0)
		वापस 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_गाँठ, fdt, cpus_गाँठ) {
		स्थिर अक्षर *device_type;
		fdt32_t *मूल्य;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_गाँठ, "device_type", शून्य);
		अगर (!device_type || म_भेद(device_type, "cpu"))
			जारी;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_गाँठ, "cpu-idle-states",
					      गिनती * माप(phandle),
					      (व्योम **)&मूल्य);
		अगर (err < 0)
			वापस err;

		/* Fill in the phandles of the idle state nodes. */
		क्रम (uint32_t i = 0; i < गिनती; ++i)
			मूल्य[i] = cpu_to_fdt32(phandle - गिनती + i);
	}

	वापस 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
अटल व्योम fdt_cpu_fixup_needed(स्थिर व्योम *fdt, पूर्णांक cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	स्थिर अक्षर *mmu_type, *extensions;
	u32 hartid, hartindex;
	पूर्णांक len;

	*disable = false;
	*add_zicntr = false;

	अगर (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		वापस;

	अगर (!fdt_गाँठ_is_enabled(fdt, cpu_offset))
		वापस;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	अगर (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	अगर (!emulated_zicntr)
		वापस;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	अगर (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

व्योम fdt_cpu_fixup(व्योम *fdt)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	पूर्णांक err, cpu_offset, cpus_offset;
	पूर्णांक reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	स्थिर पूर्णांक status_आकार = माप(काष्ठा fdt_property) +
				ROUNDUP(माप("disabled"), FDT_TAGSIZE) +
				माप("status");
	स्थिर पूर्णांक zicntr_आकार = ROUNDUP(माप("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != शून्य &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		अगर (disable)
			reserve += status_आकार;
		अगर (add_zicntr)
			reserve += zicntr_आकार;
	}

	अगर (!reserve)
		वापस;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	अगर (err < 0)
		वापस;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		अगर (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			अगर (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, शून्य),
					   err);
		}

		अगर (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			अगर (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, शून्य),
					   err);
		}
	}
}

अटल व्योम fdt_domain_based_fixup_one(व्योम *fdt, पूर्णांक nodeoff)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_आकार;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_addr || !reg_आकार)
		वापस;

	अगर (!sbi_domain_check_addr(dom, reg_addr, dom->अग्र_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		अगर (rc < 0)
			वापस;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

अटल व्योम fdt_fixup_गाँठ(व्योम *fdt, स्थिर अक्षर *compatible)
{
	पूर्णांक noff = 0;

	जबतक ((noff = fdt_गाँठ_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

व्योम fdt_aplic_fixup(व्योम *fdt)
{
	fdt_fixup_गाँठ(fdt, "riscv,aplic");
}

व्योम fdt_imsic_fixup(व्योम *fdt)
{
	fdt_fixup_गाँठ(fdt, "riscv,imsics");
}

व्योम fdt_plic_fixup(व्योम *fdt)
{
	u32 *cells;
	पूर्णांक i, cells_गिनती;
	पूर्णांक plic_off;

	plic_off = fdt_गाँठ_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	अगर (plic_off < 0) {
		plic_off = fdt_गाँठ_offset_by_compatible(fdt, 0, "riscv,plic0");
		अगर (plic_off < 0)
			वापस;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_गिनती);
	अगर (!cells)
		वापस;

	cells_गिनती = cells_गिनती / माप(u32);
	अगर (!cells_गिनती)
		वापस;

	क्रम (i = 0; i < (cells_गिनती / 2); i++) {
		अगर (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

अटल पूर्णांक fdt_resv_memory_update_गाँठ(व्योम *fdt, अचिन्हित दीर्घ addr,
				       अचिन्हित दीर्घ आकार, पूर्णांक सूचकांक_2,
				       पूर्णांक parent)
{
	पूर्णांक na = fdt_address_cells(fdt, 0);
	पूर्णांक ns = fdt_आकार_cells(fdt, 0);
	fdt32_t addr_ऊपरी, addr_निचला;
	fdt32_t आकार_ऊपरी, आकार_निचला;
	पूर्णांक subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	अक्षर name[32];

	addr_ऊपरी = (u64)addr >> 32;
	addr_निचला = addr;
	आकार_ऊपरी = (u64)आकार >> 32;
	आकार_निचला = आकार;

	अगर (na > 1 && addr_ऊपरी)
		sbi_snprintf(name, माप(name),
			     "mmode_resv%d@%x,%x", सूचकांक_2,
			     addr_ऊपरी, addr_निचला);
	अन्यथा
		sbi_snprintf(name, माप(name),
			     "mmode_resv%d@%x", सूचकांक_2,
			     addr_निचला);

	subnode = fdt_add_subnode(fdt, parent, name);
	अगर (subnode < 0)
		वापस subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	अगर (err < 0)
		वापस err;

	/* encode the <reg> property value */
	val = reg;
	अगर (na > 1)
		*val++ = cpu_to_fdt32(addr_ऊपरी);
	*val++ = cpu_to_fdt32(addr_निचला);
	अगर (ns > 1)
		*val++ = cpu_to_fdt32(आकार_ऊपरी);
	*val++ = cpu_to_fdt32(आकार_निचला);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * माप(fdt32_t));
	अगर (err < 0)
		वापस err;

	वापस 0;
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
पूर्णांक fdt_reserved_memory_fixup(व्योम *fdt)
{
	काष्ठा sbi_domain_memregion *reg;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	अचिन्हित दीर्घ filtered_base[PMP_गिनती] = { 0 };
	अचिन्हित अक्षर filtered_order[PMP_गिनती] = { 0 };
	अचिन्हित दीर्घ addr, आकार;
	पूर्णांक err, parent, i, j;
	पूर्णांक na = fdt_address_cells(fdt, 0);
	पूर्णांक ns = fdt_आकार_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	अगर (err < 0)
		वापस err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	अगर (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		अगर (parent < 0)
			वापस parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		अगर (err < 0)
			वापस err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		अगर (err < 0)
			वापस err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		अगर (err < 0)
			वापस err;
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
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			जारी;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			जारी;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			जारी;
		अगर (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			जारी;

		अगर (i >= PMP_गिनती) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			वापस SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		क्रम (j = 0; j < i; j++) {
			अगर (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				अवरोध;
			}
		}

		अगर (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	क्रम (j = 0; j < i; j++) {
		addr = filtered_base[j];
		आकार = 1UL << filtered_order[j];
		fdt_resv_memory_update_गाँठ(fdt, addr, आकार, j, parent);
	}

	वापस 0;
}

व्योम fdt_config_fixup(व्योम *fdt)
{
	पूर्णांक chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	अगर (chosen_offset < 0)
		वापस;

	config_offset = fdt_गाँठ_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	अगर (config_offset < 0)
		वापस;

	fdt_nop_गाँठ(fdt, config_offset);
}

अटल SBI_LIST_शीर्ष(fixup_list);

पूर्णांक fdt_register_general_fixup(काष्ठा fdt_general_fixup *fixup)
{
	काष्ठा fdt_general_fixup *f;

	अगर (!fixup || !fixup->name || !fixup->do_fixup)
		वापस SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, शीर्ष) {
		अगर (f == fixup)
			वापस SBI_EALREADY;
	}

	sbi_list_add_पुच्छ(&fixup->शीर्ष, &fixup_list);

	वापस 0;
}

व्योम fdt_unregister_general_fixup(काष्ठा fdt_general_fixup *fixup)
{
	अगर (!fixup)
		वापस;

	sbi_list_del(&fixup->शीर्ष);
}

व्योम fdt_fixups(व्योम *fdt)
{
	काष्ठा fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#अगर_अघोषित CONFIG_FDT_FIXUPS_PRESERVE_PMU_गाँठ
	fdt_pmu_fixup(fdt);
#पूर्ण_अगर

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, शीर्ष)
		f->do_fixup(f, fdt);
}
