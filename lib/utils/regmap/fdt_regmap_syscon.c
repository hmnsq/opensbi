/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <libfdt.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_byteorder.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/regmap/fdt_regmap.h>

تعداد syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

بنية syscon_regmap {
	u32 reg_io_عرض;
	تعداد syscon_regmap_endian reg_endian;
	بیعلامت دراز addr;
	بنية regmap rmap;
};

#تعریف_کن مقصد_syscon_regmap(__rmap)	\
	container_از((__rmap), بنية syscon_regmap, rmap)

ساكن صحيح regmap_syscon_read_8(بنية regmap *rmap, بیعلامت صحيح reg,
				بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readb((متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_8(بنية regmap *rmap, بیعلامت صحيح reg,
				 بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writeb(val, (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_16(بنية regmap *rmap, بیعلامت صحيح reg,
				بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readw((متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_16(بنية regmap *rmap, بیعلامت صحيح reg,
				 بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writew(val, (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_32(بنية regmap *rmap, بیعلامت صحيح reg,
				بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = readl((متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_32(بنية regmap *rmap, بیعلامت صحيح reg,
				 بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(val, (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_le16(بنية regmap *rmap, بیعلامت صحيح reg,
				   بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = le16_مقصد_cpu(readw((متطاير فراغ *)(srm->addr + reg)));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_le16(بنية regmap *rmap, بیعلامت صحيح reg,
				    بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writew(cpu_مقصد_le16(val), (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_le32(بنية regmap *rmap, بیعلامت صحيح reg,
				   بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = le32_مقصد_cpu(readl((متطاير فراغ *)(srm->addr + reg)));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_le32(بنية regmap *rmap, بیعلامت صحيح reg,
				    بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_le32(val), (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_be16(بنية regmap *rmap, بیعلامت صحيح reg,
				   بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = be16_مقصد_cpu(readl((متطاير فراغ *)(srm->addr + reg)));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_be16(بنية regmap *rmap, بیعلامت صحيح reg,
				    بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_be16(val), (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_read_be32(بنية regmap *rmap, بیعلامت صحيح reg,
				   بیعلامت صحيح *val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	*val = be32_مقصد_cpu(readl((متطاير فراغ *)(srm->addr + reg)));
	أرجع 0;
}

ساكن صحيح regmap_syscon_write_be32(بنية regmap *rmap, بیعلامت صحيح reg,
				    بیعلامت صحيح val)
{
	بنية syscon_regmap *srm = مقصد_syscon_regmap(rmap);

	writel(cpu_مقصد_be32(val), (متطاير فراغ *)(srm->addr + reg));
	أرجع 0;
}

ساكن صحيح regmap_syscon_init(ثابت فراغ *fdt, صحيح nodeoff,
			      ثابت بنية fdt_تطابق *تطابق)
{
	بنية syscon_regmap *srm;
	uint64_t addr, اندازه;
	ثابت fdt32_t *val;
	صحيح rc, len;

	srm = sbi_zalloc(حجم(*srm));
	إذا (!srm)
		أرجع SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_عرض = val ? fdt32_مقصد_cpu(*val) : 4;

	إذا (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	وإلا إذا (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	وإلا إذا (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	وإلا
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_گره_addr_اندازه(fdt, nodeoff, 0, &addr, &اندازه);
	إذا (rc)
		اذهب fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_لغزش = 0;
	srm->rmap.reg_stride = srm->reg_io_عرض;
	srm->rmap.reg_پایه = 0;
	srm->rmap.reg_max = اندازه / srm->reg_io_عرض;
	بدل (srm->reg_io_عرض) {
	حالة 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		اكسر;
	حالة 2:
		بدل (srm->reg_endian) {
		حالة SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			اكسر;
		حالة SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			اكسر;
		حالة SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			اكسر;
		افتراضي:
			rc = SBI_EINVAL;
			اذهب fail_free_syscon;
		}
		اكسر;
	حالة 4:
		بدل (srm->reg_endian) {
		حالة SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			اكسر;
		حالة SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			اكسر;
		حالة SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			اكسر;
		افتراضي:
			rc = SBI_EINVAL;
			اذهب fail_free_syscon;
		}
		اكسر;
	افتراضي:
		rc = SBI_EINVAL;
		اذهب fail_free_syscon;
	}

	rc = sbi_domain_ریشه_جمع_کن_memrange(addr, اندازه, صفحه_اندازه,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	إذا (rc)
		اذهب fail_free_syscon;

	rc = regmap_جمع_کن(&srm->rmap);
	إذا (rc)
		اذهب fail_free_syscon;

	أرجع 0;

fail_free_syscon:
	sbi_free(srm);
	أرجع rc;
}

ساكن ثابت بنية fdt_تطابق regmap_syscon_تطابق[] = {
	{ .compatible = "syscon" },
	{ },
};

ثابت بنية fdt_driver fdt_regmap_syscon = {
	.تطابق_جدول = regmap_syscon_تطابق,
	.init = regmap_syscon_init,
};
