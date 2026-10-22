
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

पूर्णाङ्क fdt_सङ्कलय_cpu_idle_अवस्थाः(शून्य *fdt, ध्रुव संरचना sbi_cpu_idle_state *state)
{
	पूर्णाङ्क cpu_ग्रन्थिः, cpus_ग्रन्थिः, err, idle_अवस्थाः_ग्रन्थिः;
	uint32_t गणनम्, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	यदि (err < 0)
		प्रत्यागम err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	यदि (err < 0)
		प्रत्यागम err;

	cpus_ग्रन्थिः = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_ग्रन्थिः < 0)
		प्रत्यागम cpus_ग्रन्थिः;

	/* Do nothing if the idle-states node already exists. */
	idle_अवस्थाः_ग्रन्थिः = fdt_subnode_विस्थापनम्(fdt, cpus_ग्रन्थिः, "idle-states");
	यदि (idle_अवस्थाः_ग्रन्थिः >= 0)
		प्रत्यागम 0;

	/* Create the idle-states node and its child nodes. */
	idle_अवस्थाः_ग्रन्थिः = fdt_सङ्कलय_subnode(fdt, cpus_ग्रन्थिः, "idle-states");
	यदि (idle_अवस्थाः_ग्रन्थिः < 0)
		प्रत्यागम idle_अवस्थाः_ग्रन्थिः;

	प्रति (गणनम् = 0; state->name; गणनम्++, phandle++, state++) {
		पूर्णाङ्क idle_state_ग्रन्थिः;

		idle_state_ग्रन्थिः = fdt_सङ्कलय_subnode(fdt, idle_अवस्थाः_ग्रन्थिः,
						  state->name);
		यदि (idle_state_ग्रन्थिः < 0)
			प्रत्यागम idle_state_ग्रन्थिः;

		fdt_setprop_string(fdt, idle_state_ग्रन्थिः, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		यदि (state->local_timer_stop)
			fdt_setprop_रिक्तानि(fdt, idle_state_ग्रन्थिः,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "entry-latency-us",
				state->प्रविष्टिः_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "min-residency-us",
				state->min_residency_us);
		यदि (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ग्रन्थिः,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ग्रन्थिः, "phandle", phandle);
	}

	यदि (गणनम् == 0)
		प्रत्यागम 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ग्रन्थिः, fdt, cpus_ग्रन्थिः) {
		ध्रुव वर्ण *device_type;
		fdt32_t *मानम्;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ग्रन्थिः, "device_type", रिक्त);
		यदि (!device_type || वाक्यं_तुलय(device_type, "cpu"))
			अनुवर्त;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ग्रन्थिः, "cpu-idle-states",
					      गणनम् * परिमाण(phandle),
					      (शून्य **)&मानम्);
		यदि (err < 0)
			प्रत्यागम err;

		/* Fill in the phandles of the idle state nodes. */
		प्रति (uint32_t i = 0; i < गणनम्; ++i)
			मानम्[i] = cpu_गन्तव्यम्_fdt32(phandle - गणनम् + i);
	}

	प्रत्यागम 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
