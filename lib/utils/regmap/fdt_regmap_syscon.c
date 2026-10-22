/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <libfdt.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_byteorder.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/regmap/fdt_regmap.h>

क्रमागत syscon_regmap_endian {
	SYSCON_ENDIAN_NATIVE = 0,
	SYSCON_ENDIAN_LITTLE,
	SYSCON_ENDIAN_BIG,
	SYSCON_ENDIAN_MAX
};

काष्ठा syscon_regmap {
	u32 reg_io_width;
	क्रमागत syscon_regmap_endian reg_endian;
	अचिन्हित दीर्घ addr;
	काष्ठा regmap rmap;
};

#घोषणा to_syscon_regmap(__rmap)	\
	container_of((__rmap), काष्ठा syscon_regmap, rmap)

अटल पूर्णांक regmap_syscon_read_8(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readb((अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_8(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writeb(val, (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readw((अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(val, (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = readl((अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				 अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(val, (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_le16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le16_to_cpu(readw((अस्थिर व्योम *)(srm->addr + reg)));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_le16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writew(cpu_to_le16(val), (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_le32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = le32_to_cpu(readl((अस्थिर व्योम *)(srm->addr + reg)));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_le32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_le32(val), (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_be16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be16_to_cpu(readl((अस्थिर व्योम *)(srm->addr + reg)));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_be16(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be16(val), (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_read_be32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				   अचिन्हित पूर्णांक *val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	*val = be32_to_cpu(readl((अस्थिर व्योम *)(srm->addr + reg)));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_write_be32(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
				    अचिन्हित पूर्णांक val)
{
	काष्ठा syscon_regmap *srm = to_syscon_regmap(rmap);

	writel(cpu_to_be32(val), (अस्थिर व्योम *)(srm->addr + reg));
	वापस 0;
}

अटल पूर्णांक regmap_syscon_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
			      स्थिर काष्ठा fdt_match *match)
{
	काष्ठा syscon_regmap *srm;
	uint64_t addr, आकार;
	स्थिर fdt32_t *val;
	पूर्णांक rc, len;

	srm = sbi_zalloc(माप(*srm));
	अगर (!srm)
		वापस SBI_ENOMEM;

	val = fdt_getprop(fdt, nodeoff, "reg-io-width", &len);
	srm->reg_io_width = val ? fdt32_to_cpu(*val) : 4;

	अगर (fdt_getprop(fdt, nodeoff, "native-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;
	अन्यथा अगर (fdt_getprop(fdt, nodeoff, "little-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_LITTLE;
	अन्यथा अगर (fdt_getprop(fdt, nodeoff, "big-endian", &len))
		srm->reg_endian = SYSCON_ENDIAN_BIG;
	अन्यथा
		srm->reg_endian = SYSCON_ENDIAN_NATIVE;

	rc = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, 0, &addr, &आकार);
	अगर (rc)
		जाओ fail_free_syscon;
	srm->addr = addr;

	srm->rmap.id = nodeoff;
	srm->rmap.reg_shift = 0;
	srm->rmap.reg_stride = srm->reg_io_width;
	srm->rmap.reg_base = 0;
	srm->rmap.reg_max = आकार / srm->reg_io_width;
	चयन (srm->reg_io_width) {
	हाल 1:
		srm->rmap.reg_read = regmap_syscon_read_8;
		srm->rmap.reg_write = regmap_syscon_write_8;
		अवरोध;
	हाल 2:
		चयन (srm->reg_endian) {
		हाल SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_16;
			srm->rmap.reg_write = regmap_syscon_write_16;
			अवरोध;
		हाल SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le16;
			srm->rmap.reg_write = regmap_syscon_write_le16;
			अवरोध;
		हाल SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be16;
			srm->rmap.reg_write = regmap_syscon_write_be16;
			अवरोध;
		शेष:
			rc = SBI_EINVAL;
			जाओ fail_free_syscon;
		}
		अवरोध;
	हाल 4:
		चयन (srm->reg_endian) {
		हाल SYSCON_ENDIAN_NATIVE:
			srm->rmap.reg_read = regmap_syscon_read_32;
			srm->rmap.reg_write = regmap_syscon_write_32;
			अवरोध;
		हाल SYSCON_ENDIAN_LITTLE:
			srm->rmap.reg_read = regmap_syscon_read_le32;
			srm->rmap.reg_write = regmap_syscon_write_le32;
			अवरोध;
		हाल SYSCON_ENDIAN_BIG:
			srm->rmap.reg_read = regmap_syscon_read_be32;
			srm->rmap.reg_write = regmap_syscon_write_be32;
			अवरोध;
		शेष:
			rc = SBI_EINVAL;
			जाओ fail_free_syscon;
		}
		अवरोध;
	शेष:
		rc = SBI_EINVAL;
		जाओ fail_free_syscon;
	}

	rc = sbi_domain_मूल_add_memrange(addr, आकार, PAGE_आकार,
				(SBI_DOMAIN_MEMREGION_MMIO |
				 SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
	अगर (rc)
		जाओ fail_free_syscon;

	rc = regmap_add(&srm->rmap);
	अगर (rc)
		जाओ fail_free_syscon;

	वापस 0;

fail_free_syscon:
	sbi_free(srm);
	वापस rc;
}

अटल स्थिर काष्ठा fdt_match regmap_syscon_match[] = {
	{ .compatible = "syscon" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_regmap_syscon = {
	.match_table = regmap_syscon_match,
	.init = regmap_syscon_init,
};
