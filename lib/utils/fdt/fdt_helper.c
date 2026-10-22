// SPDX-License-Identifier: BSD-2-Clause
/*
 * fdt_helper.c - Flat Device Tree manipulation helper routines
 * Implement helper routines on top of libfdt for OpenSBI usage
 *
 * Copyright (C) 2020 Bin Meng <bmeng.cn@gmail.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_hartmask.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/irqchip/aplic.h>
#شامل <sbi_utils/irqchip/imsic.h>
#شامل <sbi_utils/irqchip/plic.h>

#تعریف_کرو DEFAULT_UART_FREQ		0
#تعریف_کرو DEFAULT_UART_BAUD		115200
#تعریف_کرو DEFAULT_UART_REG_سرکاؤ		0
#تعریف_کرو DEFAULT_UART_REG_IO_چوڑائی	1
#تعریف_کرو DEFAULT_UART_REG_انحراف		0

#تعریف_کرو DEFAULT_RENESAS_SCIF_FREQ		100000000
#تعریف_کرو DEFAULT_RENESAS_SCIF_BAUD		115200

#تعریف_کرو DEFAULT_SIFIVE_UART_FREQ		0
#تعریف_کرو DEFAULT_SIFIVE_UART_BAUD		115200

#تعریف_کرو DEFAULT_SHAKTI_UART_FREQ		50000000
#تعریف_کرو DEFAULT_SHAKTI_UART_BAUD		115200

صحیح fdt_parse_phandle_with_دلائل(ثابت خالی *fdt, صحیح nodeoff,
				ثابت حرف *prop, ثابت حرف *cells_prop,
				صحیح اشاریہ_2, ساخت fdt_phandle_دلائل *برآمد_دلائل)
{
	u32 i, pcells;
	صحیح len, pnodeoff;
	ثابت fdt32_t *list, *list_انتہا, *val;

	اگر (!fdt || (nodeoff < 0) || !prop || !cells_prop || !برآمد_دلائل)
		واپس SBI_EINVAL;

	list = fdt_getprop(fdt, nodeoff, prop, &len);
	اگر (!list)
		واپس SBI_ENOENT;
	list_انتہا = list + (len / حجم(*list));

	جب (list < list_انتہا) {
		pnodeoff = fdt_گرہ_انحراف_by_phandle(fdt,
						fdt32_مقصد_cpu(*list));
		اگر (pnodeoff < 0)
			واپس pnodeoff;
		list++;

		val = fdt_getprop(fdt, pnodeoff, cells_prop, &len);
		اگر (!val)
			واپس SBI_ENOENT;
		pcells = fdt32_مقصد_cpu(*val);
		اگر (FDT_MAX_PHANDLE_دلائل < pcells)
			واپس SBI_EINVAL;
		اگر (list + pcells > list_انتہا)
			واپس SBI_ENOENT;

		اگر (اشاریہ_2 > 0) {
			list += pcells;
			اشاریہ_2--;
		} ورنہ {
			برآمد_دلائل->گرہ_انحراف = pnodeoff;
			برآمد_دلائل->دلائل_گنتی = pcells;
			برائے (i = 0; i < pcells; i++)
				برآمد_دلائل->دلائل[i] = fdt32_مقصد_cpu(list[i]);
			واپس 0;
		}
	}

	واپس SBI_ENOENT;
}

ساکن صحیح fdt_translate_پتہ(ثابت خالی *fdt, uint64_t reg, صحیح والد,
				 uint64_t *addr)
{
	صحیح i, rlen;
	صحیح cell_والد_addr, cell_بچہ_addr, cell_سائز;
	ثابت fdt32_t *ranges;
	uint64_t انحراف, caddr = 0, paddr = 0, rsize = 0;

	ranges = fdt_getprop(fdt, والد, "ranges", &rlen);
	اگر (ranges && rlen > 0) {
		cell_بچہ_addr = fdt_پتہ_cells(fdt, والد);
		اگر (cell_بچہ_addr < 1)
			واپس SBI_ENODEV;

		cell_والد_addr = fdt_پتہ_cells(fdt, fdt_والد_انحراف(fdt, والد));
		اگر (cell_والد_addr < 1)
			واپس SBI_ENODEV;

		cell_سائز = fdt_سائز_cells(fdt, والد);
		اگر (cell_سائز < 0)
			واپس SBI_ENODEV;

		برائے (i = 0; i < cell_بچہ_addr; i++)
			caddr = (caddr << 32) | fdt32_مقصد_cpu(*ranges++);
		برائے (i = 0; i < cell_والد_addr; i++)
			paddr = (paddr << 32) | fdt32_مقصد_cpu(*ranges++);
		برائے (i = 0; i < cell_سائز; i++)
			rsize = (rsize << 32) | fdt32_مقصد_cpu(*ranges++);
		اگر (reg < caddr || caddr >= (reg + rsize )) {
			sbi_printf("invalid address translation\n");
			واپس SBI_ENODEV;
		}
		انحراف = reg - caddr;
		*addr = paddr + انحراف;
	} ورنہ {
		/* No translation required */
		*addr = reg;
	}

	واپس 0;
}

