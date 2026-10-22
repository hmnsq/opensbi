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

#تعریف_کرو DEFAULT_UART_FREQ		0
#تعریف_کرو DEFAULT_UART_BAUD		115200
#تعریف_کرو DEFAULT_UART_REG_کھسکاؤ		0
#تعریف_کرو DEFAULT_UART_REG_IO_چوڑائی	1
#تعریف_کرو DEFAULT_UART_REG_وتھ		0

#تعریف_کرو DEFAULT_RENESAS_SCIF_FREQ		100000000
#تعریف_کرو DEFAULT_RENESAS_SCIF_BAUD		115200

#تعریف_کرو DEFAULT_SIFIVE_UART_FREQ		0
#تعریف_کرو DEFAULT_SIFIVE_UART_BAUD		115200

#تعریف_کرو DEFAULT_SHAKTI_UART_FREQ		50000000
#تعریف_کرو DEFAULT_SHAKTI_UART_BAUD		115200

صحيح fdt_parse_phandle_with_دلیلاں(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحيح اشاریہ_2, بنية fdt_phandle_دلیلاں *باہرلا_دلیلاں)
{
	u32 i, pcells;
	صحيح len, pnodeoff;
	ثابت fdt32_t *list, *list_سرا, *val;

	إذا (!fdt || (nodeoff < 0) || !prop || !cells_prop || !باہرلا_دلیلاں)
		أرجع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	إذا (!list)
		أرجع SBI_ENOENT;
	list_سرا = list + (len / حجم(*list));

	طالما (list < list_سرا) {
		pnodeoff = fdt_گرہ_وتھ_by_phandle(fdt,
						fdt32_منزل_cpu(*list));
		إذا (pnodeoff < 0)
			أرجع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		إذا (!val)
			أرجع SBI_ENOENT;
		pcells = fdt32_منزل_cpu(*val);
		إذا (FDT_MAX_PHANDLE_دلیلاں < pcells)
			أرجع SBI_EINVAL;
		إذا (list + pcells > list_سرا)
			أرجع SBI_ENOENT;

		إذا (اشاریہ_2 > 0) {
			list += pcells;
			اشاریہ_2--;
		} وإلا {
			باہرلا_دلیلاں->گرہ_وتھ = pnodeoff;
			باہرلا_دلیلاں->دلیلاں_گنتی = pcells;
			لكل (i = 0; i < pcells; i++)
				باہرلا_دلیلاں->دلیلاں[i] = fdt32_منزل_cpu(list[i]);
			أرجع 0;
		}
	}

	أرجع SBI_ENOENT;
}

ساكن صحيح fdt_translate_پتہ(ثابت فراغ *fdt, uint64_t reg, صحيح ماپا,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_ماپا_addr, cell_بچہ_addr, cell_ناپ;
	ثابت fdt32_t *ranges;
	uint64_t وتھ, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, ماپا, "ranges", &rlen);
	إذا (ranges && rlen > 0) {
		cell_بچہ_addr = fdt_پتہ_cells(fdt, ماپا);
		إذا (cell_بچہ_addr < 1)
			أرجع SBI_ENODEV;

		cell_ماپا_addr = fdt_پتہ_cells(fdt, fdt_ماپا_وتھ(fdt, ماپا));
		إذا (cell_ماپا_addr < 1)
			أرجع SBI_ENODEV;

		cell_ناپ = fdt_ناپ_cells(fdt, ماپا);
		إذا (cell_ناپ < 0)
			أرجع SBI_ENODEV;

		لكل (i = 0; i < cell_بچہ_addr; i++)
			caddr = (caddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_ماپا_addr; i++)
			paddr = (paddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_ناپ; i++)
			rsize = (rsize << 32) | fdt32_منزل_cpu(*ranges++);
		إذا (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			أرجع SBI_ENODEV;
		}
		وتھ = reg - caddr;
		*addr = paddr + وتھ;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	أرجع 0;
}

