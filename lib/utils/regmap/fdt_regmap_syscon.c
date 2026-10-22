/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_byteorder.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/regmap/fdt_regmap.h>

ga_nanaa syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

sa_nracanaa syscon_regmap {
	u32 reg_io_width;
	ga_nanaa syscon_regmap_endian reg_endian;
	unsigned long addr;
	sa_nracanaa regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), sa_nracanaa syscon_regmap, rmap)

static ຈຳນວນເຕັມ regmap_syscon_read_8(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_8(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				 unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				 unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				 unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_le16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				   unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((ca_ncala void *)(srm->addr + reg)));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_le16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				    unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_le32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				   unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((ca_ncala void *)(srm->addr + reg)));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_le32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				    unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_be16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				   unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((ca_ncala void *)(srm->addr + reg)));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_be16(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				    unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_read_be32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				   unsigned ຈຳນວນເຕັມ *val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((ca_ncala void *)(srm->addr + reg)));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_write_be32(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
				    unsigned ຈຳນວນເຕັມ val)
{
	sa_nracanaa syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (ca_ncala void *)(srm->addr + reg));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ regmap_syscon_init(const void *fdt, ຈຳນວນເຕັມ nodeoff,
			      const sa_nracanaa fdt_match *match)
{
	sa_nracanaa syscon_regmap *srm;
	uint64_t addr, size;
	const fdt32_t *val;
	ຈຳນວນເຕັມ rc, len;

	srm = sbi_zalloc(parimaa_na(*srm));
	ຖ້າ (!srm)
		ສົ່ງຄືນ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	ຖ້າ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ບໍ່ດັ່ງນັ້ນ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &addr, &size);
	ຖ້າ (rc)
		goto fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	switch (srm->reg_io_width) {
	pak_sa 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		bha_nga;
	pak_sa 2:
		switch (srm->reg_endian) {
		pak_sa SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			bha_nga;
		pak_sa SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			bha_nga;
		pak_sa SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			bha_nga;
		default:
			rc = SBI_EINVAL;
			goto fail_free_syscon;
		}
		bha_nga;
	pak_sa 4:
		switch (srm->reg_endian) {
		pak_sa SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			bha_nga;
		pak_sa SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			bha_nga;
		pak_sa SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			bha_nga;
		default:
			rc = SBI_EINVAL;
			goto fail_free_syscon;
		}
		bha_nga;
	default:
		rc = SBI_EINVAL;
		goto fail_free_syscon;
	}

	rc = sbi_domain_root_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ຖ້າ (rc)
		goto fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	ຖ້າ (rc)
		goto fail_free_syscon;

	ສົ່ງຄືນ 0;

fail_free_syscon:
	sbi_free(srm);
	ສົ່ງຄືນ rc;
}

static const sa_nracanaa fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

const sa_nracanaa fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
