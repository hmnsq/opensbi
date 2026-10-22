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

целое fdt_parse_phandle_with_args(const void *fdt, целое nodeoff,
				const char *prop, const char *cells_prop,
				целое index, struct fdt_phandle_args *out_args)
{
	u32 i, pcells;
	целое len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	если (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		вернуть SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	если (!list)
		вернуть SBI_ENOENT;
	list_end = list + (len / sizeof(*list));

	пока (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		если (pnodeoff < 0)
			вернуть pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		если (!val)
			вернуть SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		если (FDT_MAX_PHANDLE_ARGS < pcells)
			вернуть SBI_EINVAL;
		если (list + pcells > list_end)
			вернуть SBI_ENOENT;

		если (index > 0) {
			list += pcells;
			index--;
		} иначе {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			для (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			вернуть 0;
		}
	}

	вернуть SBI_ENOENT;
}

static целое fdt_translate_address(const void *fdt, uint64_t reg, целое parent,
				 uint64_t *addr)
{
	целое i, rlen;
	целое cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	если (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		если (cell_child_addr < 1)
			вернуть SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		если (cell_parent_addr < 1)
			вернуть SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		если (cell_size < 0)
			вернуть SBI_ENODEV;

		для (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		для (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		для (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		если (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			вернуть SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} иначе {
		/* No translation required */
		*addr = reg;
	}

	вернуть 0;
}

целое fdt_get_node_addr_size(const void *fdt, целое node, целое index,
			   uint64_t *addr, uint64_t *size)
{
	целое parent, len, i, rc;
	целое cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	если (!fdt || node < 0 || index < 0)
		вернуть SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	если (parent < 0)
		вернуть parent;
	cell_addr = fdt_address_cells(fdt, parent);
	если (cell_addr < 1)
		вернуть SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	если (cell_size < 0)
		вернуть SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	если (!prop_addr)
		вернуть SBI_ENODEV;

	если ((len / sizeof(u32)) <= (index * (cell_addr + cell_size)))
		вернуть SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	если (addr) {
		для (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			если (parent < 0)
				break;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			если (rc)
				break;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} пока (1);
	}
	temp = 0;

	если (size) {
		для (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	вернуть 0;
}

целое fdt_get_node_addr_size_by_name(const void *fdt, целое node, const char *name,
				   uint64_t *addr, uint64_t *size)
{
	целое i, j, count;
	const char *val;
	const char *regname;

	если (!fdt || node < 0 || !name)
		вернуть SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	если (!val)
		вернуть SBI_ENODEV;

	для (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		если (strcmp(name, regname) == 0)
			вернуть fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	вернуть SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, целое nodeoff)
{
	целое len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	если (!prop)
		вернуть true;

	если (!strncmp(prop, "okay", strlen("okay")))
		вернуть true;

	если (!strncmp(prop, "ok", strlen("ok")))
		вернуть true;

	вернуть false;
}

целое fdt_parse_hart_id(const void *fdt, целое cpu_offset, u32 *hartid)
{
	целое len;
	const void *prop;
	const fdt32_t *val;

	если (!fdt || cpu_offset < 0)
		вернуть SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	если (!prop || !len)
		вернуть SBI_EINVAL;
	если (strncmp (prop, "cpu", strlen ("cpu")))
		вернуть SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	если (!val || len < sizeof(fdt32_t))
		вернуть SBI_EINVAL;

	если (len > sizeof(fdt32_t))
		val++;

	если (hartid)
		*hartid = fdt32_to_cpu(*val);

	вернуть 0;
}

целое fdt_parse_cbom_block_size(const void *fdt, целое cpu_offset, unsigned long *cbom_block_size)
{
	целое len;
	const void *prop;
	const fdt32_t *val;

	если (!fdt || cpu_offset < 0)
		вернуть SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	если (!prop || !len)
		вернуть SBI_EINVAL;
	если (strncmp (prop, "cpu", strlen ("cpu")))
		вернуть SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	если (!val || len < sizeof(fdt32_t))
		вернуть SBI_EINVAL;

	если (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	вернуть 0;
}

целое fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	целое err, cpu_offset, cpus_offset;

	если (!fdt)
		вернуть SBI_EINVAL;
	если (!max_hartid)
		вернуть 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	если (cpus_offset < 0)
		вернуть cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		если (err)
			continue;

		если (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		если (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	вернуть 0;
}

целое fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	целое len, cpus_offset;

	если (!fdt || !freq)
		вернуть SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	если (cpus_offset < 0)
		вернуть cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	если (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	иначе
		вернуть SBI_ENOENT;

	вернуть 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static целое fdt_parse_isa_one_hart(const char *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	char mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	если (isa[i] == 'r' || isa[i] == 'R')
		i++;
	иначе
		вернуть SBI_EINVAL;

	если (isa[i] == 'v' || isa[i] == 'V')
		i++;
	иначе
		вернуть SBI_EINVAL;

	если (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	иначе если (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	иначе
		вернуть SBI_EINVAL;

	/* Skip base ISA extensions */
	для (; i < isa_len; i++) {
		если (isa[i] == '_')
			break;
	}

	пока (i < isa_len) {
		если (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		пока ((i < isa_len) && (isa[i] != '_') &&
		       (j < (sizeof(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		если (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			если (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		для (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	вернуть 0;
}

static void fdt_parse_isa_extensions_one_hart(const char *isa,
					      unsigned long *extensions,
					      целое len)
{
	size_t i;

	для (i = 0; i < SBI_HART_EXT_MAX; i++) {
		если (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

целое fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	struct sbi_scratch *scratch;
	struct sbi_hart_features *hfeatures;
	целое err, cpu_offset, cpus_offset, len;

	если (!fdt)
		вернуть SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	если (cpus_offset < 0)
		вернуть cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		если (err)
			continue;

		если (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		если (!scratch)
			вернуть SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		если (!hfeatures)
			вернуть SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		если (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const char *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		если (!val || len <= 0)
			вернуть SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const char *)val, hfeatures->extensions);
		если (err)
			вернуть err;
	}

	вернуть 0;
}

static целое fdt_parse_uart_node_common(const void *fdt, целое nodeoffset,
				      struct platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	целое len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	если (nodeoffset < 0 || !uart || !fdt)
		вернуть SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		вернуть SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	если (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	иначе
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	если (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	иначе
		uart->baud = default_baud;

	вернуть 0;
}

целое fdt_parse_gaisler_uart_node(const void *fdt, целое nodeoffset,
				struct platform_uart_data *uart)
{
	вернуть fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

целое fdt_parse_renesas_scif_node(const void *fdt, целое nodeoffset,
				struct platform_uart_data *uart)
{
	вернуть fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

целое fdt_parse_shakti_uart_node(const void *fdt, целое nodeoffset,
			       struct platform_uart_data *uart)
{
	вернуть fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

целое fdt_parse_sifive_uart_node(const void *fdt, целое nodeoffset,
			       struct platform_uart_data *uart)
{
	вернуть fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

целое fdt_parse_uart_node(const void *fdt, целое nodeoffset,
			struct platform_uart_data *uart)
{
	целое len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	если (rc)
		вернуть rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	если (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	иначе
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	если (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	иначе
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	если (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	иначе
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	вернуть 0;
}

целое fdt_parse_uart8250(const void *fdt, struct platform_uart_data *uart,
		   const char *compatible)
{
	целое nodeoffset;

	если (!compatible || !uart || !fdt)
		вернуть SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	если (nodeoffset < 0)
		вернуть nodeoffset;

	вернуть fdt_parse_uart_node(fdt, nodeoffset, uart);
}

целое fdt_parse_xlnx_uartlite_node(const void *fdt, целое nodeoffset,
			       struct platform_uart_data *uart)
{
	вернуть fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static целое fdt_aplic_find_imsic_node(const void *fdt, целое nodeoff,
				     struct imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	целое i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	если (val && len >= sizeof(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		если (noff < 0)
			вернуть noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		если (rc)
			вернуть rc;

		rc = imsic_data_check(imsic);
		если (rc)
			вернуть rc;

		если (imsic->targets_mmode == mmode) {
			вернуть 0;
		}
	} иначе {
		вернуть SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	если (!val || len < sizeof(fdt32_t))
		вернуть SBI_ENODEV;

	len /= sizeof(fdt32_t);

	для (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		если (noff < 0)
			вернуть noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		если (!rc)
			break;
	}

	вернуть rc;
}

целое fdt_parse_aplic_node(const void *fdt, целое nodeoff, struct aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	struct imsic_data imsic = { 0 };
	целое i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	struct aplic_delegate_data *deleg;

	если (nodeoff < 0 || !aplic || !fdt)
		вернуть SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		вернуть SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	если (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	если (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		для (i = 0; i < len; i += 2) {
			если (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				break;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	если (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		если (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			вернуть SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	если (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		если (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			вернуть SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	для (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	если (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	если (!del || len < (3 * sizeof(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / sizeof(fdt32_t);
	для (i = 0; i < dcnt; i += 3) {
		если (d >= APLIC_MAX_DELEGATE)
			break;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		если (!val || len < sizeof(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / sizeof(fdt32_t);
		для (j = 0; j < len; j++) {
			если (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			break;
		}

		если (child_found) {
			d++;
		} иначе {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	вернуть 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	целое i, len, noff = 0;

	если (!fdt)
		вернуть false;

	пока ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		если (val && len > sizeof(fdt32_t)) {
			len = len / sizeof(fdt32_t);
			для (i = 0; i < len; i += 2) {
				если (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					вернуть true;
			}
		}
	}

	вернуть false;
}

целое fdt_parse_imsic_node(const void *fdt, целое nodeoff, struct imsic_data *imsic)
{
	const fdt32_t *val;
	struct imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	целое i, rc, len, nr_parent_irqs;

	если (nodeoff < 0 || !imsic || !fdt)
		вернуть SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	если (val && len > sizeof(fdt32_t)) {
		len = len / sizeof(fdt32_t);
		nr_parent_irqs = len / 2;
		для (i = 0; i < len; i += 2) {
			если (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				break;
			}
		}
	} иначе
		вернуть SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	если (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	иначе
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	если (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} иначе {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		если ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	если (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	иначе
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	если (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	иначе
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	если (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	иначе
		вернуть SBI_EINVAL;

	для (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	для (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		если (rc < 0 || !reg_size)
			break;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	если (!imsic->regs[0].size)
		вернуть SBI_EINVAL;

	вернуть 0;
}

целое fdt_parse_plic_node(const void *fdt, целое nodeoffset, struct plic_data *plic)
{
	целое len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	если (nodeoffset < 0 || !plic || !fdt)
		вернуть SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		вернуть SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	если (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	вернуть 0;
}

целое fdt_parse_plic(const void *fdt, struct plic_data *plic, const char *compat)
{
	целое nodeoffset;

	если (!compat || !plic || !fdt)
		вернуть SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	если (nodeoffset < 0)
		вернуть nodeoffset;

	вернуть fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static целое fdt_get_aclint_addr_size_by_name(const void *fdt, целое nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	целое rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		вернуть SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	вернуть 0;
}

static целое fdt_get_aclint_addr_size(const void *fdt, целое nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	целое rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		вернуть SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	если (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	если (out_addr2)
		*out_addr2 = reg_addr;
	если (out_size2)
		*out_size2 = reg_size;

	вернуть 0;
}

целое fdt_parse_aclint_node(const void *fdt, целое nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	целое i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	если (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		вернуть SBI_EINVAL;

	если (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	иначе
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	если (rc)
		вернуть rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	если (!val || count < sizeof(fdt32_t))
		вернуть 0;
	count = count / sizeof(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	для (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		если (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		если (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		если (rc)
			continue;

		если (match_hwirq == hwirq) {
			если (hartid < first_hartid)
				first_hartid = hartid;
			если (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	если ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	вернуть 0;
}

целое fdt_parse_plmt_node(const void *fdt, целое nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	целое rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	если (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		вернуть SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	если (rc < 0)
		вернуть SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	если (!val || count < sizeof(fdt32_t))
		вернуть 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	для (i = 0; i < (count / 2); i++) {
		целое cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		если (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		если (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		если (rc)
			continue;

		если (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		если (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	вернуть 0;
}

целое fdt_parse_plicsw_node(const void *fdt, целое nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	целое rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	если (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		вернуть SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	если (rc < 0)
		вернуть SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	если (!val || count < sizeof(fdt32_t))
		вернуть 0;
	count = count / sizeof(fdt32_t);

	hcount = 0;
	для (i = 0; i < (count / 2); i++) {
		целое cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		если (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		если (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		если (rc)
			continue;

		если (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		если (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	вернуть 0;
}

целое fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const char *compatible)
{
	целое nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	если (nodeoffset < 0)
		вернуть nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	если (rc < 0 || !addr)
		вернуть SBI_ENODEV;

	вернуть 0;
}
