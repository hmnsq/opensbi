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

struct regmap *regmap_find(unsigned цео id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		ако (rmap->id == id)
			врати rmap;
	}

	врати NULL;
}

цео regmap_add(struct regmap *rmap)
{
	ако (!rmap)
		врати SBI_EINVAL;
	ако (regmap_find(rmap->id))
		врати SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	врати 0;
}

void regmap_remove(struct regmap *rmap)
{
	ако (!rmap)
		врати;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned цео reg)
{
	ако ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		врати false;
	врати true;
}

static unsigned цео regmap_reg_addr(struct regmap *rmap, unsigned цео reg)
{
	reg += rmap->reg_base;

	ако (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	инако ако (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	врати reg;
}

цео regmap_read(struct regmap *rmap, unsigned цео reg, unsigned цео *val)
{
	ако (!rmap || !regmap_reg_valid(rmap, reg))
		врати SBI_EINVAL;
	ако (!rmap->reg_read)
		врати SBI_ENOSYS;

	врати rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

цео regmap_write(struct regmap *rmap, unsigned цео reg, unsigned цео val)
{
	ако (!rmap || !regmap_reg_valid(rmap, reg))
		врати SBI_EINVAL;
	ако (!rmap->reg_write)
		врати SBI_ENOSYS;

	врати rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

цео regmap_update_bits(struct regmap *rmap, unsigned цео reg,
		       unsigned цео mask, unsigned цео val)
{
	цео rc;
	unsigned цео reg_val;

	ако (!rmap || !regmap_reg_valid(rmap, reg))
		врати SBI_EINVAL;

	ако (rmap->reg_update_bits) {
		врати rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} инако ако (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ако (rc)
			врати rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		врати rmap->reg_write(rmap, reg, reg_val);
	}

	врати SBI_ENOSYS;
}