صحيح fdt_get_گرہ_addr_ناپ(ثابت فراغ *fdt, صحيح گرہ, صحيح اشاریہ_2,
			   uint64_t *addr, uint64_t *ناپ)
{
	صحيح ماپا, len, i, rc;
	صحيح cell_addr, cell_ناپ;
	ثابت fdt32_t *prop_addr, *prop_ناپ;
	uint64_t عارضی = 0;

	إذا (!fdt || گرہ < 0 || اشاریہ_2 < 0)
		أرجع SBI_EINVAL;

	ماپا = fdt_ماپا_وتھ(fdt, گرہ);
	إذا (ماپا < 0)
		أرجع ماپا;
	cell_addr = fdt_پتہ_cells(fdt, ماپا);
	إذا (cell_addr < 1)
		أرجع SBI_ENODEV;

	cell_ناپ = fdt_ناپ_cells(fdt, ماپا);
	إذا (cell_ناپ < 0)
		أرجع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, گرہ, "reg", &len);
	إذا (!prop_addr)
		أرجع SBI_ENODEV;

	إذا ((len / حجم(u32)) <= (اشاریہ_2 * (cell_addr + cell_ناپ)))
		أرجع SBI_EINVAL;

	prop_addr = prop_addr + (اشاریہ_2 * (cell_addr + cell_ناپ));
	prop_ناپ = prop_addr + cell_addr;

	إذا (addr) {
		لكل (i = 0; i < cell_addr; i++)
			عارضی = (عارضی << 32) | fdt32_منزل_cpu(*prop_addr++);
		افعل {
			إذا (ماپا < 0)
				اكسر;
			rc  = fdt_translate_پتہ(fdt, عارضی, ماپا, addr);
			إذا (rc)
				اكسر;
			ماپا = fdt_ماپا_وتھ(fdt, ماپا);
			عارضی = *addr;
		} طالما (1);
	}
	عارضی = 0;

	إذا (ناپ) {
		لكل (i = 0; i < cell_ناپ; i++)
			عارضی = (عارضی << 32) | fdt32_منزل_cpu(*prop_ناپ++);
		*ناپ = عارضی;
	}

	أرجع 0;
}

صحيح fdt_get_گرہ_addr_ناپ_by_name(ثابت فراغ *fdt, صحيح گرہ, ثابت حرف *name,
				   uint64_t *addr, uint64_t *ناپ)
{
	صحيح i, j, گنتی;
	ثابت حرف *val;
	ثابت حرف *regname;

	إذا (!fdt || گرہ < 0 || !name)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, گرہ, "reg-names", &گنتی);
	إذا (!val)
		أرجع SBI_ENODEV;

	لكل (i = 0, j = 0; i < گنتی; i++, j++) {
		regname = val + i;

		إذا (strcmp(name, regname) == 0)
			أرجع fdt_get_گرہ_addr_ناپ(fdt, گرہ, j, addr, ناپ);

		i += لڑی_لمبائی(regname);
	}

	أرجع SBI_ENODEV;
}

