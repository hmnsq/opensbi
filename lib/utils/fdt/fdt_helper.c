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

पूर्णाङ्क fdt_parse_phandle_with_args(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				अचर वर्ण *prop, अचर वर्ण *cells_prop,
				पूर्णाङ्क index, संरचना fdt_phandle_args *out_args)
{
	u32 i, pcells;
	पूर्णाङ्क len, pnodeoff;
	अचर fdt32_t *list, *list_end, *val;

	यदि (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		फर्काऊ SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	यदि (!list)
		फर्काऊ SBI_ENOENT;
	list_end = list + (len / परिमाण(*list));

	जबसम्म (list < list_end) {
		pnodeoff = fdt_गाँठो_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		यदि (pnodeoff < 0)
			फर्काऊ pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		यदि (!val)
			फर्काऊ SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		यदि (FDT_MAX_PHANDLE_ARGS < pcells)
			फर्काऊ SBI_EINVAL;
		यदि (list + pcells > list_end)
			फर्काऊ SBI_ENOENT;

		यदि (index > 0) {
			list += pcells;
			index--;
		} अन्यथा {
			out_args->गाँठो_offset = pnodeoff;
			out_args->args_गन्ती = pcells;
			लागि (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			फर्काऊ 0;
		}
	}

	फर्काऊ SBI_ENOENT;
}

स्थिर पूर्णाङ्क fdt_translate_address(अचर शून्य *fdt, uint64_t reg, पूर्णाङ्क parent,
				 uint64_t *addr)
{
	पूर्णाङ्क i, rlen;
	पूर्णाङ्क cell_parent_addr, cell_child_addr, cell_size;
	अचर fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	यदि (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		यदि (cell_child_addr < 1)
			फर्काऊ SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		यदि (cell_parent_addr < 1)
			फर्काऊ SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		यदि (cell_size < 0)
			फर्काऊ SBI_ENODEV;

		लागि (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		लागि (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		लागि (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		यदि (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			फर्काऊ SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} अन्यथा {
		/* No translation required */
		*addr = reg;
	}

	फर्काऊ 0;
}

पूर्णाङ्क fdt_get_गाँठो_addr_size(अचर शून्य *fdt, पूर्णाङ्क गाँठो, पूर्णाङ्क index,
			   uint64_t *addr, uint64_t *size)
{
	पूर्णाङ्क parent, len, i, rc;
	पूर्णाङ्क cell_addr, cell_size;
	अचर fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	यदि (!fdt || गाँठो < 0 || index < 0)
		फर्काऊ SBI_EINVAL;

	parent = fdt_parent_offset(fdt, गाँठो);
	यदि (parent < 0)
		फर्काऊ parent;
	cell_addr = fdt_address_cells(fdt, parent);
	यदि (cell_addr < 1)
		फर्काऊ SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	यदि (cell_size < 0)
		फर्काऊ SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, गाँठो, "reg", &len);
	यदि (!prop_addr)
		फर्काऊ SBI_ENODEV;

	यदि ((len / परिमाण(u32)) <= (index * (cell_addr + cell_size)))
		फर्काऊ SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	यदि (addr) {
		लागि (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		गर {
			यदि (parent < 0)
				रोक;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			यदि (rc)
				रोक;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} जबसम्म (1);
	}
	temp = 0;

	यदि (size) {
		लागि (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	फर्काऊ 0;
}

पूर्णाङ्क fdt_get_गाँठो_addr_size_by_name(अचर शून्य *fdt, पूर्णाङ्क गाँठो, अचर वर्ण *name,
				   uint64_t *addr, uint64_t *size)
{
	पूर्णाङ्क i, j, गन्ती;
	अचर वर्ण *val;
	अचर वर्ण *regname;

	यदि (!fdt || गाँठो < 0 || !name)
		फर्काऊ SBI_EINVAL;

	val = fdt_getprop(fdt, गाँठो, "reg-names", &गन्ती);
	यदि (!val)
		फर्काऊ SBI_ENODEV;

	लागि (i = 0, j = 0; i < गन्ती; i++, j++) {
		regname = val + i;

		यदि (म_भेद(name, regname) == 0)
			फर्काऊ fdt_get_गाँठो_addr_size(fdt, गाँठो, j, addr, size);

		i += म_माप(regname);
	}

	फर्काऊ SBI_ENODEV;
}

bool fdt_गाँठो_is_enabled(अचर शून्य *fdt, पूर्णाङ्क nodeoff)
{
	पूर्णाङ्क len;
	अचर शून्य *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	यदि (!prop)
		फर्काऊ true;

	यदि (!म_भेदन(prop, "okay", म_माप("okay")))
		फर्काऊ true;

	यदि (!म_भेदन(prop, "ok", म_माप("ok")))
		फर्काऊ true;

	फर्काऊ false;
}

पूर्णाङ्क fdt_parse_hart_id(अचर शून्य *fdt, पूर्णाङ्क cpu_offset, u32 *hartid)
{
	पूर्णाङ्क len;
	अचर शून्य *prop;
	अचर fdt32_t *val;

	यदि (!fdt || cpu_offset < 0)
		फर्काऊ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	यदि (!prop || !len)
		फर्काऊ SBI_EINVAL;
	यदि (म_भेदन (prop, "cpu", म_माप ("cpu")))
		फर्काऊ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		फर्काऊ SBI_EINVAL;

	यदि (len > परिमाण(fdt32_t))
		val++;

	यदि (hartid)
		*hartid = fdt32_to_cpu(*val);

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_cbom_block_size(अचर शून्य *fdt, पूर्णाङ्क cpu_offset, अचिन्हित दीर्घ *cbom_block_size)
{
	पूर्णाङ्क len;
	अचर शून्य *prop;
	अचर fdt32_t *val;

	यदि (!fdt || cpu_offset < 0)
		फर्काऊ SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	यदि (!prop || !len)
		फर्काऊ SBI_EINVAL;
	यदि (म_भेदन (prop, "cpu", म_माप ("cpu")))
		फर्काऊ SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		फर्काऊ SBI_EINVAL;

	यदि (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_max_enabled_hart_id(अचर शून्य *fdt, u32 *max_hartid)
{
	u32 hartid;
	पूर्णाङ्क err, cpu_offset, cpus_offset;

	यदि (!fdt)
		फर्काऊ SBI_EINVAL;
	यदि (!max_hartid)
		फर्काऊ 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_offset < 0)
		फर्काऊ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (err)
			जारी;

		यदि (!fdt_गाँठो_is_enabled(fdt, cpu_offset))
			जारी;

		यदि (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_timebase_frequency(अचर शून्य *fdt, अचिन्हित दीर्घ *freq)
{
	अचर fdt32_t *val;
	पूर्णाङ्क len, cpus_offset;

	यदि (!fdt || !freq)
		फर्काऊ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_offset < 0)
		फर्काऊ cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	यदि (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	अन्यथा
		फर्काऊ SBI_ENOENT;

	फर्काऊ 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

स्थिर पूर्णाङ्क fdt_parse_isa_one_hart(अचर वर्ण *isa, अचिन्हित दीर्घ *extensions)
{
	माप_प्रकार i, j, isa_len;
	वर्ण mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = म_माप(isa);

	यदि (isa[i] == 'r' || isa[i] == 'R')
		i++;
	अन्यथा
		फर्काऊ SBI_EINVAL;

	यदि (isa[i] == 'v' || isa[i] == 'V')
		i++;
	अन्यथा
		फर्काऊ SBI_EINVAL;

	यदि (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	अन्यथा यदि (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	अन्यथा
		फर्काऊ SBI_EINVAL;

	/* Skip base ISA extensions */
	लागि (; i < isa_len; i++) {
		यदि (isa[i] == '_')
			रोक;
	}

	जबसम्म (i < isa_len) {
		यदि (isa[i] != '_') {
			i++;
			जारी;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		जबसम्म ((i < isa_len) && (isa[i] != '_') &&
		       (j < (परिमाण(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		यदि (!j)
			जारी;

#define set_multi_letter_ext(name, bit)				\
			यदि (!म_भेद(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				जारी;			\
			}

		लागि (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	फर्काऊ 0;
}

स्थिर शून्य fdt_parse_isa_extensions_one_hart(अचर वर्ण *isa,
					      अचिन्हित दीर्घ *extensions,
					      पूर्णाङ्क len)
{
	माप_प्रकार i;

	लागि (i = 0; i < SBI_HART_EXT_MAX; i++) {
		यदि (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

पूर्णाङ्क fdt_parse_isa_extensions_all_harts(अचर शून्य *fdt)
{
	u32 hartid;
	अचर fdt32_t *val;
	संरचना sbi_scratch *scratch;
	संरचना sbi_hart_features *hfeatures;
	पूर्णाङ्क err, cpu_offset, cpus_offset, len;

	यदि (!fdt)
		फर्काऊ SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	यदि (cpus_offset < 0)
		फर्काऊ cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (err)
			जारी;

		यदि (!fdt_गाँठो_is_enabled(fdt, cpu_offset))
			जारी;

		scratch = sbi_hartid_to_scratch(hartid);
		यदि (!scratch)
			फर्काऊ SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		यदि (!hfeatures)
			फर्काऊ SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		यदि (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((अचर वर्ण *)val,
							  hfeatures->extensions, len);
			जारी;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		यदि (!val || len <= 0)
			फर्काऊ SBI_ENOENT;

		err = fdt_parse_isa_one_hart((अचर वर्ण *)val, hfeatures->extensions);
		यदि (err)
			फर्काऊ err;
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क fdt_parse_uart_गाँठो_common(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				      संरचना platform_uart_data *uart,
				      अचिन्हित दीर्घ default_freq,
				      अचिन्हित दीर्घ default_baud)
{
	पूर्णाङ्क len, rc;
	अचर fdt32_t *val;
	uint64_t reg_addr, reg_size;

	यदि (nodeoffset < 0 || !uart || !fdt)
		फर्काऊ SBI_ENODEV;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		फर्काऊ SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	यदि (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	अन्यथा
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	यदि (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	अन्यथा
		uart->baud = default_baud;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_gaisler_uart_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_data *uart)
{
	फर्काऊ fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

पूर्णाङ्क fdt_parse_renesas_scif_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_data *uart)
{
	फर्काऊ fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

पूर्णाङ्क fdt_parse_shakti_uart_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	फर्काऊ fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

पूर्णाङ्क fdt_parse_sifive_uart_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	फर्काऊ fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

पूर्णाङ्क fdt_parse_uart_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			संरचना platform_uart_data *uart)
{
	पूर्णाङ्क len, rc;
	अचर fdt32_t *val;

	rc = fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	यदि (rc)
		फर्काऊ rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	यदि (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	यदि (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	यदि (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_uart8250(अचर शून्य *fdt, संरचना platform_uart_data *uart,
		   अचर वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset;

	यदि (!compatible || !uart || !fdt)
		फर्काऊ SBI_ENODEV;

	nodeoffset = fdt_गाँठो_offset_by_compatible(fdt, -1, compatible);
	यदि (nodeoffset < 0)
		फर्काऊ nodeoffset;

	फर्काऊ fdt_parse_uart_गाँठो(fdt, nodeoffset, uart);
}

पूर्णाङ्क fdt_parse_xlnx_uartlite_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_data *uart)
{
	फर्काऊ fdt_parse_uart_गाँठो_common(fdt, nodeoffset, uart, 0, 0);
}

स्थिर पूर्णाङ्क fdt_aplic_find_imsic_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoff,
				     संरचना imsic_data *imsic, bool mmode)
{
	अचर fdt32_t *val;
	पूर्णाङ्क i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	यदि (val && len >= परिमाण(fdt32_t)) {
		noff = fdt_गाँठो_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		यदि (noff < 0)
			फर्काऊ noff;

		rc = fdt_parse_imsic_गाँठो(fdt, noff, imsic);
		यदि (rc)
			फर्काऊ rc;

		rc = imsic_data_check(imsic);
		यदि (rc)
			फर्काऊ rc;

		यदि (imsic->targets_mmode == mmode) {
			फर्काऊ 0;
		}
	} अन्यथा {
		फर्काऊ SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		फर्काऊ SBI_ENODEV;

	len /= परिमाण(fdt32_t);

	लागि (i = 0; i < len; i++) {
		noff = fdt_गाँठो_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		यदि (noff < 0)
			फर्काऊ noff;

		rc = fdt_aplic_find_imsic_गाँठो(fdt, noff, imsic, mmode);
		यदि (!rc)
			रोक;
	}

	फर्काऊ rc;
}

पूर्णाङ्क fdt_parse_aplic_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoff, संरचना aplic_data *aplic)
{
	bool child_found;
	अचर fdt32_t *val;
	अचर fdt32_t *del;
	संरचना imsic_data imsic = { 0 };
	पूर्णाङ्क i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	संरचना aplic_delegate_data *deleg;

	यदि (nodeoff < 0 || !aplic || !fdt)
		फर्काऊ SBI_ENODEV;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		फर्काऊ SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	यदि (len > 0)
		aplic->अङ्क_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	यदि (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		लागि (i = 0; i < len; i += 2) {
			यदि (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				रोक;
			}
		}
		aplic->अङ्क_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_गाँठो(fdt, nodeoff, &imsic, true);
	यदि (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		यदि (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			फर्काऊ SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_गाँठो(fdt, nodeoff, &imsic, false);
	यदि (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		यदि (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			फर्काऊ SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	लागि (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	यदि (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	यदि (!del || len < (3 * परिमाण(fdt32_t)))
		गच्छ skip_delegate_parse;
	d = 0;
	dcnt = len / परिमाण(fdt32_t);
	लागि (i = 0; i < dcnt; i += 3) {
		यदि (d >= APLIC_MAX_DELEGATE)
			रोक;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		यदि (!val || len < परिमाण(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			जारी;
		}
		len = len / परिमाण(fdt32_t);
		लागि (j = 0; j < len; j++) {
			यदि (del[i] != val[j])
				जारी;
			deleg->child_index = j;
			child_found = true;
			रोक;
		}

		यदि (child_found) {
			d++;
		} अन्यथा {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	फर्काऊ 0;
}

bool fdt_check_imsic_mlevel(अचर शून्य *fdt)
{
	अचर fdt32_t *val;
	पूर्णाङ्क i, len, noff = 0;

	यदि (!fdt)
		फर्काऊ false;

	जबसम्म ((noff = fdt_गाँठो_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		यदि (val && len > परिमाण(fdt32_t)) {
			len = len / परिमाण(fdt32_t);
			लागि (i = 0; i < len; i += 2) {
				यदि (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					फर्काऊ true;
			}
		}
	}

	फर्काऊ false;
}

पूर्णाङ्क fdt_parse_imsic_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoff, संरचना imsic_data *imsic)
{
	अचर fdt32_t *val;
	संरचना imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	पूर्णाङ्क i, rc, len, nr_parent_irqs;

	यदि (nodeoff < 0 || !imsic || !fdt)
		फर्काऊ SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	यदि (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		nr_parent_irqs = len / 2;
		लागि (i = 0; i < len; i += 2) {
			यदि (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				रोक;
			}
		}
	} अन्यथा
		फर्काऊ SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	यदि (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	यदि (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} अन्यथा {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		यदि ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	यदि (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	यदि (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	यदि (val && len > 0)
		imsic->अङ्क_ids = fdt32_to_cpu(*val);
	अन्यथा
		फर्काऊ SBI_EINVAL;

	लागि (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	लागि (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_गाँठो_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		यदि (rc < 0 || !reg_size)
			रोक;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	यदि (!imsic->regs[0].size)
		फर्काऊ SBI_EINVAL;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_plic_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, संरचना plic_data *plic)
{
	पूर्णाङ्क len, rc;
	अचर fdt32_t *val;
	uint64_t reg_addr, reg_size;

	यदि (nodeoffset < 0 || !plic || !fdt)
		फर्काऊ SBI_ENODEV;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		फर्काऊ SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	यदि (len > 0)
		plic->अङ्क_src = fdt32_to_cpu(*val);

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_plic(अचर शून्य *fdt, संरचना plic_data *plic, अचर वर्ण *compat)
{
	पूर्णाङ्क nodeoffset;

	यदि (!compat || !plic || !fdt)
		फर्काऊ SBI_ENODEV;

	nodeoffset = fdt_गाँठो_offset_by_compatible(fdt, -1, compat);
	यदि (nodeoffset < 0)
		फर्काऊ nodeoffset;

	फर्काऊ fdt_parse_plic_गाँठो(fdt, nodeoffset, plic);
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_size_by_name(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
					    अचिन्हित दीर्घ *out_addr1,
					    अचिन्हित दीर्घ *out_size1,
					    अचिन्हित दीर्घ *out_addr2,
					    अचिन्हित दीर्घ *out_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_गाँठो_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_गाँठो_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		फर्काऊ SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_size(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
				    अचिन्हित दीर्घ *out_addr1,
				    अचिन्हित दीर्घ *out_size1,
				    अचिन्हित दीर्घ *out_addr2,
				    अचिन्हित दीर्घ *out_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		फर्काऊ SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	यदि (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	यदि (out_addr2)
		*out_addr2 = reg_addr;
	यदि (out_size2)
		*out_size2 = reg_size;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_aclint_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset,
			  bool for_timer, bool allow_regname,
			  अचिन्हित दीर्घ *out_addr1, अचिन्हित दीर्घ *out_size1,
			  अचिन्हित दीर्घ *out_addr2, अचिन्हित दीर्घ *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_गन्ती)
{
	अचर fdt32_t *val;
	पूर्णाङ्क i, rc, गन्ती, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	यदि (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_गन्ती)
		फर्काऊ SBI_EINVAL;

	यदि (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", रिक्त))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	अन्यथा
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	यदि (rc)
		फर्काऊ rc;

	*out_first_hartid = 0;
	*out_hart_गन्ती = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गन्ती);
	यदि (!val || गन्ती < परिमाण(fdt32_t))
		फर्काऊ 0;
	गन्ती = गन्ती / परिमाण(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	लागि (i = 0; i < (गन्ती / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_गाँठो_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		यदि (rc)
			जारी;

		यदि (match_hwirq == hwirq) {
			यदि (hartid < first_hartid)
				first_hartid = hartid;
			यदि (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	यदि ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_गन्ती = last_hartid - first_hartid + 1;
	}
	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_plmt_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, अचिन्हित दीर्घ *plmt_base,
			  अचिन्हित दीर्घ *plmt_size, u32 *hart_गन्ती)
{
	अचर fdt32_t *val;
	पूर्णाङ्क rc, i, गन्ती;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	यदि (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_गन्ती || !plmt_size)
		फर्काऊ SBI_EINVAL;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	यदि (rc < 0)
		फर्काऊ SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गन्ती);
	यदि (!val || गन्ती < परिमाण(fdt32_t))
		फर्काऊ 0;
	गन्ती = गन्ती / परिमाण(fdt32_t);

	hcount = 0;
	लागि (i = 0; i < (गन्ती / 2); i++) {
		पूर्णाङ्क cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाँठो_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		यदि (rc)
			जारी;

		यदि (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			जारी;

		यदि (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_गन्ती = hcount;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_plicsw_गाँठो(अचर शून्य *fdt, पूर्णाङ्क nodeoffset, अचिन्हित दीर्घ *plicsw_base,
			  अचिन्हित दीर्घ *size, u32 *hart_गन्ती)
{
	अचर fdt32_t *val;
	पूर्णाङ्क rc, i, गन्ती;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	यदि (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_गन्ती || !size)
		फर्काऊ SBI_EINVAL;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	यदि (rc < 0)
		फर्काऊ SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गन्ती);
	यदि (!val || गन्ती < परिमाण(fdt32_t))
		फर्काऊ 0;
	गन्ती = गन्ती / परिमाण(fdt32_t);

	hcount = 0;
	लागि (i = 0; i < (गन्ती / 2); i++) {
		पूर्णाङ्क cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाँठो_offset_by_phandle(fdt, phandle);
		यदि (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		यदि (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		यदि (rc)
			जारी;

		यदि (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			जारी;

		यदि (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_गन्ती = hcount;

	फर्काऊ 0;
}

पूर्णाङ्क fdt_parse_compat_addr(अचर शून्य *fdt, uint64_t *addr,
			  अचर वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset, rc;

	nodeoffset = fdt_गाँठो_offset_by_compatible(fdt, -1, compatible);
	यदि (nodeoffset < 0)
		फर्काऊ nodeoffset;

	rc = fdt_get_गाँठो_addr_size(fdt, nodeoffset, 0, addr, रिक्त);
	यदि (rc < 0 || !addr)
		फर्काऊ SBI_ENODEV;

	फर्काऊ 0;
}
