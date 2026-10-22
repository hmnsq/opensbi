// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_hartmask.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/irqchip/aplic.h>
#అన్తర్భావ <sbi_utils/irqchip/imsic.h>
#అన్తర్భావ <sbi_utils/irqchip/plic.h>

#నిర్వచించు DEFAULT_UART_FREQ		0
#నిర్వచించు DEFAULT_UART_BAUD		115200
#నిర్వచించు DEFAULT_UART_REG_జరుపు		0
#నిర్వచించు DEFAULT_UART_REG_IO_వెడల్పు	1
#నిర్వచించు DEFAULT_UART_REG_స్థానభ్రంశం		0

#నిర్వచించు DEFAULT_RENESAS_SCIF_FREQ		100000000
#నిర్వచించు DEFAULT_RENESAS_SCIF_BAUD		115200

#నిర్వచించు DEFAULT_SIFIVE_UART_FREQ		0
#నిర్వచించు DEFAULT_SIFIVE_UART_BAUD		115200

#నిర్వచించు DEFAULT_SHAKTI_UART_FREQ		50000000
#నిర్వచించు DEFAULT_SHAKTI_UART_BAUD		115200

పూర్ణాంకం fdt_parse_phandle_with_వాదనలు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				స్థిరరాశి అక్షరం *prop, స్థిరరాశి అక్షరం *cells_prop,
				పూర్ణాంకం సూచీ_2, నిర్మాణం fdt_phandle_వాదనలు *బహిర్గతం_వాదనలు)
{
	u32 i, pcells;
	పూర్ణాంకం len, pnodeoff;
	స్థిరరాశి fdt32_t *list, *list_చివర, *val;

	ఒకవేళ (!fdt || (nodeoff < 0) || !prop || !cells_prop || !బహిర్గతం_వాదనలు)
		తిరిగి SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	ఒకవేళ (!list)
		తిరిగి SBI_ENOENT;
	list_చివర = list + (len / పరిమాణం(*list));

	అయితే (list < list_చివర) {
		pnodeoff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt,
						fdt32_గమ్యం_cpu(*list));
		ఒకవేళ (pnodeoff < 0)
			తిరిగి pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		ఒకవేళ (!val)
			తిరిగి SBI_ENOENT;
		pcells = fdt32_గమ్యం_cpu(*val);
		ఒకవేళ (FDT_MAX_PHANDLE_వాదనలు < pcells)
			తిరిగి SBI_EINVAL;
		ఒకవేళ (list + pcells > list_చివర)
			తిరిగి SBI_ENOENT;

		ఒకవేళ (సూచీ_2 > 0) {
			list += pcells;
			సూచీ_2--;
		} లేకపోతే {
			బహిర్గతం_వాదనలు->కణుపు_స్థానభ్రంశం = pnodeoff;
			బహిర్గతం_వాదనలు->వాదనలు_లెక్క = pcells;
			కొరకు (i = 0; i < pcells; i++)
				బహిర్గతం_వాదనలు->వాదనలు[i] = fdt32_గమ్యం_cpu(list[i]);
			తిరిగి 0;
		}
	}

	తిరిగి SBI_ENOENT;
}

స్థిర పూర్ణాంకం fdt_translate_చిరునామా(స్థిరరాశి శూన్య *fdt, uint64_t reg, పూర్ణాంకం మాతృ,
				 uint64_t *addr)
{
	పూర్ణాంకం i, rlen;
	పూర్ణాంకం cell_మాతృ_addr, cell_బిడ్డ_addr, cell_కొలత;
	స్థిరరాశి fdt32_t *ranges;
	uint64_t స్థానభ్రంశం, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, మాతృ, "ranges", &rlen);
	ఒకవేళ (ranges && rlen > 0) {
		cell_బిడ్డ_addr = fdt_చిరునామా_cells(fdt, మాతృ);
		ఒకవేళ (cell_బిడ్డ_addr < 1)
			తిరిగి SBI_ENODEV;

		cell_మాతృ_addr = fdt_చిరునామా_cells(fdt, fdt_మాతృ_స్థానభ్రంశం(fdt, మాతృ));
		ఒకవేళ (cell_మాతృ_addr < 1)
			తిరిగి SBI_ENODEV;

		cell_కొలత = fdt_కొలత_cells(fdt, మాతృ);
		ఒకవేళ (cell_కొలత < 0)
			తిరిగి SBI_ENODEV;

		కొరకు (i = 0; i < cell_బిడ్డ_addr; i++)
			caddr = (caddr << 32) | fdt32_గమ్యం_cpu(*ranges++);
		కొరకు (i = 0; i < cell_మాతృ_addr; i++)
			paddr = (paddr << 32) | fdt32_గమ్యం_cpu(*ranges++);
		కొరకు (i = 0; i < cell_కొలత; i++)
			rsize = (rsize << 32) | fdt32_గమ్యం_cpu(*ranges++);
		ఒకవేళ (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			తిరిగి SBI_ENODEV;
		}
		స్థానభ్రంశం = reg - caddr;
		*addr = paddr + స్థానభ్రంశం;
	} లేకపోతే {
		/* No translation required */
		*addr = reg;
	}

	తిరిగి 0;
}

