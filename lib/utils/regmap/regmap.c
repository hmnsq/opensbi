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

struct regmap *regmap_find(unsigned 정수 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		만약 (rmap->id == id)
			반환 rmap;
	}

	반환 NULL;
}

정수 regmap_add(struct regmap *rmap)
{
	만약 (!rmap)
		반환 SBI_EINVAL;
	만약 (regmap_find(rmap->id))
		반환 SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	반환 0;
}

void regmap_remove(struct regmap *rmap)
{
	만약 (!rmap)
		반환;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned 정수 reg)
{
	만약 ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		반환 false;
	반환 true;
}

static unsigned 정수 regmap_reg_addr(struct regmap *rmap, unsigned 정수 reg)
{
	reg += rmap->reg_base;

	만약 (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	아니면 만약 (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	반환 reg;
}

정수 regmap_read(struct regmap *rmap, unsigned 정수 reg, unsigned 정수 *val)
{
	만약 (!rmap || !regmap_reg_valid(rmap, reg))
		반환 SBI_EINVAL;
	만약 (!rmap->reg_read)
		반환 SBI_ENOSYS;

	반환 rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

정수 regmap_write(struct regmap *rmap, unsigned 정수 reg, unsigned 정수 val)
{
	만약 (!rmap || !regmap_reg_valid(rmap, reg))
		반환 SBI_EINVAL;
	만약 (!rmap->reg_write)
		반환 SBI_ENOSYS;

	반환 rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

정수 regmap_update_bits(struct regmap *rmap, unsigned 정수 reg,
		       unsigned 정수 mask, unsigned 정수 val)
{
	정수 rc;
	unsigned 정수 reg_val;

	만약 (!rmap || !regmap_reg_valid(rmap, reg))
		반환 SBI_EINVAL;

	만약 (rmap->reg_update_bits) {
		반환 rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} 아니면 만약 (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		만약 (rc)
			반환 rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		반환 rmap->reg_write(rmap, reg, reg_val);
	}

	반환 SBI_ENOSYS;
}
