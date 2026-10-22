
// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_fixup.c - Flat Device Tree parsing helper routines
 * Implement helper routines to parse FDT nodes on top of
 * libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#include <libfdt.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_pmu.h>
#include <sbi_utils/fdt/fdt_helper.h>

số_nguyên fdt_add_cpu_idle_states(void *fdt, const struct sbi_cpu_idle_state *state)
{
	số_nguyên cpu_node, cpus_node, err, idle_states_node;
	uint32_t count, phandle;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	nếu (err < 0)
		trả_về err;

	err = fdt_find_max_phandle(fdt, &phandle);
	phandle++;
	nếu (err < 0)
		trả_về err;

	cpus_node = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_node < 0)
		trả_về cpus_node;

	/* Do nothing if the idle-states node already exists. */
	idle_states_node = fdt_subnode_offset(fdt, cpus_node, "idle-states");
	nếu (idle_states_node >= 0)
		trả_về 0;

	/* Create the idle-states node and its child nodes. */
	idle_states_node = fdt_add_subnode(fdt, cpus_node, "idle-states");
	nếu (idle_states_node < 0)
		trả_về idle_states_node;

	cho (count = 0; state->name; count++, phandle++, state++) {
		số_nguyên idle_state_node;

		idle_state_node = fdt_add_subnode(fdt, idle_states_node,
						  state->name);
		nếu (idle_state_node < 0)
			trả_về idle_state_node;

		fdt_setprop_string(fdt, idle_state_node, "compatible",
				   "riscv,idle-state");
		fdt_setprop_u32(fdt, idle_state_node,
				"riscv,sbi-suspend-param",
				state->suspend_param);
		nếu (state->local_timer_stop)
			fdt_setprop_empty(fdt, idle_state_node,
					  "local-timer-stop");
		fdt_setprop_u32(fdt, idle_state_node, "entry-latency-us",
				state->entry_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "exit-latency-us",
				state->exit_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "min-residency-us",
				state->min_residency_us);
		nếu (state->wakeup_latency_us)
			fdt_setprop_u32(fdt, idle_state_node,
					"wakeup-latency-us",
					state->wakeup_latency_us);
		fdt_setprop_u32(fdt, idle_state_node, "phandle", phandle);
	}

	nếu (count == 0)
		trả_về 0;

	/* Link each cpu node to the idle state nodes. */
	fdt_for_each_subnode(cpu_node, fdt, cpus_node) {
		const char *device_type;
		fdt32_t *value;

		/* Only process child nodes with device_type = "cpu". */
		device_type = fdt_getprop(fdt, cpu_node, "device_type", NULL);
		nếu (!device_type || strcmp(device_type, "cpu"))
			continue;

		/* Allocate space for the list of phandles. */
		err = fdt_setprop_placeholder(fdt, cpu_node, "cpu-idle-states",
					      count * sizeof(phandle),
					      (void **)&value);
		nếu (err < 0)
			trả_về err;

		/* Fill in the phandles of the idle state nodes. */
		cho (uint32_t i = 0; i < count; ++i)
			value[i] = cpu_to_fdt32(phandle - count + i);
	}

	trả_về 0;
}

/**
 * Report which fixups a HART DT node needs. Read-only, so the sizing pass
 * and the fixup pass below stay in sync by construction.
 */
static void fdt_cpu_fixup_needed(const void *fdt, số_nguyên cpu_offset,
				 bool emulated_zicntr, bool *disable,
				 bool *add_zicntr)
{
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	const char *mmu_type, *extensions;
	u32 hartid, hartindex;
	số_nguyên len;

	*disable = false;
	*add_zicntr = false;

	nếu (fdt_parse_hart_id(fdt, cpu_offset, &hartid))
		trả_về;

	nếu (!fdt_node_is_enabled(fdt, cpu_offset))
		trả_về;

	/*
	 * Disable a HART DT node if one of the following is true:
	 * 1. The HART is not assigned to the current domain
	 * 2. MMU is not available for the HART
	 */

	hartindex = sbi_hartid_to_hartindex(hartid);
	mmu_type = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
	nếu (!sbi_domain_is_assigned_hart(dom, hartindex) || !mmu_type || !len)
		*disable = true;

	nếu (!emulated_zicntr)
		trả_về;

	extensions = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
	/*
	 * For legacy devicetrees, don't create riscv,isa-extensions
	 * property if there hasn't been already one.
	 */
	nếu (extensions && !fdt_stringlist_contains(extensions, len, "zicntr"))
		*add_zicntr = true;
}