صحیح fdt_get_گرہ_addr_سائز(ثابت خالی *fdt, صحیح گرہ, صحیح اشاریہ_2,
			   uint64_t *addr, uint64_t *سائز)
{
	صحیح والد, len, i, rc;
	صحیح cell_addr, cell_سائز;
	ثابت fdt32_t *prop_addr, *prop_سائز;
	uint64_t عارضی = 0;

	اگر (!fdt || گرہ < 0 || اشاریہ_2 < 0)
		واپس SBI_EINVAL;

	والد = fdt_والد_انحراف(fdt, گرہ);
	اگر (والد < 0)
		واپس والد;
	cell_addr = fdt_پتہ_cells(fdt, والد);
	اگر (cell_addr < 1)
		واپس SBI_ENODEV;

	cell_سائز = fdt_سائز_cells(fdt, والد);
	اگر (cell_سائز < 0)
		واپس SBI_ENODEV;

	prop_addr = fdt_getprop(fdt, گرہ, "reg", &len);
	اگر (!prop_addr)
		واپس SBI_ENODEV;

	اگر ((len / حجم(u32)) <= (اشاریہ_2 * (cell_addr + cell_سائز)))
		واپس SBI_EINVAL;

	prop_addr = prop_addr + (اشاریہ_2 * (cell_addr + cell_سائز));
	prop_سائز = prop_addr + cell_addr;

	اگر (addr) {
		برائے (i = 0; i < cell_addr; i++)
			عارضی = (عارضی << 32) | fdt32_مقصد_cpu(*prop_addr++);
		کرو {
			اگر (والد < 0)
				توڑو;
			rc  = fdt_translate_پتہ(fdt, عارضی, والد, addr);
			اگر (rc)
				توڑو;
			والد = fdt_والد_انحراف(fdt, والد);
			عارضی = *addr;
		} جب (1);
	}
	عارضی = 0;

	اگر (سائز) {
		برائے (i = 0; i < cell_سائز; i++)
			عارضی = (عارضی << 32) | fdt32_مقصد_cpu(*prop_سائز++);
		*سائز = عارضی;
	}

	واپس 0;
}

صحیح fdt_get_گرہ_addr_سائز_by_name(ثابت خالی *fdt, صحیح گرہ, ثابت حرف *name,
				   uint64_t *addr, uint64_t *سائز)
{
	صحیح i, j, گنتی;
	ثابت حرف *val;
	ثابت حرف *regname;

	اگر (!fdt || گرہ < 0 || !name)
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, گرہ, "reg-names", &گنتی);
	اگر (!val)
		واپس SBI_ENODEV;

	برائے (i = 0, j = 0; i < گنتی; i++, j++) {
		regname = val + i;

		اگر (strcmp(name, regname) == 0)
			واپس fdt_get_گرہ_addr_سائز(fdt, گرہ, j, addr, سائز);

		i += لڑی_لمبائی(regname);
	}

	واپس SBI_ENODEV;
}

bool fdt_گرہ_is_enabled(ثابت خالی *fdt, صحیح nodeoff)
{
	صحیح len;
	ثابت خالی *prop;

	prop = fdt_getprop(fdt, nodeoff, "status", &len);
	اگر (!prop)
		واپس true;

	اگر (!لڑی_موازنہ(prop, "okay", لڑی_لمبائی("okay")))
		واپس true;

	اگر (!لڑی_موازنہ(prop, "ok", لڑی_لمبائی("ok")))
		واپس true;

	واپس false;
}

صحیح fdt_parse_hart_id(ثابت خالی *fdt, صحیح cpu_انحراف, u32 *hartid)
{
	صحیح len;
	ثابت خالی *prop;
	ثابت fdt32_t *val;

	اگر (!fdt || cpu_انحراف < 0)
		واپس SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_انحراف, "device_type", &len);
	اگر (!prop || !len)
		واپس SBI_EINVAL;
	اگر (لڑی_موازنہ (prop, "cpu", لڑی_لمبائی ("cpu")))
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_انحراف, "reg", &len);
	اگر (!val || len < حجم(fdt32_t))
		واپس SBI_EINVAL;

	اگر (len > حجم(fdt32_t))
		val++;

	اگر (hartid)
		*hartid = fdt32_مقصد_cpu(*val);

	واپس 0;
}

صحیح fdt_parse_cbom_بلاک_سائز(ثابت خالی *fdt, صحیح cpu_انحراف, غیرعلامتی طویل *cbom_بلاک_سائز)
{
	صحیح len;
	ثابت خالی *prop;
	ثابت fdt32_t *val;

	اگر (!fdt || cpu_انحراف < 0)
		واپس SBI_EINVAL;

	prop = fdt_getprop(fdt, cpu_انحراف, "device_type", &len);
	اگر (!prop || !len)
		واپس SBI_EINVAL;
	اگر (لڑی_موازنہ (prop, "cpu", لڑی_لمبائی ("cpu")))
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, cpu_انحراف, "riscv,cbom-block-size", &len);
	اگر (!val || len < حجم(fdt32_t))
		واپس SBI_EINVAL;

	اگر (cbom_بلاک_سائز)
		*cbom_بلاک_سائز = fdt32_مقصد_cpu(*val);
	واپس 0;
}

