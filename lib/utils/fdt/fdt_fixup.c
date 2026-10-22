
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

صحیح fdt_جوڑو_cpu_idle_حالتیں(خالی *fdt, ثابت ساخت sbi_cpu_idle_state *state)
{
	صحیح cpu_گرہ, cpus_گرہ, err, idle_حالتیں_گرہ;
	uint32_t گنتی, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	اگر (err < 0)
		واپس err;

	err = fdt_ڈھونڈو_max_phandle(fdt, &phandle);
	phandle++;
	اگر (err < 0)
		واپس err;

	cpus_گرہ = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_گرہ < 0)
		واپس cpus_گرہ;

	/* Do nothing if the idle-states node already exists. */
	idle_حالتیں_گرہ = fdt_subnode_انحراف(fdt, cpus_گرہ, "idle-states");
	اگر (idle_حالتیں_گرہ >= 0)
		واپس 0;

	/* Create the idle-states node and its child nodes. */
	idle_حالتیں_گرہ = fdt_جوڑو_subnode(fdt, cpus_گرہ, "idle-states");
	اگر (idle_حالتیں_گرہ < 0)
		واپس idle_حالتیں_گرہ;

	برائے (گنتی = 0; state->name; گنتی++, phandle++, state++) {
		صحیح idle_state_گرہ;

		idle_state_گرہ = fdt_جوڑو_subnode(fdt, idle_حالتیں_گرہ,
						  state->name);
		اگر (idle_state_گرہ < 0)
			واپس idle_state_گرہ;

		fdt_setprop_string(fdt, idle_state_گرہ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_گرہ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		اگر (state->local_timer_stop)
			fdt_setprop_خالی_خانے(fdt, idle_state_گرہ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_گرہ, "entry-latency-us",
				state->اندراج_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "min-residency-us",
				state->min_residency_us);
		اگر (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_گرہ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "phandle", phandle);
	}

	اگر (گنتی == 0)
		واپس 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_گرہ, fdt, cpus_گرہ) {
		ثابت حرف *device_type;
		fdt32_t *قدر;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_گرہ, "device_type", تہی);
		اگر (!device_type || strcmp(device_type, "cpu"))
			جاری;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_گرہ, "cpu-idle-states",
					      گنتی * حجم(phandle),
					      (خالی **)&قدر);
		اگر (err < 0)
			واپس err;

		/* Fill in the phandles of the idle state nodes. */
		برائے (uint32_t i = 0; i < گنتی; ++i)
			قدر[i] = cpu_مقصد_fdt32(phandle - گنتی + i);
	}

	واپس 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساکن خالی fdt_cpu_fixup_needed(ثابت خالی *fdt, صحیح cpu_انحراف,
				 bool emulated_zicntr, bool *disable,
				 bool *جوڑو_zicntr)
{
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحیح len;

	*disable = false;
	*جوڑو_zicntr = false;

	اگر (fdt_parse_hart_id(fdt, cpu_انحراف, &hartid))
		واپس;

	اگر (!fdt_گرہ_is_enabled(fdt, cpu_انحراف))
		واپس;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_مقصد_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_انحراف, "mmu-type", &len);
	اگر (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	اگر (!emulated_zicntr)
		واپس;

	extensions = fdt_getprop(fdt, cpu_انحراف, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	اگر (extensions && !fdt_stringlist_شامل_ہے(extensions, len, "zicntr"))
		*جوڑو_zicntr = true;
}

خالی fdt_cpu_fixup(خالی *fdt)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, جوڑو_zicntr;
	صحیح err, cpu_انحراف, cpus_انحراف;
	صحیح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحیح حالت_سائز = حجم(ساخت fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحیح zicntr_سائز = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != تہی &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_چکر) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_انحراف, fdt, cpus_انحراف) {
		fdt_cpu_fixup_needed(fdt, cpu_انحراف, emulated_zicntr,
				     &disable, &جوڑو_zicntr);
		اگر (disable)
			reserve += حالت_سائز;
		اگر (جوڑو_zicntr)
			reserve += zicntr_سائز;
	}

	اگر (!reserve)
		واپس;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	اگر (err < 0)
		واپس;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس;

	fdt_for_each_subnode(cpu_انحراف, fdt, cpus_انحراف) {
		fdt_cpu_fixup_needed(fdt, cpu_انحراف, emulated_zicntr,
				     &disable, &جوڑو_zicntr);

		اگر (disable) {
			err = fdt_setprop_string(fdt, cpu_انحراف, "status",
						 "disabled");
			اگر (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_انحراف, تہی),
					   err);
		}

		اگر (جوڑو_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_انحراف,
						    "riscv,isa-extensions",
						    "zicntr");
			اگر (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_انحراف, تہی),
					   err);
		}
	}
}

