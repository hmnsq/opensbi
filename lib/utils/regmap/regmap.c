/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/regmap/regmap.h>

स्थिर SBI_LIST_HEAD(regmap_list);

संरचना regmap *regmap_find(unsigned पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		संरचना regmap *rmap = to_regmap(pos);

		सचे (rmap->id == id)
			पटिनिवत्त rmap;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क regmap_add(संरचना regmap *rmap)
{
	सचे (!rmap)
		पटिनिवत्त SBI_EINVAL;
	सचे (regmap_find(rmap->id))
		पटिनिवत्त SBI_EALREADY;

	sbi_list_add(&(rmap->ग्रन्थिः), &(regmap_list));

	पटिनिवत्त 0;
}

शून्य regmap_remove(संरचना regmap *rmap)
{
	सचे (!rmap)
		पटिनिवत्त;

	sbi_list_del(&(rmap->ग्रन्थिः));
}

स्थिर bool regmap_reg_valid(संरचना regmap *rmap, unsigned पूर्णाङ्क reg)
{
	सचे ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		पटिनिवत्त false;
	पटिनिवत्त true;
}

स्थिर unsigned पूर्णाङ्क regmap_reg_addr(संरचना regmap *rmap, unsigned पूर्णाङ्क reg)
{
	reg += rmap->reg_base;

	सचे (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	अञ्ञथा सचे (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	पटिनिवत्त reg;
}

पूर्णाङ्क regmap_read(संरचना regmap *rmap, unsigned पूर्णाङ्क reg, unsigned पूर्णाङ्क *val)
{
	सचे (!rmap || !regmap_reg_valid(rmap, reg))
		पटिनिवत्त SBI_EINVAL;
	सचे (!rmap->reg_read)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_write(संरचना regmap *rmap, unsigned पूर्णाङ्क reg, unsigned पूर्णाङ्क val)
{
	सचे (!rmap || !regmap_reg_valid(rmap, reg))
		पटिनिवत्त SBI_EINVAL;
	सचे (!rmap->reg_write)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_update_bits(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
		       unsigned पूर्णाङ्क mask, unsigned पूर्णाङ्क val)
{
	पूर्णाङ्क rc;
	unsigned पूर्णाङ्क reg_val;

	सचे (!rmap || !regmap_reg_valid(rmap, reg))
		पटिनिवत्त SBI_EINVAL;

	सचे (rmap->reg_update_bits) {
		पटिनिवत्त rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} अञ्ञथा सचे (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		सचे (rc)
			पटिनिवत्त rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		पटिनिवत्त rmap->reg_write(rmap, reg, reg_val);
	}

	पटिनिवत्त SBI_ENOSYS;
}
