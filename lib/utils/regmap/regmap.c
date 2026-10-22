/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/regmap/regmap.h>

স্থির SBI_LIST_শীর্ষ(regmap_list);

কাঠামো regmap *regmap_find(unsigned পূর্ণসংখ্যা id)
{
	কাঠামো sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		কাঠামো regmap *rmap = to_regmap(pos);

		যদি (rmap->id == id)
			ফেরত rmap;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা regmap_add(কাঠামো regmap *rmap)
{
	যদি (!rmap)
		ফেরত SBI_EINVAL;
	যদি (regmap_find(rmap->id))
		ফেরত SBI_EALREADY;

	sbi_list_add(&(rmap->গ্রন্থি), &(regmap_list));

	ফেরত 0;
}

শূন্য regmap_remove(কাঠামো regmap *rmap)
{
	যদি (!rmap)
		ফেরত;

	sbi_list_del(&(rmap->গ্রন্থি));
}

স্থির bool regmap_reg_valid(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg)
{
	যদি ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ফেরত false;
	ফেরত true;
}

স্থির unsigned পূর্ণসংখ্যা regmap_reg_addr(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg)
{
	reg += rmap->reg_base;

	যদি (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	অন্যথা যদি (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ফেরত reg;
}

পূর্ণসংখ্যা regmap_read(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg, unsigned পূর্ণসংখ্যা *val)
{
	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ফেরত SBI_EINVAL;
	যদি (!rmap->reg_read)
		ফেরত SBI_ENOSYS;

	ফেরত rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

পূর্ণসংখ্যা regmap_write(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg, unsigned পূর্ণসংখ্যা val)
{
	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ফেরত SBI_EINVAL;
	যদি (!rmap->reg_write)
		ফেরত SBI_ENOSYS;

	ফেরত rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

পূর্ণসংখ্যা regmap_update_bits(কাঠামো regmap *rmap, unsigned পূর্ণসংখ্যা reg,
		       unsigned পূর্ণসংখ্যা mask, unsigned পূর্ণসংখ্যা val)
{
	পূর্ণসংখ্যা rc;
	unsigned পূর্ণসংখ্যা reg_val;

	যদি (!rmap || !regmap_reg_valid(rmap, reg))
		ফেরত SBI_EINVAL;

	যদি (rmap->reg_update_bits) {
		ফেরত rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} অন্যথা যদি (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		যদি (rc)
			ফেরত rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ফেরত rmap->reg_write(rmap, reg, reg_val);
	}

	ফেরত SBI_ENOSYS;
}
