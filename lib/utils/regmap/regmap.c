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

संरचना regmap *regmap_find(अचिन्हित पूर्णांक id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		संरचना regmap *rmap = to_regmap(pos);

		जर (rmap->id == id)
			परत rmap;
	}

	परत रिक्त;
}

पूर्णांक regmap_add(संरचना regmap *rmap)
{
	जर (!rmap)
		परत SBI_EINVAL;
	जर (regmap_find(rmap->id))
		परत SBI_EALREADY;

	sbi_list_add(&(rmap->गाठ), &(regmap_list));

	परत 0;
}

शून्य regmap_remove(संरचना regmap *rmap)
{
	जर (!rmap)
		परत;

	sbi_list_del(&(rmap->गाठ));
}

स्थिर bool regmap_reg_valid(संरचना regmap *rmap, अचिन्हित पूर्णांक reg)
{
	जर ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		परत false;
	परत true;
}

स्थिर अचिन्हित पूर्णांक regmap_reg_addr(संरचना regmap *rmap, अचिन्हित पूर्णांक reg)
{
	reg += rmap->reg_base;

	जर (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	अन्यथा जर (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	परत reg;
}

पूर्णांक regmap_read(संरचना regmap *rmap, अचिन्हित पूर्णांक reg, अचिन्हित पूर्णांक *val)
{
	जर (!rmap || !regmap_reg_valid(rmap, reg))
		परत SBI_EINVAL;
	जर (!rmap->reg_read)
		परत SBI_ENOSYS;

	परत rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णांक regmap_write(संरचना regmap *rmap, अचिन्हित पूर्णांक reg, अचिन्हित पूर्णांक val)
{
	जर (!rmap || !regmap_reg_valid(rmap, reg))
		परत SBI_EINVAL;
	जर (!rmap->reg_write)
		परत SBI_ENOSYS;

	परत rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णांक regmap_update_bits(संरचना regmap *rmap, अचिन्हित पूर्णांक reg,
		       अचिन्हित पूर्णांक mask, अचिन्हित पूर्णांक val)
{
	पूर्णांक rc;
	अचिन्हित पूर्णांक reg_val;

	जर (!rmap || !regmap_reg_valid(rmap, reg))
		परत SBI_EINVAL;

	जर (rmap->reg_update_bits) {
		परत rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} अन्यथा जर (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		जर (rc)
			परत rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		परत rmap->reg_write(rmap, reg, reg_val);
	}

	परत SBI_ENOSYS;
}
