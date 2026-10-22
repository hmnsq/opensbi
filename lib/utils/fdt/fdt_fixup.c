
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

ಪೂರ್ಣಾಙ್ಕ fdt_add_cpu_idle_states(ಶೂನ್ಯ *fdt, ಧ್ರುವ ಸಂರಚನಾ sbi_cpu_idle_state *state)
{
	ಪೂರ್ಣಾಙ್ಕ cpu_ಗಂಟು, cpus_ಗಂಟು, err, idle_states_ಗಂಟು;
	uint32_t ಎಣಿಕೆ, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	cpus_ಗಂಟು = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_ಗಂಟು < 0)
		ಪ್ರತ್ಯಾಗಮ cpus_ಗಂಟು;

	/* Do nothing if the idle-states node already exists. */
	idle_states_ಗಂಟು = fdt_subnode_offset(fdt, cpus_ಗಂಟು, "idle-states");
	ಯದಿ (idle_states_ಗಂಟು >= 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_ಗಂಟು = fdt_add_subnode(fdt, cpus_ಗಂಟು, "idle-states");
	ಯದಿ (idle_states_ಗಂಟು < 0)
		ಪ್ರತ್ಯಾಗಮ idle_states_ಗಂಟು;

	ಪ್ರತಿ (ಎಣಿಕೆ = 0; state->name; ಎಣಿಕೆ++, phandle++, state++) {
		ಪೂರ್ಣಾಙ್ಕ idle_state_ಗಂಟು;

		idle_state_ಗಂಟು = fdt_add_subnode(fdt, idle_states_ಗಂಟು,
						  state->name);
		ಯದಿ (idle_state_ಗಂಟು < 0)
			ಪ್ರತ್ಯಾಗಮ idle_state_ಗಂಟು;

		fdt_setprop_string(fdt, idle_state_ಗಂಟು, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		ಯದಿ (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_ಗಂಟು,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "min-residency-us",
				state->min_residency_us);
		ಯದಿ (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ಗಂಟು,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ಗಂಟು, "phandle", phandle);
	}

	ಯದಿ (ಎಣಿಕೆ == 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ಗಂಟು, fdt, cpus_ಗಂಟು) {
		ಧ್ರುವ ವರ್ಣ *device_type;
		fdt32_t *ಮೌಲ್ಯ;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ಗಂಟು, "device_type", ರಿಕ್ತ);
		ಯದಿ (!device_type || strcmp(device_type, "cpu"))
			ಅನುವರ್ತ;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ಗಂಟು, "cpu-idle-states",
					      ಎಣಿಕೆ * ಪರಿಮಾಣ(phandle),
					      (ಶೂನ್ಯ **)&ಮೌಲ್ಯ);
		ಯದಿ (err < 0)
			ಪ್ರತ್ಯಾಗಮ err;

		/* Fill in the phandles of the idle state nodes. */
		ಪ್ರತಿ (uint32_t i = 0; i < ಎಣಿಕೆ; ++i)
			ಮೌಲ್ಯ[i] = cpu_to_fdt32(phandle - ಎಣಿಕೆ + i);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_cpu_fixup_needed(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	ಧ್ರುವ ವರ್ಣ *mmu_type, *extensions;
	u32 hartid, hartindex;
	ಪೂರ್ಣಾಙ್ಕ len;

	*disable = false;
	*add_zicntr = false;

	ಯದಿ (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (!fdt_ಗಂಟು_is_enabled(fdt, cpu_offset))
		ಪ್ರತ್ಯಾಗಮ;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	ಯದಿ (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	ಯದಿ (!emulated_zicntr)
		ಪ್ರತ್ಯಾಗಮ;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	ಯದಿ (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

ಶೂನ್ಯ fdt_cpu_fixup(ಶೂನ್ಯ *fdt)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	ಪೂರ್ಣಾಙ್ಕ err, cpu_offset, cpus_offset;
	ಪೂರ್ಣಾಙ್ಕ reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ status_size = ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_property) +
				ROUNDUP(ಪರಿಮಾಣ("disabled"), FDT_TAGSIZE) +
				ಪರಿಮಾಣ("status");
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ zicntr_size = ROUNDUP(ಪರಿಮಾಣ("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ರಿಕ್ತ &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		ಯದಿ (disable)
			reserve += status_size;
		ಯದಿ (add_zicntr)
			reserve += zicntr_size;
	}

	ಯದಿ (!reserve)
		ಪ್ರತ್ಯಾಗಮ;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ಯದಿ (cpus_offset < 0)
		ಪ್ರತ್ಯಾಗಮ;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		ಯದಿ (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			ಯದಿ (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ರಿಕ್ತ),
					   err);
		}

		ಯದಿ (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			ಯದಿ (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ರಿಕ್ತ),
					   err);
		}
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_domain_based_fixup_one(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	uint64_t reg_addr, reg_size;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ಯದಿ (rc < 0 || !reg_addr || !reg_size)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		ಯದಿ (rc < 0)
			ಪ್ರತ್ಯಾಗಮ;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_fixup_ಗಂಟು(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *compatible)
{
	ಪೂರ್ಣಾಙ್ಕ noff = 0;

	ಯಾವತ್ ((noff = fdt_ಗಂಟು_offset_by_compatible(fdt, noff,
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
	ಪೂರ್ಣಾಙ್ಕ i, cells_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ plic_off;

	plic_off = fdt_ಗಂಟು_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	ಯದಿ (plic_off < 0) {
		plic_off = fdt_ಗಂಟು_offset_by_compatible(fdt, 0, "riscv,plic0");
		ಯದಿ (plic_off < 0)
			ಪ್ರತ್ಯಾಗಮ;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ಎಣಿಕೆ);
	ಯದಿ (!cells)
		ಪ್ರತ್ಯಾಗಮ;

	cells_ಎಣಿಕೆ = cells_ಎಣಿಕೆ / ಪರಿಮಾಣ(u32);
	ಯದಿ (!cells_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ;

	ಪ್ರತಿ (i = 0; i < (cells_ಎಣಿಕೆ / 2); i++) {
		ಯದಿ (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_resv_memory_update_ಗಂಟು(ಶೂನ್ಯ *fdt, unsigned long addr,
				       unsigned long size, ಪೂರ್ಣಾಙ್ಕ index,
				       ಪೂರ್ಣಾಙ್ಕ parent)
{
	ಪೂರ್ಣಾಙ್ಕ na = fdt_address_cells(fdt, 0);
	ಪೂರ್ಣಾಙ್ಕ ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_ಮೇಲಿನ, addr_ಕೆಳಗಿನ;
	fdt32_t size_ಮೇಲಿನ, size_ಕೆಳಗಿನ;
	ಪೂರ್ಣಾಙ್ಕ subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	ವರ್ಣ name[32];

	addr_ಮೇಲಿನ = (u64)addr >> 32;
	addr_ಕೆಳಗಿನ = addr;
	size_ಮೇಲಿನ = (u64)size >> 32;
	size_ಕೆಳಗಿನ = size;

	ಯದಿ (na > 1 && addr_ಮೇಲಿನ)
		sbi_snprintf(name, ಪರಿಮಾಣ(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_ಮೇಲಿನ, addr_ಕೆಳಗಿನ);
	ಅನ್ಯಥಾ
		sbi_snprintf(name, ಪರಿಮಾಣ(name),
			     "mmode_resv%d@%x", index,
			     addr_ಕೆಳಗಿನ);

	subnode = fdt_add_subnode(fdt, parent, name);
	ಯದಿ (subnode < 0)
		ಪ್ರತ್ಯಾಗಮ subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	/* encode the <reg> property value */
	val = reg;
	ಯದಿ (na > 1)
		*val++ = cpu_to_fdt32(addr_ಮೇಲಿನ);
	*val++ = cpu_to_fdt32(addr_ಕೆಳಗಿನ);
	ಯದಿ (ns > 1)
		*val++ = cpu_to_fdt32(size_ಮೇಲಿನ);
	*val++ = cpu_to_fdt32(size_ಕೆಳಗಿನ);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * ಪರಿಮಾಣ(fdt32_t));
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	ಪ್ರತ್ಯಾಗಮ 0;
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
ಪೂರ್ಣಾಙ್ಕ fdt_reserved_memory_fixup(ಶೂನ್ಯ *fdt)
{
	ಸಂರಚನಾ sbi_domain_memregion *reg;
	ಸಂರಚನಾ sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_ಎಣಿಕೆ] = { 0 };
	unsigned ವರ್ಣ filtered_order[PMP_ಎಣಿಕೆ] = { 0 };
	unsigned long addr, size;
	ಪೂರ್ಣಾಙ್ಕ err, parent, i, j;
	ಪೂರ್ಣಾಙ್ಕ na = fdt_address_cells(fdt, 0);
	ಪೂರ್ಣಾಙ್ಕ ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	ಯದಿ (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		ಯದಿ (parent < 0)
			ಪ್ರತ್ಯಾಗಮ parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		ಯದಿ (err < 0)
			ಪ್ರತ್ಯಾಗಮ err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		ಯದಿ (err < 0)
			ಪ್ರತ್ಯಾಗಮ err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		ಯದಿ (err < 0)
			ಪ್ರತ್ಯಾಗಮ err;
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
		ಯದಿ (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ಅನುವರ್ತ;
		ಯದಿ (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ಅನುವರ್ತ;
		ಯದಿ (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ಅನುವರ್ತ;
		ಯದಿ (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ಅನುವರ್ತ;

		ಯದಿ (i >= PMP_ಎಣಿಕೆ) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		ಪ್ರತಿ (j = 0; j < i; j++) {
			ಯದಿ (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				ಭಙ್ಗ;
			}
		}

		ಯದಿ (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	ಪ್ರತಿ (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_ಗಂಟು(fdt, addr, size, j, parent);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ fdt_config_fixup(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	ಯದಿ (chosen_offset < 0)
		ಪ್ರತ್ಯಾಗಮ;

	config_offset = fdt_ಗಂಟು_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	ಯದಿ (config_offset < 0)
		ಪ್ರತ್ಯಾಗಮ;

	fdt_nop_ಗಂಟು(fdt, config_offset);
}

ಸ್ಥಿರ SBI_LIST_HEAD(fixup_list);

ಪೂರ್ಣಾಙ್ಕ fdt_register_general_fixup(ಸಂರಚನಾ fdt_general_fixup *fixup)
{
	ಸಂರಚನಾ fdt_general_fixup *f;

	ಯದಿ (!fixup || !fixup->name || !fixup->do_fixup)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		ಯದಿ (f == fixup)
			ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ fdt_unregister_general_fixup(ಸಂರಚನಾ fdt_general_fixup *fixup)
{
	ಯದಿ (!fixup)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_del(&fixup->head);
}

ಶೂನ್ಯ fdt_fixups(ಶೂನ್ಯ *fdt)
{
	ಸಂರಚನಾ fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_ಗಂಟು
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