صحیح fdt_parse_max_enabled_hart_id(ثابت خالی *fdt, u32 *max_hartid)
{
	u32 hartid;
	صحیح err, cpu_انحراف, cpus_انحراف;

	اگر (!fdt)
		واپس SBI_EINVAL;
	اگر (!max_hartid)
		واپس 0;

	*max_hartid = 0;

	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس cpus_انحراف;

	fdt_for_each_subnode(cpu_انحراف, fdt, cpus_انحراف) {
		err = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (err)
			جاری;

		اگر (!fdt_گرہ_is_enabled(fdt, cpu_انحراف))
			جاری;

		اگر (hartid > *max_hartid)
			*max_hartid = hartid;
	}

	واپس 0;
}

صحیح fdt_parse_timebase_frequency(ثابت خالی *fdt, غیرعلامتی طویل *freq)
{
	ثابت fdt32_t *val;
	صحیح len, cpus_انحراف;

	اگر (!fdt || !freq)
		واپس SBI_EINVAL;

	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس cpus_انحراف;

	val = fdt_getprop(fdt, cpus_انحراف, "timebase-frequency", &len);
	اگر (len > 0 && val)
		*freq = fdt32_مقصد_cpu(*val);
	ورنہ
		واپس SBI_ENOENT;

	واپس 0;
}

#تعریف_کرو RISCV_ISA_EXT_NAME_LEN_MAX	32

ساکن صحیح fdt_parse_isa_one_hart(ثابت حرف *isa, غیرعلامتی طویل *extensions)
{
	size_t i, j, isa_len;
	حرف mstr[RISCV_ISA_EXT_NAME_LEN_MAX];

	i = 0;
	isa_len = لڑی_لمبائی(isa);

	اگر (isa[i] == 'r' || isa[i] == 'R')
		i++;
	ورنہ
		واپس SBI_EINVAL;

	اگر (isa[i] == 'v' || isa[i] == 'V')
		i++;
	ورنہ
		واپس SBI_EINVAL;

	اگر (isa[i] == '3' || isa[i+1] == '2')
		i += 2;
	ورنہ اگر (isa[i] == '6' || isa[i+1] == '4')
		i += 2;
	ورنہ
		واپس SBI_EINVAL;

	/* Skip base ISA extensions */
	برائے (; i < isa_len; i++) {
		اگر (isa[i] == '_')
			توڑو;
	}

	جب (i < isa_len) {
		اگر (isa[i] != '_') {
			i++;
			جاری;
		}

		/* Skip the '_' character */
		i++;

		/* Extract the multi-letter extension name */
		j = 0;
		جب ((i < isa_len) && (isa[i] != '_') &&
		       (j < (حجم(mstr) - 1)))
			mstr[j++] = isa[i++];
		mstr[j] = '\0';

		/* Skip empty multi-letter extension name */
		اگر (!j)
			جاری;

#تعریف_کرو سیٹ_multi_letter_ext(name, ثنائی_ہندسہ_2)				\
			اگر (!strcmp(mstr, name)) {		\
				__set_bit(ثنائی_ہندسہ_2, extensions);	\
				جاری;			\
			}

		برائے (j = 0; j < SBI_HART_EXT_MAX; j++) {
			سیٹ_multi_letter_ext(sbi_hart_ext[j].name,
					     sbi_hart_ext[j].id);
		}
#تعریف_منسوخ سیٹ_multi_letter_ext
	}

	واپس 0;
}

ساکن خالی fdt_parse_isa_extensions_one_hart(ثابت حرف *isa,
					      غیرعلامتی طویل *extensions,
					      صحیح len)
{
	size_t i;

	برائے (i = 0; i < SBI_HART_EXT_MAX; i++) {
		اگر (fdt_stringlist_شامل_ہے(isa, len, sbi_hart_ext[i].name))
			__set_bit(sbi_hart_ext[i].id, extensions);
	}
}

صحیح fdt_parse_isa_extensions_all_harts(ثابت خالی *fdt)
{
	u32 hartid;
	ثابت fdt32_t *val;
	ساخت sbi_scratch *scratch;
	ساخت sbi_hart_features *hfeatures;
	صحیح err, cpu_انحراف, cpus_انحراف, len;

	اگر (!fdt)
		واپس SBI_EINVAL;

	cpus_انحراف = fdt_راستہ_انحراف(fdt, "/cpus");
	اگر (cpus_انحراف < 0)
		واپس cpus_انحراف;

	fdt_for_each_subnode(cpu_انحراف, fdt, cpus_انحراف) {
		err = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (err)
			جاری;

		اگر (!fdt_گرہ_is_enabled(fdt, cpu_انحراف))
			جاری;

		scratch = sbi_hartid_مقصد_scratch(hartid);
		اگر (!scratch)
			واپس SBI_ENOENT;

		hfeatures = sbi_hart_features_ptr(scratch);
		اگر (!hfeatures)
			واپس SBI_ENOENT;

		val = fdt_getprop(fdt, cpu_انحراف, "riscv,isa-extensions", &len);
		اگر (val && len > 0) {
			fdt_parse_isa_extensions_one_hart((ثابت حرف *)val,
							  hfeatures->extensions, len);
			جاری;
		}

		val = fdt_getprop(fdt, cpu_انحراف, "riscv,isa", &len);
		اگر (!val || len <= 0)
			واپس SBI_ENOENT;

		err = fdt_parse_isa_one_hart((ثابت حرف *)val, hfeatures->extensions);
		اگر (err)
			واپس err;
	}

	واپس 0;
}

