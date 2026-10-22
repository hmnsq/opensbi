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

بنية regmap *regmap_پیدا_کن(بیعلامت صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		بنية regmap *rmap = مقصد_regmap(pos);

		إذا (rmap->id == id)
			أرجع rmap;
	}

	أرجع عدم;
}

صحيح regmap_جمع_کن(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع SBI_EINVAL;
	إذا (regmap_پیدا_کن(rmap->id))
		أرجع SBI_EALREADY;

	sbi_list_جمع_کن(&(rmap->گره), &(regmap_list));

	أرجع 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع;

	sbi_list_del(&(rmap->گره));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, بیعلامت صحيح reg)
{
	إذا ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		أرجع false;
	أرجع true;
}

ساكن بیعلامت صحيح regmap_reg_addr(بنية regmap *rmap, بیعلامت صحيح reg)
{
	reg += rmap->reg_پایه;

	إذا (rmap->reg_لغزش > 0)
		reg >>= rmap->reg_لغزش;
	وإلا إذا (rmap->reg_لغزش < 0)
		reg <<= -(rmap->reg_لغزش);

	أرجع reg;
}

صحيح regmap_read(بنية regmap *rmap, بیعلامت صحيح reg, بیعلامت صحيح *val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_read)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, بیعلامت صحيح reg, بیعلامت صحيح val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_write)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_بیتها(بنية regmap *rmap, بیعلامت صحيح reg,
		       بیعلامت صحيح ماسک, بیعلامت صحيح val)
{
	صحيح rc;
	بیعلامت صحيح reg_val;

	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;

	إذا (rmap->reg_update_بیتها) {
		أرجع rmap->reg_update_بیتها(rmap, regmap_reg_addr(rmap, reg),
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