void fdt_cpu_fixup(void *fdt)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	bool emulated_zicntr, disable, add_zicntr;
	số_nguyên err, cpu_offset, cpus_offset;
	số_nguyên reserve = 0;
	/*
	 * A new "status" property costs a header, its tag-aligned value and its
	 * name; appending "zicntr" only grows an existing tag-aligned value.
	 */
	const số_nguyên status_size = sizeof(struct fdt_property) +
				ROUNDUP(sizeof("disabled"), FDT_TAGSIZE) +
				sizeof("status");
	const số_nguyên zicntr_size = ROUNDUP(sizeof("zicntr"), FDT_TAGSIZE);

	/*
	 * Claim Zicntr extension in riscv,isa-extensions if
	 *  1. OpenSBI can emulate time CSR with a timer
	 *  2. The other two CSRs specified by Zicntr are available
	 */
	emulated_zicntr = sbi_timer_get_device() != NULL &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_CYCLE) &&
			  sbi_hart_has_csr(scratch, SBI_HART_CSR_INSTRET);

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về;

	/*
	 * fdt_open_into() trusts the size it is given, so reserve only what is
	 * really needed instead of a fixed worst case for every possible HART.
	 */
	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);
		nếu (disable)
			reserve += status_size;
		nếu (add_zicntr)
			reserve += zicntr_size;
	}

	nếu (!reserve)
		trả_về;

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + reserve);
	nếu (err < 0)
		trả_về;

	/* fdt_open_into() may have reordered the blocks, so look up again. */
	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		fdt_cpu_fixup_needed(fdt, cpu_offset, emulated_zicntr,
				     &disable, &add_zicntr);

		nếu (disable) {
			err = fdt_setprop_string(fdt, cpu_offset, "status",
						 "disabled");
			nếu (err)
				sbi_printf("%s: failed to disable %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, NULL),
					   err);
		}

		nếu (add_zicntr) {
			err = fdt_appendprop_string(fdt, cpu_offset,
						    "riscv,isa-extensions",
						    "zicntr");
			nếu (err)
				sbi_printf("%s: failed to add zicntr to %s (%d)\n",
					   __func__,
					   fdt_get_name(fdt, cpu_offset, NULL),
					   err);
		}
	}
}

static void fdt_domain_based_fixup_one(void *fdt, số_nguyên nodeoff)
{
	số_nguyên rc;
	uint64_t reg_addr, reg_size;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_addr || !reg_size)
		trả_về;

	nếu (!sbi_domain_check_addr(dom, reg_addr, dom->next_mode,
				    SBI_DOMAIN_READ | SBI_DOMAIN_WRITE | SBI_DOMAIN_MMIO)) {
		rc = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 32);
		nếu (rc < 0)
			trả_về;
		fdt_setprop_string(fdt, nodeoff, "status", "disabled");
	}
}

static void fdt_fixup_node(void *fdt, const char *compatible)
{
	số_nguyên noff = 0;

	trong_khi ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     compatible)) >= 0)
		fdt_domain_based_fixup_one(fdt, noff);
}

void fdt_aplic_fixup(void *fdt)
{
	fdt_fixup_node(fdt, "riscv,aplic");
}

void fdt_imsic_fixup(void *fdt)
{
	fdt_fixup_node(fdt, "riscv,imsics");
}

void fdt_plic_fixup(void *fdt)
{
	u32 *cells;
	số_nguyên i, cells_count;
	số_nguyên plic_off;

	plic_off = fdt_node_offset_by_compatible(fdt, 0, "sifive,plic-1.0.0");
	nếu (plic_off < 0) {
		plic_off = fdt_node_offset_by_compatible(fdt, 0, "riscv,plic0");
		nếu (plic_off < 0)
			trả_về;
	}

	cells = (u32 *)fdt_getprop(fdt, plic_off,
				   "interrupts-extended", &cells_count);
	nếu (!cells)
		trả_về;

	cells_count = cells_count / sizeof(u32);
	nếu (!cells_count)
		trả_về;

	cho (i = 0; i < (cells_count / 2); i++) {
		nếu (fdt32_to_cpu(cells[2 * i + 1]) == IRQ_M_EXT)
			cells[2 * i + 1] = cpu_to_fdt32(0xffffffff);
	}
}