स्थिर शून्य fdt_cpu_fixup_needed(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_विस्थापनम्,
				 bool emulated_zicntr, bool *disable,
				 bool *सङ्कलय_zicntr)
{
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	ध्रुव वर्ण *mmu_type, *extensions;
	u32 hartid, hartindex;
	पूर्णाङ्क len;

	*disable = false;
	*सङ्कलय_zicntr = false;

	यदि (fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid))
		प्रत्यागम;

	यदि (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_विस्थापनम्))
		प्रत्यागम;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_गन्तव्यम्_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_विस्थापनम्, "mmu-type", &len);
	यदि (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	यदि (!emulated_zicntr)
		प्रत्यागम;

	extensions = fdt_getprop(fdt, cpu_विस्थापनम्, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	यदि (extensions && !fdt_stringlist_अन्तर्भवति(extensions, len, "zicntr"))
		*सङ्कलय_zicntr = true;
}

शून्य fdt_cpu_fixup(शून्य *fdt)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, सङ्कलय_zicntr;
	पूर्णाङ्क err, cpu_विस्थापनम्, cpus_विस्थापनम्;
	पूर्णाङ्क reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ध्रुव पूर्णाङ्क स्थितिः_आकार = परिमाण(संरचना fdt_property) +
				ROUNDUP(परिमाण("disabled"), FDT_TAGSIZE) +
				परिमाण("status");
	ध्रुव पूर्णाङ्क zicntr_आकार = ROUNDUP(परिमाण("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != रिक्त &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_आवृत्तिः) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_विस्थापनम्, fdt, cpus_विस्थापनम्) {
		fdt_cpu_fixup_needed(fdt, cpu_विस्थापनम्, emulated_zicntr,
				     &disable, &सङ्कलय_zicntr);
		यदि (disable)
			reserve += स्थितिः_आकार;
		यदि (सङ्कलय_zicntr)
			reserve += zicntr_आकार;
	}

	यदि (!reserve)
		प्रत्यागम;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	यदि (err < 0)
		प्रत्यागम;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम;

	fdt_for_each_subnode(cpu_विस्थापनम्, fdt, cpus_विस्थापनम्) {
		fdt_cpu_fixup_needed(fdt, cpu_विस्थापनम्, emulated_zicntr,
				     &disable, &सङ्कलय_zicntr);

		यदि (disable) {
			err = fdt_setprop_string(fdt, cpu_विस्थापनम्, "status",
						 "disabled");
			यदि (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_विस्थापनम्, रिक्त),
					   err);
		}

		यदि (सङ्कलय_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_विस्थापनम्,
						    "riscv,isa-extensions",
						    "zicntr");
			यदि (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_विस्थापनम्, रिक्त),
					   err);
		}
	}
}

