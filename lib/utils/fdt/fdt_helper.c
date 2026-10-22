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

#وصف_ڪريو DEFAULT_UART_FREQ		0
#وصف_ڪريو DEFAULT_UART_BAUD		115200
#وصف_ڪريو DEFAULT_UART_REG_سرڪايو		0
#وصف_ڪريو DEFAULT_UART_REG_IO_ويڪر	1
#وصف_ڪريو DEFAULT_UART_REG_وٿي		0

#وصف_ڪريو DEFAULT_RENESAS_SCIF_FREQ		100000000
#وصف_ڪريو DEFAULT_RENESAS_SCIF_BAUD		115200

#وصف_ڪريو DEFAULT_SIFIVE_UART_FREQ		0
#وصف_ڪريو DEFAULT_SIFIVE_UART_BAUD		115200

#وصف_ڪريو DEFAULT_SHAKTI_UART_FREQ		50000000
#وصف_ڪريو DEFAULT_SHAKTI_UART_BAUD		115200

صحيح fdt_parse_phandle_with_دليلون(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحيح اشاريو_2, بنية fdt_phandle_دليلون *ٻاهريون_دليلون)
{
	u32 i, pcells;
	صحيح len, pnodeoff;
	ثابت fdt32_t *list, *list_پڇاڙي, *val;

	إذا (!fdt || (nodeoff < 0) || !prop || !cells_prop || !ٻاهريون_دليلون)
		أرجع SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	إذا (!list)
		أرجع SBI_ENOENT;
	list_پڇاڙي = list + (len / حجم(*list));

	طالما (list < list_پڇاڙي) {
		pnodeoff = fdt_ڳنڍ_وٿي_by_phandle(fdt,
						fdt32_منزل_cpu(*list));
		إذا (pnodeoff < 0)
			أرجع pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		إذا (!val)
			أرجع SBI_ENOENT;
		pcells = fdt32_منزل_cpu(*val);
		إذا (FDT_MAX_PHANDLE_دليلون < pcells)
			أرجع SBI_EINVAL;
		إذا (list + pcells > list_پڇاڙي)
			أرجع SBI_ENOENT;

		إذا (اشاريو_2 > 0) {
			list += pcells;
			اشاريو_2--;
		} وإلا {
			ٻاهريون_دليلون->ڳنڍ_وٿي = pnodeoff;
			ٻاهريون_دليلون->دليلون_ڳڻپ = pcells;
			لكل (i = 0; i < pcells; i++)
				ٻاهريون_دليلون->دليلون[i] = fdt32_منزل_cpu(list[i]);
			أرجع 0;
		}
	}

	أرجع SBI_ENOENT;
}

ساكن صحيح fdt_translate_پتو(ثابت فراغ *fdt, uint64_t reg, صحيح والد,
				 uint64_t *addr)
{
	صحيح i, rlen;
	صحيح cell_والد_addr, cell_ٻار_addr, cell_سائيز;
	ثابت fdt32_t *ranges;
	uint64_t وٿي, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, والد, "ranges", &rlen);
	إذا (ranges && rlen > 0) {
		cell_ٻار_addr = fdt_پتو_cells(fdt, والد);
		إذا (cell_ٻار_addr < 1)
			أرجع SBI_ENODEV;

		cell_والد_addr = fdt_پتو_cells(fdt, fdt_والد_وٿي(fdt, والد));
		إذا (cell_والد_addr < 1)
			أرجع SBI_ENODEV;

		cell_سائيز = fdt_سائيز_cells(fdt, والد);
		إذا (cell_سائيز < 0)
			أرجع SBI_ENODEV;

		لكل (i = 0; i < cell_ٻار_addr; i++)
			caddr = (caddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_والد_addr; i++)
			paddr = (paddr << 32) | fdt32_منزل_cpu(*ranges++);
		لكل (i = 0; i < cell_سائيز; i++)
			rsize = (rsize << 32) | fdt32_منزل_cpu(*ranges++);
		إذا (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			أرجع SBI_ENODEV;
		}
		وٿي = reg - caddr;
		*addr = paddr + وٿي;
	} وإلا {
		/* No translation required */
		*addr = reg;
	}

	أرجع 0;
}

