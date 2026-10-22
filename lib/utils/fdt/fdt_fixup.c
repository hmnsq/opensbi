
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

صحيح fdt_جمع_کن_cpu_idle_حالتها(فراغ *fdt, ثابت بنية sbi_cpu_idle_state *state)
{
	صحيح cpu_گره, cpus_گره, err, idle_حالتها_گره;
	uint32_t شمار, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	إذا (err < 0)
		أرجع err;

	err = fdt_پیدا_کن_max_phandle(fdt, &phandle);
	phandle++;
	إذا (err < 0)
		أرجع err;

	cpus_گره = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_گره < 0)
		أرجع cpus_گره;

	/* Do nothing if the idle-states node already exists. */
	idle_حالتها_گره = fdt_subnode_جابه_جایی(fdt, cpus_گره, "idle-states");
	إذا (idle_حالتها_گره >= 0)
		أرجع 0;

	/* Create the idle-states node and its child nodes. */
	idle_حالتها_گره = fdt_جمع_کن_subnode(fdt, cpus_گره, "idle-states");
	إذا (idle_حالتها_گره < 0)
		أرجع idle_حالتها_گره;

	لكل (شمار = 0; state->name; شمار++, phandle++, state++) {
		صحيح idle_state_گره;

		idle_state_گره = fdt_جمع_کن_subnode(fdt, idle_حالتها_گره,
						  state->name);
		إذا (idle_state_گره < 0)
			أرجع idle_state_گره;

		fdt_setprop_string(fdt, idle_state_گره, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_گره,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		إذا (state->local_timer_stop)
			fdt_setprop_خالی(fdt, idle_state_گره,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_گره, "entry-latency-us",
				state->مدخل_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "min-residency-us",
				state->min_residency_us);
		إذا (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_گره,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_گره, "phandle", phandle);
	}

	إذا (شمار == 0)
		أرجع 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_گره, fdt, cpus_گره) {
		ثابت حرف *device_type;
		fdt32_t *مقدار;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_گره, "device_type", عدم);
		إذا (!device_type || strcmp(device_type, "cpu"))
			استمر;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_گره, "cpu-idle-states",
					      شمار * حجم(phandle),
					      (فراغ **)&مقدار);
		إذا (err < 0)
			أرجع err;

		/* Fill in the phandles of the idle state nodes. */
		لكل (uint32_t i = 0; i < شمار; ++i)
			مقدار[i] = cpu_مقصد_fdt32(phandle - شمار + i);
	}

	أرجع 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساكن فراغ fdt_cpu_fixup_needed(ثابت فراغ *fdt, صحيح cpu_جابه_جایی,
				 bool emulated_zicntr, bool *disable,
				 bool *جمع_کن_zicntr)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحيح len;

	*disable = false;
	*جمع_کن_zicntr = false;

	إذا (fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid))
		أرجع;

	إذا (!fdt_گره_is_enabled(fdt, cpu_جابه_جایی))
		أرجع;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_مقصد_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_جابه_جایی, "mmu-type", &len);
	إذا (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	إذا (!emulated_zicntr)
		أرجع;

	extensions = fdt_getprop(fdt, cpu_جابه_جایی, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	إذا (extensions && !fdt_stringlist_شامل_است(extensions, len, "zicntr"))
		*جمع_کن_zicntr = true;
}

فراغ fdt_cpu_fixup(فراغ *fdt)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, جمع_کن_zicntr;
	صحيح err, cpu_جابه_جایی, cpus_جابه_جایی;
	صحيح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحيح وضعیت_اندازه = حجم(بنية fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحيح zicntr_اندازه = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != عدم &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_چرخه) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_جابه_جایی, fdt, cpus_جابه_جایی) {
		fdt_cpu_fixup_needed(fdt, cpu_جابه_جایی, emulated_zicntr,
				     &disable, &جمع_کن_zicntr);
		إذا (disable)
			reserve += وضعیت_اندازه;
		إذا (جمع_کن_zicntr)
			reserve += zicntr_اندازه;
	}

	إذا (!reserve)
		أرجع;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	إذا (err < 0)
		أرجع;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع;

	fdt_for_each_subnode(cpu_جابه_جایی, fdt, cpus_جابه_جایی) {
		fdt_cpu_fixup_needed(fdt, cpu_جابه_جایی, emulated_zicntr,
				     &disable, &جمع_کن_zicntr);

		إذا (disable) {
			err = fdt_setprop_string(fdt, cpu_جابه_جایی, "status",
						 "disabled");
			إذا (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_جابه_جایی, عدم),
					   err);
		}

		إذا (جمع_کن_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_جابه_جایی,
						    "riscv,isa-extensions",
						    "zicntr");
			إذا (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_جابه_جایی, عدم),
					   err);
		}
	}
}

