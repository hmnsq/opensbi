/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/regmap/regmap.h>

ایستا SBI_LIST_HEAD(regmap_list);

ساختار regmap *regmap_find(unsigned صحیح id)
{
	ساختار sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ساختار regmap *rmap = to_regmap(pos);

		اگر (rmap->id == id)
			بازگشت rmap;
	}

	بازگشت پوچ;
}

صحیح regmap_add(ساختار regmap *rmap)
{
	اگر (!rmap)
		بازگشت SBI_EINVAL;
	اگر (regmap_find(rmap->id))
		بازگشت SBI_EALREADY;

	sbi_list_add(&(rmap->گره), &(regmap_list));

	بازگشت 0;
}

تهی regmap_remove(ساختار regmap *rmap)
{
	اگر (!rmap)
		بازگشت;

	sbi_list_del(&(rmap->گره));
}

ایستا bool regmap_reg_valid(ساختار regmap *rmap, unsigned صحیح reg)
{
	اگر ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		بازگشت false;
	بازگشت true;
}

ایستا unsigned صحیح regmap_reg_addr(ساختار regmap *rmap, unsigned صحیح reg)
{
	reg += rmap->reg_base;

	اگر (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	وگرنه اگر (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	بازگشت reg;
}

صحیح regmap_read(ساختار regmap *rmap, unsigned صحیح reg, unsigned صحیح *val)
{
	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		بازگشت SBI_EINVAL;
	اگر (!rmap->reg_read)
		بازگشت SBI_ENOSYS;

	بازگشت rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحیح regmap_write(ساختار regmap *rmap, unsigned صحیح reg, unsigned صحیح val)
{
	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		بازگشت SBI_EINVAL;
	اگر (!rmap->reg_write)
		بازگشت SBI_ENOSYS;

	بازگشت rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحیح regmap_update_bits(ساختار regmap *rmap, unsigned صحیح reg,
		       unsigned صحیح mask, unsigned صحیح val)
{
	صحیح rc;
	unsigned صحیح reg_val;

	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		بازگشت SBI_EINVAL;

	اگر (rmap->reg_update_bits) {
		بازگشت rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} وگرنه اگر (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		اگر (rc)
			بازگشت rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		بازگشت rmap->reg_write(rmap, reg, reg_val);
	}

	بازگشت SBI_ENOSYS;
}
