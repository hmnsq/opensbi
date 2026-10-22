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
	u32 reg_io_विस्तारः;
	गणना syscon_regmap_endian reg_endian;
	अचिह्नित दीर्घ addr;
	संरचना regmap rmap;
};

#परिभाषय गन्तव्यम्_syscon_regmap(__rmap)	\
	container_सम्बन्धि((__rmap), संरचना syscon_regmap, rmap)

स्थिर पूर्णाङ्क regmap_syscon_read_8(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = readb((चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_8(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				 अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writeb(val, (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = readw((चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				 अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writew(val, (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = readl((चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				 अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writel(val, (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_le16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				   अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = le16_गन्तव्यम्_cpu(readw((चञ्चल शून्य *)(srm->addr + reg)));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_le16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				    अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writew(cpu_गन्तव्यम्_le16(val), (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_le32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				   अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = le32_गन्तव्यम्_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_le32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				    अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writel(cpu_गन्तव्यम्_le32(val), (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_be16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				   अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = be16_गन्तव्यम्_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_be16(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				    अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writel(cpu_गन्तव्यम्_be16(val), (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_read_be32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				   अचिह्नित पूर्णाङ्क *val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	*val = be32_गन्तव्यम्_cpu(readl((चञ्चल शून्य *)(srm->addr + reg)));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_write_be32(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
				    अचिह्नित पूर्णाङ्क val)
{
	संरचना syscon_regmap *srm = गन्तव्यम्_syscon_regmap(rmap);

	writel(cpu_गन्तव्यम्_be32(val), (चञ्चल शून्य *)(srm->addr + reg));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क regmap_syscon_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
			      ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना syscon_regmap *srm;
	uint64_t addr, आकार;
	ध्रुव fdt32_t *val;
	पूर्णाङ्क rc, len;

	srm = sbi_zalloc(परिमाण(*srm));
	यदि (!srm)
		प्रत्यागम SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_विस्तारः = val ? fdt32_गन्तव्यम्_cpu(*val) : 4;

	यदि (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	अन्यथा यदि (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	अन्यथा यदि (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	अन्यथा
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, 0, &addr, &आकार);
	यदि (rc)
		गच्छ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_सरणम् = 0;
	srm->rmap.reg_stride = srm->reg_io_विस्तारः;
	srm->rmap.reg_आधारः = 0;
	srm->rmap.reg_max = आकार / srm->reg_io_विस्तारः;
	विकल्प (srm->reg_io_विस्तारः) {
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

	rc = sbi_domain_मूलम्_सङ्कलय_memrange(addr, आकार, PAGE_आकार,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	यदि (rc)
		गच्छ fail_free_syscon;

	rc = regmap_सङ्कलय(&srm->rmap);
	यदि (rc)
		गच्छ fail_free_syscon;

	प्रत्यागम 0;

fail_free_syscon:
	sbi_free(srm);
	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना fdt_मेलनम् regmap_syscon_मेलनम्[] = {
	{ .compatible = "syscon" },
	{ },
};

ध्रुव संरचना fdt_driver fdt_regmap_syscon = {
	.मेलनम्_सारणी = regmap_syscon_मेलनम्,
	.init = regmap_syscon_init,
};
