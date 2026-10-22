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

संरचना regmap *regmap_find(अचिन्हित पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		संरचना regmap *rmap = to_regmap(pos);

		यदि (rmap->id == id)
			फर्काऊ rmap;
	}

	फर्काऊ रिक्त;
}

पूर्णाङ्क regmap_add(संरचना regmap *rmap)
{
	यदि (!rmap)
		फर्काऊ SBI_EINVAL;
	यदि (regmap_find(rmap->id))
		फर्काऊ SBI_EALREADY;

	sbi_list_add(&(rmap->गाँठो), &(regmap_list));

	फर्काऊ 0;
}

शून्य regmap_remove(संरचना regmap *rmap)
{
	यदि (!rmap)
		फर्काऊ;

	sbi_list_del(&(rmap->गाँठो));
}

स्थिर bool regmap_reg_valid(संरचना regmap *rmap, अचिन्हित पूर्णाङ्क reg)
{
	यदि ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		फर्काऊ false;
	फर्काऊ true;
}

स्थिर अचिन्हित पूर्णाङ्क regmap_reg_addr(संरचना regmap *rmap, अचिन्हित पूर्णाङ्क reg)
{
	reg += rmap->reg_base;

	यदि (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	अन्यथा यदि (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	फर्काऊ reg;
}

पूर्णाङ्क regmap_read(संरचना regmap *rmap, अचिन्हित पूर्णाङ्क reg, अचिन्हित पूर्णाङ्क *val)
{
	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		फर्काऊ SBI_EINVAL;
	यदि (!rmap->reg_read)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_write(संरचना regmap *rmap, अचिन्हित पूर्णाङ्क reg, अचिन्हित पूर्णाङ्क val)
{
	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		फर्काऊ SBI_EINVAL;
	यदि (!rmap->reg_write)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_update_bits(संरचना regmap *rmap, अचिन्हित पूर्णाङ्क reg,
		       अचिन्हित पूर्णाङ्क mask, अचिन्हित पूर्णाङ्क val)
{
	पूर्णाङ्क rc;
	अचिन्हित पूर्णाङ्क reg_val;

	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		फर्काऊ SBI_EINVAL;

	यदि (rmap->reg_update_bits) {
		फर्काऊ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} अन्यथा यदि (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		यदि (rc)
			फर्काऊ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		फर्काऊ rmap->reg_write(rmap, reg, reg_val);
	}

	फर्काऊ SBI_ENOSYS;
}
