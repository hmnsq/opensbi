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

sa_nracanaa regmap *regmap_find(unsigned ចំនួនគត់ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		ប្រសិនបើ (rmap->id == id)
			ត្រឡប់ rmap;
	}

	ត្រឡប់ NULL;
}

ចំនួនគត់ regmap_add(sa_nracanaa regmap *rmap)
{
	ប្រសិនបើ (!rmap)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (regmap_find(rmap->id))
		ត្រឡប់ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ត្រឡប់ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ប្រសិនបើ (!rmap)
		ត្រឡប់;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned ចំនួនគត់ reg)
{
	ប្រសិនបើ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ត្រឡប់ false;
	ត្រឡប់ true;
}

static unsigned ចំនួនគត់ regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned ចំនួនគត់ reg)
{
	reg += rmap->reg_base;

	ប្រសិនបើ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	បើមិនដូច្នេះ ប្រសិនបើ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ត្រឡប់ reg;
}

ចំនួនគត់ regmap_read(sa_nracanaa regmap *rmap, unsigned ចំនួនគត់ reg, unsigned ចំនួនគត់ *val)
{
	ប្រសិនបើ (!rmap || !regmap_reg_valid(rmap, reg))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!rmap->reg_read)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ចំនួនគត់ regmap_write(sa_nracanaa regmap *rmap, unsigned ចំនួនគត់ reg, unsigned ចំនួនគត់ val)
{
	ប្រសិនបើ (!rmap || !regmap_reg_valid(rmap, reg))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!rmap->reg_write)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ចំនួនគត់ regmap_update_bits(sa_nracanaa regmap *rmap, unsigned ចំនួនគត់ reg,
		       unsigned ចំនួនគត់ mask, unsigned ចំនួនគត់ val)
{
	ចំនួនគត់ rc;
	unsigned ចំនួនគត់ reg_val;

	ប្រសិនបើ (!rmap || !regmap_reg_valid(rmap, reg))
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (rmap->reg_update_bits) {
		ត្រឡប់ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} បើមិនដូច្នេះ ប្រសិនបើ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ត្រឡប់ rmap->reg_write(rmap, reg, reg_val);
	}

	ត្រឡប់ SBI_ENOSYS;
}
