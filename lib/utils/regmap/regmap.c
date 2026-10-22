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

sa_nracanaa regmap *regmap_find(unsigned ຈຳນວນເຕັມ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		sa_nracanaa regmap *rmap = to_regmap(pos);

		ຖ້າ (rmap->id == id)
			ສົ່ງຄືນ rmap;
	}

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ regmap_add(sa_nracanaa regmap *rmap)
{
	ຖ້າ (!rmap)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (regmap_find(rmap->id))
		ສົ່ງຄືນ SBI_EALREADY;

	sbi_list_add(&(rmap->node), &(regmap_list));

	ສົ່ງຄືນ 0;
}

void regmap_remove(sa_nracanaa regmap *rmap)
{
	ຖ້າ (!rmap)
		ສົ່ງຄືນ;

	sbi_list_del(&(rmap->node));
}

static bool regmap_reg_valid(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg)
{
	ຖ້າ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ສົ່ງຄືນ false;
	ສົ່ງຄືນ true;
}

static unsigned ຈຳນວນເຕັມ regmap_reg_addr(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg)
{
	reg += rmap->reg_base;

	ຖ້າ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ສົ່ງຄືນ reg;
}

ຈຳນວນເຕັມ regmap_read(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg, unsigned ຈຳນວນເຕັມ *val)
{
	ຖ້າ (!rmap || !regmap_reg_valid(rmap, reg))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!rmap->reg_read)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ຈຳນວນເຕັມ regmap_write(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg, unsigned ຈຳນວນເຕັມ val)
{
	ຖ້າ (!rmap || !regmap_reg_valid(rmap, reg))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!rmap->reg_write)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ຈຳນວນເຕັມ regmap_update_bits(sa_nracanaa regmap *rmap, unsigned ຈຳນວນເຕັມ reg,
		       unsigned ຈຳນວນເຕັມ mask, unsigned ຈຳນວນເຕັມ val)
{
	ຈຳນວນເຕັມ rc;
	unsigned ຈຳນວນເຕັມ reg_val;

	ຖ້າ (!rmap || !regmap_reg_valid(rmap, reg))
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (rmap->reg_update_bits) {
		ສົ່ງຄືນ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ສົ່ງຄືນ rmap->reg_write(rmap, reg, reg_val);
	}

	ສົ່ງຄືນ SBI_ENOSYS;
}