ساکن صحیح fdt_parse_uart_گرہ_common(ثابت خالی *fdt, صحیح nodeoffset,
				      ساخت platform_uart_ڈیٹا *uart,
				      غیرعلامتی طویل default_freq,
				      غیرعلامتی طویل default_baud)
{
	صحیح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_سائز;

	اگر (nodeoffset < 0 || !uart || !fdt)
		واپس SBI_ENODEV;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		واپس SBI_ENODEV;
	uart->addr = reg_addr;

	/**
	 * UART address is mandatory. clock-frequency and current-speed
	 * may not be present. Don't return error.
	 */
	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "clock-frequency", &len);
	اگر (len > 0 && val)
		uart->freq = fdt32_مقصد_cpu(*val);
	ورنہ
		uart->freq = default_freq;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "current-speed", &len);
	اگر (len > 0 && val)
		uart->baud = fdt32_مقصد_cpu(*val);
	ورنہ
		uart->baud = default_baud;

	واپس 0;
}

صحیح fdt_parse_gaisler_uart_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
				ساخت platform_uart_ڈیٹا *uart)
{
	واپس fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
}

صحیح fdt_parse_renesas_scif_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
				ساخت platform_uart_ڈیٹا *uart)
{
	واپس fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					  DEFAULT_RENESAS_SCIF_FREQ,
					  DEFAULT_RENESAS_SCIF_BAUD);
}

صحیح fdt_parse_shakti_uart_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
			       ساخت platform_uart_ڈیٹا *uart)
{
	واپس fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_SHAKTI_UART_FREQ,
					DEFAULT_SHAKTI_UART_BAUD);
}

صحیح fdt_parse_sifive_uart_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
			       ساخت platform_uart_ڈیٹا *uart)
{
	واپس fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_SIFIVE_UART_FREQ,
					DEFAULT_SIFIVE_UART_BAUD);
}

صحیح fdt_parse_uart_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
			ساخت platform_uart_ڈیٹا *uart)
{
	صحیح len, rc;
	ثابت fdt32_t *val;

	rc = fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart,
					DEFAULT_UART_FREQ,
					DEFAULT_UART_BAUD);
	اگر (rc)
		واپس rc;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
	اگر (len > 0 && val)
		uart->reg_سرکاؤ = fdt32_مقصد_cpu(*val);
	ورنہ
		uart->reg_سرکاؤ = DEFAULT_UART_REG_سرکاؤ;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
	اگر (len > 0 && val)
		uart->reg_io_چوڑائی = fdt32_مقصد_cpu(*val);
	ورنہ
		uart->reg_io_چوڑائی = DEFAULT_UART_REG_IO_چوڑائی;

	val = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-offset", &len);
	اگر (len > 0 && val)
		uart->reg_انحراف = fdt32_مقصد_cpu(*val);
	ورنہ
		uart->reg_انحراف = DEFAULT_UART_REG_انحراف;

	واپس 0;
}

صحیح fdt_parse_uart8250(ثابت خالی *fdt, ساخت platform_uart_ڈیٹا *uart,
		   ثابت حرف *compatible)
{
	صحیح nodeoffset;

	اگر (!compatible || !uart || !fdt)
		واپس SBI_ENODEV;

	nodeoffset = fdt_گرہ_انحراف_by_compatible(fdt, -1, compatible);
	اگر (nodeoffset < 0)
		واپس nodeoffset;

	واپس fdt_parse_uart_گرہ(fdt, nodeoffset, uart);
}

صحیح fdt_parse_xlnx_uartlite_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
			       ساخت platform_uart_ڈیٹا *uart)
{
	واپس fdt_parse_uart_گرہ_common(fdt, nodeoffset, uart, 0, 0);
}

