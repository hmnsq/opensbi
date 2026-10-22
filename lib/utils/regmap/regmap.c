/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/regmap/regmap.h>

நிலையான SBI_LIST_HEAD(regmap_list);

கட்டமைப்பு regmap *regmap_find(unsigned முழு id)
{
	கட்டமைப்பு sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		கட்டமைப்பு regmap *rmap = to_regmap(pos);

		எனில் (rmap->id == id)
			திரும்பு rmap;
	}

	திரும்பு வெற்று;
}

முழு regmap_add(கட்டமைப்பு regmap *rmap)
{
	எனில் (!rmap)
		திரும்பு SBI_EINVAL;
	எனில் (regmap_find(rmap->id))
		திரும்பு SBI_EALREADY;

	sbi_list_add(&(rmap->முனை), &(regmap_list));

	திரும்பு 0;
}

வெறுமை regmap_remove(கட்டமைப்பு regmap *rmap)
{
	எனில் (!rmap)
		திரும்பு;

	sbi_list_del(&(rmap->முனை));
}

நிலையான bool regmap_reg_valid(கட்டமைப்பு regmap *rmap, unsigned முழு reg)
{
	எனில் ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		திரும்பு false;
	திரும்பு true;
}

நிலையான unsigned முழு regmap_reg_addr(கட்டமைப்பு regmap *rmap, unsigned முழு reg)
{
	reg += rmap->reg_base;

	எனில் (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	இல்லையெனில் எனில் (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	திரும்பு reg;
}

முழு regmap_read(கட்டமைப்பு regmap *rmap, unsigned முழு reg, unsigned முழு *val)
{
	எனில் (!rmap || !regmap_reg_valid(rmap, reg))
		திரும்பு SBI_EINVAL;
	எனில் (!rmap->reg_read)
		திரும்பு SBI_ENOSYS;

	திரும்பு rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

முழு regmap_write(கட்டமைப்பு regmap *rmap, unsigned முழு reg, unsigned முழு val)
{
	எனில் (!rmap || !regmap_reg_valid(rmap, reg))
		திரும்பு SBI_EINVAL;
	எனில் (!rmap->reg_write)
		திரும்பு SBI_ENOSYS;

	திரும்பு rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

முழு regmap_update_bits(கட்டமைப்பு regmap *rmap, unsigned முழு reg,
		       unsigned முழு mask, unsigned முழு val)
{
	முழு rc;
	unsigned முழு reg_val;

	எனில் (!rmap || !regmap_reg_valid(rmap, reg))
		திரும்பு SBI_EINVAL;

	எனில் (rmap->reg_update_bits) {
		திரும்பு rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} இல்லையெனில் எனில் (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		எனில் (rc)
			திரும்பு rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		திரும்பு rmap->reg_write(rmap, reg, reg_val);
	}

	திரும்பு SBI_ENOSYS;
}
