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

पूर्णांक fdt_parse_phandle_with_args(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				स्थिरांक अक्षर *prop, स्थिरांक अक्षर *cells_prop,
				पूर्णांक index, संरचना fdt_phandle_args *out_args)
{
	u32 i, pcells;
	पूर्णांक len, pnodeoff;
	स्थिरांक fdt32_t *list, *list_end, *val;

	जर (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		परत SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	जर (!list)
		परत SBI_ENOENT;
	list_end = list + (len / आकार(*list));

	पर्यंत (list < list_end) {
		pnodeoff = fdt_गाठ_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		जर (pnodeoff < 0)
			परत pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		जर (!val)
			परत SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		जर (FDT_MAX_PHANDLE_ARGS < pcells)
			परत SBI_EINVAL;
		जर (list + pcells > list_end)
			परत SBI_ENOENT;

		जर (index > 0) {
			list += pcells;
			index--;
		} अन्यथा {
			out_args->गाठ_offset = pnodeoff;
			out_args->args_मोजणी = pcells;
			साठी (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			परत 0;
		}
	}

	परत SBI_ENOENT;
}

स्थिर पूर्णांक fdt_translate_address(स्थिरांक शून्य *fdt, uint64_t reg, पूर्णांक parent,
				 uint64_t *addr)
{
	पूर्णांक i, rlen;
	पूर्णांक cell_parent_addr, cell_child_addr, cell_size;
	स्थिरांक fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	जर (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		जर (cell_child_addr < 1)
			परत SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		जर (cell_parent_addr < 1)
			परत SBI_ENODEV;

		cell_size = fdt_size_cells(fdt, parent);
		जर (cell_size < 0)
			परत SBI_ENODEV;

		साठी (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		साठी (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		साठी (i = 0; i < cell_size; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		जर (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			परत SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} अन्यथा {
		/* No translation required */
		*addr = reg;
	}

	परत 0;
}

पूर्णांक fdt_get_गाठ_addr_size(स्थिरांक शून्य *fdt, पूर्णांक गाठ, पूर्णांक index,
			   uint64_t *addr, uint64_t *size)
{
	पूर्णांक parent, len, i, rc;
	पूर्णांक cell_addr, cell_size;
	स्थिरांक fdt32_t *prop_addr, *prop_size;
	uint64_t temp = 0;

	जर (!fdt || गाठ < 0 || index < 0)
		परत SBI_EINVAL;

	parent = fdt_parent_offset(fdt, गाठ);
	जर (parent < 0)
		परत parent;
	cell_addr = fdt_address_cells(fdt, parent);
	जर (cell_addr < 1)
		परत SBI_ENODEV;

	cell_size = fdt_size_cells(fdt, parent);
	जर (cell_size < 0)
		परत SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, गाठ, "reg", &len);
	जर (!prop_addr)
		परत SBI_ENODEV;

	जर ((len / आकार(u32)) <= (index * (cell_addr + cell_size)))
		परत SBI_EINVAL;

	prop_addr = prop_addr + (index * (cell_addr + cell_size));
	prop_size = prop_addr + cell_addr;

	जर (addr) {
		साठी (i = 0; i < cell_addr; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
		करा {
			जर (parent < 0)
				थांबा;
			rc  = fdt_translate_address(fdt, temp, parent, addr);
			जर (rc)
				थांबा;
			parent = fdt_parent_offset(fdt, parent);
			temp = *addr;
		} पर्यंत (1);
	}
	temp = 0;

	जर (size) {
		साठी (i = 0; i < cell_size; i++)
			temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
		*size = temp;
	}

	परत 0;
}

पूर्णांक fdt_get_गाठ_addr_size_by_name(स्थिरांक शून्य *fdt, पूर्णांक गाठ, स्थिरांक अक्षर *name,
				   uint64_t *addr, uint64_t *size)
{
	पूर्णांक i, j, मोजणी;
	स्थिरांक अक्षर *val;
	स्थिरांक अक्षर *regname;

	जर (!fdt || गाठ < 0 || !name)
		परत SBI_EINVAL;

	val = fdt_getprop(fdt, गाठ, "reg-names", &मोजणी);
	जर (!val)
		परत SBI_ENODEV;

	साठी (i = 0, j = 0; i < मोजणी; i++, j++) {
		regname = val + i;

		जर (म_भेद(name, regname) == 0)
			परत fdt_get_गाठ_addr_size(fdt, गाठ, j, addr, size);

		i += म_माप(regname);
	}

	परत SBI_ENODEV;
}

bool fdt_गाठ_is_enabled(स्थिरांक शून्य *fdt, पूर्णांक nodeoff)
{
	पूर्णांक len;
	स्थिरांक शून्य *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	जर (!prop)
		परत true;

	जर (!म_भेदन(prop, "okay", म_माप("okay")))
		परत true;

	जर (!म_भेदन(prop, "ok", म_माप("ok")))
		परत true;

	परत false;
}

पूर्णांक fdt_parse_hart_id(स्थिरांक शून्य *fdt, पूर्णांक cpu_offset, u32 *hartid)
{
	पूर्णांक len;
	स्थिरांक शून्य *prop;
	स्थिरांक fdt32_t *val;

	जर (!fdt || cpu_offset < 0)
		परत SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	जर (!prop || !len)
		परत SBI_EINVAL;
	जर (म_भेदन (prop, "cpu", म_माप ("cpu")))
		परत SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	जर (!val || len < आकार(fdt32_t))
		परत SBI_EINVAL;

	जर (len > आकार(fdt32_t))
		val++;

	जर (hartid)
		*hartid = fdt32_to_cpu(*val);

	परत 0;
}

पूर्णांक fdt_parse_cbom_block_size(स्थिरांक शून्य *fdt, पूर्णांक cpu_offset, अचिन्हित दीर्घ *cbom_block_size)
{
	पूर्णांक len;
	स्थिरांक शून्य *prop;
	स्थिरांक fdt32_t *val;

	जर (!fdt || cpu_offset < 0)
		परत SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	जर (!prop || !len)
		परत SBI_EINVAL;
	जर (म_भेदन (prop, "cpu", म_माप ("cpu")))
		परत SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	जर (!val || len < आकार(fdt32_t))
		परत SBI_EINVAL;

	जर (cbom_block_size)
		*cbom_block_size = fdt32_to_cpu(*val);
	परत 0;
}

पूर्णांक fdt_parse_max_enabled_hart_id(स्थिरांक शून्य *fdt, u32 *max_hartid)
{
	u32 hartid;
	पूर्णांक err, cpu_offset, cpus_offset;

	जर (!fdt)
		परत SBI_EINVAL;
	जर (!max_hartid)
		परत 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जर (err)
			सुरूठेवा;

		जर (!fdt_गाठ_is_enabled(fdt, cpu_offset))
			सुरूठेवा;

		जर (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	परत 0;
}

पूर्णांक fdt_parse_timebase_frequency(स्थिरांक शून्य *fdt, अचिन्हित दीर्घ *freq)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक len, cpus_offset;

	जर (!fdt || !freq)
		परत SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	जर (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	अन्यथा
		परत SBI_ENOENT;

	परत 0;
}

#define RISCV_ISA_EXT_NAME_LEN_MAX	32

स्थिर पूर्णांक fdt_parse_isa_one_hart(स्थिरांक अक्षर *isa, अचिन्हित दीर्घ *extensions)
{
	माप_प्रकार i, j, isa_len;
	अक्षर mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = म_माप(isa);

	जर (isa[i] == 'r' || isa[i] == 'R')
		i++;
	अन्यथा
		परत SBI_EINVAL;

	जर (isa[i] == 'v' || isa[i] == 'V')
		i++;
	अन्यथा
		परत SBI_EINVAL;

	जर (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	अन्यथा जर (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	अन्यथा
		परत SBI_EINVAL;

	/* Skip base ISA extensions */
	साठी (; i < isa_len; i++) {
		जर (isa[i] == '_')
			थांबा;
	}

	पर्यंत (i < isa_len) {
		जर (isa[i] != '_') {
			i++;
			सुरूठेवा;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		पर्यंत ((i < isa_len) && (isa[i] != '_') &&
		       (j < (आकार(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		जर (!j)
			सुरूठेवा;

#define set_multi_letter_ext(name, bit)				\
			जर (!म_भेद(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				सुरूठेवा;			\
			}

		साठी (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#undef set_multi_letter_ext
	}

	परत 0;
}

स्थिर शून्य fdt_parse_isa_extensions_one_hart(स्थिरांक अक्षर *isa,
					      अचिन्हित दीर्घ *extensions,
					      पूर्णांक len)
{
	माप_प्रकार i;

	साठी (i = 0; i < SBI_HART_EXT_MAX; i++) {
		जर (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

पूर्णांक fdt_parse_isa_extensions_all_harts(स्थिरांक शून्य *fdt)
{
	u32 hartid;
	स्थिरांक fdt32_t *val;
	संरचना sbi_scratch *scratch;
	संरचना sbi_hart_features *hfeatures;
	पूर्णांक err, cpu_offset, cpus_offset, len;

	जर (!fdt)
		परत SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	जर (cpus_offset < 0)
		परत cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जर (err)
			सुरूठेवा;

		जर (!fdt_गाठ_is_enabled(fdt, cpu_offset))
			सुरूठेवा;

		scratch = sbi_hartid_to_scratch(hartid);
		जर (!scratch)
			परत SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		जर (!hfeatures)
			परत SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		जर (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((स्थिरांक अक्षर *)val,
							  hfeatures->extensions, len);
			सुरूठेवा;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		जर (!val || len <= 0)
			परत SBI_ENOENT;

		err = fdt_parse_isa_one_hart((स्थिरांक अक्षर *)val, hfeatures->extensions);
		जर (err)
			परत err;
	}

	परत 0;
}

स्थिर पूर्णांक fdt_parse_uart_गाठ_common(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				      संरचना platform_uart_data *uart,
				      अचिन्हित दीर्घ default_freq,
				      अचिन्हित दीर्घ default_baud)
{
	पूर्णांक len, rc;
	स्थिरांक fdt32_t *val;
	uint64_t reg_addr, reg_size;

	जर (nodeoffset < 0 || !uart || !fdt)
		परत SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		परत SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	जर (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	अन्यथा
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	जर (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	अन्यथा
		uart->baud = default_baud;

	परत 0;
}

पूर्णांक fdt_parse_gaisler_uart_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				संरचना platform_uart_data *uart)
{
	परत fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

पूर्णांक fdt_parse_renesas_scif_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				संरचना platform_uart_data *uart)
{
	परत fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

पूर्णांक fdt_parse_shakti_uart_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			       संरचना platform_uart_data *uart)
{
	परत fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

पूर्णांक fdt_parse_sifive_uart_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			       संरचना platform_uart_data *uart)
{
	परत fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

पूर्णांक fdt_parse_uart_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			संरचना platform_uart_data *uart)
{
	पूर्णांक len, rc;
	स्थिरांक fdt32_t *val;

	rc = fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	जर (rc)
		परत rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	जर (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	जर (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	जर (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	परत 0;
}

पूर्णांक fdt_parse_uart8250(स्थिरांक शून्य *fdt, संरचना platform_uart_data *uart,
		   स्थिरांक अक्षर *compatible)
{
	पूर्णांक nodeoffset;

	जर (!compatible || !uart || !fdt)
		परत SBI_ENODEV;

	nodeoffset = fdt_गाठ_offset_by_compatible(fdt, -1, compatible);
	जर (nodeoffset < 0)
		परत nodeoffset;

	परत fdt_parse_uart_गाठ(fdt, nodeoffset, uart);
}

पूर्णांक fdt_parse_xlnx_uartlite_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			       संरचना platform_uart_data *uart)
{
	परत fdt_parse_uart_गाठ_common(fdt, nodeoffset, uart, 0, 0);
}

स्थिर पूर्णांक fdt_aplic_find_imsic_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
				     संरचना imsic_data *imsic, bool mmode)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	जर (val && len >= आकार(fdt32_t)) {
		noff = fdt_गाठ_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		जर (noff < 0)
			परत noff;

		rc = fdt_parse_imsic_गाठ(fdt, noff, imsic);
		जर (rc)
			परत rc;

		rc = imsic_data_check(imsic);
		जर (rc)
			परत rc;

		जर (imsic->targets_mmode == mmode) {
			परत 0;
		}
	} अन्यथा {
		परत SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	जर (!val || len < आकार(fdt32_t))
		परत SBI_ENODEV;

	len /= आकार(fdt32_t);

	साठी (i = 0; i < len; i++) {
		noff = fdt_गाठ_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		जर (noff < 0)
			परत noff;

		rc = fdt_aplic_find_imsic_गाठ(fdt, noff, imsic, mmode);
		जर (!rc)
			थांबा;
	}

	परत rc;
}

पूर्णांक fdt_parse_aplic_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, संरचना aplic_data *aplic)
{
	bool child_found;
	स्थिरांक fdt32_t *val;
	स्थिरांक fdt32_t *del;
	संरचना imsic_data imsic = { 0 };
	पूर्णांक i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_size;
	संरचना aplic_delegate_data *deleg;

	जर (nodeoff < 0 || !aplic || !fdt)
		परत SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		परत SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->size = reg_size;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	जर (len > 0)
		aplic->अंक_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	जर (val && len > आकार(fdt32_t)) {
		len = len / आकार(fdt32_t);
		साठी (i = 0; i < len; i += 2) {
			जर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				थांबा;
			}
		}
		aplic->अंक_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_गाठ(fdt, nodeoff, &imsic, true);
	जर (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_index_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_index_shift;
		जर (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			परत SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_गाठ(fdt, nodeoff, &imsic, false);
	जर (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_index_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_index_bits;
		aplic->msicfg_smode.hhxw = imsic.group_index_bits;
		aplic->msicfg_smode.hhxs = imsic.group_index_shift;
		जर (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			परत SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	साठी (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_index = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	जर (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	जर (!del || len < (3 * आकार(fdt32_t)))
		जा skip_delegate_parse;
	d = 0;
	dcnt = len / आकार(fdt32_t);
	साठी (i = 0; i < dcnt; i += 3) {
		जर (d >= APLIC_MAX_DELEGATE)
			थांबा;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_index = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		जर (!val || len < आकार(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
			सुरूठेवा;
		}
		len = len / आकार(fdt32_t);
		साठी (j = 0; j < len; j++) {
			जर (del[i] != val[j])
				सुरूठेवा;
			deleg->child_index = j;
			child_found = true;
			थांबा;
		}

		जर (child_found) {
			d++;
		} अन्यथा {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_index = 0;
		}
	}
skip_delegate_parse:

	परत 0;
}

bool fdt_check_imsic_mlevel(स्थिरांक शून्य *fdt)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक i, len, noff = 0;

	जर (!fdt)
		परत false;

	पर्यंत ((noff = fdt_गाठ_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		जर (val && len > आकार(fdt32_t)) {
			len = len / आकार(fdt32_t);
			साठी (i = 0; i < len; i += 2) {
				जर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					परत true;
			}
		}
	}

	परत false;
}

पूर्णांक fdt_parse_imsic_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoff, संरचना imsic_data *imsic)
{
	स्थिरांक fdt32_t *val;
	संरचना imsic_regs *regs;
	uint64_t reg_addr, reg_size;
	पूर्णांक i, rc, len, nr_parent_irqs;

	जर (nodeoff < 0 || !imsic || !fdt)
		परत SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	जर (val && len > आकार(fdt32_t)) {
		len = len / आकार(fdt32_t);
		nr_parent_irqs = len / 2;
		साठी (i = 0; i < len; i += 2) {
			जर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				थांबा;
			}
		}
	} अन्यथा
		परत SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	जर (val && len > 0)
		imsic->guest_index_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->guest_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	जर (val && len > 0) {
		imsic->hart_index_bits = fdt32_to_cpu(*val);
	} अन्यथा {
		imsic->hart_index_bits = sbi_fls(nr_parent_irqs);
		जर ((1UL << imsic->hart_index_bits) < nr_parent_irqs)
			imsic->hart_index_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	जर (val && len > 0)
		imsic->group_index_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_index_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	जर (val && len > 0)
		imsic->group_index_shift = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_index_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	जर (val && len > 0)
		imsic->अंक_ids = fdt32_to_cpu(*val);
	अन्यथा
		परत SBI_EINVAL;

	साठी (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->size = 0;
	}

	साठी (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_गाठ_addr_size(fdt, nodeoff, i,
					    &reg_addr, &reg_size);
		जर (rc < 0 || !reg_size)
			थांबा;
		regs->addr = reg_addr;
		regs->size = reg_size;
	}
	जर (!imsic->regs[0].size)
		परत SBI_EINVAL;

	परत 0;
}

पूर्णांक fdt_parse_plic_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, संरचना plic_data *plic)
{
	पूर्णांक len, rc;
	स्थिरांक fdt32_t *val;
	uint64_t reg_addr, reg_size;

	जर (nodeoffset < 0 || !plic || !fdt)
		परत SBI_ENODEV;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		परत SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	जर (len > 0)
		plic->अंक_src = fdt32_to_cpu(*val);

	परत 0;
}

पूर्णांक fdt_parse_plic(स्थिरांक शून्य *fdt, संरचना plic_data *plic, स्थिरांक अक्षर *compat)
{
	पूर्णांक nodeoffset;

	जर (!compat || !plic || !fdt)
		परत SBI_ENODEV;

	nodeoffset = fdt_गाठ_offset_by_compatible(fdt, -1, compat);
	जर (nodeoffset < 0)
		परत nodeoffset;

	परत fdt_parse_plic_गाठ(fdt, nodeoffset, plic);
}

स्थिर पूर्णांक fdt_get_aclint_addr_size_by_name(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
					    अचिन्हित दीर्घ *out_addr1,
					    अचिन्हित दीर्घ *out_size1,
					    अचिन्हित दीर्घ *out_addr2,
					    अचिन्हित दीर्घ *out_size2)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_गाठ_addr_size_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_गाठ_addr_size_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		परत SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_size;

	परत 0;
}

स्थिर पूर्णांक fdt_get_aclint_addr_size(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
				    अचिन्हित दीर्घ *out_addr1,
				    अचिन्हित दीर्घ *out_size1,
				    अचिन्हित दीर्घ *out_addr2,
				    अचिन्हित दीर्घ *out_size2)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_size;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		परत SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_size;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 1,
				    &reg_addr, &reg_size);
	जर (rc < 0 || !reg_size)
		reg_addr = reg_size = 0;
	जर (out_addr2)
		*out_addr2 = reg_addr;
	जर (out_size2)
		*out_size2 = reg_size;

	परत 0;
}

पूर्णांक fdt_parse_aclint_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset,
			  bool for_timer, bool allow_regname,
			  अचिन्हित दीर्घ *out_addr1, अचिन्हित दीर्घ *out_size1,
			  अचिन्हित दीर्घ *out_addr2, अचिन्हित दीर्घ *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_मोजणी)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक i, rc, मोजणी, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	जर (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_मोजणी)
		परत SBI_EINVAL;

	जर (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", रिक्त))
		rc = fdt_get_aclint_addr_size_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	अन्यथा
		rc = fdt_get_aclint_addr_size(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	जर (rc)
		परत rc;

	*out_first_hartid = 0;
	*out_hart_मोजणी = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &मोजणी);
	जर (!val || मोजणी < आकार(fdt32_t))
		परत 0;
	मोजणी = मोजणी / आकार(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	साठी (i = 0; i < (मोजणी / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_गाठ_offset_by_phandle(fdt, phandle);
		जर (cpu_intc_offset < 0)
			सुरूठेवा;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		जर (cpu_offset < 0)
			सुरूठेवा;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		जर (rc)
			सुरूठेवा;

		जर (match_hwirq == hwirq) {
			जर (hartid < first_hartid)
				first_hartid = hartid;
			जर (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	जर ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_मोजणी = last_hartid - first_hartid + 1;
	}
	परत 0;
}

पूर्णांक fdt_parse_plmt_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, अचिन्हित दीर्घ *plmt_base,
			  अचिन्हित दीर्घ *plmt_size, u32 *hart_मोजणी)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक rc, i, मोजणी;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	जर (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_मोजणी || !plmt_size)
		परत SBI_EINVAL;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	जर (rc < 0)
		परत SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &मोजणी);
	जर (!val || मोजणी < आकार(fdt32_t))
		परत 0;
	मोजणी = मोजणी / आकार(fdt32_t);

	hcount = 0;
	साठी (i = 0; i < (मोजणी / 2); i++) {
		पूर्णांक cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाठ_offset_by_phandle(fdt, phandle);
		जर (cpu_intc_offset < 0)
			सुरूठेवा;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		जर (cpu_offset < 0)
			सुरूठेवा;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		जर (rc)
			सुरूठेवा;

		जर (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			सुरूठेवा;

		जर (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_मोजणी = hcount;

	परत 0;
}

पूर्णांक fdt_parse_plicsw_गाठ(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset, अचिन्हित दीर्घ *plicsw_base,
			  अचिन्हित दीर्घ *size, u32 *hart_मोजणी)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक rc, i, मोजणी;
	uint64_t reg_addr, reg_size;
	u32 phandle, hwirq, hartid, hcount;

	जर (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_मोजणी || !size)
		परत SBI_EINVAL;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0,
				    &reg_addr, &reg_size);
	जर (rc < 0)
		परत SBI_ENODEV;
	*plicsw_base = reg_addr;
	*size = reg_size;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &मोजणी);
	जर (!val || मोजणी < आकार(fdt32_t))
		परत 0;
	मोजणी = मोजणी / आकार(fdt32_t);

	hcount = 0;
	साठी (i = 0; i < (मोजणी / 2); i++) {
		पूर्णांक cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाठ_offset_by_phandle(fdt, phandle);
		जर (cpu_intc_offset < 0)
			सुरूठेवा;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		जर (cpu_offset < 0)
			सुरूठेवा;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		जर (rc)
			सुरूठेवा;

		जर (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			सुरूठेवा;

		जर (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_मोजणी = hcount;

	परत 0;
}

पूर्णांक fdt_parse_compat_addr(स्थिरांक शून्य *fdt, uint64_t *addr,
			  स्थिरांक अक्षर *compatible)
{
	पूर्णांक nodeoffset, rc;

	nodeoffset = fdt_गाठ_offset_by_compatible(fdt, -1, compatible);
	जर (nodeoffset < 0)
		परत nodeoffset;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoffset, 0, addr, रिक्त);
	जर (rc < 0 || !addr)
		परत SBI_ENODEV;

	परत 0;
}
