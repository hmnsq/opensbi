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

#परिभाषय DEFAULT_UART_FREQ		0
#परिभाषय DEFAULT_UART_BAUD		115200
#परिभाषय DEFAULT_UART_REG_सरणम्		0
#परिभाषय DEFAULT_UART_REG_IO_विस्तारः	1
#परिभाषय DEFAULT_UART_REG_विस्थापनम्		0

#परिभाषय DEFAULT_RENESAS_SCIF_FREQ		100000000
#परिभाषय DEFAULT_RENESAS_SCIF_BAUD		115200

#परिभाषय DEFAULT_SIFIVE_UART_FREQ		0
#परिभाषय DEFAULT_SIFIVE_UART_BAUD		115200

#परिभाषय DEFAULT_SHAKTI_UART_FREQ		50000000
#परिभाषय DEFAULT_SHAKTI_UART_BAUD		115200

पूर्णाङ्क fdt_parse_phandle_with_तर्काः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव वर्ण *prop, ध्रुव वर्ण *cells_prop,
				पूर्णाङ्क सूचकांक_2, संरचना fdt_phandle_तर्काः *बहिर्गमनम्_तर्काः)
{
	u32 i, pcells;
	पूर्णाङ्क len, pnodeoff;
	ध्रुव fdt32_t *list, *list_अन्तः, *val;

	यदि (!fdt || (nodeoff < 0) || !prop || !cells_prop || !बहिर्गमनम्_तर्काः)
		प्रत्यागम SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	यदि (!list)
		प्रत्यागम SBI_ENOENT;
	list_अन्तः = list + (len / परिमाण(*list));

	यावत् (list < list_अन्तः) {
		pnodeoff = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt,
						fdt32_गन्तव्यम्_cpu(*list));
		यदि (pnodeoff < 0)
			प्रत्यागम pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		यदि (!val)
			प्रत्यागम SBI_ENOENT;
		pcells = fdt32_गन्तव्यम्_cpu(*val);
		यदि (FDT_MAX_PHANDLE_तर्काः < pcells)
			प्रत्यागम SBI_EINVAL;
		यदि (list + pcells > list_अन्तः)
			प्रत्यागम SBI_ENOENT;

		यदि (सूचकांक_2 > 0) {
			list += pcells;
			सूचकांक_2--;
		} अन्यथा {
			बहिर्गमनम्_तर्काः->ग्रन्थिः_विस्थापनम् = pnodeoff;
			बहिर्गमनम्_तर्काः->तर्काः_गणनम् = pcells;
			प्रति (i = 0; i < pcells; i++)
				बहिर्गमनम्_तर्काः->तर्काः[i] = fdt32_गन्तव्यम्_cpu(list[i]);
			प्रत्यागम 0;
		}
	}

	प्रत्यागम SBI_ENOENT;
}

