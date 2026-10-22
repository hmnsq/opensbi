
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

صحيح fdt_جوڙيو_cpu_idle_حالتون(فراغ *fdt, ثابت بنية sbi_cpu_idle_state *state)
{
	صحيح cpu_ڳنڍ, cpus_ڳنڍ, err, idle_حالتون_ڳنڍ;
	uint32_t ڳڻپ, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	إذا (err < 0)
		أرجع err;

	err = fdt_ڳوليو_max_phandle(fdt, &phandle);
	phandle++;
	إذا (err < 0)
		أرجع err;

	cpus_ڳنڍ = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_ڳنڍ < 0)
		أرجع cpus_ڳنڍ;

	/* Do nothing if the idle-states node already exists. */
	idle_حالتون_ڳنڍ = fdt_subnode_وٿي(fdt, cpus_ڳنڍ, "idle-states");
	إذا (idle_حالتون_ڳنڍ >= 0)
		أرجع 0;

	/* Create the idle-states node and its child nodes. */
	idle_حالتون_ڳنڍ = fdt_جوڙيو_subnode(fdt, cpus_ڳنڍ, "idle-states");
	إذا (idle_حالتون_ڳنڍ < 0)
		أرجع idle_حالتون_ڳنڍ;

	لكل (ڳڻپ = 0; state->name; ڳڻپ++, phandle++, state++) {
		صحيح idle_state_ڳنڍ;

		idle_state_ڳنڍ = fdt_جوڙيو_subnode(fdt, idle_حالتون_ڳنڍ,
						  state->name);
		إذا (idle_state_ڳنڍ < 0)
			أرجع idle_state_ڳنڍ;

		fdt_setprop_string(fdt, idle_state_ڳنڍ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ڳنڍ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		إذا (state->local_timer_stop)
			fdt_setprop_خالي_جايون(fdt, idle_state_ڳنڍ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ڳنڍ, "entry-latency-us",
				state->داخلا_latency_us);
		fdt_setprop_u32(fdt, idle_state_ڳنڍ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ڳنڍ, "min-residency-us",
				state->min_residency_us);
		إذا (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ڳنڍ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ڳنڍ, "phandle", phandle);
	}

	إذا (ڳڻپ == 0)
		أرجع 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ڳنڍ, fdt, cpus_ڳنڍ) {
		ثابت حرف *device_type;
		fdt32_t *قدر;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ڳنڍ, "device_type", عدم);
		إذا (!device_type || strcmp(device_type, "cpu"))
			استمر;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ڳنڍ, "cpu-idle-states",
					      ڳڻپ * حجم(phandle),
					      (فراغ **)&قدر);
		إذا (err < 0)
			أرجع err;

		/* Fill in the phandles of the idle state nodes. */
		لكل (uint32_t i = 0; i < ڳڻپ; ++i)
			قدر[i] = cpu_منزل_fdt32(phandle - ڳڻپ + i);
	}

	أرجع 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساكن فراغ fdt_cpu_fixup_needed(ثابت فراغ *fdt, صحيح cpu_وٿي,
				 bool emulated_zicntr, bool *disable,
				 bool *جوڙيو_zicntr)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحيح len;

	*disable = false;
	*جوڙيو_zicntr = false;

	إذا (fdt_parse_hart_id(fdt, cpu_وٿي, &hartid))
		أرجع;

	إذا (!fdt_ڳنڍ_is_enabled(fdt, cpu_وٿي))
		أرجع;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_منزل_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_وٿي, "mmu-type", &len);
	إذا (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	إذا (!emulated_zicntr)
		أرجع;

	extensions = fdt_getprop(fdt, cpu_وٿي, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	إذا (extensions && !fdt_stringlist_شامل_آهي(extensions, len, "zicntr"))
		*جوڙيو_zicntr = true;
}

فراغ fdt_cpu_fixup(فراغ *fdt)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, جوڙيو_zicntr;
	صحيح err, cpu_وٿي, cpus_وٿي;
	صحيح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحيح حالت_سائيز = حجم(بنية fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحيح zicntr_سائيز = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != عدم &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_چڪر) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_وٿي = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_وٿي < 0)
		أرجع;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_وٿي, fdt, cpus_وٿي) {
		fdt_cpu_fixup_needed(fdt, cpu_وٿي, emulated_zicntr,
				     &disable, &جوڙيو_zicntr);
		إذا (disable)
			reserve += حالت_سائيز;
		إذا (جوڙيو_zicntr)
			reserve += zicntr_سائيز;
	}

	إذا (!reserve)
		أرجع;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	إذا (err < 0)
		أرجع;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_وٿي = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_وٿي < 0)
		أرجع;

	fdt_for_each_subnode(cpu_وٿي, fdt, cpus_وٿي) {
		fdt_cpu_fixup_needed(fdt, cpu_وٿي, emulated_zicntr,
				     &disable, &جوڙيو_zicntr);

		إذا (disable) {
			err = fdt_setprop_string(fdt, cpu_وٿي, "status",
						 "disabled");
			إذا (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_وٿي, عدم),
					   err);
		}

		إذا (جوڙيو_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_وٿي,
						    "riscv,isa-extensions",
						    "zicntr");
			إذا (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_وٿي, عدم),
					   err);
		}
	}
}

