/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_byteorder.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/regmap/fdt_regmap.h>

எண்ணிக்கை syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

கட்டமைப்பு syscon_regmap {
	u32 reg_io_width;
	எண்ணிக்கை syscon_regmap_endian reg_endian;
	unsigned long addr;
	கட்டமைப்பு regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), கட்டமைப்பு syscon_regmap, rmap)

நிலையான முழு regmap_syscon_read_8(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_8(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				 unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				 unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				 unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_le16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				   unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((நிலையற்ற வெறுமை *)(srm->addr + reg)));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_le16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				    unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_le32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				   unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((நிலையற்ற வெறுமை *)(srm->addr + reg)));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_le32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				    unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_be16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				   unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((நிலையற்ற வெறுமை *)(srm->addr + reg)));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_be16(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				    unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_read_be32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				   unsigned முழு *val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((நிலையற்ற வெறுமை *)(srm->addr + reg)));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_write_be32(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
				    unsigned முழு val)
{
	கட்டமைப்பு syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (நிலையற்ற வெறுமை *)(srm->addr + reg));
	திரும்பு 0;
}

நிலையான முழு regmap_syscon_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
			      மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு syscon_regmap *srm;
	uint64_t addr, size;
	மாறிலி fdt32_t *val;
	முழு rc, len;

	srm = sbi_zalloc(அளவு(*srm));
	எனில் (!srm)
		திரும்பு SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	எனில் (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	இல்லையெனில் எனில் (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	இல்லையெனில் எனில் (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	இல்லையெனில்
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &addr, &size);
	எனில் (rc)
		செல் fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	தேர்வு (srm->reg_io_width) {
	நிலை 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		நிறுத்து;
	நிலை 2:
		தேர்வு (srm->reg_endian) {
		நிலை SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			நிறுத்து;
		நிலை SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			நிறுத்து;
		நிலை SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			நிறுத்து;
		இயல்பு:
			rc = SBI_EINVAL;
			செல் fail_free_syscon;
		}
		நிறுத்து;
	நிலை 4:
		தேர்வு (srm->reg_endian) {
		நிலை SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			நிறுத்து;
		நிலை SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			நிறுத்து;
		நிலை SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			நிறுத்து;
		இயல்பு:
			rc = SBI_EINVAL;
			செல் fail_free_syscon;
		}
		நிறுத்து;
	இயல்பு:
		rc = SBI_EINVAL;
		செல் fail_free_syscon;
	}

	rc = sbi_domain_வேர்_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	எனில் (rc)
		செல் fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	எனில் (rc)
		செல் fail_free_syscon;

	திரும்பு 0;

fail_free_syscon:
	sbi_free(srm);
	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
