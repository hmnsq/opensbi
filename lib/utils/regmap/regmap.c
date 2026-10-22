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

		إذا (rmap->id == id)
			أرجع rmap;
	}

	أرجع عدم;
}

صحيح regmap_add(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع SBI_EINVAL;
	إذا (regmap_find(rmap->id))
		أرجع SBI_EALREADY;

	sbi_list_add(&(rmap->كلي), &(regmap_list));

	أرجع 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	إذا (!rmap)
		أرجع;

	sbi_list_del(&(rmap->كلي));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, unsigned صحيح reg)
{
	إذا ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		أرجع false;
	أرجع true;
}

ساكن unsigned صحيح regmap_reg_addr(بنية regmap *rmap, unsigned صحيح reg)
{
	reg += rmap->reg_base;

	إذا (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	وإلا إذا (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	أرجع reg;
}

صحيح regmap_read(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح *val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_read)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح val)
{
	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	إذا (!rmap->reg_write)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_bits(بنية regmap *rmap, unsigned صحيح reg,
		       unsigned صحيح mask, unsigned صحيح val)
{
	صحيح rc;
	unsigned صحيح reg_val;

	إذا (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;

	إذا (rmap->reg_update_bits) {
		أرجع rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} وإلا إذا (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		إذا (rc)
			أرجع rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		أرجع rmap->reg_write(rmap, reg, reg_val);
	}

	أرجع SBI_ENOSYS;
}