स्थिर पूर्णाङ्क fdt_translate_स्थानाङ्कः(ध्रुव शून्य *fdt, uint64_t reg, पूर्णाङ्क parent,
				 uint64_t *addr)
{
	पूर्णाङ्क i, rlen;
	पूर्णाङ्क cell_parent_addr, cell_child_addr, cell_आकार;
	ध्रुव fdt32_t *ranges;
	uint64_t विस्थापनम्, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, parent, "ranges", &rlen);
	यदि (ranges && rlen > 0) {
		cell_child_addr = fdt_स्थानाङ्कः_cells(fdt, parent);
		यदि (cell_child_addr < 1)
			प्रत्यागम SBI_ENODEV;

		cell_parent_addr = fdt_स्थानाङ्कः_cells(fdt, fdt_parent_विस्थापनम्(fdt, parent));
		यदि (cell_parent_addr < 1)
			प्रत्यागम SBI_ENODEV;

		cell_आकार = fdt_आकार_cells(fdt, parent);
		यदि (cell_आकार < 0)
			प्रत्यागम SBI_ENODEV;

		प्रति (i = 0; i < cell_child_addr; i++)
			caddr = (caddr << 32) | fdt32_गन्तव्यम्_cpu(*ranges++);
		प्रति (i = 0; i < cell_parent_addr; i++)
			paddr = (paddr << 32) | fdt32_गन्तव्यम्_cpu(*ranges++);
		प्रति (i = 0; i < cell_आकार; i++)
			rsize = (rsize << 32) | fdt32_गन्तव्यम्_cpu(*ranges++);
		यदि (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			प्रत्यागम SBI_ENODEV;
		}
		विस्थापनम् = reg - caddr;
		*addr = paddr + विस्थापनम्;
	} अन्यथा {
		/* No translation required */
		*addr = reg;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_get_ग्रन्थिः_addr_आकार(ध्रुव शून्य *fdt, पूर्णाङ्क ग्रन्थिः, पूर्णाङ्क सूचकांक_2,
			   uint64_t *addr, uint64_t *आकार)
{
	पूर्णाङ्क parent, len, i, rc;
	पूर्णाङ्क cell_addr, cell_आकार;
	ध्रुव fdt32_t *prop_addr, *prop_आकार;
	uint64_t अस्थायी = 0;

	यदि (!fdt || ग्रन्थिः < 0 || सूचकांक_2 < 0)
		प्रत्यागम SBI_EINVAL;

	parent = fdt_parent_विस्थापनम्(fdt, ग्रन्थिः);
	यदि (parent < 0)
		प्रत्यागम parent;
	cell_addr = fdt_स्थानाङ्कः_cells(fdt, parent);
	यदि (cell_addr < 1)
		प्रत्यागम SBI_ENODEV;

	cell_आकार = fdt_आकार_cells(fdt, parent);
	यदि (cell_आकार < 0)
		प्रत्यागम SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ग्रन्थिः, "reg", &len);
	यदि (!prop_addr)
		प्रत्यागम SBI_ENODEV;

	यदि ((len / परिमाण(u32)) <= (सूचकांक_2 * (cell_addr + cell_आकार)))
		प्रत्यागम SBI_EINVAL;

	prop_addr = prop_addr + (सूचकांक_2 * (cell_addr + cell_आकार));
	prop_आकार = prop_addr + cell_addr;

	यदि (addr) {
		प्रति (i = 0; i < cell_addr; i++)
			अस्थायी = (अस्थायी << 32) | fdt32_गन्तव्यम्_cpu(*prop_addr++);
		कुरु {
			यदि (parent < 0)
				भङ्ग;
			rc  = fdt_translate_स्थानाङ्कः(fdt, अस्थायी, parent, addr);
			यदि (rc)
				भङ्ग;
			parent = fdt_parent_विस्थापनम्(fdt, parent);
			अस्थायी = *addr;
		} यावत् (1);
	}
	अस्थायी = 0;

	यदि (आकार) {
		प्रति (i = 0; i < cell_आकार; i++)
			अस्थायी = (अस्थायी << 32) | fdt32_गन्तव्यम्_cpu(*prop_आकार++);
		*आकार = अस्थायी;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_get_ग्रन्थिः_addr_आकार_by_name(ध्रुव शून्य *fdt, पूर्णाङ्क ग्रन्थिः, ध्रुव वर्ण *name,
				   uint64_t *addr, uint64_t *आकार)
{
	पूर्णाङ्क i, j, गणनम्;
	ध्रुव वर्ण *val;
	ध्रुव वर्ण *regname;

	यदि (!fdt || ग्रन्थिः < 0 || !name)
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, ग्रन्थिः, "reg-names", &गणनम्);
	यदि (!val)
		प्रत्यागम SBI_ENODEV;

	प्रति (i = 0, j = 0; i < गणनम्; i++, j++) {
		regname = val + i;

		यदि (वाक्यं_तुलय(name, regname) == 0)
			प्रत्यागम fdt_get_ग्रन्थिः_addr_आकार(fdt, ग्रन्थिः, j, addr, आकार);

		i += वाक्यदैर्घ्यम्(regname);
	}

	प्रत्यागम SBI_ENODEV;
}

bool fdt_ग्रन्थिः_is_enabled(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	यदि (!prop)
		प्रत्यागम true;

	यदि (!वाक्ये_तुलय(prop, "okay", वाक्यदैर्घ्यम्("okay")))
		प्रत्यागम true;

	यदि (!वाक्ये_तुलय(prop, "ok", वाक्यदैर्घ्यम्("ok")))
		प्रत्यागम true;

	प्रत्यागम false;
}

पूर्णाङ्क fdt_parse_hart_id(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_विस्थापनम्, u32 *hartid)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;
	ध्रुव fdt32_t *val;

	यदि (!fdt || cpu_विस्थापनम् < 0)
		प्रत्यागम SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_विस्थापनम्, "device_type", &len);
	यदि (!prop || !len)
		प्रत्यागम SBI_EINVAL;
	यदि (वाक्ये_तुलय (prop, "cpu", वाक्यदैर्घ्यम् ("cpu")))
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_विस्थापनम्, "reg", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		प्रत्यागम SBI_EINVAL;

	यदि (len > परिमाण(fdt32_t))
		val++;

	यदि (hartid)
		*hartid = fdt32_गन्तव्यम्_cpu(*val);

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_cbom_खण्डम्_आकार(ध्रुव शून्य *fdt, पूर्णाङ्क cpu_विस्थापनम्, अचिह्नित दीर्घ *cbom_खण्डम्_आकार)
{
	पूर्णाङ्क len;
	ध्रुव शून्य *prop;
	ध्रुव fdt32_t *val;

	यदि (!fdt || cpu_विस्थापनम् < 0)
		प्रत्यागम SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_विस्थापनम्, "device_type", &len);
	यदि (!prop || !len)
		प्रत्यागम SBI_EINVAL;
	यदि (वाक्ये_तुलय (prop, "cpu", वाक्यदैर्घ्यम् ("cpu")))
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_विस्थापनम्, "riscv,cbom-block-size", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		प्रत्यागम SBI_EINVAL;

	यदि (cbom_खण्डम्_आकार)
		*cbom_खण्डम्_आकार = fdt32_गन्तव्यम्_cpu(*val);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_max_enabled_hart_id(ध्रुव शून्य *fdt, u32 *max_hartid)
{
	u32 hartid;
	पूर्णाङ्क err, cpu_विस्थापनम्, cpus_विस्थापनम्;

	यदि (!fdt)
		प्रत्यागम SBI_EINVAL;
	यदि (!max_hartid)
		प्रत्यागम 0;

	*max_hartid = 0;

	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम cpus_विस्थापनम्;

	fdt_for_each_subnode(cpu_विस्थापनम्, fdt, cpus_विस्थापनम्) {
		err = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);
		यदि (err)
			अनुवर्त;

		यदि (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_विस्थापनम्))
			अनुवर्त;

		यदि (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_timebase_frequency(ध्रुव शून्य *fdt, अचिह्नित दीर्घ *freq)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क len, cpus_विस्थापनम्;

	यदि (!fdt || !freq)
		प्रत्यागम SBI_EINVAL;

	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम cpus_विस्थापनम्;

	val = fdt_getprop(fdt, cpus_विस्थापनम्, "timebase-frequency", &len);
	यदि (len > 0 && val)
		*freq = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		प्रत्यागम SBI_ENOENT;

	प्रत्यागम 0;
}

