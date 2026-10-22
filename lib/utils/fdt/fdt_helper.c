// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_hartmask.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/irqchip/aplic.h>
#ضمن <sbi_utils/irqchip/imsic.h>
#ضمن <sbi_utils/irqchip/plic.h>

#تعریف_کړه DEFAULT_UART_FREQ		0
#تعریف_کړه DEFAULT_UART_BAUD		115200
#تعریف_کړه DEFAULT_UART_REG_ښويدنه		0
#تعریف_کړه DEFAULT_UART_REG_IO_پلنوالی	1
#تعریف_کړه DEFAULT_UART_REG_دننه_واټن		0

#تعریف_کړه DEFAULT_RENESAS_SCIF_FREQ		100000000
#تعریف_کړه DEFAULT_RENESAS_SCIF_BAUD		115200

#تعریف_کړه DEFAULT_SIFIVE_UART_FREQ		0
#تعریف_کړه DEFAULT_SIFIVE_UART_BAUD		115200

#تعریف_کړه DEFAULT_SHAKTI_UART_FREQ		50000000
#تعریف_کړه DEFAULT_SHAKTI_UART_BAUD		115200

صحيح fdt_parse_phandle_with_دلیلونه(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحيح شاخص_2, بنية fdt_phandle_دلیلونه *بهر_دلیلونه)
{
	u32 i, pcells;
	صحيح len, pnodeoff;
	ثابت fdt32_t *list, *list_پای_ټکی, *val;

	إذا (!fdt || (nodeoff < 0) || !prop || !cells_prop || !بهر_دلیلونه)
		أرجع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	إذا (!list)
		أرجع SBI_ENOENT;
	list_پای_ټکی = list + (len / حجم(*list));

	طالما (list < list_پای_ټکی) {
		pnodeoff = fdt_غوټه_دننه_واټن_by_phandle(fdt,
						fdt32_منزل_cpu(*list));
		إذا (pnodeoff < 0)
			أرجع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		إذا (!val)
			أرجع SBI_ENOENT;
		pcells = fdt32_منزل_cpu(*val);
		إذا (FDT_MAX_PHANDLE_دلیلونه < pcells)
			أرجع SBI_EINVAL;
		إذا (list + pcells > list_پای_ټکی)
			أرجع SBI_ENOENT;

		إذا (شاخص_2 > 0) {
			list += pcells;
			شاخص_2--;
		} وإلا {
			بهر_دلیلونه->غوټه_دننه_واټن = pnodeoff;
			بهر_دلیلونه->دلیلونه_شمېر = pcells;
			لكل (i = 0; i < pcells; i++)
				بهر_دلیلونه->دلیلونه[i] = fdt32_منزل_cpu(list[i]);
			أرجع 0;
		}
	}

	أرجع SBI_ENOENT;
}

ساكن صحيح fdt_translate_پته(ثابت فراغ *fdt, uint64_t reg, صحيح پلار,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_پلار_addr, cell_ماشوم_addr, cell_اندازه;
	ثابت fdt32_t *ranges;
	uint64_t دننه_واټن, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, پلار, "ranges", &rlen);
	إذا (ranges && rlen > 0) {
		cell_ماشوم_addr = fdt_پته_cells(fdt, پلار);
		إذا (cell_ماشوم_addr < 1)
			أرجع SBI_ENODEV;

		cell_پلار_addr = fdt_پته_cells(fdt, fdt_پلار_دننه_واټن(fdt, پلار));
		إذا (cell_پلار_addr < 1)
			أرجع SBI_ENODEV;

		cell_اندازه = fdt_اندازه_cells(fdt, پلار);
		إذا (cell_اندازه < 0)
			أرجع SBI_ENODEV;

		لكل (i = 0; i < cell_ماشوم_addr; i++)
			caddr = (caddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_پلار_addr; i++)
			paddr = (paddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_اندازه; i++)
			rsize = (rsize << 32) | fdt32_منزل_cpu(*ranges++);
		إذا (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			أرجع SBI_ENODEV;
		}
		دننه_واټن = reg - caddr;
		*addr = paddr + دننه_واټن;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	أرجع 0;
}

