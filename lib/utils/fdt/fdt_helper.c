// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_hart.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/irqchip/aplic.h>
#include <sbi_utils/irqchip/imsic.h>
#include <sbi_utils/irqchip/plic.h>

#define DEFAULT_UART_FREQ		0
#define DEFAULT_UART_BAUD		115200
#define DEFAULT_UART_REG_SHIFT		0
#define DEFAULT_UART_REG_IO_WIDTH	1
#define DEFAULT_UART_REG_OFFSET		0

#define DEFAULT_RENESAS_SCIF_FREQ		100000000
#define DEFAULT_RENESAS_SCIF_BAUD		115200

#define DEFAULT_SIFIVE_UART_FREQ		0
#define DEFAULT_SIFIVE_UART_BAUD		115200

#define DEFAULT_SHAKTI_UART_FREQ		50000000
#define DEFAULT_SHAKTI_UART_BAUD		115200

цео fdt_parse_phandle_with_args(const void *fdt, цео nodeoff,
				const char *prop, const char *cells_prop,
				цео index, struct fdt_phandle_args *out_args)
{
	u32 i, pcells;
	цео len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	ако (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		врати SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ако (!list)
		врати SBI_ENOENT;
	list_end = list + (len / sizeof(*list));

	док (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ако (pnodeoff < 0)
			врати pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ако (!val)
			врати SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ако (FDT_MAX_PHANDLE_ARGS < pcells)
			врати SBI_EINVAL;
		ако (list + pcells > list_end)
			врати SBI_ENOENT;

		ако (index > 0) {
			list += pcells;
			index--;
		} инако {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			за (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			врати 0;
		}
	}

	врати SBI_ENOENT;
}

static цео fdt_translate_address(const void *fdt, uint64_t reg, цео parent,
				 uint64_t *addr)
{
	цео i, rlen;
	цео cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ако (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ако (cell_child_addr < 1)
			врати SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ако (cell_parent_addr < 1)
			врати SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ако (cell_size < 0)
			врати SBI_ENODEV;

		за (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		за (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		за (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ако (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			врати SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} инако {
		/* No translation required */
		*addr = reg;
	}

	врати 0;
}

цео fdt_get_node_addr_size(const void *fdt, цео node, цео index,
			   uint64_t *addr, uint64_t *size)
{
	цео parent, len, i, rc;
	цео cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ако (!fdt || node < 0 || index < 0)
		врати SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	ако (parent < 0)
		врати parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ако (cell_addr < 1)
		врати SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ако (cell_size < 0)
		врати SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	ако (!prop_addr)
		врати SBI_ENODEV;

	ако ((len / sizeof(u32)) <= (index * (cell_addr + cell_size)))
		врати SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ако (addr) {
		за (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			ако (parent < 0)
				break;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ако (rc)
				break;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} док (1);
	}
	temp = 0;

	ако (size) {
		за (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	врати 0;
}

цео fdt_get_node_addr_size_by_name(const void *fdt, цео node, const char *name,
				   uint64_t *addr, uint64_t *size)
{
	цео i, j, count;
	const char *val;
	const char *regname;

	ако (!fdt || node < 0 || !name)
		врати SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	ако (!val)
		врати SBI_ENODEV;

	за (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		ако (strcmp(name, regname) == 0)
			врати fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	врати SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, цео nodeoff)
{
	цео len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ако (!prop)
		врати true;

	ако (!strncmp(prop, "okay", strlen("okay")))
		врати true;

	ако (!strncmp(prop, "ok", strlen("ok")))
		врати true;

	врати false;
}

цео fdt_parse_hart_id(const void *fdt, цео cpu_offset, u32 *hartid)
{
	цео len;
	const void *prop;
	const fdt32_t *val;

	ако (!fdt || cpu_offset < 0)
		врати SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ако (!prop || !len)
		врати SBI_EINVAL;
	ако (strncmp (prop, "cpu", strlen ("cpu")))
		врати SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ако (!val || len < sizeof(fdt32_t))
		врати SBI_EINVAL;

	ако (len > sizeof(fdt32_t))
		val++;

	ако (hartid)
		*hartid = fdt32_to_cpu(*val);

	врати 0;
}

цео fdt_parse_cbom_block_size(const void *fdt, цео cpu_offset, unsigned long *cbom_block_size)
{
	цео len;
	const void *prop;
	const fdt32_t *val;

	ако (!fdt || cpu_offset < 0)
		врати SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ако (!prop || !len)
		врати SBI_EINVAL;
	ако (strncmp (prop, "cpu", strlen ("cpu")))
		врати SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ако (!val || len < sizeof(fdt32_t))
		врати SBI_EINVAL;

	ако (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	врати 0;
}

цео fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	цео err, cpu_offset, cpus_offset;

	ако (!fdt)
		врати SBI_EINVAL;
	ако (!max_hartid)
		врати 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ако (cpus_offset < 0)
		врати cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ако (err)
			continue;

		ако (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		ако (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	врати 0;
}

цео fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	цео len, cpus_offset;

	ако (!fdt || !freq)
		врати SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ако (cpus_offset < 0)
		врати cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ако (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	инако
		врати SBI_ENOENT;

	врати 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static цео fdt_parse_isa_one_hart(const char *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	char mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ако (isa[i] == 'r' || isa[i] == 'R')
		i++;
	инако
		врати SBI_EINVAL;

	ако (isa[i] == 'v' || isa[i] == 'V')
		i++;
	инако
		врати SBI_EINVAL;

	ако (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	инако ако (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	инако
		врати SBI_EINVAL;

	/* Skip base ISA extensions */
	за (; i < isa_len; i++) {
		ако (isa[i] == '_')
			break;
	}

	док (i < isa_len) {
		ако (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		док ((i < isa_len) && (isa[i] != '_') &&
		       (j < (sizeof(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ако (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			ако (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		за (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	врати 0;
}

static void fdt_parse_isa_extensions_one_hart(const char *isa,
					      unsigned long *extensions,
					      цео len)
{
	size_t i;

	за (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ако (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

цео fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	struct sbi_scratch *scratch;
	struct sbi_hart_features *hfeatures;
	цео err, cpu_offset, cpus_offset, len;

	ако (!fdt)
		врати SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ако (cpus_offset < 0)
		врати cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ако (err)
			continue;

		ако (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ако (!scratch)
			врати SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ако (!hfeatures)
			врати SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ако (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const char *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ако (!val || len <= 0)
			врати SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const char *)val, hfeatures->extensions);
		ако (err)
			врати err;
	}

	врати 0;
}

static цео fdt_parse_uart_node_common(const void *fdt, цео nodeoffset,
				      struct platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	цео len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ако (nodeoffset < 0 || !uart || !fdt)
		врати SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		врати SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ако (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	инако
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ако (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	инако
		uart->baud = default_baud;

	врати 0;
}

цео fdt_parse_gaisler_uart_node(const void *fdt, цео nodeoffset,
				struct platform_uart_data *uart)
{
	врати fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

цео fdt_parse_renesas_scif_node(const void *fdt, цео nodeoffset,
				struct platform_uart_data *uart)
{
	врати fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

цео fdt_parse_shakti_uart_node(const void *fdt, цео nodeoffset,
			       struct platform_uart_data *uart)
{
	врати fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

цео fdt_parse_sifive_uart_node(const void *fdt, цео nodeoffset,
			       struct platform_uart_data *uart)
{
	врати fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

цео fdt_parse_uart_node(const void *fdt, цео nodeoffset,
			struct platform_uart_data *uart)
{
	цео len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ако (rc)
		врати rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ако (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	инако
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ако (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	инако
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ако (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	инако
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	врати 0;
}

цео fdt_parse_uart8250(const void *fdt, struct platform_uart_data *uart,
		   const char *compatible)
{
	цео nodeoffset;

	ако (!compatible || !uart || !fdt)
		врати SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ако (nodeoffset < 0)
		врати nodeoffset;

	врати fdt_parse_uart_node(fdt, nodeoffset, uart);
}

цео fdt_parse_xlnx_uartlite_node(const void *fdt, цео nodeoffset,
			       struct platform_uart_data *uart)
{
	врати fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static цео fdt_aplic_find_imsic_node(const void *fdt, цео nodeoff,
				     struct imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	цео i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ако (val && len >= sizeof(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ако (noff < 0)
			врати noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		ако (rc)
			врати rc;

		rc = imsic_data_check(imsic);
		ако (rc)
			врати rc;

		ако (imsic->targets_mmode == mmode) {
			врати 0;
		}
	} инако {
		врати SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ако (!val || len < sizeof(fdt32_t))
		врати SBI_ENODEV;

	len /= sizeof(fdt32_t);

	за (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ако (noff < 0)
			врати noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		ако (!rc)
			break;
	}

	врати rc;
}

цео fdt_parse_aplic_node(const void *fdt, цео nodeoff, struct aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	struct imsic_data imsic = { 0 };
	цео i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	struct aplic_delegate_data *deleg;

	ако (nodeoff < 0 || !aplic || !fdt)
		врати SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		врати SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ако (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ако (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		за (i = 0; i < len; i += 2) {
			ако (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				break;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	ако (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ако (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			врати SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	ако (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ако (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			врати SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	за (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ако (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ако (!del || len < (3 * sizeof(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / sizeof(fdt32_t);
	за (i = 0; i < dcnt; i += 3) {
		ако (d >= APLIC_MAX_DELEGATE)
			break;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ако (!val || len < sizeof(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / sizeof(fdt32_t);
		за (j = 0; j < len; j++) {
			ако (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			break;
		}

		ако (child_found) {
			d++;
		} инако {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	врати 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	цео i, len, noff = 0;

	ако (!fdt)
		врати false;

	док ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ако (val && len > sizeof(fdt32_t)) {
			len = len / sizeof(fdt32_t);
			за (i = 0; i < len; i += 2) {
				ако (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					врати true;
			}
		}
	}

	врати false;
}

цео fdt_parse_imsic_node(const void *fdt, цео nodeoff, struct imsic_data *imsic)
{
	const fdt32_t *val;
	struct imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	цео i, rc, len, nr_parent_irqs;

	ако (nodeoff < 0 || !imsic || !fdt)
		врати SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ако (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		nr_parent_irqs = len / 2;
		за (i = 0; i < len; i += 2) {
			ако (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				break;
			}
		}
	} инако
		врати SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ако (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	инако
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ако (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} инако {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ако ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ако (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	инако
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ако (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	инако
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ако (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	инако
		врати SBI_EINVAL;

	за (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	за (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ако (rc < 0 || !reg_size)
			break;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ако (!imsic->regs[0].size)
		врати SBI_EINVAL;

	врати 0;
}

цео fdt_parse_plic_node(const void *fdt, цео nodeoffset, struct plic_data *plic)
{
	цео len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ако (nodeoffset < 0 || !plic || !fdt)
		врати SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		врати SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ако (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	врати 0;
}

цео fdt_parse_plic(const void *fdt, struct plic_data *plic, const char *compat)
{
	цео nodeoffset;

	ако (!compat || !plic || !fdt)
		врати SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	ако (nodeoffset < 0)
		врати nodeoffset;

	врати fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static цео fdt_get_aclint_addr_size_by_name(const void *fdt, цео nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	цео rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		врати SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	врати 0;
}

static цео fdt_get_aclint_addr_size(const void *fdt, цео nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	цео rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		врати SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ако (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ако (out_addr2)
		*out_addr2 = reg_addr;
	ако (out_size2)
		*out_size2 = reg_size;

	врати 0;
}

цео fdt_parse_aclint_node(const void *fdt, цео nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	цео i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ако (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		врати SBI_EINVAL;

	ако (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	инако
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ако (rc)
		врати rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ако (!val || count < sizeof(fdt32_t))
		врати 0;
	count = count / sizeof(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	за (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ако (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ако (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ако (rc)
			continue;

		ако (match_hwirq == hwirq) {
			ако (hartid < first_hartid)
				first_hartid = hartid;
			ако (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ако ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	врати 0;
}

цео fdt_parse_plmt_node(const void *fdt, цео nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	цео rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ако (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		врати SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ако (rc < 0)
		врати SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ако (!val || count < sizeof(fdt32_t))
		врати 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	за (i = 0; i < (count / 2); i++) {
		цео cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ако (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ако (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ако (rc)
			continue;

		ако (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ако (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	врати 0;
}

цео fdt_parse_plicsw_node(const void *fdt, цео nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	цео rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ако (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		врати SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ако (rc < 0)
		врати SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ако (!val || count < sizeof(fdt32_t))
		врати 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	за (i = 0; i < (count / 2); i++) {
		цео cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ако (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ако (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ако (rc)
			continue;

		ако (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ако (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	врати 0;
}

цео fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const char *compatible)
{
	цео nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ако (nodeoffset < 0)
		врати nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	ако (rc < 0 || !addr)
		врати SBI_ENODEV;

	врати 0;
}
