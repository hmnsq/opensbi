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

ಗಣನಾ syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ಸಂರಚನಾ syscon_regmap {
	u32 reg_io_width;
	ಗಣನಾ syscon_regmap_endian reg_endian;
	unsigned long addr;
	ಸಂರಚನಾ regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), ಸಂರಚನಾ syscon_regmap, rmap)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_8(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_8(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				 unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				 unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				 unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_le16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				   unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_le16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				    unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_le32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				   unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_le32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				    unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_be16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				   unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_be16(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				    unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_read_be32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				   unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg)));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_write_be32(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
				    unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಸಂರಚನಾ syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (ಚಞ್ಚಲ ಶೂನ್ಯ *)(srm->addr + reg));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ regmap_syscon_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
			      ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ syscon_regmap *srm;
	uint64_t addr, size;
	ಧ್ರುವ fdt32_t *val;
	ಪೂರ್ಣಾಙ್ಕ rc, len;

	srm = sbi_zalloc(ಪರಿಮಾಣ(*srm));
	ಯದಿ (!srm)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	ಯದಿ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ಅನ್ಯಥಾ ಯದಿ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ಅನ್ಯಥಾ ಯದಿ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ಅನ್ಯಥಾ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, 0, &addr, &size);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	ವಿಕಲ್ಪ (srm->reg_io_width) {
	ಪಕ್ಷ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ಭಙ್ಗ;
	ಪಕ್ಷ 2:
		ವಿಕಲ್ಪ (srm->reg_endian) {
		ಪಕ್ಷ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ಭಙ್ಗ;
		ಪಕ್ಷ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ಭಙ್ಗ;
		ಪಕ್ಷ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			rc = SBI_EINVAL;
			ಗಚ್ಛ fail_free_syscon;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 4:
		ವಿಕಲ್ಪ (srm->reg_endian) {
		ಪಕ್ಷ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ಭಙ್ಗ;
		ಪಕ್ಷ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ಭಙ್ಗ;
		ಪಕ್ಷ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			rc = SBI_EINVAL;
			ಗಚ್ಛ fail_free_syscon;
		}
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		rc = SBI_EINVAL;
		ಗಚ್ಛ fail_free_syscon;
	}

	rc = sbi_domain_ಬೇರು_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	ಯದಿ (rc)
		ಗಚ್ಛ fail_free_syscon;

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_syscon:
	sbi_free(srm);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
