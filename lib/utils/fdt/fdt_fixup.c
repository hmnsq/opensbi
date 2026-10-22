
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

صحيح fdt_جمع_کړه_cpu_idle_حالتونه(فراغ *fdt, ثابت بنية sbi_cpu_idle_state *state)
{
	صحيح cpu_غوټه, cpus_غوټه, err, idle_حالتونه_غوټه;
	uint32_t شمېر, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	إذا (err < 0)
		أرجع err;

	err = fdt_پیدا_کړه_max_phandle(fdt, &phandle);
	phandle++;
	إذا (err < 0)
		أرجع err;

	cpus_غوټه = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_غوټه < 0)
		أرجع cpus_غوټه;

	/* Do nothing if the idle-states node already exists. */
	idle_حالتونه_غوټه = fdt_subnode_دننه_واټن(fdt, cpus_غوټه, "idle-states");
	إذا (idle_حالتونه_غوټه >= 0)
		أرجع 0;

	/* Create the idle-states node and its child nodes. */
	idle_حالتونه_غوټه = fdt_جمع_کړه_subnode(fdt, cpus_غوټه, "idle-states");
	إذا (idle_حالتونه_غوټه < 0)
		أرجع idle_حالتونه_غوټه;

	لكل (شمېر = 0; state->name; شمېر++, phandle++, state++) {
		صحيح idle_state_غوټه;

		idle_state_غوټه = fdt_جمع_کړه_subnode(fdt, idle_حالتونه_غوټه,
						  state->name);
		إذا (idle_state_غوټه < 0)
			أرجع idle_state_غوټه;

		fdt_setprop_string(fdt, idle_state_غوټه, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_غوټه,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		إذا (state->local_timer_stop)
			fdt_setprop_تشه(fdt, idle_state_غوټه,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_غوټه, "entry-latency-us",
				state->ثبت_latency_us);
		fdt_setprop_u32(fdt, idle_state_غوټه, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_غوټه, "min-residency-us",
				state->min_residency_us);
		إذا (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_غوټه,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_غوټه, "phandle", phandle);
	}

	إذا (شمېر == 0)
		أرجع 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_غوټه, fdt, cpus_غوټه) {
		ثابت حرف *device_type;
		fdt32_t *ارزښت;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_غوټه, "device_type", عدم);
		إذا (!device_type || strcmp(device_type, "cpu"))
			استمر;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_غوټه, "cpu-idle-states",
					      شمېر * حجم(phandle),
					      (فراغ **)&ارزښت);
		إذا (err < 0)
			أرجع err;

		/* Fill in the phandles of the idle state nodes. */
		لكل (uint32_t i = 0; i < شمېر; ++i)
			ارزښت[i] = cpu_منزل_fdt32(phandle - شمېر + i);
	}

	أرجع 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساكن فراغ fdt_cpu_fixup_needed(ثابت فراغ *fdt, صحيح cpu_دننه_واټن,
				 bool emulated_zicntr, bool *disable,
				 bool *جمع_کړه_zicntr)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحيح len;

	*disable = false;
	*جمع_کړه_zicntr = false;

	إذا (fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid))
		أرجع;

	إذا (!fdt_غوټه_is_enabled(fdt, cpu_دننه_واټن))
		أرجع;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_منزل_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_دننه_واټن, "mmu-type", &len);
	إذا (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	إذا (!emulated_zicntr)
		أرجع;

	extensions = fdt_getprop(fdt, cpu_دننه_واټن, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	إذا (extensions && !fdt_stringlist_لري(extensions, len, "zicntr"))
		*جمع_کړه_zicntr = true;
}

فراغ fdt_cpu_fixup(فراغ *fdt)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, جمع_کړه_zicntr;
	صحيح err, cpu_دننه_واټن, cpus_دننه_واټن;
	صحيح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحيح حالت_اندازه = حجم(بنية fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحيح zicntr_اندازه = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != عدم &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_دوره) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_دننه_واټن, fdt, cpus_دننه_واټن) {
		fdt_cpu_fixup_needed(fdt, cpu_دننه_واټن, emulated_zicntr,
				     &disable, &جمع_کړه_zicntr);
		إذا (disable)
			reserve += حالت_اندازه;
		إذا (جمع_کړه_zicntr)
			reserve += zicntr_اندازه;
	}

	إذا (!reserve)
		أرجع;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	إذا (err < 0)
		أرجع;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع;

	fdt_for_each_subnode(cpu_دننه_واټن, fdt, cpus_دننه_واټن) {
		fdt_cpu_fixup_needed(fdt, cpu_دننه_واټن, emulated_zicntr,
				     &disable, &جمع_کړه_zicntr);

		إذا (disable) {
			err = fdt_setprop_string(fdt, cpu_دننه_واټن, "status",
						 "disabled");
			إذا (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_دننه_واټن, عدم),
					   err);
		}

		إذا (جمع_کړه_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_دننه_واټن,
						    "riscv,isa-extensions",
						    "zicntr");
			إذا (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_دننه_واټن, عدم),
					   err);
		}
	}
}

