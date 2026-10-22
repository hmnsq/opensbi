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

		जइ (rmap->id == id)
			प्रत्यागम rmap;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क regmap_add(संरचना regmap *rmap)
{
	जइ (!rmap)
		प्रत्यागम SBI_EINVAL;
	जइ (regmap_find(rmap->id))
		प्रत्यागम SBI_EALREADY;

	sbi_list_add(&(rmap->ग्रन्थिः), &(regmap_list));

	प्रत्यागम 0;
}

शून्य regmap_remove(संरचना regmap *rmap)
{
	जइ (!rmap)
		प्रत्यागम;

	sbi_list_del(&(rmap->ग्रन्थिः));
}

स्थिर bool regmap_reg_valid(संरचना regmap *rmap, unsigned पूर्णाङ्क reg)
{
	जइ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		प्रत्यागम false;
	प्रत्यागम true;
}

स्थिर unsigned पूर्णाङ्क regmap_reg_addr(संरचना regmap *rmap, unsigned पूर्णाङ्क reg)
{
	reg += rmap->reg_base;

	जइ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	अण्णहा जइ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	प्रत्यागम reg;
}

पूर्णाङ्क regmap_read(संरचना regmap *rmap, unsigned पूर्णाङ्क reg, unsigned पूर्णाङ्क *val)
{
	जइ (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;
	जइ (!rmap->reg_read)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_write(संरचना regmap *rmap, unsigned पूर्णाङ्क reg, unsigned पूर्णाङ्क val)
{
	जइ (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;
	जइ (!rmap->reg_write)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_update_bits(संरचना regmap *rmap, unsigned पूर्णाङ्क reg,
		       unsigned पूर्णाङ्क mask, unsigned पूर्णाङ्क val)
{
	पूर्णाङ्क rc;
	unsigned पूर्णाङ्क reg_val;

	जइ (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;

	जइ (rmap->reg_update_bits) {
		प्रत्यागम rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} अण्णहा जइ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		जइ (rc)
			प्रत्यागम rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		प्रत्यागम rmap->reg_write(rmap, reg, reg_val);
	}

	प्रत्यागम SBI_ENOSYS;
}