bool fdt_گرہ_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	إذا (!prop)
		أرجع true;

	إذا (!لڑی_مقابلہ(prop, "okay", لڑی_لمبائی("okay")))
		أرجع true;

	إذا (!لڑی_مقابلہ(prop, "ok", لڑی_لمبائی("ok")))
		أرجع true;

	أرجع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_وتھ, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_وتھ < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_وتھ, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (لڑی_مقابلہ (prop, "cpu", لڑی_لمبائی ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_وتھ, "reg", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (len > حجم(fdt32_t))
		val++;

	إذا (hartid)
		*hartid = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_cbom_بلاک_ناپ(ثابت فراغ *fdt, صحيح cpu_وتھ, بنانشانی لما *cbom_بلاک_ناپ)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_وتھ < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_وتھ, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (لڑی_مقابلہ (prop, "cpu", لڑی_لمبائی ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_وتھ, "riscv,cbom-block-size", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (cbom_بلاک_ناپ)
		*cbom_بلاک_ناپ = fdt32_منزل_cpu(*val);
	أرجع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_وتھ, cpus_وتھ;

	إذا (!fdt)
		أرجع SBI_EINVAL;
	إذا (!max_hartid)
		أرجع 0;

	*max_hartid = 0;

	cpus_وتھ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_وتھ < 0)
		أرجع cpus_وتھ;

	fdt_for_each_subnode(cpu_وتھ, fdt, cpus_وتھ) {
		err = fdt_parse_hart_id(fdt, cpu_وتھ, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_گرہ_is_enabled(fdt, cpu_وتھ))
			استمر;

		إذا (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	أرجع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, بنانشانی لما *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_وتھ;

	إذا (!fdt || !freq)
		أرجع SBI_EINVAL;

	cpus_وتھ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_وتھ < 0)
		أرجع cpus_وتھ;

	val = fdt_getprop(fdt, cpus_وتھ, "timebase-frequency", &len);
	إذا (len > 0 && val)
		*freq = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_ENOENT;

	أرجع 0;
}

#تعریف_کرو RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, بنانشانی لما *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = لڑی_لمبائی(isa);

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

#تعریف_کرو سیٹ_multi_letter_ext(name, ثنائی_ہندسہ_2)				\
			إذا (!strcmp(mstr, name)) {		\
				__set_bit(ثنائی_ہندسہ_2, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			سیٹ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#تعریف_رد سیٹ_multi_letter_ext
	}

	أرجع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      بنانشانی لما *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		إذا (fdt_stringlist_شامل_اے(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحيح fdt_parse_isa_extensions_all_harts(ثابت فراغ *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	بنية sbi_scratch *scratch;
	بنية sbi_hart_features *hfeatures;
	صحيح err, cpu_وتھ, cpus_وتھ, len;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	cpus_وتھ = fdt_رستہ_وتھ(fdt, "/cpus");
	إذا (cpus_وتھ < 0)
		أرجع cpus_وتھ;

	fdt_for_each_subnode(cpu_وتھ, fdt, cpus_وتھ) {
		err = fdt_parse_hart_id(fdt, cpu_وتھ, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_گرہ_is_enabled(fdt, cpu_وتھ))
			استمر;

		scratch = sbi_hartid_منزل_scratch(hartid);
		إذا (!scratch)
			أرجع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		إذا (!hfeatures)
			أرجع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_وتھ, "riscv,isa-extensions", &len);
		إذا (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_وتھ, "riscv,isa", &len);
		إذا (!val || len <= 0)
			أرجع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		إذا (err)
			أرجع err;
	}

	أرجع 0;
}

ساكن صحيح fdt_parse_uart_گرہ_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_انکڑے *uart,
				      بنانشانی لما default_freq,
				      بنانشانی لما default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_ناپ;

	إذا (nodeoffset < 0 || !uart || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
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

صحيح fdt_parse_gaisler_uart_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_انکڑے *uart)
{
	أرجع fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_انکڑے *uart)
{
	أرجع fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_انکڑے *uart)
{
	أرجع fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_انکڑے *uart)
{
	أرجع fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_انکڑے *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	إذا (rc)
		أرجع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	إذا (len > 0 && val)
		uart->reg_کھسکاؤ = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_کھسکاؤ = DEFAULT_UART_REG_کھسکاؤ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	إذا (len > 0 && val)
		uart->reg_io_چوڑائی = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_io_چوڑائی = DEFAULT_UART_REG_IO_چوڑائی;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	إذا (len > 0 && val)
		uart->reg_وتھ = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_وتھ = DEFAULT_UART_REG_وتھ;

	أرجع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_انکڑے *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	إذا (!compatible || !uart || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_گرہ_وتھ_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_uart_گرہ(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_انکڑے *uart)
{
	أرجع fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_لبھو_imsic_گرہ(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_انکڑے *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	إذا (val && len >= حجم(fdt32_t)) {
		noff = fdt_گرہ_وتھ_by_phandle(fdt, fdt32_منزل_cpu(*val));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_parse_imsic_گرہ(fdt, noff, imsic);
		إذا (rc)
			أرجع rc;

		rc = imsic_انکڑے_check(imsic);
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
		noff = fdt_گرہ_وتھ_by_phandle(fdt, fdt32_منزل_cpu(val[i]));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_aplic_لبھو_imsic_گرہ(fdt, noff, imsic, mmode);
		إذا (!rc)
			اكسر;
	}

	أرجع rc;
}

صحيح fdt_parse_aplic_گرہ(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_انکڑے *aplic)
{
	bool بچہ_لبھیا;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_انکڑے imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_ناپ;
	بنية aplic_delegate_انکڑے *deleg;

	إذا (nodeoff < 0 || !aplic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, 0, &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		أرجع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->ناپ = reg_ناپ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	إذا (len > 0)
		aplic->عدد_سروت = fdt32_منزل_cpu(*val);

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

	rc = fdt_aplic_لبھو_imsic_گرہ(fdt, nodeoff, &imsic, true);
	إذا (!rc) {
		aplic->ماپا_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_اشاریہ_بٹ;
		aplic->msicfg_mmode.lhxw = imsic.hart_اشاریہ_بٹ;
		aplic->msicfg_mmode.hhxw = imsic.گروہ_اشاریہ_بٹ;
		aplic->msicfg_mmode.hhxs = imsic.گروہ_اشاریہ_کھسکاؤ;
		إذا (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_ورقہ_کھسکاؤ))
			أرجع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.ادھار_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_لبھو_imsic_گرہ(fdt, nodeoff, &imsic, false);
	إذا (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_اشاریہ_بٹ;
		aplic->msicfg_smode.lhxw = imsic.hart_اشاریہ_بٹ;
		aplic->msicfg_smode.hhxw = imsic.گروہ_اشاریہ_بٹ;
		aplic->msicfg_smode.hhxs = imsic.گروہ_اشاریہ_کھسکاؤ;
		إذا (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_ورقہ_کھسکاؤ))
			أرجع SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.ادھار_addr = imsic.regs[0].addr;
	}

	لكل (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->پہلا_irq = 0;
		deleg->آخری_irq = 0;
		deleg->بچہ_اشاریہ = 0;
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

		deleg->پہلا_irq = fdt32_منزل_cpu(del[i + 1]);
		deleg->آخری_irq = fdt32_منزل_cpu(del[i + 2]);
		deleg->بچہ_اشاریہ = 0;

		بچہ_لبھیا = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		إذا (!val || len < حجم(fdt32_t)) {
			deleg->پہلا_irq = 0;
			deleg->آخری_irq = 0;
			deleg->بچہ_اشاریہ = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			إذا (del[i] != val[j])
				استمر;
			deleg->بچہ_اشاریہ = j;
			بچہ_لبھیا = true;
			اكسر;
		}

		إذا (بچہ_لبھیا) {
			d++;
		} وإلا {
			deleg->پہلا_irq = 0;
			deleg->آخری_irq = 0;
			deleg->بچہ_اشاریہ = 0;
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

	طالما ((noff = fdt_گرہ_وتھ_by_compatible(fdt, noff,
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

صحيح fdt_parse_imsic_گرہ(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_انکڑے *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_ناپ;
	صحيح i, rc, len, nr_ماپا_irqs;

	إذا (nodeoff < 0 || !imsic || !fdt)
		أرجع SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_ماپا_irqs = len / 2;
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
		imsic->guest_اشاریہ_بٹ = fdt32_منزل_cpu(*val);
	وإلا
		imsic->guest_اشاریہ_بٹ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	إذا (val && len > 0) {
		imsic->hart_اشاریہ_بٹ = fdt32_منزل_cpu(*val);
	} وإلا {
		imsic->hart_اشاریہ_بٹ = sbi_fls(nr_ماپا_irqs);
		إذا ((1UL << imsic->hart_اشاریہ_بٹ) < nr_ماپا_irqs)
			imsic->hart_اشاریہ_بٹ++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	إذا (val && len > 0)
		imsic->گروہ_اشاریہ_بٹ = fdt32_منزل_cpu(*val);
	وإلا
		imsic->گروہ_اشاریہ_بٹ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	إذا (val && len > 0)
		imsic->گروہ_اشاریہ_کھسکاؤ = fdt32_منزل_cpu(*val);
	وإلا
		imsic->گروہ_اشاریہ_کھسکاؤ = 2 * IMSIC_MMIO_ورقہ_کھسکاؤ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	إذا (val && len > 0)
		imsic->عدد_ids = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->ناپ = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoff, i,
					    &reg_addr, &reg_ناپ);
		إذا (rc < 0 || !reg_ناپ)
			اكسر;
		regs->addr = reg_addr;
		regs->ناپ = reg_ناپ;
	}
	إذا (!imsic->regs[0].ناپ)
		أرجع SBI_EINVAL;

	أرجع 0;
}

صحيح fdt_parse_plic_گرہ(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_انکڑے *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_ناپ;

	إذا (nodeoffset < 0 || !plic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		أرجع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->ناپ = reg_ناپ;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	إذا (len > 0)
		plic->عدد_src = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_انکڑے *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	إذا (!compat || !plic || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_گرہ_وتھ_by_compatible(fdt, -1, compat);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_plic_گرہ(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_ناپ_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    بنانشانی لما *باہرلا_addr1,
					    بنانشانی لما *باہرلا_size1,
					    بنانشانی لما *باہرلا_addr2,
					    بنانشانی لما *باہرلا_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_ناپ;

	rc = fdt_get_گرہ_addr_ناپ_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		reg_addr = reg_ناپ = 0;
	*باہرلا_addr1 = reg_addr;
	*باہرلا_size1 = reg_ناپ;

	rc = fdt_get_گرہ_addr_ناپ_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		أرجع SBI_ENODEV;
	*باہرلا_addr2 = reg_addr;
	*باہرلا_size2 = reg_ناپ;

	أرجع 0;
}

ساكن صحيح fdt_get_aclint_addr_ناپ(ثابت فراغ *fdt, صحيح nodeoffset,
				    بنانشانی لما *باہرلا_addr1,
				    بنانشانی لما *باہرلا_size1,
				    بنانشانی لما *باہرلا_addr2,
				    بنانشانی لما *باہرلا_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_ناپ;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		أرجع SBI_ENODEV;
	*باہرلا_addr1 = reg_addr;
	*باہرلا_size1 = reg_ناپ;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 1,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0 || !reg_ناپ)
		reg_addr = reg_ناپ = 0;
	إذا (باہرلا_addr2)
		*باہرلا_addr2 = reg_addr;
	إذا (باہرلا_size2)
		*باہرلا_size2 = reg_ناپ;

	أرجع 0;
}

صحيح fdt_parse_aclint_گرہ(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  بنانشانی لما *باہرلا_addr1, بنانشانی لما *باہرلا_size1,
			  بنانشانی لما *باہرلا_addr2, بنانشانی لما *باہرلا_size2,
			  u32 *باہرلا_پہلا_hartid, u32 *باہرلا_hart_گنتی)
{
	ثابت fdt32_t *val;
	صحيح i, rc, گنتی, cpu_وتھ, cpu_intc_وتھ;
	u32 phandle, hwirq, hartid, پہلا_hartid, آخری_hartid;
	u32 میل_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	إذا (nodeoffset < 0 || !fdt ||
	    !باہرلا_addr1 || !باہرلا_size1 ||
	    !باہرلا_پہلا_hartid || !باہرلا_hart_گنتی)
		أرجع SBI_EINVAL;

	إذا (for_timer && allow_regname && باہرلا_addr2 && باہرلا_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_ناپ_by_name(fdt, nodeoffset,
						      باہرلا_addr1, باہرلا_size1,
						      باہرلا_addr2, باہرلا_size2);
	وإلا
		rc = fdt_get_aclint_addr_ناپ(fdt, nodeoffset,
					      باہرلا_addr1, باہرلا_size1,
					      باہرلا_addr2, باہرلا_size2);
	إذا (rc)
		أرجع rc;

	*باہرلا_پہلا_hartid = 0;
	*باہرلا_hart_گنتی = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	إذا (!val || گنتی < حجم(fdt32_t))
		أرجع 0;
	گنتی = گنتی / حجم(fdt32_t);

	پہلا_hartid = -1U;
	آخری_hartid = 0;
	لكل (i = 0; i < (گنتی / 2); i++) {
		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[(2 * i) + 1]);

		cpu_intc_وتھ = fdt_گرہ_وتھ_by_phandle(fdt, phandle);
		إذا (cpu_intc_وتھ < 0)
			استمر;

		cpu_وتھ = fdt_ماپا_وتھ(fdt, cpu_intc_وتھ);
		إذا (cpu_وتھ < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وتھ, &hartid);
		إذا (rc)
			استمر;

		إذا (میل_hwirq == hwirq) {
			إذا (hartid < پہلا_hartid)
				پہلا_hartid = hartid;
			إذا (hartid > آخری_hartid)
				آخری_hartid = hartid;
		}
	}

	إذا ((آخری_hartid >= پہلا_hartid) && پہلا_hartid != -1U) {
		*باہرلا_پہلا_hartid = پہلا_hartid;
		*باہرلا_hart_گنتی = آخری_hartid - پہلا_hartid + 1;
	}
	أرجع 0;
}

صحيح fdt_parse_plmt_گرہ(ثابت فراغ *fdt, صحيح nodeoffset, بنانشانی لما *plmt_ادھار,
			  بنانشانی لما *plmt_ناپ, u32 *hart_گنتی)
{
	ثابت fdt32_t *val;
	صحيح rc, i, گنتی;
	uint64_t reg_addr, reg_ناپ;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plmt_ادھار ||
	    !hart_گنتی || !plmt_ناپ)
		أرجع SBI_EINVAL;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plmt_ادھار = reg_addr;
	*plmt_ناپ = reg_ناپ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	إذا (!val || گنتی < حجم(fdt32_t))
		أرجع 0;
	گنتی = گنتی / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (گنتی / 2); i++) {
		صحيح cpu_وتھ, cpu_intc_وتھ;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_وتھ = fdt_گرہ_وتھ_by_phandle(fdt, phandle);
		إذا (cpu_intc_وتھ < 0)
			استمر;

		cpu_وتھ = fdt_ماپا_وتھ(fdt, cpu_intc_وتھ);
		إذا (cpu_وتھ < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وتھ, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بٹ <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_گنتی = hcount;

	أرجع 0;
}

صحيح fdt_parse_plicsw_گرہ(ثابت فراغ *fdt, صحيح nodeoffset, بنانشانی لما *plicsw_ادھار,
			  بنانشانی لما *ناپ, u32 *hart_گنتی)
{
	ثابت fdt32_t *val;
	صحيح rc, i, گنتی;
	uint64_t reg_addr, reg_ناپ;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plicsw_ادھار ||
	    !hart_گنتی || !ناپ)
		أرجع SBI_EINVAL;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0,
				    &reg_addr, &reg_ناپ);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plicsw_ادھار = reg_addr;
	*ناپ = reg_ناپ;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	إذا (!val || گنتی < حجم(fdt32_t))
		أرجع 0;
	گنتی = گنتی / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (گنتی / 2); i++) {
		صحيح cpu_وتھ, cpu_intc_وتھ;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_وتھ = fdt_گرہ_وتھ_by_phandle(fdt, phandle);
		إذا (cpu_intc_وتھ < 0)
			استمر;

		cpu_وتھ = fdt_ماپا_وتھ(fdt, cpu_intc_وتھ);
		إذا (cpu_وتھ < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وتھ, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بٹ <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_گنتی = hcount;

	أرجع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_گرہ_وتھ_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	rc = fdt_get_گرہ_addr_ناپ(fdt, nodeoffset, 0, addr, عدم);
	إذا (rc < 0 || !addr)
		أرجع SBI_ENODEV;

	أرجع 0;
}
