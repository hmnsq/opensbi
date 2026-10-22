// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_hartmask.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/aplic.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/imsic.h>
#অন্তর্ভাৱ <sbi_utils/irqchip/plic.h>

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

পূর্ণাঙ্ক fdt_parse_phandle_with_args(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				ধ্রুৱ ৱর্ণ *prop, ধ্রুৱ ৱর্ণ *cells_prop,
				পূর্ণাঙ্ক index, সংরচনা fdt_phandle_args *out_args)
{
	u32 i, pcells;
	পূর্ণাঙ্ক len, pnodeoff;
	ধ্রুৱ fdt32_t *list, *list_end, *val;

	যদি (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		ঘূৰাই SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	যদি (!list)
		ঘূৰাই SBI_ENOENT;
	list_end = list + (len / পরিমাণ(*list));

	যেতিয়ালৈ (list < list_end) {
		pnodeoff = fdt_গ্ৰন্থি_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		যদি (pnodeoff < 0)
			ঘূৰাই pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		যদি (!val)
			ঘূৰাই SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		যদি (FDT_MAX_PHANDLE_ARGS < pcells)
			ঘূৰাই SBI_EINVAL;
		যদি (list + pcells > list_end)
			ঘূৰাই SBI_ENOENT;

		যদি (index > 0) {
			list += pcells;
			index--;
		} অন্যথা {
			out_args->গ্ৰন্থি_offset = pnodeoff;
			out_args->args_গুনতি = pcells;
			বাবে (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			ঘূৰাই 0;
		}
	}

	ঘূৰাই SBI_ENOENT;
}

স্থির পূর্ণাঙ্ক fdt_translate_address(ধ্রুৱ শূন্য *fdt, uint64_t reg, পূর্ণাঙ্ক parent,
				 uint64_t *addr)
{
	পূর্ণাঙ্ক i, rlen;
	পূর্ণাঙ্ক cell_parent_addr, cell_child_addr, cell_size;
	ধ্রুৱ fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	যদি (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		যদি (cell_child_addr < 1)
			ঘূৰাই SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		যদি (cell_parent_addr < 1)
			ঘূৰাই SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		যদি (cell_size < 0)
			ঘূৰাই SBI_ENODEV;

		বাবে (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		বাবে (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		বাবে (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		যদি (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			ঘূৰাই SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} অন্যথা {
		/* No translation required */
		*addr = reg;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_get_গ্ৰন্থি_addr_size(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক গ্ৰন্থি, পূর্ণাঙ্ক index,
			   uint64_t *addr, uint64_t *size)
{
	পূর্ণাঙ্ক parent, len, i, rc;
	পূর্ণাঙ্ক cell_addr, cell_size;
	ধ্রুৱ fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	যদি (!fdt || গ্ৰন্থি < 0 || index < 0)
		ঘূৰাই SBI_EINVAL;

	parent = fdt_parent_offset(fdt, গ্ৰন্থি);
	যদি (parent < 0)
		ঘূৰাই parent;
	cell_addr = fdt_address_cells(fdt, parent);
	যদি (cell_addr < 1)
		ঘূৰাই SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	যদি (cell_size < 0)
		ঘূৰাই SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, গ্ৰন্থি, "reg", &len);
	যদি (!prop_addr)
		ঘূৰাই SBI_ENODEV;

	যদি ((len / পরিমাণ(u32)) <= (index * (cell_addr + cell_size)))
		ঘূৰাই SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	যদি (addr) {
		বাবে (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		কৰা {
			যদি (parent < 0)
				ভঙ্গ;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			যদি (rc)
				ভঙ্গ;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} যেতিয়ালৈ (1);
	}
	temp = 0;

	যদি (size) {
		বাবে (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_get_গ্ৰন্থি_addr_size_by_name(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক গ্ৰন্থি, ধ্রুৱ ৱর্ণ *name,
				   uint64_t *addr, uint64_t *size)
{
	পূর্ণাঙ্ক i, j, গুনতি;
	ধ্রুৱ ৱর্ণ *val;
	ধ্রুৱ ৱর্ণ *regname;

	যদি (!fdt || গ্ৰন্থি < 0 || !name)
		ঘূৰাই SBI_EINVAL;

	val = fdt_getprop(fdt, গ্ৰন্থি, "reg-names", &গুনতি);
	যদি (!val)
		ঘূৰাই SBI_ENODEV;

	বাবে (i = 0, j = 0; i < গুনতি; i++, j++) {
		regname = val + i;

		যদি (strcmp(name, regname) == 0)
			ঘূৰাই fdt_get_গ্ৰন্থি_addr_size(fdt, গ্ৰন্থি, j, addr, size);

		i += strlen(regname);
	}

	ঘূৰাই SBI_ENODEV;
}

bool fdt_গ্ৰন্থি_is_enabled(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff)
{
	পূর্ণাঙ্ক len;
	ধ্রুৱ শূন্য *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	যদি (!prop)
		ঘূৰাই true;

	যদি (!strncmp(prop, "okay", strlen("okay")))
		ঘূৰাই true;

	যদি (!strncmp(prop, "ok", strlen("ok")))
		ঘূৰাই true;

	ঘূৰাই false;
}

পূর্ণাঙ্ক fdt_parse_hart_id(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক cpu_offset, u32 *hartid)
{
	পূর্ণাঙ্ক len;
	ধ্রুৱ শূন্য *prop;
	ধ্রুৱ fdt32_t *val;

	যদি (!fdt || cpu_offset < 0)
		ঘূৰাই SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	যদি (!prop || !len)
		ঘূৰাই SBI_EINVAL;
	যদি (strncmp (prop, "cpu", strlen ("cpu")))
		ঘূৰাই SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	যদি (!val || len < পরিমাণ(fdt32_t))
		ঘূৰাই SBI_EINVAL;

	যদি (len > পরিমাণ(fdt32_t))
		val++;

	যদি (hartid)
		*hartid = fdt32_to_cpu(*val);

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_cbom_block_size(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক cpu_offset, unsigned long *cbom_block_size)
{
	পূর্ণাঙ্ক len;
	ধ্রুৱ শূন্য *prop;
	ধ্রুৱ fdt32_t *val;

	যদি (!fdt || cpu_offset < 0)
		ঘূৰাই SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	যদি (!prop || !len)
		ঘূৰাই SBI_EINVAL;
	যদি (strncmp (prop, "cpu", strlen ("cpu")))
		ঘূৰাই SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	যদি (!val || len < পরিমাণ(fdt32_t))
		ঘূৰাই SBI_EINVAL;

	যদি (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_max_enabled_hart_id(ধ্রুৱ শূন্য *fdt, u32 *max_hartid)
{
	u32 hartid;
	পূর্ণাঙ্ক err, cpu_offset, cpus_offset;

	যদি (!fdt)
		ঘূৰাই SBI_EINVAL;
	যদি (!max_hartid)
		ঘূৰাই 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ঘূৰাই cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			অনুৱর্ত;

		যদি (!fdt_গ্ৰন্থি_is_enabled(fdt, cpu_offset))
			অনুৱর্ত;

		যদি (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_timebase_frequency(ধ্রুৱ শূন্য *fdt, unsigned long *freq)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক len, cpus_offset;

	যদি (!fdt || !freq)
		ঘূৰাই SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ঘূৰাই cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	যদি (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	অন্যথা
		ঘূৰাই SBI_ENOENT;

	ঘূৰাই 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

স্থির পূর্ণাঙ্ক fdt_parse_isa_one_hart(ধ্রুৱ ৱর্ণ *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	ৱর্ণ mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	যদি (isa[i] == 'r' || isa[i] == 'R')
		i++;
	অন্যথা
		ঘূৰাই SBI_EINVAL;

	যদি (isa[i] == 'v' || isa[i] == 'V')
		i++;
	অন্যথা
		ঘূৰাই SBI_EINVAL;

	যদি (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	অন্যথা যদি (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	অন্যথা
		ঘূৰাই SBI_EINVAL;

	/* Skip base ISA extensions */
	বাবে (; i < isa_len; i++) {
		যদি (isa[i] == '_')
			ভঙ্গ;
	}

	যেতিয়ালৈ (i < isa_len) {
		যদি (isa[i] != '_') {
			i++;
			অনুৱর্ত;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		যেতিয়ালৈ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (পরিমাণ(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		যদি (!j)
			অনুৱর্ত;

#define set_multi_letter_ext(name, bit)				\
			যদি (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				অনুৱর্ত;			\
			}

		বাবে (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	ঘূৰাই 0;
}

স্থির শূন্য fdt_parse_isa_extensions_one_hart(ধ্রুৱ ৱর্ণ *isa,
					      unsigned long *extensions,
					      পূর্ণাঙ্ক len)
{
	size_t i;

	বাবে (i = 0; i < SBI_HART_EXT_MAX; i++) {
		যদি (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

পূর্ণাঙ্ক fdt_parse_isa_extensions_all_harts(ধ্রুৱ শূন্য *fdt)
{
	u32 hartid;
	ধ্রুৱ fdt32_t *val;
	সংরচনা sbi_scratch *scratch;
	সংরচনা sbi_hart_features *hfeatures;
	পূর্ণাঙ্ক err, cpu_offset, cpus_offset, len;

	যদি (!fdt)
		ঘূৰাই SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	যদি (cpus_offset < 0)
		ঘূৰাই cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (err)
			অনুৱর্ত;

		যদি (!fdt_গ্ৰন্থি_is_enabled(fdt, cpu_offset))
			অনুৱর্ত;

		scratch = sbi_hartid_to_scratch(hartid);
		যদি (!scratch)
			ঘূৰাই SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		যদি (!hfeatures)
			ঘূৰাই SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		যদি (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ধ্রুৱ ৱর্ণ *)val,
							  hfeatures->extensions, len);
			অনুৱর্ত;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		যদি (!val || len <= 0)
			ঘূৰাই SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ধ্রুৱ ৱর্ণ *)val, hfeatures->extensions);
		যদি (err)
			ঘূৰাই err;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক fdt_parse_uart_গ্ৰন্থি_common(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				      সংরচনা platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	পূর্ণাঙ্ক len, rc;
	ধ্রুৱ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	যদি (nodeoffset < 0 || !uart || !fdt)
		ঘূৰাই SBI_ENODEV;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		ঘূৰাই SBI_ENODEV;
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

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_gaisler_uart_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				সংরচনা platform_uart_data *uart)
{
	ঘূৰাই fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

পূর্ণাঙ্ক fdt_parse_renesas_scif_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				সংরচনা platform_uart_data *uart)
{
	ঘূৰাই fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

পূর্ণাঙ্ক fdt_parse_shakti_uart_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			       সংরচনা platform_uart_data *uart)
{
	ঘূৰাই fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

পূর্ণাঙ্ক fdt_parse_sifive_uart_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			       সংরচনা platform_uart_data *uart)
{
	ঘূৰাই fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

পূর্ণাঙ্ক fdt_parse_uart_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			সংরচনা platform_uart_data *uart)
{
	পূর্ণাঙ্ক len, rc;
	ধ্রুৱ fdt32_t *val;

	rc = fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	যদি (rc)
		ঘূৰাই rc;

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

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_uart8250(ধ্রুৱ শূন্য *fdt, সংরচনা platform_uart_data *uart,
		   ধ্রুৱ ৱর্ণ *compatible)
{
	পূর্ণাঙ্ক nodeoffset;

	যদি (!compatible || !uart || !fdt)
		ঘূৰাই SBI_ENODEV;

	nodeoffset = fdt_গ্ৰন্থি_offset_by_compatible(fdt, -1, compatible);
	যদি (nodeoffset < 0)
		ঘূৰাই nodeoffset;

	ঘূৰাই fdt_parse_uart_গ্ৰন্থি(fdt, nodeoffset, uart);
}

পূর্ণাঙ্ক fdt_parse_xlnx_uartlite_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			       সংরচনা platform_uart_data *uart)
{
	ঘূৰাই fdt_parse_uart_গ্ৰন্থি_common(fdt, nodeoffset, uart, 0, 0);
}

স্থির পূর্ণাঙ্ক fdt_aplic_find_imsic_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				     সংরচনা imsic_data *imsic, bool mmode)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	যদি (val && len >= পরিমাণ(fdt32_t)) {
		noff = fdt_গ্ৰন্থি_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		যদি (noff < 0)
			ঘূৰাই noff;

		rc = fdt_parse_imsic_গ্ৰন্থি(fdt, noff, imsic);
		যদি (rc)
			ঘূৰাই rc;

		rc = imsic_data_check(imsic);
		যদি (rc)
			ঘূৰাই rc;

		যদি (imsic->targets_mmode == mmode) {
			ঘূৰাই 0;
		}
	} অন্যথা {
		ঘূৰাই SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	যদি (!val || len < পরিমাণ(fdt32_t))
		ঘূৰাই SBI_ENODEV;

	len /= পরিমাণ(fdt32_t);

	বাবে (i = 0; i < len; i++) {
		noff = fdt_গ্ৰন্থি_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		যদি (noff < 0)
			ঘূৰাই noff;

		rc = fdt_aplic_find_imsic_গ্ৰন্থি(fdt, noff, imsic, mmode);
		যদি (!rc)
			ভঙ্গ;
	}

	ঘূৰাই rc;
}

পূর্ণাঙ্ক fdt_parse_aplic_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, সংরচনা aplic_data *aplic)
{
	bool child_found;
	ধ্রুৱ fdt32_t *val;
	ধ্রুৱ fdt32_t *del;
	সংরচনা imsic_data imsic = { 0 };
	পূর্ণাঙ্ক i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	সংরচনা aplic_delegate_data *deleg;

	যদি (nodeoff < 0 || !aplic || !fdt)
		ঘূৰাই SBI_ENODEV;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		ঘূৰাই SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	যদি (len > 0)
		aplic->অঙ্ক_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	যদি (val && len > পরিমাণ(fdt32_t)) {
		len = len / পরিমাণ(fdt32_t);
		বাবে (i = 0; i < len; i += 2) {
			যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ভঙ্গ;
			}
		}
		aplic->অঙ্ক_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_গ্ৰন্থি(fdt, nodeoff, &imsic, true);
	যদি (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		যদি (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ঘূৰাই SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_গ্ৰন্থি(fdt, nodeoff, &imsic, false);
	যদি (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		যদি (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			ঘূৰাই SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	বাবে (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	যদি (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	যদি (!del || len < (3 * পরিমাণ(fdt32_t)))
		গচ্ছ skip_delegate_parse;
	d = 0;
	dcnt = len / পরিমাণ(fdt32_t);
	বাবে (i = 0; i < dcnt; i += 3) {
		যদি (d >= APLIC_MAX_DELEGATE)
			ভঙ্গ;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		যদি (!val || len < পরিমাণ(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			অনুৱর্ত;
		}
		len = len / পরিমাণ(fdt32_t);
		বাবে (j = 0; j < len; j++) {
			যদি (del[i] != val[j])
				অনুৱর্ত;
			deleg->child_index = j;
			child_found = true;
			ভঙ্গ;
		}

		যদি (child_found) {
			d++;
		} অন্যথা {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	ঘূৰাই 0;
}

bool fdt_check_imsic_mlevel(ধ্রুৱ শূন্য *fdt)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক i, len, noff = 0;

	যদি (!fdt)
		ঘূৰাই false;

	যেতিয়ালৈ ((noff = fdt_গ্ৰন্থি_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		যদি (val && len > পরিমাণ(fdt32_t)) {
			len = len / পরিমাণ(fdt32_t);
			বাবে (i = 0; i < len; i += 2) {
				যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					ঘূৰাই true;
			}
		}
	}

	ঘূৰাই false;
}

পূর্ণাঙ্ক fdt_parse_imsic_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, সংরচনা imsic_data *imsic)
{
	ধ্রুৱ fdt32_t *val;
	সংরচনা imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	পূর্ণাঙ্ক i, rc, len, nr_parent_irqs;

	যদি (nodeoff < 0 || !imsic || !fdt)
		ঘূৰাই SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	যদি (val && len > পরিমাণ(fdt32_t)) {
		len = len / পরিমাণ(fdt32_t);
		nr_parent_irqs = len / 2;
		বাবে (i = 0; i < len; i += 2) {
			যদি (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ভঙ্গ;
			}
		}
	} অন্যথা
		ঘূৰাই SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	যদি (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	অন্যথা
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	যদি (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} অন্যথা {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		যদি ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	যদি (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	অন্যথা
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	যদি (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	অন্যথা
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	যদি (val && len > 0)
		imsic->অঙ্ক_ids = fdt32_to_cpu(*val);
	অন্যথা
		ঘূৰাই SBI_EINVAL;

	বাবে (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	বাবে (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		যদি (rc < 0 || !reg_size)
			ভঙ্গ;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	যদি (!imsic->regs[0].size)
		ঘূৰাই SBI_EINVAL;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_plic_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, সংরচনা plic_data *plic)
{
	পূর্ণাঙ্ক len, rc;
	ধ্রুৱ fdt32_t *val;
	uint64_t reg_addr, reg_size;

	যদি (nodeoffset < 0 || !plic || !fdt)
		ঘূৰাই SBI_ENODEV;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		ঘূৰাই SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	যদি (len > 0)
		plic->অঙ্ক_src = fdt32_to_cpu(*val);

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_plic(ধ্রুৱ শূন্য *fdt, সংরচনা plic_data *plic, ধ্রুৱ ৱর্ণ *compat)
{
	পূর্ণাঙ্ক nodeoffset;

	যদি (!compat || !plic || !fdt)
		ঘূৰাই SBI_ENODEV;

	nodeoffset = fdt_গ্ৰন্থি_offset_by_compatible(fdt, -1, compat);
	যদি (nodeoffset < 0)
		ঘূৰাই nodeoffset;

	ঘূৰাই fdt_parse_plic_গ্ৰন্থি(fdt, nodeoffset, plic);
}

স্থির পূর্ণাঙ্ক fdt_get_aclint_addr_size_by_name(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	পূর্ণাঙ্ক rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_গ্ৰন্থি_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_গ্ৰন্থি_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		ঘূৰাই SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক fdt_get_aclint_addr_size(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	পূর্ণাঙ্ক rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		ঘূৰাই SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	যদি (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	যদি (out_addr2)
		*out_addr2 = reg_addr;
	যদি (out_size2)
		*out_size2 = reg_size;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_aclint_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_গুনতি)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক i, rc, গুনতি, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	যদি (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_গুনতি)
		ঘূৰাই SBI_EINVAL;

	যদি (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", রিক্ত))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	অন্যথা
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	যদি (rc)
		ঘূৰাই rc;

	*out_first_hartid = 0;
	*out_hart_গুনতি = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		ঘূৰাই 0;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	বাবে (i = 0; i < (গুনতি / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুৱর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুৱর্ত;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		যদি (rc)
			অনুৱর্ত;

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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_plmt_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_গুনতি)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক rc, i, গুনতি;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	যদি (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_গুনতি || !plmt_size)
		ঘূৰাই SBI_EINVAL;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	যদি (rc < 0)
		ঘূৰাই SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		ঘূৰাই 0;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	hcount = 0;
	বাবে (i = 0; i < (গুনতি / 2); i++) {
		পূর্ণাঙ্ক cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুৱর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুৱর্ত;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		যদি (rc)
			অনুৱর্ত;

		যদি (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			অনুৱর্ত;

		যদি (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_গুনতি = hcount;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_plicsw_গ্ৰন্থি(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_গুনতি)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক rc, i, গুনতি;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	যদি (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_গুনতি || !size)
		ঘূৰাই SBI_EINVAL;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	যদি (rc < 0)
		ঘূৰাই SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &গুনতি);
	যদি (!val || গুনতি < পরিমাণ(fdt32_t))
		ঘূৰাই 0;
	গুনতি = গুনতি / পরিমাণ(fdt32_t);

	hcount = 0;
	বাবে (i = 0; i < (গুনতি / 2); i++) {
		পূর্ণাঙ্ক cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);
		যদি (cpu_intc_offset < 0)
			অনুৱর্ত;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		যদি (cpu_offset < 0)
			অনুৱর্ত;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		যদি (rc)
			অনুৱর্ত;

		যদি (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			অনুৱর্ত;

		যদি (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_গুনতি = hcount;

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_parse_compat_addr(ধ্রুৱ শূন্য *fdt, uint64_t *addr,
			  ধ্রুৱ ৱর্ণ *compatible)
{
	পূর্ণাঙ্ক nodeoffset, rc;

	nodeoffset = fdt_গ্ৰন্থি_offset_by_compatible(fdt, -1, compatible);
	যদি (nodeoffset < 0)
		ঘূৰাই nodeoffset;

	rc = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoffset, 0, addr, রিক্ত);
	যদি (rc < 0 || !addr)
		ঘূৰাই SBI_ENODEV;

	ঘূৰাই 0;
}
