/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/regmap/regmap.h>

ಸ್ಥಿರ SBI_LIST_HEAD(regmap_list);

ಸಂರಚನಾ regmap *regmap_find(unsigned ಪೂರ್ಣಾಙ್ಕ id)
{
	ಸಂರಚನಾ sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ಸಂರಚನಾ regmap *rmap = to_regmap(pos);

		ಯದಿ (rmap->id == id)
			ಪ್ರತ್ಯಾಗಮ rmap;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ regmap_add(ಸಂರಚನಾ regmap *rmap)
{
	ಯದಿ (!rmap)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (regmap_find(rmap->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	sbi_list_add(&(rmap->ಗಂಟು), &(regmap_list));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ regmap_remove(ಸಂರಚನಾ regmap *rmap)
{
	ಯದಿ (!rmap)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_del(&(rmap->ಗಂಟು));
}

ಸ್ಥಿರ bool regmap_reg_valid(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg)
{
	ಯದಿ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ಪ್ರತ್ಯಾಗಮ false;
	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ regmap_reg_addr(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg)
{
	reg += rmap->reg_base;

	ಯದಿ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ಅನ್ಯಥಾ ಯದಿ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ಪ್ರತ್ಯಾಗಮ reg;
}

ಪೂರ್ಣಾಙ್ಕ regmap_read(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg, unsigned ಪೂರ್ಣಾಙ್ಕ *val)
{
	ಯದಿ (!rmap || !regmap_reg_valid(rmap, reg))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!rmap->reg_read)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ಪೂರ್ಣಾಙ್ಕ regmap_write(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg, unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಯದಿ (!rmap || !regmap_reg_valid(rmap, reg))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!rmap->reg_write)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ಪೂರ್ಣಾಙ್ಕ regmap_update_bits(ಸಂರಚನಾ regmap *rmap, unsigned ಪೂರ್ಣಾಙ್ಕ reg,
		       unsigned ಪೂರ್ಣಾಙ್ಕ mask, unsigned ಪೂರ್ಣಾಙ್ಕ val)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	unsigned ಪೂರ್ಣಾಙ್ಕ reg_val;

	ಯದಿ (!rmap || !regmap_reg_valid(rmap, reg))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (rmap->reg_update_bits) {
		ಪ್ರತ್ಯಾಗಮ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ಅನ್ಯಥಾ ಯದಿ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ಪ್ರತ್ಯಾಗಮ rmap->reg_write(rmap, reg, reg_val);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;
}