పూర్ణాంకం fdt_get_కణుపు_addr_కొలత(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం కణుపు, పూర్ణాంకం సూచీ_2,
			   uint64_t *addr, uint64_t *కొలత)
{
	పూర్ణాంకం మాతృ, len, i, rc;
	పూర్ణాంకం cell_addr, cell_కొలత;
	స్థిరరాశి fdt32_t *prop_addr, *prop_కొలత;
	uint64_t తాత్కాలిక = 0;

	ఒకవేళ (!fdt || కణుపు < 0 || సూచీ_2 < 0)
		తిరిగి SBI_EINVAL;

	మాతృ = fdt_మాతృ_స్థానభ్రంశం(fdt, కణుపు);
	ఒకవేళ (మాతృ < 0)
		తిరిగి మాతృ;
	cell_addr = fdt_చిరునామా_cells(fdt, మాతృ);
	ఒకవేళ (cell_addr < 1)
		తిరిగి SBI_ENODEV;

	cell_కొలత = fdt_కొలత_cells(fdt, మాతృ);
	ఒకవేళ (cell_కొలత < 0)
		తిరిగి SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, కణుపు, "reg", &len);
	ఒకవేళ (!prop_addr)
		తిరిగి SBI_ENODEV;

	ఒకవేళ ((len / పరిమాణం(u32)) <= (సూచీ_2 * (cell_addr + cell_కొలత)))
		తిరిగి SBI_EINVAL;

	prop_addr = prop_addr + (సూచీ_2 * (cell_addr + cell_కొలత));
	prop_కొలత = prop_addr + cell_addr;

	ఒకవేళ (addr) {
		కొరకు (i = 0; i < cell_addr; i++)
			తాత్కాలిక = (తాత్కాలిక << 32) | fdt32_గమ్యం_cpu(*prop_addr++);
		చేయి {
			ఒకవేళ (మాతృ < 0)
				ఆపు;
			rc  = fdt_translate_చిరునామా(fdt, తాత్కాలిక, మాతృ, addr);
			ఒకవేళ (rc)
				ఆపు;
			మాతృ = fdt_మాతృ_స్థానభ్రంశం(fdt, మాతృ);
			తాత్కాలిక = *addr;
		} అయితే (1);
	}
	తాత్కాలిక = 0;

	ఒకవేళ (కొలత) {
		కొరకు (i = 0; i < cell_కొలత; i++)
			తాత్కాలిక = (తాత్కాలిక << 32) | fdt32_గమ్యం_cpu(*prop_కొలత++);
		*కొలత = తాత్కాలిక;
	}

	తిరిగి 0;
}

పూర్ణాంకం fdt_get_కణుపు_addr_కొలత_by_name(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం కణుపు, స్థిరరాశి అక్షరం *name,
				   uint64_t *addr, uint64_t *కొలత)
{
	పూర్ణాంకం i, j, లెక్క;
	స్థిరరాశి అక్షరం *val;
	స్థిరరాశి అక్షరం *regname;

	ఒకవేళ (!fdt || కణుపు < 0 || !name)
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, కణుపు, "reg-names", &లెక్క);
	ఒకవేళ (!val)
		తిరిగి SBI_ENODEV;

	కొరకు (i = 0, j = 0; i < లెక్క; i++, j++) {
		regname = val + i;

		ఒకవేళ (strcmp(name, regname) == 0)
			తిరిగి fdt_get_కణుపు_addr_కొలత(fdt, కణుపు, j, addr, కొలత);

		i += అక్షరశ్రేణి_పొడవు(regname);
	}

	తిరిగి SBI_ENODEV;
}

bool fdt_కణుపు_is_enabled(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff)
{
	పూర్ణాంకం len;
	స్థిరరాశి శూన్య *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	ఒకవేళ (!prop)
		తిరిగి true;

	ఒకవేళ (!అక్షరశ్రేణి_పోలిక(prop, "okay", అక్షరశ్రేణి_పొడవు("okay")))
		తిరిగి true;

	ఒకవేళ (!అక్షరశ్రేణి_పోలిక(prop, "ok", అక్షరశ్రేణి_పొడవు("ok")))
		తిరిగి true;

	తిరిగి false;
}

