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

شمارش syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ساختار syscon_regmap {
	u32 reg_io_width;
	شمارش syscon_regmap_endian reg_endian;
	unsigned long addr;
	ساختار regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), ساختار syscon_regmap, rmap)

ایستا صحیح regmap_syscon_read_8(ساختار regmap *rmap, unsigned صحیح reg,
				unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_8(ساختار regmap *rmap, unsigned صحیح reg,
				 unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_16(ساختار regmap *rmap, unsigned صحیح reg,
				unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_16(ساختار regmap *rmap, unsigned صحیح reg,
				 unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_32(ساختار regmap *rmap, unsigned صحیح reg,
				unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_32(ساختار regmap *rmap, unsigned صحیح reg,
				 unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_le16(ساختار regmap *rmap, unsigned صحیح reg,
				   unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((فرار تهی *)(srm->addr + reg)));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_le16(ساختار regmap *rmap, unsigned صحیح reg,
				    unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_le32(ساختار regmap *rmap, unsigned صحیح reg,
				   unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((فرار تهی *)(srm->addr + reg)));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_le32(ساختار regmap *rmap, unsigned صحیح reg,
				    unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_be16(ساختار regmap *rmap, unsigned صحیح reg,
				   unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((فرار تهی *)(srm->addr + reg)));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_be16(ساختار regmap *rmap, unsigned صحیح reg,
				    unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_read_be32(ساختار regmap *rmap, unsigned صحیح reg,
				   unsigned صحیح *val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((فرار تهی *)(srm->addr + reg)));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_write_be32(ساختار regmap *rmap, unsigned صحیح reg,
				    unsigned صحیح val)
{
	ساختار syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (فرار تهی *)(srm->addr + reg));
	بازگشت 0;
}

ایستا صحیح regmap_syscon_init(ثابت تهی *fdt, صحیح nodeoff,
			      ثابت ساختار fdt_match *match)
{
	ساختار syscon_regmap *srm;
	uint64_t addr, size;
	ثابت fdt32_t *val;
	صحیح rc, len;

	srm = sbi_zalloc(اندازه(*srm));
	اگر (!srm)
		بازگشت SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	اگر (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	وگرنه اگر (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	وگرنه اگر (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	وگرنه
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &addr, &size);
	اگر (rc)
		برو fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	انتخاب (srm->reg_io_width) {
	حالت 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		بشکن;
	حالت 2:
		انتخاب (srm->reg_endian) {
		حالت SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			بشکن;
		حالت SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			بشکن;
		حالت SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			بشکن;
		پیشفرض:
			rc = SBI_EINVAL;
			برو fail_free_syscon;
		}
		بشکن;
	حالت 4:
		انتخاب (srm->reg_endian) {
		حالت SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			بشکن;
		حالت SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			بشکن;
		حالت SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			بشکن;
		پیشفرض:
			rc = SBI_EINVAL;
			برو fail_free_syscon;
		}
		بشکن;
	پیشفرض:
		rc = SBI_EINVAL;
		برو fail_free_syscon;
	}

	rc = sbi_domain_ریشه_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	اگر (rc)
		برو fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	اگر (rc)
		برو fail_free_syscon;

	بازگشت 0;

fail_free_syscon:
	sbi_free(srm);
	بازگشت rc;
}

ایستا ثابت ساختار fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ثابت ساختار fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
