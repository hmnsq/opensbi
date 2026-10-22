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

struct regmap *regmap_find(unsigned целое id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		если (rmap->id == id)
			вернуть rmap;
	}

	вернуть NULL;
}

целое regmap_add(struct regmap *rmap)
{
	если (!rmap)
		вернуть SBI_EINVAL;
	если (regmap_find(rmap->id))
		вернуть SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	вернуть 0;
}

void regmap_remove(struct regmap *rmap)
{
	если (!rmap)
		вернуть;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned целое reg)
{
	если ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		вернуть false;
	вернуть true;
}

static unsigned целое regmap_reg_addr(struct regmap *rmap, unsigned целое reg)
{
	reg += rmap->reg_base;

	если (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	иначе если (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	вернуть reg;
}

целое regmap_read(struct regmap *rmap, unsigned целое reg, unsigned целое *val)
{
	если (!rmap || !regmap_reg_valid(rmap, reg))
		вернуть SBI_EINVAL;
	если (!rmap->reg_read)
		вернуть SBI_ENOSYS;

	вернуть rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

целое regmap_write(struct regmap *rmap, unsigned целое reg, unsigned целое val)
{
	если (!rmap || !regmap_reg_valid(rmap, reg))
		вернуть SBI_EINVAL;
	если (!rmap->reg_write)
		вернуть SBI_ENOSYS;

	вернуть rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

целое regmap_update_bits(struct regmap *rmap, unsigned целое reg,
		       unsigned целое mask, unsigned целое val)
{
	целое rc;
	unsigned целое reg_val;

	если (!rmap || !regmap_reg_valid(rmap, reg))
		вернуть SBI_EINVAL;

	если (rmap->reg_update_bits) {
		вернуть rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} иначе если (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		если (rc)
			вернуть rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		вернуть rmap->reg_write(rmap, reg, reg_val);
	}

	вернуть SBI_ENOSYS;
}
