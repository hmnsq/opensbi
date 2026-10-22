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

#تعریف_کن DEFAULT_UART_FREQ		0
#تعریف_کن DEFAULT_UART_BAUD		115200
#تعریف_کن DEFAULT_UART_REG_لغزش		0
#تعریف_کن DEFAULT_UART_REG_IO_عرض	1
#تعریف_کن DEFAULT_UART_REG_جابه_جایی		0

#تعریف_کن DEFAULT_RENESAS_SCIF_FREQ		100000000
#تعریف_کن DEFAULT_RENESAS_SCIF_BAUD		115200

#تعریف_کن DEFAULT_SIFIVE_UART_FREQ		0
#تعریف_کن DEFAULT_SIFIVE_UART_BAUD		115200

#تعریف_کن DEFAULT_SHAKTI_UART_FREQ		50000000
#تعریف_کن DEFAULT_SHAKTI_UART_BAUD		115200

صحيح fdt_parse_phandle_with_آرگومانها(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحيح شاخص_2, بنية fdt_phandle_آرگومانها *بیرون_آرگومانها)
{
	u32 i, pcells;
	صحيح len, pnodeoff;
	ثابت fdt32_t *list, *list_انتها, *val;

	إذا (!fdt || (nodeoff < 0) || !prop || !cells_prop || !بیرون_آرگومانها)
		أرجع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	إذا (!list)
		أرجع SBI_ENOENT;
	list_انتها = list + (len / حجم(*list));

	طالما (list < list_انتها) {
		pnodeoff = fdt_گره_جابه_جایی_by_phandle(fdt,
						fdt32_مقصد_cpu(*list));
		إذا (pnodeoff < 0)
			أرجع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		إذا (!val)
			أرجع SBI_ENOENT;
		pcells = fdt32_مقصد_cpu(*val);
		إذا (FDT_MAX_PHANDLE_آرگومانها < pcells)
			أرجع SBI_EINVAL;
		إذا (list + pcells > list_انتها)
			أرجع SBI_ENOENT;

		إذا (شاخص_2 > 0) {
			list += pcells;
			شاخص_2--;
		} وإلا {
			بیرون_آرگومانها->گره_جابه_جایی = pnodeoff;
			بیرون_آرگومانها->آرگومانها_شمار = pcells;
			لكل (i = 0; i < pcells; i++)
				بیرون_آرگومانها->آرگومانها[i] = fdt32_مقصد_cpu(list[i]);
			أرجع 0;
		}
	}

	أرجع SBI_ENOENT;
}

ساكن صحيح fdt_translate_آدرس(ثابت فراغ *fdt, uint64_t reg, صحيح والد,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_والد_addr, cell_فرزند_addr, cell_اندازه;
	ثابت fdt32_t *ranges;
	uint64_t جابه_جایی, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, والد, "ranges", &rlen);
	إذا (ranges && rlen > 0) {
		cell_فرزند_addr = fdt_آدرس_cells(fdt, والد);
		إذا (cell_فرزند_addr < 1)
			أرجع SBI_ENODEV;

		cell_والد_addr = fdt_آدرس_cells(fdt, fdt_والد_جابه_جایی(fdt, والد));
		إذا (cell_والد_addr < 1)
			أرجع SBI_ENODEV;

		cell_اندازه = fdt_اندازه_cells(fdt, والد);
		إذا (cell_اندازه < 0)
			أرجع SBI_ENODEV;

		لكل (i = 0; i < cell_فرزند_addr; i++)
			caddr = (caddr << 32) | fdt32_مقصد_cpu(*ranges++);
		لكل (i = 0; i < cell_والد_addr; i++)
			paddr = (paddr << 32) | fdt32_مقصد_cpu(*ranges++);
		لكل (i = 0; i < cell_اندازه; i++)
			rsize = (rsize << 32) | fdt32_مقصد_cpu(*ranges++);
		إذا (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			أرجع SBI_ENODEV;
		}
		جابه_جایی = reg - caddr;
		*addr = paddr + جابه_جایی;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	أرجع 0;
}