పూర్ణాంకం fdt_parse_hart_id(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం cpu_స్థానభ్రంశం, u32 *hartid)
{
	పూర్ణాంకం len;
	స్థిరరాశి శూన్య *prop;
	స్థిరరాశి fdt32_t *val;

	ఒకవేళ (!fdt || cpu_స్థానభ్రంశం < 0)
		తిరిగి SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "device_type", &len);
	ఒకవేళ (!prop || !len)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (అక్షరశ్రేణి_పోలిక (prop, "cpu", అక్షరశ్రేణి_పొడవు ("cpu")))
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "reg", &len);
	ఒకవేళ (!val || len < పరిమాణం(fdt32_t))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (len > పరిమాణం(fdt32_t))
		val++;

	ఒకవేళ (hartid)
		*hartid = fdt32_గమ్యం_cpu(*val);

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_cbom_ఖండం_కొలత(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం cpu_స్థానభ్రంశం, గుర్తులేని దీర్ఘ *cbom_ఖండం_కొలత)
{
	పూర్ణాంకం len;
	స్థిరరాశి శూన్య *prop;
	స్థిరరాశి fdt32_t *val;

	ఒకవేళ (!fdt || cpu_స్థానభ్రంశం < 0)
		తిరిగి SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "device_type", &len);
	ఒకవేళ (!prop || !len)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (అక్షరశ్రేణి_పోలిక (prop, "cpu", అక్షరశ్రేణి_పొడవు ("cpu")))
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "riscv,cbom-block-size", &len);
	ఒకవేళ (!val || len < పరిమాణం(fdt32_t))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (cbom_ఖండం_కొలత)
		*cbom_ఖండం_కొలత = fdt32_గమ్యం_cpu(*val);
	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_max_enabled_hart_id(స్థిరరాశి శూన్య *fdt, u32 *max_hartid)
{
	u32 hartid;
	పూర్ణాంకం err, cpu_స్థానభ్రంశం, cpus_స్థానభ్రంశం;

	ఒకవేళ (!fdt)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!max_hartid)
		తిరిగి 0;

	*max_hartid = 0;

	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి cpus_స్థానభ్రంశం;

	fdt_for_each_subnode(cpu_స్థానభ్రంశం, fdt, cpus_స్థానభ్రంశం) {
		err = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);
		ఒకవేళ (err)
			కొనసాగు;

		ఒకవేళ (!fdt_కణుపు_is_enabled(fdt, cpu_స్థానభ్రంశం))
			కొనసాగు;

		ఒకవేళ (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_timebase_frequency(స్థిరరాశి శూన్య *fdt, గుర్తులేని దీర్ఘ *freq)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం len, cpus_స్థానభ్రంశం;

	ఒకవేళ (!fdt || !freq)
		తిరిగి SBI_EINVAL;

	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి cpus_స్థానభ్రంశం;

	val = fdt_getprop(fdt, cpus_స్థానభ్రంశం, "timebase-frequency", &len);
	ఒకవేళ (len > 0 && val)
		*freq = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		తిరిగి SBI_ENOENT;

	తిరిగి 0;
}

#నిర్వచించు RISCV_ISA_EXT_NAME_LEN_MAX	32