ساكن فراغ fdt_domain_based_fixup_one(فراغ *fdt, صحيح nodeoff)
{
	صحيح rc;
	uint64_t reg_addr, reg_سائيز;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_addr || !reg_سائيز)
		أرجع;

	إذا (!sbi_domain_check_addr(dom, reg_addr, dom->ايندڙ_گهڻائي,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		إذا (rc < 0)
			أرجع;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساكن فراغ fdt_fixup_ڳنڍ(فراغ *fdt, ثابت حرف *compatible)
{
	صحيح noff = 0;

	طالما ((noff = fdt_ڳنڍ_وٿي_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

فراغ fdt_aplic_fixup(فراغ *fdt)
{
	fdt_fixup_ڳنڍ(fdt, "riscv,aplic");
}

فراغ fdt_imsic_fixup(فراغ *fdt)
{
	fdt_fixup_ڳنڍ(fdt, "riscv,imsics");
}

فراغ fdt_plic_fixup(فراغ *fdt)
{
	u32 *cells;
	صحيح i, cells_ڳڻپ;
	صحيح plic_off;

	plic_off = fdt_ڳنڍ_وٿي_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	إذا (plic_off < 0) {
		plic_off = fdt_ڳنڍ_وٿي_by_compatible(fdt, 0, "riscv,plic0");
		إذا (plic_off < 0)
			أرجع;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ڳڻپ);
	إذا (!cells)
		أرجع;

	cells_ڳڻپ = cells_ڳڻپ / حجم(u32);
	إذا (!cells_ڳڻپ)
		أرجع;

	لكل (i = 0; i < (cells_ڳڻپ / 2); i++) {
		إذا (fdt32_منزل_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_منزل_fdt32(0xffffffff);
	}
}

ساكن صحيح fdt_resv_يادگيري_update_ڳنڍ(فراغ *fdt, بنانشان ڊگهو addr,
				       بنانشان ڊگهو سائيز, صحيح اشاريو_2,
				       صحيح والد)
{
	صحيح na = fdt_پتو_cells(fdt, 0);
	صحيح ns = fdt_سائيز_cells(fdt, 0);
	fdt32_t addr_مٿاهون, addr_هيٺيون;
	fdt32_t سائيز_مٿاهون, سائيز_هيٺيون;
	صحيح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_مٿاهون = (u64)addr >> 32;
	addr_هيٺيون = addr;
	سائيز_مٿاهون = (u64)سائيز >> 32;
	سائيز_هيٺيون = سائيز;

	إذا (na > 1 && addr_مٿاهون)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", اشاريو_2,
			     addr_مٿاهون, addr_هيٺيون);
	وإلا
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", اشاريو_2,
			     addr_هيٺيون);

	subnode = fdt_جوڙيو_subnode(fdt, والد, name);
	إذا (subnode < 0)
		أرجع subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_خالي_جايون(fdt, subnode, "no-map");
	إذا (err < 0)
		أرجع err;

	/* encode the <reg> property value */
	val = reg;
	إذا (na > 1)
		*val++ = cpu_منزل_fdt32(addr_مٿاهون);
	*val++ = cpu_منزل_fdt32(addr_هيٺيون);
	إذا (ns > 1)
		*val++ = cpu_منزل_fdt32(سائيز_مٿاهون);
	*val++ = cpu_منزل_fdt32(سائيز_هيٺيون);

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
صحيح fdt_reserved_يادگيري_fixup(فراغ *fdt)
{
	بنية sbi_domain_memregion *reg;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنانشان ڊگهو filtered_بنياد[PMP_ڳڻپ] = { 0 };
	بنانشان حرف filtered_ترتيب[PMP_ڳڻپ] = { 0 };
	بنانشان ڊگهو addr, سائيز;
	صحيح err, والد, i, j;
	صحيح na = fdt_پتو_cells(fdt, 0);
	صحيح ns = fdt_سائيز_cells(fdt, 0);

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
	والد = fdt_رستو_وٿي(fdt, "/reserved-memory");
	إذا (والد < 0) {
		/* if such node does not exist, create one */
		والد = fdt_جوڙيو_subnode(fdt, 0, "reserved-memory");
		إذا (والد < 0)
			أرجع والد;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_خالي_جايون(fdt, والد, "ranges");
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

		إذا (i >= PMP_ڳڻپ) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			أرجع SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->بنياد_2;
		لكل (j = 0; j < i; j++) {
			إذا (addr == filtered_بنياد[j]
			    && filtered_ترتيب[j] < reg->ترتيب) {
				overlap = true;
				filtered_ترتيب[j] = reg->ترتيب;
				اكسر;
			}
		}

		إذا (!overlap) {
			filtered_بنياد[i] = reg->بنياد_2;
			filtered_ترتيب[i] = reg->ترتيب;
			i++;
		}
	}

	لكل (j = 0; j < i; j++) {
		addr = filtered_بنياد[j];
		سائيز = 1UL << filtered_ترتيب[j];
		fdt_resv_يادگيري_update_ڳنڍ(fdt, addr, سائيز, j, والد);
	}

	أرجع 0;
}

فراغ fdt_config_fixup(فراغ *fdt)
{
	صحيح chosen_وٿي, config_وٿي;

	chosen_وٿي = fdt_رستو_وٿي(fdt, "/chosen");
	إذا (chosen_وٿي < 0)
		أرجع;

	config_وٿي = fdt_ڳنڍ_وٿي_by_compatible(fdt, chosen_وٿي, "opensbi,config");
	إذا (config_وٿي < 0)
		أرجع;

	fdt_nop_ڳنڍ(fdt, config_وٿي);
}

ساكن SBI_LIST_سر(fixup_list);

صحيح fdt_رجسٽر_general_fixup(بنية fdt_general_fixup *fixup)
{
	بنية fdt_general_fixup *f;

	إذا (!fixup || !fixup->name || !fixup->do_fixup)
		أرجع SBI_EINVAL;

	sbi_list_for_each_داخلا(f, &fixup_list, سر) {
		إذا (f == fixup)
			أرجع SBI_EALREADY;
	}

	sbi_list_جوڙيو_پڇ(&fixup->سر, &fixup_list);

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

	fdt_reserved_يادگيري_fixup(fdt);

#جيڪڏهن_وصف_نه_ٿيل CONFIG_FDT_FIXUPS_PRESERVE_PMU_ڳنڍ
	fdt_pmu_fixup(fdt);
#جيڪڏهن_پورو

	fdt_config_fixup(fdt);

	sbi_list_for_each_داخلا(f, &fixup_list, سر)
		f->do_fixup(f, fdt);
}