ساکن صحیح fdt_aplic_ڈھونڈو_imsic_گرہ(ثابت خالی *fdt, صحیح nodeoff,
				     ساخت imsic_ڈیٹا *imsic, bool mmode)
{
	ثابت fdt32_t *val;
	صحیح i, len, noff, rc;

	val = fdt_getprop(fdt, nodeoff, "msi-parent", &len);
	اگر (val && len >= حجم(fdt32_t)) {
		noff = fdt_گرہ_انحراف_by_phandle(fdt, fdt32_مقصد_cpu(*val));
		اگر (noff < 0)
			واپس noff;

		rc = fdt_parse_imsic_گرہ(fdt, noff, imsic);
		اگر (rc)
			واپس rc;

		rc = imsic_ڈیٹا_check(imsic);
		اگر (rc)
			واپس rc;

		اگر (imsic->targets_mmode == mmode) {
			واپس 0;
		}
	} ورنہ {
		واپس SBI_ENODEV;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
	اگر (!val || len < حجم(fdt32_t))
		واپس SBI_ENODEV;

	len /= حجم(fdt32_t);

	برائے (i = 0; i < len; i++) {
		noff = fdt_گرہ_انحراف_by_phandle(fdt, fdt32_مقصد_cpu(val[i]));
		اگر (noff < 0)
			واپس noff;

		rc = fdt_aplic_ڈھونڈو_imsic_گرہ(fdt, noff, imsic, mmode);
		اگر (!rc)
			توڑو;
	}

	واپس rc;
}

صحیح fdt_parse_aplic_گرہ(ثابت خالی *fdt, صحیح nodeoff, ساخت aplic_ڈیٹا *aplic)
{
	bool بچہ_ملا;
	ثابت fdt32_t *val;
	ثابت fdt32_t *del;
	ساخت imsic_ڈیٹا imsic = { 0 };
	صحیح i, j, d, dcnt, len, rc;
	uint64_t reg_addr, reg_سائز;
	ساخت aplic_delegate_ڈیٹا *deleg;

	اگر (nodeoff < 0 || !aplic || !fdt)
		واپس SBI_ENODEV;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		واپس SBI_ENODEV;
	aplic->unique_id = nodeoff;
	aplic->addr = reg_addr;
	aplic->سائز = reg_سائز;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-sources", &len);
	اگر (len > 0)
		aplic->عدد_ماخذ = fdt32_مقصد_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	اگر (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		برائے (i = 0; i < len; i += 2) {
			اگر (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT) {
				aplic->targets_mmode = true;
				توڑو;
			}
		}
		aplic->عدد_idc = len / 2;
	}

	rc = fdt_aplic_ڈھونڈو_imsic_گرہ(fdt, nodeoff, &imsic, true);
	اگر (!rc) {
		aplic->والد_unique_id = imsic.unique_id;
		aplic->targets_mmode = true;
		aplic->has_msicfg_mmode = true;
		aplic->msicfg_mmode.lhxs = imsic.guest_اشاریہ_بٹ;
		aplic->msicfg_mmode.lhxw = imsic.hart_اشاریہ_بٹ;
		aplic->msicfg_mmode.hhxw = imsic.گروہ_اشاریہ_بٹ;
		aplic->msicfg_mmode.hhxs = imsic.گروہ_اشاریہ_سرکاؤ;
		اگر (aplic->msicfg_mmode.hhxs < (2 * IMSIC_MMIO_صفحہ_سرکاؤ))
			واپس SBI_EINVAL;
		aplic->msicfg_mmode.hhxs -= 24;
		aplic->msicfg_mmode.بنیاد_addr = imsic.regs[0].addr;
	}

	rc = fdt_aplic_ڈھونڈو_imsic_گرہ(fdt, nodeoff, &imsic, false);
	اگر (!rc) {
		aplic->has_msicfg_smode = true;
		aplic->msicfg_smode.lhxs = imsic.guest_اشاریہ_بٹ;
		aplic->msicfg_smode.lhxw = imsic.hart_اشاریہ_بٹ;
		aplic->msicfg_smode.hhxw = imsic.گروہ_اشاریہ_بٹ;
		aplic->msicfg_smode.hhxs = imsic.گروہ_اشاریہ_سرکاؤ;
		اگر (aplic->msicfg_smode.hhxs < (2 * IMSIC_MMIO_صفحہ_سرکاؤ))
			واپس SBI_EINVAL;
		aplic->msicfg_smode.hhxs -= 24;
		aplic->msicfg_smode.بنیاد_addr = imsic.regs[0].addr;
	}

	برائے (d = 0; d < APLIC_MAX_DELEGATE; d++) {
		deleg = &aplic->delegate[d];
		deleg->پہلا_irq = 0;
		deleg->آخری_irq = 0;
		deleg->بچہ_اشاریہ = 0;
	}

	del = fdt_getprop(fdt, nodeoff, "riscv,delegation", &len);
	اگر (!del)
		del = fdt_getprop(fdt, nodeoff, "riscv,delegate", &len);
	اگر (!del || len < (3 * حجم(fdt32_t)))
		جاؤ skip_delegate_parse;
	d = 0;
	dcnt = len / حجم(fdt32_t);
	برائے (i = 0; i < dcnt; i += 3) {
		اگر (d >= APLIC_MAX_DELEGATE)
			توڑو;
		deleg = &aplic->delegate[d];

		deleg->پہلا_irq = fdt32_مقصد_cpu(del[i + 1]);
		deleg->آخری_irq = fdt32_مقصد_cpu(del[i + 2]);
		deleg->بچہ_اشاریہ = 0;

		بچہ_ملا = false;
		val = fdt_getprop(fdt, nodeoff, "riscv,children", &len);
		اگر (!val || len < حجم(fdt32_t)) {
			deleg->پہلا_irq = 0;
			deleg->آخری_irq = 0;
			deleg->بچہ_اشاریہ = 0;
			جاری;
		}
		len = len / حجم(fdt32_t);
		برائے (j = 0; j < len; j++) {
			اگر (del[i] != val[j])
				جاری;
			deleg->بچہ_اشاریہ = j;
			بچہ_ملا = true;
			توڑو;
		}

		اگر (بچہ_ملا) {
			d++;
		} ورنہ {
			deleg->پہلا_irq = 0;
			deleg->آخری_irq = 0;
			deleg->بچہ_اشاریہ = 0;
		}
	}
skip_delegate_parse:

	واپس 0;
}

bool fdt_check_imsic_mlevel(ثابت خالی *fdt)
{
	ثابت fdt32_t *val;
	صحیح i, len, noff = 0;

	اگر (!fdt)
		واپس false;

	جب ((noff = fdt_گرہ_انحراف_by_compatible(fdt, noff,
						     "riscv,imsics")) >= 0) {
		val = fdt_getprop(fdt, noff, "interrupts-extended", &len);
		اگر (val && len > حجم(fdt32_t)) {
			len = len / حجم(fdt32_t);
			برائے (i = 0; i < len; i += 2) {
				اگر (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT)
					واپس true;
			}
		}
	}

	واپس false;
}

صحیح fdt_parse_imsic_گرہ(ثابت خالی *fdt, صحیح nodeoff, ساخت imsic_ڈیٹا *imsic)
{
	ثابت fdt32_t *val;
	ساخت imsic_regs *regs;
	uint64_t reg_addr, reg_سائز;
	صحیح i, rc, len, nr_والد_irqs;

	اگر (nodeoff < 0 || !imsic || !fdt)
		واپس SBI_ENODEV;

	imsic->unique_id = nodeoff;
	imsic->targets_mmode = false;
	val = fdt_getprop(fdt, nodeoff, "interrupts-extended", &len);
	اگر (val && len > حجم(fdt32_t)) {
		len = len / حجم(fdt32_t);
		nr_والد_irqs = len / 2;
		برائے (i = 0; i < len; i += 2) {
			اگر (fdt32_مقصد_cpu(val[i + 1]) == IRQ_M_EXT) {
				imsic->targets_mmode = true;
				توڑو;
			}
		}
	} ورنہ
		واپس SBI_EINVAL;

	val = fdt_getprop(fdt, nodeoff, "riscv,guest-index-bits", &len);
	اگر (val && len > 0)
		imsic->guest_اشاریہ_بٹ = fdt32_مقصد_cpu(*val);
	ورنہ
		imsic->guest_اشاریہ_بٹ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,hart-index-bits", &len);
	اگر (val && len > 0) {
		imsic->hart_اشاریہ_بٹ = fdt32_مقصد_cpu(*val);
	} ورنہ {
		imsic->hart_اشاریہ_بٹ = sbi_fls(nr_والد_irqs);
		اگر ((1UL << imsic->hart_اشاریہ_بٹ) < nr_والد_irqs)
			imsic->hart_اشاریہ_بٹ++;
	}

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-bits", &len);
	اگر (val && len > 0)
		imsic->گروہ_اشاریہ_بٹ = fdt32_مقصد_cpu(*val);
	ورنہ
		imsic->گروہ_اشاریہ_بٹ = 0;

	val = fdt_getprop(fdt, nodeoff, "riscv,group-index-shift", &len);
	اگر (val && len > 0)
		imsic->گروہ_اشاریہ_سرکاؤ = fdt32_مقصد_cpu(*val);
	ورنہ
		imsic->گروہ_اشاریہ_سرکاؤ = 2 * IMSIC_MMIO_صفحہ_سرکاؤ;

	val = fdt_getprop(fdt, nodeoff, "riscv,num-ids", &len);
	اگر (val && len > 0)
		imsic->عدد_ids = fdt32_مقصد_cpu(*val);
	ورنہ
		واپس SBI_EINVAL;

	برائے (i = 0; i < IMSIC_MAX_REGS; i++) {
		regs = &imsic->regs[i];
		regs->addr = 0;
		regs->سائز = 0;
	}

	برائے (i = 0; i < (IMSIC_MAX_REGS - 1); i++) {
		regs = &imsic->regs[i];

		rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, i,
					    &reg_addr, &reg_سائز);
		اگر (rc < 0 || !reg_سائز)
			توڑو;
		regs->addr = reg_addr;
		regs->سائز = reg_سائز;
	}
	اگر (!imsic->regs[0].سائز)
		واپس SBI_EINVAL;

	واپس 0;
}

