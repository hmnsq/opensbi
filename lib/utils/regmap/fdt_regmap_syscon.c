/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_byteorder.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/regmap/fdt_regmap.h>

ಎಣಿಕೆ syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ರಚನೆ syscon_regmap {
	u32 reg_io_ಅಗಲ;
	ಎಣಿಕೆ syscon_regmap_endian reg_endian;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr;
	ರಚನೆ regmap rmap;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_syscon_regmap(__rmap)	\
	container_ದ((__rmap), ರಚನೆ syscon_regmap, rmap)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_8(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = readb((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_8(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writeb(val, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = readw((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writew(val, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writel(val, (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_le16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = le16_ಗಮ್ಯ_cpu(readw((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_le16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writew(cpu_ಗಮ್ಯ_le16(val), (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_le32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = le32_ಗಮ್ಯ_cpu(readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_le32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writel(cpu_ಗಮ್ಯ_le32(val), (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_be16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = be16_ಗಮ್ಯ_cpu(readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_be16(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writel(cpu_ಗಮ್ಯ_be16(val), (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_read_be32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	*val = be32_ಗಮ್ಯ_cpu(readl((ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_write_be32(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
				    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ರಚನೆ syscon_regmap *srm = ಗಮ್ಯ_syscon_regmap(rmap);

	writel(cpu_ಗಮ್ಯ_be32(val), (ಅಸ್ಥಿರ ಶೂನ್ಯ *)(srm->addr + reg));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regmap_syscon_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ syscon_regmap *srm;
	uint64_t addr, ಗಾತ್ರ_2;
	ಸ್ಥಿರಾಂಕ fdt32_t *val;
	ಪೂರ್ಣಾಂಕ rc, len;

	srm = sbi_zalloc(ಗಾತ್ರ(*srm));
	ಆದರೆ (!srm)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_ಅಗಲ = val ? fdt32_ಗಮ್ಯ_cpu(*val) : 4;

	ಆದರೆ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ಇಲ್ಲದಿದ್ದರೆ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, 0, &addr, &ಗಾತ್ರ_2);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_ಸರಿಸು = 0;
	srm->rmap.reg_stride = srm->reg_io_ಅಗಲ;
	srm->rmap.reg_ತಳ = 0;
	srm->rmap.reg_max = ಗಾತ್ರ_2 / srm->reg_io_ಅಗಲ;
	ಆಯ್ಕೆ (srm->reg_io_ಅಗಲ) {
	ಸ್ಥಿತಿ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2:
		ಆಯ್ಕೆ (srm->reg_endian) {
		ಸ್ಥಿತಿ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			rc = SBI_EINVAL;
			ಹೋಗು fail_free_syscon;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 4:
		ಆಯ್ಕೆ (srm->reg_endian) {
		ಸ್ಥಿತಿ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			rc = SBI_EINVAL;
			ಹೋಗು fail_free_syscon;
		}
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		rc = SBI_EINVAL;
		ಹೋಗು fail_free_syscon;
	}

	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(addr, ಗಾತ್ರ_2, ಪುಟ_ಗಾತ್ರ,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಆದರೆ (rc)
		ಹೋಗು fail_free_syscon;

	rc = regmap_ಕೂಡಿಸು(&srm->rmap);
	ಆದರೆ (rc)
		ಹೋಗು fail_free_syscon;

	ಹಿಂತಿರುಗಿ 0;

fail_free_syscon:
	sbi_free(srm);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ regmap_syscon_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "syscon" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_regmap_syscon = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = regmap_syscon_ಹೊಂದಿಕೆ,
	.init = regmap_syscon_init,
};
