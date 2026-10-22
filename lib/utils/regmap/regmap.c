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

ساكن SBI_LIST_HEAD(regmap_list);

بنية regmap *regmap_find(unsigned صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		بنية regmap *rmap = to_regmap(pos);

		ܐܢ (rmap->id == id)
			ܦܢܐ rmap;
	}

	ܦܢܐ عدم;
}

صحيح regmap_add(بنية regmap *rmap)
{
	ܐܢ (!rmap)
		ܦܢܐ SBI_EINVAL;
	ܐܢ (regmap_find(rmap->id))
		ܦܢܐ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ܦܢܐ 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	ܐܢ (!rmap)
		ܦܢܐ;

	sbi_list_del(&(rmap->node));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, unsigned صحيح reg)
{
	ܐܢ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ܦܢܐ false;
	ܦܢܐ true;
}

ساكن unsigned صحيح regmap_reg_addr(بنية regmap *rmap, unsigned صحيح reg)
{
	reg += rmap->reg_base;

	ܐܢ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ܐܚܪܢܐ ܐܢ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ܦܢܐ reg;
}

صحيح regmap_read(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح *val)
{
	ܐܢ (!rmap || !regmap_reg_valid(rmap, reg))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!rmap->reg_read)
		ܦܢܐ SBI_ENOSYS;

	ܦܢܐ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح val)
{
	ܐܢ (!rmap || !regmap_reg_valid(rmap, reg))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!rmap->reg_write)
		ܦܢܐ SBI_ENOSYS;

	ܦܢܐ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_bits(بنية regmap *rmap, unsigned صحيح reg,
		       unsigned صحيح mask, unsigned صحيح val)
{
	صحيح rc;
	unsigned صحيح reg_val;

	ܐܢ (!rmap || !regmap_reg_valid(rmap, reg))
		ܦܢܐ SBI_EINVAL;

	ܐܢ (rmap->reg_update_bits) {
		ܦܢܐ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ܐܚܪܢܐ ܐܢ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ܐܢ (rc)
			ܦܢܐ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ܦܢܐ rmap->reg_write(rmap, reg, reg_val);
	}

	ܦܢܐ SBI_ENOSYS;
}
