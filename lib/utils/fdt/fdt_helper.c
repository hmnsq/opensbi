// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_hartmask.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/irqchip/aplic.h>
#समावेश <sbi_utils/irqchip/imsic.h>
#समावेश <sbi_utils/irqchip/plic.h>

#घोषणा DEFAULT_UART_FREQ		0
#घोषणा DEFAULT_UART_BAUD		115200
#घोषणा DEFAULT_UART_REG_SHIFT		0
#घोषणा DEFAULT_UART_REG_IO_WIDTH	1
#घोषणा DEFAULT_UART_REG_OFFSET		0

#घोषणा DEFAULT_RENESAS_SCIF_FREQ		100000000
#घोषणा DEFAULT_RENESAS_SCIF_BAUD		115200

#घोषणा DEFAULT_SIFIVE_UART_FREQ		0
#घोषणा DEFAULT_SIFIVE_UART_BAUD		115200

#घोषणा DEFAULT_SHAKTI_UART_FREQ		50000000
#घोषणा DEFAULT_SHAKTI_UART_BAUD		115200

पूर्णांक fdt_parse_phandle_with_args(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				स्थिर अक्षर *prop, स्थिर अक्षर *cells_prop,
				पूर्णांक सूचकांक_2, काष्ठा fdt_phandle_args *out_args)
{
	u32 i, pcells;
	पूर्णांक len, pnodeoff;
	स्थिर fdt32_t *list, *list_end, *val;

	अगर (!fdt || (nodeoff < 0) || !prop || !cells_prop || !out_args)
		वापस SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	अगर (!list)
		वापस SBI_ENOENT;
	list_end = list + (len / माप(*list));

	जबतक (list < list_end) {
		pnodeoff = fdt_गाँठ_offset_by_phandle(fdt,
						fdt32_to_cpu(*list));
		अगर (pnodeoff < 0)
			वापस pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		अगर (!val)
			वापस SBI_ENOENT;
		pcells = fdt32_to_cpu(*val);
		अगर (FDT_MAX_PHANDLE_ARGS < pcells)
			वापस SBI_EINVAL;
		अगर (list + pcells > list_end)
			वापस SBI_ENOENT;

		अगर (सूचकांक_2 > 0) {
			list += pcells;
			सूचकांक_2--;
		} अन्यथा {
			out_args->गाँठ_offset = pnodeoff;
			out_args->args_गिनती = pcells;
			क्रम (i = 0; i < pcells; i++)
				out_args->args[i] = fdt32_to_cpu(list[i]);
			वापस 0;
		}
	}

	वापस SBI_ENOENT;
}

