
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_math.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_fixup.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_pmu.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>

పూర్ణాంకం fdt_కూడు_cpu_idle_స్థితులు(శూన్య *fdt, స్థిరరాశి నిర్మాణం sbi_cpu_idle_state *state)
{
	పూర్ణాంకం cpu_కణుపు, cpus_కణుపు, err, idle_స్థితులు_కణుపు;
	uint32_t లెక్క, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ఒకవేళ (err < 0)
		తిరిగి err;

	err = fdt_కనుగొను_max_phandle(fdt, &phandle);
	phandle++;
	ఒకవేళ (err < 0)
		తిరిగి err;

	cpus_కణుపు = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_కణుపు < 0)
		తిరిగి cpus_కణుపు;

	/* Do nothing if the idle-states node already exists. */
	idle_స్థితులు_కణుపు = fdt_subnode_స్థానభ్రంశం(fdt, cpus_కణుపు, "idle-states");
	ఒకవేళ (idle_స్థితులు_కణుపు >= 0)
		తిరిగి 0;

	/* Create the idle-states node and its child nodes. */
	idle_స్థితులు_కణుపు = fdt_కూడు_subnode(fdt, cpus_కణుపు, "idle-states");
	ఒకవేళ (idle_స్థితులు_కణుపు < 0)
		తిరిగి idle_స్థితులు_కణుపు;

	కొరకు (లెక్క = 0; state->name; లెక్క++, phandle++, state++) {
		పూర్ణాంకం idle_state_కణుపు;

		idle_state_కణుపు = fdt_కూడు_subnode(fdt, idle_స్థితులు_కణుపు,
						  state->name);
		ఒకవేళ (idle_state_కణుపు < 0)
			తిరిగి idle_state_కణుపు;

		fdt_setprop_string(fdt, idle_state_కణుపు, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_కణుపు,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ఒకవేళ (state->local_timer_stop)
			fdt_setprop_ఖాళీలు(fdt, idle_state_కణుపు,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_కణుపు, "entry-latency-us",
				state->నమోదు_latency_us);
		fdt_setprop_u32(fdt, idle_state_కణుపు, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_కణుపు, "min-residency-us",
				state->min_residency_us);
		ఒకవేళ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_కణుపు,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_కణుపు, "phandle", phandle);
	}

	ఒకవేళ (లెక్క == 0)
		తిరిగి 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_కణుపు, fdt, cpus_కణుపు) {
		స్థిరరాశి అక్షరం *device_type;
		fdt32_t *విలువ;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_కణుపు, "device_type", ఖాళీ);
		ఒకవేళ (!device_type || strcmp(device_type, "cpu"))
			కొనసాగు;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_కణుపు, "cpu-idle-states",
					      లెక్క * పరిమాణం(phandle),
					      (శూన్య **)&విలువ);
		ఒకవేళ (err < 0)
			తిరిగి err;

		/* Fill in the phandles of the idle state nodes. */
		కొరకు (uint32_t i = 0; i < లెక్క; ++i)
			విలువ[i] = cpu_గమ్యం_fdt32(phandle - లెక్క + i);
	}

	తిరిగి 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
స్థిర శూన్య fdt_cpu_fixup_needed(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం cpu_స్థానభ్రంశం,
				 bool emulated_zicntr, bool *disable,
				 bool *కూడు_zicntr)
{
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	స్థిరరాశి అక్షరం *mmu_type, *extensions;
	u32 hartid, hartindex;
	పూర్ణాంకం len;

	*disable = false;
	*కూడు_zicntr = false;

	ఒకవేళ (fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid))
		తిరిగి;

	ఒకవేళ (!fdt_కణుపు_is_enabled(fdt, cpu_స్థానభ్రంశం))
		తిరిగి;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_గమ్యం_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "mmu-type", &len);
	ఒకవేళ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ఒకవేళ (!emulated_zicntr)
		తిరిగి;

	extensions = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ఒకవేళ (extensions && !fdt_stringlist_కలిగియుంది(extensions, len, "zicntr"))
		*కూడు_zicntr = true;
}

