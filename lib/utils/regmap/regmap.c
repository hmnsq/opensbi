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

		𐡄𐡍 (rmap->id == id)
			أرجع rmap;
	}

	أرجع عدم;
}

صحيح regmap_add(بنية regmap *rmap)
{
	𐡄𐡍 (!rmap)
		أرجع SBI_EINVAL;
	𐡄𐡍 (regmap_find(rmap->id))
		أرجع SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	أرجع 0;
}

فراغ regmap_remove(بنية regmap *rmap)
{
	𐡄𐡍 (!rmap)
		أرجع;

	sbi_list_del(&(rmap->node));
}

ساكن bool regmap_reg_valid(بنية regmap *rmap, unsigned صحيح reg)
{
	𐡄𐡍 ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		أرجع false;
	أرجع true;
}

ساكن unsigned صحيح regmap_reg_addr(بنية regmap *rmap, unsigned صحيح reg)
{
	reg += rmap->reg_base;

	𐡄𐡍 (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	وإلا 𐡄𐡍 (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	أرجع reg;
}

صحيح regmap_read(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح *val)
{
	𐡄𐡍 (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!rmap->reg_read)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_write(بنية regmap *rmap, unsigned صحيح reg, unsigned صحيح val)
{
	𐡄𐡍 (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!rmap->reg_write)
		أرجع SBI_ENOSYS;

	أرجع rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحيح regmap_update_bits(بنية regmap *rmap, unsigned صحيح reg,
		       unsigned صحيح mask, unsigned صحيح val)
{
	صحيح rc;
	unsigned صحيح reg_val;

	𐡄𐡍 (!rmap || !regmap_reg_valid(rmap, reg))
		أرجع SBI_EINVAL;

	𐡄𐡍 (rmap->reg_update_bits) {
		أرجع rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} وإلا 𐡄𐡍 (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		𐡄𐡍 (rc)
			أرجع rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		أرجع rmap->reg_write(rmap, reg, reg_val);
	}

	أرجع SBI_ENOSYS;
}
