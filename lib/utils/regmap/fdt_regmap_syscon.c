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

সংরচনা syscon_regmap {
	u32 reg_io_width;
	গণনা syscon_regmap_endian reg_endian;
	unsigned long addr;
	সংরচনা regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), সংরচনা syscon_regmap, rmap)

স্থির পূর্ণাঙ্ক regmap_syscon_read_8(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_8(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				 unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				 unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				 unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_le16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				   unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((চঞ্চল শূন্য *)(srm->addr + reg)));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_le16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				    unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_le32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				   unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((চঞ্চল শূন্য *)(srm->addr + reg)));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_le32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				    unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_be16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				   unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((চঞ্চল শূন্য *)(srm->addr + reg)));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_be16(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				    unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_read_be32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				   unsigned পূর্ণাঙ্ক *val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((চঞ্চল শূন্য *)(srm->addr + reg)));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_write_be32(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
				    unsigned পূর্ণাঙ্ক val)
{
	সংরচনা syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (চঞ্চল শূন্য *)(srm->addr + reg));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক regmap_syscon_init(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
			      ধ্রুব সংরচনা fdt_match *match)
{
	সংরচনা syscon_regmap *srm;
	uint64_t addr, size;
	ধ্রুব fdt32_t *val;
	পূর্ণাঙ্ক rc, len;

	srm = sbi_zalloc(পরিমাণ(*srm));
	যদি (!srm)
		প্রত্যাগম SBI_ENOMEM;

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

	rc = fdt_get_গ্রন্থি_addr_size(fdt, nodeoff, 0, &addr, &size);
	যদি (rc)
		গচ্ছ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	বিকল্প (srm->reg_io_width) {
	পক্ষ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ভঙ্গ;
	পক্ষ 2:
		বিকল্প (srm->reg_endian) {
		পক্ষ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ভঙ্গ;
		পক্ষ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ভঙ্গ;
		পক্ষ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ভঙ্গ;
		স্বভাব:
			rc = SBI_EINVAL;
			গচ্ছ fail_free_syscon;
		}
		ভঙ্গ;
	পক্ষ 4:
		বিকল্প (srm->reg_endian) {
		পক্ষ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ভঙ্গ;
		পক্ষ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ভঙ্গ;
		পক্ষ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ভঙ্গ;
		স্বভাব:
			rc = SBI_EINVAL;
			গচ্ছ fail_free_syscon;
		}
		ভঙ্গ;
	স্বভাব:
		rc = SBI_EINVAL;
		গচ্ছ fail_free_syscon;
	}

	rc = sbi_domain_মূল_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	যদি (rc)
		গচ্ছ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	যদি (rc)
		গচ্ছ fail_free_syscon;

	প্রত্যাগম 0;

fail_free_syscon:
	sbi_free(srm);
	প্রত্যাগম rc;
}

স্থির ধ্রুব সংরচনা fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ধ্রুব সংরচনা fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
