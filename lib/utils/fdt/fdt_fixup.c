
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_math.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_fixup.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_pmu.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>

ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_cpu_idle_ಸ್ಥಿತಿಗಳು(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_cpu_idle_state *state)
{
	ಪೂರ್ಣಾಂಕ cpu_ಗಂಟು, cpus_ಗಂಟು, err, idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು;
	uint32_t ಎಣಿಕೆ_2, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	err = fdt_ಕಂಡುಹಿಡಿ_max_phandle(fdt, &phandle);
	phandle++;
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	cpus_ಗಂಟು = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಗಂಟು < 0)
		ಹಿಂತಿರುಗಿ cpus_ಗಂಟು;

	/* Do nothing if the idle-states node already exists. */
	idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು = fdt_subnode_ಸ್ಥಾನಾಂತರ(fdt, cpus_ಗಂಟು, "idle-states");
	ಆದರೆ (idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು >= 0)
		ಹಿಂತಿರುಗಿ 0;

	/* Create the idle-states node and its child nodes. */
	idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು = fdt_ಕೂಡಿಸು_subnode(fdt, cpus_ಗಂಟು, "idle-states");
	ಆದರೆ (idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು < 0)
		ಹಿಂತಿರುಗಿ idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು;

	ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; state->name; ಎಣಿಕೆ_2++, phandle++, state++) {
		ಪೂರ್ಣಾಂಕ idle_state_ಗಂಟು;

		idle_state_ಗಂಟು = fdt_ಕೂಡಿಸು_subnode(fdt, idle_ಸ್ಥಿತಿಗಳು_ಗಂಟು,
						  state->name);
		ಆದರೆ (idle_state_ಗಂಟು < 0)
			ಹಿಂತಿರುಗಿ idle_state_ಗಂಟು;

		fdt_setprop_string(fdt, idle_state_ಗಂಟು, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ಆದರೆ (state->local_timer_stop)
			fdt_setprop_ಖಾಲಿಗಳು(fdt, idle_state_ಗಂಟು,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "entry-latency-us",
				state->ನಮೂದು_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "min-residency-us",
				state->min_residency_us);
		ಆದರೆ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ಗಂಟು,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "phandle", phandle);
	}

	ಆದರೆ (ಎಣಿಕೆ_2 == 0)
		ಹಿಂತಿರುಗಿ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ಗಂಟು, fdt, cpus_ಗಂಟು) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *device_type;
		fdt32_t *ಮೌಲ್ಯ;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ಗಂಟು, "device_type", ಖಾಲಿ);
		ಆದರೆ (!device_type || strcmp(device_type, "cpu"))
			ಮುಂದುವರಿ;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ಗಂಟು, "cpu-idle-states",
					      ಎಣಿಕೆ_2 * ಗಾತ್ರ(phandle),
					      (ಶೂನ್ಯ **)&ಮೌಲ್ಯ);
		ಆದರೆ (err < 0)
			ಹಿಂತಿರುಗಿ err;

		/* Fill in the phandles of the idle state nodes. */
		ಗಾಗಿ (uint32_t i = 0; i < ಎಣಿಕೆ_2; ++i)
			ಮೌಲ್ಯ[i] = cpu_ಗಮ್ಯ_fdt32(phandle - ಎಣಿಕೆ_2 + i);
	}

	ಹಿಂತಿರುಗಿ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_cpu_fixup_needed(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ cpu_ಸ್ಥಾನಾಂತರ,
				 bool emulated_zicntr, bool *disable,
				 bool *ಕೂಡಿಸು_zicntr)
{
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mmu_type, *extensions;
	u32 hartid, hartindex;
	ಪೂರ್ಣಾಂಕ len;

	*disable = false;
	*ಕೂಡಿಸು_zicntr = false;

	ಆದರೆ (fdt_parse_hart_id(fdt, cpu_ಸ್ಥಾನಾಂತರ, &hartid))
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_ಗಮ್ಯ_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "mmu-type", &len);
	ಆದರೆ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ಆದರೆ (!emulated_zicntr)
		ಹಿಂತಿರುಗಿ;

	extensions = fdt_getprop(fdt, cpu_ಸ್ಥಾನಾಂತರ, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ಆದರೆ (extensions && !fdt_stringlist_ಒಳಗೊಂಡಿದೆ(extensions, len, "zicntr"))
		*ಕೂಡಿಸು_zicntr = true;
}

