/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_byteorder.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/regmap/fdt_regmap.h>

గణన syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

నిర్మాణం syscon_regmap {
	u32 reg_io_వెడల్పు;
	గణన syscon_regmap_endian reg_endian;
	గుర్తులేని దీర్ఘ addr;
	నిర్మాణం regmap rmap;
};

#నిర్వచించు గమ్యం_syscon_regmap(__rmap)	\
	container_యొక్క((__rmap), నిర్మాణం syscon_regmap, rmap)

స్థిర పూర్ణాంకం regmap_syscon_read_8(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = readb((అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_8(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				 గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writeb(val, (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = readw((అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				 గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writew(val, (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = readl((అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				 గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writel(val, (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_le16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				   గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = le16_గమ్యం_cpu(readw((అస్థిర శూన్య *)(srm->addr + reg)));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_le16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				    గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writew(cpu_గమ్యం_le16(val), (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_le32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				   గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = le32_గమ్యం_cpu(readl((అస్థిర శూన్య *)(srm->addr + reg)));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_le32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				    గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writel(cpu_గమ్యం_le32(val), (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_be16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				   గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = be16_గమ్యం_cpu(readl((అస్థిర శూన్య *)(srm->addr + reg)));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_be16(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				    గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writel(cpu_గమ్యం_be16(val), (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_read_be32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				   గుర్తులేని పూర్ణాంకం *val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	*val = be32_గమ్యం_cpu(readl((అస్థిర శూన్య *)(srm->addr + reg)));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_write_be32(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
				    గుర్తులేని పూర్ణాంకం val)
{
	నిర్మాణం syscon_regmap *srm = గమ్యం_syscon_regmap(rmap);

	writel(cpu_గమ్యం_be32(val), (అస్థిర శూన్య *)(srm->addr + reg));
	తిరిగి 0;
}

స్థిర పూర్ణాంకం regmap_syscon_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
			      స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం syscon_regmap *srm;
	uint64_t addr, కొలత;
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం rc, len;

	srm = sbi_zalloc(పరిమాణం(*srm));
	ఒకవేళ (!srm)
		తిరిగి SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_వెడల్పు = val ? fdt32_గమ్యం_cpu(*val) : 4;

	ఒకవేళ (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	లేకపోతే ఒకవేళ (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	లేకపోతే ఒకవేళ (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	లేకపోతే
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &addr, &కొలత);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_జరుపు = 0;
	srm->rmap.reg_stride = srm->reg_io_వెడల్పు;
	srm->rmap.reg_పునాది = 0;
	srm->rmap.reg_max = కొలత / srm->reg_io_వెడల్పు;
	ఎంపిక (srm->reg_io_వెడల్పు) {
	స్థితి 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		ఆపు;
	స్థితి 2:
		ఎంపిక (srm->reg_endian) {
		స్థితి SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			ఆపు;
		స్థితి SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			ఆపు;
		స్థితి SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			ఆపు;
		అప్రమేయ:
			rc = SBI_EINVAL;
			వెళ్ళు fail_free_syscon;
		}
		ఆపు;
	స్థితి 4:
		ఎంపిక (srm->reg_endian) {
		స్థితి SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			ఆపు;
		స్థితి SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			ఆపు;
		స్థితి SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			ఆపు;
		అప్రమేయ:
			rc = SBI_EINVAL;
			వెళ్ళు fail_free_syscon;
		}
		ఆపు;
	అప్రమేయ:
		rc = SBI_EINVAL;
		వెళ్ళు fail_free_syscon;
	}

	rc = sbi_domain_మూలం_కూడు_memrange(addr, కొలత, పుట_కొలత,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	ఒకవేళ (rc)
		వెళ్ళు fail_free_syscon;

	rc = regmap_కూడు(&srm->rmap);
	ఒకవేళ (rc)
		వెళ్ళు fail_free_syscon;

	తిరిగి 0;

fail_free_syscon:
	sbi_free(srm);
	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక regmap_syscon_సరిపోలిక[] = {
	{ .compatible = "syscon" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_regmap_syscon = {
	.సరిపోలిక_పట్టిక = regmap_syscon_సరిపోలిక,
	.init = regmap_syscon_init,
};