صحیح fdt_parse_plic_گرہ(ثابت خالی *fdt, صحیح nodeoffset, ساخت plic_ڈیٹا *plic)
{
	صحیح len, rc;
	ثابت fdt32_t *val;
	uint64_t reg_addr, reg_سائز;

	اگر (nodeoffset < 0 || !plic || !fdt)
		واپس SBI_ENODEV;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		واپس SBI_ENODEV;
	plic->unique_id = nodeoffset;
	plic->addr = reg_addr;
	plic->سائز = reg_سائز;

	val = fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
	اگر (len > 0)
		plic->عدد_src = fdt32_مقصد_cpu(*val);

	واپس 0;
}

صحیح fdt_parse_plic(ثابت خالی *fdt, ساخت plic_ڈیٹا *plic, ثابت حرف *compat)
{
	صحیح nodeoffset;

	اگر (!compat || !plic || !fdt)
		واپس SBI_ENODEV;

	nodeoffset = fdt_گرہ_انحراف_by_compatible(fdt, -1, compat);
	اگر (nodeoffset < 0)
		واپس nodeoffset;

	واپس fdt_parse_plic_گرہ(fdt, nodeoffset, plic);
}

ساکن صحیح fdt_get_aclint_addr_سائز_by_name(ثابت خالی *fdt, صحیح nodeoffset,
					    غیرعلامتی طویل *برآمد_addr1,
					    غیرعلامتی طویل *برآمد_size1,
					    غیرعلامتی طویل *برآمد_addr2,
					    غیرعلامتی طویل *برآمد_size2)
{
	صحیح rc;
	uint64_t reg_addr, reg_سائز;

	rc = fdt_get_گرہ_addr_سائز_by_name(fdt, nodeoffset, "mtime",
					    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		reg_addr = reg_سائز = 0;
	*برآمد_addr1 = reg_addr;
	*برآمد_size1 = reg_سائز;

	rc = fdt_get_گرہ_addr_سائز_by_name(fdt, nodeoffset, "mtimecmp",
					    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		واپس SBI_ENODEV;
	*برآمد_addr2 = reg_addr;
	*برآمد_size2 = reg_سائز;

	واپس 0;
}

ساکن صحیح fdt_get_aclint_addr_سائز(ثابت خالی *fdt, صحیح nodeoffset,
				    غیرعلامتی طویل *برآمد_addr1,
				    غیرعلامتی طویل *برآمد_size1,
				    غیرعلامتی طویل *برآمد_addr2,
				    غیرعلامتی طویل *برآمد_size2)
{
	صحیح rc;
	uint64_t reg_addr, reg_سائز;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		واپس SBI_ENODEV;
	*برآمد_addr1 = reg_addr;
	*برآمد_size1 = reg_سائز;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 1,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0 || !reg_سائز)
		reg_addr = reg_سائز = 0;
	اگر (برآمد_addr2)
		*برآمد_addr2 = reg_addr;
	اگر (برآمد_size2)
		*برآمد_size2 = reg_سائز;

	واپس 0;
}

صحیح fdt_parse_aclint_گرہ(ثابت خالی *fdt, صحیح nodeoffset,
			  bool for_timer, bool allow_regname,
			  غیرعلامتی طویل *برآمد_addr1, غیرعلامتی طویل *برآمد_size1,
			  غیرعلامتی طویل *برآمد_addr2, غیرعلامتی طویل *برآمد_size2,
			  u32 *برآمد_پہلا_hartid, u32 *برآمد_hart_گنتی)
{
	ثابت fdt32_t *val;
	صحیح i, rc, گنتی, cpu_انحراف, cpu_intc_انحراف;
	u32 phandle, hwirq, hartid, پہلا_hartid, آخری_hartid;
	u32 مطابقت_hwirq = (for_timer) ? IRQ_M_TIMER : IRQ_M_SOFT;

	اگر (nodeoffset < 0 || !fdt ||
	    !برآمد_addr1 || !برآمد_size1 ||
	    !برآمد_پہلا_hartid || !برآمد_hart_گنتی)
		واپس SBI_EINVAL;

	اگر (for_timer && allow_regname && برآمد_addr2 && برآمد_size2 &&
	    fdt_getprop(fdt, nodeoffset, "reg-names", تہی))
		rc = fdt_get_aclint_addr_سائز_by_name(fdt, nodeoffset,
						      برآمد_addr1, برآمد_size1,
						      برآمد_addr2, برآمد_size2);
	ورنہ
		rc = fdt_get_aclint_addr_سائز(fdt, nodeoffset,
					      برآمد_addr1, برآمد_size1,
					      برآمد_addr2, برآمد_size2);
	اگر (rc)
		واپس rc;

	*برآمد_پہلا_hartid = 0;
	*برآمد_hart_گنتی = 0;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	اگر (!val || گنتی < حجم(fdt32_t))
		واپس 0;
	گنتی = گنتی / حجم(fdt32_t);

	پہلا_hartid = -1U;
	آخری_hartid = 0;
	برائے (i = 0; i < (گنتی / 2); i++) {
		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[(2 * i) + 1]);

		cpu_intc_انحراف = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
		اگر (cpu_intc_انحراف < 0)
			جاری;

		cpu_انحراف = fdt_والد_انحراف(fdt, cpu_intc_انحراف);
		اگر (cpu_انحراف < 0)
			جاری;

		rc = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);
		اگر (rc)
			جاری;

		اگر (مطابقت_hwirq == hwirq) {
			اگر (hartid < پہلا_hartid)
				پہلا_hartid = hartid;
			اگر (hartid > آخری_hartid)
				آخری_hartid = hartid;
		}
	}

	اگر ((آخری_hartid >= پہلا_hartid) && پہلا_hartid != -1U) {
		*برآمد_پہلا_hartid = پہلا_hartid;
		*برآمد_hart_گنتی = آخری_hartid - پہلا_hartid + 1;
	}
	واپس 0;
}

