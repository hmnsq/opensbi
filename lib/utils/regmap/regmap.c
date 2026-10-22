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

sa_nracanaa regmap *regmap_find(unsigned puur_naa_nka id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		ပေႃး (rmap->id == id)
			ပွၵ်ႈ rmap;
	}

	ပွၵ်ႈ NULL;
}

puur_naa_nka regmap_add(sa_nracanaa regmap *rmap)
{
	ပေႃး (!rmap)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (regmap_find(rmap->id))
		ပွၵ်ႈ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ပွၵ်ႈ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ပေႃး (!rmap)
		ပွၵ်ႈ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	ပေႃး ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ပွၵ်ႈ false;
	ပွၵ်ႈ true;
}

static unsigned puur_naa_nka regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg)
{
	reg += rmap->reg_base;

	ပေႃး (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ပေႃးဢမ်ႇ ပေႃး (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ပွၵ်ႈ reg;
}

puur_naa_nka regmap_read(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka *val)
{
	ပေႃး (!rmap || !regmap_reg_valid(rmap, reg))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!rmap->reg_read)
		ပွၵ်ႈ SBI_ENOSYS;

	ပွၵ်ႈ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_write(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg, unsigned puur_naa_nka val)
{
	ပေႃး (!rmap || !regmap_reg_valid(rmap, reg))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!rmap->reg_write)
		ပွၵ်ႈ SBI_ENOSYS;

	ပွၵ်ႈ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

puur_naa_nka regmap_update_bits(sa_nracanaa regmap *rmap, unsigned puur_naa_nka reg,
		       unsigned puur_naa_nka mask, unsigned puur_naa_nka val)
{
	puur_naa_nka rc;
	unsigned puur_naa_nka reg_val;

	ပေႃး (!rmap || !regmap_reg_valid(rmap, reg))
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (rmap->reg_update_bits) {
		ပွၵ်ႈ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ပေႃးဢမ်ႇ ပေႃး (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ပေႃး (rc)
			ပွၵ်ႈ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ပွၵ်ႈ rmap->reg_write(rmap, reg, reg_val);
	}

	ပွၵ်ႈ SBI_ENOSYS;
}