స్థిర పూర్ణాంకం fdt_parse_isa_one_hart(స్థిరరాశి అక్షరం *isa, గుర్తులేని దీర్ఘ *extensions)
{
	size_t i, j, isa_len;
	అక్షరం mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = అక్షరశ్రేణి_పొడవు(isa);

	ఒకవేళ (isa[i] == 'r' || isa[i] == 'R')
		i++;
	లేకపోతే
		తిరిగి SBI_EINVAL;

	ఒకవేళ (isa[i] == 'v' || isa[i] == 'V')
		i++;
	లేకపోతే
		తిరిగి SBI_EINVAL;

	ఒకవేళ (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	లేకపోతే ఒకవేళ (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	లేకపోతే
		తిరిగి SBI_EINVAL;

	/* Skip base ISA extensions */
	కొరకు (; i < isa_len; i++) {
		ఒకవేళ (isa[i] == '_')
			ఆపు;
	}

	అయితే (i < isa_len) {
		ఒకవేళ (isa[i] != '_') {
			i++;
			కొనసాగు;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		అయితే ((i < isa_len) && (isa[i] != '_') &&
		       (j < (పరిమాణం(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		ఒకవేళ (!j)
			కొనసాగు;

#నిర్వచించు సమితి_multi_letter_ext(name, బిట్టు_2)				\
			ఒకవేళ (!strcmp(mstr, name)) {		\
				__set_bit(బిట్టు_2, extensions);	\
				కొనసాగు;			\
			}

		కొరకు (j = 0; j < SBI_HART_EXT_MAX; j++) {
			సమితి_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#నిర్వచనం_రద్దు సమితి_multi_letter_ext
	}

	తిరిగి 0;
}

స్థిర శూన్య fdt_parse_isa_extensions_one_hart(స్థిరరాశి అక్షరం *isa,
					      గుర్తులేని దీర్ఘ *extensions,
					      పూర్ణాంకం len)
{
	size_t i;

	కొరకు (i = 0; i < SBI_HART_EXT_MAX; i++) {
		ఒకవేళ (fdt_stringlist_కలిగియుంది(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

పూర్ణాంకం fdt_parse_isa_extensions_all_harts(స్థిరరాశి శూన్య *fdt)
{
	u32 hartid;
	స్థిరరాశి fdt32_t *val;
	నిర్మాణం sbi_scratch *scratch;
	నిర్మాణం sbi_hart_features *hfeatures;
	పూర్ణాంకం err, cpu_స్థానభ్రంశం, cpus_స్థానభ్రంశం, len;

	ఒకవేళ (!fdt)
		తిరిగి SBI_EINVAL;

	cpus_స్థానభ్రంశం = fdt_మార్గం_స్థానభ్రంశం(fdt, "/cpus");
	ఒకవేళ (cpus_స్థానభ్రంశం < 0)
		తిరిగి cpus_స్థానభ్రంశం;

	fdt_for_each_subnode(cpu_స్థానభ్రంశం, fdt, cpus_స్థానభ్రంశం) {
		err = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);
		ఒకవేళ (err)
			కొనసాగు;

		ఒకవేళ (!fdt_కణుపు_is_enabled(fdt, cpu_స్థానభ్రంశం))
			కొనసాగు;

		scratch = sbi_hartid_గమ్యం_scratch(hartid);
		ఒకవేళ (!scratch)
			తిరిగి SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		ఒకవేళ (!hfeatures)
			తిరిగి SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "riscv,isa-extensions", &len);
		ఒకవేళ (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((స్థిరరాశి అక్షరం *)val,
							  hfeatures->extensions, len);
			కొనసాగు;
		}

		val = fdt_getprop(fdt, cpu_స్థానభ్రంశం, "riscv,isa", &len);
		ఒకవేళ (!val || len <= 0)
			తిరిగి SBI_ENOENT;

		err = fdt_parse_isa_one_hart((స్థిరరాశి అక్షరం *)val, hfeatures->extensions);
		ఒకవేళ (err)
			తిరిగి err;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_parse_uart_కణుపు_common(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				      నిర్మాణం platform_uart_దత్తాంశం *uart,
				      గుర్తులేని దీర్ఘ default_freq,
				      గుర్తులేని దీర్ఘ default_baud)
{
	పూర్ణాంకం len, rc;
	స్థిరరాశి fdt32_t *val;
	uint64_t reg_addr, reg_కొలత;

	ఒకవేళ (nodeoffset < 0 || !uart || !fdt)
		తిరిగి SBI_ENODEV;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		తిరిగి SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	ఒకవేళ (len > 0 && val)
		uart->freq = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	ఒకవేళ (len > 0 && val)
		uart->baud = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		uart->baud = default_baud;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_gaisler_uart_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	తిరిగి fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

పూర్ణాంకం fdt_parse_renesas_scif_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	తిరిగి fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

పూర్ణాంకం fdt_parse_shakti_uart_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			       నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	తిరిగి fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

పూర్ణాంకం fdt_parse_sifive_uart_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			       నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	తిరిగి fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

పూర్ణాంకం fdt_parse_uart_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	పూర్ణాంకం len, rc;
	స్థిరరాశి fdt32_t *val;

	rc = fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	ఒకవేళ (rc)
		తిరిగి rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	ఒకవేళ (len > 0 && val)
		uart->reg_జరుపు = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		uart->reg_జరుపు = DEFAULT_UART_REG_జరుపు;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	ఒకవేళ (len > 0 && val)
		uart->reg_io_వెడల్పు = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		uart->reg_io_వెడల్పు = DEFAULT_UART_REG_IO_వెడల్పు;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	ఒకవేళ (len > 0 && val)
		uart->reg_స్థానభ్రంశం = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		uart->reg_స్థానభ్రంశం = DEFAULT_UART_REG_స్థానభ్రంశం;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_uart8250(స్థిరరాశి శూన్య *fdt, నిర్మాణం platform_uart_దత్తాంశం *uart,
		   స్థిరరాశి అక్షరం *compatible)
{
	పూర్ణాంకం nodeoffset;

	ఒకవేళ (!compatible || !uart || !fdt)
		తిరిగి SBI_ENODEV;

	nodeoffset = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, -1, compatible);
	ఒకవేళ (nodeoffset < 0)
		తిరిగి nodeoffset;

	తిరిగి fdt_parse_uart_కణుపు(fdt, nodeoffset, uart);
}

పూర్ణాంకం fdt_parse_xlnx_uartlite_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			       నిర్మాణం platform_uart_దత్తాంశం *uart)
{
	తిరిగి fdt_parse_uart_కణుపు_common(fdt, nodeoffset, uart, 0, 0);
}

స్థిర పూర్ణాంకం fdt_aplic_కనుగొను_imsic_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				     నిర్మాణం imsic_దత్తాంశం *imsic, bool mmode)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	ఒకవేళ (val && len >= పరిమాణం(fdt32_t)) {
		noff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, fdt32_గమ్యం_cpu(*val));
		ఒకవేళ (noff < 0)
			తిరిగి noff;

		rc = fdt_parse_imsic_కణుపు(fdt, noff, imsic);
		ఒకవేళ (rc)
			తిరిగి rc;

		rc = imsic_దత్తాంశం_check(imsic);
		ఒకవేళ (rc)
			తిరిగి rc;

		ఒకవేళ (imsic->targets_mmode == mmode) {
			తిరిగి 0;
		}
	} లేకపోతే {
		తిరిగి SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	ఒకవేళ (!val || len < పరిమాణం(fdt32_t))
		తిరిగి SBI_ENODEV;

	len /= పరిమాణం(fdt32_t);

	కొరకు (i = 0; i < len; i++) {
		noff = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, fdt32_గమ్యం_cpu(val[i]));
		ఒకవేళ (noff < 0)
			తిరిగి noff;

		rc = fdt_aplic_కనుగొను_imsic_కణుపు(fdt, noff, imsic, mmode);
		ఒకవేళ (!rc)
			ఆపు;
	}

	తిరిగి rc;
}

పూర్ణాంకం fdt_parse_aplic_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, నిర్మాణం aplic_దత్తాంశం *aplic)
{
	bool బిడ్డ_దొరికింది;
	స్థిరరాశి fdt32_t *val;
	స్థిరరాశి fdt32_t *del;
	నిర్మాణం imsic_దత్తాంశం imsic = { 0 };
	పూర్ణాంకం i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_కొలత;
	నిర్మాణం aplic_delegate_దత్తాంశం *deleg;

	ఒకవేళ (nodeoff < 0 || !aplic || !fdt)
		తిరిగి SBI_ENODEV;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		తిరిగి SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->కొలత = reg_కొలత;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	ఒకవేళ (len > 0)
		aplic->అంకె_ఆధారం = fdt32_గమ్యం_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ఒకవేళ (val && len > పరిమాణం(fdt32_t)) {
		len = len / పరిమాణం(fdt32_t);
		కొరకు (i = 0; i < len; i += 2) {
			ఒకవేళ (fdt32_గమ్యం_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				ఆపు;
			}
		}
		aplic->అంకె_idc = len / 2;
	}

	rc = fdt_aplic_కనుగొను_imsic_కణుపు(fdt, nodeoff, &imsic, true);
	ఒకవేళ (!rc) {
		aplic->మాతృ_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_సూచీ_బిట్లు;
		aplic->msicfg_mmode.lhxw = imsic.hart_సూచీ_బిట్లు;
		aplic->msicfg_mmode.hhxw = imsic.సమూహం_సూచీ_బిట్లు;
		aplic->msicfg_mmode.hhxs = imsic.సమూహం_సూచీ_జరుపు;
		ఒకవేళ (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_పుట_జరుపు))
			తిరిగి SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.పునాది_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_కనుగొను_imsic_కణుపు(fdt, nodeoff, &imsic, false);
	ఒకవేళ (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_సూచీ_బిట్లు;
		aplic->msicfg_smode.lhxw = imsic.hart_సూచీ_బిట్లు;
		aplic->msicfg_smode.hhxw = imsic.సమూహం_సూచీ_బిట్లు;
		aplic->msicfg_smode.hhxs = imsic.సమూహం_సూచీ_జరుపు;
		ఒకవేళ (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_పుట_జరుపు))
			తిరిగి SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.పునాది_addr = imsic.regs[0].addr;
	}

	కొరకు (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->మొదటి_irq = 0;
		deleg->చివరి_irq = 0;
		deleg->బిడ్డ_సూచీ = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	ఒకవేళ (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	ఒకవేళ (!del || len < (3 * పరిమాణం(fdt32_t)))
		వెళ్ళు skip_delegate_parse;
	d = 0;
	dcnt = len / పరిమాణం(fdt32_t);
	కొరకు (i = 0; i < dcnt; i += 3) {
		ఒకవేళ (d >= APLIC_MAX_DELEGATE)
			ఆపు;
		deleg = &aplic->delegate[d];

		deleg->మొదటి_irq = fdt32_గమ్యం_cpu(del[i + 1]);
		deleg->చివరి_irq = fdt32_గమ్యం_cpu(del[i + 2]);
		deleg->బిడ్డ_సూచీ = 0;

		బిడ్డ_దొరికింది = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		ఒకవేళ (!val || len < పరిమాణం(fdt32_t)) {
			deleg->మొదటి_irq = 0;
			deleg->చివరి_irq = 0;
			deleg->బిడ్డ_సూచీ = 0;
			కొనసాగు;
		}
		len = len / పరిమాణం(fdt32_t);
		కొరకు (j = 0; j < len; j++) {
			ఒకవేళ (del[i] != val[j])
				కొనసాగు;
			deleg->బిడ్డ_సూచీ = j;
			బిడ్డ_దొరికింది = true;
			ఆపు;
		}

		ఒకవేళ (బిడ్డ_దొరికింది) {
			d++;
		} లేకపోతే {
			deleg->మొదటి_irq = 0;
			deleg->చివరి_irq = 0;
			deleg->బిడ్డ_సూచీ = 0;
		}
	}
skip_delegate_parse:

	తిరిగి 0;
}

bool fdt_check_imsic_mlevel(స్థిరరాశి శూన్య *fdt)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం i, len, noff = 0;

	ఒకవేళ (!fdt)
		తిరిగి false;

	అయితే ((noff = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		ఒకవేళ (val && len > పరిమాణం(fdt32_t)) {
			len = len / పరిమాణం(fdt32_t);
			కొరకు (i = 0; i < len; i += 2) {
				ఒకవేళ (fdt32_గమ్యం_cpu(val[i + 1]) == IRQ_M_EXT)
					తిరిగి true;
			}
		}
	}

	తిరిగి false;
}

పూర్ణాంకం fdt_parse_imsic_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, నిర్మాణం imsic_దత్తాంశం *imsic)
{
	స్థిరరాశి fdt32_t *val;
	నిర్మాణం imsic_regs *regs;
	uint64_t reg_addr, reg_కొలత;
	పూర్ణాంకం i, rc, len, nr_మాతృ_irqs;

	ఒకవేళ (nodeoff < 0 || !imsic || !fdt)
		తిరిగి SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	ఒకవేళ (val && len > పరిమాణం(fdt32_t)) {
		len = len / పరిమాణం(fdt32_t);
		nr_మాతృ_irqs = len / 2;
		కొరకు (i = 0; i < len; i += 2) {
			ఒకవేళ (fdt32_గమ్యం_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				ఆపు;
			}
		}
	} లేకపోతే
		తిరిగి SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	ఒకవేళ (val && len > 0)
		imsic->guest_సూచీ_బిట్లు = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		imsic->guest_సూచీ_బిట్లు = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	ఒకవేళ (val && len > 0) {
		imsic->hart_సూచీ_బిట్లు = fdt32_గమ్యం_cpu(*val);
	} లేకపోతే {
		imsic->hart_సూచీ_బిట్లు = sbi_fls(nr_మాతృ_irqs);
		ఒకవేళ ((1UL << imsic->hart_సూచీ_బిట్లు) < nr_మాతృ_irqs)
			imsic->hart_సూచీ_బిట్లు++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	ఒకవేళ (val && len > 0)
		imsic->సమూహం_సూచీ_బిట్లు = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		imsic->సమూహం_సూచీ_బిట్లు = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	ఒకవేళ (val && len > 0)
		imsic->సమూహం_సూచీ_జరుపు = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		imsic->సమూహం_సూచీ_జరుపు = 2 * IMSIC_MMIO_పుట_జరుపు;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	ఒకవేళ (val && len > 0)
		imsic->అంకె_ids = fdt32_గమ్యం_cpu(*val);
	లేకపోతే
		తిరిగి SBI_EINVAL;

	కొరకు (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->కొలత = 0;
	}

	కొరకు (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, i,
					    &reg_addr, &reg_కొలత);
		ఒకవేళ (rc < 0 || !reg_కొలత)
			ఆపు;
		regs->addr = reg_addr;
		regs->కొలత = reg_కొలత;
	}
	ఒకవేళ (!imsic->regs[0].కొలత)
		తిరిగి SBI_EINVAL;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_plic_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, నిర్మాణం plic_దత్తాంశం *plic)
{
	పూర్ణాంకం len, rc;
	స్థిరరాశి fdt32_t *val;
	uint64_t reg_addr, reg_కొలత;

	ఒకవేళ (nodeoffset < 0 || !plic || !fdt)
		తిరిగి SBI_ENODEV;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		తిరిగి SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->కొలత = reg_కొలత;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	ఒకవేళ (len > 0)
		plic->అంకె_src = fdt32_గమ్యం_cpu(*val);

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_plic(స్థిరరాశి శూన్య *fdt, నిర్మాణం plic_దత్తాంశం *plic, స్థిరరాశి అక్షరం *compat)
{
	పూర్ణాంకం nodeoffset;

	ఒకవేళ (!compat || !plic || !fdt)
		తిరిగి SBI_ENODEV;

	nodeoffset = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, -1, compat);
	ఒకవేళ (nodeoffset < 0)
		తిరిగి nodeoffset;

	తిరిగి fdt_parse_plic_కణుపు(fdt, nodeoffset, plic);
}

స్థిర పూర్ణాంకం fdt_get_aclint_addr_కొలత_by_name(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
					    గుర్తులేని దీర్ఘ *బహిర్గతం_addr1,
					    గుర్తులేని దీర్ఘ *బహిర్గతం_size1,
					    గుర్తులేని దీర్ఘ *బహిర్గతం_addr2,
					    గుర్తులేని దీర్ఘ *బహిర్గతం_size2)
{
	పూర్ణాంకం rc;
	uint64_t reg_addr, reg_కొలత;

	rc = fdt_get_కణుపు_addr_కొలత_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		reg_addr = reg_కొలత = 0;
	*బహిర్గతం_addr1 = reg_addr;
	*బహిర్గతం_size1 = reg_కొలత;

	rc = fdt_get_కణుపు_addr_కొలత_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		తిరిగి SBI_ENODEV;
	*బహిర్గతం_addr2 = reg_addr;
	*బహిర్గతం_size2 = reg_కొలత;

	తిరిగి 0;
}

స్థిర పూర్ణాంకం fdt_get_aclint_addr_కొలత(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
				    గుర్తులేని దీర్ఘ *బహిర్గతం_addr1,
				    గుర్తులేని దీర్ఘ *బహిర్గతం_size1,
				    గుర్తులేని దీర్ఘ *బహిర్గతం_addr2,
				    గుర్తులేని దీర్ఘ *బహిర్గతం_size2)
{
	పూర్ణాంకం rc;
	uint64_t reg_addr, reg_కొలత;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		తిరిగి SBI_ENODEV;
	*బహిర్గతం_addr1 = reg_addr;
	*బహిర్గతం_size1 = reg_కొలత;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 1,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0 || !reg_కొలత)
		reg_addr = reg_కొలత = 0;
	ఒకవేళ (బహిర్గతం_addr2)
		*బహిర్గతం_addr2 = reg_addr;
	ఒకవేళ (బహిర్గతం_size2)
		*బహిర్గతం_size2 = reg_కొలత;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_aclint_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset,
			  bool for_timer, bool allow_regname,
			  గుర్తులేని దీర్ఘ *బహిర్గతం_addr1, గుర్తులేని దీర్ఘ *బహిర్గతం_size1,
			  గుర్తులేని దీర్ఘ *బహిర్గతం_addr2, గుర్తులేని దీర్ఘ *బహిర్గతం_size2,
			  u32 *బహిర్గతం_మొదటి_hartid, u32 *బహిర్గతం_hart_లెక్క)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం i, rc, లెక్క, cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;
	u32 phandle, hwirq, hartid, మొదటి_hartid, చివరి_hartid;
	u32 సరిపోలిక_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	ఒకవేళ (nodeoffset < 0 || !fdt ||
	    !బహిర్గతం_addr1 || !బహిర్గతం_size1 ||
	    !బహిర్గతం_మొదటి_hartid || !బహిర్గతం_hart_లెక్క)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (for_timer && allow_regname && బహిర్గతం_addr2 && బహిర్గతం_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", ఖాళీ))
		rc = fdt_get_aclint_addr_కొలత_by_name(fdt, nodeoffset,
						      బహిర్గతం_addr1, బహిర్గతం_size1,
						      బహిర్గతం_addr2, బహిర్గతం_size2);
	లేకపోతే
		rc = fdt_get_aclint_addr_కొలత(fdt, nodeoffset,
					      బహిర్గతం_addr1, బహిర్గతం_size1,
					      బహిర్గతం_addr2, బహిర్గతం_size2);
	ఒకవేళ (rc)
		తిరిగి rc;

	*బహిర్గతం_మొదటి_hartid = 0;
	*బహిర్గతం_hart_లెక్క = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &లెక్క);
	ఒకవేళ (!val || లెక్క < పరిమాణం(fdt32_t))
		తిరిగి 0;
	లెక్క = లెక్క / పరిమాణం(fdt32_t);

	మొదటి_hartid = -1U;
	చివరి_hartid = 0;
	కొరకు (i = 0; i < (లెక్క / 2); i++) {
		phandle = fdt32_గమ్యం_cpu(val[2 * i]);
		hwirq = fdt32_గమ్యం_cpu(val[(2 * i) + 1]);

		cpu_intc_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);
		ఒకవేళ (cpu_intc_స్థానభ్రంశం < 0)
			కొనసాగు;

		cpu_స్థానభ్రంశం = fdt_మాతృ_స్థానభ్రంశం(fdt, cpu_intc_స్థానభ్రంశం);
		ఒకవేళ (cpu_స్థానభ్రంశం < 0)
			కొనసాగు;

		rc = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);
		ఒకవేళ (rc)
			కొనసాగు;

		ఒకవేళ (సరిపోలిక_hwirq == hwirq) {
			ఒకవేళ (hartid < మొదటి_hartid)
				మొదటి_hartid = hartid;
			ఒకవేళ (hartid > చివరి_hartid)
				చివరి_hartid = hartid;
		}
	}

	ఒకవేళ ((చివరి_hartid >= మొదటి_hartid) && మొదటి_hartid != -1U) {
		*బహిర్గతం_మొదటి_hartid = మొదటి_hartid;
		*బహిర్గతం_hart_లెక్క = చివరి_hartid - మొదటి_hartid + 1;
	}
	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_plmt_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, గుర్తులేని దీర్ఘ *plmt_పునాది,
			  గుర్తులేని దీర్ఘ *plmt_కొలత, u32 *hart_లెక్క)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం rc, i, లెక్క;
	uint64_t reg_addr, reg_కొలత;
	u32 phandle, hwirq, hartid, hcount;

	ఒకవేళ (nodeoffset < 0 || !fdt || !plmt_పునాది ||
	    !hart_లెక్క || !plmt_కొలత)
		తిరిగి SBI_EINVAL;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0)
		తిరిగి SBI_ENODEV;
	*plmt_పునాది = reg_addr;
	*plmt_కొలత = reg_కొలత;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &లెక్క);
	ఒకవేళ (!val || లెక్క < పరిమాణం(fdt32_t))
		తిరిగి 0;
	లెక్క = లెక్క / పరిమాణం(fdt32_t);

	hcount = 0;
	కొరకు (i = 0; i < (లెక్క / 2); i++) {
		పూర్ణాంకం cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;

		phandle = fdt32_గమ్యం_cpu(val[2 * i]);
		hwirq = fdt32_గమ్యం_cpu(val[2 * i + 1]);

		cpu_intc_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);
		ఒకవేళ (cpu_intc_స్థానభ్రంశం < 0)
			కొనసాగు;

		cpu_స్థానభ్రంశం = fdt_మాతృ_స్థానభ్రంశం(fdt, cpu_intc_స్థానభ్రంశం);
		ఒకవేళ (cpu_స్థానభ్రంశం < 0)
			కొనసాగు;

		rc = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);

		ఒకవేళ (rc)
			కొనసాగు;

		ఒకవేళ (SBI_HARTMASK_MAX_బిట్లు <= sbi_hartid_గమ్యం_hartindex(hartid))
			కొనసాగు;

		ఒకవేళ (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_లెక్క = hcount;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_plicsw_కణుపు(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset, గుర్తులేని దీర్ఘ *plicsw_పునాది,
			  గుర్తులేని దీర్ఘ *కొలత, u32 *hart_లెక్క)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం rc, i, లెక్క;
	uint64_t reg_addr, reg_కొలత;
	u32 phandle, hwirq, hartid, hcount;

	ఒకవేళ (nodeoffset < 0 || !fdt || !plicsw_పునాది ||
	    !hart_లెక్క || !కొలత)
		తిరిగి SBI_EINVAL;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0,
				    &reg_addr, &reg_కొలత);
	ఒకవేళ (rc < 0)
		తిరిగి SBI_ENODEV;
	*plicsw_పునాది = reg_addr;
	*కొలత = reg_కొలత;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &లెక్క);
	ఒకవేళ (!val || లెక్క < పరిమాణం(fdt32_t))
		తిరిగి 0;
	లెక్క = లెక్క / పరిమాణం(fdt32_t);

	hcount = 0;
	కొరకు (i = 0; i < (లెక్క / 2); i++) {
		పూర్ణాంకం cpu_స్థానభ్రంశం, cpu_intc_స్థానభ్రంశం;

		phandle = fdt32_గమ్యం_cpu(val[2 * i]);
		hwirq = fdt32_గమ్యం_cpu(val[2 * i + 1]);

		cpu_intc_స్థానభ్రంశం = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);
		ఒకవేళ (cpu_intc_స్థానభ్రంశం < 0)
			కొనసాగు;

		cpu_స్థానభ్రంశం = fdt_మాతృ_స్థానభ్రంశం(fdt, cpu_intc_స్థానభ్రంశం);
		ఒకవేళ (cpu_స్థానభ్రంశం < 0)
			కొనసాగు;

		rc = fdt_parse_hart_id(fdt, cpu_స్థానభ్రంశం, &hartid);

		ఒకవేళ (rc)
			కొనసాగు;

		ఒకవేళ (SBI_HARTMASK_MAX_బిట్లు <= sbi_hartid_గమ్యం_hartindex(hartid))
			కొనసాగు;

		ఒకవేళ (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_లెక్క = hcount;

	తిరిగి 0;
}

పూర్ణాంకం fdt_parse_compat_addr(స్థిరరాశి శూన్య *fdt, uint64_t *addr,
			  స్థిరరాశి అక్షరం *compatible)
{
	పూర్ణాంకం nodeoffset, rc;

	nodeoffset = fdt_కణుపు_స్థానభ్రంశం_by_compatible(fdt, -1, compatible);
	ఒకవేళ (nodeoffset < 0)
		తిరిగి nodeoffset;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoffset, 0, addr, ఖాళీ);
	ఒకవేళ (rc < 0 || !addr)
		తిరిగి SBI_ENODEV;

	తిరిగి 0;
}