अटल पूर्णांक fdt_translate_address(स्थिर व्योम *fdt, uint64_t reg, पूर्णांक parent,
				 uint64_t *addr)
{
	पूर्णांक i, rlen;
	पूर्णांक cell_parent_addr, cell_child_addr, cell_आकार;
	स्थिर fdt32_t *ranges;
	uint64_t offset, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	अगर (ranges && rlen > 0) {
		cell_child_addr = fdt_address_cells(fdt, parent);
		अगर (cell_child_addr < 1)
			वापस SBI_ENODEV;

		cell_parent_addr = fdt_address_cells(fdt, fdt_parent_offset(fdt, parent));
		अगर (cell_parent_addr < 1)
			वापस SBI_ENODEV;

		cell_आकार = fdt_आकार_cells(fdt, parent);
		अगर (cell_आकार < 0)
			वापस SBI_ENODEV;

		क्रम (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_to_cpu(*ranges++);
		क्रम (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_to_cpu(*ranges++);
		क्रम (i = 0; i < cell_आकार; i++)
			rsize = (rsize << 32) | fdt32_to_cpu(*ranges++);
		अगर (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			वापस SBI_ENODEV;
		}
		offset = reg - caddr;
		*addr = paddr + offset;
	} अन्यथा {
		/* No translation required */
		*addr = reg;
	}

	वापस 0;
}

पूर्णांक fdt_get_गाँठ_addr_आकार(स्थिर व्योम *fdt, पूर्णांक गाँठ, पूर्णांक सूचकांक_2,
			   uint64_t *addr, uint64_t *आकार)
{
	पूर्णांक parent, len, i, rc;
	पूर्णांक cell_addr, cell_आकार;
	स्थिर fdt32_t *prop_addr, *prop_आकार;
	uint64_t अस्थायी = 0;

	अगर (!fdt || गाँठ < 0 || सूचकांक_2 < 0)
		वापस SBI_EINVAL;

	parent = fdt_parent_offset(fdt, गाँठ);
	अगर (parent < 0)
		वापस parent;
	cell_addr = fdt_address_cells(fdt, parent);
	अगर (cell_addr < 1)
		वापस SBI_ENODEV;

	cell_आकार = fdt_आकार_cells(fdt, parent);
	अगर (cell_आकार < 0)
		वापस SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, गाँठ, "reg", &len);
	अगर (!prop_addr)
		वापस SBI_ENODEV;

	अगर ((len / माप(u32)) <= (सूचकांक_2 * (cell_addr + cell_आकार)))
		वापस SBI_EINVAL;

	prop_addr = prop_addr + (सूचकांक_2 * (cell_addr + cell_आकार));
	prop_आकार = prop_addr + cell_addr;

	अगर (addr) {
		क्रम (i = 0; i < cell_addr; i++)
			अस्थायी = (अस्थायी << 32) | fdt32_to_cpu(*prop_addr++);
		करो {
			अगर (parent < 0)
				अवरोध;
			rc  = fdt_translate_address(fdt, अस्थायी, parent, addr);
			अगर (rc)
				अवरोध;
			parent = fdt_parent_offset(fdt, parent);
			अस्थायी = *addr;
		} जबतक (1);
	}
	अस्थायी = 0;

	अगर (आकार) {
		क्रम (i = 0; i < cell_आकार; i++)
			अस्थायी = (अस्थायी << 32) | fdt32_to_cpu(*prop_आकार++);
		*आकार = अस्थायी;
	}

	वापस 0;
}

पूर्णांक fdt_get_गाँठ_addr_आकार_by_name(स्थिर व्योम *fdt, पूर्णांक गाँठ, स्थिर अक्षर *name,
				   uint64_t *addr, uint64_t *आकार)
{
	पूर्णांक i, j, गिनती;
	स्थिर अक्षर *val;
	स्थिर अक्षर *regname;

	अगर (!fdt || गाँठ < 0 || !name)
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, गाँठ, "reg-names", &गिनती);
	अगर (!val)
		वापस SBI_ENODEV;

	क्रम (i = 0, j = 0; i < गिनती; i++, j++) {
		regname = val + i;

		अगर (म_भेद(name, regname) == 0)
			वापस fdt_get_गाँठ_addr_आकार(fdt, गाँठ, j, addr, आकार);

		i += म_माप(regname);
	}

	वापस SBI_ENODEV;
}

bool fdt_गाँठ_is_enabled(स्थिर व्योम *fdt, पूर्णांक nodeoff)
{
	पूर्णांक len;
	स्थिर व्योम *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	अगर (!prop)
		वापस true;

	अगर (!म_भेदन(prop, "okay", म_माप("okay")))
		वापस true;

	अगर (!म_भेदन(prop, "ok", म_माप("ok")))
		वापस true;

	वापस false;
}

पूर्णांक fdt_parse_hart_id(स्थिर व्योम *fdt, पूर्णांक cpu_offset, u32 *hartid)
{
	पूर्णांक len;
	स्थिर व्योम *prop;
	स्थिर fdt32_t *val;

	अगर (!fdt || cpu_offset < 0)
		वापस SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	अगर (!prop || !len)
		वापस SBI_EINVAL;
	अगर (म_भेदन (prop, "cpu", म_माप ("cpu")))
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "reg", &len);
	अगर (!val || len < माप(fdt32_t))
		वापस SBI_EINVAL;

	अगर (len > माप(fdt32_t))
		val++;

	अगर (hartid)
		*hartid = fdt32_to_cpu(*val);

	वापस 0;
}

पूर्णांक fdt_parse_cbom_block_आकार(स्थिर व्योम *fdt, पूर्णांक cpu_offset, अचिन्हित दीर्घ *cbom_block_आकार)
{
	पूर्णांक len;
	स्थिर व्योम *prop;
	स्थिर fdt32_t *val;

	अगर (!fdt || cpu_offset < 0)
		वापस SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
	अगर (!prop || !len)
		वापस SBI_EINVAL;
	अगर (म_भेदन (prop, "cpu", म_माप ("cpu")))
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_offset, "riscv,cbom-block-size", &len);
	अगर (!val || len < माप(fdt32_t))
		वापस SBI_EINVAL;

	अगर (cbom_block_आकार)
		*cbom_block_आकार = fdt32_to_cpu(*val);
	वापस 0;
}

