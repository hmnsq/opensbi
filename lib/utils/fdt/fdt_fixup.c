
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

صحيح fdt_جوڑو_cpu_idle_حالتاں(فراغ *fdt, ثابت بنية sbi_cpu_idle_state *state)
{
	صحيح cpu_گرہ, cpus_گرہ, err, idle_حالتاں_گرہ;
	uint32_t گنتی, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	إذا (err < 0)
		أرجع err;

	err = fdt_لبھو_max_phandle(fdt, &phandle);
	phandle++;
	إذا (err < 0)
		أرجع err;

	cpus_گرہ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_گرہ < 0)
		أرجع cpus_گرہ;

	/* Do nothing if the idle-states node already exists. */
	idle_حالتاں_گرہ = fdt_subnode_وتھ(fdt, cpus_گرہ, "idle-states");
	إذا (idle_حالتاں_گرہ >= 0)
		أرجع 0;

	/* Create the idle-states node and its child nodes. */
	idle_حالتاں_گرہ = fdt_جوڑو_subnode(fdt, cpus_گرہ, "idle-states");
	إذا (idle_حالتاں_گرہ < 0)
		أرجع idle_حالتاں_گرہ;

	لكل (گنتی = 0; state->name; گنتی++, phandle++, state++) {
		صحيح idle_state_گرہ;

		idle_state_گرہ = fdt_جوڑو_subnode(fdt, idle_حالتاں_گرہ,
						  state->name);
		إذا (idle_state_گرہ < 0)
			أرجع idle_state_گرہ;

		fdt_setprop_string(fdt, idle_state_گرہ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_گرہ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		إذا (state->local_timer_stop)
			fdt_setprop_خالی_تھاواں(fdt, idle_state_گرہ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_گرہ, "entry-latency-us",
				state->اندراج_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "min-residency-us",
				state->min_residency_us);
		إذا (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_گرہ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_گرہ, "phandle", phandle);
	}

	إذا (گنتی == 0)
		أرجع 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_گرہ, fdt, cpus_گرہ) {
		ثابت حرف *device_type;
		fdt32_t *قدر;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_گرہ, "device_type", عدم);
		إذا (!device_type || strcmp(device_type, "cpu"))
			استمر;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_گرہ, "cpu-idle-states",
					      گنتی * حجم(phandle),
					      (فراغ **)&قدر);
		إذا (err < 0)
			أرجع err;

		/* Fill in the phandles of the idle state nodes. */
		لكل (uint32_t i = 0; i < گنتی; ++i)
			قدر[i] = cpu_منزل_fdt32(phandle - گنتی + i);
	}

	أرجع 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ساكن فراغ fdt_cpu_fixup_needed(ثابت فراغ *fdt, صحيح cpu_وتھ,
				 bool emulated_zicntr, bool *disable,
				 bool *جوڑو_zicntr)
{
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	ثابت حرف *mmu_type, *extensions;
	u32 hartid, hartindex;
	صحيح len;

	*disable = false;
	*جوڑو_zicntr = false;

	إذا (fdt_parse_hart_id(fdt, cpu_وتھ, &hartid))
		أرجع;

	إذا (!fdt_گرہ_is_enabled(fdt, cpu_وتھ))
		أرجع;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_منزل_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_وتھ, "mmu-type", &len);
	إذا (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	إذا (!emulated_zicntr)
		أرجع;

	extensions = fdt_getprop(fdt, cpu_وتھ, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	إذا (extensions && !fdt_stringlist_شامل_اے(extensions, len, "zicntr"))
		*جوڑو_zicntr = true;
}

فراغ fdt_cpu_fixup(فراغ *fdt)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, جوڑو_zicntr;
	صحيح err, cpu_وتھ, cpus_وتھ;
	صحيح reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ثابت صحيح حالت_ناپ = حجم(بنية fdt_property) +
				ROUNDUP(حجم("disabled"), FDT_TAGSIZE) +
				حجم("status");
	ثابت صحيح zicntr_ناپ = ROUNDUP(حجم("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != عدم &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_چکر) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_وتھ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_وتھ < 0)
		أرجع;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_وتھ, fdt, cpus_وتھ) {
		fdt_cpu_fixup_needed(fdt, cpu_وتھ, emulated_zicntr,
				     &disable, &جوڑو_zicntr);
		إذا (disable)
			reserve += حالت_ناپ;
		إذا (جوڑو_zicntr)
			reserve += zicntr_ناپ;
	}

	إذا (!reserve)
		أرجع;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	إذا (err < 0)
		أرجع;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_وتھ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_وتھ < 0)
		أرجع;

	fdt_for_each_subnode(cpu_وتھ, fdt, cpus_وتھ) {
		fdt_cpu_fixup_needed(fdt, cpu_وتھ, emulated_zicntr,
				     &disable, &جوڑو_zicntr);

		إذا (disable) {
			err = fdt_setprop_string(fdt, cpu_وتھ, "status",
						 "disabled");
			إذا (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_وتھ, عدم),
					   err);
		}

		إذا (جوڑو_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_وتھ,
						    "riscv,isa-extensions",
						    "zicntr");
			إذا (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_وتھ, عدم),
					   err);
		}
	}
}