صحيح fdt_get_ڳنڍ_addr_سائيز(ثابت فراغ *fdt, صحيح ڳنڍ, صحيح اشاريو_2,
			   uint64_t *addr, uint64_t *سائيز)
{
	صحيح والد, len, i, rc;
	صحيح cell_addr, cell_سائيز;
	ثابت fdt32_t *prop_addr, *prop_سائيز;
	uint64_t عارضي = 0;

	إذا (!fdt || ڳنڍ < 0 || اشاريو_2 < 0)
		أرجع SBI_EINVAL;

	والد = fdt_والد_وٿي(fdt, ڳنڍ);
	إذا (والد < 0)
		أرجع والد;
	cell_addr = fdt_پتو_cells(fdt, والد);
	إذا (cell_addr < 1)
		أرجع SBI_ENODEV;

	cell_سائيز = fdt_سائيز_cells(fdt, والد);
	إذا (cell_سائيز < 0)
		أرجع SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, ڳنڍ, "reg", &len);
	إذا (!prop_addr)
		أرجع SBI_ENODEV;

	إذا ((len / حجم(u32)) <= (اشاريو_2 * (cell_addr + cell_سائيز)))
		أرجع SBI_EINVAL;

	prop_addr = prop_addr + (اشاريو_2 * (cell_addr + cell_سائيز));
	prop_سائيز = prop_addr + cell_addr;

	إذا (addr) {
		لكل (i = 0; i < cell_addr; i++)
			عارضي = (عارضي << 32) | fdt32_منزل_cpu(*prop_addr++);
		افعل {
			إذا (والد < 0)
				اكسر;
			rc  = fdt_translate_پتو(fdt, عارضي, والد, addr);
			إذا (rc)
				اكسر;
			والد = fdt_والد_وٿي(fdt, والد);
			عارضي = *addr;
		} طالما (1);
	}
	عارضي = 0;

	إذا (سائيز) {
		لكل (i = 0; i < cell_سائيز; i++)
			عارضي = (عارضي << 32) | fdt32_منزل_cpu(*prop_سائيز++);
		*سائيز = عارضي;
	}

	أرجع 0;
}

صحيح fdt_get_ڳنڍ_addr_سائيز_by_name(ثابت فراغ *fdt, صحيح ڳنڍ, ثابت حرف *name,
				   uint64_t *addr, uint64_t *سائيز)
{
	صحيح i, j, ڳڻپ;
	ثابت حرف *val;
	ثابت حرف *regname;

	إذا (!fdt || ڳنڍ < 0 || !name)
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, ڳنڍ, "reg-names", &ڳڻپ);
	إذا (!val)
		أرجع SBI_ENODEV;

	لكل (i = 0, j = 0; i < ڳڻپ; i++, j++) {
		regname = val + i;

		إذا (strcmp(name, regname) == 0)
			أرجع fdt_get_ڳنڍ_addr_سائيز(fdt, ڳنڍ, j, addr, سائيز);

		i += لڙي_ڊيگهه(regname);
	}

	أرجع SBI_ENODEV;
}

bool fdt_ڳنڍ_is_enabled(ثابت فراغ *fdt, صحيح nodeoff)
{
	صحيح len;
	ثابت فراغ *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	إذا (!prop)
		أرجع true;

	إذا (!لڙي_ڀيٽ(prop, "okay", لڙي_ڊيگهه("okay")))
		أرجع true;

	إذا (!لڙي_ڀيٽ(prop, "ok", لڙي_ڊيگهه("ok")))
		أرجع true;

	أرجع false;
}

صحيح fdt_parse_hart_id(ثابت فراغ *fdt, صحيح cpu_وٿي, u32 *hartid)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_وٿي < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_وٿي, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (لڙي_ڀيٽ (prop, "cpu", لڙي_ڊيگهه ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_وٿي, "reg", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (len > حجم(fdt32_t))
		val++;

	إذا (hartid)
		*hartid = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_cbom_بلاڪ_سائيز(ثابت فراغ *fdt, صحيح cpu_وٿي, بنانشان ڊگهو *cbom_بلاڪ_سائيز)
{
	صحيح len;
	ثابت فراغ *prop;
	ثابت fdt32_t *val;

	إذا (!fdt || cpu_وٿي < 0)
		أرجع SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_وٿي, "device_type", &len);
	إذا (!prop || !len)
		أرجع SBI_EINVAL;
	إذا (لڙي_ڀيٽ (prop, "cpu", لڙي_ڊيگهه ("cpu")))
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_وٿي, "riscv,cbom-block-size", &len);
	إذا (!val || len < حجم(fdt32_t))
		أرجع SBI_EINVAL;

	إذا (cbom_بلاڪ_سائيز)
		*cbom_بلاڪ_سائيز = fdt32_منزل_cpu(*val);
	أرجع 0;
}

