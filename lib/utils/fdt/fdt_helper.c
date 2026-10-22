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

ចំនួនគត់ fdt_parse_phandle_with_args(const void *fdt, ចំនួនគត់ nodeoff,
				const var_na *prop, const var_na *cells_prop,
				ចំនួនគត់ index, sa_nracanaa fdt_phandle_args *out_args)
{
	u32 i, pcells;
	ចំនួនគត់ len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	ប្រសិនបើ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ត្រឡប់ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ប្រសិនបើ (!list)
		ត្រឡប់ SBI_ENOENT;
	list_end = list + (len / parimaa_na(*list));

	ខណៈពេល (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ប្រសិនបើ (pnodeoff < 0)
			ត្រឡប់ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ប្រសិនបើ (!val)
			ត្រឡប់ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ប្រសិនបើ (FDT_MAX_PHANDLE_ARGS < pcells)
			ត្រឡប់ SBI_EINVAL;
		ប្រសិនបើ (list + pcells > list_end)
			ត្រឡប់ SBI_ENOENT;

		ប្រសិនបើ (index > 0) {
			list += pcells;
			index--;
		} បើមិនដូច្នេះ {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			សម្រាប់ (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ត្រឡប់ 0;
		}
	}

	ត្រឡប់ SBI_ENOENT;
}

static ចំនួនគត់ fdt_translate_address(const void *fdt, uint64_t reg, ចំនួនគត់ parent,
				 uint64_t *addr)
{
	ចំនួនគត់ i, rlen;
	ចំនួនគត់ cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ប្រសិនបើ (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ប្រសិនបើ (cell_child_addr < 1)
			ត្រឡប់ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ប្រសិនបើ (cell_parent_addr < 1)
			ត្រឡប់ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ប្រសិនបើ (cell_size < 0)
			ត្រឡប់ SBI_ENODEV;

		សម្រាប់ (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		សម្រាប់ (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		សម្រាប់ (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ប្រសិនបើ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ត្រឡប់ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} បើមិនដូច្នេះ {
		/* No translation required */
		*addr = reg;
	}

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_get_node_addr_size(const void *fdt, ចំនួនគត់ node, ចំនួនគត់ index,
			   uint64_t *addr, uint64_t *size)
{
	ចំនួនគត់ parent, len, i, rc;
	ចំនួនគត់ cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ប្រសិនបើ (!fdt || node < 0 || index < 0)
		ត្រឡប់ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	ប្រសិនបើ (parent < 0)
		ត្រឡប់ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ប្រសិនបើ (cell_addr < 1)
		ត្រឡប់ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ប្រសិនបើ (cell_size < 0)
		ត្រឡប់ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	ប្រសិនបើ (!prop_addr)
		ត្រឡប់ SBI_ENODEV;

	ប្រសិនបើ ((len / parimaa_na(u32)) <= (index * (cell_addr + cell_size)))
		ត្រឡប់ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ប្រសិនបើ (addr) {
		សម្រាប់ (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			ប្រសិនបើ (parent < 0)
				bha_nga;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ប្រសិនបើ (rc)
				bha_nga;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ខណៈពេល (1);
	}
	temp = 0;

	ប្រសិនបើ (size) {
		សម្រាប់ (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_get_node_addr_size_by_name(const void *fdt, ចំនួនគត់ node, const var_na *name,
				   uint64_t *addr, uint64_t *size)
{
	ចំនួនគត់ i, j, count;
	const var_na *val;
	const var_na *regname;

	ប្រសិនបើ (!fdt || node < 0 || !name)
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	ប្រសិនបើ (!val)
		ត្រឡប់ SBI_ENODEV;

	សម្រាប់ (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		ប្រសិនបើ (strcmp(name, regname) == 0)
			ត្រឡប់ fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	ត្រឡប់ SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, ចំនួនគត់ nodeoff)
{
	ចំនួនគត់ len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ប្រសិនបើ (!prop)
		ត្រឡប់ true;

	ប្រសិនបើ (!strncmp(prop, "okay", strlen("okay")))
		ត្រឡប់ true;

	ប្រសិនបើ (!strncmp(prop, "ok", strlen("ok")))
		ត្រឡប់ true;

	ត្រឡប់ false;
}

ចំនួនគត់ fdt_parse_hart_id(const void *fdt, ចំនួនគត់ cpu_offset, u32 *hartid)
{
	ចំនួនគត់ len;
	const void *prop;
	const fdt32_t *val;

	ប្រសិនបើ (!fdt || cpu_offset < 0)
		ត្រឡប់ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ប្រសិនបើ (!prop || !len)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (strncmp (prop, "cpu", strlen ("cpu")))
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ប្រសិនបើ (!val || len < parimaa_na(fdt32_t))
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (len > parimaa_na(fdt32_t))
		val++;

	ប្រសិនបើ (hartid)
		*hartid = fdt32_to_cpu(*val);

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_cbom_block_size(const void *fdt, ចំនួនគត់ cpu_offset, unsigned long *cbom_block_size)
{
	ចំនួនគត់ len;
	const void *prop;
	const fdt32_t *val;

	ប្រសិនបើ (!fdt || cpu_offset < 0)
		ត្រឡប់ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ប្រសិនបើ (!prop || !len)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (strncmp (prop, "cpu", strlen ("cpu")))
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ប្រសិនបើ (!val || len < parimaa_na(fdt32_t))
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	ចំនួនគត់ err, cpu_offset, cpus_offset;

	ប្រសិនបើ (!fdt)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!max_hartid)
		ត្រឡប់ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ប្រសិនបើ (cpus_offset < 0)
		ត្រឡប់ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ប្រសិនបើ (err)
			continue;

		ប្រសិនបើ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		ប្រសិនបើ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	ចំនួនគត់ len, cpus_offset;

	ប្រសិនបើ (!fdt || !freq)
		ត្រឡប់ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ប្រសិនបើ (cpus_offset < 0)
		ត្រឡប់ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ប្រសិនបើ (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_ENOENT;

	ត្រឡប់ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static ចំនួនគត់ fdt_parse_isa_one_hart(const var_na *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	var_na mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ប្រសិនបើ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	បើមិនដូច្នេះ ប្រសិនបើ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	/* Skip base ISA extensions */
	សម្រាប់ (; i < isa_len; i++) {
		ប្រសិនបើ (isa[i] == '_')
			bha_nga;
	}

	ខណៈពេល (i < isa_len) {
		ប្រសិនបើ (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ខណៈពេល ((i < isa_len) && (isa[i] != '_') &&
		       (j < (parimaa_na(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ប្រសិនបើ (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			ប្រសិនបើ (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		សម្រាប់ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ត្រឡប់ 0;
}

static void fdt_parse_isa_extensions_one_hart(const var_na *isa,
					      unsigned long *extensions,
					      ចំនួនគត់ len)
{
	size_t i;

	សម្រាប់ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ប្រសិនបើ (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ចំនួនគត់ fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa sbi_hart_features *hfeatures;
	ចំនួនគត់ err, cpu_offset, cpus_offset, len;

	ប្រសិនបើ (!fdt)
		ត្រឡប់ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ប្រសិនបើ (cpus_offset < 0)
		ត្រឡប់ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ប្រសិនបើ (err)
			continue;

		ប្រសិនបើ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ប្រសិនបើ (!scratch)
			ត្រឡប់ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ប្រសិនបើ (!hfeatures)
			ត្រឡប់ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ប្រសិនបើ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const var_na *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ប្រសិនបើ (!val || len <= 0)
			ត្រឡប់ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const var_na *)val, hfeatures->extensions);
		ប្រសិនបើ (err)
			ត្រឡប់ err;
	}

	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_parse_uart_node_common(const void *fdt, ចំនួនគត់ nodeoffset,
				      sa_nracanaa platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	ចំនួនគត់ len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ប្រសិនបើ (nodeoffset < 0 || !uart || !fdt)
		ត្រឡប់ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		ត្រឡប់ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ប្រសិនបើ (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ប្រសិនបើ (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		uart->baud = default_baud;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_gaisler_uart_node(const void *fdt, ចំនួនគត់ nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ត្រឡប់ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ចំនួនគត់ fdt_parse_renesas_scif_node(const void *fdt, ចំនួនគត់ nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ត្រឡប់ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ចំនួនគត់ fdt_parse_shakti_uart_node(const void *fdt, ចំនួនគត់ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ត្រឡប់ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ចំនួនគត់ fdt_parse_sifive_uart_node(const void *fdt, ចំនួនគត់ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ត្រឡប់ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ចំនួនគត់ fdt_parse_uart_node(const void *fdt, ចំនួនគត់ nodeoffset,
			sa_nracanaa platform_uart_data *uart)
{
	ចំនួនគត់ len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ប្រសិនបើ (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ប្រសិនបើ (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ប្រសិនបើ (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_uart8250(const void *fdt, sa_nracanaa platform_uart_data *uart,
		   const var_na *compatible)
{
	ចំនួនគត់ nodeoffset;

	ប្រសិនបើ (!compatible || !uart || !fdt)
		ត្រឡប់ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ប្រសិនបើ (nodeoffset < 0)
		ត្រឡប់ nodeoffset;

	ត្រឡប់ fdt_parse_uart_node(fdt, nodeoffset, uart);
}

ចំនួនគត់ fdt_parse_xlnx_uartlite_node(const void *fdt, ចំនួនគត់ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ត្រឡប់ fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static ចំនួនគត់ fdt_aplic_find_imsic_node(const void *fdt, ចំនួនគត់ nodeoff,
				     sa_nracanaa imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	ចំនួនគត់ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ប្រសិនបើ (val && len >= parimaa_na(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ប្រសិនបើ (noff < 0)
			ត្រឡប់ noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		rc = imsic_data_check(imsic);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		ប្រសិនបើ (imsic->targets_mmode == mmode) {
			ត្រឡប់ 0;
		}
	} បើមិនដូច្នេះ {
		ត្រឡប់ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ប្រសិនបើ (!val || len < parimaa_na(fdt32_t))
		ត្រឡប់ SBI_ENODEV;

	len /= parimaa_na(fdt32_t);

	សម្រាប់ (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ប្រសិនបើ (noff < 0)
			ត្រឡប់ noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		ប្រសិនបើ (!rc)
			bha_nga;
	}

	ត្រឡប់ rc;
}

ចំនួនគត់ fdt_parse_aplic_node(const void *fdt, ចំនួនគត់ nodeoff, sa_nracanaa aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	sa_nracanaa imsic_data imsic = { 0 };
	ចំនួនគត់ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	sa_nracanaa aplic_delegate_data *deleg;

	ប្រសិនបើ (nodeoff < 0 || !aplic || !fdt)
		ត្រឡប់ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		ត្រឡប់ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ប្រសិនបើ (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ប្រសិនបើ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		សម្រាប់ (i = 0; i < len; i += 2) {
			ប្រសិនបើ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				bha_nga;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	ប្រសិនបើ (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ប្រសិនបើ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ត្រឡប់ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	ប្រសិនបើ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ប្រសិនបើ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ត្រឡប់ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	សម្រាប់ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ប្រសិនបើ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ប្រសិនបើ (!del || len < (3 * parimaa_na(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / parimaa_na(fdt32_t);
	សម្រាប់ (i = 0; i < dcnt; i += 3) {
		ប្រសិនបើ (d >= APLIC_MAX_DELEGATE)
			bha_nga;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ប្រសិនបើ (!val || len < parimaa_na(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / parimaa_na(fdt32_t);
		សម្រាប់ (j = 0; j < len; j++) {
			ប្រសិនបើ (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			bha_nga;
		}

		ប្រសិនបើ (child_found) {
			d++;
		} បើមិនដូច្នេះ {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ត្រឡប់ 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	ចំនួនគត់ i, len, noff = 0;

	ប្រសិនបើ (!fdt)
		ត្រឡប់ false;

	ខណៈពេល ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ប្រសិនបើ (val && len > parimaa_na(fdt32_t)) {
			len = len / parimaa_na(fdt32_t);
			សម្រាប់ (i = 0; i < len; i += 2) {
				ប្រសិនបើ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ត្រឡប់ true;
			}
		}
	}

	ត្រឡប់ false;
}

ចំនួនគត់ fdt_parse_imsic_node(const void *fdt, ចំនួនគត់ nodeoff, sa_nracanaa imsic_data *imsic)
{
	const fdt32_t *val;
	sa_nracanaa imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	ចំនួនគត់ i, rc, len, nr_parent_irqs;

	ប្រសិនបើ (nodeoff < 0 || !imsic || !fdt)
		ត្រឡប់ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ប្រសិនបើ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		nr_parent_irqs = len / 2;
		សម្រាប់ (i = 0; i < len; i += 2) {
			ប្រសិនបើ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				bha_nga;
			}
		}
	} បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ប្រសិនបើ (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ប្រសិនបើ (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} បើមិនដូច្នេះ {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ប្រសិនបើ ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ប្រសិនបើ (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ប្រសិនបើ (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ប្រសិនបើ (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	សម្រាប់ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	សម្រាប់ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ប្រសិនបើ (rc < 0 || !reg_size)
			bha_nga;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ប្រសិនបើ (!imsic->regs[0].size)
		ត្រឡប់ SBI_EINVAL;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_plic_node(const void *fdt, ចំនួនគត់ nodeoffset, sa_nracanaa plic_data *plic)
{
	ចំនួនគត់ len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ប្រសិនបើ (nodeoffset < 0 || !plic || !fdt)
		ត្រឡប់ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		ត្រឡប់ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ប្រសិនបើ (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_plic(const void *fdt, sa_nracanaa plic_data *plic, const var_na *compat)
{
	ចំនួនគត់ nodeoffset;

	ប្រសិនបើ (!compat || !plic || !fdt)
		ត្រឡប់ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	ប្រសិនបើ (nodeoffset < 0)
		ត្រឡប់ nodeoffset;

	ត្រឡប់ fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static ចំនួនគត់ fdt_get_aclint_addr_size_by_name(const void *fdt, ចំនួនគត់ nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	ចំនួនគត់ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		ត្រឡប់ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_get_aclint_addr_size(const void *fdt, ចំនួនគត់ nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	ចំនួនគត់ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		ត្រឡប់ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ប្រសិនបើ (out_addr2)
		*out_addr2 = reg_addr;
	ប្រសិនបើ (out_size2)
		*out_size2 = reg_size;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_aclint_node(const void *fdt, ចំនួនគត់ nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	ចំនួនគត់ i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ប្រសិនបើ (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	បើមិនដូច្នេះ
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ប្រសិនបើ (rc)
		ត្រឡប់ rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ប្រសិនបើ (!val || count < parimaa_na(fdt32_t))
		ត្រឡប់ 0;
	count = count / parimaa_na(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	សម្រាប់ (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ប្រសិនបើ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ប្រសិនបើ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ប្រសិនបើ (rc)
			continue;

		ប្រសិនបើ (match_hwirq == hwirq) {
			ប្រសិនបើ (hartid < first_hartid)
				first_hartid = hartid;
			ប្រសិនបើ (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ប្រសិនបើ ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_plmt_node(const void *fdt, ចំនួនគត់ nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	ចំនួនគត់ rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ប្រសិនបើ (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		ត្រឡប់ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0)
		ត្រឡប់ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ប្រសិនបើ (!val || count < parimaa_na(fdt32_t))
		ត្រឡប់ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	សម្រាប់ (i = 0; i < (count / 2); i++) {
		ចំនួនគត់ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ប្រសិនបើ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ប្រសិនបើ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ប្រសិនបើ (rc)
			continue;

		ប្រសិនបើ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ប្រសិនបើ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_plicsw_node(const void *fdt, ចំនួនគត់ nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	ចំនួនគត់ rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ប្រសិនបើ (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		ត្រឡប់ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ប្រសិនបើ (rc < 0)
		ត្រឡប់ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ប្រសិនបើ (!val || count < parimaa_na(fdt32_t))
		ត្រឡប់ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	សម្រាប់ (i = 0; i < (count / 2); i++) {
		ចំនួនគត់ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ប្រសិនបើ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ប្រសិនបើ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ប្រសិនបើ (rc)
			continue;

		ប្រសិនបើ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ប្រសិនបើ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const var_na *compatible)
{
	ចំនួនគត់ nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ប្រសិនបើ (nodeoffset < 0)
		ត្រឡប់ nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	ប្រសិនបើ (rc < 0 || !addr)
		ត្រឡប់ SBI_ENODEV;

	ត្រឡប់ 0;
}