ساكن فراغ fdt_domain_based_fixup_one(فراغ *fdt, صحيح nodeoff)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_addr || !reg_اندازه)
		أرجع;

	إذا (!sbi_domain_check_addr(dom, reg_addr, dom->بعدی_نما,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		إذا (rc < 0)
			أرجع;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساكن فراغ fdt_fixup_گره(فراغ *fdt, ثابت حرف *compatible)
{
	صحيح noff = 0;

	طالما ((noff = fdt_گره_جابه_جایی_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

فراغ fdt_aplic_fixup(فراغ *fdt)
{
	fdt_fixup_گره(fdt, "riscv,aplic");
}

فراغ fdt_imsic_fixup(فراغ *fdt)
{
	fdt_fixup_گره(fdt, "riscv,imsics");
}

فراغ fdt_plic_fixup(فراغ *fdt)
{
	u32 *cells;
	صحيح i, cells_شمار;
	صحيح plic_off;

	plic_off = fdt_گره_جابه_جایی_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	إذا (plic_off < 0) {
		plic_off = fdt_گره_جابه_جایی_by_compatible(fdt, 0, "riscv,plic0");
		إذا (plic_off < 0)
			أرجع;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_شمار);
	إذا (!cells)
		أرجع;

	cells_شمار = cells_شمار / حجم(u32);
	إذا (!cells_شمار)
		أرجع;

	لكل (i = 0; i < (cells_شمار / 2); i++) {
		إذا (fdt32_مقصد_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_مقصد_fdt32(0xffffffff);
	}
}

ساكن صحيح fdt_resv_حافظه_update_گره(فراغ *fdt, بیعلامت دراز addr,
				       بیعلامت دراز اندازه, صحيح شاخص_2,
				       صحيح والد)
{
	صحيح na = fdt_آدرس_cells(fdt, 0);
	صحيح ns = fdt_اندازه_cells(fdt, 0);
	fdt32_t addr_بالا, addr_کم;
	fdt32_t اندازه_بالا, اندازه_کم;
	صحيح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_بالا = (u64)addr >> 32;
	addr_کم = addr;
	اندازه_بالا = (u64)اندازه >> 32;
	اندازه_کم = اندازه;

	إذا (na > 1 && addr_بالا)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", شاخص_2,
			     addr_بالا, addr_کم);
	وإلا
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", شاخص_2,
			     addr_کم);

	subnode = fdt_جمع_کن_subnode(fdt, والد, name);
	إذا (subnode < 0)
		أرجع subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_خالی(fdt, subnode, "no-map");
	إذا (err < 0)
		أرجع err;

	/* encode the <reg> property value */
	val = reg;
	إذا (na > 1)
		*val++ = cpu_مقصد_fdt32(addr_بالا);
	*val++ = cpu_مقصد_fdt32(addr_کم);
	إذا (ns > 1)
		*val++ = cpu_مقصد_fdt32(اندازه_بالا);
	*val++ = cpu_مقصد_fdt32(اندازه_کم);

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
	بیعلامت دراز filtered_پایه[PMP_شمار] = { 0 };
	بیعلامت حرف filtered_ترتیب[PMP_شمار] = { 0 };
	بیعلامت دراز addr, اندازه;
	صحيح err, والد, i, j;
	صحيح na = fdt_آدرس_cells(fdt, 0);
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
	والد = fdt_مسیر_جابه_جایی(fdt, "/reserved-memory");
	إذا (والد < 0) {
		/* if such node does not exist, create one */
		والد = fdt_جمع_کن_subnode(fdt, 0, "reserved-memory");
		إذا (والد < 0)
			أرجع والد;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_خالی(fdt, والد, "ranges");
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, والد, "#size-cells", ns);
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, والد, "#address-cells", na);
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

		إذا (i >= PMP_شمار) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			أرجع SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->پایه_2;
		لكل (j = 0; j < i; j++) {
			إذا (addr == filtered_پایه[j]
			    && filtered_ترتیب[j] < reg->ترتیب) {
				overlap = true;
				filtered_ترتیب[j] = reg->ترتیب;
				اكسر;
			}
		}

		إذا (!overlap) {
			filtered_پایه[i] = reg->پایه_2;
			filtered_ترتیب[i] = reg->ترتیب;
			i++;
		}
	}

	لكل (j = 0; j < i; j++) {
		addr = filtered_پایه[j];
		اندازه = 1UL << filtered_ترتیب[j];
		fdt_resv_حافظه_update_گره(fdt, addr, اندازه, j, والد);
	}

	أرجع 0;
}

فراغ fdt_config_fixup(فراغ *fdt)
{
	صحيح chosen_جابه_جایی, config_جابه_جایی;

	chosen_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/chosen");
	إذا (chosen_جابه_جایی < 0)
		أرجع;

	config_جابه_جایی = fdt_گره_جابه_جایی_by_compatible(fdt, chosen_جابه_جایی, "opensbi,config");
	إذا (config_جابه_جایی < 0)
		أرجع;

	fdt_nop_گره(fdt, config_جابه_جایی);
}

ساكن SBI_LIST_سر(fixup_list);

صحيح fdt_ثبات_general_fixup(بنية fdt_general_fixup *fixup)
{
	بنية fdt_general_fixup *f;

	إذا (!fixup || !fixup->name || !fixup->do_fixup)
		أرجع SBI_EINVAL;

	sbi_list_for_each_مدخل(f, &fixup_list, سر) {
		إذا (f == fixup)
			أرجع SBI_EALREADY;
	}

	sbi_list_جمع_کن_دم(&fixup->سر, &fixup_list);

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

#اگر_تعریف_نشده CONFIG_FDT_FIXUPS_PRESERVE_PMU_گره
	fdt_pmu_fixup(fdt);
#پایان_اگر

	fdt_config_fixup(fdt);

	sbi_list_for_each_مدخل(f, &fixup_list, سر)
		f->do_fixup(f, fdt);
}