ساكن فراغ fdt_domain_based_fixup_one(فراغ *fdt, صحيح nodeoff)
{
	صحيح rc;
	uint64_t reg_addr, reg_ناپ;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_addr || !reg_ناپ)
		أرجع;

	إذا (!sbi_domain_check_addr(dom, reg_addr, dom->اگلا_منوال,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		إذا (rc < 0)
			أرجع;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ساكن فراغ fdt_fixup_گرہ(فراغ *fdt, ثابت حرف *compatible)
{
	صحيح noff = 0;

	طالما ((noff = fdt_گرہ_وتھ_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

فراغ fdt_aplic_fixup(فراغ *fdt)
{
	fdt_fixup_گرہ(fdt, "riscv,aplic");
}

فراغ fdt_imsic_fixup(فراغ *fdt)
{
	fdt_fixup_گرہ(fdt, "riscv,imsics");
}

فراغ fdt_plic_fixup(فراغ *fdt)
{
	u32 *cells;
	صحيح i, cells_گنتی;
	صحيح plic_off;

	plic_off = fdt_گرہ_وتھ_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	إذا (plic_off < 0) {
		plic_off = fdt_گرہ_وتھ_by_compatible(fdt, 0, "riscv,plic0");
		إذا (plic_off < 0)
			أرجع;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_گنتی);
	إذا (!cells)
		أرجع;

	cells_گنتی = cells_گنتی / حجم(u32);
	إذا (!cells_گنتی)
		أرجع;

	لكل (i = 0; i < (cells_گنتی / 2); i++) {
		إذا (fdt32_منزل_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_منزل_fdt32(0xffffffff);
	}
}

ساكن صحيح fdt_resv_حافظہ_update_گرہ(فراغ *fdt, بنانشانی لما addr,
				       بنانشانی لما ناپ, صحيح اشاریہ_2,
				       صحيح ماپا)
{
	صحيح na = fdt_پتہ_cells(fdt, 0);
	صحيح ns = fdt_ناپ_cells(fdt, 0);
	fdt32_t addr_بلند, addr_کم;
	fdt32_t ناپ_بلند, ناپ_کم;
	صحيح subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	حرف name[32];

	addr_بلند = (u64)addr >> 32;
	addr_کم = addr;
	ناپ_بلند = (u64)ناپ >> 32;
	ناپ_کم = ناپ;

	إذا (na > 1 && addr_بلند)
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x,%x", اشاریہ_2,
			     addr_بلند, addr_کم);
	وإلا
		sbi_snprintf(name, حجم(name),
			     "mmode_resv%d@%x", اشاریہ_2,
			     addr_کم);

	subnode = fdt_جوڑو_subnode(fdt, ماپا, name);
	إذا (subnode < 0)
		أرجع subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_خالی_تھاواں(fdt, subnode, "no-map");
	إذا (err < 0)
		أرجع err;

	/* encode the <reg> property value */
	val = reg;
	إذا (na > 1)
		*val++ = cpu_منزل_fdt32(addr_بلند);
	*val++ = cpu_منزل_fdt32(addr_کم);
	إذا (ns > 1)
		*val++ = cpu_منزل_fdt32(ناپ_بلند);
	*val++ = cpu_منزل_fdt32(ناپ_کم);

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
صحيح fdt_reserved_حافظہ_fixup(فراغ *fdt)
{
	بنية sbi_domain_memregion *reg;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنانشانی لما filtered_ادھار[PMP_گنتی] = { 0 };
	بنانشانی حرف filtered_ترتیب[PMP_گنتی] = { 0 };
	بنانشانی لما addr, ناپ;
	صحيح err, ماپا, i, j;
	صحيح na = fdt_پتہ_cells(fdt, 0);
	صحيح ns = fdt_ناپ_cells(fdt, 0);

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
	ماپا = fdt_رستہ_وتھ(fdt, "/reserved-memory");
	إذا (ماپا < 0) {
		/* if such node does not exist, create one */
		ماپا = fdt_جوڑو_subnode(fdt, 0, "reserved-memory");
		إذا (ماپا < 0)
			أرجع ماپا;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_خالی_تھاواں(fdt, ماپا, "ranges");
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, ماپا, "#size-cells", ns);
		إذا (err < 0)
			أرجع err;

		err = fdt_setprop_u32(fdt, ماپا, "#address-cells", na);
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

		إذا (i >= PMP_گنتی) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			أرجع SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->ادھار_2;
		لكل (j = 0; j < i; j++) {
			إذا (addr == filtered_ادھار[j]
			    && filtered_ترتیب[j] < reg->ترتیب) {
				overlap = true;
				filtered_ترتیب[j] = reg->ترتیب;
				اكسر;
			}
		}

		إذا (!overlap) {
			filtered_ادھار[i] = reg->ادھار_2;
			filtered_ترتیب[i] = reg->ترتیب;
			i++;
		}
	}

	لكل (j = 0; j < i; j++) {
		addr = filtered_ادھار[j];
		ناپ = 1UL << filtered_ترتیب[j];
		fdt_resv_حافظہ_update_گرہ(fdt, addr, ناپ, j, ماپا);
	}

	أرجع 0;
}

فراغ fdt_config_fixup(فراغ *fdt)
{
	صحيح chosen_وتھ, config_وتھ;

	chosen_وتھ = fdt_رستہ_وتھ(fdt, "/chosen");
	إذا (chosen_وتھ < 0)
		أرجع;

	config_وتھ = fdt_گرہ_وتھ_by_compatible(fdt, chosen_وتھ, "opensbi,config");
	إذا (config_وتھ < 0)
		أرجع;

	fdt_nop_گرہ(fdt, config_وتھ);
}

ساكن SBI_LIST_سر(fixup_list);

صحيح fdt_رجسٹر_general_fixup(بنية fdt_general_fixup *fixup)
{
	بنية fdt_general_fixup *f;

	إذا (!fixup || !fixup->name || !fixup->do_fixup)
		أرجع SBI_EINVAL;

	sbi_list_for_each_اندراج(f, &fixup_list, سر) {
		إذا (f == fixup)
			أرجع SBI_EALREADY;
	}

	sbi_list_جوڑو_پوچھ(&fixup->سر, &fixup_list);

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

	fdt_reserved_حافظہ_fixup(fdt);

#جے_بنا_تعریف CONFIG_FDT_FIXUPS_PRESERVE_PMU_گرہ
	fdt_pmu_fixup(fdt);
#جے_انت

	fdt_config_fixup(fdt);

	sbi_list_for_each_اندراج(f, &fixup_list, سر)
		f->do_fixup(f, fdt);
}
