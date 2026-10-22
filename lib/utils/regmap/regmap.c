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

sa_nracanaa regmap *regmap_find(unsigned จำนวนเต็ม id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		ถ้า (rmap->id == id)
			คืน rmap;
	}

	คืน NULL;
}

จำนวนเต็ม regmap_add(sa_nracanaa regmap *rmap)
{
	ถ้า (!rmap)
		คืน SBI_EINVAL;
	ถ้า (regmap_find(rmap->id))
		คืน SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	คืน 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ถ้า (!rmap)
		คืน;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned จำนวนเต็ม reg)
{
	ถ้า ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		คืน false;
	คืน true;
}

static unsigned จำนวนเต็ม regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned จำนวนเต็ม reg)
{
	reg += rmap->reg_base;

	ถ้า (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	มิฉะนั้น ถ้า (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	คืน reg;
}

จำนวนเต็ม regmap_read(sa_nracanaa regmap *rmap, unsigned จำนวนเต็ม reg, unsigned จำนวนเต็ม *val)
{
	ถ้า (!rmap || !regmap_reg_valid(rmap, reg))
		คืน SBI_EINVAL;
	ถ้า (!rmap->reg_read)
		คืน SBI_ENOSYS;

	คืน rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

จำนวนเต็ม regmap_write(sa_nracanaa regmap *rmap, unsigned จำนวนเต็ม reg, unsigned จำนวนเต็ม val)
{
	ถ้า (!rmap || !regmap_reg_valid(rmap, reg))
		คืน SBI_EINVAL;
	ถ้า (!rmap->reg_write)
		คืน SBI_ENOSYS;

	คืน rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

จำนวนเต็ม regmap_update_bits(sa_nracanaa regmap *rmap, unsigned จำนวนเต็ม reg,
		       unsigned จำนวนเต็ม mask, unsigned จำนวนเต็ม val)
{
	จำนวนเต็ม rc;
	unsigned จำนวนเต็ม reg_val;

	ถ้า (!rmap || !regmap_reg_valid(rmap, reg))
		คืน SBI_EINVAL;

	ถ้า (rmap->reg_update_bits) {
		คืน rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} มิฉะนั้น ถ้า (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ถ้า (rc)
			คืน rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		คืน rmap->reg_write(rmap, reg, reg_val);
	}

	คืน SBI_ENOSYS;
}
