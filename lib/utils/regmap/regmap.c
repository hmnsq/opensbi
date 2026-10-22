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

		ᨀᨘ (rmap->id == id)
			ᨀᨘᨒ rmap;
	}

	ᨀᨘᨒ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ᨀᨘ (!rmap)
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (regmap_find(rmap->id))
		ᨀᨘᨒ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ᨀᨘᨒ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ᨀᨘ (!rmap)
		ᨀᨘᨒ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ᨀᨘ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ᨀᨘᨒ false;
	ᨀᨘᨒ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ᨀᨘ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ᨀᨘᨁ ᨀᨘ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ᨀᨘᨒ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ᨀᨘ (!rmap || !regmap_reg_valid(rmap, reg))
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (!rmap->reg_read)
		ᨀᨘᨒ SBI_ENOSYS;

	ᨀᨘᨒ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ᨀᨘ (!rmap || !regmap_reg_valid(rmap, reg))
		ᨀᨘᨒ SBI_EINVAL;
	ᨀᨘ (!rmap->reg_write)
		ᨀᨘᨒ SBI_ENOSYS;

	ᨀᨘᨒ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ᨀᨘ (!rmap || !regmap_reg_valid(rmap, reg))
		ᨀᨘᨒ SBI_EINVAL;

	ᨀᨘ (rmap->reg_update_bits) {
		ᨀᨘᨒ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ᨀᨘᨁ ᨀᨘ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ᨀᨘ (rc)
			ᨀᨘᨒ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ᨀᨘᨒ rmap->reg_write(rmap, reg, reg_val);
	}

	ᨀᨘᨒ SBI_ENOSYS;
}
