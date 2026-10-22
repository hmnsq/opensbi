
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_math.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_fixup.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_pmu.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>

પૂર્ણાંક fdt_add_cpu_idle_states(શૂન્ય *fdt, અચલ સંરચના sbi_cpu_idle_state *state)
{
	પૂર્ણાંક cpu_ગાંઠ, cpus_ગાંઠ, err, idle_states_ગાંઠ;
	uint32_t ગણતરી, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	જો (err < 0)
		પાછા err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	જો (err < 0)
		પાછા err;

	cpus_ગાંઠ = fdt_path_offset(fdt, "/cpus");
	જો (cpus_ગાંઠ < 0)
		પાછા cpus_ગાંઠ;

	/* Do nothing if the idle-states node already exists. */
	idle_states_ગાંઠ = fdt_subnode_offset(fdt, cpus_ગાંઠ, "idle-states");
	જો (idle_states_ગાંઠ >= 0)
		પાછા 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_ગાંઠ = fdt_add_subnode(fdt, cpus_ગાંઠ, "idle-states");
	જો (idle_states_ગાંઠ < 0)
		પાછા idle_states_ગાંઠ;

	માટે (ગણતરી = 0; state->name; ગણતરી++, phandle++, state++) {
		પૂર્ણાંક idle_state_ગાંઠ;

		idle_state_ગાંઠ = fdt_add_subnode(fdt, idle_states_ગાંઠ,
						  state->name);
		જો (idle_state_ગાંઠ < 0)
			પાછા idle_state_ગાંઠ;

		fdt_setprop_string(fdt, idle_state_ગાંઠ, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_ગાંઠ,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		જો (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_ગાંઠ,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_ગાંઠ, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_ગાંઠ, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_ગાંઠ, "min-residency-us",
				state->min_residency_us);
		જો (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_ગાંઠ,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_ગાંઠ, "phandle", phandle);
	}

	જો (ગણતરી == 0)
		પાછા 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_ગાંઠ, fdt, cpus_ગાંઠ) {
		અચલ અક્ષર *device_type;
		fdt32_t *કિંમત;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_ગાંઠ, "device_type", ખાલી);
		જો (!device_type || strcmp(device_type, "cpu"))
			ચાલુ;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_ગાંઠ, "cpu-idle-states",
					      ગણતરી * કદ(phandle),
					      (શૂન્ય **)&કિંમત);
		જો (err < 0)
			પાછા err;

		/* Fill in the phandles of the idle state nodes. */
		માટે (uint32_t i = 0; i < ગણતરી; ++i)
			કિંમત[i] = cpu_to_fdt32(phandle - ગણતરી + i);
	}

	પાછા 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
