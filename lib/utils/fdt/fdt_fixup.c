
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_fixup.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_pmu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>

ਪੂਰਨ fdt_ਜੋੜੋ_cpu_idle_ਹਾਲਤਾਂ(ਖਾਲੀ *fdt, ਅਚਲ ਬਣਤਰ sbi_cpu_idle_state *state)
{
	ਪੂਰਨ cpu_ਗੰਢ, cpus_ਗੰਢ, err, idle_ਹਾਲਤਾਂ_ਗੰਢ;
	uint32_t ਗਿਣਤੀ_2, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	err = fdt_ਲੱਭੋ_max_phandle(fdt, &phandle);
	phandle++;
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	cpus_ਗੰਢ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਗੰਢ < 0)
		ਵਾਪਸ cpus_ਗੰਢ;

	/* Do nothing if the idle-states node already exists. */
	idle_ਹਾਲਤਾਂ_ਗੰਢ = fdt_subnode_ਵਿੱਥ(fdt, cpus_ਗੰਢ, "idle-states");
	ਜੇ (idle_ਹਾਲਤਾਂ_ਗੰਢ >= 0)
		ਵਾਪਸ 0;

	/* Create the idle-states node and its child nodes. */
	idle_ਹਾਲਤਾਂ_ਗੰਢ = fdt_ਜੋੜੋ_subnode(fdt, cpus_ਗੰਢ, "idle-states");
	ਜੇ (idle_ਹਾਲਤਾਂ_ਗੰਢ < 0)
		ਵਾਪਸ idle_ਹਾਲਤਾਂ_ਗੰਢ;

	ਲਈ (ਗਿਣਤੀ_2 = 0; state->name; ਗਿਣਤੀ_2++, phandle++, state++) {
		ਪੂਰਨ idle_state_ਗੰਢ;

		idle_state_ਗੰਢ = fdt_ਜੋੜੋ_subnode(fdt, idle_ਹਾਲਤਾਂ_ਗੰਢ,
						  state->name);
		ਜੇ (idle_state_ਗੰਢ < 0)
			ਵਾਪਸ idle_state_ਗੰਢ;

		fdt_setprop_string(fdt, idle_state_ਗੰਢ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ਗੰਢ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ਜੇ (state->local_timer_stop)
			fdt_setprop_ਖਾਲੀ_ਥਾਵਾਂ(fdt, idle_state_ਗੰਢ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ਗੰਢ, "entry-latency-us",
				state->ਇੰਦਰਾਜ_latency_us);
		fdt_setprop_u32(fdt, idle_state_ਗੰਢ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ਗੰਢ, "min-residency-us",
				state->min_residency_us);
		ਜੇ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ਗੰਢ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ਗੰਢ, "phandle", phandle);
	}

	ਜੇ (ਗਿਣਤੀ_2 == 0)
		ਵਾਪਸ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ਗੰਢ, fdt, cpus_ਗੰਢ) {
		ਅਚਲ ਅੱਖਰ *device_type;
		fdt32_t *ਮੁੱਲ;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ਗੰਢ, "device_type", ਨਿਰਾ);
		ਜੇ (!device_type || strcmp(device_type, "cpu"))
			ਜਾਰੀ;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ਗੰਢ, "cpu-idle-states",
					      ਗਿਣਤੀ_2 * ਆਕਾਰ(phandle),
					      (ਖਾਲੀ **)&ਮੁੱਲ);
		ਜੇ (err < 0)
			ਵਾਪਸ err;

		/* Fill in the phandles of the idle state nodes. */
		ਲਈ (uint32_t i = 0; i < ਗਿਣਤੀ_2; ++i)
			ਮੁੱਲ[i] = cpu_ਮੰਜ਼ਿਲ_fdt32(phandle - ਗਿਣਤੀ_2 + i);
	}

	ਵਾਪਸ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ਸਥਿਰ ਖਾਲੀ fdt_cpu_fixup_needed(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ cpu_ਵਿੱਥ,
				 bool emulated_zicntr, bool *disable,
				 bool *ਜੋੜੋ_zicntr)
{
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਅਚਲ ਅੱਖਰ *mmu_type, *extensions;
	u32 hartid, hartindex;
	ਪੂਰਨ len;

	*disable = false;
	*ਜੋੜੋ_zicntr = false;

	ਜੇ (fdt_parse_hart_id(fdt, cpu_ਵਿੱਥ, &hartid))
		ਵਾਪਸ;

	ਜੇ (!fdt_ਗੰਢ_is_enabled(fdt, cpu_ਵਿੱਥ))
		ਵਾਪਸ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_ਮੰਜ਼ਿਲ_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_ਵਿੱਥ, "mmu-type", &len);
	ਜੇ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ਜੇ (!emulated_zicntr)
		ਵਾਪਸ;

	extensions = fdt_getprop(fdt, cpu_ਵਿੱਥ, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ਜੇ (extensions && !fdt_stringlist_ਸ਼ਾਮਲ_ਹੈ(extensions, len, "zicntr"))
		*ਜੋੜੋ_zicntr = true;
}

