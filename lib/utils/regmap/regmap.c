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

struct regmap *regmap_find(unsigned ціле id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		struct regmap *rmap = to_regmap(pos);

		якщо (rmap->id == id)
			повернути rmap;
	}

	повернути NULL;
}

ціле regmap_add(struct regmap *rmap)
{
	якщо (!rmap)
		повернути SBI_EINVAL;
	якщо (regmap_find(rmap->id))
		повернути SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	повернути 0;
}

void regmap_remove(struct regmap *rmap)
{
	якщо (!rmap)
		повернути;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(struct regmap *rmap, unsigned ціле reg)
{
	якщо ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		повернути false;
	повернути true;
}

static unsigned ціле regmap_reg_addr(struct regmap *rmap, unsigned ціле reg)
{
	reg += rmap->reg_base;

	якщо (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	інакше якщо (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	повернути reg;
}

ціле regmap_read(struct regmap *rmap, unsigned ціле reg, unsigned ціле *val)
{
	якщо (!rmap || !regmap_reg_valid(rmap, reg))
		повернути SBI_EINVAL;
	якщо (!rmap->reg_read)
		повернути SBI_ENOSYS;

	повернути rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ціле regmap_write(struct regmap *rmap, unsigned ціле reg, unsigned ціле val)
{
	якщо (!rmap || !regmap_reg_valid(rmap, reg))
		повернути SBI_EINVAL;
	якщо (!rmap->reg_write)
		повернути SBI_ENOSYS;

	повернути rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ціле regmap_update_bits(struct regmap *rmap, unsigned ціле reg,
		       unsigned ціле mask, unsigned ціле val)
{
	ціле rc;
	unsigned ціле reg_val;

	якщо (!rmap || !regmap_reg_valid(rmap, reg))
		повернути SBI_EINVAL;

	якщо (rmap->reg_update_bits) {
		повернути rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} інакше якщо (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		якщо (rc)
			повернути rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		повернути rmap->reg_write(rmap, reg, reg_val);
	}

	повернути SBI_ENOSYS;
}
