/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_byteorder.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/regmap/fdt_regmap.h>

شمار syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ساخت syscon_regmap {
	u32 reg_io_چوڑائی;
	شمار syscon_regmap_endian reg_endian;
	غیرعلامتی طویل addr;
	ساخت regmap rmap;
};

#تعریف_کرو مقصد_syscon_regmap(__rmap)	\
	container_کا((__rmap), ساخت syscon_regmap, rmap)

ساکن صحیح regmap_syscon_read_8(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readb((متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_8(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				 غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writeb(val, (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readw((متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				 غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writew(val, (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readl((متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				 غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(val, (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_le16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				   غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = le16_مقصد_cpu(readw((متلون خالی *)(srm->addr + reg)));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_le16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				    غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writew(cpu_مقصد_le16(val), (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_le32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				   غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = le32_مقصد_cpu(readl((متلون خالی *)(srm->addr + reg)));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_le32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				    غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_le32(val), (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_be16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				   غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = be16_مقصد_cpu(readl((متلون خالی *)(srm->addr + reg)));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_be16(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				    غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_be16(val), (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_read_be32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				   غیرعلامتی صحیح *val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = be32_مقصد_cpu(readl((متلون خالی *)(srm->addr + reg)));
	واپس 0;
}

ساکن صحیح regmap_syscon_write_be32(ساخت regmap *rmap, غیرعلامتی صحیح reg,
				    غیرعلامتی صحیح val)
{
	ساخت syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_be32(val), (متلون خالی *)(srm->addr + reg));
	واپس 0;
}

ساکن صحیح regmap_syscon_init(ثابت خالی *fdt, صحیح nodeoff,
			      ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت syscon_regmap *srm;
	uint64_t addr, سائز;
	ثابت fdt32_t *val;
	صحیح rc, len;

	srm = sbi_zalloc(حجم(*srm));
	اگر (!srm)
		واپس SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_چوڑائی = val ? fdt32_مقصد_cpu(*val) : 4;

	اگر (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ورنہ اگر (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ورنہ اگر (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ورنہ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_گرہ_addr_سائز(fdt, nodeoff, 0, &addr, &سائز);
	اگر (rc)
		جاؤ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_سرکاؤ = 0;
	srm->rmap.reg_stride = srm->reg_io_چوڑائی;
	srm->rmap.reg_بنیاد = 0;
	srm->rmap.reg_max = سائز / srm->reg_io_چوڑائی;
	انتخاب (srm->reg_io_چوڑائی) {
	حالت 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		توڑو;
	حالت 2:
		انتخاب (srm->reg_endian) {
		حالت SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			توڑو;
		حالت SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			توڑو;
		حالت SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			توڑو;
		طےشدہ:
			rc = SBI_EINVAL;
			جاؤ fail_free_syscon;
		}
		توڑو;
	حالت 4:
		انتخاب (srm->reg_endian) {
		حالت SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			توڑو;
		حالت SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			توڑو;
		حالت SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			توڑو;
		طےشدہ:
			rc = SBI_EINVAL;
			جاؤ fail_free_syscon;
		}
		توڑو;
	طےشدہ:
		rc = SBI_EINVAL;
		جاؤ fail_free_syscon;
	}

	rc = sbi_domain_جڑ_جوڑو_memrange(addr, سائز, صفحہ_سائز,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (rc)
		جاؤ fail_free_syscon;

	rc = regmap_جوڑو(&srm->rmap);
	اگر (rc)
		جاؤ fail_free_syscon;

	واپس 0;

fail_free_syscon:
	sbi_free(srm);
	واپس rc;
}

ساکن ثابت ساخت fdt_مطابقت regmap_syscon_مطابقت[] = {
	{ .compatible = "syscon" },
	{ },
};

ثابت ساخت fdt_driver fdt_regmap_syscon = {
	.مطابقت_جدول = regmap_syscon_مطابقت,
	.init = regmap_syscon_init,
};
