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

		ᱡᱩᱫᱤ (rmap->id == id)
			ᱨᱩᱣᱟᱹᱲ rmap;
	}

	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ᱡᱩᱫᱤ (!rmap)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (regmap_find(rmap->id))
		ᱨᱩᱣᱟᱹᱲ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ᱨᱩᱣᱟᱹᱲ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ᱡᱩᱫᱤ (!rmap)
		ᱨᱩᱣᱟᱹᱲ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ᱡᱩᱫᱤ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ᱨᱩᱣᱟᱹᱲ false;
	ᱨᱩᱣᱟᱹᱲ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ᱡᱩᱫᱤ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ᱵᱟᱝ ᱡᱩᱫᱤ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ᱨᱩᱣᱟᱹᱲ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ᱡᱩᱫᱤ (!rmap || !regmap_reg_valid(rmap, reg))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!rmap->reg_read)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱨᱩᱣᱟᱹᱲ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ᱡᱩᱫᱤ (!rmap || !regmap_reg_valid(rmap, reg))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!rmap->reg_write)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱨᱩᱣᱟᱹᱲ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ᱡᱩᱫᱤ (!rmap || !regmap_reg_valid(rmap, reg))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;

	ᱡᱩᱫᱤ (rmap->reg_update_bits) {
		ᱨᱩᱣᱟᱹᱲ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ᱡᱩᱫᱤ (rc)
			ᱨᱩᱣᱟᱹᱲ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ᱨᱩᱣᱟᱹᱲ rmap->reg_write(rmap, reg, reg_val);
	}

	ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;
}