ساكن فراغ fdt_domain_based_fixup_one(فراغ *fdt, صحيح nodeoff)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_addr || !reg_اندازه)
		أرجع;

	إذا (!sbi_domain_check_addr(dom, reg_addr, dom->بل_ډېرپېښ,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		إذا (rc < 0)
			أرجع;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساكن فراغ fdt_fixup_غوټه(فراغ *fdt, ثابت حرف *compatible)
{
	صحيح noff = 0;

	طالما ((noff = fdt_غوټه_دننه_واټن_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

فراغ fdt_aplic_fixup(فراغ *fdt)
{
	fdt_fixup_غوټه(fdt, "riscv,aplic");
}

فراغ fdt_imsic_fixup(فراغ *fdt)
{
	fdt_fixup_غوټه(fdt, "riscv,imsics");
}

فراغ fdt_plic_fixup(فراغ *fdt)
{
	u32 *cells;
	صحيح i, cells_شمېر;
	صحيح plic_off;

	plic_off = fdt_غوټه_دننه_واټن_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	إذا (plic_off < 0) {
		plic_off = fdt_غوټه_دننه_واټن_by_compatible(fdt, 0, "riscv,plic0");
		إذا (plic_off < 0)
			أرجع;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_شمېر);
	إذا (!cells)
		أرجع;

	cells_شمېر = cells_شمېر / حجم(u32);
	إذا (!cells_شمېر)
		أرجع;

	لكل (i = 0; i < (cells_شمېر / 2); i++) {
		إذا (fdt32_منزل_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_منزل_fdt32(0xffffffff);
	}
}

ساكن صحيح fdt_resv_حافظه_update_غوټه(فراغ *fdt, بینښه اوږد addr,
				       بینښه اوږد اندازه, صحيح شاخص_2,
				       صحيح پلار)
{
	صحيح na = fdt_پته_cells(fdt, 0);
	صحيح ns = fdt_اندازه_cells(fdt, 0);
	fdt32_t addr_لوړ, addr_ټیټ;
	fdt32_t اندازه_لوړ, اندازه_ټیټ;
	صحيح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_لوړ = (u64)addr >> 32;
	addr_ټیټ = addr;
	اندازه_لوړ = (u64)اندازه >> 32;
	اندازه_ټیټ = اندازه;

	إذا (na > 1 && addr_لوړ)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", شاخص_2,
			     addr_لوړ, addr_ټیټ);
	وإلا
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", شاخص_2,
			     addr_ټیټ);

	subnode = fdt_جمع_کړه_subnode(fdt, پلار, name);
	إذا (subnode < 0)
		أرجع subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_تشه(fdt, subnode, "no-map");
	إذا (err < 0)
		أرجع err;

	/* encode the <reg> property value */
	val = reg;
	إذا (na > 1)
		*val++ = cpu_منزل_fdt32(addr_لوړ);
	*val++ = cpu_منزل_fdt32(addr_ټیټ);
	إذا (ns > 1)
		*val++ = cpu_منزل_fdt32(اندازه_لوړ);
	*val++ = cpu_منزل_fdt32(اندازه_ټیټ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * حجم(fdt32_t));
	إذا (err < 0)
		أرجع err;

	أرجع 0;
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
صحيح fdt_reserved_حافظه_fixup(فراغ *fdt)
{
	بنية sbi_domain_memregion *reg;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بینښه اوږد filtered_بنسټ[PMP_شمېر] = { 0 };
	بینښه حرف filtered_ترتیب[PMP_شمېر] = { 0 };
	بینښه اوږد addr, اندازه;
	صحيح err, پلار, i, j;
	صحيح na = fdt_پته_cells(fdt, 0);
	صحيح ns = fdt_اندازه_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	إذا (err < 0)
		أرجع err;

	/* try to locate the reserved memory node */
	پلار = fdt_لار_دننه_واټن(fdt, "/reserved-memory");
	إذا (پلار < 0) {
		/* if such node does not exist, create one */
		پلار = fdt_جمع_کړه_subnode(fdt, 0, "reserved-memory");
		إذا (پلار < 0)
			أرجع پلار;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_تشه(fdt, پلار, "ranges");
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, پلار, "#size-cells", ns);
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, پلار, "#address-cells", na);
		إذا (err < 0)
			أرجع err;
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
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			استمر;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			استمر;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			استمر;
		إذا (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			استمر;

		إذا (i >= PMP_شمېر) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			أرجع SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->بنسټ_2;
		لكل (j = 0; j < i; j++) {
			إذا (addr == filtered_بنسټ[j]
			    && filtered_ترتیب[j] < reg->ترتیب) {
				overlap = true;
				filtered_ترتیب[j] = reg->ترتیب;
				اكسر;
			}
		}

		إذا (!overlap) {
			filtered_بنسټ[i] = reg->بنسټ_2;
			filtered_ترتیب[i] = reg->ترتیب;
			i++;
		}
	}

	لكل (j = 0; j < i; j++) {
		addr = filtered_بنسټ[j];
		اندازه = 1UL << filtered_ترتیب[j];
		fdt_resv_حافظه_update_غوټه(fdt, addr, اندازه, j, پلار);
	}

	أرجع 0;
}

فراغ fdt_config_fixup(فراغ *fdt)
{
	صحيح chosen_دننه_واټن, config_دننه_واټن;

	chosen_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/chosen");
	إذا (chosen_دننه_واټن < 0)
		أرجع;

	config_دننه_واټن = fdt_غوټه_دننه_واټن_by_compatible(fdt, chosen_دننه_واټن, "opensbi,config");
	إذا (config_دننه_واټن < 0)
		أرجع;

	fdt_nop_غوټه(fdt, config_دننه_واټن);
}

ساكن SBI_LIST_سر(fixup_list);

صحيح fdt_ثبتوونکی_general_fixup(بنية fdt_general_fixup *fixup)
{
	بنية fdt_general_fixup *f;

	إذا (!fixup || !fixup->name || !fixup->do_fixup)
		أرجع SBI_EINVAL;

	sbi_list_for_each_ثبت(f, &fixup_list, سر) {
		إذا (f == fixup)
			أرجع SBI_EALREADY;
	}

	sbi_list_جمع_کړه_لکۍ(&fixup->سر, &fixup_list);

	أرجع 0;
}

فراغ fdt_unregister_general_fixup(بنية fdt_general_fixup *fixup)
{
	إذا (!fixup)
		أرجع;

	sbi_list_del(&fixup->سر);
}

فراغ fdt_fixups(فراغ *fdt)
{
	بنية fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_حافظه_fixup(fdt);

#که_نه_تعریف_شوی CONFIG_FDT_FIXUPS_PRESERVE_PMU_غوټه
	fdt_pmu_fixup(fdt);
#د_که_پای

	fdt_config_fixup(fdt);

	sbi_list_for_each_ثبت(f, &fixup_list, سر)
		f->do_fixup(f, fdt);
}
