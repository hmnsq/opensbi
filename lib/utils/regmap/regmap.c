/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/regmap/regmap.h>

স্থির SBI_LIST_HEAD(regmap_list);

সংরচনা regmap *regmap_find(unsigned পূর্ণাঙ্ক id)
{
	সংরচনা sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		সংরচনা regmap *rmap = to_regmap(pos);

		যদি (rmap->id == id)
			ঘূৰাই rmap;
	}

	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক regmap_add(সংরচনা regmap *rmap)
{
	যদি (!rmap)
		ঘূৰাই SBI_EINVAL;
	যদি (regmap_find(rmap->id))
		ঘূৰাই SBI_EALREADY;

	sbi_list_add(&(rmap->গ্ৰন্থি), &(regmap_list));

	ঘূৰাই 0;
}

শূন্য regmap_remove(সংরচনা regmap *rmap)
{
	যদি (!rmap)
		ঘূৰাই;

	sbi_list_del(&(rmap->গ্ৰন্থি));
}

স্থির bool regmap_reg_valid(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg)
{
	যদি ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ঘূৰাই false;
	ঘূৰাই true;
}

স্থির unsigned পূর্ণাঙ্ক regmap_reg_addr(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg)
{
	reg += rmap->reg_base;

	যদি (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	অন্যথা যদি (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ঘূৰাই reg;
}

পূর্ণাঙ্ক regmap_read(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg, unsigned পূর্ণাঙ্ক *val)
{
	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ঘূৰাই SBI_EINVAL;
	যদি (!rmap->reg_read)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

পূর্ণাঙ্ক regmap_write(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg, unsigned পূর্ণাঙ্ক val)
{
	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ঘূৰাই SBI_EINVAL;
	যদি (!rmap->reg_write)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

পূর্ণাঙ্ক regmap_update_bits(সংরচনা regmap *rmap, unsigned পূর্ণাঙ্ক reg,
		       unsigned পূর্ণাঙ্ক mask, unsigned পূর্ণাঙ্ক val)
{
	পূর্ণাঙ্ক rc;
	unsigned পূর্ণাঙ্ক reg_val;

	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ঘূৰাই SBI_EINVAL;

	যদি (rmap->reg_update_bits) {
		ঘূৰাই rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} অন্যথা যদি (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		যদি (rc)
			ঘূৰাই rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ঘূৰাই rmap->reg_write(rmap, reg, reg_val);
	}

	ঘূৰাই SBI_ENOSYS;
}
