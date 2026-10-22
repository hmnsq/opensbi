// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_hartmask.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/irqchip/aplic.h>
#অন্তর্ভাব <sbi_utils/irqchip/imsic.h>
#অন্তর্ভাব <sbi_utils/irqchip/plic.h>

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

পূর্ণসংখ্যা fdt_parse_phandle_with_args(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				ধ্রুবক অক্ষর *prop, ধ্রুবক অক্ষর *cells_prop,
				পূর্ণসংখ্যা সূচক_2, কাঠামো fdt_phandle_args *out_args)
{
	u32 i, pcells;
	পূর্ণসংখ্যা len, pnodeoff;
	ধ্রুবক fdt32_t *list, *list_end, *val;

	যদি (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ফেরত SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	যদি (!list)
		ফেরত SBI_ENOENT;
	list_end = list + (len / আকার(*list));

	যখন (list < list_end) {
		pnodeoff = fdt_গ্রন্থি_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		যদি (pnodeoff < 0)
			ফেরত pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		যদি (!val)
			ফেরত SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		যদি (FDT_MAX_PHANDLE_ARGS < pcells)
			ফেরত SBI_EINVAL;
		যদি (list + pcells > list_end)
			ফেরত SBI_ENOENT;

		যদি (সূচক_2 > 0) {
			list += pcells;
			সূচক_2--;
		} অন্যথা {
			out_args->গ্রন্থি_offset = pnodeoff;
			out_args->args_গুনতি = pcells;
			জন্য (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ফেরত 0;
		}
	}

	ফেরত SBI_ENOENT;
}

স্থির পূর্ণসংখ্যা fdt_translate_address(ধ্রুবক শূন্য *fdt, uint64_t reg, পূর্ণসংখ্যা parent,
				 uint64_t *addr)
{
	পূর্ণসংখ্যা i, rlen;
	পূর্ণসংখ্যা cell_parent_addr, cell_child_addr, cell_আকার;
	ধ্রুবক fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	যদি (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		যদি (cell_child_addr < 1)
			ফেরত SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		যদি (cell_parent_addr < 1)
			ফেরত SBI_ENODEV;

		cell_আকার = fdt_আকার_cells(fdt, parent);
		যদি (cell_আকার < 0)
			ফেরত SBI_ENODEV;

		জন্য (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		জন্য (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		জন্য (i = 0; i < cell_আকার; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		যদি (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ফেরত SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} অন্যথা {
		/* No translation required */
		*addr = reg;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_get_গ্রন্থি_addr_আকার(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা গ্রন্থি, পূর্ণসংখ্যা সূচক_2,
			   uint64_t *addr, uint64_t *আকার_2)
{
	পূর্ণসংখ্যা parent, len, i, rc;
	পূর্ণসংখ্যা cell_addr, cell_আকার;
	ধ্রুবক fdt32_t *prop_addr, *prop_আকার;
	uint64_t অস্থায়ী = 0;

	যদি (!fdt || গ্রন্থি < 0 || সূচক_2 < 0)
		ফেরত SBI_EINVAL;

	parent = fdt_parent_offset(fdt, গ্রন্থি);
	যদি (parent < 0)
		ফেরত parent;
	cell_addr = fdt_address_cells(fdt, parent);
	যদি (cell_addr < 1)
		ফেরত SBI_ENODEV;

	cell_আকার = fdt_আকার_cells(fdt, parent);
	যদি (cell_আকার < 0)
		ফেরত SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, গ্রন্থি, "reg", &len);
	যদি (!prop_addr)
		ফেরত SBI_ENODEV;

	যদি ((len / আকার(u32)) <= (সূচক_2 * (cell_addr + cell_আকার)))
		ফেরত SBI_EINVAL;

	prop_addr = prop_addr + (সূচক_2 * (cell_addr + cell_আকার));
	prop_আকার = prop_addr + cell_addr;

	যদি (addr) {
		জন্য (i = 0; i < cell_addr; i++)
			অস্থায়ী = (অস্থায়ী << 32) | fdt32_to_cpu(*prop_addr++);
		করো {
			যদি (parent < 0)
				বিরতি;
			rc  = fdt_translate_address(fdt, অস্থায়ী, parent, addr);
			যদি (rc)
				বিরতি;
			parent = fdt_parent_offset(fdt, parent);
			অস্থায়ী = *addr;
		} যখন (1);
	}
	অস্থায়ী = 0;

	যদি (আকার_2) {
		জন্য (i = 0; i < cell_আকার; i++)
			অস্থায়ী = (অস্থায়ী << 32) | fdt32_to_cpu(*prop_আকার++);
		*আকার_2 = অস্থায়ী;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_get_গ্রন্থি_addr_আকার_by_name(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা গ্রন্থি, ধ্রুবক অক্ষর *name,
				   uint64_t *addr, uint64_t *আকার_2)
{
	পূর্ণসংখ্যা i, j, গুনতি;
	ধ্রুবক অক্ষর *val;
	ধ্রুবক অক্ষর *regname;

	যদি (!fdt || গ্রন্থি < 0 || !name)
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, গ্রন্থি, "reg-names", &গুনতি);
	যদি (!val)
		ফেরত SBI_ENODEV;

	জন্য (i = 0, j = 0; i < গুনতি; i++, j++) {
		regname = val + i;

		যদি (strcmp(name, regname) == 0)
			ফেরত fdt_get_গ্রন্থি_addr_আকার(fdt, গ্রন্থি, j, addr, আকার_2);

		i += strlen(regname);
	}

	ফেরত SBI_ENODEV;
}

bool fdt_গ্রন্থি_is_enabled(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff)
{
	পূর্ণসংখ্যা len;
	ধ্রুবক শূন্য *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	যদি (!prop)
		ফেরত true;

	যদি (!strncmp(prop, "okay", strlen("okay")))
		ফেরত true;

	যদি (!strncmp(prop, "ok", strlen("ok")))
		ফেরত true;

	ফেরত false;
}

পূর্ণসংখ্যা fdt_parse_hart_id(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা cpu_offset, u32 *hartid)
{
	পূর্ণসংখ্যা len;
	ধ্রুবক শূন্য *prop;
	ধ্রুবক fdt32_t *val;

	যদি (!fdt || cpu_offset < 0)
		ফেরত SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	যদি (!prop || !len)
		ফেরত SBI_EINVAL;
	যদি (strncmp (prop, "cpu", strlen ("cpu")))
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	যদি (!val || len < আকার(fdt32_t))
		ফেরত SBI_EINVAL;

	যদি (len > আকার(fdt32_t))
		val++;

	যদি (hartid)
		*hartid = fdt32_to_cpu(*val);

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_cbom_block_আকার(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা cpu_offset, unsigned long *cbom_block_আকার)
{
	পূর্ণসংখ্যা len;
	ধ্রুবক শূন্য *prop;
	ধ্রুবক fdt32_t *val;

	যদি (!fdt || cpu_offset < 0)
		ফেরত SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	যদি (!prop || !len)
		ফেরত SBI_EINVAL;
	যদি (strncmp (prop, "cpu", strlen ("cpu")))
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	যদি (!val || len < আকার(fdt32_t))
		ফেরত SBI_EINVAL;

	যদি (cbom_block_আকার)
		*cbom_block_আকার = fdt32_to_cpu(*val);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_max_enabled_hart_id(ধ্রুবক শূন্য *fdt, u32 *max_hartid)
{
	u32 hartid;
	পূর্ণসংখ্যা err, cpu_offset, cpus_offset;

	যদি (!fdt)
		ফেরত SBI_EINVAL;
	যদি (!max_hartid)
		ফেরত 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			চালু;

		যদি (!fdt_গ্রন্থি_is_enabled(fdt, cpu_offset))
			চালু;

		যদি (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_timebase_frequency(ধ্রুবক শূন্য *fdt, unsigned long *freq)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা len, cpus_offset;

	যদি (!fdt || !freq)
		ফেরত SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	যদি (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	অন্যথা
		ফেরত SBI_ENOENT;

	ফেরত 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

স্থির পূর্ণসংখ্যা fdt_parse_isa_one_hart(ধ্রুবক অক্ষর *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	অক্ষর mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	যদি (isa[i] == 'r' || isa[i] == 'R')
		i++;
	অন্যথা
		ফেরত SBI_EINVAL;

	যদি (isa[i] == 'v' || isa[i] == 'V')
		i++;
	অন্যথা
		ফেরত SBI_EINVAL;

	যদি (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	অন্যথা যদি (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	অন্যথা
		ফেরত SBI_EINVAL;

	/* Skip base ISA extensions */
	জন্য (; i < isa_len; i++) {
		যদি (isa[i] == '_')
			বিরতি;
	}

	যখন (i < isa_len) {
		যদি (isa[i] != '_') {
			i++;
			চালু;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		যখন ((i < isa_len) && (isa[i] != '_') &&
		       (j < (আকার(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		যদি (!j)
			চালু;

#define set_multi_letter_ext(name, bit)				\
			যদি (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				চালু;			\
			}

		জন্য (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ফেরত 0;
}

স্থির শূন্য fdt_parse_isa_extensions_one_hart(ধ্রুবক অক্ষর *isa,
					      unsigned long *extensions,
					      পূর্ণসংখ্যা len)
{
	size_t i;

	জন্য (i = 0; i < SBI_HART_EXT_MAX; i++) {
		যদি (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

পূর্ণসংখ্যা fdt_parse_isa_extensions_all_harts(ধ্রুবক শূন্য *fdt)
{
	u32 hartid;
	ধ্রুবক fdt32_t *val;
	কাঠামো sbi_scratch *scratch;
	কাঠামো sbi_hart_features *hfeatures;
	পূর্ণসংখ্যা err, cpu_offset, cpus_offset, len;

	যদি (!fdt)
		ফেরত SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ফেরত cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			চালু;

		যদি (!fdt_গ্রন্থি_is_enabled(fdt, cpu_offset))
			চালু;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			ফেরত SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		যদি (!hfeatures)
			ফেরত SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		যদি (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ধ্রুবক অক্ষর *)val,
							  hfeatures->extensions, len);
			চালু;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		যদি (!val || len <= 0)
			ফেরত SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ধ্রুবক অক্ষর *)val, hfeatures->extensions);
		যদি (err)
			ফেরত err;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_parse_uart_গ্রন্থি_common(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				      কাঠামো platform_uart_তথ্য *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	পূর্ণসংখ্যা len, rc;
	ধ্রুবক fdt32_t *val;
	uint64_t reg_addr, reg_আকার;

	যদি (nodeoffset < 0 || !uart || !fdt)
		ফেরত SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		ফেরত SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	যদি (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	অন্যথা
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	যদি (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	অন্যথা
		uart->baud = default_baud;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_gaisler_uart_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				কাঠামো platform_uart_তথ্য *uart)
{
	ফেরত fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

পূর্ণসংখ্যা fdt_parse_renesas_scif_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				কাঠামো platform_uart_তথ্য *uart)
{
	ফেরত fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

পূর্ণসংখ্যা fdt_parse_shakti_uart_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			       কাঠামো platform_uart_তথ্য *uart)
{
	ফেরত fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

পূর্ণসংখ্যা fdt_parse_sifive_uart_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			       কাঠামো platform_uart_তথ্য *uart)
{
	ফেরত fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

পূর্ণসংখ্যা fdt_parse_uart_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			কাঠামো platform_uart_তথ্য *uart)
{
	পূর্ণসংখ্যা len, rc;
	ধ্রুবক fdt32_t *val;

	rc = fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	যদি (rc)
		ফেরত rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	যদি (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	অন্যথা
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	যদি (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	অন্যথা
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	যদি (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	অন্যথা
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_uart8250(ধ্রুবক শূন্য *fdt, কাঠামো platform_uart_তথ্য *uart,
		   ধ্রুবক অক্ষর *compatible)
{
	পূর্ণসংখ্যা nodeoffset;

	যদি (!compatible || !uart || !fdt)
		ফেরত SBI_ENODEV;

	nodeoffset = fdt_গ্রন্থি_offset_by_compatible(fdt, -1, compatible);
	যদি (nodeoffset < 0)
		ফেরত nodeoffset;

	ফেরত fdt_parse_uart_গ্রন্থি(fdt, nodeoffset, uart);
}

পূর্ণসংখ্যা fdt_parse_xlnx_uartlite_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			       কাঠামো platform_uart_তথ্য *uart)
{
	ফেরত fdt_parse_uart_গ্রন্থি_common(fdt, nodeoffset, uart, 0, 0);
}

স্থির পূর্ণসংখ্যা fdt_aplic_find_imsic_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
				     কাঠামো imsic_তথ্য *imsic, bool mmode)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	যদি (val && len >= আকার(fdt32_t)) {
		noff = fdt_গ্রন্থি_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		যদি (noff < 0)
			ফেরত noff;

		rc = fdt_parse_imsic_গ্রন্থি(fdt, noff, imsic);
		যদি (rc)
			ফেরত rc;

		rc = imsic_তথ্য_check(imsic);
		যদি (rc)
			ফেরত rc;

		যদি (imsic->targets_mmode == mmode) {
			ফেরত 0;
		}
	} অন্যথা {
		ফেরত SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	যদি (!val || len < আকার(fdt32_t))
		ফেরত SBI_ENODEV;

	len /= আকার(fdt32_t);

	জন্য (i = 0; i < len; i++) {
		noff = fdt_গ্রন্থি_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		যদি (noff < 0)
			ফেরত noff;

		rc = fdt_aplic_find_imsic_গ্রন্থি(fdt, noff, imsic, mmode);
		যদি (!rc)
			বিরতি;
	}

	ফেরত rc;
}

পূর্ণসংখ্যা fdt_parse_aplic_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, কাঠামো aplic_তথ্য *aplic)
{
	bool child_found;
	ধ্রুবক fdt32_t *val;
	ধ্রুবক fdt32_t *del;
	কাঠামো imsic_তথ্য imsic = { 0 };
	পূর্ণসংখ্যা i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_আকার;
	কাঠামো aplic_delegate_তথ্য *deleg;

	যদি (nodeoff < 0 || !aplic || !fdt)
		ফেরত SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		ফেরত SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->আকার_2 = reg_আকার;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	যদি (len > 0)
		aplic->অঙ্ক_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	যদি (val && len > আকার(fdt32_t)) {
		len = len / আকার(fdt32_t);
		জন্য (i = 0; i < len; i += 2) {
			যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				বিরতি;
			}
		}
		aplic->অঙ্ক_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_গ্রন্থি(fdt, nodeoff, &imsic, true);
	যদি (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_সূচক_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_সূচক_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_সূচক_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_সূচক_shift;
		যদি (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ফেরত SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_গ্রন্থি(fdt, nodeoff, &imsic, false);
	যদি (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_সূচক_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_সূচক_bits;
		aplic->msicfg_smode.hhxw = imsic.group_সূচক_bits;
		aplic->msicfg_smode.hhxs = imsic.group_সূচক_shift;
		যদি (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ফেরত SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	জন্য (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_সূচক = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	যদি (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	যদি (!del || len < (3 * আকার(fdt32_t)))
		যাও skip_delegate_parse;
	d = 0;
	dcnt = len / আকার(fdt32_t);
	জন্য (i = 0; i < dcnt; i += 3) {
		যদি (d >= APLIC_MAX_DELEGATE)
			বিরতি;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_সূচক = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		যদি (!val || len < আকার(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_সূচক = 0;
			চালু;
		}
		len = len / আকার(fdt32_t);
		জন্য (j = 0; j < len; j++) {
			যদি (del[i] != val[j])
				চালু;
			deleg->child_সূচক = j;
			child_found = true;
			বিরতি;
		}

		যদি (child_found) {
			d++;
		} অন্যথা {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_সূচক = 0;
		}
	}
skip_delegate_parse:

	ফেরত 0;
}

bool fdt_check_imsic_mlevel(ধ্রুবক শূন্য *fdt)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা i, len, noff = 0;

	যদি (!fdt)
		ফেরত false;

	যখন ((noff = fdt_গ্রন্থি_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		যদি (val && len > আকার(fdt32_t)) {
			len = len / আকার(fdt32_t);
			জন্য (i = 0; i < len; i += 2) {
				যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ফেরত true;
			}
		}
	}

	ফেরত false;
}

পূর্ণসংখ্যা fdt_parse_imsic_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, কাঠামো imsic_তথ্য *imsic)
{
	ধ্রুবক fdt32_t *val;
	কাঠামো imsic_regs *regs;
	uint64_t reg_addr, reg_আকার;
	পূর্ণসংখ্যা i, rc, len, nr_parent_irqs;

	যদি (nodeoff < 0 || !imsic || !fdt)
		ফেরত SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	যদি (val && len > আকার(fdt32_t)) {
		len = len / আকার(fdt32_t);
		nr_parent_irqs = len / 2;
		জন্য (i = 0; i < len; i += 2) {
			যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				বিরতি;
			}
		}
	} অন্যথা
		ফেরত SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	যদি (val && len > 0)
		imsic->guest_সূচক_bits = fdt32_to_cpu(*val);
	অন্যথা
		imsic->guest_সূচক_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	যদি (val && len > 0) {
		imsic->hart_সূচক_bits = fdt32_to_cpu(*val);
	} অন্যথা {
		imsic->hart_সূচক_bits = sbi_fls(nr_parent_irqs);
		যদি ((1UL << imsic->hart_সূচক_bits) < nr_parent_irqs)
			imsic->hart_সূচক_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	যদি (val && len > 0)
		imsic->group_সূচক_bits = fdt32_to_cpu(*val);
	অন্যথা
		imsic->group_সূচক_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	যদি (val && len > 0)
		imsic->group_সূচক_shift = fdt32_to_cpu(*val);
	অন্যথা
		imsic->group_সূচক_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	যদি (val && len > 0)
		imsic->অঙ্ক_ids = fdt32_to_cpu(*val);
	অন্যথা
		ফেরত SBI_EINVAL;

	জন্য (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->আকার_2 = 0;
	}

	জন্য (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, i,
					    &reg_addr, &reg_আকার);
		যদি (rc < 0 || !reg_আকার)
			বিরতি;
		regs->addr = reg_addr;
		regs->আকার_2 = reg_আকার;
	}
	যদি (!imsic->regs[0].আকার_2)
		ফেরত SBI_EINVAL;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_plic_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, কাঠামো plic_তথ্য *plic)
{
	পূর্ণসংখ্যা len, rc;
	ধ্রুবক fdt32_t *val;
	uint64_t reg_addr, reg_আকার;

	যদি (nodeoffset < 0 || !plic || !fdt)
		ফেরত SBI_ENODEV;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		ফেরত SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->আকার_2 = reg_আকার;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	যদি (len > 0)
		plic->অঙ্ক_src = fdt32_to_cpu(*val);

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_plic(ধ্রুবক শূন্য *fdt, কাঠামো plic_তথ্য *plic, ধ্রুবক অক্ষর *compat)
{
	পূর্ণসংখ্যা nodeoffset;

	যদি (!compat || !plic || !fdt)
		ফেরত SBI_ENODEV;

	nodeoffset = fdt_গ্রন্থি_offset_by_compatible(fdt, -1, compat);
	যদি (nodeoffset < 0)
		ফেরত nodeoffset;

	ফেরত fdt_parse_plic_গ্রন্থি(fdt, nodeoffset, plic);
}

স্থির পূর্ণসংখ্যা fdt_get_aclint_addr_আকার_by_name(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	পূর্ণসংখ্যা rc;
	uint64_t reg_addr, reg_আকার;

	rc = fdt_get_গ্রন্থি_addr_আকার_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		reg_addr = reg_আকার = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_আকার;

	rc = fdt_get_গ্রন্থি_addr_আকার_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		ফেরত SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_আকার;

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_get_aclint_addr_আকার(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	পূর্ণসংখ্যা rc;
	uint64_t reg_addr, reg_আকার;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		ফেরত SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_আকার;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 1,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0 || !reg_আকার)
		reg_addr = reg_আকার = 0;
	যদি (out_addr2)
		*out_addr2 = reg_addr;
	যদি (out_size2)
		*out_size2 = reg_আকার;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_aclint_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_গুনতি)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা i, rc, গুনতি, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	যদি (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_গুনতি)
		ফেরত SBI_EINVAL;

	যদি (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ফাঁকা))
		rc = fdt_get_aclint_addr_আকার_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	অন্যথা
		rc = fdt_get_aclint_addr_আকার(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	যদি (rc)
		ফেরত rc;

	*out_first_hartid = 0;
	*out_hart_গুনতি = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < আকার(fdt32_t))
		ফেরত 0;
	গুনতি = গুনতি / আকার(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	জন্য (i = 0; i < (গুনতি / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			চালু;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			চালু;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (rc)
			চালু;

		যদি (match_hwirq == hwirq) {
			যদি (hartid < first_hartid)
				first_hartid = hartid;
			যদি (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	যদি ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_গুনতি = last_hartid - first_hartid + 1;
	}
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_plmt_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_আকার, u32 *hart_গুনতি)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা rc, i, গুনতি;
	uint64_t reg_addr, reg_আকার;
	u32 phandle, hwirq, hartid, hcount;

	যদি (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_গুনতি || !plmt_আকার)
		ফেরত SBI_EINVAL;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0)
		ফেরত SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_আকার = reg_আকার;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < আকার(fdt32_t))
		ফেরত 0;
	গুনতি = গুনতি / আকার(fdt32_t);

	hcount = 0;
	জন্য (i = 0; i < (গুনতি / 2); i++) {
		পূর্ণসংখ্যা cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			চালু;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			চালু;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		যদি (rc)
			চালু;

		যদি (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			চালু;

		যদি (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_গুনতি = hcount;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_plicsw_গ্রন্থি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, unsigned long *plicsw_base,
			  unsigned long *আকার_2, u32 *hart_গুনতি)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা rc, i, গুনতি;
	uint64_t reg_addr, reg_আকার;
	u32 phandle, hwirq, hartid, hcount;

	যদি (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_গুনতি || !আকার_2)
		ফেরত SBI_EINVAL;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0,
				    &reg_addr, &reg_আকার);
	যদি (rc < 0)
		ফেরত SBI_ENODEV;
	*plicsw_base = reg_addr;
	*আকার_2 = reg_আকার;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < আকার(fdt32_t))
		ফেরত 0;
	গুনতি = গুনতি / আকার(fdt32_t);

	hcount = 0;
	জন্য (i = 0; i < (গুনতি / 2); i++) {
		পূর্ণসংখ্যা cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			চালু;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			চালু;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		যদি (rc)
			চালু;

		যদি (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			চালু;

		যদি (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_গুনতি = hcount;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_parse_compat_addr(ধ্রুবক শূন্য *fdt, uint64_t *addr,
			  ধ্রুবক অক্ষর *compatible)
{
	পূর্ণসংখ্যা nodeoffset, rc;

	nodeoffset = fdt_গ্রন্থি_offset_by_compatible(fdt, -1, compatible);
	যদি (nodeoffset < 0)
		ফেরত nodeoffset;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoffset, 0, addr, ফাঁকা);
	যদি (rc < 0 || !addr)
		ফেরত SBI_ENODEV;

	ফেরত 0;
}