સ્થિર શૂન્ય fdt_cpu_fixup_needed(અચલ શૂન્ય *fdt, પૂર્ણાંક cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	અચલ અક્ષર *mmu_type, *extensions;
	u32 hartid, hartindex;
	પૂર્ણાંક len;

	*disable = false;
	*add_zicntr = false;

	જો (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		પાછા;

	જો (!fdt_ગાંઠ_is_enabled(fdt, cpu_offset))
		પાછા;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	જો (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	જો (!emulated_zicntr)
		પાછા;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	જો (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

શૂન્ય fdt_cpu_fixup(શૂન્ય *fdt)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	પૂર્ણાંક err, cpu_offset, cpus_offset;
	પૂર્ણાંક reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	અચલ પૂર્ણાંક status_size = કદ(સંરચના fdt_property) +
				ROUNDUP(કદ("disabled"), FDT_TAGSIZE) +
				કદ("status");
	અચલ પૂર્ણાંક zicntr_size = ROUNDUP(કદ("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != ખાલી &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		જો (disable)
			reserve += status_size;
		જો (add_zicntr)
			reserve += zicntr_size;
	}

	જો (!reserve)
		પાછા;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	જો (err < 0)
		પાછા;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	જો (cpus_offset < 0)
		પાછા;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		જો (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			જો (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ખાલી),
					   err);
		}

		જો (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			જો (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, ખાલી),
					   err);
		}
	}
}

સ્થિર શૂન્ય fdt_domain_based_fixup_one(શૂન્ય *fdt, પૂર્ણાંક nodeoff)
{
	પૂર્ણાંક rc;
	uint64_t reg_addr, reg_size;
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	જો (rc < 0 || !reg_addr || !reg_size)
		પાછા;

	જો (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		જો (rc < 0)
			પાછા;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

સ્થિર શૂન્ય fdt_fixup_ગાંઠ(શૂન્ય *fdt, અચલ અક્ષર *compatible)
{
	પૂર્ણાંક noff = 0;

	જ્યારે ((noff = fdt_ગાંઠ_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

શૂન્ય fdt_aplic_fixup(શૂન્ય *fdt)
{
	fdt_fixup_ગાંઠ(fdt, "riscv,aplic");
}

શૂન્ય fdt_imsic_fixup(શૂન્ય *fdt)
{
	fdt_fixup_ગાંઠ(fdt, "riscv,imsics");
}

શૂન્ય fdt_plic_fixup(શૂન્ય *fdt)
{
	u32 *cells;
	પૂર્ણાંક i, cells_ગણતરી;
	પૂર્ણાંક plic_off;

	plic_off = fdt_ગાંઠ_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	જો (plic_off < 0) {
		plic_off = fdt_ગાંઠ_offset_by_compatible(fdt, 0, "riscv,plic0");
		જો (plic_off < 0)
			પાછા;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_ગણતરી);
	જો (!cells)
		પાછા;

	cells_ગણતરી = cells_ગણતરી / કદ(u32);
	જો (!cells_ગણતરી)
		પાછા;

	માટે (i = 0; i < (cells_ગણતરી / 2); i++) {
		જો (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

સ્થિર પૂર્ણાંક fdt_resv_memory_update_ગાંઠ(શૂન્ય *fdt, unsigned long addr,
				       unsigned long size, પૂર્ણાંક index,
				       પૂર્ણાંક parent)
{
	પૂર્ણાંક na = fdt_address_cells(fdt, 0);
	પૂર્ણાંક ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_ઉપલું, addr_નીચલું;
	fdt32_t size_ઉપલું, size_નીચલું;
	પૂર્ણાંક subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	અક્ષર name[32];

	addr_ઉપલું = (u64)addr >> 32;
	addr_નીચલું = addr;
	size_ઉપલું = (u64)size >> 32;
	size_નીચલું = size;

	જો (na > 1 && addr_ઉપલું)
		sbi_snprintf(name, કદ(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_ઉપલું, addr_નીચલું);
	નહીંતર
		sbi_snprintf(name, કદ(name),
			     "mmode_resv%d@%x", index,
			     addr_નીચલું);

	subnode = fdt_add_subnode(fdt, parent, name);
	જો (subnode < 0)
		પાછા subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	જો (err < 0)
		પાછા err;

	/* encode the <reg> property value */
	val = reg;
	જો (na > 1)
		*val++ = cpu_to_fdt32(addr_ઉપલું);
	*val++ = cpu_to_fdt32(addr_નીચલું);
	જો (ns > 1)
		*val++ = cpu_to_fdt32(size_ઉપલું);
	*val++ = cpu_to_fdt32(size_નીચલું);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * કદ(fdt32_t));
	જો (err < 0)
		પાછા err;

	પાછા 0;
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
પૂર્ણાંક fdt_reserved_memory_fixup(શૂન્ય *fdt)
{
	સંરચના sbi_domain_memregion *reg;
	સંરચના sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_ગણતરી] = { 0 };
	unsigned અક્ષર filtered_order[PMP_ગણતરી] = { 0 };
	unsigned long addr, size;
	પૂર્ણાંક err, parent, i, j;
	પૂર્ણાંક na = fdt_address_cells(fdt, 0);
	પૂર્ણાંક ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	જો (err < 0)
		પાછા err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	જો (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		જો (parent < 0)
			પાછા parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		જો (err < 0)
			પાછા err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		જો (err < 0)
			પાછા err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		જો (err < 0)
			પાછા err;
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
		જો (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			ચાલુ;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			ચાલુ;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			ચાલુ;
		જો (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			ચાલુ;

		જો (i >= PMP_ગણતરી) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			પાછા SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		માટે (j = 0; j < i; j++) {
			જો (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				તોડો;
			}
		}

		જો (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	માટે (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_ગાંઠ(fdt, addr, size, j, parent);
	}

	પાછા 0;
}

શૂન્ય fdt_config_fixup(શૂન્ય *fdt)
{
	પૂર્ણાંક chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	જો (chosen_offset < 0)
		પાછા;

	config_offset = fdt_ગાંઠ_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	જો (config_offset < 0)
		પાછા;

	fdt_nop_ગાંઠ(fdt, config_offset);
}

સ્થિર SBI_LIST_HEAD(fixup_list);

પૂર્ણાંક fdt_register_general_fixup(સંરચના fdt_general_fixup *fixup)
{
	સંરચના fdt_general_fixup *f;

	જો (!fixup || !fixup->name || !fixup->do_fixup)
		પાછા SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		જો (f == fixup)
			પાછા SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	પાછા 0;
}

શૂન્ય fdt_unregister_general_fixup(સંરચના fdt_general_fixup *fixup)
{
	જો (!fixup)
		પાછા;

	sbi_list_del(&fixup->head);
}

શૂન્ય fdt_fixups(શૂન્ય *fdt)
{
	સંરચના fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_ગાંઠ
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