صحيح fdt_parse_max_enabled_hart_id(ثابت فراغ *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحيح err, cpu_وٿي, cpus_وٿي;

	إذا (!fdt)
		أرجع SBI_EINVAL;
	إذا (!max_hartid)
		أرجع 0;

	*max_hartid = 0;

	cpus_وٿي = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_وٿي < 0)
		أرجع cpus_وٿي;

	fdt_for_each_subnode(cpu_وٿي, fdt, cpus_وٿي) {
		err = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_ڳنڍ_is_enabled(fdt, cpu_وٿي))
			استمر;

		إذا (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	أرجع 0;
}

صحيح fdt_parse_timebase_frequency(ثابت فراغ *fdt, بنانشان ڊگهو *freq)
{
	ثابت fdt32_t *val;
	صحيح len, cpus_وٿي;

	إذا (!fdt || !freq)
		أرجع SBI_EINVAL;

	cpus_وٿي = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_وٿي < 0)
		أرجع cpus_وٿي;

	val = fdt_getprop(fdt, cpus_وٿي, "timebase-frequency", &len);
	إذا (len > 0 && val)
		*freq = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_ENOENT;

	أرجع 0;
}

#وصف_ڪريو RISCV_ISA_EXT_NAME_LEN_MAX	32

ساكن صحيح fdt_parse_isa_one_hart(ثابت حرف *isa, بنانشان ڊگهو *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = لڙي_ڊيگهه(isa);

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

#وصف_ڪريو سيٽ_multi_letter_ext(name, ٻٽو_انگ_2)				\
			إذا (!strcmp(mstr, name)) {		\
				__set_bit(ٻٽو_انگ_2, extensions);	\
				استمر;			\
			}

		لكل (j = 0; j < SBI_HART_EXT_MAX; j++) {
			سيٽ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#وصف_رد سيٽ_multi_letter_ext
	}

	أرجع 0;
}

