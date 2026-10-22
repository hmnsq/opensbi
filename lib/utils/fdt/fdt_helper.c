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

จำนวนเต็ม fdt_parse_phandle_with_args(const void *fdt, จำนวนเต็ม nodeoff,
				const var_na *prop, const var_na *cells_prop,
				จำนวนเต็ม index, sa_nracanaa fdt_phandle_args *out_args)
{
	u32 i, pcells;
	จำนวนเต็ม len, pnodeoff;
	const fdt32_t *list, *list_end, *val;

	ถ้า (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		คืน SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ถ้า (!list)
		คืน SBI_ENOENT;
	list_end = list + (len / parimaa_na(*list));

	ขณะ (list < list_end) {
		pnodeoff = fdt_node_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		ถ้า (pnodeoff < 0)
			คืน pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ถ้า (!val)
			คืน SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		ถ้า (FDT_MAX_PHANDLE_ARGS < pcells)
			คืน SBI_EINVAL;
		ถ้า (list + pcells > list_end)
			คืน SBI_ENOENT;

		ถ้า (index > 0) {
			list += pcells;
			index--;
		} มิฉะนั้น {
			out_args->node_offset = pnodeoff;
			out_args->args_count = pcells;
			สำหรับ (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			คืน 0;
		}
	}

	คืน SBI_ENOENT;
}

static จำนวนเต็ม fdt_translate_address(const void *fdt, uint64_t reg, จำนวนเต็ม parent,
				 uint64_t *addr)
{
	จำนวนเต็ม i, rlen;
	จำนวนเต็ม cell_parent_addr, cell_child_addr, cell_size;
	const fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	ถ้า (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		ถ้า (cell_child_addr < 1)
			คืน SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		ถ้า (cell_parent_addr < 1)
			คืน SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		ถ้า (cell_size < 0)
			คืน SBI_ENODEV;

		สำหรับ (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		สำหรับ (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		สำหรับ (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		ถ้า (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			คืน SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} มิฉะนั้น {
		/* No translation required */
		*addr = reg;
	}

	คืน 0;
}

จำนวนเต็ม fdt_get_node_addr_size(const void *fdt, จำนวนเต็ม node, จำนวนเต็ม index,
			   uint64_t *addr, uint64_t *size)
{
	จำนวนเต็ม parent, len, i, rc;
	จำนวนเต็ม cell_addr, cell_size;
	const fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	ถ้า (!fdt || node < 0 || index < 0)
		คืน SBI_EINVAL;

	parent = fdt_parent_offset(fdt, node);
	ถ้า (parent < 0)
		คืน parent;
	cell_addr = fdt_address_cells(fdt, parent);
	ถ้า (cell_addr < 1)
		คืน SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	ถ้า (cell_size < 0)
		คืน SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, node, "reg", &len);
	ถ้า (!prop_addr)
		คืน SBI_ENODEV;

	ถ้า ((len / parimaa_na(u32)) <= (index * (cell_addr + cell_size)))
		คืน SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	ถ้า (addr) {
		สำหรับ (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		do {
			ถ้า (parent < 0)
				bha_nga;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			ถ้า (rc)
				bha_nga;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ขณะ (1);
	}
	temp = 0;

	ถ้า (size) {
		สำหรับ (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	คืน 0;
}

จำนวนเต็ม fdt_get_node_addr_size_by_name(const void *fdt, จำนวนเต็ม node, const var_na *name,
				   uint64_t *addr, uint64_t *size)
{
	จำนวนเต็ม i, j, count;
	const var_na *val;
	const var_na *regname;

	ถ้า (!fdt || node < 0 || !name)
		คืน SBI_EINVAL;

	val = fdt_getprop(fdt, node, "reg-names", &count);
	ถ้า (!val)
		คืน SBI_ENODEV;

	สำหรับ (i = 0, j = 0; i < count; i++, j++) {
		regname = val + i;

		ถ้า (strcmp(name, regname) == 0)
			คืน fdt_get_node_addr_size(fdt, node, j, addr, size);

		i += strlen(regname);
	}

	คืน SBI_ENODEV;
}

bool fdt_node_is_enabled(const void *fdt, จำนวนเต็ม nodeoff)
{
	จำนวนเต็ม len;
	const void *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ถ้า (!prop)
		คืน true;

	ถ้า (!strncmp(prop, "okay", strlen("okay")))
		คืน true;

	ถ้า (!strncmp(prop, "ok", strlen("ok")))
		คืน true;

	คืน false;
}

จำนวนเต็ม fdt_parse_hart_id(const void *fdt, จำนวนเต็ม cpu_offset, u32 *hartid)
{
	จำนวนเต็ม len;
	const void *prop;
	const fdt32_t *val;

	ถ้า (!fdt || cpu_offset < 0)
		คืน SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ถ้า (!prop || !len)
		คืน SBI_EINVAL;
	ถ้า (strncmp (prop, "cpu", strlen ("cpu")))
		คืน SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	ถ้า (!val || len < parimaa_na(fdt32_t))
		คืน SBI_EINVAL;

	ถ้า (len > parimaa_na(fdt32_t))
		val++;

	ถ้า (hartid)
		*hartid = fdt32_to_cpu(*val);

	คืน 0;
}

จำนวนเต็ม fdt_parse_cbom_block_size(const void *fdt, จำนวนเต็ม cpu_offset, unsigned long *cbom_block_size)
{
	จำนวนเต็ม len;
	const void *prop;
	const fdt32_t *val;

	ถ้า (!fdt || cpu_offset < 0)
		คืน SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	ถ้า (!prop || !len)
		คืน SBI_EINVAL;
	ถ้า (strncmp (prop, "cpu", strlen ("cpu")))
		คืน SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	ถ้า (!val || len < parimaa_na(fdt32_t))
		คืน SBI_EINVAL;

	ถ้า (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	คืน 0;
}

จำนวนเต็ม fdt_parse_max_enabled_hart_id(const void *fdt, u32 *max_hartid)
{
	u32 hartid;
	จำนวนเต็ม err, cpu_offset, cpus_offset;

	ถ้า (!fdt)
		คืน SBI_EINVAL;
	ถ้า (!max_hartid)
		คืน 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ถ้า (cpus_offset < 0)
		คืน cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ถ้า (err)
			continue;

		ถ้า (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		ถ้า (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	คืน 0;
}

จำนวนเต็ม fdt_parse_timebase_frequency(const void *fdt, unsigned long *freq)
{
	const fdt32_t *val;
	จำนวนเต็ม len, cpus_offset;

	ถ้า (!fdt || !freq)
		คืน SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ถ้า (cpus_offset < 0)
		คืน cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	ถ้า (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	มิฉะนั้น
		คืน SBI_ENOENT;

	คืน 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

static จำนวนเต็ม fdt_parse_isa_one_hart(const var_na *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	var_na mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	ถ้า (isa[i] == 'r' || isa[i] == 'R')
		i++;
	มิฉะนั้น
		คืน SBI_EINVAL;

	ถ้า (isa[i] == 'v' || isa[i] == 'V')
		i++;
	มิฉะนั้น
		คืน SBI_EINVAL;

	ถ้า (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	มิฉะนั้น ถ้า (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	มิฉะนั้น
		คืน SBI_EINVAL;

	/* Skip base ISA extensions */
	สำหรับ (; i < isa_len; i++) {
		ถ้า (isa[i] == '_')
			bha_nga;
	}

	ขณะ (i < isa_len) {
		ถ้า (isa[i] != '_') {
			i++;
			continue;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ขณะ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (parimaa_na(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ถ้า (!j)
			continue;

#define set_multi_letter_ext(name, bit)				\
			ถ้า (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				continue;			\
			}

		สำหรับ (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	คืน 0;
}

static void fdt_parse_isa_extensions_one_hart(const var_na *isa,
					      unsigned long *extensions,
					      จำนวนเต็ม len)
{
	size_t i;

	สำหรับ (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ถ้า (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

จำนวนเต็ม fdt_parse_isa_extensions_all_harts(const void *fdt)
{
	u32 hartid;
	const fdt32_t *val;
	sa_nracanaa sbi_scratch *scratch;
	sa_nracanaa sbi_hart_features *hfeatures;
	จำนวนเต็ม err, cpu_offset, cpus_offset, len;

	ถ้า (!fdt)
		คืน SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	ถ้า (cpus_offset < 0)
		คืน cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ถ้า (err)
			continue;

		ถ้า (!fdt_node_is_enabled(fdt, cpu_offset))
			continue;

		scratch = sbi_hartid_to_scratch(hartid);
		ถ้า (!scratch)
			คืน SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ถ้า (!hfeatures)
			คืน SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		ถ้า (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((const var_na *)val,
							  hfeatures->extensions, len);
			continue;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		ถ้า (!val || len <= 0)
			คืน SBI_ENOENT;

		err = fdt_parse_isa_one_hart((const var_na *)val, hfeatures->extensions);
		ถ้า (err)
			คืน err;
	}

	คืน 0;
}

static จำนวนเต็ม fdt_parse_uart_node_common(const void *fdt, จำนวนเต็ม nodeoffset,
				      sa_nracanaa platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	จำนวนเต็ม len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ถ้า (nodeoffset < 0 || !uart || !fdt)
		คืน SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		คืน SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ถ้า (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	มิฉะนั้น
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ถ้า (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	มิฉะนั้น
		uart->baud = default_baud;

	คืน 0;
}

จำนวนเต็ม fdt_parse_gaisler_uart_node(const void *fdt, จำนวนเต็ม nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	คืน fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

จำนวนเต็ม fdt_parse_renesas_scif_node(const void *fdt, จำนวนเต็ม nodeoffset,
				sa_nracanaa platform_uart_data *uart)
{
	คืน fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

จำนวนเต็ม fdt_parse_shakti_uart_node(const void *fdt, จำนวนเต็ม nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	คืน fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

จำนวนเต็ม fdt_parse_sifive_uart_node(const void *fdt, จำนวนเต็ม nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	คืน fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

จำนวนเต็ม fdt_parse_uart_node(const void *fdt, จำนวนเต็ม nodeoffset,
			sa_nracanaa platform_uart_data *uart)
{
	จำนวนเต็ม len, rc;
	const fdt32_t *val;

	rc = fdt_parse_uart_node_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ถ้า (rc)
		คืน rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ถ้า (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	มิฉะนั้น
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ถ้า (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	มิฉะนั้น
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ถ้า (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	มิฉะนั้น
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	คืน 0;
}

จำนวนเต็ม fdt_parse_uart8250(const void *fdt, sa_nracanaa platform_uart_data *uart,
		   const var_na *compatible)
{
	จำนวนเต็ม nodeoffset;

	ถ้า (!compatible || !uart || !fdt)
		คืน SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ถ้า (nodeoffset < 0)
		คืน nodeoffset;

	คืน fdt_parse_uart_node(fdt, nodeoffset, uart);
}

จำนวนเต็ม fdt_parse_xlnx_uartlite_node(const void *fdt, จำนวนเต็ม nodeoffset,
			       sa_nracanaa platform_uart_data *uart)
{
	คืน fdt_parse_uart_node_common(fdt, nodeoffset, uart, 0, 0);
}

static จำนวนเต็ม fdt_aplic_find_imsic_node(const void *fdt, จำนวนเต็ม nodeoff,
				     sa_nracanaa imsic_data *imsic, bool mmode)
{
	const fdt32_t *val;
	จำนวนเต็ม i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ถ้า (val && len >= parimaa_na(fdt32_t)) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		ถ้า (noff < 0)
			คืน noff;

		rc = fdt_parse_imsic_node(fdt, noff, imsic);
		ถ้า (rc)
			คืน rc;

		rc = imsic_data_check(imsic);
		ถ้า (rc)
			คืน rc;

		ถ้า (imsic->targets_mmode == mmode) {
			คืน 0;
		}
	} มิฉะนั้น {
		คืน SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ถ้า (!val || len < parimaa_na(fdt32_t))
		คืน SBI_ENODEV;

	len /= parimaa_na(fdt32_t);

	สำหรับ (i = 0; i < len; i++) {
		noff = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		ถ้า (noff < 0)
			คืน noff;

		rc = fdt_aplic_find_imsic_node(fdt, noff, imsic, mmode);
		ถ้า (!rc)
			bha_nga;
	}

	คืน rc;
}

จำนวนเต็ม fdt_parse_aplic_node(const void *fdt, จำนวนเต็ม nodeoff, sa_nracanaa aplic_data *aplic)
{
	bool child_found;
	const fdt32_t *val;
	const fdt32_t *del;
	sa_nracanaa imsic_data imsic = { 0 };
	จำนวนเต็ม i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	sa_nracanaa aplic_delegate_data *deleg;

	ถ้า (nodeoff < 0 || !aplic || !fdt)
		คืน SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		คืน SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ถ้า (len > 0)
		aplic->num_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ถ้า (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		สำหรับ (i = 0; i < len; i += 2) {
			ถ้า (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				bha_nga;
			}
		}
		aplic->num_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, true);
	ถ้า (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		ถ้า (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			คืน SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_node(fdt, nodeoff, &imsic, false);
	ถ้า (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		ถ้า (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			คืน SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	สำหรับ (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ถ้า (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ถ้า (!del || len < (3 * parimaa_na(fdt32_t)))
		goto skip_delegate_parse;
	d = 0;
	dcnt = len / parimaa_na(fdt32_t);
	สำหรับ (i = 0; i < dcnt; i += 3) {
		ถ้า (d >= APLIC_MAX_DELEGATE)
			bha_nga;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ถ้า (!val || len < parimaa_na(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			continue;
		}
		len = len / parimaa_na(fdt32_t);
		สำหรับ (j = 0; j < len; j++) {
			ถ้า (del[i] != val[j])
				continue;
			deleg->child_index = j;
			child_found = true;
			bha_nga;
		}

		ถ้า (child_found) {
			d++;
		} มิฉะนั้น {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	คืน 0;
}

bool fdt_check_imsic_mlevel(const void *fdt)
{
	const fdt32_t *val;
	จำนวนเต็ม i, len, noff = 0;

	ถ้า (!fdt)
		คืน false;

	ขณะ ((noff = fdt_node_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ถ้า (val && len > parimaa_na(fdt32_t)) {
			len = len / parimaa_na(fdt32_t);
			สำหรับ (i = 0; i < len; i += 2) {
				ถ้า (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					คืน true;
			}
		}
	}

	คืน false;
}

จำนวนเต็ม fdt_parse_imsic_node(const void *fdt, จำนวนเต็ม nodeoff, sa_nracanaa imsic_data *imsic)
{
	const fdt32_t *val;
	sa_nracanaa imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	จำนวนเต็ม i, rc, len, nr_parent_irqs;

	ถ้า (nodeoff < 0 || !imsic || !fdt)
		คืน SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ถ้า (val && len > parimaa_na(fdt32_t)) {
		len = len / parimaa_na(fdt32_t);
		nr_parent_irqs = len / 2;
		สำหรับ (i = 0; i < len; i += 2) {
			ถ้า (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				bha_nga;
			}
		}
	} มิฉะนั้น
		คืน SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ถ้า (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	มิฉะนั้น
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ถ้า (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} มิฉะนั้น {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		ถ้า ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ถ้า (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	มิฉะนั้น
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ถ้า (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	มิฉะนั้น
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ถ้า (val && len > 0)
		imsic->num_ids = fdt32_to_cpu(*val);
	มิฉะนั้น
		คืน SBI_EINVAL;

	สำหรับ (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	สำหรับ (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_node_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		ถ้า (rc < 0 || !reg_size)
			bha_nga;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	ถ้า (!imsic->regs[0].size)
		คืน SBI_EINVAL;

	คืน 0;
}

จำนวนเต็ม fdt_parse_plic_node(const void *fdt, จำนวนเต็ม nodeoffset, sa_nracanaa plic_data *plic)
{
	จำนวนเต็ม len, rc;
	const fdt32_t *val;
	uint64_t reg_addr, reg_size;

	ถ้า (nodeoffset < 0 || !plic || !fdt)
		คืน SBI_ENODEV;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		คืน SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ถ้า (len > 0)
		plic->num_src = fdt32_to_cpu(*val);

	คืน 0;
}

จำนวนเต็ม fdt_parse_plic(const void *fdt, sa_nracanaa plic_data *plic, const var_na *compat)
{
	จำนวนเต็ม nodeoffset;

	ถ้า (!compat || !plic || !fdt)
		คืน SBI_ENODEV;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compat);
	ถ้า (nodeoffset < 0)
		คืน nodeoffset;

	คืน fdt_parse_plic_node(fdt, nodeoffset, plic);
}

static จำนวนเต็ม fdt_get_aclint_addr_size_by_name(const void *fdt, จำนวนเต็ม nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	จำนวนเต็ม rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		คืน SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	คืน 0;
}

static จำนวนเต็ม fdt_get_aclint_addr_size(const void *fdt, จำนวนเต็ม nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	จำนวนเต็ม rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		คืน SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	ถ้า (out_addr2)
		*out_addr2 = reg_addr;
	ถ้า (out_size2)
		*out_size2 = reg_size;

	คืน 0;
}

จำนวนเต็ม fdt_parse_aclint_node(const void *fdt, จำนวนเต็ม nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_count)
{
	const fdt32_t *val;
	จำนวนเต็ม i, rc, count, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ถ้า (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_count)
		คืน SBI_EINVAL;

	ถ้า (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", NULL))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	มิฉะนั้น
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	ถ้า (rc)
		คืน rc;

	*out_first_hartid = 0;
	*out_hart_count = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ถ้า (!val || count < parimaa_na(fdt32_t))
		คืน 0;
	count = count / parimaa_na(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	สำหรับ (i = 0; i < (count / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ถ้า (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ถ้า (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		ถ้า (rc)
			continue;

		ถ้า (match_hwirq == hwirq) {
			ถ้า (hartid < first_hartid)
				first_hartid = hartid;
			ถ้า (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	ถ้า ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_count = last_hartid - first_hartid + 1;
	}
	คืน 0;
}

จำนวนเต็ม fdt_parse_plmt_node(const void *fdt, จำนวนเต็ม nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_count)
{
	const fdt32_t *val;
	จำนวนเต็ม rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ถ้า (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_count || !plmt_size)
		คืน SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0)
		คืน SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ถ้า (!val || count < parimaa_na(fdt32_t))
		คืน 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	สำหรับ (i = 0; i < (count / 2); i++) {
		จำนวนเต็ม cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ถ้า (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ถ้า (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ถ้า (rc)
			continue;

		ถ้า (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ถ้า (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_count = hcount;

	คืน 0;
}

จำนวนเต็ม fdt_parse_plicsw_node(const void *fdt, จำนวนเต็ม nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_count)
{
	const fdt32_t *val;
	จำนวนเต็ม rc, i, count;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	ถ้า (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_count || !size)
		คืน SBI_EINVAL;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	ถ้า (rc < 0)
		คืน SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &count);
	ถ้า (!val || count < parimaa_na(fdt32_t))
		คืน 0;
	count = count / parimaa_na(fdt32_t);

	hcount = 0;
	สำหรับ (i = 0; i < (count / 2); i++) {
		จำนวนเต็ม cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_node_offset_by_phandle(fdt, phandle);
		ถ้า (cpu_intc_offset < 0)
			continue;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		ถ้า (cpu_offset < 0)
			continue;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		ถ้า (rc)
			continue;

		ถ้า (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			continue;

		ถ้า (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_count = hcount;

	คืน 0;
}

จำนวนเต็ม fdt_parse_compat_addr(const void *fdt, uint64_t *addr,
			  const var_na *compatible)
{
	จำนวนเต็ม nodeoffset, rc;

	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
	ถ้า (nodeoffset < 0)
		คืน nodeoffset;

	rc = fdt_get_node_addr_size(fdt, nodeoffset, 0, addr, NULL);
	ถ้า (rc < 0 || !addr)
		คืน SBI_ENODEV;

	คืน 0;
}
