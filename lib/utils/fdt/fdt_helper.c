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

정수 fdt_parse_phandle_with_args(const void *fdt, 정수 nodeoff,
				const char *prop, const char *cells_prop,
				정수 index, struct fdt_phandle_args *out_args)
{
	u32 i, pcells;
	정수 len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	만약 (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		반환 SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	만약 (!list)
		반환 SBI_ENOENT;
	list_end = list + (len / sizeof(*list));

	동안 (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		만약 (pnodeoff < 0)
			반환 pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		만약 (!val)
			반환 SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		만약 (FDT_MAX_PHANDLE_ARGS < pcells)
			반환 SBI_EINVAL;
		만약 (list + pcells > list_end)
			반환 SBI_ENOENT;

		만약 (index > 0) {
			list += pcells;
			index--;
		} 아니면 {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			위해 (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			반환 0;
		}
	}

	반환 SBI_ENOENT;
}

static 정수 fdt_translate_address(const void *fdt, uint64_t reg, 정수 parent,
				 uint64_t *addr)
{
	정수 i, rlen;
	정수 cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	만약 (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		만약 (cell_child_addr < 1)
			반환 SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		만약 (cell_parent_addr < 1)
			반환 SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		만약 (cell_size < 0)
			반환 SBI_ENODEV;

		위해 (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		위해 (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		위해 (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		만약 (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			반환 SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} 아니면 {
		/* No translation required */
		*addr = reg;
	}

	반환 0;
}

정수 fdt_get_node_addr_size(const void *fdt, 정수 node, 정수 index,
			   uint64_t *addr, uint64_t *size)
{
	정수 parent, len, i, rc;
	정수 cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	만약 (!fdt || node < 0 || index < 0)
		반환 SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	만약 (parent < 0)
		반환 parent;
	cell_addr = fdt_address_cells(fdt, parent);
	만약 (cell_addr < 1)
		반환 SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	만약 (cell_size < 0)
		반환 SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	만약 (!prop_addr)
		반환 SBI_ENODEV;

	만약 ((len / sizeof(u32)) <= (index * (cell_addr + cell_size)))
		반환 SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	만약 (addr) {
		위해 (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			만약 (parent < 0)
				break;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			만약 (rc)
				break;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} 동안 (1);
	}
	temp = 0;

	만약 (size) {
		위해 (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	반환 0;
}

정수 fdt_get_node_addr_size_by_name(const void *fdt, 정수 node, const char *name,
				   uint64_t *addr, uint64_t *size)
{
	정수 i, j, count;
	const char *val;
	const char *regname;

	만약 (!fdt || node < 0 || !name)
		반환 SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	만약 (!val)
		반환 SBI_ENODEV;

	위해 (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		만약 (strcmp(name, regname) == 0)
			반환 fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	반환 SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, 정수 nodeoff)
{
	정수 len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	만약 (!prop)
		반환 true;

	만약 (!strncmp(prop, "okay", strlen("okay")))
		반환 true;

	만약 (!strncmp(prop, "ok", strlen("ok")))
		반환 true;

	반환 false;
}

정수 fdt_parse_hart_id(const void *fdt, 정수 cpu_offset, u32 *hartid)
{
	정수 len;
	const void *prop;
	const fdt32_t *val;

	만약 (!fdt || cpu_offset < 0)
		반환 SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	만약 (!prop || !len)
		반환 SBI_EINVAL;
	만약 (strncmp (prop, "cpu", strlen ("cpu")))
		반환 SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	만약 (!val || len < sizeof(fdt32_t))
		반환 SBI_EINVAL;

	만약 (len > sizeof(fdt32_t))
		val++;

	만약 (hartid)
		*hartid = fdt32_to_cpu(*val);

	반환 0;
}

정수 fdt_parse_cbom_block_size(const void *fdt, 정수 cpu_offset, unsigned long *cbom_block_size)
{
	정수 len;
	const void *prop;
	const fdt32_t *val;

	만약 (!fdt || cpu_offset < 0)
		반환 SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	만약 (!prop || !len)
		반환 SBI_EINVAL;
	만약 (strncmp (prop, "cpu", strlen ("cpu")))
		반환 SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	만약 (!val || len < sizeof(fdt32_t))
		반환 SBI_EINVAL;

	만약 (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	반환 0;
}

정수 fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	정수 err, cpu_offset, cpus_offset;

	만약 (!fdt)
		반환 SBI_EINVAL;
	만약 (!max_hartid)
		반환 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	만약 (cpus_offset < 0)
		반환 cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		만약 (err)
			continue;

		만약 (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		만약 (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	반환 0;
}

정수 fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	정수 len, cpus_offset;

	만약 (!fdt || !freq)
		반환 SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	만약 (cpus_offset < 0)
		반환 cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	만약 (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	아니면
		반환 SBI_ENOENT;

	반환 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static 정수 fdt_parse_isa_one_hart(const char *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	char mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	만약 (isa[i] == 'r' || isa[i] == 'R')
		i++;
	아니면
		반환 SBI_EINVAL;

	만약 (isa[i] == 'v' || isa[i] == 'V')
		i++;
	아니면
		반환 SBI_EINVAL;

	만약 (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	아니면 만약 (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	아니면
		반환 SBI_EINVAL;

	/* Skip base ISA extensions */
	위해 (; i < isa_len; i++) {
		만약 (isa[i] == '_')
			break;
	}

	동안 (i < isa_len) {
		만약 (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		동안 ((i < isa_len) && (isa[i] != '_') &&
		       (j < (sizeof(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		만약 (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			만약 (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		위해 (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	반환 0;
}

static void fdt_parse_isa_extensions_one_hart(const char *isa,
					      unsigned long *extensions,
					      정수 len)
{
	size_t i;

	위해 (i = 0; i < SBI_HART_EXT_MAX; i++) {
		만약 (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

정수 fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	struct sbi_scratch *scratch;
	struct sbi_hart_features *hfeatures;
	정수 err, cpu_offset, cpus_offset, len;

	만약 (!fdt)
		반환 SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	만약 (cpus_offset < 0)
		반환 cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		만약 (err)
			continue;

		만약 (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		만약 (!scratch)
			반환 SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		만약 (!hfeatures)
			반환 SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		만약 (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const char *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		만약 (!val || len <= 0)
			반환 SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const char *)val, hfeatures->extensions);
		만약 (err)
			반환 err;
	}

	반환 0;
}

static 정수 fdt_parse_uart_node_common(const void *fdt, 정수 nodeoffset,
				      struct platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	정수 len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	만약 (nodeoffset < 0 || !uart || !fdt)
		반환 SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		반환 SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	만약 (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	아니면
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	만약 (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	아니면
		uart->baud = default_baud;

	반환 0;
}

정수 fdt_parse_gaisler_uart_node(const void *fdt, 정수 nodeoffset,
				struct platform_uart_data *uart)
{
	반환 fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

정수 fdt_parse_renesas_scif_node(const void *fdt, 정수 nodeoffset,
				struct platform_uart_data *uart)
{
	반환 fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

정수 fdt_parse_shakti_uart_node(const void *fdt, 정수 nodeoffset,
			       struct platform_uart_data *uart)
{
	반환 fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

정수 fdt_parse_sifive_uart_node(const void *fdt, 정수 nodeoffset,
			       struct platform_uart_data *uart)
{
	반환 fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

정수 fdt_parse_uart_node(const void *fdt, 정수 nodeoffset,
			struct platform_uart_data *uart)
{
	정수 len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	만약 (rc)
		반환 rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	만약 (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	아니면
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	만약 (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	아니면
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	만약 (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	아니면
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	반환 0;
}

정수 fdt_parse_uart8250(const void *fdt, struct platform_uart_data *uart,
		   const char *compatible)
{
	정수 nodeoffset;

	만약 (!compatible || !uart || !fdt)
		반환 SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	만약 (nodeoffset < 0)
		반환 nodeoffset;

	반환 fdt_parse_uart_node(fdt, nodeoffset, uart);
}

정수 fdt_parse_xlnx_uartlite_node(const void *fdt, 정수 nodeoffset,
			       struct platform_uart_data *uart)
{
	반환 fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static 정수 fdt_aplic_find_imsic_node(const void *fdt, 정수 nodeoff,
				     struct imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	정수 i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	만약 (val && len >= sizeof(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		만약 (noff < 0)
			반환 noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		만약 (rc)
			반환 rc;

		rc = imsic_data_check(imsic);
		만약 (rc)
			반환 rc;

		만약 (imsic->targets_mmode == mmode) {
			반환 0;
		}
	} 아니면 {
		반환 SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	만약 (!val || len < sizeof(fdt32_t))
		반환 SBI_ENODEV;

	len /= sizeof(fdt32_t);

	위해 (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		만약 (noff < 0)
			반환 noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		만약 (!rc)
			break;
	}

	반환 rc;
}

정수 fdt_parse_aplic_node(const void *fdt, 정수 nodeoff, struct aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	struct imsic_data imsic = { 0 };
	정수 i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	struct aplic_delegate_data *deleg;

	만약 (nodeoff < 0 || !aplic || !fdt)
		반환 SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		반환 SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	만약 (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	만약 (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		위해 (i = 0; i < len; i += 2) {
			만약 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				break;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	만약 (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		만약 (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			반환 SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	만약 (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		만약 (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			반환 SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	위해 (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	만약 (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	만약 (!del || len < (3 * sizeof(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / sizeof(fdt32_t);
	위해 (i = 0; i < dcnt; i += 3) {
		만약 (d >= APLIC_MAX_DELEGATE)
			break;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		만약 (!val || len < sizeof(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / sizeof(fdt32_t);
		위해 (j = 0; j < len; j++) {
			만약 (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			break;
		}

		만약 (child_found) {
			d++;
		} 아니면 {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	반환 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	정수 i, len, noff = 0;

	만약 (!fdt)
		반환 false;

	동안 ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		만약 (val && len > sizeof(fdt32_t)) {
			len = len / sizeof(fdt32_t);
			위해 (i = 0; i < len; i += 2) {
				만약 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					반환 true;
			}
		}
	}

	반환 false;
}

정수 fdt_parse_imsic_node(const void *fdt, 정수 nodeoff, struct imsic_data *imsic)
{
	const fdt32_t *val;
	struct imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	정수 i, rc, len, nr_parent_irqs;

	만약 (nodeoff < 0 || !imsic || !fdt)
		반환 SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	만약 (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		nr_parent_irqs = len / 2;
		위해 (i = 0; i < len; i += 2) {
			만약 (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				break;
			}
		}
	} 아니면
		반환 SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	만약 (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	아니면
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	만약 (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} 아니면 {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		만약 ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	만약 (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	아니면
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	만약 (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	아니면
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	만약 (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	아니면
		반환 SBI_EINVAL;

	위해 (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	위해 (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		만약 (rc < 0 || !reg_size)
			break;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	만약 (!imsic->regs[0].size)
		반환 SBI_EINVAL;

	반환 0;
}

정수 fdt_parse_plic_node(const void *fdt, 정수 nodeoffset, struct plic_data *plic)
{
	정수 len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	만약 (nodeoffset < 0 || !plic || !fdt)
		반환 SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		반환 SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	만약 (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	반환 0;
}

정수 fdt_parse_plic(const void *fdt, struct plic_data *plic, const char *compat)
{
	정수 nodeoffset;

	만약 (!compat || !plic || !fdt)
		반환 SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	만약 (nodeoffset < 0)
		반환 nodeoffset;

	반환 fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static 정수 fdt_get_aclint_addr_size_by_name(const void *fdt, 정수 nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	정수 rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		반환 SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	반환 0;
}

static 정수 fdt_get_aclint_addr_size(const void *fdt, 정수 nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	정수 rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		반환 SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	만약 (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	만약 (out_addr2)
		*out_addr2 = reg_addr;
	만약 (out_size2)
		*out_size2 = reg_size;

	반환 0;
}

정수 fdt_parse_aclint_node(const void *fdt, 정수 nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	정수 i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	만약 (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		반환 SBI_EINVAL;

	만약 (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	아니면
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	만약 (rc)
		반환 rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	만약 (!val || count < sizeof(fdt32_t))
		반환 0;
	count = count / sizeof(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	위해 (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		만약 (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		만약 (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		만약 (rc)
			continue;

		만약 (match_hwirq == hwirq) {
			만약 (hartid < first_hartid)
				first_hartid = hartid;
			만약 (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	만약 ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	반환 0;
}

정수 fdt_parse_plmt_node(const void *fdt, 정수 nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	정수 rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	만약 (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		반환 SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	만약 (rc < 0)
		반환 SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	만약 (!val || count < sizeof(fdt32_t))
		반환 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	위해 (i = 0; i < (count / 2); i++) {
		정수 cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		만약 (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		만약 (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		만약 (rc)
			continue;

		만약 (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		만약 (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	반환 0;
}

정수 fdt_parse_plicsw_node(const void *fdt, 정수 nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	정수 rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	만약 (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		반환 SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	만약 (rc < 0)
		반환 SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	만약 (!val || count < sizeof(fdt32_t))
		반환 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	위해 (i = 0; i < (count / 2); i++) {
		정수 cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		만약 (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		만약 (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		만약 (rc)
			continue;

		만약 (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		만약 (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	반환 0;
}

정수 fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const char *compatible)
{
	정수 nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	만약 (nodeoffset < 0)
		반환 nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	만약 (rc < 0 || !addr)
		반환 SBI_ENODEV;

	반환 0;
}