ਖਾਲੀ fdt_cpu_fixup(ਖਾਲੀ *fdt)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, ਜੋੜੋ_zicntr;
	ਪੂਰਨ err, cpu_ਵਿੱਥ, cpus_ਵਿੱਥ;
	ਪੂਰਨ reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ਅਚਲ ਪੂਰਨ ਸਥਿਤੀ_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ fdt_property) +
				ROUNDUP(ਆਕਾਰ("disabled"), FDT_TAGSIZE) +
				ਆਕਾਰ("status");
	ਅਚਲ ਪੂਰਨ zicntr_ਮਾਪ = ROUNDUP(ਆਕਾਰ("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ਨਿਰਾ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_ਚੱਕਰ) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_ਵਿੱਥ, fdt, cpus_ਵਿੱਥ) {
		fdt_cpu_fixup_needed(fdt, cpu_ਵਿੱਥ, emulated_zicntr,
				     &disable, &ਜੋੜੋ_zicntr);
		ਜੇ (disable)
			reserve += ਸਥਿਤੀ_ਮਾਪ;
		ਜੇ (ਜੋੜੋ_zicntr)
			reserve += zicntr_ਮਾਪ;
	}

	ਜੇ (!reserve)
		ਵਾਪਸ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ਜੇ (err < 0)
		ਵਾਪਸ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/cpus");
	ਜੇ (cpus_ਵਿੱਥ < 0)
		ਵਾਪਸ;

	fdt_for_each_subnode(cpu_ਵਿੱਥ, fdt, cpus_ਵਿੱਥ) {
		fdt_cpu_fixup_needed(fdt, cpu_ਵਿੱਥ, emulated_zicntr,
				     &disable, &ਜੋੜੋ_zicntr);

		ਜੇ (disable) {
			err = fdt_setprop_string(fdt, cpu_ਵਿੱਥ, "status",
						 "disabled");
			ਜੇ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_ਵਿੱਥ, ਨਿਰਾ),
					   err);
		}

		ਜੇ (ਜੋੜੋ_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_ਵਿੱਥ,
						    "riscv,isa-extensions",
						    "zicntr");
			ਜੇ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_ਵਿੱਥ, ਨਿਰਾ),
					   err);
		}
	}
}