static số_nguyên fdt_resv_memory_update_node(void *fdt, unsigned long addr,
				       unsigned long size, số_nguyên index,
				       số_nguyên parent)
{
	số_nguyên na = fdt_address_cells(fdt, 0);
	số_nguyên ns = fdt_size_cells(fdt, 0);
	fdt32_t addr_high, addr_low;
	fdt32_t size_high, size_low;
	số_nguyên subnode, err;
	fdt32_t reg[4];
	fdt32_t *val;
	char name[32];

	addr_high = (u64)addr >> 32;
	addr_low = addr;
	size_high = (u64)size >> 32;
	size_low = size;

	nếu (na > 1 && addr_high)
		sbi_snprintf(name, sizeof(name),
			     "mmode_resv%d@%x,%x", index,
			     addr_high, addr_low);
	không_thì
		sbi_snprintf(name, sizeof(name),
			     "mmode_resv%d@%x", index,
			     addr_low);

	subnode = fdt_add_subnode(fdt, parent, name);
	nếu (subnode < 0)
		trả_về subnode;

	/*
	 * Tell operating system not to create a virtual
	 * mapping of the region as part of its standard
	 * mapping of system memory.
	 */
	err = fdt_setprop_empty(fdt, subnode, "no-map");
	nếu (err < 0)
		trả_về err;

	/* encode the <reg> property value */
	val = reg;
	nếu (na > 1)
		*val++ = cpu_to_fdt32(addr_high);
	*val++ = cpu_to_fdt32(addr_low);
	nếu (ns > 1)
		*val++ = cpu_to_fdt32(size_high);
	*val++ = cpu_to_fdt32(size_low);

	err = fdt_setprop(fdt, subnode, "reg", reg,
			  (na + ns) * sizeof(fdt32_t));
	nếu (err < 0)
		trả_về err;

	trả_về 0;
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
số_nguyên fdt_reserved_memory_fixup(void *fdt)
{
	struct sbi_domain_memregion *reg;
	struct sbi_domain *dom = sbi_domain_thishart_ptr();
	unsigned long filtered_base[PMP_COUNT] = { 0 };
	unsigned char filtered_order[PMP_COUNT] = { 0 };
	unsigned long addr, size;
	số_nguyên err, parent, i, j;
	số_nguyên na = fdt_address_cells(fdt, 0);
	số_nguyên ns = fdt_size_cells(fdt, 0);

	/*
	 * Expand the device tree to accommodate new node
	 * by the following estimated size:
	 *
	 * Each PMP memory region entry occupies 64 bytes.
	 * With 16 PMP memory regions we need 64 * 16 = 1024 bytes.
	 */
	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + 1024);
	nếu (err < 0)
		trả_về err;

	/* try to locate the reserved memory node */
	parent = fdt_path_offset(fdt, "/reserved-memory");
	nếu (parent < 0) {
		/* if such node does not exist, create one */
		parent = fdt_add_subnode(fdt, 0, "reserved-memory");
		nếu (parent < 0)
			trả_về parent;

		/*
		 * reserved-memory node has 3 required properties:
		 * - #address-cells: the same value as the root node
		 * - #size-cells: the same value as the root node
		 * - ranges: should be empty
		 */

		err = fdt_setprop_empty(fdt, parent, "ranges");
		nếu (err < 0)
			trả_về err;

		err = fdt_setprop_u32(fdt, parent, "#size-cells", ns);
		nếu (err < 0)
			trả_về err;

		err = fdt_setprop_u32(fdt, parent, "#address-cells", na);
		nếu (err < 0)
			trả_về err;
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
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_MMIO)
			continue;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_READABLE)
			continue;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_WRITABLE)
			continue;
		nếu (reg->flags & SBI_DOMAIN_MEMREGION_SU_EXECUTABLE)
			continue;

		nếu (i >= PMP_COUNT) {
			sbi_printf("%s: Too many memory regions to fixup.\n",
				   __func__);
			trả_về SBI_ENOSPC;
		}

		bool overlap = false;
		addr = reg->base;
		cho (j = 0; j < i; j++) {
			nếu (addr == filtered_base[j]
			    && filtered_order[j] < reg->order) {
				overlap = true;
				filtered_order[j] = reg->order;
				break;
			}
		}

		nếu (!overlap) {
			filtered_base[i] = reg->base;
			filtered_order[i] = reg->order;
			i++;
		}
	}

	cho (j = 0; j < i; j++) {
		addr = filtered_base[j];
		size = 1UL << filtered_order[j];
		fdt_resv_memory_update_node(fdt, addr, size, j, parent);
	}

	trả_về 0;
}

void fdt_config_fixup(void *fdt)
{
	số_nguyên chosen_offset, config_offset;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	nếu (chosen_offset < 0)
		trả_về;

	config_offset = fdt_node_offset_by_compatible(fdt, chosen_offset, "opensbi,config");
	nếu (config_offset < 0)
		trả_về;

	fdt_nop_node(fdt, config_offset);
}

static SBI_LIST_HEAD(fixup_list);

số_nguyên fdt_register_general_fixup(struct fdt_general_fixup *fixup)
{
	struct fdt_general_fixup *f;

	nếu (!fixup || !fixup->name || !fixup->do_fixup)
		trả_về SBI_EINVAL;

	sbi_list_for_each_entry(f, &fixup_list, head) {
		nếu (f == fixup)
			trả_về SBI_EALREADY;
	}

	sbi_list_add_tail(&fixup->head, &fixup_list);

	trả_về 0;
}

void fdt_unregister_general_fixup(struct fdt_general_fixup *fixup)
{
	nếu (!fixup)
		trả_về;

	sbi_list_del(&fixup->head);
}

void fdt_fixups(void *fdt)
{
	struct fdt_general_fixup *f;

	fdt_aplic_fixup(fdt);

	fdt_imsic_fixup(fdt);

	fdt_plic_fixup(fdt);

	fdt_reserved_memory_fixup(fdt);

#ifndef CONFIG_FDT_FIXUPS_PRESERVE_PMU_NODE
	fdt_pmu_fixup(fdt);
#endif

	fdt_config_fixup(fdt);

	sbi_list_for_each_entry(f, &fixup_list, head)
		f->do_fixup(f, fdt);
}
