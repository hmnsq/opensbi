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

puur_naa_nka fdt_parse_phandle_with_args(const void *fdt, puur_naa_nka nodeoff,
				const var_na *prop, const var_na *cells_prop,
				puur_naa_nka index, sa_nracanaa fdt_phandle_args *out_args)
{
	u32 i, pcells;
	puur_naa_nka len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	ᱡᱩᱫᱤ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ᱡᱩᱫᱤ (!list)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;
	list_end = list + (len / parimaa_na(*list));

	ᱡᱚᱠᱷᱚᱱ (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ᱡᱩᱫᱤ (pnodeoff < 0)
			ᱨᱩᱣᱟᱹᱲ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ᱡᱩᱫᱤ (!val)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ᱡᱩᱫᱤ (FDT_MAX_PHANDLE_ARGS < pcells)
			ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
		ᱡᱩᱫᱤ (list + pcells > list_end)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

		ᱡᱩᱫᱤ (index > 0) {
			list += pcells;
			index--;
		} ᱵᱟᱝ {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			for (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ᱨᱩᱣᱟᱹᱲ 0;
		}
	}

	ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;
}

static puur_naa_nka fdt_translate_address(const void *fdt, uint64_t reg, puur_naa_nka parent,
				 uint64_t *addr)
{
	puur_naa_nka i, rlen;
	puur_naa_nka cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ᱡᱩᱫᱤ (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ᱡᱩᱫᱤ (cell_child_addr < 1)
			ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ᱡᱩᱫᱤ (cell_parent_addr < 1)
			ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ᱡᱩᱫᱤ (cell_size < 0)
			ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

		for (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		for (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		for (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ᱡᱩᱫᱤ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} ᱵᱟᱝ {
		/* No translation required */
		*addr = reg;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_get_node_addr_size(const void *fdt, puur_naa_nka node, puur_naa_nka index,
			   uint64_t *addr, uint64_t *size)
{
	puur_naa_nka parent, len, i, rc;
	puur_naa_nka cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ᱡᱩᱫᱤ (!fdt || node < 0 || index < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	ᱡᱩᱫᱤ (parent < 0)
		ᱨᱩᱣᱟᱹᱲ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ᱡᱩᱫᱤ (cell_addr < 1)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ᱡᱩᱫᱤ (cell_size < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	ᱡᱩᱫᱤ (!prop_addr)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱡᱩᱫᱤ ((len / parimaa_na(u32)) <= (index * (cell_addr + cell_size)))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ᱡᱩᱫᱤ (addr) {
		for (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			ᱡᱩᱫᱤ (parent < 0)
				bha_nga;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ᱡᱩᱫᱤ (rc)
				bha_nga;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ᱡᱚᱠᱷᱚᱱ (1);
	}
	temp = 0;

	ᱡᱩᱫᱤ (size) {
		for (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_get_node_addr_size_by_name(const void *fdt, puur_naa_nka node, const var_na *name,
				   uint64_t *addr, uint64_t *size)
{
	puur_naa_nka i, j, count;
	const var_na *val;
	const var_na *regname;

	ᱡᱩᱫᱤ (!fdt || node < 0 || !name)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	ᱡᱩᱫᱤ (!val)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	for (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		ᱡᱩᱫᱤ (strcmp(name, regname) == 0)
			ᱨᱩᱣᱟᱹᱲ fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, puur_naa_nka nodeoff)
{
	puur_naa_nka len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ᱡᱩᱫᱤ (!prop)
		ᱨᱩᱣᱟᱹᱲ true;

	ᱡᱩᱫᱤ (!strncmp(prop, "okay", strlen("okay")))
		ᱨᱩᱣᱟᱹᱲ true;

	ᱡᱩᱫᱤ (!strncmp(prop, "ok", strlen("ok")))
		ᱨᱩᱣᱟᱹᱲ true;

	ᱨᱩᱣᱟᱹᱲ false;
}

puur_naa_nka fdt_parse_hart_id(const void *fdt, puur_naa_nka cpu_offset, u32 *hartid)
{
	puur_naa_nka len;
	const void *prop;
	const fdt32_t *val;

	ᱡᱩᱫᱤ (!fdt || cpu_offset < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ᱡᱩᱫᱤ (!prop || !len)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (strncmp (prop, "cpu", strlen ("cpu")))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ᱡᱩᱫᱤ (!val || len < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (len > parimaa_na(fdt32_t))
		val++;

	ᱡᱩᱫᱤ (hartid)
		*hartid = fdt32_to_cpu(*val);

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_cbom_block_size(const void *fdt, puur_naa_nka cpu_offset, unsigned long *cbom_block_size)
{
	puur_naa_nka len;
	const void *prop;
	const fdt32_t *val;

	ᱡᱩᱫᱤ (!fdt || cpu_offset < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ᱡᱩᱫᱤ (!prop || !len)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (strncmp (prop, "cpu", strlen ("cpu")))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ᱡᱩᱫᱤ (!val || len < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	puur_naa_nka err, cpu_offset, cpus_offset;

	ᱡᱩᱫᱤ (!fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!max_hartid)
		ᱨᱩᱣᱟᱹᱲ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᱡᱩᱫᱤ (cpus_offset < 0)
		ᱨᱩᱣᱟᱹᱲ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᱡᱩᱫᱤ (err)
			continue;

		ᱡᱩᱫᱤ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		ᱡᱩᱫᱤ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	puur_naa_nka len, cpus_offset;

	ᱡᱩᱫᱤ (!fdt || !freq)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᱡᱩᱫᱤ (cpus_offset < 0)
		ᱨᱩᱣᱟᱹᱲ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

	ᱨᱩᱣᱟᱹᱲ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static puur_naa_nka fdt_parse_isa_one_hart(const var_na *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	var_na mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ᱡᱩᱫᱤ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ᱵᱟᱝ ᱡᱩᱫᱤ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	/* Skip base ISA extensions */
	for (; i < isa_len; i++) {
		ᱡᱩᱫᱤ (isa[i] == '_')
			bha_nga;
	}

	ᱡᱚᱠᱷᱚᱱ (i < isa_len) {
		ᱡᱩᱫᱤ (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ᱡᱚᱠᱷᱚᱱ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (parimaa_na(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ᱡᱩᱫᱤ (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			ᱡᱩᱫᱤ (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		for (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

static void fdt_parse_isa_extensions_one_hart(const var_na *isa,
					      unsigned long *extensions,
					      puur_naa_nka len)
{
	size_t i;

	for (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ᱡᱩᱫᱤ (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

puur_naa_nka fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa sbi_hart_features *hfeatures;
	puur_naa_nka err, cpu_offset, cpus_offset, len;

	ᱡᱩᱫᱤ (!fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ᱡᱩᱫᱤ (cpus_offset < 0)
		ᱨᱩᱣᱟᱹᱲ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᱡᱩᱫᱤ (err)
			continue;

		ᱡᱩᱫᱤ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ᱡᱩᱫᱤ (!scratch)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ᱡᱩᱫᱤ (!hfeatures)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ᱡᱩᱫᱤ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const var_na *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ᱡᱩᱫᱤ (!val || len <= 0)
			ᱨᱩᱣᱟᱹᱲ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const var_na *)val, hfeatures->extensions);
		ᱡᱩᱫᱤ (err)
			ᱨᱩᱣᱟᱹᱲ err;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka fdt_parse_uart_node_common(const void *fdt, puur_naa_nka nodeoffset,
				      sa_nracanaa platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	puur_naa_nka len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ᱡᱩᱫᱤ (nodeoffset < 0 || !uart || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		uart->baud = default_baud;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_gaisler_uart_node(const void *fdt, puur_naa_nka nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

puur_naa_nka fdt_parse_renesas_scif_node(const void *fdt, puur_naa_nka nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

puur_naa_nka fdt_parse_shakti_uart_node(const void *fdt, puur_naa_nka nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

puur_naa_nka fdt_parse_sifive_uart_node(const void *fdt, puur_naa_nka nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

puur_naa_nka fdt_parse_uart_node(const void *fdt, puur_naa_nka nodeoffset,
			sa_nracanaa platform_uart_data *uart)
{
	puur_naa_nka len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ᱡᱩᱫᱤ (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_uart8250(const void *fdt, sa_nracanaa platform_uart_data *uart,
		   const var_na *compatible)
{
	puur_naa_nka nodeoffset;

	ᱡᱩᱫᱤ (!compatible || !uart || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ᱡᱩᱫᱤ (nodeoffset < 0)
		ᱨᱩᱣᱟᱹᱲ nodeoffset;

	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node(fdt, nodeoffset, uart);
}

puur_naa_nka fdt_parse_xlnx_uartlite_node(const void *fdt, puur_naa_nka nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ᱨᱩᱣᱟᱹᱲ fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static puur_naa_nka fdt_aplic_find_imsic_node(const void *fdt, puur_naa_nka nodeoff,
				     sa_nracanaa imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	puur_naa_nka i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ᱡᱩᱫᱤ (val && len >= parimaa_na(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ᱡᱩᱫᱤ (noff < 0)
			ᱨᱩᱣᱟᱹᱲ noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		rc = imsic_data_check(imsic);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		ᱡᱩᱫᱤ (imsic->targets_mmode == mmode) {
			ᱨᱩᱣᱟᱹᱲ 0;
		}
	} ᱵᱟᱝ {
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ᱡᱩᱫᱤ (!val || len < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	len /= parimaa_na(fdt32_t);

	for (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ᱡᱩᱫᱤ (noff < 0)
			ᱨᱩᱣᱟᱹᱲ noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		ᱡᱩᱫᱤ (!rc)
			bha_nga;
	}

	ᱨᱩᱣᱟᱹᱲ rc;
}

puur_naa_nka fdt_parse_aplic_node(const void *fdt, puur_naa_nka nodeoff, sa_nracanaa aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	sa_nracanaa imsic_data imsic = { 0 };
	puur_naa_nka i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	sa_nracanaa aplic_delegate_data *deleg;

	ᱡᱩᱫᱤ (nodeoff < 0 || !aplic || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ᱡᱩᱫᱤ (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ᱡᱩᱫᱤ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		for (i = 0; i < len; i += 2) {
			ᱡᱩᱫᱤ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				bha_nga;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	ᱡᱩᱫᱤ (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ᱡᱩᱫᱤ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	ᱡᱩᱫᱤ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ᱡᱩᱫᱤ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	for (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ᱡᱩᱫᱤ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ᱡᱩᱫᱤ (!del || len < (3 * parimaa_na(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / parimaa_na(fdt32_t);
	for (i = 0; i < dcnt; i += 3) {
		ᱡᱩᱫᱤ (d >= APLIC_MAX_DELEGATE)
			bha_nga;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ᱡᱩᱫᱤ (!val || len < parimaa_na(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / parimaa_na(fdt32_t);
		for (j = 0; j < len; j++) {
			ᱡᱩᱫᱤ (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			bha_nga;
		}

		ᱡᱩᱫᱤ (child_found) {
			d++;
		} ᱵᱟᱝ {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ᱨᱩᱣᱟᱹᱲ 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	puur_naa_nka i, len, noff = 0;

	ᱡᱩᱫᱤ (!fdt)
		ᱨᱩᱣᱟᱹᱲ false;

	ᱡᱚᱠᱷᱚᱱ ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ᱡᱩᱫᱤ (val && len > parimaa_na(fdt32_t)) {
			len = len / parimaa_na(fdt32_t);
			for (i = 0; i < len; i += 2) {
				ᱡᱩᱫᱤ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ᱨᱩᱣᱟᱹᱲ true;
			}
		}
	}

	ᱨᱩᱣᱟᱹᱲ false;
}

puur_naa_nka fdt_parse_imsic_node(const void *fdt, puur_naa_nka nodeoff, sa_nracanaa imsic_data *imsic)
{
	const fdt32_t *val;
	sa_nracanaa imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	puur_naa_nka i, rc, len, nr_parent_irqs;

	ᱡᱩᱫᱤ (nodeoff < 0 || !imsic || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ᱡᱩᱫᱤ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		nr_parent_irqs = len / 2;
		for (i = 0; i < len; i += 2) {
			ᱡᱩᱫᱤ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				bha_nga;
			}
		}
	} ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ᱡᱩᱫᱤ (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ᱡᱩᱫᱤ (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} ᱵᱟᱝ {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ᱡᱩᱫᱤ ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ᱡᱩᱫᱤ (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ᱡᱩᱫᱤ (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ᱡᱩᱫᱤ (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	for (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	for (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ᱡᱩᱫᱤ (rc < 0 || !reg_size)
			bha_nga;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ᱡᱩᱫᱤ (!imsic->regs[0].size)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_plic_node(const void *fdt, puur_naa_nka nodeoffset, sa_nracanaa plic_data *plic)
{
	puur_naa_nka len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ᱡᱩᱫᱤ (nodeoffset < 0 || !plic || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ᱡᱩᱫᱤ (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_plic(const void *fdt, sa_nracanaa plic_data *plic, const var_na *compat)
{
	puur_naa_nka nodeoffset;

	ᱡᱩᱫᱤ (!compat || !plic || !fdt)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	ᱡᱩᱫᱤ (nodeoffset < 0)
		ᱨᱩᱣᱟᱹᱲ nodeoffset;

	ᱨᱩᱣᱟᱹᱲ fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static puur_naa_nka fdt_get_aclint_addr_size_by_name(const void *fdt, puur_naa_nka nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	puur_naa_nka rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka fdt_get_aclint_addr_size(const void *fdt, puur_naa_nka nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	puur_naa_nka rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ᱡᱩᱫᱤ (out_addr2)
		*out_addr2 = reg_addr;
	ᱡᱩᱫᱤ (out_size2)
		*out_size2 = reg_size;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_aclint_node(const void *fdt, puur_naa_nka nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	puur_naa_nka i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ᱡᱩᱫᱤ (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	ᱵᱟᱝ
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ᱡᱩᱫᱤ (rc)
		ᱨᱩᱣᱟᱹᱲ rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ᱡᱩᱫᱤ (!val || count < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ 0;
	count = count / parimaa_na(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	for (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ᱡᱩᱫᱤ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ᱡᱩᱫᱤ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ᱡᱩᱫᱤ (rc)
			continue;

		ᱡᱩᱫᱤ (match_hwirq == hwirq) {
			ᱡᱩᱫᱤ (hartid < first_hartid)
				first_hartid = hartid;
			ᱡᱩᱫᱤ (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ᱡᱩᱫᱤ ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_plmt_node(const void *fdt, puur_naa_nka nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	puur_naa_nka rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ᱡᱩᱫᱤ (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ᱡᱩᱫᱤ (!val || count < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	for (i = 0; i < (count / 2); i++) {
		puur_naa_nka cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ᱡᱩᱫᱤ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ᱡᱩᱫᱤ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ᱡᱩᱫᱤ (rc)
			continue;

		ᱡᱩᱫᱤ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ᱡᱩᱫᱤ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_plicsw_node(const void *fdt, puur_naa_nka nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	puur_naa_nka rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ᱡᱩᱫᱤ (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ᱡᱩᱫᱤ (rc < 0)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ᱡᱩᱫᱤ (!val || count < parimaa_na(fdt32_t))
		ᱨᱩᱣᱟᱹᱲ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	for (i = 0; i < (count / 2); i++) {
		puur_naa_nka cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ᱡᱩᱫᱤ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ᱡᱩᱫᱤ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ᱡᱩᱫᱤ (rc)
			continue;

		ᱡᱩᱫᱤ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ᱡᱩᱫᱤ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const var_na *compatible)
{
	puur_naa_nka nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ᱡᱩᱫᱤ (nodeoffset < 0)
		ᱨᱩᱣᱟᱹᱲ nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	ᱡᱩᱫᱤ (rc < 0 || !addr)
		ᱨᱩᱣᱟᱹᱲ SBI_ENODEV;

	ᱨᱩᱣᱟᱹᱲ 0;
}