पूर्णांक fdt_parse_max_enabled_hart_id(स्थिर व्योम *fdt, u32 *max_hartid)
{
	u32 hartid;
	पूर्णांक err, cpu_offset, cpus_offset;

	अगर (!fdt)
		वापस SBI_EINVAL;
	अगर (!max_hartid)
		वापस 0;

	*max_hartid = 0;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		अगर (err)
			जारी;

		अगर (!fdt_गाँठ_is_enabled(fdt, cpu_offset))
			जारी;

		अगर (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	वापस 0;
}

पूर्णांक fdt_parse_timebase_frequency(स्थिर व्योम *fdt, अचिन्हित दीर्घ *freq)
{
	स्थिर fdt32_t *val;
	पूर्णांक len, cpus_offset;

	अगर (!fdt || !freq)
		वापस SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस cpus_offset;

	val = fdt_getprop(fdt, cpus_offset, "timebase-frequency", &len);
	अगर (len > 0 && val)
		*freq = fdt32_to_cpu(*val);
	अन्यथा
		वापस SBI_ENOENT;

	वापस 0;
}

#घोषणा RISCV_ISA_EXT_NAME_LEN_MAX	32

अटल पूर्णांक fdt_parse_isa_one_hart(स्थिर अक्षर *isa, अचिन्हित दीर्घ *extensions)
{
	माप_प्रकार i, j, isa_len;
	अक्षर mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = म_माप(isa);

	अगर (isa[i] == 'r' || isa[i] == 'R')
		i++;
	अन्यथा
		वापस SBI_EINVAL;

	अगर (isa[i] == 'v' || isa[i] == 'V')
		i++;
	अन्यथा
		वापस SBI_EINVAL;

	अगर (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	अन्यथा अगर (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	अन्यथा
		वापस SBI_EINVAL;

	/* Skip base ISA extensions */
	क्रम (; i < isa_len; i++) {
		अगर (isa[i] == '_')
			अवरोध;
	}

	जबतक (i < isa_len) {
		अगर (isa[i] != '_') {
			i++;
			जारी;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		जबतक ((i < isa_len) && (isa[i] != '_') &&
		       (j < (माप(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		अगर (!j)
			जारी;

#घोषणा set_multi_letter_ext(name, bit)				\
			अगर (!म_भेद(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				जारी;			\
			}

		क्रम (j = 0; j < SBI_HART_EXT_MAX; j++) {
			set_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#अघोषित set_multi_letter_ext
	}

	वापस 0;
}

अटल व्योम fdt_parse_isa_extensions_one_hart(स्थिर अक्षर *isa,
					      अचिन्हित दीर्घ *extensions,
					      पूर्णांक len)
{
	माप_प्रकार i;

	क्रम (i = 0; i < SBI_HART_EXT_MAX; i++) {
		अगर (fdt_stringlist_contains(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

पूर्णांक fdt_parse_isa_extensions_all_harts(स्थिर व्योम *fdt)
{
	u32 hartid;
	स्थिर fdt32_t *val;
	काष्ठा sbi_scratch *scratch;
	काष्ठा sbi_hart_features *hfeatures;
	पूर्णांक err, cpu_offset, cpus_offset, len;

	अगर (!fdt)
		वापस SBI_EINVAL;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	अगर (cpus_offset < 0)
		वापस cpus_offset;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		अगर (err)
			जारी;

		अगर (!fdt_गाँठ_is_enabled(fdt, cpu_offset))
			जारी;

		scratch = sbi_hartid_to_scratch(hartid);
		अगर (!scratch)
			वापस SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		अगर (!hfeatures)
			वापस SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa-extensions", &len);
		अगर (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((स्थिर अक्षर *)val,
							  hfeatures->extensions, len);
			जारी;
		}

		val = fdt_getprop(fdt, cpu_offset, "riscv,isa", &len);
		अगर (!val || len <= 0)
			वापस SBI_ENOENT;

		err = fdt_parse_isa_one_hart((स्थिर अक्षर *)val, hfeatures->extensions);
		अगर (err)
			वापस err;
	}

	वापस 0;
}

अटल पूर्णांक fdt_parse_uart_गाँठ_common(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				      काष्ठा platform_uart_तथ्य *uart,
				      अचिन्हित दीर्घ default_freq,
				      अचिन्हित दीर्घ default_baud)
{
	पूर्णांक len, rc;
	स्थिर fdt32_t *val;
	uint64_t reg_addr, reg_आकार;

	अगर (nodeoffset < 0 || !uart || !fdt)
		वापस SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		वापस SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	अगर (len > 0 && val)
		uart->freq = fdt32_to_cpu(*val);
	अन्यथा
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	अगर (len > 0 && val)
		uart->baud = fdt32_to_cpu(*val);
	अन्यथा
		uart->baud = default_baud;

	वापस 0;
}

पूर्णांक fdt_parse_gaisler_uart_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				काष्ठा platform_uart_तथ्य *uart)
{
	वापस fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

पूर्णांक fdt_parse_renesas_scif_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				काष्ठा platform_uart_तथ्य *uart)
{
	वापस fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

पूर्णांक fdt_parse_shakti_uart_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			       काष्ठा platform_uart_तथ्य *uart)
{
	वापस fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

पूर्णांक fdt_parse_sifive_uart_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			       काष्ठा platform_uart_तथ्य *uart)
{
	वापस fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

पूर्णांक fdt_parse_uart_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			काष्ठा platform_uart_तथ्य *uart)
{
	पूर्णांक len, rc;
	स्थिर fdt32_t *val;

	rc = fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	अगर (rc)
		वापस rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	अगर (len > 0 && val)
		uart->reg_shift = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_shift = DEFAULT_UART_REG_SHIFT;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	अगर (len > 0 && val)
		uart->reg_io_width = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_io_width = DEFAULT_UART_REG_IO_WIDTH;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	अगर (len > 0 && val)
		uart->reg_offset = fdt32_to_cpu(*val);
	अन्यथा
		uart->reg_offset = DEFAULT_UART_REG_OFFSET;

	वापस 0;
}

पूर्णांक fdt_parse_uart8250(स्थिर व्योम *fdt, काष्ठा platform_uart_तथ्य *uart,
		   स्थिर अक्षर *compatible)
{
	पूर्णांक nodeoffset;

	अगर (!compatible || !uart || !fdt)
		वापस SBI_ENODEV;

	nodeoffset = fdt_गाँठ_offset_by_compatible(fdt, -1, compatible);
	अगर (nodeoffset < 0)
		वापस nodeoffset;

	वापस fdt_parse_uart_गाँठ(fdt, nodeoffset, uart);
}

पूर्णांक fdt_parse_xlnx_uartlite_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			       काष्ठा platform_uart_तथ्य *uart)
{
	वापस fdt_parse_uart_गाँठ_common(fdt, nodeoffset, uart, 0, 0);
}

अटल पूर्णांक fdt_aplic_find_imsic_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				     काष्ठा imsic_तथ्य *imsic, bool mmode)
{
	स्थिर fdt32_t *val;
	पूर्णांक i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	अगर (val && len >= माप(fdt32_t)) {
		noff = fdt_गाँठ_offset_by_phandle(fdt, fdt32_to_cpu(*val));
		अगर (noff < 0)
			वापस noff;

		rc = fdt_parse_imsic_गाँठ(fdt, noff, imsic);
		अगर (rc)
			वापस rc;

		rc = imsic_तथ्य_check(imsic);
		अगर (rc)
			वापस rc;

		अगर (imsic->targets_mmode == mmode) {
			वापस 0;
		}
	} अन्यथा {
		वापस SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	अगर (!val || len < माप(fdt32_t))
		वापस SBI_ENODEV;

	len /= माप(fdt32_t);

	क्रम (i = 0; i < len; i++) {
		noff = fdt_गाँठ_offset_by_phandle(fdt, fdt32_to_cpu(val[i]));
		अगर (noff < 0)
			वापस noff;

		rc = fdt_aplic_find_imsic_गाँठ(fdt, noff, imsic, mmode);
		अगर (!rc)
			अवरोध;
	}

	वापस rc;
}

पूर्णांक fdt_parse_aplic_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoff, काष्ठा aplic_तथ्य *aplic)
{
	bool child_found;
	स्थिर fdt32_t *val;
	स्थिर fdt32_t *del;
	काष्ठा imsic_तथ्य imsic = { 0 };
	पूर्णांक i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_आकार;
	काष्ठा aplic_delegate_तथ्य *deleg;

	अगर (nodeoff < 0 || !aplic || !fdt)
		वापस SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		वापस SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	अगर (len > 0)
		aplic->अंक_source = fdt32_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	अगर (val && len > माप(fdt32_t)) {
		len = len / माप(fdt32_t);
		क्रम (i = 0; i < len; i += 2) {
			अगर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				अवरोध;
			}
		}
		aplic->अंक_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_गाँठ(fdt, nodeoff, &imsic, true);
	अगर (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_सूचकांक_bits;
		aplic->msicfg_mmode.lhxw = imsic.hart_सूचकांक_bits;
		aplic->msicfg_mmode.hhxw = imsic.group_सूचकांक_bits;
		aplic->msicfg_mmode.hhxs = imsic.group_सूचकांक_shift;
		अगर (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			वापस SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.base_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_गाँठ(fdt, nodeoff, &imsic, false);
	अगर (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_सूचकांक_bits;
		aplic->msicfg_smode.lhxw = imsic.hart_सूचकांक_bits;
		aplic->msicfg_smode.hhxw = imsic.group_सूचकांक_bits;
		aplic->msicfg_smode.hhxs = imsic.group_सूचकांक_shift;
		अगर (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_SHIFT))
			वापस SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.base_addr = imsic.regs[0].addr;
	}

	क्रम (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->last_irq = 0;
		deleg->child_सूचकांक = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	अगर (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	अगर (!del || len < (3 * माप(fdt32_t)))
		जाओ skip_delegate_parse;
	d = 0;
	dcnt = len / माप(fdt32_t);
	क्रम (i = 0; i < dcnt; i += 3) {
		अगर (d >= APLIC_MAX_DELEGATE)
			अवरोध;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_to_cpu(del[i + 1]);
		deleg->last_irq = fdt32_to_cpu(del[i + 2]);
		deleg->child_सूचकांक = 0;

		child_found = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		अगर (!val || len < माप(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_सूचकांक = 0;
			जारी;
		}
		len = len / माप(fdt32_t);
		क्रम (j = 0; j < len; j++) {
			अगर (del[i] != val[j])
				जारी;
			deleg->child_सूचकांक = j;
			child_found = true;
			अवरोध;
		}

		अगर (child_found) {
			d++;
		} अन्यथा {
			deleg->first_irq = 0;
			deleg->last_irq = 0;
			deleg->child_सूचकांक = 0;
		}
	}
skip_delegate_parse:

	वापस 0;
}

bool fdt_check_imsic_mlevel(स्थिर व्योम *fdt)
{
	स्थिर fdt32_t *val;
	पूर्णांक i, len, noff = 0;

	अगर (!fdt)
		वापस false;

	जबतक ((noff = fdt_गाँठ_offset_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		अगर (val && len > माप(fdt32_t)) {
			len = len / माप(fdt32_t);
			क्रम (i = 0; i < len; i += 2) {
				अगर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT)
					वापस true;
			}
		}
	}

	वापस false;
}

पूर्णांक fdt_parse_imsic_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoff, काष्ठा imsic_तथ्य *imsic)
{
	स्थिर fdt32_t *val;
	काष्ठा imsic_regs *regs;
	uint64_t reg_addr, reg_आकार;
	पूर्णांक i, rc, len, nr_parent_irqs;

	अगर (nodeoff < 0 || !imsic || !fdt)
		वापस SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	अगर (val && len > माप(fdt32_t)) {
		len = len / माप(fdt32_t);
		nr_parent_irqs = len / 2;
		क्रम (i = 0; i < len; i += 2) {
			अगर (fdt32_to_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				अवरोध;
			}
		}
	} अन्यथा
		वापस SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	अगर (val && len > 0)
		imsic->guest_सूचकांक_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->guest_सूचकांक_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	अगर (val && len > 0) {
		imsic->hart_सूचकांक_bits = fdt32_to_cpu(*val);
	} अन्यथा {
		imsic->hart_सूचकांक_bits = sbi_fls(nr_parent_irqs);
		अगर ((1UL << imsic->hart_सूचकांक_bits) < nr_parent_irqs)
			imsic->hart_सूचकांक_bits++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	अगर (val && len > 0)
		imsic->group_सूचकांक_bits = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_सूचकांक_bits = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	अगर (val && len > 0)
		imsic->group_सूचकांक_shift = fdt32_to_cpu(*val);
	अन्यथा
		imsic->group_सूचकांक_shift = 2 * IMSIC_MMIO_PAGE_SHIFT;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	अगर (val && len > 0)
		imsic->अंक_ids = fdt32_to_cpu(*val);
	अन्यथा
		वापस SBI_EINVAL;

	क्रम (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->आकार = 0;
	}

	क्रम (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, i,
					    &reg_addr, &reg_आकार);
		अगर (rc < 0 || !reg_आकार)
			अवरोध;
		regs->addr = reg_addr;
		regs->आकार = reg_आकार;
	}
	अगर (!imsic->regs[0].आकार)
		वापस SBI_EINVAL;

	वापस 0;
}

पूर्णांक fdt_parse_plic_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset, काष्ठा plic_तथ्य *plic)
{
	पूर्णांक len, rc;
	स्थिर fdt32_t *val;
	uint64_t reg_addr, reg_आकार;

	अगर (nodeoffset < 0 || !plic || !fdt)
		वापस SBI_ENODEV;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		वापस SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	अगर (len > 0)
		plic->अंक_src = fdt32_to_cpu(*val);

	वापस 0;
}

पूर्णांक fdt_parse_plic(स्थिर व्योम *fdt, काष्ठा plic_तथ्य *plic, स्थिर अक्षर *compat)
{
	पूर्णांक nodeoffset;

	अगर (!compat || !plic || !fdt)
		वापस SBI_ENODEV;

	nodeoffset = fdt_गाँठ_offset_by_compatible(fdt, -1, compat);
	अगर (nodeoffset < 0)
		वापस nodeoffset;

	वापस fdt_parse_plic_गाँठ(fdt, nodeoffset, plic);
}

अटल पूर्णांक fdt_get_aclint_addr_आकार_by_name(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
					    अचिन्हित दीर्घ *out_addr1,
					    अचिन्हित दीर्घ *out_size1,
					    अचिन्हित दीर्घ *out_addr2,
					    अचिन्हित दीर्घ *out_size2)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_आकार;

	rc = fdt_get_गाँठ_addr_आकार_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		reg_addr = reg_आकार = 0;
	*out_addr1 = reg_addr;
	*out_size1 = reg_आकार;

	rc = fdt_get_गाँठ_addr_आकार_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		वापस SBI_ENODEV;
	*out_addr2 = reg_addr;
	*out_size2 = reg_आकार;

	वापस 0;
}

अटल पूर्णांक fdt_get_aclint_addr_आकार(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
				    अचिन्हित दीर्घ *out_addr1,
				    अचिन्हित दीर्घ *out_size1,
				    अचिन्हित दीर्घ *out_addr2,
				    अचिन्हित दीर्घ *out_size2)
{
	पूर्णांक rc;
	uint64_t reg_addr, reg_आकार;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		वापस SBI_ENODEV;
	*out_addr1 = reg_addr;
	*out_size1 = reg_आकार;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 1,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0 || !reg_आकार)
		reg_addr = reg_आकार = 0;
	अगर (out_addr2)
		*out_addr2 = reg_addr;
	अगर (out_size2)
		*out_size2 = reg_आकार;

	वापस 0;
}

पूर्णांक fdt_parse_aclint_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset,
			  bool for_timer, bool allow_regname,
			  अचिन्हित दीर्घ *out_addr1, अचिन्हित दीर्घ *out_size1,
			  अचिन्हित दीर्घ *out_addr2, अचिन्हित दीर्घ *out_size2,
			  u32 *out_first_hartid, u32 *out_hart_गिनती)
{
	स्थिर fdt32_t *val;
	पूर्णांक i, rc, गिनती, cpu_offset, cpu_intc_offset;
	u32 phandle, hwirq, hartid, first_hartid, last_hartid;
	u32 match_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	अगर (nodeoffset < 0 || !fdt ||
	    !out_addr1 || !out_size1 ||
	    !out_first_hartid || !out_hart_गिनती)
		वापस SBI_EINVAL;

	अगर (for_timer && allow_regname && out_addr2 && out_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", शून्य))
		rc = fdt_get_aclint_addr_आकार_by_name(fdt, nodeoffset,
						      out_addr1, out_size1,
						      out_addr2, out_size2);
	अन्यथा
		rc = fdt_get_aclint_addr_आकार(fdt, nodeoffset,
					      out_addr1, out_size1,
					      out_addr2, out_size2);
	अगर (rc)
		वापस rc;

	*out_first_hartid = 0;
	*out_hart_गिनती = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गिनती);
	अगर (!val || गिनती < माप(fdt32_t))
		वापस 0;
	गिनती = गिनती / माप(fdt32_t);

	first_hartid = -1U;
	last_hartid = 0;
	क्रम (i = 0; i < (गिनती / 2); i++) {
		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[(2 * i) + 1]);

		cpu_intc_offset = fdt_गाँठ_offset_by_phandle(fdt, phandle);
		अगर (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		अगर (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		अगर (rc)
			जारी;

		अगर (match_hwirq == hwirq) {
			अगर (hartid < first_hartid)
				first_hartid = hartid;
			अगर (hartid > last_hartid)
				last_hartid = hartid;
		}
	}

	अगर ((last_hartid >= first_hartid) && first_hartid != -1U) {
		*out_first_hartid = first_hartid;
		*out_hart_गिनती = last_hartid - first_hartid + 1;
	}
	वापस 0;
}

पूर्णांक fdt_parse_plmt_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset, अचिन्हित दीर्घ *plmt_base,
			  अचिन्हित दीर्घ *plmt_आकार, u32 *hart_गिनती)
{
	स्थिर fdt32_t *val;
	पूर्णांक rc, i, गिनती;
	uint64_t reg_addr, reg_आकार;
	u32 phandle, hwirq, hartid, hcount;

	अगर (nodeoffset < 0 || !fdt || !plmt_base ||
	    !hart_गिनती || !plmt_आकार)
		वापस SBI_EINVAL;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0)
		वापस SBI_ENODEV;
	*plmt_base = reg_addr;
	*plmt_आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गिनती);
	अगर (!val || गिनती < माप(fdt32_t))
		वापस 0;
	गिनती = गिनती / माप(fdt32_t);

	hcount = 0;
	क्रम (i = 0; i < (गिनती / 2); i++) {
		पूर्णांक cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाँठ_offset_by_phandle(fdt, phandle);
		अगर (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		अगर (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		अगर (rc)
			जारी;

		अगर (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			जारी;

		अगर (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_गिनती = hcount;

	वापस 0;
}

पूर्णांक fdt_parse_plicsw_गाँठ(स्थिर व्योम *fdt, पूर्णांक nodeoffset, अचिन्हित दीर्घ *plicsw_base,
			  अचिन्हित दीर्घ *आकार, u32 *hart_गिनती)
{
	स्थिर fdt32_t *val;
	पूर्णांक rc, i, गिनती;
	uint64_t reg_addr, reg_आकार;
	u32 phandle, hwirq, hartid, hcount;

	अगर (nodeoffset < 0 || !fdt || !plicsw_base ||
	    !hart_गिनती || !आकार)
		वापस SBI_EINVAL;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	अगर (rc < 0)
		वापस SBI_ENODEV;
	*plicsw_base = reg_addr;
	*आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गिनती);
	अगर (!val || गिनती < माप(fdt32_t))
		वापस 0;
	गिनती = गिनती / माप(fdt32_t);

	hcount = 0;
	क्रम (i = 0; i < (गिनती / 2); i++) {
		पूर्णांक cpu_offset, cpu_intc_offset;

		phandle = fdt32_to_cpu(val[2 * i]);
		hwirq = fdt32_to_cpu(val[2 * i + 1]);

		cpu_intc_offset = fdt_गाँठ_offset_by_phandle(fdt, phandle);
		अगर (cpu_intc_offset < 0)
			जारी;

		cpu_offset = fdt_parent_offset(fdt, cpu_intc_offset);
		अगर (cpu_offset < 0)
			जारी;

		rc = fdt_parse_hart_id(fdt, cpu_offset, &hartid);

		अगर (rc)
			जारी;

		अगर (SBI_HARTMASK_MAX_BITS <= sbi_hartid_to_hartindex(hartid))
			जारी;

		अगर (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_गिनती = hcount;

	वापस 0;
}

पूर्णांक fdt_parse_compat_addr(स्थिर व्योम *fdt, uint64_t *addr,
			  स्थिर अक्षर *compatible)
{
	पूर्णांक nodeoffset, rc;

	nodeoffset = fdt_गाँठ_offset_by_compatible(fdt, -1, compatible);
	अगर (nodeoffset < 0)
		वापस nodeoffset;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoffset, 0, addr, शून्य);
	अगर (rc < 0 || !addr)
		वापस SBI_ENODEV;

	वापस 0;
}
