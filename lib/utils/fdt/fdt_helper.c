// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_hartmask.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/irqchip/aplic.h>
#अन्तर्भाव <sbi_utils/irqchip/imsic.h>
#अन्तर्भाव <sbi_utils/irqchip/plic.h>

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

पूर्णाङ्क fdt_parse_phandle_with_args(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव वर्ण *prop, ध्रुव वर्ण *cells_prop,
				पूर्णाङ्क index, संरचना fdt_phandle_args *out_args)
{
	u32 i, pcells;
	पूर्णाङ्क len, pnodeoff;
	ध्रुव fdt32_t *list, *list_end, *val;

	सचे (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		पटिनिवत्त SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	सचे (!list)
		पटिनिवत्त SBI_ENOENT;
	list_end = list + (len / परिमाण(*list));

	याव (list < list_end) {
		pnodeoff = fdt_ग्रन्थिः_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		सचे (pnodeoff < 0)
			पटिनिवत्त pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		सचे (!val)
			पटिनिवत्त SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		सचे (FDT_MAX_PHANDLE_ARGS < pcells)
			पटिनिवत्त SBI_EINVAL;
		सचे (list + pcells > list_end)
			पटिनिवत्त SBI_ENOENT;

		सचे (index > 0) {
			list += pcells;
			index--;
		} अञ्ञथा {
			out_args->ग्रन्थिः_offset = pnodeoff;
			out_args->args_गणनम् = pcells;
			अत्थाय (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			पटिनिवत्त 0;
		}
	}

	पटिनिवत्त SBI_ENOENT;
}

स्थिर पूर्णाङ्क fdt_translate_address(ध्रुव शून्य *fdt, uint64_t reg, पूर्णाङ्क parent,
				 uint64_t *addr)
{
	पूर्णाङ्क i, rlen;
	पूर्णाङ्क cell_parent_addr, cell_child_addr, cell_size;
	ध्रुव fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	सचे (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		सचे (cell_child_addr < 1)
			पटिनिवत्त SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		सचे (cell_parent_addr < 1)
			पटिनिवत्त SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		सचे (cell_size < 0)
			पटिनिवत्त SBI_ENODEV;

		अत्थाय (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		अत्थाय (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		अत्थाय (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		सचे (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			पटिनिवत्त SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} अञ्ञथा {
		/* No translation required */
		*addr = reg;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_get_ग्रन्थिः_addr_size(ध्रुव शून्य *fdt, पूर्णाङ्क ग्रन्थिः, पूर्णाङ्क index,
			   uint64_t *addr, uint64_t *size)
{
	पूर्णाङ्क parent, len, i, rc;
	पूर्णाङ्क cell_addr, cell_size;
	ध्रुव fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	सचे (!fdt || ग्रन्थिः < 0 || index < 0)
		पटिनिवत्त SBI_EINVAL;

	parent = fdt_parent_offset(fdt, ग्रन्थिः);
	सचे (parent < 0)
		पटिनिवत्त parent;
	cell_addr = fdt_address_cells(fdt, parent);
	सचे (cell_addr < 1)
		पटिनिवत्त SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	सचे (cell_size < 0)
		पटिनिवत्त SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ग्रन्थिः, "reg", &len);
	सचे (!prop_addr)
		पटिनिवत्त SBI_ENODEV;

	सचे ((len / परिमाण(u32)) <= (index * (cell_addr + cell_size)))
		पटिनिवत्त SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	सचे (addr) {
		अत्थाय (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		करोहि {
			सचे (parent < 0)
				भङ्ग;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			सचे (rc)
				भङ्ग;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} याव (1);
	}
	temp = 0;

	सचे (size) {
		अत्थाय (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_get_ग्रन्थिः_addr_size_by_name(ध्रुव शून्य *fdt, पूर्णाङ्क ग्रन्थिः, ध्रुव वर्ण *name,
				   uint64_t *addr, uint64_t *size)
{
	पूर्णाङ्क i, j, गणनम्;
	ध्रुव वर्ण *val;
	ध्रुव वर्ण *regname;

	सचे (!fdt || ग्रन्थिः < 0 || !name)
		पटिनिवत्त SBI_EINVAL;

	val = fdt_getprop(fdt, ग्रन्थिः, "reg-names", &गणनम्);
	सचे (!val)
		पटिनिवत्त SBI_ENODEV;

	अत्थाय (i = 0, j = 0; i < गणनम्; i++, j++) {
		regname = val + i;

		सचे (strcmp(name, regname) == 0)
			पटिनिवत्त fdt_get_ग्रन्थिः_addr_size(fdt, ग्रन्थिः, j, addr, size);

		i += strlen(regname);
	}

	पटिनिवत्त SBI_ENODEV;
}

bool fdt_ग्रन्थिः_is_enabled(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	सचे (!prop)
		पटिनिवत्त true;

	सचे (!strncmp(prop, "okay", strlen("okay")))
		पटिनिवत्त true;

	सचे (!strncmp(prop, "ok", strlen("ok")))
		पटिनिवत्त true;

	पटिनिवत्त false;
}

पूर्णाङ्क fdt_parse_hart_id(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_offset, u32 *hartid)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;
	ध्रुव fdt32_t *val;

	सचे (!fdt || cpu_offset < 0)
		पटिनिवत्त SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	सचे (!prop || !len)
		पटिनिवत्त SBI_EINVAL;
	सचे (strncmp (prop, "cpu", strlen ("cpu")))
		पटिनिवत्त SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	सचे (!val || len < परिमाण(fdt32_t))
		पटिनिवत्त SBI_EINVAL;

	सचे (len > परिमाण(fdt32_t))
		val++;

	सचे (hartid)
		*hartid = fdt32_to_cpu(*val);

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_cbom_block_size(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_offset, unsigned long *cbom_block_size)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;
	ध्रुव fdt32_t *val;

	सचे (!fdt || cpu_offset < 0)
		पटिनिवत्त SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	सचे (!prop || !len)
		पटिनिवत्त SBI_EINVAL;
	सचे (strncmp (prop, "cpu", strlen ("cpu")))
		पटिनिवत्त SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	सचे (!val || len < परिमाण(fdt32_t))
		पटिनिवत्त SBI_EINVAL;

	सचे (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_max_enabled_hart_id(ध्रुव शून्य *fdt, u32 *max_hartid)
{
	u32 hartid;
	पूर्णाङ्क err, cpu_offset, cpus_offset;

	सचे (!fdt)
		पटिनिवत्त SBI_EINVAL;
	सचे (!max_hartid)
		पटिनिवत्त 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	सचे (cpus_offset < 0)
		पटिनिवत्त cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (err)
			अनुवर्त;

		सचे (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_offset))
			अनुवर्त;

		सचे (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_timebase_frequency(ध्रुव शून्य *fdt, unsigned long *freq)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क len, cpus_offset;

	सचे (!fdt || !freq)
		पटिनिवत्त SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	सचे (cpus_offset < 0)
		पटिनिवत्त cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	सचे (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	अञ्ञथा
		पटिनिवत्त SBI_ENOENT;

	पटिनिवत्त 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

स्थिर पूर्णाङ्क fdt_parse_isa_one_hart(ध्रुव वर्ण *isa, unsigned long *extensions)
{
	size_t i, j, isa_len;
	वर्ण mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = strlen(isa);

	सचे (isa[i] == 'r' || isa[i] == 'R')
		i++;
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	सचे (isa[i] == 'v' || isa[i] == 'V')
		i++;
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	सचे (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	अञ्ञथा सचे (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	/* Skip base ISA extensions */
	अत्थाय (; i < isa_len; i++) {
		सचे (isa[i] == '_')
			भङ्ग;
	}

	याव (i < isa_len) {
		सचे (isa[i] != '_') {
			i++;
			अनुवर्त;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		याव ((i < isa_len) && (isa[i] != '_') &&
		       (j < (परिमाण(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		सचे (!j)
			अनुवर्त;

#define set_multi_letter_ext(name, bit)				\
			सचे (!strcmp(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				अनुवर्त;			\
			}

		अत्थाय (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	पटिनिवत्त 0;
}

स्थिर शून्य fdt_parse_isa_extensions_one_hart(ध्रुव वर्ण *isa,
					      unsigned long *extensions,
					      पूर्णाङ्क len)
{
	size_t i;

	अत्थाय (i = 0; i < SBI_HART_EXT_MAX; i++) {
		सचे (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

पूर्णाङ्क fdt_parse_isa_extensions_all_harts(ध्रुव शून्य *fdt)
{
	u32 hartid;
	ध्रुव fdt32_t *val;
	संरचना sbi_scratch *scratch;
	संरचना sbi_hart_features *hfeatures;
	पूर्णाङ्क err, cpu_offset, cpus_offset, len;

	सचे (!fdt)
		पटिनिवत्त SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	सचे (cpus_offset < 0)
		पटिनिवत्त cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (err)
			अनुवर्त;

		सचे (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_offset))
			अनुवर्त;

		scratch = sbi_hartid_to_scratch(hartid);
		सचे (!scratch)
			पटिनिवत्त SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		सचे (!hfeatures)
			पटिनिवत्त SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		सचे (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ध्रुव वर्ण *)val,
							  hfeatures->extensions, len);
			अनुवर्त;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		सचे (!val || len <= 0)
			पटिनिवत्त SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ध्रुव वर्ण *)val, hfeatures->extensions);
		सचे (err)
			पटिनिवत्त err;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क fdt_parse_uart_ग्रन्थिः_common(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				      संरचना platform_uart_data *uart,
				      unsigned long default_freq,
				      unsigned long default_baud)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;
	uint64_t reg_addr, reg_size;

	सचे (nodeoffset < 0 || !uart || !fdt)
		पटिनिवत्त SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		पटिनिवत्त SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	सचे (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	अञ्ञथा
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	सचे (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	अञ्ञथा
		uart->baud = default_baud;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_gaisler_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_data *uart)
{
	पटिनिवत्त fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

पूर्णाङ्क fdt_parse_renesas_scif_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_data *uart)
{
	पटिनिवत्त fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

पूर्णाङ्क fdt_parse_shakti_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	पटिनिवत्त fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

पूर्णाङ्क fdt_parse_sifive_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	पटिनिवत्त fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

पूर्णाङ्क fdt_parse_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			संरचना platform_uart_data *uart)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;

	rc = fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	सचे (rc)
		पटिनिवत्त rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	सचे (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	अञ्ञथा
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	सचे (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	अञ्ञथा
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	सचे (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	अञ्ञथा
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_uart8250(ध्रुव शून्य *fdt, संरचना platform_uart_data *uart,
		   ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset;

	सचे (!compatible || !uart || !fdt)
		पटिनिवत्त SBI_ENODEV;

	nodeoffset = fdt_ग्रन्थिः_offset_by_compatible(fdt, -1, compatible);
	सचे (nodeoffset < 0)
		पटिनिवत्त nodeoffset;

	पटिनिवत्त fdt_parse_uart_ग्रन्थिः(fdt, nodeoffset, uart);
}

पूर्णाङ्क fdt_parse_xlnx_uartlite_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	पटिनिवत्त fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart, 0, 0);
}

स्थिर पूर्णाङ्क fdt_aplic_find_imsic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				     संरचना imsic_data *imsic, bool mmode)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	सचे (val && len >= परिमाण(fdt32_t)) {
		noff = fdt_ग्रन्थिः_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		सचे (noff < 0)
			पटिनिवत्त noff;

		rc = fdt_parse_imsic_ग्रन्थिः(fdt, noff, imsic);
		सचे (rc)
			पटिनिवत्त rc;

		rc = imsic_data_check(imsic);
		सचे (rc)
			पटिनिवत्त rc;

		सचे (imsic->targets_mmode == mmode) {
			पटिनिवत्त 0;
		}
	} अञ्ञथा {
		पटिनिवत्त SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	सचे (!val || len < परिमाण(fdt32_t))
		पटिनिवत्त SBI_ENODEV;

	len /= परिमाण(fdt32_t);

	अत्थाय (i = 0; i < len; i++) {
		noff = fdt_ग्रन्थिः_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		सचे (noff < 0)
			पटिनिवत्त noff;

		rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, noff, imsic, mmode);
		सचे (!rc)
			भङ्ग;
	}

	पटिनिवत्त rc;
}

पूर्णाङ्क fdt_parse_aplic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, संरचना aplic_data *aplic)
{
	bool child_found;
	ध्रुव fdt32_t *val;
	ध्रुव fdt32_t *del;
	संरचना imsic_data imsic = { 0 };
	पूर्णाङ्क i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	संरचना aplic_delegate_data *deleg;

	सचे (nodeoff < 0 || !aplic || !fdt)
		पटिनिवत्त SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		पटिनिवत्त SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	सचे (len > 0)
		aplic->अङ्कः_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	सचे (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		अत्थाय (i = 0; i < len; i += 2) {
			सचे (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				भङ्ग;
			}
		}
		aplic->अङ्कः_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, nodeoff, &imsic, true);
	सचे (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		सचे (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			पटिनिवत्त SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, nodeoff, &imsic, false);
	सचे (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		सचे (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			पटिनिवत्त SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	अत्थाय (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	सचे (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	सचे (!del || len < (3 * परिमाण(fdt32_t)))
		गच्छ skip_delegate_parse;
	d = 0;
	dcnt = len / परिमाण(fdt32_t);
	अत्थाय (i = 0; i < dcnt; i += 3) {
		सचे (d >= APLIC_MAX_DELEGATE)
			भङ्ग;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		सचे (!val || len < परिमाण(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			अनुवर्त;
		}
		len = len / परिमाण(fdt32_t);
		अत्थाय (j = 0; j < len; j++) {
			सचे (del[i] != val[j])
				अनुवर्त;
			deleg->child_index = j;
			child_found = true;
			भङ्ग;
		}

		सचे (child_found) {
			d++;
		} अञ्ञथा {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	पटिनिवत्त 0;
}

bool fdt_check_imsic_mlevel(ध्रुव शून्य *fdt)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, len, noff = 0;

	सचे (!fdt)
		पटिनिवत्त false;

	याव ((noff = fdt_ग्रन्थिः_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		सचे (val && len > परिमाण(fdt32_t)) {
			len = len / परिमाण(fdt32_t);
			अत्थाय (i = 0; i < len; i += 2) {
				सचे (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					पटिनिवत्त true;
			}
		}
	}

	पटिनिवत्त false;
}

पूर्णाङ्क fdt_parse_imsic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, संरचना imsic_data *imsic)
{
	ध्रुव fdt32_t *val;
	संरचना imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	पूर्णाङ्क i, rc, len, nr_parent_irqs;

	सचे (nodeoff < 0 || !imsic || !fdt)
		पटिनिवत्त SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	सचे (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		nr_parent_irqs = len / 2;
		अत्थाय (i = 0; i < len; i += 2) {
			सचे (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				भङ्ग;
			}
		}
	} अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	सचे (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	अञ्ञथा
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	सचे (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} अञ्ञथा {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		सचे ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	सचे (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	अञ्ञथा
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	सचे (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	अञ्ञथा
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	सचे (val && len > 0)
		imsic->अङ्कः_ids = fdt32_to_cpu(*val);
	अञ्ञथा
		पटिनिवत्त SBI_EINVAL;

	अत्थाय (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	अत्थाय (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		सचे (rc < 0 || !reg_size)
			भङ्ग;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	सचे (!imsic->regs[0].size)
		पटिनिवत्त SBI_EINVAL;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_plic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, संरचना plic_data *plic)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;
	uint64_t reg_addr, reg_size;

	सचे (nodeoffset < 0 || !plic || !fdt)
		पटिनिवत्त SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		पटिनिवत्त SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	सचे (len > 0)
		plic->अङ्कः_src = fdt32_to_cpu(*val);

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_plic(ध्रुव शून्य *fdt, संरचना plic_data *plic, ध्रुव वर्ण *compat)
{
	पूर्णाङ्क nodeoffset;

	सचे (!compat || !plic || !fdt)
		पटिनिवत्त SBI_ENODEV;

	nodeoffset = fdt_ग्रन्थिः_offset_by_compatible(fdt, -1, compat);
	सचे (nodeoffset < 0)
		पटिनिवत्त nodeoffset;

	पटिनिवत्त fdt_parse_plic_ग्रन्थिः(fdt, nodeoffset, plic);
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_size_by_name(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
					    unsigned long *out_addr1,
					    unsigned long *out_size1,
					    unsigned long *out_addr2,
					    unsigned long *out_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ग्रन्थिः_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ग्रन्थिः_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		पटिनिवत्त SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_size(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				    unsigned long *out_addr1,
				    unsigned long *out_size1,
				    unsigned long *out_addr2,
				    unsigned long *out_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		पटिनिवत्त SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	सचे (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	सचे (out_addr2)
		*out_addr2 = reg_addr;
	सचे (out_size2)
		*out_size2 = reg_size;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_aclint_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			  bool for_timer, bool allow_regname,
			  unsigned long *out_addr1, unsigned long *out_size1,
			  unsigned long *out_addr2, unsigned long *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, rc, गणनम्, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	सचे (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_गणनम्)
		पटिनिवत्त SBI_EINVAL;

	सचे (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", रिक्त))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	अञ्ञथा
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	सचे (rc)
		पटिनिवत्त rc;

	*out_first_hartid = 0;
	*out_hart_गणनम् = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	सचे (!val || गणनम् < परिमाण(fdt32_t))
		पटिनिवत्त 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	अत्थाय (i = 0; i < (गणनम् / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);
		सचे (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		सचे (cpu_offset < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		सचे (rc)
			अनुवर्त;

		सचे (match_hwirq == hwirq) {
			सचे (hartid < first_hartid)
				first_hartid = hartid;
			सचे (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	सचे ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_गणनम् = last_hartid - first_hartid + 1;
	}
	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_plmt_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, unsigned long *plmt_base,
			  unsigned long *plmt_size, u32 *hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, i, गणनम्;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	सचे (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_गणनम् || !plmt_size)
		पटिनिवत्त SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	सचे (rc < 0)
		पटिनिवत्त SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	सचे (!val || गणनम् < परिमाण(fdt32_t))
		पटिनिवत्त 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	hcount = 0;
	अत्थाय (i = 0; i < (गणनम् / 2); i++) {
		पूर्णाङ्क cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);
		सचे (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		सचे (cpu_offset < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		सचे (rc)
			अनुवर्त;

		सचे (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			अनुवर्त;

		सचे (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_गणनम् = hcount;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_plicsw_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, unsigned long *plicsw_base,
			  unsigned long *size, u32 *hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, i, गणनम्;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	सचे (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_गणनम् || !size)
		पटिनिवत्त SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	सचे (rc < 0)
		पटिनिवत्त SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	सचे (!val || गणनम् < परिमाण(fdt32_t))
		पटिनिवत्त 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	hcount = 0;
	अत्थाय (i = 0; i < (गणनम् / 2); i++) {
		पूर्णाङ्क cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);
		सचे (cpu_intc_offset < 0)
			अनुवर्त;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		सचे (cpu_offset < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		सचे (rc)
			अनुवर्त;

		सचे (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			अनुवर्त;

		सचे (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_गणनम् = hcount;

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_parse_compat_addr(ध्रुव शून्य *fdt, uint64_t *addr,
			  ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset, rc;

	nodeoffset = fdt_ग्रन्थिः_offset_by_compatible(fdt, -1, compatible);
	सचे (nodeoffset < 0)
		पटिनिवत्त nodeoffset;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoffset, 0, addr, रिक्त);
	सचे (rc < 0 || !addr)
		पटिनिवत्त SBI_ENODEV;

	पटिनिवत्त 0;
}