ساكن فراغ fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      بنانشان ڊگهو *extensions,
					      صحيح len)
{
	size_t i;

	لكل (i = 0; i < SBI_HART_EXT_MAX; i++) {
		إذا (fdt_stringlist_شامل_آهي(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحيح fdt_parse_isa_extensions_all_harts(ثابت فراغ *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	بنية sbi_scratch *scratch;
	بنية sbi_hart_features *hfeatures;
	صحيح err, cpu_وٿي, cpus_وٿي, len;

	إذا (!fdt)
		أرجع SBI_EINVAL;

	cpus_وٿي = fdt_رستو_وٿي(fdt, "/cpus");
	إذا (cpus_وٿي < 0)
		أرجع cpus_وٿي;

	fdt_for_each_subnode(cpu_وٿي, fdt, cpus_وٿي) {
		err = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);
		إذا (err)
			استمر;

		إذا (!fdt_ڳنڍ_is_enabled(fdt, cpu_وٿي))
			استمر;

		scratch = sbi_hartid_منزل_scratch(hartid);
		إذا (!scratch)
			أرجع SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		إذا (!hfeatures)
			أرجع SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_وٿي, "riscv,isa-extensions", &len);
		إذا (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			استمر;
		}

		val = fdt_getprop(fdt, cpu_وٿي, "riscv,isa", &len);
		إذا (!val || len <= 0)
			أرجع SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		إذا (err)
			أرجع err;
	}

	أرجع 0;
}

ساكن صحيح fdt_parse_uart_ڳنڍ_common(ثابت فراغ *fdt, صحيح nodeoffset,
				      بنية platform_uart_معلومات *uart,
				      بنانشان ڊگهو default_freq,
				      بنانشان ڊگهو default_baud)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_سائيز;

	إذا (nodeoffset < 0 || !uart || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
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

صحيح fdt_parse_gaisler_uart_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحيح fdt_parse_renesas_scif_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
				بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحيح fdt_parse_shakti_uart_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحيح fdt_parse_sifive_uart_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحيح fdt_parse_uart_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
			بنية platform_uart_معلومات *uart)
{
	صحيح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	إذا (rc)
		أرجع rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	إذا (len > 0 && val)
		uart->reg_سرڪايو = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_سرڪايو = DEFAULT_UART_REG_سرڪايو;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	إذا (len > 0 && val)
		uart->reg_io_ويڪر = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_io_ويڪر = DEFAULT_UART_REG_IO_ويڪر;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	إذا (len > 0 && val)
		uart->reg_وٿي = fdt32_منزل_cpu(*val);
	وإلا
		uart->reg_وٿي = DEFAULT_UART_REG_وٿي;

	أرجع 0;
}

صحيح fdt_parse_uart8250(ثابت فراغ *fdt, بنية platform_uart_معلومات *uart,
		   ثابت حرف *compatible)
{
	صحيح nodeoffset;

	إذا (!compatible || !uart || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_ڳنڍ_وٿي_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_uart_ڳنڍ(fdt, nodeoffset, uart);
}

صحيح fdt_parse_xlnx_uartlite_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
			       بنية platform_uart_معلومات *uart)
{
	أرجع fdt_parse_uart_ڳنڍ_common(fdt, nodeoffset, uart, 0, 0);
}

ساكن صحيح fdt_aplic_ڳوليو_imsic_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoff,
				     بنية imsic_معلومات *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحيح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	إذا (val && len >= حجم(fdt32_t)) {
		noff = fdt_ڳنڍ_وٿي_by_phandle(fdt, fdt32_منزل_cpu(*val));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_parse_imsic_ڳنڍ(fdt, noff, imsic);
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
		noff = fdt_ڳنڍ_وٿي_by_phandle(fdt, fdt32_منزل_cpu(val[i]));
		إذا (noff < 0)
			أرجع noff;

		rc = fdt_aplic_ڳوليو_imsic_ڳنڍ(fdt, noff, imsic, mmode);
		إذا (!rc)
			اكسر;
	}

	أرجع rc;
}

صحيح fdt_parse_aplic_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoff, بنية aplic_معلومات *aplic)
{
	bool ٻار_مليو;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	بنية imsic_معلومات imsic = { 0 };
	صحيح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_سائيز;
	بنية aplic_delegate_معلومات *deleg;

	إذا (nodeoff < 0 || !aplic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, 0, &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		أرجع SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->سائيز = reg_سائيز;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	إذا (len > 0)
		aplic->انگ_ذريعو = fdt32_منزل_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	إذا (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		لكل (i = 0; i < len; i += 2) {
			إذا (fdt32_منزل_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				اكسر;
			}
		}
		aplic->انگ_idc = len / 2;
	}

	rc = fdt_aplic_ڳوليو_imsic_ڳنڍ(fdt, nodeoff, &imsic, true);
	إذا (!rc) {
		aplic->والد_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_اشاريو_بٽ;
		aplic->msicfg_mmode.lhxw = imsic.hart_اشاريو_بٽ;
		aplic->msicfg_mmode.hhxw = imsic.گروهه_اشاريو_بٽ;
		aplic->msicfg_mmode.hhxs = imsic.گروهه_اشاريو_سرڪايو;
		إذا (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_صفحو_سرڪايو))
			أرجع SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.بنياد_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_ڳوليو_imsic_ڳنڍ(fdt, nodeoff, &imsic, false);
	إذا (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_اشاريو_بٽ;
		aplic->msicfg_smode.lhxw = imsic.hart_اشاريو_بٽ;
		aplic->msicfg_smode.hhxw = imsic.گروهه_اشاريو_بٽ;
		aplic->msicfg_smode.hhxs = imsic.گروهه_اشاريو_سرڪايو;
		إذا (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_صفحو_سرڪايو))
			أرجع SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.بنياد_addr = imsic.regs[0].addr;
	}

	لكل (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->پهريون_irq = 0;
		deleg->آخري_irq = 0;
		deleg->ٻار_اشاريو = 0;
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

		deleg->پهريون_irq = fdt32_منزل_cpu(del[i + 1]);
		deleg->آخري_irq = fdt32_منزل_cpu(del[i + 2]);
		deleg->ٻار_اشاريو = 0;

		ٻار_مليو = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		إذا (!val || len < حجم(fdt32_t)) {
			deleg->پهريون_irq = 0;
			deleg->آخري_irq = 0;
			deleg->ٻار_اشاريو = 0;
			استمر;
		}
		len = len / حجم(fdt32_t);
		لكل (j = 0; j < len; j++) {
			إذا (del[i] != val[j])
				استمر;
			deleg->ٻار_اشاريو = j;
			ٻار_مليو = true;
			اكسر;
		}

		إذا (ٻار_مليو) {
			d++;
		} وإلا {
			deleg->پهريون_irq = 0;
			deleg->آخري_irq = 0;
			deleg->ٻار_اشاريو = 0;
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

	طالما ((noff = fdt_ڳنڍ_وٿي_by_compatible(fdt, noff,
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

صحيح fdt_parse_imsic_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoff, بنية imsic_معلومات *imsic)
{
	ثابت fdt32_t *val;
	بنية imsic_regs *regs;
	uint64_t reg_addr, reg_سائيز;
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
			إذا (fdt32_منزل_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				اكسر;
			}
		}
	} وإلا
		أرجع SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	إذا (val && len > 0)
		imsic->guest_اشاريو_بٽ = fdt32_منزل_cpu(*val);
	وإلا
		imsic->guest_اشاريو_بٽ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	إذا (val && len > 0) {
		imsic->hart_اشاريو_بٽ = fdt32_منزل_cpu(*val);
	} وإلا {
		imsic->hart_اشاريو_بٽ = sbi_fls(nr_والد_irqs);
		إذا ((1UL << imsic->hart_اشاريو_بٽ) < nr_والد_irqs)
			imsic->hart_اشاريو_بٽ++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	إذا (val && len > 0)
		imsic->گروهه_اشاريو_بٽ = fdt32_منزل_cpu(*val);
	وإلا
		imsic->گروهه_اشاريو_بٽ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	إذا (val && len > 0)
		imsic->گروهه_اشاريو_سرڪايو = fdt32_منزل_cpu(*val);
	وإلا
		imsic->گروهه_اشاريو_سرڪايو = 2 * IMSIC_MMIO_صفحو_سرڪايو;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	إذا (val && len > 0)
		imsic->انگ_ids = fdt32_منزل_cpu(*val);
	وإلا
		أرجع SBI_EINVAL;

	لكل (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->سائيز = 0;
	}

	لكل (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoff, i,
					    &reg_addr, &reg_سائيز);
		إذا (rc < 0 || !reg_سائيز)
			اكسر;
		regs->addr = reg_addr;
		regs->سائيز = reg_سائيز;
	}
	إذا (!imsic->regs[0].سائيز)
		أرجع SBI_EINVAL;

	أرجع 0;
}

صحيح fdt_parse_plic_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset, بنية plic_معلومات *plic)
{
	صحيح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_سائيز;

	إذا (nodeoffset < 0 || !plic || !fdt)
		أرجع SBI_ENODEV;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		أرجع SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->سائيز = reg_سائيز;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	إذا (len > 0)
		plic->انگ_src = fdt32_منزل_cpu(*val);

	أرجع 0;
}

صحيح fdt_parse_plic(ثابت فراغ *fdt, بنية plic_معلومات *plic, ثابت حرف *compat)
{
	صحيح nodeoffset;

	إذا (!compat || !plic || !fdt)
		أرجع SBI_ENODEV;

	nodeoffset = fdt_ڳنڍ_وٿي_by_compatible(fdt, -1, compat);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	أرجع fdt_parse_plic_ڳنڍ(fdt, nodeoffset, plic);
}

ساكن صحيح fdt_get_aclint_addr_سائيز_by_name(ثابت فراغ *fdt, صحيح nodeoffset,
					    بنانشان ڊگهو *ٻاهريون_addr1,
					    بنانشان ڊگهو *ٻاهريون_size1,
					    بنانشان ڊگهو *ٻاهريون_addr2,
					    بنانشان ڊگهو *ٻاهريون_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_سائيز;

	rc = fdt_get_ڳنڍ_addr_سائيز_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		reg_addr = reg_سائيز = 0;
	*ٻاهريون_addr1 = reg_addr;
	*ٻاهريون_size1 = reg_سائيز;

	rc = fdt_get_ڳنڍ_addr_سائيز_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		أرجع SBI_ENODEV;
	*ٻاهريون_addr2 = reg_addr;
	*ٻاهريون_size2 = reg_سائيز;

	أرجع 0;
}

ساكن صحيح fdt_get_aclint_addr_سائيز(ثابت فراغ *fdt, صحيح nodeoffset,
				    بنانشان ڊگهو *ٻاهريون_addr1,
				    بنانشان ڊگهو *ٻاهريون_size1,
				    بنانشان ڊگهو *ٻاهريون_addr2,
				    بنانشان ڊگهو *ٻاهريون_size2)
{
	صحيح rc;
	uint64_t reg_addr, reg_سائيز;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		أرجع SBI_ENODEV;
	*ٻاهريون_addr1 = reg_addr;
	*ٻاهريون_size1 = reg_سائيز;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 1,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0 || !reg_سائيز)
		reg_addr = reg_سائيز = 0;
	إذا (ٻاهريون_addr2)
		*ٻاهريون_addr2 = reg_addr;
	إذا (ٻاهريون_size2)
		*ٻاهريون_size2 = reg_سائيز;

	أرجع 0;
}

صحيح fdt_parse_aclint_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset,
			  bool for_timer, bool allow_regname,
			  بنانشان ڊگهو *ٻاهريون_addr1, بنانشان ڊگهو *ٻاهريون_size1,
			  بنانشان ڊگهو *ٻاهريون_addr2, بنانشان ڊگهو *ٻاهريون_size2,
			  u32 *ٻاهريون_پهريون_hartid, u32 *ٻاهريون_hart_ڳڻپ)
{
	ثابت fdt32_t *val;
	صحيح i, rc, ڳڻپ, cpu_وٿي, cpu_intc_وٿي;
	u32 phandle, hwirq, hartid, پهريون_hartid, آخري_hartid;
	u32 ميلاپ_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	إذا (nodeoffset < 0 || !fdt ||
	    !ٻاهريون_addr1 || !ٻاهريون_size1 ||
	    !ٻاهريون_پهريون_hartid || !ٻاهريون_hart_ڳڻپ)
		أرجع SBI_EINVAL;

	إذا (for_timer && allow_regname && ٻاهريون_addr2 && ٻاهريون_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", عدم))
		rc = fdt_get_aclint_addr_سائيز_by_name(fdt, nodeoffset,
						      ٻاهريون_addr1, ٻاهريون_size1,
						      ٻاهريون_addr2, ٻاهريون_size2);
	وإلا
		rc = fdt_get_aclint_addr_سائيز(fdt, nodeoffset,
					      ٻاهريون_addr1, ٻاهريون_size1,
					      ٻاهريون_addr2, ٻاهريون_size2);
	إذا (rc)
		أرجع rc;

	*ٻاهريون_پهريون_hartid = 0;
	*ٻاهريون_hart_ڳڻپ = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ڳڻپ);
	إذا (!val || ڳڻپ < حجم(fdt32_t))
		أرجع 0;
	ڳڻپ = ڳڻپ / حجم(fdt32_t);

	پهريون_hartid = -1U;
	آخري_hartid = 0;
	لكل (i = 0; i < (ڳڻپ / 2); i++) {
		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[(2 * i) + 1]);

		cpu_intc_وٿي = fdt_ڳنڍ_وٿي_by_phandle(fdt, phandle);
		إذا (cpu_intc_وٿي < 0)
			استمر;

		cpu_وٿي = fdt_والد_وٿي(fdt, cpu_intc_وٿي);
		إذا (cpu_وٿي < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);
		إذا (rc)
			استمر;

		إذا (ميلاپ_hwirq == hwirq) {
			إذا (hartid < پهريون_hartid)
				پهريون_hartid = hartid;
			إذا (hartid > آخري_hartid)
				آخري_hartid = hartid;
		}
	}

	إذا ((آخري_hartid >= پهريون_hartid) && پهريون_hartid != -1U) {
		*ٻاهريون_پهريون_hartid = پهريون_hartid;
		*ٻاهريون_hart_ڳڻپ = آخري_hartid - پهريون_hartid + 1;
	}
	أرجع 0;
}

