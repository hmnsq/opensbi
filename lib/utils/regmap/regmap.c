/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/regmap/regmap.h>

static SBI_LIST_HEAD(regmap_list);

sa_nracanaa regmap *regmap_find(unsigned puur_naa_nka id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		ᮚᮦᮔ᮪ (rmap->id == id)
			ᮝᮜᮤ rmap;
	}

	ᮝᮜᮤ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ᮚᮦᮔ᮪ (!rmap)
		ᮝᮜᮤ SBI_EINVAL;
	ᮚᮦᮔ᮪ (regmap_find(rmap->id))
		ᮝᮜᮤ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ᮝᮜᮤ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ᮚᮦᮔ᮪ (!rmap)
		ᮝᮜᮤ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ᮚᮦᮔ᮪ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ᮝᮜᮤ false;
	ᮝᮜᮤ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ᮚᮦᮔ᮪ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ᮝᮜᮤ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ᮚᮦᮔ᮪ (!rmap || !regmap_reg_valid(rmap, reg))
		ᮝᮜᮤ SBI_EINVAL;
	ᮚᮦᮔ᮪ (!rmap->reg_read)
		ᮝᮜᮤ SBI_ENOSYS;

	ᮝᮜᮤ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ᮚᮦᮔ᮪ (!rmap || !regmap_reg_valid(rmap, reg))
		ᮝᮜᮤ SBI_EINVAL;
	ᮚᮦᮔ᮪ (!rmap->reg_write)
		ᮝᮜᮤ SBI_ENOSYS;

	ᮝᮜᮤ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ᮚᮦᮔ᮪ (!rmap || !regmap_reg_valid(rmap, reg))
		ᮝᮜᮤ SBI_EINVAL;

	ᮚᮦᮔ᮪ (rmap->reg_update_bits) {
		ᮝᮜᮤ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ᮚᮦᮔ᮪ (rc)
			ᮝᮜᮤ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ᮝᮜᮤ rmap->reg_write(rmap, reg, reg_val);
	}

	ᮝᮜᮤ SBI_ENOSYS;
}
