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
	अचिन्हित दीर्घ addr;
	संरचना regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), संरचना syscon_regmap, rmap)

स्थिर पूर्णांक regmap_syscon_read_8(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_8(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_le16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((अस्थिर शून्य *)(srm->addr + reg)));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_le16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_le32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((अस्थिर शून्य *)(srm->addr + reg)));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_le32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_be16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((अस्थिर शून्य *)(srm->addr + reg)));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_be16(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_read_be32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((अस्थिर शून्य *)(srm->addr + reg)));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_write_be32(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	संरचना syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (अस्थिर शून्य *)(srm->addr + reg));
	परत 0;
}

स्थिर पूर्णांक regmap_syscon_init(स्थिरांक शून्य *fdt, पूर्णांक nodeoff,
			      स्थिरांक संरचना fdt_match *match)
{
	संरचना syscon_regmap *srm;
	uint64_t addr, size;
	स्थिरांक fdt32_t *val;
	पूर्णांक rc, len;

	srm = sbi_zalloc(आकार(*srm));
	जर (!srm)
		परत SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	जर (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	अन्यथा जर (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	अन्यथा जर (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	अन्यथा
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_गाठ_addr_size(fdt, nodeoff, 0, &addr, &size);
	जर (rc)
		जा fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	निवड (srm->reg_io_width) {
	स्थिती 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		थांबा;
	स्थिती 2:
		निवड (srm->reg_endian) {
		स्थिती SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			थांबा;
		स्थिती SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			थांबा;
		स्थिती SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			थांबा;
		पूर्वनिर्धारित:
			rc = SBI_EINVAL;
			जा fail_free_syscon;
		}
		थांबा;
	स्थिती 4:
		निवड (srm->reg_endian) {
		स्थिती SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			थांबा;
		स्थिती SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			थांबा;
		स्थिती SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			थांबा;
		पूर्वनिर्धारित:
			rc = SBI_EINVAL;
			जा fail_free_syscon;
		}
		थांबा;
	पूर्वनिर्धारित:
		rc = SBI_EINVAL;
		जा fail_free_syscon;
	}

	rc = sbi_domain_मूळ_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	जर (rc)
		जा fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	जर (rc)
		जा fail_free_syscon;

	परत 0;

fail_free_syscon:
	sbi_free(srm);
	परत rc;
}

स्थिर स्थिरांक संरचना fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
