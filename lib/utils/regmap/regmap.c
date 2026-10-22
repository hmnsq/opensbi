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

struct regmap *regmap_find(unsigned цяло id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		ако (rmap->id == id)
			върни rmap;
	}

	върни NULL;
}

цяло regmap_add(struct regmap *rmap)
{
	ако (!rmap)
		върни SBI_EINVAL;
	ако (regmap_find(rmap->id))
		върни SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	върни 0;
}

void regmap_remove(struct regmap *rmap)
{
	ако (!rmap)
		върни;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned цяло reg)
{
	ако ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		върни false;
	върни true;
}

static unsigned цяло regmap_reg_addr(struct regmap *rmap, unsigned цяло reg)
{
	reg += rmap->reg_base;

	ако (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	иначе ако (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	върни reg;
}

цяло regmap_read(struct regmap *rmap, unsigned цяло reg, unsigned цяло *val)
{
	ако (!rmap || !regmap_reg_valid(rmap, reg))
		върни SBI_EINVAL;
	ако (!rmap->reg_read)
		върни SBI_ENOSYS;

	върни rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

цяло regmap_write(struct regmap *rmap, unsigned цяло reg, unsigned цяло val)
{
	ако (!rmap || !regmap_reg_valid(rmap, reg))
		върни SBI_EINVAL;
	ако (!rmap->reg_write)
		върни SBI_ENOSYS;

	върни rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

цяло regmap_update_bits(struct regmap *rmap, unsigned цяло reg,
		       unsigned цяло mask, unsigned цяло val)
{
	цяло rc;
	unsigned цяло reg_val;

	ако (!rmap || !regmap_reg_valid(rmap, reg))
		върни SBI_EINVAL;

	ако (rmap->reg_update_bits) {
		върни rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} иначе ако (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ако (rc)
			върни rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		върни rmap->reg_write(rmap, reg, reg_val);
	}

	върни SBI_ENOSYS;
}
