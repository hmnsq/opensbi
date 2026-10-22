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

ಸ್ಥಿರ SBI_LIST_ತಲೆ(regmap_list);

ರಚನೆ regmap *regmap_ಕಂಡುಹಿಡಿ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ id)
{
	ರಚನೆ sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ರಚನೆ regmap *rmap = ಗಮ್ಯ_regmap(pos);

		ಆದರೆ (rmap->id == id)
			ಹಿಂತಿರುಗಿ rmap;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ regmap_ಕೂಡಿಸು(ರಚನೆ regmap *rmap)
{
	ಆದರೆ (!rmap)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (regmap_ಕಂಡುಹಿಡಿ(rmap->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	sbi_list_ಕೂಡಿಸು(&(rmap->ಗಂಟು), &(regmap_list));

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ regmap_remove(ರಚನೆ regmap *rmap)
{
	ಆದರೆ (!rmap)
		ಹಿಂತಿರುಗಿ;

	sbi_list_del(&(rmap->ಗಂಟು));
}

ಸ್ಥಿರ bool regmap_reg_valid(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg)
{
	ಆದರೆ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ಹಿಂತಿರುಗಿ false;
	ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ regmap_reg_addr(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg)
{
	reg += rmap->reg_ತಳ;

	ಆದರೆ (rmap->reg_ಸರಿಸು > 0)
		reg >>= rmap->reg_ಸರಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rmap->reg_ಸರಿಸು < 0)
		reg <<= -(rmap->reg_ಸರಿಸು);

	ಹಿಂತಿರುಗಿ reg;
}

ಪೂರ್ಣಾಂಕ regmap_read(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *val)
{
	ಆದರೆ (!rmap || !regmap_reg_valid(rmap, reg))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!rmap->reg_read)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ಪೂರ್ಣಾಂಕ regmap_write(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ಆದರೆ (!rmap || !regmap_reg_valid(rmap, reg))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!rmap->reg_write)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ಪೂರ್ಣಾಂಕ regmap_update_ಬಿಟ್ಟುಗಳು(ರಚನೆ regmap *rmap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg,
		       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ reg_val;

	ಆದರೆ (!rmap || !regmap_reg_valid(rmap, reg))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (rmap->reg_update_ಬಿಟ್ಟುಗಳು) {
		ಹಿಂತಿರುಗಿ rmap->reg_update_ಬಿಟ್ಟುಗಳು(rmap, regmap_reg_addr(rmap, reg),
					     ಮುಖವಾಡ, val);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		reg_val &= ~ಮುಖವಾಡ;
		reg_val |= val & ಮುಖವಾಡ;
		ಹಿಂತಿರುಗಿ rmap->reg_write(rmap, reg, reg_val);
	}

	ಹಿಂತಿರುಗಿ SBI_ENOSYS;
}