శూన్య fdt_cpu_fixup(శూన్య *fdt)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, కూడు_zicntr;
	పూర్ణాంకం err, cpu_స్థానభ్రంశం, cpus_స్థానభ్రంశం;
	పూర్ణాంకం reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	స్థిరరాశి పూర్ణాంకం స్థితి_కొలత = పరిమాణం(నిర్మాణం fdt_property) +
				ROUNDUP(పరిమాణం("disabled"), FDT_TAGSIZE) +
				పరిమాణం("status");
	స్థిరరాశి పూర్ణాంకం zicntr_కొలత = ROUNDUP(పరిమాణం("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ఖాళీ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_చక్రం) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_స్థానభ్రంశం, fdt, cpus_స్థానభ్రంశం) {
		fdt_cpu_fixup_needed(fdt, cpu_స్థానభ్రంశం, emulated_zicntr,
				     &disable, &కూడు_zicntr);
		ఒకవేళ (disable)
			reserve += స్థితి_కొలత;
		ఒకవేళ (కూడు_zicntr)
			reserve += zicntr_కొలత;
	}

	ఒకవేళ (!reserve)
		తిరిగి;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ఒకవేళ (err < 0)
		తిరిగి;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి;

	fdt_for_each_subnode(cpu_స్థానభ్రంశం, fdt, cpus_స్థానభ్రంశం) {
		fdt_cpu_fixup_needed(fdt, cpu_స్థానభ్రంశం, emulated_zicntr,
				     &disable, &కూడు_zicntr);

		ఒకవేళ (disable) {
			err = fdt_setprop_string(fdt, cpu_స్థానభ్రంశం, "status",
						 "disabled");
			ఒకవేళ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_స్థానభ్రంశం, ఖాళీ),
					   err);
		}

		ఒకవేళ (కూడు_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_స్థానభ్రంశం,
						    "riscv,isa-extensions",
						    "zicntr");
			ఒకవేళ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_స్థానభ్రంశం, ఖాళీ),
					   err);
		}
	}
}