صحیح fdt_parse_plmt_گرہ(ثابت خالی *fdt, صحیح nodeoffset, غیرعلامتی طویل *plmt_بنیاد,
			  غیرعلامتی طویل *plmt_سائز, u32 *hart_گنتی)
{
	ثابت fdt32_t *val;
	صحیح rc, i, گنتی;
	uint64_t reg_addr, reg_سائز;
	u32 phandle, hwirq, hartid, hcount;

	اگر (nodeoffset < 0 || !fdt || !plmt_بنیاد ||
	    !hart_گنتی || !plmt_سائز)
		واپس SBI_EINVAL;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0)
		واپس SBI_ENODEV;
	*plmt_بنیاد = reg_addr;
	*plmt_سائز = reg_سائز;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	اگر (!val || گنتی < حجم(fdt32_t))
		واپس 0;
	گنتی = گنتی / حجم(fdt32_t);

	hcount = 0;
	برائے (i = 0; i < (گنتی / 2); i++) {
		صحیح cpu_انحراف, cpu_intc_انحراف;

		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[2 * i + 1]);

		cpu_intc_انحراف = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
		اگر (cpu_intc_انحراف < 0)
			جاری;

		cpu_انحراف = fdt_والد_انحراف(fdt, cpu_intc_انحراف);
		اگر (cpu_انحراف < 0)
			جاری;

		rc = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);

		اگر (rc)
			جاری;

		اگر (SBI_HARTMASK_MAX_بٹ <= sbi_hartid_مقصد_hartindex(hartid))
			جاری;

		اگر (hwirq == IRQ_M_TIMER)
			hcount++;
	}

	*hart_گنتی = hcount;

	واپس 0;
}