صحيح fdt_get_گره_addr_اندازه(ثابت فراغ *fdt, صحيح گره, صحيح شاخص_2,
			   uint64_t *addr, uint64_t *اندازه)
{
	صحيح والد, len, i, rc;
	صحيح cell_addr, cell_اندازه;
	ثابت fdt32_t *prop_addr, *prop_اندازه;
	uint64_t موقت = 0;

	إذا (!fdt || گره < 0 || شاخص_2 < 0)
		أرجع SBI_EINVAL;

	والد = fdt_والد_جابه_جایی(fdt, گره);
	إذا (والد < 0)
		أرجع والد;
	cell_addr = fdt_آدرس_cells(fdt, والد);
	إذا (cell_addr < 1)
		أرجع SBI_ENODEV;

	cell_اندازه = fdt_اندازه_cells(fdt, والد);
	إذا (cell_اندازه < 0)
		أرجع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, گره, "reg", &len);
	إذا (!prop_addr)
		أرجع SBI_ENODEV;

	إذا ((len / حجم(u32)) <= (شاخص_2 * (cell_addr + cell_اندازه)))
		أرجع SBI_EINVAL;

	prop_addr = prop_addr + (شاخص_2 * (cell_addr + cell_اندازه));
	prop_اندازه = prop_addr + cell_addr;

	إذا (addr) {
		لكل (i = 0; i < cell_addr; i++)
			موقت = (موقت << 32) | fdt32_مقصد_cpu(*prop_addr++);
		افعل {
			إذا (والد < 0)
				اكسر;
			rc  = fdt_translate_آدرس(fdt, موقت, والد, addr);
			إذا (rc)
				اكسر;
			والد = fdt_والد_جابه_جایی(fdt, والد);
			موقت = *addr;
		} طالما (1);
	}
	موقت = 0;

	إذا (اندازه) {
		لكل (i = 0; i < cell_اندازه; i++)
			موقت = (موقت << 32) | fdt32_مقصد_cpu(*prop_اندازه++);
		*اندازه = موقت;
	}

	أرجع 0;
}

صحيح fdt_get_گره_addr_اندازه_by_name(ثابت فراغ *fdt, صحيح گره, ثابت حرف *name,
				   uint64_t *addr, uint64_t *اندازه)
{
	صحيح i, j, شمار;
	ثابت حرف *val;
	ثابت حرف *regname;

	إذا (!fdt || گره < 0 || !name)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, گره, "reg-names", &شمار);
	إذا (!val)
		أرجع SBI_ENODEV;

	لكل (i = 0, j = 0; i < شمار; i++, j++) {
		regname = val + i;

		إذا (strcmp(name, regname) == 0)
			أرجع fdt_get_گره_addr_اندازه(fdt, گره, j, addr, اندازه);

		i += طول_رشته(regname);
	}

	أرجع SBI_ENODEV;
}

