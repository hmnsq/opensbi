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

sa_nracanaa regmap *regmap_find(unsigned පූර්ණ_සංඛ්‍යා id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		නම් (rmap->id == id)
			ආපසු rmap;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා regmap_add(sa_nracanaa regmap *rmap)
{
	නම් (!rmap)
		ආපසු SBI_EINVAL;
	නම් (regmap_find(rmap->id))
		ආපසු SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ආපසු 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	නම් (!rmap)
		ආපසු;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned පූර්ණ_සංඛ්‍යා reg)
{
	නම් ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ආපසු false;
	ආපසු true;
}

static unsigned පූර්ණ_සංඛ්‍යා regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned පූර්ණ_සංඛ්‍යා reg)
{
	reg += rmap->reg_base;

	නම් (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	නැතහොත් නම් (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ආපසු reg;
}

පූර්ණ_සංඛ්‍යා regmap_read(sa_nracanaa regmap *rmap, unsigned පූර්ණ_සංඛ්‍යා reg, unsigned පූර්ණ_සංඛ්‍යා *val)
{
	නම් (!rmap || !regmap_reg_valid(rmap, reg))
		ආපසු SBI_EINVAL;
	නම් (!rmap->reg_read)
		ආපසු SBI_ENOSYS;

	ආපසු rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

පූර්ණ_සංඛ්‍යා regmap_write(sa_nracanaa regmap *rmap, unsigned පූර්ණ_සංඛ්‍යා reg, unsigned පූර්ණ_සංඛ්‍යා val)
{
	නම් (!rmap || !regmap_reg_valid(rmap, reg))
		ආපසු SBI_EINVAL;
	නම් (!rmap->reg_write)
		ආපසු SBI_ENOSYS;

	ආපසු rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

පූර්ණ_සංඛ්‍යා regmap_update_bits(sa_nracanaa regmap *rmap, unsigned පූර්ණ_සංඛ්‍යා reg,
		       unsigned පූර්ණ_සංඛ්‍යා mask, unsigned පූර්ණ_සංඛ්‍යා val)
{
	පූර්ණ_සංඛ්‍යා rc;
	unsigned පූර්ණ_සංඛ්‍යා reg_val;

	නම් (!rmap || !regmap_reg_valid(rmap, reg))
		ආපසු SBI_EINVAL;

	නම් (rmap->reg_update_bits) {
		ආපසු rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} නැතහොත් නම් (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		නම් (rc)
			ආපසු rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ආපසු rmap->reg_write(rmap, reg, reg_val);
	}

	ආපසු SBI_ENOSYS;
}
