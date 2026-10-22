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

ຈຳນວນເຕັມ fdt_parse_phandle_with_args(const void *fdt, ຈຳນວນເຕັມ nodeoff,
				const var_na *prop, const var_na *cells_prop,
				ຈຳນວນເຕັມ index, sa_nracanaa fdt_phandle_args *out_args)
{
	u32 i, pcells;
	ຈຳນວນເຕັມ len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	ຖ້າ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ສົ່ງຄືນ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ຖ້າ (!list)
		ສົ່ງຄືນ SBI_ENOENT;
	list_end = list + (len / parimaa_na(*list));

	ໃນຂະນະທີ່ (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ຖ້າ (pnodeoff < 0)
			ສົ່ງຄືນ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ຖ້າ (!val)
			ສົ່ງຄືນ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ຖ້າ (FDT_MAX_PHANDLE_ARGS < pcells)
			ສົ່ງຄືນ SBI_EINVAL;
		ຖ້າ (list + pcells > list_end)
			ສົ່ງຄືນ SBI_ENOENT;

		ຖ້າ (index > 0) {
			list += pcells;
			index--;
		} ບໍ່ດັ່ງນັ້ນ {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			ສຳລັບ (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ສົ່ງຄືນ 0;
		}
	}

	ສົ່ງຄືນ SBI_ENOENT;
}

static ຈຳນວນເຕັມ fdt_translate_address(const void *fdt, uint64_t reg, ຈຳນວນເຕັມ parent,
				 uint64_t *addr)
{
	ຈຳນວນເຕັມ i, rlen;
	ຈຳນວນເຕັມ cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ຖ້າ (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ຖ້າ (cell_child_addr < 1)
			ສົ່ງຄືນ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ຖ້າ (cell_parent_addr < 1)
			ສົ່ງຄືນ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ຖ້າ (cell_size < 0)
			ສົ່ງຄືນ SBI_ENODEV;

		ສຳລັບ (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		ສຳລັບ (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		ສຳລັບ (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ຖ້າ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ສົ່ງຄືນ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} ບໍ່ດັ່ງນັ້ນ {
		/* No translation required */
		*addr = reg;
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_get_node_addr_size(const void *fdt, ຈຳນວນເຕັມ node, ຈຳນວນເຕັມ index,
			   uint64_t *addr, uint64_t *size)
{
	ຈຳນວນເຕັມ parent, len, i, rc;
	ຈຳນວນເຕັມ cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ຖ້າ (!fdt || node < 0 || index < 0)
		ສົ່ງຄືນ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	ຖ້າ (parent < 0)
		ສົ່ງຄືນ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ຖ້າ (cell_addr < 1)
		ສົ່ງຄືນ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ຖ້າ (cell_size < 0)
		ສົ່ງຄືນ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	ຖ້າ (!prop_addr)
		ສົ່ງຄືນ SBI_ENODEV;

	ຖ້າ ((len / parimaa_na(u32)) <= (index * (cell_addr + cell_size)))
		ສົ່ງຄືນ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ຖ້າ (addr) {
		ສຳລັບ (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			ຖ້າ (parent < 0)
				bha_nga;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ຖ້າ (rc)
				bha_nga;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ໃນຂະນະທີ່ (1);
	}
	temp = 0;

	ຖ້າ (size) {
		ສຳລັບ (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_get_node_addr_size_by_name(const void *fdt, ຈຳນວນເຕັມ node, const var_na *name,
				   uint64_t *addr, uint64_t *size)
{
	ຈຳນວນເຕັມ i, j, count;
	const var_na *val;
	const var_na *regname;

	ຖ້າ (!fdt || node < 0 || !name)
		ສົ່ງຄືນ SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	ຖ້າ (!val)
		ສົ່ງຄືນ SBI_ENODEV;

	ສຳລັບ (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		ຖ້າ (strcmp(name, regname) == 0)
			ສົ່ງຄືນ fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	ສົ່ງຄືນ SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, ຈຳນວນເຕັມ nodeoff)
{
	ຈຳນວນເຕັມ len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ຖ້າ (!prop)
		ສົ່ງຄືນ true;

	ຖ້າ (!strncmp(prop, "okay", strlen("okay")))
		ສົ່ງຄືນ true;

	ຖ້າ (!strncmp(prop, "ok", strlen("ok")))
		ສົ່ງຄືນ true;

	ສົ່ງຄືນ false;
}

ຈຳນວນເຕັມ fdt_parse_hart_id(const void *fdt, ຈຳນວນເຕັມ cpu_offset, u32 *hartid)
{
	ຈຳນວນເຕັມ len;
	const void *prop;
	const fdt32_t *val;

	ຖ້າ (!fdt || cpu_offset < 0)
		ສົ່ງຄືນ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ຖ້າ (!prop || !len)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (strncmp (prop, "cpu", strlen ("cpu")))
		ສົ່ງຄືນ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ຖ້າ (!val || len < parimaa_na(fdt32_t))
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (len > parimaa_na(fdt32_t))
		val++;

	ຖ້າ (hartid)
		*hartid = fdt32_to_cpu(*val);

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_cbom_block_size(const void *fdt, ຈຳນວນເຕັມ cpu_offset, unsigned long *cbom_block_size)
{
	ຈຳນວນເຕັມ len;
	const void *prop;
	const fdt32_t *val;

	ຖ້າ (!fdt || cpu_offset < 0)
		ສົ່ງຄືນ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ຖ້າ (!prop || !len)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (strncmp (prop, "cpu", strlen ("cpu")))
		ສົ່ງຄືນ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ຖ້າ (!val || len < parimaa_na(fdt32_t))
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	ຈຳນວນເຕັມ err, cpu_offset, cpus_offset;

	ຖ້າ (!fdt)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!max_hartid)
		ສົ່ງຄືນ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ຖ້າ (cpus_offset < 0)
		ສົ່ງຄືນ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ຖ້າ (err)
			continue;

		ຖ້າ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		ຖ້າ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ len, cpus_offset;

	ຖ້າ (!fdt || !freq)
		ສົ່ງຄືນ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ຖ້າ (cpus_offset < 0)
		ສົ່ງຄືນ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ຖ້າ (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_ENOENT;

	ສົ່ງຄືນ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static ຈຳນວນເຕັມ fdt_parse_isa_one_hart(const var_na *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	var_na mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ຖ້າ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	/* Skip base ISA extensions */
	ສຳລັບ (; i < isa_len; i++) {
		ຖ້າ (isa[i] == '_')
			bha_nga;
	}

	ໃນຂະນະທີ່ (i < isa_len) {
		ຖ້າ (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ໃນຂະນະທີ່ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (parimaa_na(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ຖ້າ (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			ຖ້າ (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		ສຳລັບ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ສົ່ງຄືນ 0;
}

static void fdt_parse_isa_extensions_one_hart(const var_na *isa,
					      unsigned long *extensions,
					      ຈຳນວນເຕັມ len)
{
	size_t i;

	ສຳລັບ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ຖ້າ (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

ຈຳນວນເຕັມ fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa sbi_hart_features *hfeatures;
	ຈຳນວນເຕັມ err, cpu_offset, cpus_offset, len;

	ຖ້າ (!fdt)
		ສົ່ງຄືນ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ຖ້າ (cpus_offset < 0)
		ສົ່ງຄືນ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ຖ້າ (err)
			continue;

		ຖ້າ (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ຖ້າ (!scratch)
			ສົ່ງຄືນ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ຖ້າ (!hfeatures)
			ສົ່ງຄືນ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ຖ້າ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const var_na *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ຖ້າ (!val || len <= 0)
			ສົ່ງຄືນ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const var_na *)val, hfeatures->extensions);
		ຖ້າ (err)
			ສົ່ງຄືນ err;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_parse_uart_node_common(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				      sa_nracanaa platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	ຈຳນວນເຕັມ len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ຖ້າ (nodeoffset < 0 || !uart || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		ສົ່ງຄືນ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ຖ້າ (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ຖ້າ (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		uart->baud = default_baud;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_gaisler_uart_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ສົ່ງຄືນ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

ຈຳນວນເຕັມ fdt_parse_renesas_scif_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	ສົ່ງຄືນ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

ຈຳນວນເຕັມ fdt_parse_shakti_uart_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ສົ່ງຄືນ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

ຈຳນວນເຕັມ fdt_parse_sifive_uart_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ສົ່ງຄືນ fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

ຈຳນວນເຕັມ fdt_parse_uart_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			sa_nracanaa platform_uart_data *uart)
{
	ຈຳນວນເຕັມ len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ຖ້າ (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ຖ້າ (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ຖ້າ (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_uart8250(const void *fdt, sa_nracanaa platform_uart_data *uart,
		   const var_na *compatible)
{
	ຈຳນວນເຕັມ nodeoffset;

	ຖ້າ (!compatible || !uart || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ຖ້າ (nodeoffset < 0)
		ສົ່ງຄືນ nodeoffset;

	ສົ່ງຄືນ fdt_parse_uart_node(fdt, nodeoffset, uart);
}

ຈຳນວນເຕັມ fdt_parse_xlnx_uartlite_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	ສົ່ງຄືນ fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static ຈຳນວນເຕັມ fdt_aplic_find_imsic_node(const void *fdt, ຈຳນວນເຕັມ nodeoff,
				     sa_nracanaa imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ຖ້າ (val && len >= parimaa_na(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ຖ້າ (noff < 0)
			ສົ່ງຄືນ noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		rc = imsic_data_check(imsic);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		ຖ້າ (imsic->targets_mmode == mmode) {
			ສົ່ງຄືນ 0;
		}
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ຖ້າ (!val || len < parimaa_na(fdt32_t))
		ສົ່ງຄືນ SBI_ENODEV;

	len /= parimaa_na(fdt32_t);

	ສຳລັບ (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ຖ້າ (noff < 0)
			ສົ່ງຄືນ noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		ຖ້າ (!rc)
			bha_nga;
	}

	ສົ່ງຄືນ rc;
}

ຈຳນວນເຕັມ fdt_parse_aplic_node(const void *fdt, ຈຳນວນເຕັມ nodeoff, sa_nracanaa aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	sa_nracanaa imsic_data imsic = { 0 };
	ຈຳນວນເຕັມ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	sa_nracanaa aplic_delegate_data *deleg;

	ຖ້າ (nodeoff < 0 || !aplic || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		ສົ່ງຄືນ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ຖ້າ (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ຖ້າ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		ສຳລັບ (i = 0; i < len; i += 2) {
			ຖ້າ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				bha_nga;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	ຖ້າ (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ຖ້າ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ສົ່ງຄືນ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	ຖ້າ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ຖ້າ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ສົ່ງຄືນ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	ສຳລັບ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ຖ້າ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ຖ້າ (!del || len < (3 * parimaa_na(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / parimaa_na(fdt32_t);
	ສຳລັບ (i = 0; i < dcnt; i += 3) {
		ຖ້າ (d >= APLIC_MAX_DELEGATE)
			bha_nga;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ຖ້າ (!val || len < parimaa_na(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / parimaa_na(fdt32_t);
		ສຳລັບ (j = 0; j < len; j++) {
			ຖ້າ (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			bha_nga;
		}

		ຖ້າ (child_found) {
			d++;
		} ບໍ່ດັ່ງນັ້ນ {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ສົ່ງຄືນ 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ i, len, noff = 0;

	ຖ້າ (!fdt)
		ສົ່ງຄືນ false;

	ໃນຂະນະທີ່ ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ຖ້າ (val && len > parimaa_na(fdt32_t)) {
			len = len / parimaa_na(fdt32_t);
			ສຳລັບ (i = 0; i < len; i += 2) {
				ຖ້າ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ສົ່ງຄືນ true;
			}
		}
	}

	ສົ່ງຄືນ false;
}

ຈຳນວນເຕັມ fdt_parse_imsic_node(const void *fdt, ຈຳນວນເຕັມ nodeoff, sa_nracanaa imsic_data *imsic)
{
	const fdt32_t *val;
	sa_nracanaa imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	ຈຳນວນເຕັມ i, rc, len, nr_parent_irqs;

	ຖ້າ (nodeoff < 0 || !imsic || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ຖ້າ (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		nr_parent_irqs = len / 2;
		ສຳລັບ (i = 0; i < len; i += 2) {
			ຖ້າ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				bha_nga;
			}
		}
	} ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ຖ້າ (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ຖ້າ (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} ບໍ່ດັ່ງນັ້ນ {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ຖ້າ ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ຖ້າ (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ຖ້າ (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ຖ້າ (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	ສຳລັບ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	ສຳລັບ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ຖ້າ (rc < 0 || !reg_size)
			bha_nga;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ຖ້າ (!imsic->regs[0].size)
		ສົ່ງຄືນ SBI_EINVAL;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_plic_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset, sa_nracanaa plic_data *plic)
{
	ຈຳນວນເຕັມ len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ຖ້າ (nodeoffset < 0 || !plic || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		ສົ່ງຄືນ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ຖ້າ (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_plic(const void *fdt, sa_nracanaa plic_data *plic, const var_na *compat)
{
	ຈຳນວນເຕັມ nodeoffset;

	ຖ້າ (!compat || !plic || !fdt)
		ສົ່ງຄືນ SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	ຖ້າ (nodeoffset < 0)
		ສົ່ງຄືນ nodeoffset;

	ສົ່ງຄືນ fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static ຈຳນວນເຕັມ fdt_get_aclint_addr_size_by_name(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	ຈຳນວນເຕັມ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		ສົ່ງຄືນ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_get_aclint_addr_size(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	ຈຳນວນເຕັມ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		ສົ່ງຄືນ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ຖ້າ (out_addr2)
		*out_addr2 = reg_addr;
	ຖ້າ (out_size2)
		*out_size2 = reg_size;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_aclint_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ຖ້າ (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	ບໍ່ດັ່ງນັ້ນ
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ຖ້າ (rc)
		ສົ່ງຄືນ rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ຖ້າ (!val || count < parimaa_na(fdt32_t))
		ສົ່ງຄືນ 0;
	count = count / parimaa_na(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	ສຳລັບ (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ຖ້າ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ຖ້າ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ຖ້າ (rc)
			continue;

		ຖ້າ (match_hwirq == hwirq) {
			ຖ້າ (hartid < first_hartid)
				first_hartid = hartid;
			ຖ້າ (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ຖ້າ ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_plmt_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ຖ້າ (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		ສົ່ງຄືນ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0)
		ສົ່ງຄືນ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ຖ້າ (!val || count < parimaa_na(fdt32_t))
		ສົ່ງຄືນ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	ສຳລັບ (i = 0; i < (count / 2); i++) {
		ຈຳນວນເຕັມ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ຖ້າ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ຖ້າ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ຖ້າ (rc)
			continue;

		ຖ້າ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ຖ້າ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_plicsw_node(const void *fdt, ຈຳນວນເຕັມ nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	ຈຳນວນເຕັມ rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ຖ້າ (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		ສົ່ງຄືນ SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ຖ້າ (rc < 0)
		ສົ່ງຄືນ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ຖ້າ (!val || count < parimaa_na(fdt32_t))
		ສົ່ງຄືນ 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	ສຳລັບ (i = 0; i < (count / 2); i++) {
		ຈຳນວນເຕັມ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ຖ້າ (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ຖ້າ (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ຖ້າ (rc)
			continue;

		ຖ້າ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ຖ້າ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const var_na *compatible)
{
	ຈຳນວນເຕັມ nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ຖ້າ (nodeoffset < 0)
		ສົ່ງຄືນ nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	ຖ້າ (rc < 0 || !addr)
		ສົ່ງຄືນ SBI_ENODEV;

	ສົ່ງຄືນ 0;
}
