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

sa_nracanaa regmap *regmap_find(unsigned ကိန်းပြည့် id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		အကယ်၍ (rmap->id == id)
			ပြန်ပေး rmap;
	}

	ပြန်ပေး NULL;
}

ကိန်းပြည့် regmap_add(sa_nracanaa regmap *rmap)
{
	အကယ်၍ (!rmap)
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (regmap_find(rmap->id))
		ပြန်ပေး SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ပြန်ပေး 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	အကယ်၍ (!rmap)
		ပြန်ပေး;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned ကိန်းပြည့် reg)
{
	အကယ်၍ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ပြန်ပေး false;
	ပြန်ပေး true;
}

static unsigned ကိန်းပြည့် regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned ကိန်းပြည့် reg)
{
	reg += rmap->reg_base;

	အကယ်၍ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	မဟုတ်ပါက အကယ်၍ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ပြန်ပေး reg;
}

ကိန်းပြည့် regmap_read(sa_nracanaa regmap *rmap, unsigned ကိန်းပြည့် reg, unsigned ကိန်းပြည့် *val)
{
	အကယ်၍ (!rmap || !regmap_reg_valid(rmap, reg))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!rmap->reg_read)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ကိန်းပြည့် regmap_write(sa_nracanaa regmap *rmap, unsigned ကိန်းပြည့် reg, unsigned ကိန်းပြည့် val)
{
	အကယ်၍ (!rmap || !regmap_reg_valid(rmap, reg))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!rmap->reg_write)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ကိန်းပြည့် regmap_update_bits(sa_nracanaa regmap *rmap, unsigned ကိန်းပြည့် reg,
		       unsigned ကိန်းပြည့် mask, unsigned ကိန်းပြည့် val)
{
	ကိန်းပြည့် rc;
	unsigned ကိန်းပြည့် reg_val;

	အကယ်၍ (!rmap || !regmap_reg_valid(rmap, reg))
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (rmap->reg_update_bits) {
		ပြန်ပေး rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} မဟုတ်ပါက အကယ်၍ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		အကယ်၍ (rc)
			ပြန်ပေး rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ပြန်ပေး rmap->reg_write(rmap, reg, reg_val);
	}

	ပြန်ပေး SBI_ENOSYS;
}