#परिभाषय RISCV_ISA_EXT_NAME_LEN_MAX	32

स्थिर पूर्णाङ्क fdt_parse_isa_one_hart(ध्रुव वर्ण *isa, अचिह्नित दीर्घ *extensions)
{
	size_t i, j, isa_len;
	वर्ण mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = वाक्यदैर्घ्यम्(isa);

	यदि (isa[i] == 'r' || isa[i] == 'R')
		i++;
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	यदि (isa[i] == 'v' || isa[i] == 'V')
		i++;
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	यदि (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	अन्यथा यदि (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	/* Skip base ISA extensions */
	प्रति (; i < isa_len; i++) {
		यदि (isa[i] == '_')
			भङ्ग;
	}

	यावत् (i < isa_len) {
		यदि (isa[i] != '_') {
			i++;
			अनुवर्त;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		यावत् ((i < isa_len) && (isa[i] != '_') &&
		       (j < (परिमाण(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		यदि (!j)
			अनुवर्त;

#परिभाषय समुच्चयः_multi_letter_ext(name, bit)				\
			यदि (!वाक्यं_तुलय(mstr, name)) {		\
				__set_bit(bit, extensions);	\
				अनुवर्त;			\
			}

		प्रति (j = 0; j < SBI_HART_EXT_MAX; j++) {
			समुच्चयः_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#अपरिभाषय समुच्चयः_multi_letter_ext
	}

	प्रत्यागम 0;
}

स्थिर शून्य fdt_parse_isa_extensions_one_hart(ध्रुव वर्ण *isa,
					      अचिह्नित दीर्घ *extensions,
					      पूर्णाङ्क len)
{
	size_t i;

	प्रति (i = 0; i < SBI_HART_EXT_MAX; i++) {
		यदि (fdt_stringlist_अन्तर्भवति(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

पूर्णाङ्क fdt_parse_isa_extensions_all_harts(ध्रुव शून्य *fdt)
{
	u32 hartid;
	ध्रुव fdt32_t *val;
	संरचना sbi_scratch *scratch;
	संरचना sbi_hart_features *hfeatures;
	पूर्णाङ्क err, cpu_विस्थापनम्, cpus_विस्थापनम्, len;

	यदि (!fdt)
		प्रत्यागम SBI_EINVAL;

	cpus_विस्थापनम् = fdt_मार्गः_विस्थापनम्(fdt, "/cpus");
	यदि (cpus_विस्थापनम् < 0)
		प्रत्यागम cpus_विस्थापनम्;

	fdt_for_each_subnode(cpu_विस्थापनम्, fdt, cpus_विस्थापनम्) {
		err = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);
		यदि (err)
			अनुवर्त;

		यदि (!fdt_ग्रन्थिः_is_enabled(fdt, cpu_विस्थापनम्))
			अनुवर्त;

		scratch = sbi_hartid_गन्तव्यम्_scratch(hartid);
		यदि (!scratch)
			प्रत्यागम SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		यदि (!hfeatures)
			प्रत्यागम SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_विस्थापनम्, "riscv,isa-extensions", &len);
		यदि (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ध्रुव वर्ण *)val,
							  hfeatures->extensions, len);
			अनुवर्त;
		}

		val = fdt_getprop(fdt, cpu_विस्थापनम्, "riscv,isa", &len);
		यदि (!val || len <= 0)
			प्रत्यागम SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ध्रुव वर्ण *)val, hfeatures->extensions);
		यदि (err)
			प्रत्यागम err;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_parse_uart_ग्रन्थिः_common(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				      संरचना platform_uart_दत्त *uart,
				      अचिह्नित दीर्घ default_freq,
				      अचिह्नित दीर्घ default_baud)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;
	uint64_t reg_addr, reg_आकार;

	यदि (nodeoffset < 0 || !uart || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		प्रत्यागम SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	यदि (len > 0 && val)
		uart->freq = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	यदि (len > 0 && val)
		uart->baud = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		uart->baud = default_baud;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_gaisler_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_दत्त *uart)
{
	प्रत्यागम fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

पूर्णाङ्क fdt_parse_renesas_scif_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				संरचना platform_uart_दत्त *uart)
{
	प्रत्यागम fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

पूर्णाङ्क fdt_parse_shakti_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_दत्त *uart)
{
	प्रत्यागम fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

पूर्णाङ्क fdt_parse_sifive_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_दत्त *uart)
{
	प्रत्यागम fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

पूर्णाङ्क fdt_parse_uart_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			संरचना platform_uart_दत्त *uart)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;

	rc = fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	यदि (rc)
		प्रत्यागम rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	यदि (len > 0 && val)
		uart->reg_सरणम् = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		uart->reg_सरणम् = DEFAULT_UART_REG_सरणम्;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	यदि (len > 0 && val)
		uart->reg_io_विस्तारः = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		uart->reg_io_विस्तारः = DEFAULT_UART_REG_IO_विस्तारः;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	यदि (len > 0 && val)
		uart->reg_विस्थापनम् = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		uart->reg_विस्थापनम् = DEFAULT_UART_REG_विस्थापनम्;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_uart8250(ध्रुव शून्य *fdt, संरचना platform_uart_दत्त *uart,
		   ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset;

	यदि (!compatible || !uart || !fdt)
		प्रत्यागम SBI_ENODEV;

	nodeoffset = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, -1, compatible);
	यदि (nodeoffset < 0)
		प्रत्यागम nodeoffset;

	प्रत्यागम fdt_parse_uart_ग्रन्थिः(fdt, nodeoffset, uart);
}

पूर्णाङ्क fdt_parse_xlnx_uartlite_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			       संरचना platform_uart_दत्त *uart)
{
	प्रत्यागम fdt_parse_uart_ग्रन्थिः_common(fdt, nodeoffset, uart, 0, 0);
}

स्थिर पूर्णाङ्क fdt_aplic_find_imsic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				     संरचना imsic_दत्त *imsic, bool mmode)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	यदि (val && len >= परिमाण(fdt32_t)) {
		noff = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, fdt32_गन्तव्यम्_cpu(*val));
		यदि (noff < 0)
			प्रत्यागम noff;

		rc = fdt_parse_imsic_ग्रन्थिः(fdt, noff, imsic);
		यदि (rc)
			प्रत्यागम rc;

		rc = imsic_दत्त_check(imsic);
		यदि (rc)
			प्रत्यागम rc;

		यदि (imsic->targets_mmode == mmode) {
			प्रत्यागम 0;
		}
	} अन्यथा {
		प्रत्यागम SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	यदि (!val || len < परिमाण(fdt32_t))
		प्रत्यागम SBI_ENODEV;

	len /= परिमाण(fdt32_t);

	प्रति (i = 0; i < len; i++) {
		noff = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, fdt32_गन्तव्यम्_cpu(val[i]));
		यदि (noff < 0)
			प्रत्यागम noff;

		rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, noff, imsic, mmode);
		यदि (!rc)
			भङ्ग;
	}

	प्रत्यागम rc;
}

पूर्णाङ्क fdt_parse_aplic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, संरचना aplic_दत्त *aplic)
{
	bool child_प्राप्तम्;
	ध्रुव fdt32_t *val;
	ध्रुव fdt32_t *del;
	संरचना imsic_दत्त imsic = { 0 };
	पूर्णाङ्क i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_आकार;
	संरचना aplic_delegate_दत्त *deleg;

	यदि (nodeoff < 0 || !aplic || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		प्रत्यागम SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	यदि (len > 0)
		aplic->अङ्कः_source = fdt32_गन्तव्यम्_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	यदि (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		प्रति (i = 0; i < len; i += 2) {
			यदि (fdt32_गन्तव्यम्_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				भङ्ग;
			}
		}
		aplic->अङ्कः_idc = len / 2;
	}

	rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, nodeoff, &imsic, true);
	यदि (!rc) {
		aplic->parent_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_सूचकांक_द्वयङ्काः;
		aplic->msicfg_mmode.lhxw = imsic.hart_सूचकांक_द्वयङ्काः;
		aplic->msicfg_mmode.hhxw = imsic.गणः_सूचकांक_द्वयङ्काः;
		aplic->msicfg_mmode.hhxs = imsic.गणः_सूचकांक_सरणम्;
		यदि (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_PAGE_सरणम्))
			प्रत्यागम SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.आधारः_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_find_imsic_ग्रन्थिः(fdt, nodeoff, &imsic, false);
	यदि (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_सूचकांक_द्वयङ्काः;
		aplic->msicfg_smode.lhxw = imsic.hart_सूचकांक_द्वयङ्काः;
		aplic->msicfg_smode.hhxw = imsic.गणः_सूचकांक_द्वयङ्काः;
		aplic->msicfg_smode.hhxs = imsic.गणः_सूचकांक_सरणम्;
		यदि (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_PAGE_सरणम्))
			प्रत्यागम SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.आधारः_addr = imsic.regs[0].addr;
	}

	प्रति (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->first_irq = 0;
		deleg->चरमः_irq = 0;
		deleg->child_सूचकांक = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	यदि (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	यदि (!del || len < (3 * परिमाण(fdt32_t)))
		गच्छ skip_delegate_parse;
	d = 0;
	dcnt = len / परिमाण(fdt32_t);
	प्रति (i = 0; i < dcnt; i += 3) {
		यदि (d >= APLIC_MAX_DELEGATE)
			भङ्ग;
		deleg = &aplic->delegate[d];

		deleg->first_irq = fdt32_गन्तव्यम्_cpu(del[i + 1]);
		deleg->चरमः_irq = fdt32_गन्तव्यम्_cpu(del[i + 2]);
		deleg->child_सूचकांक = 0;

		child_प्राप्तम् = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		यदि (!val || len < परिमाण(fdt32_t)) {
			deleg->first_irq = 0;
			deleg->चरमः_irq = 0;
			deleg->child_सूचकांक = 0;
			अनुवर्त;
		}
		len = len / परिमाण(fdt32_t);
		प्रति (j = 0; j < len; j++) {
			यदि (del[i] != val[j])
				अनुवर्त;
			deleg->child_सूचकांक = j;
			child_प्राप्तम् = true;
			भङ्ग;
		}

		यदि (child_प्राप्तम्) {
			d++;
		} अन्यथा {
			deleg->first_irq = 0;
			deleg->चरमः_irq = 0;
			deleg->child_सूचकांक = 0;
		}
	}
skip_delegate_parse:

	प्रत्यागम 0;
}

bool fdt_check_imsic_mlevel(ध्रुव शून्य *fdt)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, len, noff = 0;

	यदि (!fdt)
		प्रत्यागम false;

	यावत् ((noff = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		यदि (val && len > परिमाण(fdt32_t)) {
			len = len / परिमाण(fdt32_t);
			प्रति (i = 0; i < len; i += 2) {
				यदि (fdt32_गन्तव्यम्_cpu(val[i + 1]) == IRQ_M_EXT)
					प्रत्यागम true;
			}
		}
	}

	प्रत्यागम false;
}

पूर्णाङ्क fdt_parse_imsic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, संरचना imsic_दत्त *imsic)
{
	ध्रुव fdt32_t *val;
	संरचना imsic_regs *regs;
	uint64_t reg_addr, reg_आकार;
	पूर्णाङ्क i, rc, len, nr_parent_irqs;

	यदि (nodeoff < 0 || !imsic || !fdt)
		प्रत्यागम SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	यदि (val && len > परिमाण(fdt32_t)) {
		len = len / परिमाण(fdt32_t);
		nr_parent_irqs = len / 2;
		प्रति (i = 0; i < len; i += 2) {
			यदि (fdt32_गन्तव्यम्_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				भङ्ग;
			}
		}
	} अन्यथा
		प्रत्यागम SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	यदि (val && len > 0)
		imsic->guest_सूचकांक_द्वयङ्काः = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		imsic->guest_सूचकांक_द्वयङ्काः = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	यदि (val && len > 0) {
		imsic->hart_सूचकांक_द्वयङ्काः = fdt32_गन्तव्यम्_cpu(*val);
	} अन्यथा {
		imsic->hart_सूचकांक_द्वयङ्काः = sbi_fls(nr_parent_irqs);
		यदि ((1UL << imsic->hart_सूचकांक_द्वयङ्काः) < nr_parent_irqs)
			imsic->hart_सूचकांक_द्वयङ्काः++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	यदि (val && len > 0)
		imsic->गणः_सूचकांक_द्वयङ्काः = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		imsic->गणः_सूचकांक_द्वयङ्काः = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	यदि (val && len > 0)
		imsic->गणः_सूचकांक_सरणम् = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		imsic->गणः_सूचकांक_सरणम् = 2 * IMSIC_MMIO_PAGE_सरणम्;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	यदि (val && len > 0)
		imsic->अङ्कः_ids = fdt32_गन्तव्यम्_cpu(*val);
	अन्यथा
		प्रत्यागम SBI_EINVAL;

	प्रति (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->आकार = 0;
	}

	प्रति (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, i,
					    &reg_addr, &reg_आकार);
		यदि (rc < 0 || !reg_आकार)
			भङ्ग;
		regs->addr = reg_addr;
		regs->आकार = reg_आकार;
	}
	यदि (!imsic->regs[0].आकार)
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_plic_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, संरचना plic_दत्त *plic)
{
	पूर्णाङ्क len, rc;
	ध्रुव fdt32_t *val;
	uint64_t reg_addr, reg_आकार;

	यदि (nodeoffset < 0 || !plic || !fdt)
		प्रत्यागम SBI_ENODEV;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		प्रत्यागम SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	यदि (len > 0)
		plic->अङ्कः_src = fdt32_गन्तव्यम्_cpu(*val);

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_plic(ध्रुव शून्य *fdt, संरचना plic_दत्त *plic, ध्रुव वर्ण *compat)
{
	पूर्णाङ्क nodeoffset;

	यदि (!compat || !plic || !fdt)
		प्रत्यागम SBI_ENODEV;

	nodeoffset = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, -1, compat);
	यदि (nodeoffset < 0)
		प्रत्यागम nodeoffset;

	प्रत्यागम fdt_parse_plic_ग्रन्थिः(fdt, nodeoffset, plic);
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_आकार_by_name(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
					    अचिह्नित दीर्घ *बहिर्गमनम्_addr1,
					    अचिह्नित दीर्घ *बहिर्गमनम्_size1,
					    अचिह्नित दीर्घ *बहिर्गमनम्_addr2,
					    अचिह्नित दीर्घ *बहिर्गमनम्_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_आकार;

	rc = fdt_get_ग्रन्थिः_addr_आकार_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		reg_addr = reg_आकार = 0;
	*बहिर्गमनम्_addr1 = reg_addr;
	*बहिर्गमनम्_size1 = reg_आकार;

	rc = fdt_get_ग्रन्थिः_addr_आकार_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		प्रत्यागम SBI_ENODEV;
	*बहिर्गमनम्_addr2 = reg_addr;
	*बहिर्गमनम्_size2 = reg_आकार;

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_get_aclint_addr_आकार(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
				    अचिह्नित दीर्घ *बहिर्गमनम्_addr1,
				    अचिह्नित दीर्घ *बहिर्गमनम्_size1,
				    अचिह्नित दीर्घ *बहिर्गमनम्_addr2,
				    अचिह्नित दीर्घ *बहिर्गमनम्_size2)
{
	पूर्णाङ्क rc;
	uint64_t reg_addr, reg_आकार;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		प्रत्यागम SBI_ENODEV;
	*बहिर्गमनम्_addr1 = reg_addr;
	*बहिर्गमनम्_size1 = reg_आकार;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 1,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0 || !reg_आकार)
		reg_addr = reg_आकार = 0;
	यदि (बहिर्गमनम्_addr2)
		*बहिर्गमनम्_addr2 = reg_addr;
	यदि (बहिर्गमनम्_size2)
		*बहिर्गमनम्_size2 = reg_आकार;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_aclint_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset,
			  bool for_timer, bool allow_regname,
			  अचिह्नित दीर्घ *बहिर्गमनम्_addr1, अचिह्नित दीर्घ *बहिर्गमनम्_size1,
			  अचिह्नित दीर्घ *बहिर्गमनम्_addr2, अचिह्नित दीर्घ *बहिर्गमनम्_size2,
			  u32 *बहिर्गमनम्_first_hartid, u32 *बहिर्गमनम्_hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क i, rc, गणनम्, cpu_विस्थापनम्, cpu_intc_विस्थापनम्;
	u32 phandle, hwirq, hartid, first_hartid, चरमः_hartid;
	u32 मेलनम्_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	यदि (nodeoffset < 0 || !fdt ||
	    !बहिर्गमनम्_addr1 || !बहिर्गमनम्_size1 ||
	    !बहिर्गमनम्_first_hartid || !बहिर्गमनम्_hart_गणनम्)
		प्रत्यागम SBI_EINVAL;

	यदि (for_timer && allow_regname && बहिर्गमनम्_addr2 && बहिर्गमनम्_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", रिक्त))
		rc = fdt_get_aclint_addr_आकार_by_name(fdt, nodeoffset,
						      बहिर्गमनम्_addr1, बहिर्गमनम्_size1,
						      बहिर्गमनम्_addr2, बहिर्गमनम्_size2);
	अन्यथा
		rc = fdt_get_aclint_addr_आकार(fdt, nodeoffset,
					      बहिर्गमनम्_addr1, बहिर्गमनम्_size1,
					      बहिर्गमनम्_addr2, बहिर्गमनम्_size2);
	यदि (rc)
		प्रत्यागम rc;

	*बहिर्गमनम्_first_hartid = 0;
	*बहिर्गमनम्_hart_गणनम् = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	यदि (!val || गणनम् < परिमाण(fdt32_t))
		प्रत्यागम 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	first_hartid = -1U;
	चरमः_hartid = 0;
	प्रति (i = 0; i < (गणनम् / 2); i++) {
		phandle = fdt32_गन्तव्यम्_cpu(val[2 * i]);
		hwirq = fdt32_गन्तव्यम्_cpu(val[(2 * i) + 1]);

		cpu_intc_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, phandle);
		यदि (cpu_intc_विस्थापनम् < 0)
			अनुवर्त;

		cpu_विस्थापनम् = fdt_parent_विस्थापनम्(fdt, cpu_intc_विस्थापनम्);
		यदि (cpu_विस्थापनम् < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);
		यदि (rc)
			अनुवर्त;

		यदि (मेलनम्_hwirq == hwirq) {
			यदि (hartid < first_hartid)
				first_hartid = hartid;
			यदि (hartid > चरमः_hartid)
				चरमः_hartid = hartid;
		}
	}

	यदि ((चरमः_hartid >= first_hartid) && first_hartid != -1U) {
		*बहिर्गमनम्_first_hartid = first_hartid;
		*बहिर्गमनम्_hart_गणनम् = चरमः_hartid - first_hartid + 1;
	}
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_plmt_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, अचिह्नित दीर्घ *plmt_आधारः,
			  अचिह्नित दीर्घ *plmt_आकार, u32 *hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, i, गणनम्;
	uint64_t reg_addr, reg_आकार;
	u32 phandle, hwirq, hartid, hcount;

	यदि (nodeoffset < 0 || !fdt || !plmt_आधारः ||
	    !hart_गणनम् || !plmt_आकार)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0)
		प्रत्यागम SBI_ENODEV;
	*plmt_आधारः = reg_addr;
	*plmt_आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	यदि (!val || गणनम् < परिमाण(fdt32_t))
		प्रत्यागम 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	hcount = 0;
	प्रति (i = 0; i < (गणनम् / 2); i++) {
		पूर्णाङ्क cpu_विस्थापनम्, cpu_intc_विस्थापनम्;

		phandle = fdt32_गन्तव्यम्_cpu(val[2 * i]);
		hwirq = fdt32_गन्तव्यम्_cpu(val[2 * i + 1]);

		cpu_intc_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, phandle);
		यदि (cpu_intc_विस्थापनम् < 0)
			अनुवर्त;

		cpu_विस्थापनम् = fdt_parent_विस्थापनम्(fdt, cpu_intc_विस्थापनम्);
		यदि (cpu_विस्थापनम् < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);

		यदि (rc)
			अनुवर्त;

		यदि (SBI_HARTMASK_MAX_द्वयङ्काः <= sbi_hartid_गन्तव्यम्_hartindex(hartid))
			अनुवर्त;

		यदि (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_गणनम् = hcount;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_plicsw_ग्रन्थिः(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset, अचिह्नित दीर्घ *plicsw_आधारः,
			  अचिह्नित दीर्घ *आकार, u32 *hart_गणनम्)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, i, गणनम्;
	uint64_t reg_addr, reg_आकार;
	u32 phandle, hwirq, hartid, hcount;

	यदि (nodeoffset < 0 || !fdt || !plicsw_आधारः ||
	    !hart_गणनम् || !आकार)
		प्रत्यागम SBI_EINVAL;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0,
				    &reg_addr, &reg_आकार);
	यदि (rc < 0)
		प्रत्यागम SBI_ENODEV;
	*plicsw_आधारः = reg_addr;
	*आकार = reg_आकार;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &गणनम्);
	यदि (!val || गणनम् < परिमाण(fdt32_t))
		प्रत्यागम 0;
	गणनम् = गणनम् / परिमाण(fdt32_t);

	hcount = 0;
	प्रति (i = 0; i < (गणनम् / 2); i++) {
		पूर्णाङ्क cpu_विस्थापनम्, cpu_intc_विस्थापनम्;

		phandle = fdt32_गन्तव्यम्_cpu(val[2 * i]);
		hwirq = fdt32_गन्तव्यम्_cpu(val[2 * i + 1]);

		cpu_intc_विस्थापनम् = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, phandle);
		यदि (cpu_intc_विस्थापनम् < 0)
			अनुवर्त;

		cpu_विस्थापनम् = fdt_parent_विस्थापनम्(fdt, cpu_intc_विस्थापनम्);
		यदि (cpu_विस्थापनम् < 0)
			अनुवर्त;

		rc = fdt_parse_hart_id(fdt, cpu_विस्थापनम्, &hartid);

		यदि (rc)
			अनुवर्त;

		यदि (SBI_HARTMASK_MAX_द्वयङ्काः <= sbi_hartid_गन्तव्यम्_hartindex(hartid))
			अनुवर्त;

		यदि (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_गणनम् = hcount;

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_parse_compat_addr(ध्रुव शून्य *fdt, uint64_t *addr,
			  ध्रुव वर्ण *compatible)
{
	पूर्णाङ्क nodeoffset, rc;

	nodeoffset = fdt_ग्रन्थिः_विस्थापनम्_by_compatible(fdt, -1, compatible);
	यदि (nodeoffset < 0)
		प्रत्यागम nodeoffset;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoffset, 0, addr, रिक्त);
	यदि (rc < 0 || !addr)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम 0;
}