स्थिर शून्य fdt_domain_based_fixup_one(शून्य *fdt, पूर्णाङ्क nodeoff)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_आकार;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_addr || !reg_आकार)
		प्रत्यागम;

	यदि (!sbi_domain_check_addr(dom, reg_addr, dom->अग्र_बहुलकम्,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		यदि (rc < 0)
			प्रत्यागम;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

स्थिर शून्य fdt_fixup_ग्रन्थिः(शून्य *fdt, ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क noff = 0;

	यावत् ((noff = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, noff,
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

	plic_off = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	यदि (plic_off < 0) {
		plic_off = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, 0, "riscv,plic0");
		यदि (plic_off < 0)
			प्रत्यागम;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_गणनम्);
	यदि (!cells)
		प्रत्यागम;

	cells_गणनम् = cells_गणनम् / परिमाण(u32);
	यदि (!cells_गणनम्)
		प्रत्यागम;

	प्रति (i = 0; i < (cells_गणनम् / 2); i++) {
		यदि (fdt32_गन्तव्यम्_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_गन्तव्यम्_fdt32(0xffffffff);
	}
}

स्थिर पूर्णाङ्क fdt_resv_स्मृतिः_update_ग्रन्थिः(शून्य *fdt, अचिह्नित दीर्घ addr,
				       अचिह्नित दीर्घ आकार, पूर्णाङ्क सूचकांक_2,
				       पूर्णाङ्क parent)
{
	पूर्णाङ्क na = fdt_स्थानाङ्कः_cells(fdt, 0);
	पूर्णाङ्क ns = fdt_आकार_cells(fdt, 0);
	fdt32_t addr_उच्चः, addr_नीचः;
	fdt32_t आकार_उच्चः, आकार_नीचः;
	पूर्णाङ्क subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	वर्ण name[32];

	addr_उच्चः = (u64)addr >> 32;
	addr_नीचः = addr;
	आकार_उच्चः = (u64)आकार >> 32;
	आकार_नीचः = आकार;

	यदि (na > 1 && addr_उच्चः)
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x,%x", सूचकांक_2,
			     addr_उच्चः, addr_नीचः);
	अन्यथा
		sbi_snprintf(name, परिमाण(name),
			     "mmode_resv%d@%x", सूचकांक_2,
			     addr_नीचः);

	subnode = fdt_सङ्कलय_subnode(fdt, parent, name);
	यदि (subnode < 0)
		प्रत्यागम subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_रिक्तानि(fdt, subnode, "no-map");
	यदि (err < 0)
		प्रत्यागम err;

	/* encode the <reg> property value */
	val = reg;
	यदि (na > 1)
		*val++ = cpu_गन्तव्यम्_fdt32(addr_उच्चः);
	*val++ = cpu_गन्तव्यम्_fdt32(addr_नीचः);
	यदि (ns > 1)
		*val++ = cpu_गन्तव्यम्_fdt32(आकार_उच्चः);
	*val++ = cpu_गन्तव्यम्_fdt32(आकार_नीचः);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * परिमाण(fdt32_t));
	यदि (err < 0)
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
पूर्णाङ्क fdt_reserved_स्मृतिः_fixup(शून्य *fdt)
{
	संरचना sbi_domain_memregion *reg;
	संरचना sbi_domain *dom = sbi_domain_thishart_ptr();
	अचिह्नित दीर्घ filtered_आधारः[PMP_गणनम्] = { 0 };
	अचिह्नित वर्ण filtered_क्रमः[PMP_गणनम्] = { 0 };
	अचिह्नित दीर्घ addr, आकार;
	पूर्णाङ्क err, parent, i, j;
	पूर्णाङ्क na = fdt_स्थानाङ्कः_cells(fdt, 0);
	पूर्णाङ्क ns = fdt_आकार_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	यदि (err < 0)
		प्रत्यागम err;

	/* try to locate the reserved memory node */
	parent = fdt_मार्गः_विस्थापनम्(fdt, "/reserved-memory");
	यदि (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_सङ्कलय_subnode(fdt, 0, "reserved-memory");
		यदि (parent < 0)
			प्रत्यागम parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_रिक्तानि(fdt, parent, "ranges");
		यदि (err < 0)
			प्रत्यागम err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		यदि (err < 0)
			प्रत्यागम err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		यदि (err < 0)
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
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			अनुवर्त;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			अनुवर्त;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			अनुवर्त;
		यदि (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			अनुवर्त;

		यदि (i >= PMP_गणनम्) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			प्रत्यागम SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->आधारः_2;
		प्रति (j = 0; j < i; j++) {
			यदि (addr == filtered_आधारः[j]
			    && filtered_क्रमः[j] < reg->क्रमः) {
				overlap = true;
				filtered_क्रमः[j] = reg->क्रमः;
				भङ्ग;
			}
		}

		यदि (!overlap) {
			filtered_आधारः[i] = reg->आधारः_2;
			filtered_क्रमः[i] = reg->क्रमः;
			i++;
		}
	}

	प्रति (j = 0; j < i; j++) {
		addr = filtered_आधारः[j];
		आकार = 1UL << filtered_क्रमः[j];
		fdt_resv_स्मृतिः_update_ग्रन्थिः(fdt, addr, आकार, j, parent);
	}

	प्रत्यागम 0;
}

शून्य fdt_config_fixup(शून्य *fdt)
{
	पूर्णाङ्क chosen_विस्थापनम्, config_विस्थापनम्;

	chosen_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/chosen");
	यदि (chosen_विस्थापनम् < 0)
		प्रत्यागम;

	config_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, chosen_विस्थापनम्, "opensbi,config");
	यदि (config_विस्थापनम् < 0)
		प्रत्यागम;

	fdt_nop_ग्रन्थिः(fdt, config_विस्थापनम्);
}

स्थिर SBI_LIST_शीर्ष(fixup_list);

पूर्णाङ्क fdt_पञ्जिकाकोशः_general_fixup(संरचना fdt_general_fixup *fixup)
{
	संरचना fdt_general_fixup *f;

	यदि (!fixup || !fixup->name || !fixup->do_fixup)
		प्रत्यागम SBI_EINVAL;

	sbi_list_for_each_प्रविष्टिः(f, &fixup_list, शीर्ष) {
		यदि (f == fixup)
			प्रत्यागम SBI_EALREADY;
	}

	sbi_list_सङ्कलय_पुच्छ(&fixup->शीर्ष, &fixup_list);

	प्रत्यागम 0;
}

शून्य fdt_unregister_general_fixup(संरचना fdt_general_fixup *fixup)
{
	यदि (!fixup)
		प्रत्यागम;

	sbi_list_del(&fixup->शीर्ष);
}

शून्य fdt_fixups(शून्य *fdt)
{
	संरचना fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_स्मृतिः_fixup(fdt);

#यदि_अपरिभाषितम् CONFIG_FDT_FIXUPS_PRESERVE_PMU_ग्रन्थिः
	fdt_pmu_fixup(fdt);
#यदि_अन्तः

	fdt_config_fixup(fdt);

	sbi_list_for_each_प्रविष्टिः(f, &fixup_list, शीर्ष)
		f->do_fixup(f, fdt);
}