ساکن خالی fdt_domain_based_fixup_one(خالی *fdt, صحیح nodeoff)
{
	صحیح rc;
	uint64_t reg_addr, reg_سائز;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_addr || !reg_سائز)
		واپس;

	اگر (!sbi_domain_check_addr(dom, reg_addr, dom->اگلا_منوال,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		اگر (rc < 0)
			واپس;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساکن خالی fdt_fixup_گرہ(خالی *fdt, ثابت حرف *compatible)
{
	صحیح noff = 0;

	جب ((noff = fdt_گرہ_انحراف_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

خالی fdt_aplic_fixup(خالی *fdt)
{
	fdt_fixup_گرہ(fdt, "riscv,aplic");
}

خالی fdt_imsic_fixup(خالی *fdt)
{
	fdt_fixup_گرہ(fdt, "riscv,imsics");
}

خالی fdt_plic_fixup(خالی *fdt)
{
	u32 *cells;
	صحیح i, cells_گنتی;
	صحیح plic_off;

	plic_off = fdt_گرہ_انحراف_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	اگر (plic_off < 0) {
		plic_off = fdt_گرہ_انحراف_by_compatible(fdt, 0, "riscv,plic0");
		اگر (plic_off < 0)
			واپس;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_گنتی);
	اگر (!cells)
		واپس;

	cells_گنتی = cells_گنتی / حجم(u32);
	اگر (!cells_گنتی)
		واپس;

	برائے (i = 0; i < (cells_گنتی / 2); i++) {
		اگر (fdt32_مقصد_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_مقصد_fdt32(0xffffffff);
	}
}

ساکن صحیح fdt_resv_حافظہ_update_گرہ(خالی *fdt, غیرعلامتی طویل addr,
				       غیرعلامتی طویل سائز, صحیح اشاریہ_2,
				       صحیح والد)
{
	صحیح na = fdt_پتہ_cells(fdt, 0);
	صحیح ns = fdt_سائز_cells(fdt, 0);
	fdt32_t addr_بلند, addr_کم;
	fdt32_t سائز_بلند, سائز_کم;
	صحیح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_بلند = (u64)addr >> 32;
	addr_کم = addr;
	سائز_بلند = (u64)سائز >> 32;
	سائز_کم = سائز;

	اگر (na > 1 && addr_بلند)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", اشاریہ_2,
			     addr_بلند, addr_کم);
	ورنہ
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", اشاریہ_2,
			     addr_کم);

	subnode = fdt_جوڑو_subnode(fdt, والد, name);
	اگر (subnode < 0)
		واپس subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_خالی_خانے(fdt, subnode, "no-map");
	اگر (err < 0)
		واپس err;

	/* encode the <reg> property value */
	val = reg;
	اگر (na > 1)
		*val++ = cpu_مقصد_fdt32(addr_بلند);
	*val++ = cpu_مقصد_fdt32(addr_کم);
	اگر (ns > 1)
		*val++ = cpu_مقصد_fdt32(سائز_بلند);
	*val++ = cpu_مقصد_fdt32(سائز_کم);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * حجم(fdt32_t));
	اگر (err < 0)
		واپس err;

	واپس 0;
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
صحیح fdt_reserved_حافظہ_fixup(خالی *fdt)
{
	ساخت sbi_domain_memregion *reg;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	غیرعلامتی طویل filtered_بنیاد[PMP_گنتی] = { 0 };
	غیرعلامتی حرف filtered_ترتیب[PMP_گنتی] = { 0 };
	غیرعلامتی طویل addr, سائز;
	صحیح err, والد, i, j;
	صحیح na = fdt_پتہ_cells(fdt, 0);
	صحیح ns = fdt_سائز_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	اگر (err < 0)
		واپس err;

	/* try to locate the reserved memory node */
	والد = fdt_راستہ_انحراف(fdt, "/reserved-memory");
	اگر (والد < 0) {
		/* if such node does not exist, create one */
		والد = fdt_جوڑو_subnode(fdt, 0, "reserved-memory");
		اگر (والد < 0)
			واپس والد;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_خالی_خانے(fdt, والد, "ranges");
		اگر (err < 0)
			واپس err;

		err = fdt_setprop_u32(fdt, والد, "#size-cells", ns);
		اگر (err < 0)
			واپس err;

		err = fdt_setprop_u32(fdt, والد, "#address-cells", na);
		اگر (err < 0)
			واپس err;
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
			جاری;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			جاری;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			جاری;
		اگر (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			جاری;

		اگر (i >= PMP_گنتی) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			واپس SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->بنیاد_2;
		برائے (j = 0; j < i; j++) {
			اگر (addr == filtered_بنیاد[j]
			    && filtered_ترتیب[j] < reg->ترتیب) {
				overlap = true;
				filtered_ترتیب[j] = reg->ترتیب;
				توڑو;
			}
		}

		اگر (!overlap) {
			filtered_بنیاد[i] = reg->بنیاد_2;
			filtered_ترتیب[i] = reg->ترتیب;
			i++;
		}
	}

	برائے (j = 0; j < i; j++) {
		addr = filtered_بنیاد[j];
		سائز = 1UL << filtered_ترتیب[j];
		fdt_resv_حافظہ_update_گرہ(fdt, addr, سائز, j, والد);
	}

	واپس 0;
}

خالی fdt_config_fixup(خالی *fdt)
{
	صحیح chosen_انحراف, config_انحراف;

	chosen_انحراف = fdt_راستہ_انحراف(fdt, "/chosen");
	اگر (chosen_انحراف < 0)
		واپس;

	config_انحراف = fdt_گرہ_انحراف_by_compatible(fdt, chosen_انحراف, "opensbi,config");
	اگر (config_انحراف < 0)
		واپس;

	fdt_nop_گرہ(fdt, config_انحراف);
}

ساکن SBI_LIST_سر(fixup_list);

صحیح fdt_رجسٹر_general_fixup(ساخت fdt_general_fixup *fixup)
{
	ساخت fdt_general_fixup *f;

	اگر (!fixup || !fixup->name || !fixup->do_fixup)
		واپس SBI_EINVAL;

	sbi_list_for_each_اندراج(f, &fixup_list, سر) {
		اگر (f == fixup)
			واپس SBI_EALREADY;
	}

	sbi_list_جوڑو_دم(&fixup->سر, &fixup_list);

	واپس 0;
}

خالی fdt_unregister_general_fixup(ساخت fdt_general_fixup *fixup)
{
	اگر (!fixup)
		واپس;

	sbi_list_del(&fixup->سر);
}

خالی fdt_fixups(خالی *fdt)
{
	ساخت fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_حافظہ_fixup(fdt);

#اگر_غیر_تعریف_شدہ CONFIG_FDT_FIXUPS_PRESERVE_PMU_گرہ
	fdt_pmu_fixup(fdt);
#اگر_ختم

	fdt_config_fixup(fdt);

	sbi_list_for_each_اندراج(f, &fixup_list, سر)
		f->do_fixup(f, fdt);
}