صحیح fdt_parse_plicsw_گرہ(ثابت خالی *fdt, صحیح nodeoffset, غیرعلامتی طویل *plicsw_بنیاد,
			  غیرعلامتی طویل *سائز, u32 *hart_گنتی)
{
	ثابت fdt32_t *val;
	صحیح rc, i, گنتی;
	uint64_t reg_addr, reg_سائز;
	u32 phandle, hwirq, hartid, hcount;

	اگر (nodeoffset < 0 || !fdt || !plicsw_بنیاد ||
	    !hart_گنتی || !سائز)
		واپس SBI_EINVAL;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0,
				    &reg_addr, &reg_سائز);
	اگر (rc < 0)
		واپس SBI_ENODEV;
	*plicsw_بنیاد = reg_addr;
	*سائز = reg_سائز;

	val = fdt_getprop(fdt, nodeoffset, "interrupts-extended", &گنتی);
	اگر (!val || گنتی < حجم(fdt32_t))
		واپس 0;
	گنتی = گنتی / حجم(fdt32_t);

	hcount = 0;
	برائے (i = 0; i < (گنتی / 2); i++) {
		صحیح cpu_انحراف, cpu_intc_انحراف;

		phandle = fdt32_مقصد_cpu(val[2 * i]);
		hwirq = fdt32_مقصد_cpu(val[2 * i + 1]);

		cpu_intc_انحراف = fdt_گرہ_انحراف_by_phandle(fdt, phandle);
		اگر (cpu_intc_انحراف < 0)
			جاری;

		cpu_انحراف = fdt_والد_انحراف(fdt, cpu_intc_انحراف);
		اگر (cpu_انحراف < 0)
			جاری;

		rc = fdt_parse_hart_id(fdt, cpu_انحراف, &hartid);

		اگر (rc)
			جاری;

		اگر (SBI_HARTMASK_MAX_بٹ <= sbi_hartid_مقصد_hartindex(hartid))
			جاری;

		اگر (hwirq == IRQ_M_SOFT)
			hcount++;
	}

	*hart_گنتی = hcount;

	واپس 0;
}

صحیح fdt_parse_compat_addr(ثابت خالی *fdt, uint64_t *addr,
			  ثابت حرف *compatible)
{
	صحیح nodeoffset, rc;

	nodeoffset = fdt_گرہ_انحراف_by_compatible(fdt, -1, compatible);
	اگر (nodeoffset < 0)
		واپس nodeoffset;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoffset, 0, addr, تہی);
	اگر (rc < 0 || !addr)
		واپس SBI_ENODEV;

	واپس 0;
}
