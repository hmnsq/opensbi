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

struct regmap *regmap_find(unsigned số_nguyên id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		nếu (rmap->id == id)
			trả_về rmap;
	}

	trả_về NULL;
}

số_nguyên regmap_add(struct regmap *rmap)
{
	nếu (!rmap)
		trả_về SBI_EINVAL;
	nếu (regmap_find(rmap->id))
		trả_về SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	trả_về 0;
}

void regmap_remove(struct regmap *rmap)
{
	nếu (!rmap)
		trả_về;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned số_nguyên reg)
{
	nếu ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		trả_về false;
	trả_về true;
}

static unsigned số_nguyên regmap_reg_addr(struct regmap *rmap, unsigned số_nguyên reg)
{
	reg += rmap->reg_base;

	nếu (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	không_thì nếu (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	trả_về reg;
}

số_nguyên regmap_read(struct regmap *rmap, unsigned số_nguyên reg, unsigned số_nguyên *val)
{
	nếu (!rmap || !regmap_reg_valid(rmap, reg))
		trả_về SBI_EINVAL;
	nếu (!rmap->reg_read)
		trả_về SBI_ENOSYS;

	trả_về rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

số_nguyên regmap_write(struct regmap *rmap, unsigned số_nguyên reg, unsigned số_nguyên val)
{
	nếu (!rmap || !regmap_reg_valid(rmap, reg))
		trả_về SBI_EINVAL;
	nếu (!rmap->reg_write)
		trả_về SBI_ENOSYS;

	trả_về rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

số_nguyên regmap_update_bits(struct regmap *rmap, unsigned số_nguyên reg,
		       unsigned số_nguyên mask, unsigned số_nguyên val)
{
	số_nguyên rc;
	unsigned số_nguyên reg_val;

	nếu (!rmap || !regmap_reg_valid(rmap, reg))
		trả_về SBI_EINVAL;

	nếu (rmap->reg_update_bits) {
		trả_về rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} không_thì nếu (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		nếu (rc)
			trả_về rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		trả_về rmap->reg_write(rmap, reg, reg_val);
	}

	trả_về SBI_ENOSYS;
}
