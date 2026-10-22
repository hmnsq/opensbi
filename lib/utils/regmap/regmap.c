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

		ᤀᤡ᤺ (rmap->id == id)
			ᤀᤡᤴ rmap;
	}

	ᤀᤡᤴ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ᤀᤡ᤺ (!rmap)
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (regmap_find(rmap->id))
		ᤀᤡᤴ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ᤀᤡᤴ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ᤀᤡ᤺ (!rmap)
		ᤀᤡᤴ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ᤀᤡ᤺ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ᤀᤡᤴ false;
	ᤀᤡᤴ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ᤀᤡ᤺ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ᤀᤡᤴ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ᤀᤡ᤺ (!rmap || !regmap_reg_valid(rmap, reg))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!rmap->reg_read)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡᤴ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ᤀᤡ᤺ (!rmap || !regmap_reg_valid(rmap, reg))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!rmap->reg_write)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡᤴ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ᤀᤡ᤺ (!rmap || !regmap_reg_valid(rmap, reg))
		ᤀᤡᤴ SBI_EINVAL;

	ᤀᤡ᤺ (rmap->reg_update_bits) {
		ᤀᤡᤴ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ᤀᤡ᤺ (rc)
			ᤀᤡᤴ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ᤀᤡᤴ rmap->reg_write(rmap, reg, reg_val);
	}

	ᤀᤡᤴ SBI_ENOSYS;
}