bool fdt_گره_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	إذا (!prop)
		أرجع true;

	إذا (!مقایسه_رشته(prop, "okay", طول_رشته("okay")))
		أرجع true;

	إذا (!مقایسه_رشته(prop, "ok", طول_رشته("ok")))
		أرجع true;

	أرجع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_جابه_جایی, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_جابه_جایی < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_جابه_جایی, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (مقایسه_رشته (prop, "cpu", طول_رشته ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_جابه_جایی, "reg", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (len > حجم(fdt32_t))
		val++;

	إذا (hartid)
		*hartid = fdt32_مقصد_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_cbom_بلوک_اندازه(ثابت فراغ *fdt, صحيح cpu_جابه_جایی, بیعلامت دراز *cbom_بلوک_اندازه)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_جابه_جایی < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_جابه_جایی, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (مقایسه_رشته (prop, "cpu", طول_رشته ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_جابه_جایی, "riscv,cbom-block-size", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (cbom_بلوک_اندازه)
		*cbom_بلوک_اندازه = fdt32_مقصد_cpu(*val);
	أرجع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_جابه_جایی, cpus_جابه_جایی;

	إذا (!fdt)
		أرجع SBI_EINVAL;
	إذا (!max_hartid)
		أرجع 0;

	*max_hartid = 0;

	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع cpus_جابه_جایی;

	fdt_for_each_subnode(cpu_جابه_جایی, fdt, cpus_جابه_جایی) {
		err = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_گره_is_enabled(fdt, cpu_جابه_جایی))
			استمر;

		إذا (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	أرجع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, بیعلامت دراز *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_جابه_جایی;

	إذا (!fdt || !freq)
		أرجع SBI_EINVAL;

	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع cpus_جابه_جایی;

	val = fdt_getprop(fdt, cpus_جابه_جایی, "timebase-frequency", &len);
	إذا (len > 0 && val)
		*freq = fdt32_مقصد_cpu(*val);
	وإلا
		أرجع SBI_ENOENT;

	أرجع 0;
}

#تعریف_کن RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, بیعلامت دراز *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = طول_رشته(isa);

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

#تعریف_کن مجموعه_multi_letter_ext(name, بیت_2)				\
			إذا (!strcmp(mstr, name)) {		\
				__set_bit(بیت_2, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			مجموعه_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#لغو_تعریف مجموعه_multi_letter_ext
	}

	أرجع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      بیعلامت دراز *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		إذا (fdt_stringlist_شامل_است(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحيح fdt_parse_isa_extensions_all_harts(ثابت فراغ *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	بنية sbi_scratch *scratch;
	بنية sbi_hart_features *hfeatures;
	صحيح err, cpu_جابه_جایی, cpus_جابه_جایی, len;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	cpus_جابه_جایی = fdt_مسیر_جابه_جایی(fdt, "/cpus");
	إذا (cpus_جابه_جایی < 0)
		أرجع cpus_جابه_جایی;

	fdt_for_each_subnode(cpu_جابه_جایی, fdt, cpus_جابه_جایی) {
		err = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_گره_is_enabled(fdt, cpu_جابه_جایی))
			استمر;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		إذا (!scratch)
			أرجع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		إذا (!hfeatures)
			أرجع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_جابه_جایی, "riscv,isa-extensions", &len);
		إذا (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_جابه_جایی, "riscv,isa", &len);
		إذا (!val || len <= 0)
			أرجع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		إذا (err)
			أرجع err;
	}

	أرجع 0;
}

ساكن صحيح fdt_parse_uart_گره_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_داده *uart,
				      بیعلامت دراز default_freq,
				      بیعلامت دراز default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_اندازه;

	إذا (nodeoffset < 0 || !uart || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0,
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
		uart->freq = fdt32_مقصد_cpu(*val);
	وإلا
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	إذا (len > 0 && val)
		uart->baud = fdt32_مقصد_cpu(*val);
	وإلا
		uart->baud = default_baud;

	أرجع 0;
}

صحيح fdt_parse_gaisler_uart_گره(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_داده *uart)
{
	أرجع fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_گره(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_داده *uart)
{
	أرجع fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_گره(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_داده *uart)
{
	أرجع fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_گره(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_داده *uart)
{
	أرجع fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_گره(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_داده *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_گره_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	إذا (rc)
		أرجع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	إذا (len > 0 && val)
		uart->reg_لغزش = fdt32_مقصد_cpu(*val);
	وإلا
		uart->reg_لغزش = DEFAULT_UART_REG_لغزش;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	إذا (len > 0 && val)
		uart->reg_io_عرض = fdt32_مقصد_cpu(*val);
	وإلا
		uart->reg_io_عرض = DEFAULT_UART_REG_IO_عرض;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	إذا (len > 0 && val)
		uart->reg_جابه_جایی = fdt32_مقصد_cpu(*val);
	وإلا
		uart->reg_جابه_جایی = DEFAULT_UART_REG_جابه_جایی;

	أرجع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_داده *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	إذا (!compatible || !uart || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_گره_جابه_جایی_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_uart_گره(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_گره(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_داده *uart)
{
	أرجع fdt_parse_uart_گره_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_پیدا_کن_imsic_گره(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_داده *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	إذا (val && len >= حجم(fdt32_t)) {
		noff = fdt_گره_جابه_جایی_by_phandle(fdt, fdt32_مقصد_cpu(*val));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_parse_imsic_گره(fdt, noff, imsic);
		إذا (rc)
			أرجع rc;

		rc = imsic_داده_check(imsic);
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
		noff = fdt_گره_جابه_جایی_by_phandle(fdt, fdt32_مقصد_cpu(val[i]));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_aplic_پیدا_کن_imsic_گره(fdt, noff, imsic, mmode);
		إذا (!rc)
			اكسر;
	}

	أرجع rc;
}

صحيح fdt_parse_aplic_گره(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_داده *aplic)
{
	bool فرزند_یافت;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_داده imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_اندازه;
	بنية aplic_delegate_داده *deleg;

	إذا (nodeoff < 0 || !aplic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	إذا (len > 0)
		aplic->رقم_منبع = fdt32_مقصد_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->رقم_idc = len / 2;
	}

	rc = fdt_aplic_پیدا_کن_imsic_گره(fdt, nodeoff, &imsic, true);
	إذا (!rc) {
		aplic->والد_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_شاخص_بیتها;
		aplic->msicfg_mmode.lhxw = imsic.hart_شاخص_بیتها;
		aplic->msicfg_mmode.hhxw = imsic.گروه_شاخص_بیتها;
		aplic->msicfg_mmode.hhxs = imsic.گروه_شاخص_لغزش;
		إذا (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_صفحه_لغزش))
			أرجع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.پایه_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_پیدا_کن_imsic_گره(fdt, nodeoff, &imsic, false);
	إذا (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_شاخص_بیتها;
		aplic->msicfg_smode.lhxw = imsic.hart_شاخص_بیتها;
		aplic->msicfg_smode.hhxw = imsic.گروه_شاخص_بیتها;
		aplic->msicfg_smode.hhxs = imsic.گروه_شاخص_لغزش;
		إذا (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_صفحه_لغزش))
			أرجع SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.پایه_addr = imsic.regs[0].addr;
	}

	لكل (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->اولی_irq = 0;
		deleg->آخرین_irq = 0;
		deleg->فرزند_شاخص = 0;
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

		deleg->اولی_irq = fdt32_مقصد_cpu(del[i + 1]);
		deleg->آخرین_irq = fdt32_مقصد_cpu(del[i + 2]);
		deleg->فرزند_شاخص = 0;

		فرزند_یافت = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		إذا (!val || len < حجم(fdt32_t)) {
			deleg->اولی_irq = 0;
			deleg->آخرین_irq = 0;
			deleg->فرزند_شاخص = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			إذا (del[i] != val[j])
				استمر;
			deleg->فرزند_شاخص = j;
			فرزند_یافت = true;
			اكسر;
		}

		إذا (فرزند_یافت) {
			d++;
		} وإلا {
			deleg->اولی_irq = 0;
			deleg->آخرین_irq = 0;
			deleg->فرزند_شاخص = 0;
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

	طالما ((noff = fdt_گره_جابه_جایی_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		إذا (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			لكل (i = 0; i < len; i += 2) {
				إذا (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT)
					أرجع true;
			}
		}
	}

	أرجع false;
}

صحيح fdt_parse_imsic_گره(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_داده *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_اندازه;
	صحيح i, rc, len, nr_والد_irqs;

	إذا (nodeoff < 0 || !imsic || !fdt)
		أرجع SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_والد_irqs = len / 2;
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	إذا (val && len > 0)
		imsic->guest_شاخص_بیتها = fdt32_مقصد_cpu(*val);
	وإلا
		imsic->guest_شاخص_بیتها = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	إذا (val && len > 0) {
		imsic->hart_شاخص_بیتها = fdt32_مقصد_cpu(*val);
	} وإلا {
		imsic->hart_شاخص_بیتها = sbi_fls(nr_والد_irqs);
		إذا ((1UL << imsic->hart_شاخص_بیتها) < nr_والد_irqs)
			imsic->hart_شاخص_بیتها++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	إذا (val && len > 0)
		imsic->گروه_شاخص_بیتها = fdt32_مقصد_cpu(*val);
	وإلا
		imsic->گروه_شاخص_بیتها = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	إذا (val && len > 0)
		imsic->گروه_شاخص_لغزش = fdt32_مقصد_cpu(*val);
	وإلا
		imsic->گروه_شاخص_لغزش = 2 * IMSIC_MMIO_صفحه_لغزش;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	إذا (val && len > 0)
		imsic->رقم_ids = fdt32_مقصد_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->اندازه = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, i,
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

صحيح fdt_parse_plic_گره(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_داده *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_اندازه;

	إذا (nodeoffset < 0 || !plic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	إذا (len > 0)
		plic->رقم_src = fdt32_مقصد_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_داده *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	إذا (!compat || !plic || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_گره_جابه_جایی_by_compatible(fdt, -1, compat);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_plic_گره(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_اندازه_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    بیعلامت دراز *بیرون_addr1,
					    بیعلامت دراز *بیرون_size1,
					    بیعلامت دراز *بیرون_addr2,
					    بیعلامت دراز *بیرون_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;

	rc = fdt_get_گره_addr_اندازه_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		reg_addr = reg_اندازه = 0;
	*بیرون_addr1 = reg_addr;
	*بیرون_size1 = reg_اندازه;

	rc = fdt_get_گره_addr_اندازه_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	*بیرون_addr2 = reg_addr;
	*بیرون_size2 = reg_اندازه;

	أرجع 0;
}

ساكن صحيح fdt_get_aclint_addr_اندازه(ثابت فراغ *fdt, صحيح nodeoffset,
				    بیعلامت دراز *بیرون_addr1,
				    بیعلامت دراز *بیرون_size1,
				    بیعلامت دراز *بیرون_addr2,
				    بیعلامت دراز *بیرون_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_اندازه;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		أرجع SBI_ENODEV;
	*بیرون_addr1 = reg_addr;
	*بیرون_size1 = reg_اندازه;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 1,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0 || !reg_اندازه)
		reg_addr = reg_اندازه = 0;
	إذا (بیرون_addr2)
		*بیرون_addr2 = reg_addr;
	إذا (بیرون_size2)
		*بیرون_size2 = reg_اندازه;

	أرجع 0;
}

صحيح fdt_parse_aclint_گره(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  بیعلامت دراز *بیرون_addr1, بیعلامت دراز *بیرون_size1,
			  بیعلامت دراز *بیرون_addr2, بیعلامت دراز *بیرون_size2,
			  u32 *بیرون_اولی_hartid, u32 *بیرون_hart_شمار)
{
	ثابت fdt32_t *val;
	صحيح i, rc, شمار, cpu_جابه_جایی, cpu_intc_جابه_جایی;
	u32 phandle, hwirq, hartid, اولی_hartid, آخرین_hartid;
	u32 تطابق_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	إذا (nodeoffset < 0 || !fdt ||
	    !بیرون_addr1 || !بیرون_size1 ||
	    !بیرون_اولی_hartid || !بیرون_hart_شمار)
		أرجع SBI_EINVAL;

	إذا (for_timer && allow_regname && بیرون_addr2 && بیرون_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_اندازه_by_name(fdt, nodeoffset,
						      بیرون_addr1, بیرون_size1,
						      بیرون_addr2, بیرون_size2);
	وإلا
		rc = fdt_get_aclint_addr_اندازه(fdt, nodeoffset,
					      بیرون_addr1, بیرون_size1,
					      بیرون_addr2, بیرون_size2);
	إذا (rc)
		أرجع rc;

	*بیرون_اولی_hartid = 0;
	*بیرون_hart_شمار = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	إذا (!val || شمار < حجم(fdt32_t))
		أرجع 0;
	شمار = شمار / حجم(fdt32_t);

	اولی_hartid = -1U;
	آخرین_hartid = 0;
	لكل (i = 0; i < (شمار / 2); i++) {
		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[(2 * i) + 1]);

		cpu_intc_جابه_جایی = fdt_گره_جابه_جایی_by_phandle(fdt, phandle);
		إذا (cpu_intc_جابه_جایی < 0)
			استمر;

		cpu_جابه_جایی = fdt_والد_جابه_جایی(fdt, cpu_intc_جابه_جایی);
		إذا (cpu_جابه_جایی < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);
		إذا (rc)
			استمر;

		إذا (تطابق_hwirq == hwirq) {
			إذا (hartid < اولی_hartid)
				اولی_hartid = hartid;
			إذا (hartid > آخرین_hartid)
				آخرین_hartid = hartid;
		}
	}

	إذا ((آخرین_hartid >= اولی_hartid) && اولی_hartid != -1U) {
		*بیرون_اولی_hartid = اولی_hartid;
		*بیرون_hart_شمار = آخرین_hartid - اولی_hartid + 1;
	}
	أرجع 0;
}

صحيح fdt_parse_plmt_گره(ثابت فراغ *fdt, صحيح nodeoffset, بیعلامت دراز *plmt_پایه,
			  بیعلامت دراز *plmt_اندازه, u32 *hart_شمار)
{
	ثابت fdt32_t *val;
	صحيح rc, i, شمار;
	uint64_t reg_addr, reg_اندازه;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plmt_پایه ||
	    !hart_شمار || !plmt_اندازه)
		أرجع SBI_EINVAL;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plmt_پایه = reg_addr;
	*plmt_اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	إذا (!val || شمار < حجم(fdt32_t))
		أرجع 0;
	شمار = شمار / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (شمار / 2); i++) {
		صحيح cpu_جابه_جایی, cpu_intc_جابه_جایی;

		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[2 * i + 1]);

		cpu_intc_جابه_جایی = fdt_گره_جابه_جایی_by_phandle(fdt, phandle);
		إذا (cpu_intc_جابه_جایی < 0)
			استمر;

		cpu_جابه_جایی = fdt_والد_جابه_جایی(fdt, cpu_intc_جابه_جایی);
		إذا (cpu_جابه_جایی < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بیتها <= sbi_hartid_مقصد_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_شمار = hcount;

	أرجع 0;
}

صحيح fdt_parse_plicsw_گره(ثابت فراغ *fdt, صحيح nodeoffset, بیعلامت دراز *plicsw_پایه,
			  بیعلامت دراز *اندازه, u32 *hart_شمار)
{
	ثابت fdt32_t *val;
	صحيح rc, i, شمار;
	uint64_t reg_addr, reg_اندازه;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plicsw_پایه ||
	    !hart_شمار || !اندازه)
		أرجع SBI_EINVAL;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0,
				    &reg_addr, &reg_اندازه);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plicsw_پایه = reg_addr;
	*اندازه = reg_اندازه;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &شمار);
	إذا (!val || شمار < حجم(fdt32_t))
		أرجع 0;
	شمار = شمار / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (شمار / 2); i++) {
		صحيح cpu_جابه_جایی, cpu_intc_جابه_جایی;

		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[2 * i + 1]);

		cpu_intc_جابه_جایی = fdt_گره_جابه_جایی_by_phandle(fdt, phandle);
		إذا (cpu_intc_جابه_جایی < 0)
			استمر;

		cpu_جابه_جایی = fdt_والد_جابه_جایی(fdt, cpu_intc_جابه_جایی);
		إذا (cpu_جابه_جایی < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_جابه_جایی, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بیتها <= sbi_hartid_مقصد_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_شمار = hcount;

	أرجع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_گره_جابه_جایی_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoffset, 0, addr, عدم);
	إذا (rc < 0 || !addr)
		أرجع SBI_ENODEV;

	أرجع 0;
}
