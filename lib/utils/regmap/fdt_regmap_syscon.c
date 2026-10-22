/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_byteorder.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/regmap/fdt_regmap.h>

ગણના syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

સંરચના syscon_regmap {
	u32 reg_io_width;
	ગણના syscon_regmap_endian reg_endian;
	unsigned long addr;
	સંરચના regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), સંરચના syscon_regmap, rmap)

સ્થિર પૂર્ણાંક regmap_syscon_read_8(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_8(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				 unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				 unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				 unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_le16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				   unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((અસ્થિર શૂન્ય *)(srm->addr + reg)));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_le16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				    unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_le32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				   unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((અસ્થિર શૂન્ય *)(srm->addr + reg)));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_le32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				    unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_be16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				   unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((અસ્થિર શૂન્ય *)(srm->addr + reg)));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_be16(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				    unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_read_be32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				   unsigned પૂર્ણાંક *val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((અસ્થિર શૂન્ય *)(srm->addr + reg)));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_write_be32(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
				    unsigned પૂર્ણાંક val)
{
	સંરચના syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (અસ્થિર શૂન્ય *)(srm->addr + reg));
	પાછા 0;
}

સ્થિર પૂર્ણાંક regmap_syscon_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
			      અચલ સંરચના fdt_match *match)
{
	સંરચના syscon_regmap *srm;
	uint64_t addr, size;
	અચલ fdt32_t *val;
	પૂર્ણાંક rc, len;

	srm = sbi_zalloc(કદ(*srm));
	જો (!srm)
		પાછા SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	જો (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	નહીંતર જો (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	નહીંતર જો (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	નહીંતર
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &addr, &size);
	જો (rc)
		જાઓ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	પસંદગી (srm->reg_io_width) {
	સ્થિતિ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		તોડો;
	સ્થિતિ 2:
		પસંદગી (srm->reg_endian) {
		સ્થિતિ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			તોડો;
		સ્થિતિ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			તોડો;
		સ્થિતિ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			તોડો;
		મૂળભૂત:
			rc = SBI_EINVAL;
			જાઓ fail_free_syscon;
		}
		તોડો;
	સ્થિતિ 4:
		પસંદગી (srm->reg_endian) {
		સ્થિતિ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			તોડો;
		સ્થિતિ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			તોડો;
		સ્થિતિ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			તોડો;
		મૂળભૂત:
			rc = SBI_EINVAL;
			જાઓ fail_free_syscon;
		}
		તોડો;
	મૂળભૂત:
		rc = SBI_EINVAL;
		જાઓ fail_free_syscon;
	}

	rc = sbi_domain_મૂળ_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	જો (rc)
		જાઓ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	જો (rc)
		જાઓ fail_free_syscon;

	પાછા 0;

fail_free_syscon:
	sbi_free(srm);
	પાછા rc;
}

સ્થિર અચલ સંરચના fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

અચલ સંરચના fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