ಶೂನ್ಯ fdt_cpu_fixup(ಶೂನ್ಯ *fdt)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, ಕೂಡಿಸು_zicntr;
	ಪೂರ್ಣಾಂಕ err, cpu_ಸ್ಥಾನಾಂತರ, cpus_ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ fdt_property) +
				ROUNDUP(ಗಾತ್ರ("disabled"), FDT_TAGSIZE) +
				ಗಾತ್ರ("status");
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ zicntr_ಗಾತ್ರ = ROUNDUP(ಗಾತ್ರ("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ಖಾಲಿ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_ಚಕ್ರ) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_ಸ್ಥಾನಾಂತರ, fdt, cpus_ಸ್ಥಾನಾಂತರ) {
		fdt_cpu_fixup_needed(fdt, cpu_ಸ್ಥಾನಾಂತರ, emulated_zicntr,
				     &disable, &ಕೂಡಿಸು_zicntr);
		ಆದರೆ (disable)
			reserve += ಸ್ಥಿತಿ_ಗಾತ್ರ;
		ಆದರೆ (ಕೂಡಿಸು_zicntr)
			reserve += zicntr_ಗಾತ್ರ;
	}

	ಆದರೆ (!reserve)
		ಹಿಂತಿರುಗಿ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/cpus");
	ಆದರೆ (cpus_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ;

	fdt_for_each_subnode(cpu_ಸ್ಥಾನಾಂತರ, fdt, cpus_ಸ್ಥಾನಾಂತರ) {
		fdt_cpu_fixup_needed(fdt, cpu_ಸ್ಥಾನಾಂತರ, emulated_zicntr,
				     &disable, &ಕೂಡಿಸು_zicntr);

		ಆದರೆ (disable) {
			err = fdt_setprop_string(fdt, cpu_ಸ್ಥಾನಾಂತರ, "status",
						 "disabled");
			ಆದರೆ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_ಸ್ಥಾನಾಂತರ, ಖಾಲಿ),
					   err);
		}

		ಆದರೆ (ಕೂಡಿಸು_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_ಸ್ಥಾನಾಂತರ,
						    "riscv,isa-extensions",
						    "zicntr");
			ಆದರೆ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_ಸ್ಥಾನಾಂತರ, ಖಾಲಿ),
					   err);
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_domain_based_fixup_one(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff)
{
	ಪೂರ್ಣಾಂಕ rc;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &reg_addr, &reg_ಗಾತ್ರ);
	ಆದರೆ (rc < 0 || !reg_addr || !reg_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!sbi_domain_check_addr(dom, reg_addr, dom->ಮುಂದಿನ_ಬಹುಲಕ,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ಆದರೆ (rc < 0)
			ಹಿಂತಿರುಗಿ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_fixup_ಗಂಟು(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible)
{
	ಪೂರ್ಣಾಂಕ noff = 0;

	ಆಗ ((noff = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

ಶೂನ್ಯ fdt_aplic_fixup(ಶೂನ್ಯ *fdt)
{
	fdt_fixup_ಗಂಟು(fdt, "riscv,aplic");
}

ಶೂನ್ಯ fdt_imsic_fixup(ಶೂನ್ಯ *fdt)
{
	fdt_fixup_ಗಂಟು(fdt, "riscv,imsics");
}

ಶೂನ್ಯ fdt_plic_fixup(ಶೂನ್ಯ *fdt)
{
	u32 *cells;
	ಪೂರ್ಣಾಂಕ i, cells_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ plic_off;

	plic_off = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ಆದರೆ (plic_off < 0) {
		plic_off = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, 0, "riscv,plic0");
		ಆದರೆ (plic_off < 0)
			ಹಿಂತಿರುಗಿ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ಎಣಿಕೆ);
	ಆದರೆ (!cells)
		ಹಿಂತಿರುಗಿ;

	cells_ಎಣಿಕೆ = cells_ಎಣಿಕೆ / ಗಾತ್ರ(u32);
	ಆದರೆ (!cells_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ;

	ಗಾಗಿ (i = 0; i < (cells_ಎಣಿಕೆ / 2); i++) {
		ಆದರೆ (fdt32_ಗಮ್ಯ_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_ಗಮ್ಯ_fdt32(0xffffffff);
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_resv_ಸ್ಮರಣೆ_update_ಗಂಟು(ಶೂನ್ಯ *fdt, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr,
				       ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2,
				       ಪೂರ್ಣಾಂಕ ಪೋಷಕ)
{
	ಪೂರ್ಣಾಂಕ na = fdt_ವಿಳಾಸ_cells(fdt, 0);
	ಪೂರ್ಣಾಂಕ ns = fdt_ಗಾತ್ರ_cells(fdt, 0);
	fdt32_t addr_ಮೇಲಿನ, addr_ಕೆಳಗಿನ;
	fdt32_t ಗಾತ್ರ_ಮೇಲಿನ, ಗಾತ್ರ_ಕೆಳಗಿನ;
	ಪೂರ್ಣಾಂಕ subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	ಅಕ್ಷರ name[32];

	addr_ಮೇಲಿನ = (u64)addr >> 32;
	addr_ಕೆಳಗಿನ = addr;
	ಗಾತ್ರ_ಮೇಲಿನ = (u64)ಗಾತ್ರ_2 >> 32;
	ಗಾತ್ರ_ಕೆಳಗಿನ = ಗಾತ್ರ_2;

	ಆದರೆ (na > 1 && addr_ಮೇಲಿನ)
		sbi_snprintf(name, ಗಾತ್ರ(name),
			     "mmode_resv%d@%x,%x", ಸೂಚ್ಯಂಕ_2,
			     addr_ಮೇಲಿನ, addr_ಕೆಳಗಿನ);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_snprintf(name, ಗಾತ್ರ(name),
			     "mmode_resv%d@%x", ಸೂಚ್ಯಂಕ_2,
			     addr_ಕೆಳಗಿನ);

	subnode = fdt_ಕೂಡಿಸು_subnode(fdt, ಪೋಷಕ, name);
	ಆದರೆ (subnode < 0)
		ಹಿಂತಿರುಗಿ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_ಖಾಲಿಗಳು(fdt, subnode, "no-map");
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	/* encode the <reg> property value */
	val = reg;
	ಆದರೆ (na > 1)
		*val++ = cpu_ಗಮ್ಯ_fdt32(addr_ಮೇಲಿನ);
	*val++ = cpu_ಗಮ್ಯ_fdt32(addr_ಕೆಳಗಿನ);
	ಆದರೆ (ns > 1)
		*val++ = cpu_ಗಮ್ಯ_fdt32(ಗಾತ್ರ_ಮೇಲಿನ);
	*val++ = cpu_ಗಮ್ಯ_fdt32(ಗಾತ್ರ_ಕೆಳಗಿನ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * ಗಾತ್ರ(fdt32_t));
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	ಹಿಂತಿರುಗಿ 0;
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
ಪೂರ್ಣಾಂಕ fdt_reserved_ಸ್ಮರಣೆ_fixup(ಶೂನ್ಯ *fdt)
{
	ರಚನೆ sbi_domain_memregion *reg;
	ರಚನೆ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ filtered_ತಳ[PMP_ಎಣಿಕೆ] = { 0 };
	ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ filtered_ಕ್ರಮ[PMP_ಎಣಿಕೆ] = { 0 };
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, ಗಾತ್ರ_2;
	ಪೂರ್ಣಾಂಕ err, ಪೋಷಕ, i, j;
	ಪೂರ್ಣಾಂಕ na = fdt_ವಿಳಾಸ_cells(fdt, 0);
	ಪೂರ್ಣಾಂಕ ns = fdt_ಗಾತ್ರ_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	/* try to locate the reserved memory node */
	ಪೋಷಕ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/reserved-memory");
	ಆದರೆ (ಪೋಷಕ < 0) {
		/* if such node does not exist, create one */
		ಪೋಷಕ = fdt_ಕೂಡಿಸು_subnode(fdt, 0, "reserved-memory");
		ಆದರೆ (ಪೋಷಕ < 0)
			ಹಿಂತಿರುಗಿ ಪೋಷಕ;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_ಖಾಲಿಗಳು(fdt, ಪೋಷಕ, "ranges");
		ಆದರೆ (err < 0)
			ಹಿಂತಿರುಗಿ err;

		err = fdt_setprop_u32(fdt, ಪೋಷಕ, "#size-cells", ns);
		ಆದರೆ (err < 0)
			ಹಿಂತಿರುಗಿ err;

		err = fdt_setprop_u32(fdt, ಪೋಷಕ, "#address-cells", na);
		ಆದರೆ (err < 0)
			ಹಿಂತಿರುಗಿ err;
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
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ಮುಂದುವರಿ;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ಮುಂದುವರಿ;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ಮುಂದುವರಿ;
		ಆದರೆ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ಮುಂದುವರಿ;

		ಆದರೆ (i >= PMP_ಎಣಿಕೆ) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ಹಿಂತಿರುಗಿ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->ತಳ_2;
		ಗಾಗಿ (j = 0; j < i; j++) {
			ಆದರೆ (addr == filtered_ತಳ[j]
			    && filtered_ಕ್ರಮ[j] < reg->ಕ್ರಮ) {
				overlap = true;
				filtered_ಕ್ರಮ[j] = reg->ಕ್ರಮ;
				ನಿಲ್ಲಿಸು;
			}
		}

		ಆದರೆ (!overlap) {
			filtered_ತಳ[i] = reg->ತಳ_2;
			filtered_ಕ್ರಮ[i] = reg->ಕ್ರಮ;
			i++;
		}
	}

	ಗಾಗಿ (j = 0; j < i; j++) {
		addr = filtered_ತಳ[j];
		ಗಾತ್ರ_2 = 1UL << filtered_ಕ್ರಮ[j];
		fdt_resv_ಸ್ಮರಣೆ_update_ಗಂಟು(fdt, addr, ಗಾತ್ರ_2, j, ಪೋಷಕ);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ fdt_config_fixup(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ chosen_ಸ್ಥಾನಾಂತರ, config_ಸ್ಥಾನಾಂತರ;

	chosen_ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/chosen");
	ಆದರೆ (chosen_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ;

	config_ಸ್ಥಾನಾಂತರ = fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(fdt, chosen_ಸ್ಥಾನಾಂತರ, "opensbi,config");
	ಆದರೆ (config_ಸ್ಥಾನಾಂತರ < 0)
		ಹಿಂತಿರುಗಿ;

	fdt_nop_ಗಂಟು(fdt, config_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ SBI_LIST_ತಲೆ(fixup_list);

ಪೂರ್ಣಾಂಕ fdt_ರಿಜಿಸ್ಟರ್_general_fixup(ರಚನೆ fdt_general_fixup *fixup)
{
	ರಚನೆ fdt_general_fixup *f;

	ಆದರೆ (!fixup || !fixup->name || !fixup->do_fixup)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	sbi_list_for_each_ನಮೂದು(f, &fixup_list, ತಲೆ) {
		ಆದರೆ (f == fixup)
			ಹಿಂತಿರುಗಿ SBI_EALREADY;
	}

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&fixup->ತಲೆ, &fixup_list);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ fdt_unregister_general_fixup(ರಚನೆ fdt_general_fixup *fixup)
{
	ಆದರೆ (!fixup)
		ಹಿಂತಿರುಗಿ;

	sbi_list_del(&fixup->ತಲೆ);
}

ಶೂನ್ಯ fdt_fixups(ಶೂನ್ಯ *fdt)
{
	ರಚನೆ fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_ಸ್ಮರಣೆ_fixup(fdt);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_FDT_FIXUPS_PRESERVE_PMU_ಗಂಟು
	fdt_pmu_fixup(fdt);
#ಆದರೆ_ಅಂತ್ಯ

	fdt_config_fixup(fdt);

	sbi_list_for_each_ನಮೂದು(f, &fixup_list, ತಲೆ)
		f->do_fixup(f, fdt);
}