صحيح fdt_get_غوټه_addr_اندازه(ثابت فراغ *fdt, صحيح غوټه, صحيح شاخص_2,
			   uint64_t *addr, uint64_t *اندازه)
{
	صحيح پلار, len, i, rc;
	صحيح cell_addr, cell_اندازه;
	ثابت fdt32_t *prop_addr, *prop_اندازه;
	uint64_t لنډمهاله = 0;

	إذا (!fdt || غوټه < 0 || شاخص_2 < 0)
		أرجع SBI_EINVAL;

	پلار = fdt_پلار_دننه_واټن(fdt, غوټه);
	إذا (پلار < 0)
		أرجع پلار;
	cell_addr = fdt_پته_cells(fdt, پلار);
	إذا (cell_addr < 1)
		أرجع SBI_ENODEV;

	cell_اندازه = fdt_اندازه_cells(fdt, پلار);
	إذا (cell_اندازه < 0)
		أرجع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, غوټه, "reg", &len);
	إذا (!prop_addr)
		أرجع SBI_ENODEV;

	إذا ((len / حجم(u32)) <= (شاخص_2 * (cell_addr + cell_اندازه)))
		أرجع SBI_EINVAL;

	prop_addr = prop_addr + (شاخص_2 * (cell_addr + cell_اندازه));
	prop_اندازه = prop_addr + cell_addr;

	إذا (addr) {
		لكل (i = 0; i < cell_addr; i++)
			لنډمهاله = (لنډمهاله << 32) | fdt32_منزل_cpu(*prop_addr++);
		افعل {
			إذا (پلار < 0)
				اكسر;
			rc  = fdt_translate_پته(fdt, لنډمهاله, پلار, addr);
			إذا (rc)
				اكسر;
			پلار = fdt_پلار_دننه_واټن(fdt, پلار);
			لنډمهاله = *addr;
		} طالما (1);
	}
	لنډمهاله = 0;

	إذا (اندازه) {
		لكل (i = 0; i < cell_اندازه; i++)
			لنډمهاله = (لنډمهاله << 32) | fdt32_منزل_cpu(*prop_اندازه++);
		*اندازه = لنډمهاله;
	}

	أرجع 0;
}

صحيح fdt_get_غوټه_addr_اندازه_by_name(ثابت فراغ *fdt, صحيح غوټه, ثابت حرف *name,
				   uint64_t *addr, uint64_t *اندازه)
{
	صحيح i, j, شمېر;
	ثابت حرف *val;
	ثابت حرف *regname;

	إذا (!fdt || غوټه < 0 || !name)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, غوټه, "reg-names", &شمېر);
	إذا (!val)
		أرجع SBI_ENODEV;

	لكل (i = 0, j = 0; i < شمېر; i++, j++) {
		regname = val + i;

		إذا (strcmp(name, regname) == 0)
			أرجع fdt_get_غوټه_addr_اندازه(fdt, غوټه, j, addr, اندازه);

		i += مزي_اوږدوالی(regname);
	}

	أرجع SBI_ENODEV;
}

