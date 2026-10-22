/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/regmap/regmap.h>

ସ୍ଥିର SBI_LIST_HEAD(regmap_list);

ସଂରଚନା regmap *regmap_find(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
	ସଂରଚନା sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ସଂରଚନା regmap *rmap = to_regmap(pos);

		ଯଦି (rmap->id == id)
			ଫେରନ୍ତୁ rmap;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_add(ସଂରଚନା regmap *rmap)
{
	ଯଦି (!rmap)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (regmap_find(rmap->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	sbi_list_add(&(rmap->ଗଣ୍ଠି), &(regmap_list));

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ regmap_remove(ସଂରଚନା regmap *rmap)
{
	ଯଦି (!rmap)
		ଫେରନ୍ତୁ;

	sbi_list_del(&(rmap->ଗଣ୍ଠି));
}

ସ୍ଥିର bool regmap_reg_valid(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg)
{
	ଯଦି ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ଫେରନ୍ତୁ false;
	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_reg_addr(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg)
{
	reg += rmap->reg_base;

	ଯଦି (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	ଅନ୍ୟଥା ଯଦି (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	ଫେରନ୍ତୁ reg;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_read(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *val)
{
	ଯଦି (!rmap || !regmap_reg_valid(rmap, reg))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!rmap->reg_read)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_write(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ଯଦି (!rmap || !regmap_reg_valid(rmap, reg))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!rmap->reg_write)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା regmap_update_bits(ସଂରଚନା regmap *rmap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg,
		       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg_val;

	ଯଦି (!rmap || !regmap_reg_valid(rmap, reg))
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (rmap->reg_update_bits) {
		ଫେରନ୍ତୁ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} ଅନ୍ୟଥା ଯଦି (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		ଫେରନ୍ତୁ rmap->reg_write(rmap, reg, reg_val);
	}

	ଫେରନ୍ତୁ SBI_ENOSYS;
}
