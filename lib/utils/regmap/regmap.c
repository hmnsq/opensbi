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

		אם (rmap->id == id)
			החזר rmap;
	}

	החזר عدم;
}

صحيح regmap_add(بنية regmap *rmap)
{
	אם (!rmap)
		החזר SBI_EINVAL;
	אם (regmap_find(rmap->id))
		החזר SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	החזר 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	אם (!rmap)
		החזר;

	sbi_list_del(&(rmap->node));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, unsigned صحيح reg)
{
	אם ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		החזר false;
	החזר true;
}

ساكن unsigned صحيح regmap_reg_addr(بنية regmap *rmap, unsigned صحيح reg)
{
	reg += rmap->reg_base;

	אם (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	אחרת אם (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	החזר reg;
}

صحيح regmap_read(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح *val)
{
	אם (!rmap || !regmap_reg_valid(rmap, reg))
		החזר SBI_EINVAL;
	אם (!rmap->reg_read)
		החזר SBI_ENOSYS;

	החזר rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح val)
{
	אם (!rmap || !regmap_reg_valid(rmap, reg))
		החזר SBI_EINVAL;
	אם (!rmap->reg_write)
		החזר SBI_ENOSYS;

	החזר rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_bits(بنية regmap *rmap, unsigned صحيح reg,
		       unsigned صحيح mask, unsigned صحيح val)
{
	صحيح rc;
	unsigned صحيح reg_val;

	אם (!rmap || !regmap_reg_valid(rmap, reg))
		החזר SBI_EINVAL;

	אם (rmap->reg_update_bits) {
		החזר rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} אחרת אם (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		אם (rc)
			החזר rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		החזר rmap->reg_write(rmap, reg, reg_val);
	}

	החזר SBI_ENOSYS;
}
