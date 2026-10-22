// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_hartmask.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/aplic.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/imsic.h>
#അന്തര്ഭാവ <sbi_utils/irqchip/plic.h>

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

പൂർണ്ണസംഖ്യ fdt_parse_phandle_with_args(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				സ്ഥിരം അക്ഷരം *prop, സ്ഥിരം അക്ഷരം *cells_prop,
				പൂർണ്ണസംഖ്യ index, ഘടന fdt_phandle_args *out_args)
{
	u32 i, pcells;
	പൂർണ്ണസംഖ്യ len, pnodeoff;
	സ്ഥിരം fdt32_t *list, *list_end, *val;

	എങ്കിൽ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		തിരികെ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	എങ്കിൽ (!list)
		തിരികെ SBI_ENOENT;
	list_end = list + (len / വലുപ്പം(*list));

	ആകുമ്പോൾ (list < list_end) {
		pnodeoff = fdt_കണ്ണി_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		എങ്കിൽ (pnodeoff < 0)
			തിരികെ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		എങ്കിൽ (!val)
			തിരികെ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		എങ്കിൽ (FDT_MAX_PHANDLE_ARGS < pcells)
			തിരികെ SBI_EINVAL;
		എങ്കിൽ (list + pcells > list_end)
			തിരികെ SBI_ENOENT;

		എങ്കിൽ (index > 0) {
			list += pcells;
			index--;
		} അല്ലെങ്കിൽ {
			out_args->കണ്ണി_offset = pnodeoff;
			out_args->args_എണ്ണം = pcells;
			വേണ്ടി (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			തിരികെ 0;
		}
	}

	തിരികെ SBI_ENOENT;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_translate_address(സ്ഥിരം ശൂന്യം *fdt, uint64_t reg, പൂർണ്ണസംഖ്യ parent,
				 uint64_t *addr)
{
	പൂർണ്ണസംഖ്യ i, rlen;
	പൂർണ്ണസംഖ്യ cell_parent_addr, cell_child_addr, cell_size;
	സ്ഥിരം fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	എങ്കിൽ (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		എങ്കിൽ (cell_child_addr < 1)
			തിരികെ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		എങ്കിൽ (cell_parent_addr < 1)
			തിരികെ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		എങ്കിൽ (cell_size < 0)
			തിരികെ SBI_ENODEV;

		വേണ്ടി (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		വേണ്ടി (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		വേണ്ടി (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		എങ്കിൽ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			തിരികെ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} അല്ലെങ്കിൽ {
		/* No translation required */
		*addr = reg;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_get_കണ്ണി_addr_size(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ കണ്ണി, പൂർണ്ണസംഖ്യ index,
			   uint64_t *addr, uint64_t *size)
{
	പൂർണ്ണസംഖ്യ parent, len, i, rc;
	പൂർണ്ണസംഖ്യ cell_addr, cell_size;
	സ്ഥിരം fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	എങ്കിൽ (!fdt || കണ്ണി < 0 || index < 0)
		തിരികെ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, കണ്ണി);
	എങ്കിൽ (parent < 0)
		തിരികെ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	എങ്കിൽ (cell_addr < 1)
		തിരികെ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	എങ്കിൽ (cell_size < 0)
		തിരികെ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, കണ്ണി, "reg", &len);
	എങ്കിൽ (!prop_addr)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ ((len / വലുപ്പം(u32)) <= (index * (cell_addr + cell_size)))
		തിരികെ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	എങ്കിൽ (addr) {
		വേണ്ടി (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		ചെയ്യുക {
			എങ്കിൽ (parent < 0)
				നിർത്തുക;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			എങ്കിൽ (rc)
				നിർത്തുക;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} ആകുമ്പോൾ (1);
	}
	temp = 0;

	എങ്കിൽ (size) {
		വേണ്ടി (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_get_കണ്ണി_addr_size_by_name(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ കണ്ണി, സ്ഥിരം അക്ഷരം *name,
				   uint64_t *addr, uint64_t *size)
{
	പൂർണ്ണസംഖ്യ i, j, എണ്ണം;
	സ്ഥിരം അക്ഷരം *val;
	സ്ഥിരം അക്ഷരം *regname;

	എങ്കിൽ (!fdt || കണ്ണി < 0 || !name)
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, കണ്ണി, "reg-names", &എണ്ണം);
	എങ്കിൽ (!val)
		തിരികെ SBI_ENODEV;

	വേണ്ടി (i = 0, j = 0; i < എണ്ണം; i++, j++) {
		regname = val + i;

		എങ്കിൽ (strcmp(name, regname) == 0)
			തിരികെ fdt_get_കണ്ണി_addr_size(fdt, കണ്ണി, j, addr, size);

		i += strlen(regname);
	}

	തിരികെ SBI_ENODEV;
}

bool fdt_കണ്ണി_is_enabled(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff)
{
	പൂർണ്ണസംഖ്യ len;
	സ്ഥിരം ശൂന്യം *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	എങ്കിൽ (!prop)
		തിരികെ true;

	എങ്കിൽ (!strncmp(prop, "okay", strlen("okay")))
		തിരികെ true;

	എങ്കിൽ (!strncmp(prop, "ok", strlen("ok")))
		തിരികെ true;

	തിരികെ false;
}

പൂർണ്ണസംഖ്യ fdt_parse_hart_id(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ cpu_offset, u32 *hartid)
{
	പൂർണ്ണസംഖ്യ len;
	സ്ഥിരം ശൂന്യം *prop;
	സ്ഥിരം fdt32_t *val;

	എങ്കിൽ (!fdt || cpu_offset < 0)
		തിരികെ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	എങ്കിൽ (!prop || !len)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (strncmp (prop, "cpu", strlen ("cpu")))
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	എങ്കിൽ (!val || len < വലുപ്പം(fdt32_t))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (len > വലുപ്പം(fdt32_t))
		val++;

	എങ്കിൽ (hartid)
		*hartid = fdt32_to_cpu(*val);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_cbom_block_size(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ cpu_offset, unsigned long *cbom_block_size)
{
	പൂർണ്ണസംഖ്യ len;
	സ്ഥിരം ശൂന്യം *prop;
	സ്ഥിരം fdt32_t *val;

	എങ്കിൽ (!fdt || cpu_offset < 0)
		തിരികെ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	എങ്കിൽ (!prop || !len)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (strncmp (prop, "cpu", strlen ("cpu")))
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	എങ്കിൽ (!val || len < വലുപ്പം(fdt32_t))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_max_enabled_hart_id(സ്ഥിരം ശൂന്യം *fdt, u32 *max_hartid)
{
	u32 hartid;
	പൂർണ്ണസംഖ്യ err, cpu_offset, cpus_offset;

	എങ്കിൽ (!fdt)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!max_hartid)
		തിരികെ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (err)
			തുടരുക;

		എങ്കിൽ (!fdt_കണ്ണി_is_enabled(fdt, cpu_offset))
			തുടരുക;

		എങ്കിൽ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_timebase_frequency(സ്ഥിരം ശൂന്യം *fdt, unsigned long *freq)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ len, cpus_offset;

	എങ്കിൽ (!fdt || !freq)
		തിരികെ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	എങ്കിൽ (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		തിരികെ SBI_ENOENT;

	തിരികെ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_parse_isa_one_hart(സ്ഥിരം അക്ഷരം *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	അക്ഷരം mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	എങ്കിൽ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	അല്ലെങ്കിൽ എങ്കിൽ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	/* Skip base ISA extensions */
	വേണ്ടി (; i < isa_len; i++) {
		എങ്കിൽ (isa[i] == '_')
			നിർത്തുക;
	}

	ആകുമ്പോൾ (i < isa_len) {
		എങ്കിൽ (isa[i] != '_') {
			i++;
			തുടരുക;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		ആകുമ്പോൾ ((i < isa_len) && (isa[i] != '_') &&
		       (j < (വലുപ്പം(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		എങ്കിൽ (!j)
			തുടരുക;

#define set_multi_letter_ext(name, bit)				\
			എങ്കിൽ (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				തുടരുക;			\
			}

		വേണ്ടി (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം fdt_parse_isa_extensions_one_hart(സ്ഥിരം അക്ഷരം *isa,
					      unsigned long *extensions,
					      പൂർണ്ണസംഖ്യ len)
{
	size_t i;

	വേണ്ടി (i = 0; i < SBI_HART_EXT_MAX; i++) {
		എങ്കിൽ (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

പൂർണ്ണസംഖ്യ fdt_parse_isa_extensions_all_harts(സ്ഥിരം ശൂന്യം *fdt)
{
	u32 hartid;
	സ്ഥിരം fdt32_t *val;
	ഘടന sbi_scratch *scratch;
	ഘടന sbi_hart_features *hfeatures;
	പൂർണ്ണസംഖ്യ err, cpu_offset, cpus_offset, len;

	എങ്കിൽ (!fdt)
		തിരികെ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	എങ്കിൽ (cpus_offset < 0)
		തിരികെ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (err)
			തുടരുക;

		എങ്കിൽ (!fdt_കണ്ണി_is_enabled(fdt, cpu_offset))
			തുടരുക;

		scratch = sbi_hartid_to_scratch(hartid);
		എങ്കിൽ (!scratch)
			തിരികെ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		എങ്കിൽ (!hfeatures)
			തിരികെ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		എങ്കിൽ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((സ്ഥിരം അക്ഷരം *)val,
							  hfeatures->extensions, len);
			തുടരുക;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		എങ്കിൽ (!val || len <= 0)
			തിരികെ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((സ്ഥിരം അക്ഷരം *)val, hfeatures->extensions);
		എങ്കിൽ (err)
			തിരികെ err;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_parse_uart_കണ്ണി_common(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				      ഘടന platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	പൂർണ്ണസംഖ്യ len, rc;
	സ്ഥിരം fdt32_t *val;
	uint64_t reg_addr, reg_size;

	എങ്കിൽ (nodeoffset < 0 || !uart || !fdt)
		തിരികെ SBI_ENODEV;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		തിരികെ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	എങ്കിൽ (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	എങ്കിൽ (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		uart->baud = default_baud;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_gaisler_uart_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				ഘടന platform_uart_data *uart)
{
	തിരികെ fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

പൂർണ്ണസംഖ്യ fdt_parse_renesas_scif_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				ഘടന platform_uart_data *uart)
{
	തിരികെ fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

പൂർണ്ണസംഖ്യ fdt_parse_shakti_uart_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			       ഘടന platform_uart_data *uart)
{
	തിരികെ fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

പൂർണ്ണസംഖ്യ fdt_parse_sifive_uart_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			       ഘടന platform_uart_data *uart)
{
	തിരികെ fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

പൂർണ്ണസംഖ്യ fdt_parse_uart_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			ഘടന platform_uart_data *uart)
{
	പൂർണ്ണസംഖ്യ len, rc;
	സ്ഥിരം fdt32_t *val;

	rc = fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	എങ്കിൽ (rc)
		തിരികെ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	എങ്കിൽ (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	എങ്കിൽ (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	എങ്കിൽ (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_uart8250(സ്ഥിരം ശൂന്യം *fdt, ഘടന platform_uart_data *uart,
		   സ്ഥിരം അക്ഷരം *compatible)
{
	പൂർണ്ണസംഖ്യ nodeoffset;

	എങ്കിൽ (!compatible || !uart || !fdt)
		തിരികെ SBI_ENODEV;

	nodeoffset = fdt_കണ്ണി_offset_by_compatible(fdt, -1, compatible);
	എങ്കിൽ (nodeoffset < 0)
		തിരികെ nodeoffset;

	തിരികെ fdt_parse_uart_കണ്ണി(fdt, nodeoffset, uart);
}

പൂർണ്ണസംഖ്യ fdt_parse_xlnx_uartlite_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			       ഘടന platform_uart_data *uart)
{
	തിരികെ fdt_parse_uart_കണ്ണി_common(fdt, nodeoffset, uart, 0, 0);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_aplic_find_imsic_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				     ഘടന imsic_data *imsic, bool mmode)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	എങ്കിൽ (val && len >= വലുപ്പം(fdt32_t)) {
		noff = fdt_കണ്ണി_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		എങ്കിൽ (noff < 0)
			തിരികെ noff;

		rc = fdt_parse_imsic_കണ്ണി(fdt, noff, imsic);
		എങ്കിൽ (rc)
			തിരികെ rc;

		rc = imsic_data_check(imsic);
		എങ്കിൽ (rc)
			തിരികെ rc;

		എങ്കിൽ (imsic->targets_mmode == mmode) {
			തിരികെ 0;
		}
	} അല്ലെങ്കിൽ {
		തിരികെ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	എങ്കിൽ (!val || len < വലുപ്പം(fdt32_t))
		തിരികെ SBI_ENODEV;

	len /= വലുപ്പം(fdt32_t);

	വേണ്ടി (i = 0; i < len; i++) {
		noff = fdt_കണ്ണി_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		എങ്കിൽ (noff < 0)
			തിരികെ noff;

		rc = fdt_aplic_find_imsic_കണ്ണി(fdt, noff, imsic, mmode);
		എങ്കിൽ (!rc)
			നിർത്തുക;
	}

	തിരികെ rc;
}

പൂർണ്ണസംഖ്യ fdt_parse_aplic_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, ഘടന aplic_data *aplic)
{
	bool child_found;
	സ്ഥിരം fdt32_t *val;
	സ്ഥിരം fdt32_t *del;
	ഘടന imsic_data imsic = { 0 };
	പൂർണ്ണസംഖ്യ i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	ഘടന aplic_delegate_data *deleg;

	എങ്കിൽ (nodeoff < 0 || !aplic || !fdt)
		തിരികെ SBI_ENODEV;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		തിരികെ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	എങ്കിൽ (len > 0)
		aplic->അക്കം_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	എങ്കിൽ (val && len > വലുപ്പം(fdt32_t)) {
		len = len / വലുപ്പം(fdt32_t);
		വേണ്ടി (i = 0; i < len; i += 2) {
			എങ്കിൽ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				നിർത്തുക;
			}
		}
		aplic->അക്കം_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_കണ്ണി(fdt, nodeoff, &imsic, true);
	എങ്കിൽ (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		എങ്കിൽ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			തിരികെ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_കണ്ണി(fdt, nodeoff, &imsic, false);
	എങ്കിൽ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		എങ്കിൽ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			തിരികെ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	വേണ്ടി (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	എങ്കിൽ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	എങ്കിൽ (!del || len < (3 * വലുപ്പം(fdt32_t)))
		പോകുക skip_delegate_parse;
	d = 0;
	dcnt = len / വലുപ്പം(fdt32_t);
	വേണ്ടി (i = 0; i < dcnt; i += 3) {
		എങ്കിൽ (d >= APLIC_MAX_DELEGATE)
			നിർത്തുക;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		എങ്കിൽ (!val || len < വലുപ്പം(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			തുടരുക;
		}
		len = len / വലുപ്പം(fdt32_t);
		വേണ്ടി (j = 0; j < len; j++) {
			എങ്കിൽ (del[i] != val[j])
				തുടരുക;
			deleg->child_index = j;
			child_found = true;
			നിർത്തുക;
		}

		എങ്കിൽ (child_found) {
			d++;
		} അല്ലെങ്കിൽ {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	തിരികെ 0;
}

bool fdt_check_imsic_mlevel(സ്ഥിരം ശൂന്യം *fdt)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ i, len, noff = 0;

	എങ്കിൽ (!fdt)
		തിരികെ false;

	ആകുമ്പോൾ ((noff = fdt_കണ്ണി_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		എങ്കിൽ (val && len > വലുപ്പം(fdt32_t)) {
			len = len / വലുപ്പം(fdt32_t);
			വേണ്ടി (i = 0; i < len; i += 2) {
				എങ്കിൽ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					തിരികെ true;
			}
		}
	}

	തിരികെ false;
}

പൂർണ്ണസംഖ്യ fdt_parse_imsic_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, ഘടന imsic_data *imsic)
{
	സ്ഥിരം fdt32_t *val;
	ഘടന imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	പൂർണ്ണസംഖ്യ i, rc, len, nr_parent_irqs;

	എങ്കിൽ (nodeoff < 0 || !imsic || !fdt)
		തിരികെ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	എങ്കിൽ (val && len > വലുപ്പം(fdt32_t)) {
		len = len / വലുപ്പം(fdt32_t);
		nr_parent_irqs = len / 2;
		വേണ്ടി (i = 0; i < len; i += 2) {
			എങ്കിൽ (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				നിർത്തുക;
			}
		}
	} അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	എങ്കിൽ (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	എങ്കിൽ (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} അല്ലെങ്കിൽ {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		എങ്കിൽ ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	എങ്കിൽ (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	എങ്കിൽ (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	എങ്കിൽ (val && len > 0)
		imsic->അക്കം_ids = fdt32_to_cpu(*val);
	അല്ലെങ്കിൽ
		തിരികെ SBI_EINVAL;

	വേണ്ടി (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	വേണ്ടി (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		എങ്കിൽ (rc < 0 || !reg_size)
			നിർത്തുക;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	എങ്കിൽ (!imsic->regs[0].size)
		തിരികെ SBI_EINVAL;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_plic_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, ഘടന plic_data *plic)
{
	പൂർണ്ണസംഖ്യ len, rc;
	സ്ഥിരം fdt32_t *val;
	uint64_t reg_addr, reg_size;

	എങ്കിൽ (nodeoffset < 0 || !plic || !fdt)
		തിരികെ SBI_ENODEV;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		തിരികെ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	എങ്കിൽ (len > 0)
		plic->അക്കം_src = fdt32_to_cpu(*val);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_plic(സ്ഥിരം ശൂന്യം *fdt, ഘടന plic_data *plic, സ്ഥിരം അക്ഷരം *compat)
{
	പൂർണ്ണസംഖ്യ nodeoffset;

	എങ്കിൽ (!compat || !plic || !fdt)
		തിരികെ SBI_ENODEV;

	nodeoffset = fdt_കണ്ണി_offset_by_compatible(fdt, -1, compat);
	എങ്കിൽ (nodeoffset < 0)
		തിരികെ nodeoffset;

	തിരികെ fdt_parse_plic_കണ്ണി(fdt, nodeoffset, plic);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_get_aclint_addr_size_by_name(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	പൂർണ്ണസംഖ്യ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_കണ്ണി_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_കണ്ണി_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		തിരികെ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_get_aclint_addr_size(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	പൂർണ്ണസംഖ്യ rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		തിരികെ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	എങ്കിൽ (out_addr2)
		*out_addr2 = reg_addr;
	എങ്കിൽ (out_size2)
		*out_size2 = reg_size;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_aclint_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_എണ്ണം)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ i, rc, എണ്ണം, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	എങ്കിൽ (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_എണ്ണം)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ശൂന്യ))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	അല്ലെങ്കിൽ
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	എങ്കിൽ (rc)
		തിരികെ rc;

	*out_first_hartid = 0;
	*out_hart_എണ്ണം = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &എണ്ണം);
	എങ്കിൽ (!val || എണ്ണം < വലുപ്പം(fdt32_t))
		തിരികെ 0;
	എണ്ണം = എണ്ണം / വലുപ്പം(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	വേണ്ടി (i = 0; i < (എണ്ണം / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
		എങ്കിൽ (cpu_intc_offset < 0)
			തുടരുക;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		എങ്കിൽ (cpu_offset < 0)
			തുടരുക;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		എങ്കിൽ (rc)
			തുടരുക;

		എങ്കിൽ (match_hwirq == hwirq) {
			എങ്കിൽ (hartid < first_hartid)
				first_hartid = hartid;
			എങ്കിൽ (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	എങ്കിൽ ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_എണ്ണം = last_hartid - first_hartid + 1;
	}
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_plmt_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_എണ്ണം)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ rc, i, എണ്ണം;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	എങ്കിൽ (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_എണ്ണം || !plmt_size)
		തിരികെ SBI_EINVAL;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0)
		തിരികെ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &എണ്ണം);
	എങ്കിൽ (!val || എണ്ണം < വലുപ്പം(fdt32_t))
		തിരികെ 0;
	എണ്ണം = എണ്ണം / വലുപ്പം(fdt32_t);

	hcount = 0;
	വേണ്ടി (i = 0; i < (എണ്ണം / 2); i++) {
		പൂർണ്ണസംഖ്യ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
		എങ്കിൽ (cpu_intc_offset < 0)
			തുടരുക;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		എങ്കിൽ (cpu_offset < 0)
			തുടരുക;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		എങ്കിൽ (rc)
			തുടരുക;

		എങ്കിൽ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			തുടരുക;

		എങ്കിൽ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_എണ്ണം = hcount;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_plicsw_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_എണ്ണം)
{
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ rc, i, എണ്ണം;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	എങ്കിൽ (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_എണ്ണം || !size)
		തിരികെ SBI_EINVAL;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	എങ്കിൽ (rc < 0)
		തിരികെ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &എണ്ണം);
	എങ്കിൽ (!val || എണ്ണം < വലുപ്പം(fdt32_t))
		തിരികെ 0;
	എണ്ണം = എണ്ണം / വലുപ്പം(fdt32_t);

	hcount = 0;
	വേണ്ടി (i = 0; i < (എണ്ണം / 2); i++) {
		പൂർണ്ണസംഖ്യ cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_കണ്ണി_offset_by_phandle(fdt, phandle);
		എങ്കിൽ (cpu_intc_offset < 0)
			തുടരുക;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		എങ്കിൽ (cpu_offset < 0)
			തുടരുക;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		എങ്കിൽ (rc)
			തുടരുക;

		എങ്കിൽ (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			തുടരുക;

		എങ്കിൽ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_എണ്ണം = hcount;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_parse_compat_addr(സ്ഥിരം ശൂന്യം *fdt, uint64_t *addr,
			  സ്ഥിരം അക്ഷരം *compatible)
{
	പൂർണ്ണസംഖ്യ nodeoffset, rc;

	nodeoffset = fdt_കണ്ണി_offset_by_compatible(fdt, -1, compatible);
	എങ്കിൽ (nodeoffset < 0)
		തിരികെ nodeoffset;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoffset, 0, addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 || !addr)
		തിരികെ SBI_ENODEV;

	തിരികെ 0;
}
