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

số_nguyên fdt_parse_phandle_with_args(const void *fdt, số_nguyên nodeoff,
				const char *prop, const char *cells_prop,
				số_nguyên index, struct fdt_phandle_args *out_args)
{
	u32 i, pcells;
	số_nguyên len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	nếu (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		trả_về SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	nếu (!list)
		trả_về SBI_ENOENT;
	list_end = list + (len / sizeof(*list));

	trong_khi (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		nếu (pnodeoff < 0)
			trả_về pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		nếu (!val)
			trả_về SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		nếu (FDT_MAX_PHANDLE_ARGS < pcells)
			trả_về SBI_EINVAL;
		nếu (list + pcells > list_end)
			trả_về SBI_ENOENT;

		nếu (index > 0) {
			list += pcells;
			index--;
		} không_thì {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			cho (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			trả_về 0;
		}
	}

	trả_về SBI_ENOENT;
}

static số_nguyên fdt_translate_address(const void *fdt, uint64_t reg, số_nguyên parent,
				 uint64_t *addr)
{
	số_nguyên i, rlen;
	số_nguyên cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	nếu (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		nếu (cell_child_addr < 1)
			trả_về SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		nếu (cell_parent_addr < 1)
			trả_về SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		nếu (cell_size < 0)
			trả_về SBI_ENODEV;

		cho (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		cho (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		cho (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		nếu (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			trả_về SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} không_thì {
		/* No translation required */
		*addr = reg;
	}

	trả_về 0;
}

số_nguyên fdt_get_node_addr_size(const void *fdt, số_nguyên node, số_nguyên index,
			   uint64_t *addr, uint64_t *size)
{
	số_nguyên parent, len, i, rc;
	số_nguyên cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	nếu (!fdt || node < 0 || index < 0)
		trả_về SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	nếu (parent < 0)
		trả_về parent;
	cell_addr = fdt_address_cells(fdt, parent);
	nếu (cell_addr < 1)
		trả_về SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	nếu (cell_size < 0)
		trả_về SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	nếu (!prop_addr)
		trả_về SBI_ENODEV;

	nếu ((len / sizeof(u32)) <= (index * (cell_addr + cell_size)))
		trả_về SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	nếu (addr) {
		cho (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			nếu (parent < 0)
				break;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			nếu (rc)
				break;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} trong_khi (1);
	}
	temp = 0;

	nếu (size) {
		cho (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	trả_về 0;
}

số_nguyên fdt_get_node_addr_size_by_name(const void *fdt, số_nguyên node, const char *name,
				   uint64_t *addr, uint64_t *size)
{
	số_nguyên i, j, count;
	const char *val;
	const char *regname;

	nếu (!fdt || node < 0 || !name)
		trả_về SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	nếu (!val)
		trả_về SBI_ENODEV;

	cho (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		nếu (strcmp(name, regname) == 0)
			trả_về fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	trả_về SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, số_nguyên nodeoff)
{
	số_nguyên len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	nếu (!prop)
		trả_về true;

	nếu (!strncmp(prop, "okay", strlen("okay")))
		trả_về true;

	nếu (!strncmp(prop, "ok", strlen("ok")))
		trả_về true;

	trả_về false;
}

số_nguyên fdt_parse_hart_id(const void *fdt, số_nguyên cpu_offset, u32 *hartid)
{
	số_nguyên len;
	const void *prop;
	const fdt32_t *val;

	nếu (!fdt || cpu_offset < 0)
		trả_về SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	nếu (!prop || !len)
		trả_về SBI_EINVAL;
	nếu (strncmp (prop, "cpu", strlen ("cpu")))
		trả_về SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	nếu (!val || len < sizeof(fdt32_t))
		trả_về SBI_EINVAL;

	nếu (len > sizeof(fdt32_t))
		val++;

	nếu (hartid)
		*hartid = fdt32_to_cpu(*val);

	trả_về 0;
}

số_nguyên fdt_parse_cbom_block_size(const void *fdt, số_nguyên cpu_offset, unsigned long *cbom_block_size)
{
	số_nguyên len;
	const void *prop;
	const fdt32_t *val;

	nếu (!fdt || cpu_offset < 0)
		trả_về SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	nếu (!prop || !len)
		trả_về SBI_EINVAL;
	nếu (strncmp (prop, "cpu", strlen ("cpu")))
		trả_về SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	nếu (!val || len < sizeof(fdt32_t))
		trả_về SBI_EINVAL;

	nếu (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	trả_về 0;
}

số_nguyên fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	số_nguyên err, cpu_offset, cpus_offset;

	nếu (!fdt)
		trả_về SBI_EINVAL;
	nếu (!max_hartid)
		trả_về 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		nếu (err)
			continue;

		nếu (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		nếu (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	trả_về 0;
}

số_nguyên fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	số_nguyên len, cpus_offset;

	nếu (!fdt || !freq)
		trả_về SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	nếu (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	không_thì
		trả_về SBI_ENOENT;

	trả_về 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static số_nguyên fdt_parse_isa_one_hart(const char *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	char mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	nếu (isa[i] == 'r' || isa[i] == 'R')
		i++;
	không_thì
		trả_về SBI_EINVAL;

	nếu (isa[i] == 'v' || isa[i] == 'V')
		i++;
	không_thì
		trả_về SBI_EINVAL;

	nếu (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	không_thì nếu (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	không_thì
		trả_về SBI_EINVAL;

	/* Skip base ISA extensions */
	cho (; i < isa_len; i++) {
		nếu (isa[i] == '_')
			break;
	}

	trong_khi (i < isa_len) {
		nếu (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		trong_khi ((i < isa_len) && (isa[i] != '_') &&
		       (j < (sizeof(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		nếu (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			nếu (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		cho (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	trả_về 0;
}

static void fdt_parse_isa_extensions_one_hart(const char *isa,
					      unsigned long *extensions,
					      số_nguyên len)
{
	size_t i;

	cho (i = 0; i < SBI_HART_EXT_MAX; i++) {
		nếu (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

số_nguyên fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	struct sbi_scratch *scratch;
	struct sbi_hart_features *hfeatures;
	số_nguyên err, cpu_offset, cpus_offset, len;

	nếu (!fdt)
		trả_về SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	nếu (cpus_offset < 0)
		trả_về cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		nếu (err)
			continue;

		nếu (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		nếu (!scratch)
			trả_về SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		nếu (!hfeatures)
			trả_về SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		nếu (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const char *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		nếu (!val || len <= 0)
			trả_về SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const char *)val, hfeatures->extensions);
		nếu (err)
			trả_về err;
	}

	trả_về 0;
}

static số_nguyên fdt_parse_uart_node_common(const void *fdt, số_nguyên nodeoffset,
				      struct platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	số_nguyên len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	nếu (nodeoffset < 0 || !uart || !fdt)
		trả_về SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		trả_về SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	nếu (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	không_thì
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	nếu (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	không_thì
		uart->baud = default_baud;

	trả_về 0;
}

số_nguyên fdt_parse_gaisler_uart_node(const void *fdt, số_nguyên nodeoffset,
				struct platform_uart_data *uart)
{
	trả_về fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

số_nguyên fdt_parse_renesas_scif_node(const void *fdt, số_nguyên nodeoffset,
				struct platform_uart_data *uart)
{
	trả_về fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

số_nguyên fdt_parse_shakti_uart_node(const void *fdt, số_nguyên nodeoffset,
			       struct platform_uart_data *uart)
{
	trả_về fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

số_nguyên fdt_parse_sifive_uart_node(const void *fdt, số_nguyên nodeoffset,
			       struct platform_uart_data *uart)
{
	trả_về fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

số_nguyên fdt_parse_uart_node(const void *fdt, số_nguyên nodeoffset,
			struct platform_uart_data *uart)
{
	số_nguyên len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	nếu (rc)
		trả_về rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	nếu (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	không_thì
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	nếu (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	không_thì
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	nếu (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	không_thì
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	trả_về 0;
}

số_nguyên fdt_parse_uart8250(const void *fdt, struct platform_uart_data *uart,
		   const char *compatible)
{
	số_nguyên nodeoffset;

	nếu (!compatible || !uart || !fdt)
		trả_về SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	nếu (nodeoffset < 0)
		trả_về nodeoffset;

	trả_về fdt_parse_uart_node(fdt, nodeoffset, uart);
}

số_nguyên fdt_parse_xlnx_uartlite_node(const void *fdt, số_nguyên nodeoffset,
			       struct platform_uart_data *uart)
{
	trả_về fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static số_nguyên fdt_aplic_find_imsic_node(const void *fdt, số_nguyên nodeoff,
				     struct imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	số_nguyên i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	nếu (val && len >= sizeof(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		nếu (noff < 0)
			trả_về noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		nếu (rc)
			trả_về rc;

		rc = imsic_data_check(imsic);
		nếu (rc)
			trả_về rc;

		nếu (imsic->targets_mmode == mmode) {
			trả_về 0;
		}
	} không_thì {
		trả_về SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	nếu (!val || len < sizeof(fdt32_t))
		trả_về SBI_ENODEV;

	len /= sizeof(fdt32_t);

	cho (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		nếu (noff < 0)
			trả_về noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		nếu (!rc)
			break;
	}

	trả_về rc;
}

số_nguyên fdt_parse_aplic_node(const void *fdt, số_nguyên nodeoff, struct aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	struct imsic_data imsic = { 0 };
	số_nguyên i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	struct aplic_delegate_data *deleg;

	nếu (nodeoff < 0 || !aplic || !fdt)
		trả_về SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		trả_về SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	nếu (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	nếu (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		cho (i = 0; i < len; i += 2) {
			nếu (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				break;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	nếu (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		nếu (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			trả_về SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	nếu (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		nếu (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			trả_về SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	cho (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	nếu (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	nếu (!del || len < (3 * sizeof(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / sizeof(fdt32_t);
	cho (i = 0; i < dcnt; i += 3) {
		nếu (d >= APLIC_MAX_DELEGATE)
			break;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		nếu (!val || len < sizeof(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / sizeof(fdt32_t);
		cho (j = 0; j < len; j++) {
			nếu (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			break;
		}

		nếu (child_found) {
			d++;
		} không_thì {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	trả_về 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	số_nguyên i, len, noff = 0;

	nếu (!fdt)
		trả_về false;

	trong_khi ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		nếu (val && len > sizeof(fdt32_t)) {
			len = len / sizeof(fdt32_t);
			cho (i = 0; i < len; i += 2) {
				nếu (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					trả_về true;
			}
		}
	}

	trả_về false;
}

số_nguyên fdt_parse_imsic_node(const void *fdt, số_nguyên nodeoff, struct imsic_data *imsic)
{
	const fdt32_t *val;
	struct imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	số_nguyên i, rc, len, nr_parent_irqs;

	nếu (nodeoff < 0 || !imsic || !fdt)
		trả_về SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	nếu (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		nr_parent_irqs = len / 2;
		cho (i = 0; i < len; i += 2) {
			nếu (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				break;
			}
		}
	} không_thì
		trả_về SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	nếu (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	không_thì
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	nếu (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} không_thì {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		nếu ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	nếu (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	không_thì
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	nếu (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	không_thì
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	nếu (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	không_thì
		trả_về SBI_EINVAL;

	cho (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	cho (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		nếu (rc < 0 || !reg_size)
			break;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	nếu (!imsic->regs[0].size)
		trả_về SBI_EINVAL;

	trả_về 0;
}

số_nguyên fdt_parse_plic_node(const void *fdt, số_nguyên nodeoffset, struct plic_data *plic)
{
	số_nguyên len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	nếu (nodeoffset < 0 || !plic || !fdt)
		trả_về SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		trả_về SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	nếu (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	trả_về 0;
}

số_nguyên fdt_parse_plic(const void *fdt, struct plic_data *plic, const char *compat)
{
	số_nguyên nodeoffset;

	nếu (!compat || !plic || !fdt)
		trả_về SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	nếu (nodeoffset < 0)
		trả_về nodeoffset;

	trả_về fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static số_nguyên fdt_get_aclint_addr_size_by_name(const void *fdt, số_nguyên nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	số_nguyên rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		trả_về SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	trả_về 0;
}

static số_nguyên fdt_get_aclint_addr_size(const void *fdt, số_nguyên nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	số_nguyên rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		trả_về SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	nếu (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	nếu (out_addr2)
		*out_addr2 = reg_addr;
	nếu (out_size2)
		*out_size2 = reg_size;

	trả_về 0;
}

số_nguyên fdt_parse_aclint_node(const void *fdt, số_nguyên nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	số_nguyên i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	nếu (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		trả_về SBI_EINVAL;

	nếu (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	không_thì
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	nếu (rc)
		trả_về rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	nếu (!val || count < sizeof(fdt32_t))
		trả_về 0;
	count = count / sizeof(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	cho (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		nếu (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		nếu (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		nếu (rc)
			continue;

		nếu (match_hwirq == hwirq) {
			nếu (hartid < first_hartid)
				first_hartid = hartid;
			nếu (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	nếu ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	trả_về 0;
}

số_nguyên fdt_parse_plmt_node(const void *fdt, số_nguyên nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	số_nguyên rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	nếu (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		trả_về SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	nếu (rc < 0)
		trả_về SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	nếu (!val || count < sizeof(fdt32_t))
		trả_về 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	cho (i = 0; i < (count / 2); i++) {
		số_nguyên cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		nếu (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		nếu (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		nếu (rc)
			continue;

		nếu (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		nếu (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	trả_về 0;
}

số_nguyên fdt_parse_plicsw_node(const void *fdt, số_nguyên nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	số_nguyên rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	nếu (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		trả_về SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	nếu (rc < 0)
		trả_về SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	nếu (!val || count < sizeof(fdt32_t))
		trả_về 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	cho (i = 0; i < (count / 2); i++) {
		số_nguyên cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		nếu (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		nếu (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		nếu (rc)
			continue;

		nếu (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		nếu (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	trả_về 0;
}

số_nguyên fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const char *compatible)
{
	số_nguyên nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	nếu (nodeoffset < 0)
		trả_về nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	nếu (rc < 0 || !addr)
		trả_về SBI_ENODEV;

	trả_về 0;
}