bool fdt_غوټه_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	إذا (!prop)
		أرجع true;

	إذا (!مزي_پرتله(prop, "okay", مزي_اوږدوالی("okay")))
		أرجع true;

	إذا (!مزي_پرتله(prop, "ok", مزي_اوږدوالی("ok")))
		أرجع true;

	أرجع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_دننه_واټن, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_دننه_واټن < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_دننه_واټن, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (مزي_پرتله (prop, "cpu", مزي_اوږدوالی ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_دننه_واټن, "reg", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (len > حجم(fdt32_t))
		val++;

	إذا (hartid)
		*hartid = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_cbom_بلاک_اندازه(ثابت فراغ *fdt, صحيح cpu_دننه_واټن, بینښه اوږد *cbom_بلاک_اندازه)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_دننه_واټن < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_دننه_واټن, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (مزي_پرتله (prop, "cpu", مزي_اوږدوالی ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_دننه_واټن, "riscv,cbom-block-size", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (cbom_بلاک_اندازه)
		*cbom_بلاک_اندازه = fdt32_منزل_cpu(*val);
	أرجع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_دننه_واټن, cpus_دننه_واټن;

	إذا (!fdt)
		أرجع SBI_EINVAL;
	إذا (!max_hartid)
		أرجع 0;

	*max_hartid = 0;

	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع cpus_دننه_واټن;

	fdt_for_each_subnode(cpu_دننه_واټن, fdt, cpus_دننه_واټن) {
		err = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_غوټه_is_enabled(fdt, cpu_دننه_واټن))
			استمر;

		إذا (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	أرجع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, بینښه اوږد *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_دننه_واټن;

	إذا (!fdt || !freq)
		أرجع SBI_EINVAL;

	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع cpus_دننه_واټن;

	val = fdt_getprop(fdt, cpus_دننه_واټن, "timebase-frequency", &len);
	إذا (len > 0 && val)
		*freq = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_ENOENT;

	أرجع 0;
}

#تعریف_کړه RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, بینښه اوږد *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = مزي_اوږدوالی(isa);

	إذا (isa[i] == 'r' || isa[i] == 'R')
		i++;
	وإلا
		أرجع SBI_EINVAL;

	إذا (isa[i] == 'v' || isa[i] == 'V')
		i++;
	وإلا
		أرجع SBI_EINVAL;

	إذا (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	وإلا إذا (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	وإلا
		أرجع SBI_EINVAL;

	/* Skip base ISA extensions */
	لكل (; i < isa_len; i++) {
		إذا (isa[i] == '_')
			اكسر;
	}

	طالما (i < isa_len) {
		إذا (isa[i] != '_') {
			i++;
			استمر;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		طالما ((i < isa_len) && (isa[i] != '_') &&
		       (j < (حجم(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		إذا (!j)
			استمر;

#تعریف_کړه سېټ_multi_letter_ext(name, بټ_2)				\
			إذا (!strcmp(mstr, name)) {		\
				__set_bit(بټ_2, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			سېټ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#تعریف_لغوه سېټ_multi_letter_ext
	}

	أرجع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      بینښه اوږد *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		إذا (fdt_stringlist_لري(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحيح fdt_parse_isa_extensions_all_harts(ثابت فراغ *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	بنية sbi_scratch *scratch;
	بنية sbi_hart_features *hfeatures;
	صحيح err, cpu_دننه_واټن, cpus_دننه_واټن, len;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	cpus_دننه_واټن = fdt_لار_دننه_واټن(fdt, "/cpus");
	إذا (cpus_دننه_واټن < 0)
		أرجع cpus_دننه_واټن;

	fdt_for_each_subnode(cpu_دننه_واټن, fdt, cpus_دننه_واټن) {
		err = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_غوټه_is_enabled(fdt, cpu_دننه_واټن))
			استمر;

		scratch = sbi_hartid_منزل_scratch(hartid);
		إذا (!scratch)
			أرجع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		إذا (!hfeatures)
			أرجع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_دننه_واټن, "riscv,isa-extensions", &len);
		إذا (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_دننه_واټن, "riscv,isa", &len);
		إذا (!val || len <= 0)
			أرجع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		إذا (err)
			أرجع err;
	}

	أرجع 0;
}

ساكن صحيح fdt_parse_uart_غوټه_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_معلومات *uart,
				      بینښه اوږد default_freq,
				      بینښه اوږد default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_اندازه;

	إذا (nodeoffset < 0 || !uart || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	إذا (len > 0 && val)
		uart->freq = fdt32_منزل_cpu(*val);
	وإلا
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	إذا (len > 0 && val)
		uart->baud = fdt32_منزل_cpu(*val);
	وإلا
		uart->baud = default_baud;

	أرجع 0;
}

صحيح fdt_parse_gaisler_uart_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_معلومات *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	إذا (rc)
		أرجع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	إذا (len > 0 && val)
		uart->reg_ښويدنه = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_ښويدنه = DEFAULT_UART_REG_ښويدنه;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	إذا (len > 0 && val)
		uart->reg_io_پلنوالی = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_io_پلنوالی = DEFAULT_UART_REG_IO_پلنوالی;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	إذا (len > 0 && val)
		uart->reg_دننه_واټن = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_دننه_واټن = DEFAULT_UART_REG_دننه_واټن;

	أرجع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_معلومات *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	إذا (!compatible || !uart || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_غوټه_دننه_واټن_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_uart_غوټه(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_غوټه_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_پیدا_کړه_imsic_غوټه(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_معلومات *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	إذا (val && len >= حجم(fdt32_t)) {
		noff = fdt_غوټه_دننه_واټن_by_phandle(fdt, fdt32_منزل_cpu(*val));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_parse_imsic_غوټه(fdt, noff, imsic);
		إذا (rc)
			أرجع rc;

		rc = imsic_معلومات_check(imsic);
		إذا (rc)
			أرجع rc;

		إذا (imsic->targets_mmode == mmode) {
			أرجع 0;
		}
	} وإلا {
		أرجع SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_ENODEV;

	len /= حجم(fdt32_t);

	لكل (i = 0; i < len; i++) {
		noff = fdt_غوټه_دننه_واټن_by_phandle(fdt, fdt32_منزل_cpu(val[i]));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_aplic_پیدا_کړه_imsic_غوټه(fdt, noff, imsic, mmode);
		إذا (!rc)
			اكسر;
	}

	أرجع rc;
}

صحيح fdt_parse_aplic_غوټه(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_معلومات *aplic)
{
	bool ماشوم_وموندل;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_معلومات imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_اندازه;
	بنية aplic_delegate_معلومات *deleg;

	إذا (nodeoff < 0 || !aplic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	إذا (len > 0)
		aplic->عدد_سرچینه = fdt32_منزل_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_منزل_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->عدد_idc = len / 2;
	}

	rc = fdt_aplic_پیدا_کړه_imsic_غوټه(fdt, nodeoff, &imsic, true);
	إذا (!rc) {
		aplic->پلار_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_شاخص_بټونه;
		aplic->msicfg_mmode.lhxw = imsic.hart_شاخص_بټونه;
		aplic->msicfg_mmode.hhxw = imsic.ډله_شاخص_بټونه;
		aplic->msicfg_mmode.hhxs = imsic.ډله_شاخص_ښويدنه;
		إذا (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_پاڼه_ښويدنه))
			أرجع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.بنسټ_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_پیدا_کړه_imsic_غوټه(fdt, nodeoff, &imsic, false);
	إذا (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_شاخص_بټونه;
		aplic->msicfg_smode.lhxw = imsic.hart_شاخص_بټونه;
		aplic->msicfg_smode.hhxw = imsic.ډله_شاخص_بټونه;
		aplic->msicfg_smode.hhxs = imsic.ډله_شاخص_ښويدنه;
		إذا (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_پاڼه_ښويدنه))
			أرجع SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.بنسټ_addr = imsic.regs[0].addr;
	}

	لكل (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->لومړی_irq = 0;
		deleg->وروستی_irq = 0;
		deleg->ماشوم_شاخص = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	إذا (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	إذا (!del || len < (3 * حجم(fdt32_t)))
		اذهب skip_delegate_parse;
	d = 0;
	dcnt = len / حجم(fdt32_t);
	لكل (i = 0; i < dcnt; i += 3) {
		إذا (d >= APLIC_MAX_DELEGATE)
			اكسر;
		deleg = &aplic->delegate[d];

		deleg->لومړی_irq = fdt32_منزل_cpu(del[i + 1]);
		deleg->وروستی_irq = fdt32_منزل_cpu(del[i + 2]);
		deleg->ماشوم_شاخص = 0;

		ماشوم_وموندل = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		إذا (!val || len < حجم(fdt32_t)) {
			deleg->لومړی_irq = 0;
			deleg->وروستی_irq = 0;
			deleg->ماشوم_شاخص = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			إذا (del[i] != val[j])
				استمر;
			deleg->ماشوم_شاخص = j;
			ماشوم_وموندل = true;
			اكسر;
		}

		إذا (ماشوم_وموندل) {
			d++;
		} وإلا {
			deleg->لومړی_irq = 0;
			deleg->وروستی_irq = 0;
			deleg->ماشوم_شاخص = 0;
		}
	}
skip_delegate_parse:

	أرجع 0;
}

bool fdt_check_imsic_mlevel(ثابت فراغ *fdt)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff = 0;

	إذا (!fdt)
		أرجع false;

	طالما ((noff = fdt_غوټه_دننه_واټن_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		إذا (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			لكل (i = 0; i < len; i += 2) {
				إذا (fdt32_منزل_cpu(val[i + 1]) == IRQ_M_EXT)
					أرجع true;
			}
		}
	}

	أرجع false;
}

صحيح fdt_parse_imsic_غوټه(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_معلومات *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_اندازه;
	صحيح i, rc, len, nr_پلار_irqs;

	إذا (nodeoff < 0 || !imsic || !fdt)
		أرجع SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_پلار_irqs = len / 2;
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_منزل_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	إذا (val && len > 0)
		imsic->guest_شاخص_بټونه = fdt32_منزل_cpu(*val);
	وإلا
		imsic->guest_شاخص_بټونه = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	إذا (val && len > 0) {
		imsic->hart_شاخص_بټونه = fdt32_منزل_cpu(*val);
	} وإلا {
		imsic->hart_شاخص_بټونه = sbi_fls(nr_پلار_irqs);
		إذا ((1UL << imsic->hart_شاخص_بټونه) < nr_پلار_irqs)
			imsic->hart_شاخص_بټونه++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	إذا (val && len > 0)
		imsic->ډله_شاخص_بټونه = fdt32_منزل_cpu(*val);
	وإلا
		imsic->ډله_شاخص_بټونه = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	إذا (val && len > 0)
		imsic->ډله_شاخص_ښويدنه = fdt32_منزل_cpu(*val);
	وإلا
		imsic->ډله_شاخص_ښويدنه = 2 * IMSIC_MMIO_پاڼه_ښويدنه;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	إذا (val && len > 0)
		imsic->عدد_ids = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->اندازه = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoff, i,
					    &reg_addr, &reg_اندازه);
		إذا (rc < 0 || !reg_اندازه)
			اكسر;
		regs->addr = reg_addr;
		regs->اندازه = reg_اندازه;
	}
	إذا (!imsic->regs[0].اندازه)
		أرجع SBI_EINVAL;

	أرجع 0;
}

صحيح fdt_parse_plic_غوټه(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_معلومات *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_اندازه;

	إذا (nodeoffset < 0 || !plic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	إذا (len > 0)
		plic->عدد_src = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_معلومات *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	إذا (!compat || !plic || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_غوټه_دننه_واټن_by_compatible(fdt, -1, compat);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_plic_غوټه(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_اندازه_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    بینښه اوږد *بهر_addr1,
					    بینښه اوږد *بهر_size1,
					    بینښه اوږد *بهر_addr2,
					    بینښه اوږد *بهر_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;

	rc = fdt_get_غوټه_addr_اندازه_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		reg_addr = reg_اندازه = 0;
	*بهر_addr1 = reg_addr;
	*بهر_size1 = reg_اندازه;

	rc = fdt_get_غوټه_addr_اندازه_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	*بهر_addr2 = reg_addr;
	*بهر_size2 = reg_اندازه;

	أرجع 0;
}

ساكن صحيح fdt_get_aclint_addr_اندازه(ثابت فراغ *fdt, صحيح nodeoffset,
				    بینښه اوږد *بهر_addr1,
				    بینښه اوږد *بهر_size1,
				    بینښه اوږد *بهر_addr2,
				    بینښه اوږد *بهر_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	*بهر_addr1 = reg_addr;
	*بهر_size1 = reg_اندازه;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 1,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		reg_addr = reg_اندازه = 0;
	إذا (بهر_addr2)
		*بهر_addr2 = reg_addr;
	إذا (بهر_size2)
		*بهر_size2 = reg_اندازه;

	أرجع 0;
}

صحيح fdt_parse_aclint_غوټه(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  بینښه اوږد *بهر_addr1, بینښه اوږد *بهر_size1,
			  بینښه اوږد *بهر_addr2, بینښه اوږد *بهر_size2,
			  u32 *بهر_لومړی_hartid, u32 *بهر_hart_شمېر)
{
	ثابت fdt32_t *val;
	صحيح i, rc, شمېر, cpu_دننه_واټن, cpu_intc_دننه_واټن;
	u32 phandle, hwirq, hartid, لومړی_hartid, وروستی_hartid;
	u32 سمون_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	إذا (nodeoffset < 0 || !fdt ||
	    !بهر_addr1 || !بهر_size1 ||
	    !بهر_لومړی_hartid || !بهر_hart_شمېر)
		أرجع SBI_EINVAL;

	إذا (for_timer && allow_regname && بهر_addr2 && بهر_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_اندازه_by_name(fdt, nodeoffset,
						      بهر_addr1, بهر_size1,
						      بهر_addr2, بهر_size2);
	وإلا
		rc = fdt_get_aclint_addr_اندازه(fdt, nodeoffset,
					      بهر_addr1, بهر_size1,
					      بهر_addr2, بهر_size2);
	إذا (rc)
		أرجع rc;

	*بهر_لومړی_hartid = 0;
	*بهر_hart_شمېر = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع 0;
	شمېر = شمېر / حجم(fdt32_t);

	لومړی_hartid = -1U;
	وروستی_hartid = 0;
	لكل (i = 0; i < (شمېر / 2); i++) {
		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[(2 * i) + 1]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);
		إذا (rc)
			استمر;

		إذا (سمون_hwirq == hwirq) {
			إذا (hartid < لومړی_hartid)
				لومړی_hartid = hartid;
			إذا (hartid > وروستی_hartid)
				وروستی_hartid = hartid;
		}
	}

	إذا ((وروستی_hartid >= لومړی_hartid) && لومړی_hartid != -1U) {
		*بهر_لومړی_hartid = لومړی_hartid;
		*بهر_hart_شمېر = وروستی_hartid - لومړی_hartid + 1;
	}
	أرجع 0;
}

صحيح fdt_parse_plmt_غوټه(ثابت فراغ *fdt, صحيح nodeoffset, بینښه اوږد *plmt_بنسټ,
			  بینښه اوږد *plmt_اندازه, u32 *hart_شمېر)
{
	ثابت fdt32_t *val;
	صحيح rc, i, شمېر;
	uint64_t reg_addr, reg_اندازه;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plmt_بنسټ ||
	    !hart_شمېر || !plmt_اندازه)
		أرجع SBI_EINVAL;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plmt_بنسټ = reg_addr;
	*plmt_اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع 0;
	شمېر = شمېر / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (شمېر / 2); i++) {
		صحيح cpu_دننه_واټن, cpu_intc_دننه_واټن;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بټونه <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_شمېر = hcount;

	أرجع 0;
}

صحيح fdt_parse_plicsw_غوټه(ثابت فراغ *fdt, صحيح nodeoffset, بینښه اوږد *plicsw_بنسټ,
			  بینښه اوږد *اندازه, u32 *hart_شمېر)
{
	ثابت fdt32_t *val;
	صحيح rc, i, شمېر;
	uint64_t reg_addr, reg_اندازه;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plicsw_بنسټ ||
	    !hart_شمېر || !اندازه)
		أرجع SBI_EINVAL;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plicsw_بنسټ = reg_addr;
	*اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمېر);
	إذا (!val || شمېر < حجم(fdt32_t))
		أرجع 0;
	شمېر = شمېر / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (شمېر / 2); i++) {
		صحيح cpu_دننه_واټن, cpu_intc_دننه_واټن;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_دننه_واټن = fdt_غوټه_دننه_واټن_by_phandle(fdt, phandle);
		إذا (cpu_intc_دننه_واټن < 0)
			استمر;

		cpu_دننه_واټن = fdt_پلار_دننه_واټن(fdt, cpu_intc_دننه_واټن);
		إذا (cpu_دننه_واټن < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_دننه_واټن, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بټونه <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_شمېر = hcount;

	أرجع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_غوټه_دننه_واټن_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	rc = fdt_get_غوټه_addr_اندازه(fdt, nodeoffset, 0, addr, عدم);
	إذا (rc < 0 || !addr)
		أرجع SBI_ENODEV;

	أرجع 0;
}
