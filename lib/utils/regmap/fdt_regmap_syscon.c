/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_byteorder.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/regmap/fdt_regmap.h>

ਗਿਣਤੀ syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

ਬਣਤਰ syscon_regmap {
	u32 reg_io_ਚੌੜਾਈ;
	ਗਿਣਤੀ syscon_regmap_endian reg_endian;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr;
	ਬਣਤਰ regmap rmap;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_syscon_regmap(__rmap)	\
	container_ਦਾ((__rmap), ਬਣਤਰ syscon_regmap, rmap)

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_8(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = readb((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_8(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writeb(val, (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = readw((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writew(val, (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = readl((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writel(val, (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_le16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = le16_ਮੰਜ਼ਿਲ_cpu(readw((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg)));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_le16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writew(cpu_ਮੰਜ਼ਿਲ_le16(val), (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_le32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = le32_ਮੰਜ਼ਿਲ_cpu(readl((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg)));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_le32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writel(cpu_ਮੰਜ਼ਿਲ_le32(val), (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_be16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = be16_ਮੰਜ਼ਿਲ_cpu(readl((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg)));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_be16(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writel(cpu_ਮੰਜ਼ਿਲ_be16(val), (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_read_be32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	*val = be32_ਮੰਜ਼ਿਲ_cpu(readl((ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg)));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_write_be32(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਬਣਤਰ syscon_regmap *srm = ਮੰਜ਼ਿਲ_syscon_regmap(rmap);

	writel(cpu_ਮੰਜ਼ਿਲ_be32(val), (ਅਸਥਿਰ ਖਾਲੀ *)(srm->addr + reg));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ regmap_syscon_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
			      ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ syscon_regmap *srm;
	uint64_t addr, ਮਾਪ;
	ਅਚਲ fdt32_t *val;
	ਪੂਰਨ rc, len;

	srm = sbi_zalloc(ਆਕਾਰ(*srm));
	ਜੇ (!srm)
		ਵਾਪਸ SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_ਚੌੜਾਈ = val ? fdt32_ਮੰਜ਼ਿਲ_cpu(*val) : 4;

	ਜੇ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	ਨਹੀਂਤਾਂ ਜੇ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	ਨਹੀਂਤਾਂ ਜੇ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	ਨਹੀਂਤਾਂ
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, 0, &addr, &ਮਾਪ);
	ਜੇ (rc)
		ਜਾਓ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_ਖਿਸਕਾਓ = 0;
	srm->rmap.reg_stride = srm->reg_io_ਚੌੜਾਈ;
	srm->rmap.reg_ਅਧਾਰ = 0;
	srm->rmap.reg_max = ਮਾਪ / srm->reg_io_ਚੌੜਾਈ;
	ਚੋਣ (srm->reg_io_ਚੌੜਾਈ) {
	ਹਾਲਤ 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ਰੋਕੋ;
	ਹਾਲਤ 2:
		ਚੋਣ (srm->reg_endian) {
		ਹਾਲਤ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ਰੋਕੋ;
		ਹਾਲਤ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ਰੋਕੋ;
		ਹਾਲਤ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ਰੋਕੋ;
		ਮੂਲ:
			rc = SBI_EINVAL;
			ਜਾਓ fail_free_syscon;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 4:
		ਚੋਣ (srm->reg_endian) {
		ਹਾਲਤ SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ਰੋਕੋ;
		ਹਾਲਤ SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ਰੋਕੋ;
		ਹਾਲਤ SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ਰੋਕੋ;
		ਮੂਲ:
			rc = SBI_EINVAL;
			ਜਾਓ fail_free_syscon;
		}
		ਰੋਕੋ;
	ਮੂਲ:
		rc = SBI_EINVAL;
		ਜਾਓ fail_free_syscon;
	}

	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(addr, ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ਜੇ (rc)
		ਜਾਓ fail_free_syscon;

	rc = regmap_ਜੋੜੋ(&srm->rmap);
	ਜੇ (rc)
		ਜਾਓ fail_free_syscon;

	ਵਾਪਸ 0;

fail_free_syscon:
	sbi_free(srm);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ regmap_syscon_ਮੇਲ[] = {
	{ .compatible = "syscon" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_regmap_syscon = {
	.ਮੇਲ_ਸਾਰਣੀ = regmap_syscon_ਮੇਲ,
	.init = regmap_syscon_init,
};
