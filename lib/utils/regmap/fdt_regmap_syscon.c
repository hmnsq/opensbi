/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/regmap/fdt_regmap.h>

गणना syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

संरचना syscon_regmap {
	u32 reg_io_width;
	गणना syscon_regmap_endian reg_endian;
	unsigned long addr;
	संरचना regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), संरचना syscon_regmap, rmap)

स्थिर पूर्णाङ्क regmap_syscon_read_8(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_8(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				 unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				 unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				 unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_le16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				   unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((चञ्चल शून्य *)(srm->addr + reg)));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_le16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				    unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_le32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				   unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_le32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				    unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_be16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				   unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_be16(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				    unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_be32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				   unsigned पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_be32(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
				    unsigned पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (चञ्चल शून्य *)(srm->addr + reg));
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क regmap_syscon_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_match *match)
{
	संरचना syscon_regmap *srm;
	uint64_t addr, size;
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, len;

	srm = sbi_zalloc(परिमाण(*srm));
	सचे (!srm)
		पटिनिवत्त SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	सचे (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	अञ्ञथा सचे (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	अञ्ञथा सचे (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	अञ्ञथा
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &addr, &size);
	सचे (rc)
		गच्छ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	विकल्प (srm->reg_io_width) {
	पक्ष 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		भङ्ग;
	पक्ष 2:
		विकल्प (srm->reg_endian) {
		पक्ष SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			भङ्ग;
		पक्ष SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			भङ्ग;
		पक्ष SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			भङ्ग;
		स्वभाव:
			rc = SBI_EINVAL;
			गच्छ fail_free_syscon;
		}
		भङ्ग;
	पक्ष 4:
		विकल्प (srm->reg_endian) {
		पक्ष SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			भङ्ग;
		पक्ष SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			भङ्ग;
		पक्ष SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			भङ्ग;
		स्वभाव:
			rc = SBI_EINVAL;
			गच्छ fail_free_syscon;
		}
		भङ्ग;
	स्वभाव:
		rc = SBI_EINVAL;
		गच्छ fail_free_syscon;
	}

	rc = sbi_domain_मूलम्_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	सचे (rc)
		गच्छ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	सचे (rc)
		गच्छ fail_free_syscon;

	पटिनिवत्त 0;

fail_free_syscon:
	sbi_free(srm);
	पटिनिवत्त rc;
}

स्थिर ध्रुव संरचना fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
