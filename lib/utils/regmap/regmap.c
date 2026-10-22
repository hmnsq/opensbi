/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/regmap/regmap.h>

ساكن SBI_LIST_سر(regmap_list);

بنية regmap *regmap_لبھو(بنانشانی صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		بنية regmap *rmap = منزل_regmap(pos);

		إذا (rmap->id == id)
			أرجع rmap;
	}

	أرجع عدم;
}

صحيح regmap_جوڑو(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع SBI_EINVAL;
	إذا (regmap_لبھو(rmap->id))
		أرجع SBI_EALREADY;

	sbi_list_جوڑو(&(rmap->گرہ), &(regmap_list));

	أرجع 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع;

	sbi_list_del(&(rmap->گرہ));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, بنانشانی صحيح reg)
{
	إذا ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		أرجع false;
	أرجع true;
}

ساكن بنانشانی صحيح regmap_reg_addr(بنية regmap *rmap, بنانشانی صحيح reg)
{
	reg += rmap->reg_ادھار;

	إذا (rmap->reg_کھسکاؤ > 0)
		reg >>= rmap->reg_کھسکاؤ;
	وإلا إذا (rmap->reg_کھسکاؤ < 0)
		reg <<= -(rmap->reg_کھسکاؤ);

	أرجع reg;
}

صحيح regmap_read(بنية regmap *rmap, بنانشانی صحيح reg, بنانشانی صحيح *val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_read)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, بنانشانی صحيح reg, بنانشانی صحيح val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_write)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_بٹ(بنية regmap *rmap, بنانشانی صحيح reg,
		       بنانشانی صحيح نقاب, بنانشانی صحيح val)
{
	صحيح rc;
	بنانشانی صحيح reg_val;

	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;

	إذا (rmap->reg_update_بٹ) {
		أرجع rmap->reg_update_بٹ(rmap, regmap_reg_addr(rmap, reg),
					     نقاب, val);
	} وإلا إذا (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		إذا (rc)
			أرجع rc;

		reg_val &= ~نقاب;
		reg_val |= val & نقاب;
		أرجع rmap->reg_write(rmap, reg, reg_val);
	}

	أرجع SBI_ENOSYS;
}
