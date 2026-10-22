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

		ꦪꦼꦤ꧀ (rmap->id == id)
			ꦮꦭꦶ rmap;
	}

	ꦮꦭꦶ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ꦪꦼꦤ꧀ (!rmap)
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (regmap_find(rmap->id))
		ꦮꦭꦶ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ꦮꦭꦶ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ꦪꦼꦤ꧀ (!rmap)
		ꦮꦭꦶ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ꦪꦼꦤ꧀ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ꦮꦭꦶ false;
	ꦮꦭꦶ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ꦪꦼꦤ꧀ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ꦮꦭꦶ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ꦪꦼꦤ꧀ (!rmap || !regmap_reg_valid(rmap, reg))
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (!rmap->reg_read)
		ꦮꦭꦶ SBI_ENOSYS;

	ꦮꦭꦶ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ꦪꦼꦤ꧀ (!rmap || !regmap_reg_valid(rmap, reg))
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (!rmap->reg_write)
		ꦮꦭꦶ SBI_ENOSYS;

	ꦮꦭꦶ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ꦪꦼꦤ꧀ (!rmap || !regmap_reg_valid(rmap, reg))
		ꦮꦭꦶ SBI_EINVAL;

	ꦪꦼꦤ꧀ (rmap->reg_update_bits) {
		ꦮꦭꦶ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ꦮꦭꦶ rmap->reg_write(rmap, reg, reg_val);
	}

	ꦮꦭꦶ SBI_ENOSYS;
}
