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

		もし (rmap->id == id)
			戻る rmap;
	}

	戻る NULL;
}

整数 regmap_add(struct regmap *rmap)
{
	もし (!rmap)
		戻る SBI_EINVAL;
	もし (regmap_find(rmap->id))
		戻る SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	戻る 0;
}

void regmap_remove(struct regmap *rmap)
{
	もし (!rmap)
		戻る;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned 整数 reg)
{
	もし ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		戻る false;
	戻る true;
}

static unsigned 整数 regmap_reg_addr(struct regmap *rmap, unsigned 整数 reg)
{
	reg += rmap->reg_base;

	もし (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	否则 もし (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	戻る reg;
}

整数 regmap_read(struct regmap *rmap, unsigned 整数 reg, unsigned 整数 *val)
{
	もし (!rmap || !regmap_reg_valid(rmap, reg))
		戻る SBI_EINVAL;
	もし (!rmap->reg_read)
		戻る SBI_ENOSYS;

	戻る rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

整数 regmap_write(struct regmap *rmap, unsigned 整数 reg, unsigned 整数 val)
{
	もし (!rmap || !regmap_reg_valid(rmap, reg))
		戻る SBI_EINVAL;
	もし (!rmap->reg_write)
		戻る SBI_ENOSYS;

	戻る rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

整数 regmap_update_bits(struct regmap *rmap, unsigned 整数 reg,
		       unsigned 整数 mask, unsigned 整数 val)
{
	整数 rc;
	unsigned 整数 reg_val;

	もし (!rmap || !regmap_reg_valid(rmap, reg))
		戻る SBI_EINVAL;

	もし (rmap->reg_update_bits) {
		戻る rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} 否则 もし (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		もし (rc)
			戻る rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		戻る rmap->reg_write(rmap, reg, reg_val);
	}

	戻る SBI_ENOSYS;
}