స్థిర శూన్య fdt_domain_based_fixup_one(శూన్య *fdt, పూర్ణాంకం nodeoff)
{
	పూర్ణాంకం rc;
	uint64_t reg_addr, reg_కొలత;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_addr || !reg_కొలత)
		తిరిగి;

	ఒకవేళ (!sbi_domain_check_addr(dom, reg_addr, dom->తదుపరి_బహుళకం,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ఒకవేళ (rc < 0)
			తిరిగి;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

స్థిర శూన్య fdt_fixup_కణుపు(శూన్య *fdt, స్థిరరాశి అక్షరం *compatible)
{
	పూర్ణాంకం noff = 0;

	అయితే ((noff = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

శూన్య fdt_aplic_fixup(శూన్య *fdt)
{
	fdt_fixup_కణుపు(fdt, "riscv,aplic");
}

శూన్య fdt_imsic_fixup(శూన్య *fdt)
{
	fdt_fixup_కణుపు(fdt, "riscv,imsics");
}

శూన్య fdt_plic_fixup(శూన్య *fdt)
{
	u32 *cells;
	పూర్ణాంకం i, cells_లెక్క;
	పూర్ణాంకం plic_off;

	plic_off = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ఒకవేళ (plic_off < 0) {
		plic_off = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, 0, "riscv,plic0");
		ఒకవేళ (plic_off < 0)
			తిరిగి;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_లెక్క);
	ఒకవేళ (!cells)
		తిరిగి;

	cells_లెక్క = cells_లెక్క / పరిమాణం(u32);
	ఒకవేళ (!cells_లెక్క)
		తిరిగి;

	కొరకు (i = 0; i < (cells_లెక్క / 2); i++) {
		ఒకవేళ (fdt32_గమ్యం_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_గమ్యం_fdt32(0xffffffff);
	}
}

స్థిర పూర్ణాంకం fdt_resv_జ్ఞాపకం_update_కణుపు(శూన్య *fdt, గుర్తులేని దీర్ఘ addr,
				       గుర్తులేని దీర్ఘ కొలత, పూర్ణాంకం సూచీ_2,
				       పూర్ణాంకం మాతృ)
{
	పూర్ణాంకం na = fdt_చిరునామా_cells(fdt, 0);
	పూర్ణాంకం ns = fdt_కొలత_cells(fdt, 0);
	fdt32_t addr_ఎగువ, addr_దిగువ;
	fdt32_t కొలత_ఎగువ, కొలత_దిగువ;
	పూర్ణాంకం subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	అక్షరం name[32];

	addr_ఎగువ = (u64)addr >> 32;
	addr_దిగువ = addr;
	కొలత_ఎగువ = (u64)కొలత >> 32;
	కొలత_దిగువ = కొలత;

	ఒకవేళ (na > 1 && addr_ఎగువ)
		sbi_snprintf(name, పరిమాణం(name),
			     "mmode_resv%d@%x,%x", సూచీ_2,
			     addr_ఎగువ, addr_దిగువ);
	లేకపోతే
		sbi_snprintf(name, పరిమాణం(name),
			     "mmode_resv%d@%x", సూచీ_2,
			     addr_దిగువ);

	subnode = fdt_కూడు_subnode(fdt, మాతృ, name);
	ఒకవేళ (subnode < 0)
		తిరిగి subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_ఖాళీలు(fdt, subnode, "no-map");
	ఒకవేళ (err < 0)
		తిరిగి err;

	/* encode the <reg> property value */
	val = reg;
	ఒకవేళ (na > 1)
		*val++ = cpu_గమ్యం_fdt32(addr_ఎగువ);
	*val++ = cpu_గమ్యం_fdt32(addr_దిగువ);
	ఒకవేళ (ns > 1)
		*val++ = cpu_గమ్యం_fdt32(కొలత_ఎగువ);
	*val++ = cpu_గమ్యం_fdt32(కొలత_దిగువ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * పరిమాణం(fdt32_t));
	ఒకవేళ (err < 0)
		తిరిగి err;

	తిరిగి 0;
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
పూర్ణాంకం fdt_reserved_జ్ఞాపకం_fixup(శూన్య *fdt)
{
	నిర్మాణం sbi_domain_memregion *reg;
	నిర్మాణం sbi_domain *dom = sbi_domain_thishart_ptr();
	గుర్తులేని దీర్ఘ filtered_పునాది[PMP_లెక్క] = { 0 };
	గుర్తులేని అక్షరం filtered_క్రమం[PMP_లెక్క] = { 0 };
	గుర్తులేని దీర్ఘ addr, కొలత;
	పూర్ణాంకం err, మాతృ, i, j;
	పూర్ణాంకం na = fdt_చిరునామా_cells(fdt, 0);
	పూర్ణాంకం ns = fdt_కొలత_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ఒకవేళ (err < 0)
		తిరిగి err;

	/* try to locate the reserved memory node */
	మాతృ = fdt_మార్గం_స్థానభ్రంశం(fdt, "/reserved-memory");
	ఒకవేళ (మాతృ < 0) {
		/* if such node does not exist, create one */
		మాతృ = fdt_కూడు_subnode(fdt, 0, "reserved-memory");
		ఒకవేళ (మాతృ < 0)
			తిరిగి మాతృ;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_ఖాళీలు(fdt, మాతృ, "ranges");
		ఒకవేళ (err < 0)
			తిరిగి err;

		err = fdt_setprop_u32(fdt, మాతృ, "#size-cells", ns);
		ఒకవేళ (err < 0)
			తిరిగి err;

		err = fdt_setprop_u32(fdt, మాతృ, "#address-cells", na);
		ఒకవేళ (err < 0)
			తిరిగి err;
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
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			కొనసాగు;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			కొనసాగు;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			కొనసాగు;
		ఒకవేళ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			కొనసాగు;

		ఒకవేళ (i >= PMP_లెక్క) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			తిరిగి SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->పునాది_2;
		కొరకు (j = 0; j < i; j++) {
			ఒకవేళ (addr == filtered_పునాది[j]
			    && filtered_క్రమం[j] < reg->క్రమం) {
				overlap = true;
				filtered_క్రమం[j] = reg->క్రమం;
				ఆపు;
			}
		}

		ఒకవేళ (!overlap) {
			filtered_పునాది[i] = reg->పునాది_2;
			filtered_క్రమం[i] = reg->క్రమం;
			i++;
		}
	}

	కొరకు (j = 0; j < i; j++) {
		addr = filtered_పునాది[j];
		కొలత = 1UL << filtered_క్రమం[j];
		fdt_resv_జ్ఞాపకం_update_కణుపు(fdt, addr, కొలత, j, మాతృ);
	}

	తిరిగి 0;
}

శూన్య fdt_config_fixup(శూన్య *fdt)
{
	పూర్ణాంకం chosen_స్థానభ్రంశం, config_స్థానభ్రంశం;

	chosen_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/chosen");
	ఒకవేళ (chosen_స్థానభ్రంశం < 0)
		తిరిగి;

	config_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, chosen_స్థానభ్రంశం, "opensbi,config");
	ఒకవేళ (config_స్థానభ్రంశం < 0)
		తిరిగి;

	fdt_nop_కణుపు(fdt, config_స్థానభ్రంశం);
}

స్థిర SBI_LIST_తల(fixup_list);

పూర్ణాంకం fdt_రిజిస్టరు_general_fixup(నిర్మాణం fdt_general_fixup *fixup)
{
	నిర్మాణం fdt_general_fixup *f;

	ఒకవేళ (!fixup || !fixup->name || !fixup->do_fixup)
		తిరిగి SBI_EINVAL;

	sbi_list_for_each_నమోదు(f, &fixup_list, తల) {
		ఒకవేళ (f == fixup)
			తిరిగి SBI_EALREADY;
	}

	sbi_list_కూడు_తోక(&fixup->తల, &fixup_list);

	తిరిగి 0;
}

శూన్య fdt_unregister_general_fixup(నిర్మాణం fdt_general_fixup *fixup)
{
	ఒకవేళ (!fixup)
		తిరిగి;

	sbi_list_del(&fixup->తల);
}

శూన్య fdt_fixups(శూన్య *fdt)
{
	నిర్మాణం fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_జ్ఞాపకం_fixup(fdt);

#నిర్వచితం_కాకపోతే CONFIG_FDT_FIXUPS_PRESERVE_PMU_కణుపు
	fdt_pmu_fixup(fdt);
#అయితే_ముగింపు

	fdt_config_fixup(fdt);

	sbi_list_for_each_నమోదు(f, &fixup_list, తల)
		f->do_fixup(f, fdt);
}
