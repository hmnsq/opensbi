/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_byteorder.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাব <sbi_utils/regmap/fdt_regmap.h>

গণনা syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

কাঠামো syscon_regmap {
	u32 reg_io_width;
	গণনা syscon_regmap_endian reg_endian;
	unsigned long addr;
	কাঠামো regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), কাঠামো syscon_regmap, rmap)

স্থির পূর্ণসংখ্যা regmap_syscon_read_8(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_8(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				 unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				 unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				 unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_le16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				   unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((অস্থির শূন্য *)(srm->addr + reg)));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_le16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				    unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_le32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				   unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((অস্থির শূন্য *)(srm->addr + reg)));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_le32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				    unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_be16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				   unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((অস্থির শূন্য *)(srm->addr + reg)));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_be16(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				    unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_read_be32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				   unsigned পূর্ণসংখ্যা *val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((অস্থির শূন্য *)(srm->addr + reg)));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_write_be32(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
				    unsigned পূর্ণসংখ্যা val)
{
	কাঠামো syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (অস্থির শূন্য *)(srm->addr + reg));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা regmap_syscon_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff,
			      ধ্রুবক কাঠামো fdt_match *match)
{
	কাঠামো syscon_regmap *srm;
	uint64_t addr, আকার_2;
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা rc, len;

	srm = sbi_zalloc(আকার(*srm));
	যদি (!srm)
		ফেরত SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	যদি (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	অন্যথা যদি (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	অন্যথা যদি (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	অন্যথা
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &addr, &আকার_2);
	যদি (rc)
		যাও fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = আকার_2 / srm->reg_io_width;
	নির্বাচন (srm->reg_io_width) {
	ক্ষেত্র 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		বিরতি;
	ক্ষেত্র 2:
		নির্বাচন (srm->reg_endian) {
		ক্ষেত্র SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			বিরতি;
		ক্ষেত্র SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			বিরতি;
		ক্ষেত্র SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			বিরতি;
		পূর্বনির্ধারিত:
			rc = SBI_EINVAL;
			যাও fail_free_syscon;
		}
		বিরতি;
	ক্ষেত্র 4:
		নির্বাচন (srm->reg_endian) {
		ক্ষেত্র SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			বিরতি;
		ক্ষেত্র SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			বিরতি;
		ক্ষেত্র SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			বিরতি;
		পূর্বনির্ধারিত:
			rc = SBI_EINVAL;
			যাও fail_free_syscon;
		}
		বিরতি;
	পূর্বনির্ধারিত:
		rc = SBI_EINVAL;
		যাও fail_free_syscon;
	}

	rc = sbi_domain_মূল_add_memrange(addr, আকার_2, PAGE_আকার,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (rc)
		যাও fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	যদি (rc)
		যাও fail_free_syscon;

	ফেরত 0;

fail_free_syscon:
	sbi_free(srm);
	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
