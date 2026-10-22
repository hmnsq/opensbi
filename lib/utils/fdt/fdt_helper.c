// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_hartmask.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/irqchip/aplic.h>
#அந்தர்பாவ <sbi_utils/irqchip/imsic.h>
#அந்தர்பாவ <sbi_utils/irqchip/plic.h>

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

முழு fdt_parse_phandle_with_args(மாறிலி வெறுமை *fdt, முழு nodeoff,
				மாறிலி எழுத்து *prop, மாறிலி எழுத்து *cells_prop,
				முழு index, கட்டமைப்பு fdt_phandle_args *out_args)
{
	u32 i, pcells;
	முழு len, pnodeoff;
	மாறிலி fdt32_t *list, *list_end, *val;

	எனில் (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		திரும்பு SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	எனில் (!list)
		திரும்பு SBI_ENOENT;
	list_end = list + (len / அளவு(*list));

	போது (list < list_end) {
		pnodeoff = fdt_முனை_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		எனில் (pnodeoff < 0)
			திரும்பு pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		எனில் (!val)
			திரும்பு SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		எனில் (FDT_MAX_PHANDLE_ARGS < pcells)
			திரும்பு SBI_EINVAL;
		எனில் (list + pcells > list_end)
			திரும்பு SBI_ENOENT;

		எனில் (index > 0) {
			list += pcells;
			index--;
		} இல்லையெனில் {
			out_args->முனை_offset = pnodeoff;
			out_args->args_மொத்தம் = pcells;
			க்கு (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			திரும்பு 0;
		}
	}

	திரும்பு SBI_ENOENT;
}

நிலையான முழு fdt_translate_address(மாறிலி வெறுமை *fdt, uint64_t reg, முழு parent,
				 uint64_t *addr)
{
	முழு i, rlen;
	முழு cell_parent_addr, cell_child_addr, cell_size;
	மாறிலி fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	எனில் (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		எனில் (cell_child_addr < 1)
			திரும்பு SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		எனில் (cell_parent_addr < 1)
			திரும்பு SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		எனில் (cell_size < 0)
			திரும்பு SBI_ENODEV;

		க்கு (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		க்கு (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		க்கு (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		எனில் (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			திரும்பு SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} இல்லையெனில் {
		/* No translation required */
		*addr = reg;
	}

	திரும்பு 0;
}

முழு fdt_get_முனை_addr_size(மாறிலி வெறுமை *fdt, முழு முனை, முழு index,
			   uint64_t *addr, uint64_t *size)
{
	முழு parent, len, i, rc;
	முழு cell_addr, cell_size;
	மாறிலி fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	எனில் (!fdt || முனை < 0 || index < 0)
		திரும்பு SBI_EINVAL;

	parent = fdt_parent_offset(fdt, முனை);
	எனில் (parent < 0)
		திரும்பு parent;
	cell_addr = fdt_address_cells(fdt, parent);
	எனில் (cell_addr < 1)
		திரும்பு SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	எனில் (cell_size < 0)
		திரும்பு SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, முனை, "reg", &len);
	எனில் (!prop_addr)
		திரும்பு SBI_ENODEV;

	எனில் ((len / அளவு(u32)) <= (index * (cell_addr + cell_size)))
		திரும்பு SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	எனில் (addr) {
		க்கு (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		செய் {
			எனில் (parent < 0)
				நிறுத்து;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			எனில் (rc)
				நிறுத்து;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} போது (1);
	}
	temp = 0;

	எனில் (size) {
		க்கு (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	திரும்பு 0;
}

முழு fdt_get_முனை_addr_size_by_name(மாறிலி வெறுமை *fdt, முழு முனை, மாறிலி எழுத்து *name,
				   uint64_t *addr, uint64_t *size)
{
	முழு i, j, மொத்தம்;
	மாறிலி எழுத்து *val;
	மாறிலி எழுத்து *regname;

	எனில் (!fdt || முனை < 0 || !name)
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, முனை, "reg-names", &மொத்தம்);
	எனில் (!val)
		திரும்பு SBI_ENODEV;

	க்கு (i = 0, j = 0; i < மொத்தம்; i++, j++) {
		regname = val + i;

		எனில் (strcmp(name, regname) == 0)
			திரும்பு fdt_get_முனை_addr_size(fdt, முனை, j, addr, size);

		i += strlen(regname);
	}

	திரும்பு SBI_ENODEV;
}

bool fdt_முனை_is_enabled(மாறிலி வெறுமை *fdt, முழு nodeoff)
{
	முழு len;
	மாறிலி வெறுமை *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	எனில் (!prop)
		திரும்பு true;

	எனில் (!strncmp(prop, "okay", strlen("okay")))
		திரும்பு true;

	எனில் (!strncmp(prop, "ok", strlen("ok")))
		திரும்பு true;

	திரும்பு false;
}

முழு fdt_parse_hart_id(மாறிலி வெறுமை *fdt, முழு cpu_offset, u32 *hartid)
{
	முழு len;
	மாறிலி வெறுமை *prop;
	மாறிலி fdt32_t *val;

	எனில் (!fdt || cpu_offset < 0)
		திரும்பு SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	எனில் (!prop || !len)
		திரும்பு SBI_EINVAL;
	எனில் (strncmp (prop, "cpu", strlen ("cpu")))
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	எனில் (!val || len < அளவு(fdt32_t))
		திரும்பு SBI_EINVAL;

	எனில் (len > அளவு(fdt32_t))
		val++;

	எனில் (hartid)
		*hartid = fdt32_to_cpu(*val);

	திரும்பு 0;
}

முழு fdt_parse_cbom_block_size(மாறிலி வெறுமை *fdt, முழு cpu_offset, unsigned long *cbom_block_size)
{
	முழு len;
	மாறிலி வெறுமை *prop;
	மாறிலி fdt32_t *val;

	எனில் (!fdt || cpu_offset < 0)
		திரும்பு SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	எனில் (!prop || !len)
		திரும்பு SBI_EINVAL;
	எனில் (strncmp (prop, "cpu", strlen ("cpu")))
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	எனில் (!val || len < அளவு(fdt32_t))
		திரும்பு SBI_EINVAL;

	எனில் (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	திரும்பு 0;
}

முழு fdt_parse_max_enabled_hart_id(மாறிலி வெறுமை *fdt, u32 *max_hartid)
{
	u32 hartid;
	முழு err, cpu_offset, cpus_offset;

	எனில் (!fdt)
		திரும்பு SBI_EINVAL;
	எனில் (!max_hartid)
		திரும்பு 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		எனில் (err)
			தொடர்;

		எனில் (!fdt_முனை_is_enabled(fdt, cpu_offset))
			தொடர்;

		எனில் (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	திரும்பு 0;
}

முழு fdt_parse_timebase_frequency(மாறிலி வெறுமை *fdt, unsigned long *freq)
{
	மாறிலி fdt32_t *val;
	முழு len, cpus_offset;

	எனில் (!fdt || !freq)
		திரும்பு SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	எனில் (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	இல்லையெனில்
		திரும்பு SBI_ENOENT;

	திரும்பு 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

நிலையான முழு fdt_parse_isa_one_hart(மாறிலி எழுத்து *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	எழுத்து mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	எனில் (isa[i] == 'r' || isa[i] == 'R')
		i++;
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	எனில் (isa[i] == 'v' || isa[i] == 'V')
		i++;
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	எனில் (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	இல்லையெனில் எனில் (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	/* Skip base ISA extensions */
	க்கு (; i < isa_len; i++) {
		எனில் (isa[i] == '_')
			நிறுத்து;
	}

	போது (i < isa_len) {
		எனில் (isa[i] != '_') {
			i++;
			தொடர்;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		போது ((i < isa_len) && (isa[i] != '_') &&
		       (j < (அளவு(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		எனில் (!j)
			தொடர்;

#define set_multi_letter_ext(name, bit)				\
			எனில் (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				தொடர்;			\
			}

		க்கு (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	திரும்பு 0;
}

நிலையான வெறுமை fdt_parse_isa_extensions_one_hart(மாறிலி எழுத்து *isa,
					      unsigned long *extensions,
					      முழு len)
{
	size_t i;

	க்கு (i = 0; i < SBI_HART_EXT_MAX; i++) {
		எனில் (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

முழு fdt_parse_isa_extensions_all_harts(மாறிலி வெறுமை *fdt)
{
	u32 hartid;
	மாறிலி fdt32_t *val;
	கட்டமைப்பு sbi_scratch *scratch;
	கட்டமைப்பு sbi_hart_features *hfeatures;
	முழு err, cpu_offset, cpus_offset, len;

	எனில் (!fdt)
		திரும்பு SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	எனில் (cpus_offset < 0)
		திரும்பு cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		எனில் (err)
			தொடர்;

		எனில் (!fdt_முனை_is_enabled(fdt, cpu_offset))
			தொடர்;

		scratch = sbi_hartid_to_scratch(hartid);
		எனில் (!scratch)
			திரும்பு SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		எனில் (!hfeatures)
			திரும்பு SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		எனில் (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((மாறிலி எழுத்து *)val,
							  hfeatures->extensions, len);
			தொடர்;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		எனில் (!val || len <= 0)
			திரும்பு SBI_ENOENT;

		err = fdt_parse_isa_one_hart((மாறிலி எழுத்து *)val, hfeatures->extensions);
		எனில் (err)
			திரும்பு err;
	}

	திரும்பு 0;
}

நிலையான முழு fdt_parse_uart_முனை_common(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				      கட்டமைப்பு platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	முழு len, rc;
	மாறிலி fdt32_t *val;
	uint64_t reg_addr, reg_size;

	எனில் (nodeoffset < 0 || !uart || !fdt)
		திரும்பு SBI_ENODEV;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		திரும்பு SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	எனில் (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	இல்லையெனில்
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	எனில் (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	இல்லையெனில்
		uart->baud = default_baud;

	திரும்பு 0;
}

முழு fdt_parse_gaisler_uart_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				கட்டமைப்பு platform_uart_data *uart)
{
	திரும்பு fdt_parse_uart_முனை_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

முழு fdt_parse_renesas_scif_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				கட்டமைப்பு platform_uart_data *uart)
{
	திரும்பு fdt_parse_uart_முனை_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

முழு fdt_parse_shakti_uart_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			       கட்டமைப்பு platform_uart_data *uart)
{
	திரும்பு fdt_parse_uart_முனை_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

முழு fdt_parse_sifive_uart_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			       கட்டமைப்பு platform_uart_data *uart)
{
	திரும்பு fdt_parse_uart_முனை_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

முழு fdt_parse_uart_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			கட்டமைப்பு platform_uart_data *uart)
{
	முழு len, rc;
	மாறிலி fdt32_t *val;

	rc = fdt_parse_uart_முனை_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	எனில் (rc)
		திரும்பு rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	எனில் (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	இல்லையெனில்
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	எனில் (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	இல்லையெனில்
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	எனில் (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	இல்லையெனில்
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	திரும்பு 0;
}

முழு fdt_parse_uart8250(மாறிலி வெறுமை *fdt, கட்டமைப்பு platform_uart_data *uart,
		   மாறிலி எழுத்து *compatible)
{
	முழு nodeoffset;

	எனில் (!compatible || !uart || !fdt)
		திரும்பு SBI_ENODEV;

	nodeoffset = fdt_முனை_offset_by_compatible(fdt, -1, compatible);
	எனில் (nodeoffset < 0)
		திரும்பு nodeoffset;

	திரும்பு fdt_parse_uart_முனை(fdt, nodeoffset, uart);
}

முழு fdt_parse_xlnx_uartlite_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			       கட்டமைப்பு platform_uart_data *uart)
{
	திரும்பு fdt_parse_uart_முனை_common(fdt, nodeoffset, uart, 0, 0);
}

நிலையான முழு fdt_aplic_find_imsic_முனை(மாறிலி வெறுமை *fdt, முழு nodeoff,
				     கட்டமைப்பு imsic_data *imsic, bool mmode)
{
	மாறிலி fdt32_t *val;
	முழு i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	எனில் (val && len >= அளவு(fdt32_t)) {
		noff = fdt_முனை_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		எனில் (noff < 0)
			திரும்பு noff;

		rc = fdt_parse_imsic_முனை(fdt, noff, imsic);
		எனில் (rc)
			திரும்பு rc;

		rc = imsic_data_check(imsic);
		எனில் (rc)
			திரும்பு rc;

		எனில் (imsic->targets_mmode == mmode) {
			திரும்பு 0;
		}
	} இல்லையெனில் {
		திரும்பு SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	எனில் (!val || len < அளவு(fdt32_t))
		திரும்பு SBI_ENODEV;

	len /= அளவு(fdt32_t);

	க்கு (i = 0; i < len; i++) {
		noff = fdt_முனை_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		எனில் (noff < 0)
			திரும்பு noff;

		rc = fdt_aplic_find_imsic_முனை(fdt, noff, imsic, mmode);
		எனில் (!rc)
			நிறுத்து;
	}

	திரும்பு rc;
}

முழு fdt_parse_aplic_முனை(மாறிலி வெறுமை *fdt, முழு nodeoff, கட்டமைப்பு aplic_data *aplic)
{
	bool child_found;
	மாறிலி fdt32_t *val;
	மாறிலி fdt32_t *del;
	கட்டமைப்பு imsic_data imsic = { 0 };
	முழு i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	கட்டமைப்பு aplic_delegate_data *deleg;

	எனில் (nodeoff < 0 || !aplic || !fdt)
		திரும்பு SBI_ENODEV;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		திரும்பு SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	எனில் (len > 0)
		aplic->இலக்கம்_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	எனில் (val && len > அளவு(fdt32_t)) {
		len = len / அளவு(fdt32_t);
		க்கு (i = 0; i < len; i += 2) {
			எனில் (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				நிறுத்து;
			}
		}
		aplic->இலக்கம்_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_முனை(fdt, nodeoff, &imsic, true);
	எனில் (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		எனில் (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			திரும்பு SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_முனை(fdt, nodeoff, &imsic, false);
	எனில் (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		எனில் (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			திரும்பு SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	க்கு (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	எனில் (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	எனில் (!del || len < (3 * அளவு(fdt32_t)))
		செல் skip_delegate_parse;
	d = 0;
	dcnt = len / அளவு(fdt32_t);
	க்கு (i = 0; i < dcnt; i += 3) {
		எனில் (d >= APLIC_MAX_DELEGATE)
			நிறுத்து;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		எனில் (!val || len < அளவு(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			தொடர்;
		}
		len = len / அளவு(fdt32_t);
		க்கு (j = 0; j < len; j++) {
			எனில் (del[i] != val[j])
				தொடர்;
			deleg->child_index = j;
			child_found = true;
			நிறுத்து;
		}

		எனில் (child_found) {
			d++;
		} இல்லையெனில் {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	திரும்பு 0;
}

bool fdt_check_imsic_mlevel(மாறிலி வெறுமை *fdt)
{
	மாறிலி fdt32_t *val;
	முழு i, len, noff = 0;

	எனில் (!fdt)
		திரும்பு false;

	போது ((noff = fdt_முனை_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		எனில் (val && len > அளவு(fdt32_t)) {
			len = len / அளவு(fdt32_t);
			க்கு (i = 0; i < len; i += 2) {
				எனில் (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					திரும்பு true;
			}
		}
	}

	திரும்பு false;
}

முழு fdt_parse_imsic_முனை(மாறிலி வெறுமை *fdt, முழு nodeoff, கட்டமைப்பு imsic_data *imsic)
{
	மாறிலி fdt32_t *val;
	கட்டமைப்பு imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	முழு i, rc, len, nr_parent_irqs;

	எனில் (nodeoff < 0 || !imsic || !fdt)
		திரும்பு SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	எனில் (val && len > அளவு(fdt32_t)) {
		len = len / அளவு(fdt32_t);
		nr_parent_irqs = len / 2;
		க்கு (i = 0; i < len; i += 2) {
			எனில் (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				நிறுத்து;
			}
		}
	} இல்லையெனில்
		திரும்பு SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	எனில் (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	இல்லையெனில்
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	எனில் (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} இல்லையெனில் {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		எனில் ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	எனில் (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	இல்லையெனில்
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	எனில் (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	இல்லையெனில்
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	எனில் (val && len > 0)
		imsic->இலக்கம்_ids = fdt32_to_cpu(*val);
	இல்லையெனில்
		திரும்பு SBI_EINVAL;

	க்கு (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	க்கு (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_முனை_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		எனில் (rc < 0 || !reg_size)
			நிறுத்து;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	எனில் (!imsic->regs[0].size)
		திரும்பு SBI_EINVAL;

	திரும்பு 0;
}

முழு fdt_parse_plic_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset, கட்டமைப்பு plic_data *plic)
{
	முழு len, rc;
	மாறிலி fdt32_t *val;
	uint64_t reg_addr, reg_size;

	எனில் (nodeoffset < 0 || !plic || !fdt)
		திரும்பு SBI_ENODEV;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		திரும்பு SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	எனில் (len > 0)
		plic->இலக்கம்_src = fdt32_to_cpu(*val);

	திரும்பு 0;
}

முழு fdt_parse_plic(மாறிலி வெறுமை *fdt, கட்டமைப்பு plic_data *plic, மாறிலி எழுத்து *compat)
{
	முழு nodeoffset;

	எனில் (!compat || !plic || !fdt)
		திரும்பு SBI_ENODEV;

	nodeoffset = fdt_முனை_offset_by_compatible(fdt, -1, compat);
	எனில் (nodeoffset < 0)
		திரும்பு nodeoffset;

	திரும்பு fdt_parse_plic_முனை(fdt, nodeoffset, plic);
}

நிலையான முழு fdt_get_aclint_addr_size_by_name(மாறிலி வெறுமை *fdt, முழு nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	முழு rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_முனை_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_முனை_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		திரும்பு SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	திரும்பு 0;
}

நிலையான முழு fdt_get_aclint_addr_size(மாறிலி வெறுமை *fdt, முழு nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	முழு rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		திரும்பு SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	எனில் (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	எனில் (out_addr2)
		*out_addr2 = reg_addr;
	எனில் (out_size2)
		*out_size2 = reg_size;

	திரும்பு 0;
}

முழு fdt_parse_aclint_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_மொத்தம்)
{
	மாறிலி fdt32_t *val;
	முழு i, rc, மொத்தம், cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	எனில் (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_மொத்தம்)
		திரும்பு SBI_EINVAL;

	எனில் (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", வெற்று))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	இல்லையெனில்
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	எனில் (rc)
		திரும்பு rc;

	*out_first_hartid = 0;
	*out_hart_மொத்தம் = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &மொத்தம்);
	எனில் (!val || மொத்தம் < அளவு(fdt32_t))
		திரும்பு 0;
	மொத்தம் = மொத்தம் / அளவு(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	க்கு (i = 0; i < (மொத்தம் / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_முனை_offset_by_phandle(fdt, phandle);
		எனில் (cpu_intc_offset < 0)
			தொடர்;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		எனில் (cpu_offset < 0)
			தொடர்;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		எனில் (rc)
			தொடர்;

		எனில் (match_hwirq == hwirq) {
			எனில் (hartid < first_hartid)
				first_hartid = hartid;
			எனில் (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	எனில் ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_மொத்தம் = last_hartid - first_hartid + 1;
	}
	திரும்பு 0;
}

முழு fdt_parse_plmt_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_மொத்தம்)
{
	மாறிலி fdt32_t *val;
	முழு rc, i, மொத்தம்;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	எனில் (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_மொத்தம் || !plmt_size)
		திரும்பு SBI_EINVAL;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	எனில் (rc < 0)
		திரும்பு SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &மொத்தம்);
	எனில் (!val || மொத்தம் < அளவு(fdt32_t))
		திரும்பு 0;
	மொத்தம் = மொத்தம் / அளவு(fdt32_t);

	hcount = 0;
	க்கு (i = 0; i < (மொத்தம் / 2); i++) {
		முழு cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_முனை_offset_by_phandle(fdt, phandle);
		எனில் (cpu_intc_offset < 0)
			தொடர்;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		எனில் (cpu_offset < 0)
			தொடர்;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		எனில் (rc)
			தொடர்;

		எனில் (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			தொடர்;

		எனில் (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_மொத்தம் = hcount;

	திரும்பு 0;
}

முழு fdt_parse_plicsw_முனை(மாறிலி வெறுமை *fdt, முழு nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_மொத்தம்)
{
	மாறிலி fdt32_t *val;
	முழு rc, i, மொத்தம்;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	எனில் (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_மொத்தம் || !size)
		திரும்பு SBI_EINVAL;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	எனில் (rc < 0)
		திரும்பு SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &மொத்தம்);
	எனில் (!val || மொத்தம் < அளவு(fdt32_t))
		திரும்பு 0;
	மொத்தம் = மொத்தம் / அளவு(fdt32_t);

	hcount = 0;
	க்கு (i = 0; i < (மொத்தம் / 2); i++) {
		முழு cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_முனை_offset_by_phandle(fdt, phandle);
		எனில் (cpu_intc_offset < 0)
			தொடர்;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		எனில் (cpu_offset < 0)
			தொடர்;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		எனில் (rc)
			தொடர்;

		எனில் (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			தொடர்;

		எனில் (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_மொத்தம் = hcount;

	திரும்பு 0;
}

முழு fdt_parse_compat_addr(மாறிலி வெறுமை *fdt, uint64_t *addr,
			  மாறிலி எழுத்து *compatible)
{
	முழு nodeoffset, rc;

	nodeoffset = fdt_முனை_offset_by_compatible(fdt, -1, compatible);
	எனில் (nodeoffset < 0)
		திரும்பு nodeoffset;

	rc = fdt_get_முனை_addr_size(fdt, nodeoffset, 0, addr, வெற்று);
	எனில் (rc < 0 || !addr)
		திரும்பு SBI_ENODEV;

	திரும்பு 0;
}