ਸਥਿਰ ਖਾਲੀ fdt_domain_based_fixup_one(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff)
{
	ਪੂਰਨ rc;
	uint64_t reg_addr, reg_ਮਾਪ;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &reg_addr, &reg_ਮਾਪ);
	ਜੇ (rc < 0 || !reg_addr || !reg_ਮਾਪ)
		ਵਾਪਸ;

	ਜੇ (!sbi_domain_check_addr(dom, reg_addr, dom->ਅਗਲਾ_ਬਹੁਲਕ,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ਜੇ (rc < 0)
			ਵਾਪਸ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ਸਥਿਰ ਖਾਲੀ fdt_fixup_ਗੰਢ(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *compatible)
{
	ਪੂਰਨ noff = 0;

	ਜਦੋਂ ((noff = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

ਖਾਲੀ fdt_aplic_fixup(ਖਾਲੀ *fdt)
{
	fdt_fixup_ਗੰਢ(fdt, "riscv,aplic");
}

ਖਾਲੀ fdt_imsic_fixup(ਖਾਲੀ *fdt)
{
	fdt_fixup_ਗੰਢ(fdt, "riscv,imsics");
}

ਖਾਲੀ fdt_plic_fixup(ਖਾਲੀ *fdt)
{
	u32 *cells;
	ਪੂਰਨ i, cells_ਗਿਣਤੀ;
	ਪੂਰਨ plic_off;

	plic_off = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ਜੇ (plic_off < 0) {
		plic_off = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, 0, "riscv,plic0");
		ਜੇ (plic_off < 0)
			ਵਾਪਸ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ਗਿਣਤੀ);
	ਜੇ (!cells)
		ਵਾਪਸ;

	cells_ਗਿਣਤੀ = cells_ਗਿਣਤੀ / ਆਕਾਰ(u32);
	ਜੇ (!cells_ਗਿਣਤੀ)
		ਵਾਪਸ;

	ਲਈ (i = 0; i < (cells_ਗਿਣਤੀ / 2); i++) {
		ਜੇ (fdt32_ਮੰਜ਼ਿਲ_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_ਮੰਜ਼ਿਲ_fdt32(0xffffffff);
	}
}

ਸਥਿਰ ਪੂਰਨ fdt_resv_ਸਮ੍ਰਿਤੀ_update_ਗੰਢ(ਖਾਲੀ *fdt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ, ਪੂਰਨ ਸੂਚਕਾਂਕ_2,
				       ਪੂਰਨ ਮਾਪਾ)
{
	ਪੂਰਨ na = fdt_ਪਤਾ_cells(fdt, 0);
	ਪੂਰਨ ns = fdt_ਮਾਪ_cells(fdt, 0);
	fdt32_t addr_ਉੱਚਾ, addr_ਨੀਵਾਂ;
	fdt32_t ਮਾਪ_ਉੱਚਾ, ਮਾਪ_ਨੀਵਾਂ;
	ਪੂਰਨ subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	ਅੱਖਰ name[32];

	addr_ਉੱਚਾ = (u64)addr >> 32;
	addr_ਨੀਵਾਂ = addr;
	ਮਾਪ_ਉੱਚਾ = (u64)ਮਾਪ >> 32;
	ਮਾਪ_ਨੀਵਾਂ = ਮਾਪ;

	ਜੇ (na > 1 && addr_ਉੱਚਾ)
		sbi_snprintf(name, ਆਕਾਰ(name),
			     "mmode_resv%d@%x,%x", ਸੂਚਕਾਂਕ_2,
			     addr_ਉੱਚਾ, addr_ਨੀਵਾਂ);
	ਨਹੀਂਤਾਂ
		sbi_snprintf(name, ਆਕਾਰ(name),
			     "mmode_resv%d@%x", ਸੂਚਕਾਂਕ_2,
			     addr_ਨੀਵਾਂ);

	subnode = fdt_ਜੋੜੋ_subnode(fdt, ਮਾਪਾ, name);
	ਜੇ (subnode < 0)
		ਵਾਪਸ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_ਖਾਲੀ_ਥਾਵਾਂ(fdt, subnode, "no-map");
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	/* encode the <reg> property value */
	val = reg;
	ਜੇ (na > 1)
		*val++ = cpu_ਮੰਜ਼ਿਲ_fdt32(addr_ਉੱਚਾ);
	*val++ = cpu_ਮੰਜ਼ਿਲ_fdt32(addr_ਨੀਵਾਂ);
	ਜੇ (ns > 1)
		*val++ = cpu_ਮੰਜ਼ਿਲ_fdt32(ਮਾਪ_ਉੱਚਾ);
	*val++ = cpu_ਮੰਜ਼ਿਲ_fdt32(ਮਾਪ_ਨੀਵਾਂ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * ਆਕਾਰ(fdt32_t));
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	ਵਾਪਸ 0;
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
ਪੂਰਨ fdt_reserved_ਸਮ੍ਰਿਤੀ_fixup(ਖਾਲੀ *fdt)
{
	ਬਣਤਰ sbi_domain_memregion *reg;
	ਬਣਤਰ sbi_domain *dom = sbi_domain_thishart_ptr();
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ filtered_ਅਧਾਰ[PMP_ਗਿਣਤੀ] = { 0 };
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ filtered_ਕ੍ਰਮ[PMP_ਗਿਣਤੀ] = { 0 };
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, ਮਾਪ;
	ਪੂਰਨ err, ਮਾਪਾ, i, j;
	ਪੂਰਨ na = fdt_ਪਤਾ_cells(fdt, 0);
	ਪੂਰਨ ns = fdt_ਮਾਪ_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ਜੇ (err < 0)
		ਵਾਪਸ err;

	/* try to locate the reserved memory node */
	ਮਾਪਾ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/reserved-memory");
	ਜੇ (ਮਾਪਾ < 0) {
		/* if such node does not exist, create one */
		ਮਾਪਾ = fdt_ਜੋੜੋ_subnode(fdt, 0, "reserved-memory");
		ਜੇ (ਮਾਪਾ < 0)
			ਵਾਪਸ ਮਾਪਾ;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_ਖਾਲੀ_ਥਾਵਾਂ(fdt, ਮਾਪਾ, "ranges");
		ਜੇ (err < 0)
			ਵਾਪਸ err;

		err = fdt_setprop_u32(fdt, ਮਾਪਾ, "#size-cells", ns);
		ਜੇ (err < 0)
			ਵਾਪਸ err;

		err = fdt_setprop_u32(fdt, ਮਾਪਾ, "#address-cells", na);
		ਜੇ (err < 0)
			ਵਾਪਸ err;
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
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ਜਾਰੀ;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ਜਾਰੀ;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ਜਾਰੀ;
		ਜੇ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ਜਾਰੀ;

		ਜੇ (i >= PMP_ਗਿਣਤੀ) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ਵਾਪਸ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->ਅਧਾਰ_2;
		ਲਈ (j = 0; j < i; j++) {
			ਜੇ (addr == filtered_ਅਧਾਰ[j]
			    && filtered_ਕ੍ਰਮ[j] < reg->ਕ੍ਰਮ) {
				overlap = true;
				filtered_ਕ੍ਰਮ[j] = reg->ਕ੍ਰਮ;
				ਰੋਕੋ;
			}
		}

		ਜੇ (!overlap) {
			filtered_ਅਧਾਰ[i] = reg->ਅਧਾਰ_2;
			filtered_ਕ੍ਰਮ[i] = reg->ਕ੍ਰਮ;
			i++;
		}
	}

	ਲਈ (j = 0; j < i; j++) {
		addr = filtered_ਅਧਾਰ[j];
		ਮਾਪ = 1UL << filtered_ਕ੍ਰਮ[j];
		fdt_resv_ਸਮ੍ਰਿਤੀ_update_ਗੰਢ(fdt, addr, ਮਾਪ, j, ਮਾਪਾ);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ fdt_config_fixup(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ chosen_ਵਿੱਥ, config_ਵਿੱਥ;

	chosen_ਵਿੱਥ = fdt_ਰਸਤਾ_ਵਿੱਥ(fdt, "/chosen");
	ਜੇ (chosen_ਵਿੱਥ < 0)
		ਵਾਪਸ;

	config_ਵਿੱਥ = fdt_ਗੰਢ_ਵਿੱਥ_by_compatible(fdt, chosen_ਵਿੱਥ, "opensbi,config");
	ਜੇ (config_ਵਿੱਥ < 0)
		ਵਾਪਸ;

	fdt_nop_ਗੰਢ(fdt, config_ਵਿੱਥ);
}

ਸਥਿਰ SBI_LIST_ਸਿਰ(fixup_list);

ਪੂਰਨ fdt_ਰਜਿਸਟਰ_general_fixup(ਬਣਤਰ fdt_general_fixup *fixup)
{
	ਬਣਤਰ fdt_general_fixup *f;

	ਜੇ (!fixup || !fixup->name || !fixup->do_fixup)
		ਵਾਪਸ SBI_EINVAL;

	sbi_list_for_each_ਇੰਦਰਾਜ(f, &fixup_list, ਸਿਰ) {
		ਜੇ (f == fixup)
			ਵਾਪਸ SBI_EALREADY;
	}

	sbi_list_ਜੋੜੋ_ਪੂਛ(&fixup->ਸਿਰ, &fixup_list);

	ਵਾਪਸ 0;
}

ਖਾਲੀ fdt_unregister_general_fixup(ਬਣਤਰ fdt_general_fixup *fixup)
{
	ਜੇ (!fixup)
		ਵਾਪਸ;

	sbi_list_del(&fixup->ਸਿਰ);
}

ਖਾਲੀ fdt_fixups(ਖਾਲੀ *fdt)
{
	ਬਣਤਰ fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_ਸਮ੍ਰਿਤੀ_fixup(fdt);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_FDT_FIXUPS_PRESERVE_PMU_ਗੰਢ
	fdt_pmu_fixup(fdt);
#ਜੇ_ਅੰਤ

	fdt_config_fixup(fdt);

	sbi_list_for_each_ਇੰਦਰਾਜ(f, &fixup_list, ਸਿਰ)
		f->do_fixup(f, fdt);
}