صحيح fdt_parse_plmt_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset, بنانشان ڊگهو *plmt_بنياد,
			  بنانشان ڊگهو *plmt_سائيز, u32 *hart_ڳڻپ)
{
	ثابت fdt32_t *val;
	صحيح rc, i, ڳڻپ;
	uint64_t reg_addr, reg_سائيز;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plmt_بنياد ||
	    !hart_ڳڻپ || !plmt_سائيز)
		أرجع SBI_EINVAL;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plmt_بنياد = reg_addr;
	*plmt_سائيز = reg_سائيز;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ڳڻپ);
	إذا (!val || ڳڻپ < حجم(fdt32_t))
		أرجع 0;
	ڳڻپ = ڳڻپ / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (ڳڻپ / 2); i++) {
		صحيح cpu_وٿي, cpu_intc_وٿي;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_وٿي = fdt_ڳنڍ_وٿي_by_phandle(fdt, phandle);
		إذا (cpu_intc_وٿي < 0)
			استمر;

		cpu_وٿي = fdt_والد_وٿي(fdt, cpu_intc_وٿي);
		إذا (cpu_وٿي < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بٽ <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_ڳڻپ = hcount;

	أرجع 0;
}

صحيح fdt_parse_plicsw_ڳنڍ(ثابت فراغ *fdt, صحيح nodeoffset, بنانشان ڊگهو *plicsw_بنياد,
			  بنانشان ڊگهو *سائيز, u32 *hart_ڳڻپ)
{
	ثابت fdt32_t *val;
	صحيح rc, i, ڳڻپ;
	uint64_t reg_addr, reg_سائيز;
	u32 phandle, hwirq, hartid, hcount;

	إذا (nodeoffset < 0 || !fdt || !plicsw_بنياد ||
	    !hart_ڳڻپ || !سائيز)
		أرجع SBI_EINVAL;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائيز);
	إذا (rc < 0)
		أرجع SBI_ENODEV;
	*plicsw_بنياد = reg_addr;
	*سائيز = reg_سائيز;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &ڳڻپ);
	إذا (!val || ڳڻپ < حجم(fdt32_t))
		أرجع 0;
	ڳڻپ = ڳڻپ / حجم(fdt32_t);

	hcount = 0;
	لكل (i = 0; i < (ڳڻپ / 2); i++) {
		صحيح cpu_وٿي, cpu_intc_وٿي;

		phandle = fdt32_منزل_cpu(val[2 * i]);
		hwirq = fdt32_منزل_cpu(val[2 * i + 1]);

		cpu_intc_وٿي = fdt_ڳنڍ_وٿي_by_phandle(fdt, phandle);
		إذا (cpu_intc_وٿي < 0)
			استمر;

		cpu_وٿي = fdt_والد_وٿي(fdt, cpu_intc_وٿي);
		إذا (cpu_وٿي < 0)
			استمر;

		rc = fdt_parse_hart_id(fdt, cpu_وٿي, &hartid);

		إذا (rc)
			استمر;

		إذا (SBI_HARTMASK_MAX_بٽ <= sbi_hartid_منزل_hartindex(hartid))
			استمر;

		إذا (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_ڳڻپ = hcount;

	أرجع 0;
}

صحيح fdt_parse_compat_addr(ثابت فراغ *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحيح nodeoffset, rc;

	nodeoffset = fdt_ڳنڍ_وٿي_by_compatible(fdt, -1, compatible);
	إذا (nodeoffset < 0)
		أرجع nodeoffset;

	rc = fdt_get_ڳنڍ_addr_سائيز(fdt, nodeoffset, 0, addr, عدم);
	إذا (rc < 0 || !addr)
		أرجع SBI_ENODEV;

	أرجع 0;
}
