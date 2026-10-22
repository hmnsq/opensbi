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

sa_nracanaa regmap *regmap_find(unsigned ཧྲིལ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		གལ་སྲིད (rmap->id == id)
			ལོག rmap;
	}

	ལོག NULL;
}

ཧྲིལ regmap_add(sa_nracanaa regmap *rmap)
{
	གལ་སྲིད (!rmap)
		ལོག SBI_EINVAL;
	གལ་སྲིད (regmap_find(rmap->id))
		ལོག SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ལོག 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	གལ་སྲིད (!rmap)
		ལོག;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned ཧྲིལ reg)
{
	གལ་སྲིད ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ལོག false;
	ལོག true;
}

static unsigned ཧྲིལ regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned ཧྲིལ reg)
{
	reg += rmap->reg_base;

	གལ་སྲིད (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	དེ་མིན གལ་སྲིད (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ལོག reg;
}

ཧྲིལ regmap_read(sa_nracanaa regmap *rmap, unsigned ཧྲིལ reg, unsigned ཧྲིལ *val)
{
	གལ་སྲིད (!rmap || !regmap_reg_valid(rmap, reg))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!rmap->reg_read)
		ལོག SBI_ENOSYS;

	ལོག rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ཧྲིལ regmap_write(sa_nracanaa regmap *rmap, unsigned ཧྲིལ reg, unsigned ཧྲིལ val)
{
	གལ་སྲིད (!rmap || !regmap_reg_valid(rmap, reg))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!rmap->reg_write)
		ལོག SBI_ENOSYS;

	ལོག rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ཧྲིལ regmap_update_bits(sa_nracanaa regmap *rmap, unsigned ཧྲིལ reg,
		       unsigned ཧྲིལ mask, unsigned ཧྲིལ val)
{
	ཧྲིལ rc;
	unsigned ཧྲིལ reg_val;

	གལ་སྲིད (!rmap || !regmap_reg_valid(rmap, reg))
		ལོག SBI_EINVAL;

	གལ་སྲིད (rmap->reg_update_bits) {
		ལོག rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} དེ་མིན གལ་སྲིད (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		གལ་སྲིད (rc)
			ལོག rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ལོག rmap->reg_write(rmap, reg, reg_val);
	}

	ལོག SBI_ENOSYS;
}
