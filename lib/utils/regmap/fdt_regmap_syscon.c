/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_byteorder.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/regmap/fdt_regmap.h>

ଗଣନା syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ସଂରଚନା syscon_regmap {
	u32 reg_io_width;
	ଗଣନା syscon_regmap_endian reg_endian;
	unsigned long addr;
	ସଂରଚନା regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), ସଂରଚନା syscon_regmap, rmap)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_8(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_8(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_le16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg)));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_le16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_le32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg)));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_le32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_be16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg)));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_be16(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_read_be32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg)));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_write_be32(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
				    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ସଂରଚନା syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (ଅସ୍ଥିର ଶୂନ୍ୟ *)(srm->addr + reg));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_syscon_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
			      ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା syscon_regmap *srm;
	uint64_t addr, size;
	ଅଚଳ fdt32_t *val;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, len;

	srm = sbi_zalloc(ଆକାର(*srm));
	ଯଦି (!srm)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	ଯଦି (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ଅନ୍ୟଥା ଯଦି (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ଅନ୍ୟଥା ଯଦି (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ଅନ୍ୟଥା
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &addr, &size);
	ଯଦି (rc)
		ଯାଅ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	ବାଛ (srm->reg_io_width) {
	ସ୍ଥିତି 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 2:
		ବାଛ (srm->reg_endian) {
		ସ୍ଥିତି SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			rc = SBI_EINVAL;
			ଯାଅ fail_free_syscon;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 4:
		ବାଛ (srm->reg_endian) {
		ସ୍ଥିତି SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			rc = SBI_EINVAL;
			ଯାଅ fail_free_syscon;
		}
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		rc = SBI_EINVAL;
		ଯାଅ fail_free_syscon;
	}

	rc = sbi_domain_ମୂଳ_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ଯଦି (rc)
		ଯାଅ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	ଯଦି (rc)
		ଯାଅ fail_free_syscon;

	ଫେରନ୍ତୁ 0;

fail_free_syscon:
	sbi_free(srm);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
