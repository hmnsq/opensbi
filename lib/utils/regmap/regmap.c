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

struct regmap *regmap_find(unsigned 整数 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		如果 (rmap->id == id)
			返回 rmap;
	}

	返回 NULL;
}

整数 regmap_add(struct regmap *rmap)
{
	如果 (!rmap)
		返回 SBI_EINVAL;
	如果 (regmap_find(rmap->id))
		返回 SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	返回 0;
}

void regmap_remove(struct regmap *rmap)
{
	如果 (!rmap)
		返回;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned 整数 reg)
{
	如果 ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		返回 false;
	返回 true;
}

static unsigned 整数 regmap_reg_addr(struct regmap *rmap, unsigned 整数 reg)
{
	reg += rmap->reg_base;

	如果 (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	否则 如果 (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	返回 reg;
}

整数 regmap_read(struct regmap *rmap, unsigned 整数 reg, unsigned 整数 *val)
{
	如果 (!rmap || !regmap_reg_valid(rmap, reg))
		返回 SBI_EINVAL;
	如果 (!rmap->reg_read)
		返回 SBI_ENOSYS;

	返回 rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

整数 regmap_write(struct regmap *rmap, unsigned 整数 reg, unsigned 整数 val)
{
	如果 (!rmap || !regmap_reg_valid(rmap, reg))
		返回 SBI_EINVAL;
	如果 (!rmap->reg_write)
		返回 SBI_ENOSYS;

	返回 rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

整数 regmap_update_bits(struct regmap *rmap, unsigned 整数 reg,
		       unsigned 整数 mask, unsigned 整数 val)
{
	整数 rc;
	unsigned 整数 reg_val;

	如果 (!rmap || !regmap_reg_valid(rmap, reg))
		返回 SBI_EINVAL;

	如果 (rmap->reg_update_bits) {
		返回 rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} 否则 如果 (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		如果 (rc)
			返回 rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		返回 rmap->reg_write(rmap, reg, reg_val);
	}

	返回 SBI_ENOSYS;
}
