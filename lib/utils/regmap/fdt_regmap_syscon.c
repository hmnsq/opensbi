/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_byteorder.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/regmap/fdt_regmap.h>

എണ്ണൽ syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ഘടന syscon_regmap {
	u32 reg_io_width;
	എണ്ണൽ syscon_regmap_endian reg_endian;
	unsigned long addr;
	ഘടന regmap rmap;
};

#define to_syscon_regmap(__rmap)	\
	container_of((__rmap), ഘടന syscon_regmap, rmap)

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_8(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_8(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				 unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				 unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				 unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_le16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				   unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((അസ്ഥിര ശൂന്യം *)(srm->addr + reg)));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_le16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				    unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_le32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				   unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((അസ്ഥിര ശൂന്യം *)(srm->addr + reg)));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_le32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				    unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_be16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				   unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((അസ്ഥിര ശൂന്യം *)(srm->addr + reg)));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_be16(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				    unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_read_be32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				   unsigned പൂർണ്ണസംഖ്യ *val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((അസ്ഥിര ശൂന്യം *)(srm->addr + reg)));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_write_be32(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
				    unsigned പൂർണ്ണസംഖ്യ val)
{
	ഘടന syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (അസ്ഥിര ശൂന്യം *)(srm->addr + reg));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ regmap_syscon_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
			      സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന syscon_regmap *srm;
	uint64_t addr, size;
	സ്ഥിരം fdt32_t *val;
	പൂർണ്ണസംഖ്യ rc, len;

	srm = sbi_zalloc(വലുപ്പം(*srm));
	എങ്കിൽ (!srm)
		തിരികെ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	എങ്കിൽ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	അല്ലെങ്കിൽ എങ്കിൽ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	അല്ലെങ്കിൽ എങ്കിൽ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	അല്ലെങ്കിൽ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &addr, &size);
	എങ്കിൽ (rc)
		പോകുക fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = size / srm->reg_io_width;
	തിരഞ്ഞെടുപ്പ് (srm->reg_io_width) {
	അവസ്ഥ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		നിർത്തുക;
	അവസ്ഥ 2:
		തിരഞ്ഞെടുപ്പ് (srm->reg_endian) {
		അവസ്ഥ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			നിർത്തുക;
		അവസ്ഥ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			നിർത്തുക;
		അവസ്ഥ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			rc = SBI_EINVAL;
			പോകുക fail_free_syscon;
		}
		നിർത്തുക;
	അവസ്ഥ 4:
		തിരഞ്ഞെടുപ്പ് (srm->reg_endian) {
		അവസ്ഥ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			നിർത്തുക;
		അവസ്ഥ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			നിർത്തുക;
		അവസ്ഥ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			rc = SBI_EINVAL;
			പോകുക fail_free_syscon;
		}
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		rc = SBI_EINVAL;
		പോകുക fail_free_syscon;
	}

	rc = sbi_domain_വേര്_add_memrange(addr, size, PAGE_SIZE,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	എങ്കിൽ (rc)
		പോകുക fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	എങ്കിൽ (rc)
		പോകുക fail_free_syscon;

	തിരികെ 0;

fail_free_syscon:
	sbi_free(srm);
	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
