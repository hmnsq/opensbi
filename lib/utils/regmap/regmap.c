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

بنية regmap *regmap_پیدا_کړه(بینښه صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		بنية regmap *rmap = منزل_regmap(pos);

		إذا (rmap->id == id)
			أرجع rmap;
	}

	أرجع عدم;
}

صحيح regmap_جمع_کړه(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع SBI_EINVAL;
	إذا (regmap_پیدا_کړه(rmap->id))
		أرجع SBI_EALREADY;

	sbi_list_جمع_کړه(&(rmap->غوټه), &(regmap_list));

	أرجع 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع;

	sbi_list_del(&(rmap->غوټه));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, بینښه صحيح reg)
{
	إذا ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		أرجع false;
	أرجع true;
}

ساكن بینښه صحيح regmap_reg_addr(بنية regmap *rmap, بینښه صحيح reg)
{
	reg += rmap->reg_بنسټ;

	إذا (rmap->reg_ښويدنه > 0)
		reg >>= rmap->reg_ښويدنه;
	وإلا إذا (rmap->reg_ښويدنه < 0)
		reg <<= -(rmap->reg_ښويدنه);

	أرجع reg;
}

صحيح regmap_read(بنية regmap *rmap, بینښه صحيح reg, بینښه صحيح *val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_read)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, بینښه صحيح reg, بینښه صحيح val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_write)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_بټونه(بنية regmap *rmap, بینښه صحيح reg,
		       بینښه صحيح ماسک, بینښه صحيح val)
{
	صحيح rc;
	بینښه صحيح reg_val;

	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;

	إذا (rmap->reg_update_بټونه) {
		أرجع rmap->reg_update_بټونه(rmap, regmap_reg_addr(rmap, reg),
					     ماسک, val);
	} وإلا إذا (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		إذا (rc)
			أرجع rc;

		reg_val &= ~ماسک;
		reg_val |= val & ماسک;
		أرجع rmap->reg_write(rmap, reg, reg_val);
	}

	أرجع SBI_ENOSYS;
}
