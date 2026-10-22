/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/regmap/regmap.h>

స్థిర SBI_LIST_తల(regmap_list);

నిర్మాణం regmap *regmap_కనుగొను(గుర్తులేని పూర్ణాంకం id)
{
	నిర్మాణం sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		నిర్మాణం regmap *rmap = గమ్యం_regmap(pos);

		ఒకవేళ (rmap->id == id)
			తిరిగి rmap;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం regmap_కూడు(నిర్మాణం regmap *rmap)
{
	ఒకవేళ (!rmap)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (regmap_కనుగొను(rmap->id))
		తిరిగి SBI_EALREADY;

	sbi_list_కూడు(&(rmap->కణుపు), &(regmap_list));

	తిరిగి 0;
}

శూన్య regmap_remove(నిర్మాణం regmap *rmap)
{
	ఒకవేళ (!rmap)
		తిరిగి;

	sbi_list_del(&(rmap->కణుపు));
}

స్థిర bool regmap_reg_valid(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg)
{
	ఒకవేళ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		తిరిగి false;
	తిరిగి true;
}

స్థిర గుర్తులేని పూర్ణాంకం regmap_reg_addr(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg)
{
	reg += rmap->reg_పునాది;

	ఒకవేళ (rmap->reg_జరుపు > 0)
		reg >>= rmap->reg_జరుపు;
	లేకపోతే ఒకవేళ (rmap->reg_జరుపు < 0)
		reg <<= -(rmap->reg_జరుపు);

	తిరిగి reg;
}

పూర్ణాంకం regmap_read(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg, గుర్తులేని పూర్ణాంకం *val)
{
	ఒకవేళ (!rmap || !regmap_reg_valid(rmap, reg))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!rmap->reg_read)
		తిరిగి SBI_ENOSYS;

	తిరిగి rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

పూర్ణాంకం regmap_write(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg, గుర్తులేని పూర్ణాంకం val)
{
	ఒకవేళ (!rmap || !regmap_reg_valid(rmap, reg))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!rmap->reg_write)
		తిరిగి SBI_ENOSYS;

	తిరిగి rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

పూర్ణాంకం regmap_update_బిట్లు(నిర్మాణం regmap *rmap, గుర్తులేని పూర్ణాంకం reg,
		       గుర్తులేని పూర్ణాంకం ముసుగు, గుర్తులేని పూర్ణాంకం val)
{
	పూర్ణాంకం rc;
	గుర్తులేని పూర్ణాంకం reg_val;

	ఒకవేళ (!rmap || !regmap_reg_valid(rmap, reg))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (rmap->reg_update_బిట్లు) {
		తిరిగి rmap->reg_update_బిట్లు(rmap, regmap_reg_addr(rmap, reg),
					     ముసుగు, val);
	} లేకపోతే ఒకవేళ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ఒకవేళ (rc)
			తిరిగి rc;

		reg_val &= ~ముసుగు;
		reg_val |= val & ముసుగు;
		తిరిగి rmap->reg_write(rmap, reg, reg_val);
	}

	తిరిగి SBI_ENOSYS;
}
