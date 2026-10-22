/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/regmap/regmap.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(regmap_list);

ਬਣਤਰ regmap *regmap_ਲੱਭੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ id)
{
	ਬਣਤਰ sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ਬਣਤਰ regmap *rmap = ਮੰਜ਼ਿਲ_regmap(pos);

		ਜੇ (rmap->id == id)
			ਵਾਪਸ rmap;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ regmap_ਜੋੜੋ(ਬਣਤਰ regmap *rmap)
{
	ਜੇ (!rmap)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (regmap_ਲੱਭੋ(rmap->id))
		ਵਾਪਸ SBI_EALREADY;

	sbi_list_ਜੋੜੋ(&(rmap->ਗੰਢ), &(regmap_list));

	ਵਾਪਸ 0;
}

ਖਾਲੀ regmap_remove(ਬਣਤਰ regmap *rmap)
{
	ਜੇ (!rmap)
		ਵਾਪਸ;

	sbi_list_del(&(rmap->ਗੰਢ));
}

ਸਥਿਰ bool regmap_reg_valid(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg)
{
	ਜੇ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		ਵਾਪਸ false;
	ਵਾਪਸ true;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ regmap_reg_addr(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg)
{
	reg += rmap->reg_ਅਧਾਰ;

	ਜੇ (rmap->reg_ਖਿਸਕਾਓ > 0)
		reg >>= rmap->reg_ਖਿਸਕਾਓ;
	ਨਹੀਂਤਾਂ ਜੇ (rmap->reg_ਖਿਸਕਾਓ < 0)
		reg <<= -(rmap->reg_ਖਿਸਕਾਓ);

	ਵਾਪਸ reg;
}

ਪੂਰਨ regmap_read(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *val)
{
	ਜੇ (!rmap || !regmap_reg_valid(rmap, reg))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!rmap->reg_read)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

ਪੂਰਨ regmap_write(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਜੇ (!rmap || !regmap_reg_valid(rmap, reg))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!rmap->reg_write)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

ਪੂਰਨ regmap_update_ਬਿੱਟ(ਬਣਤਰ regmap *rmap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ reg_val;

	ਜੇ (!rmap || !regmap_reg_valid(rmap, reg))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (rmap->reg_update_ਬਿੱਟ) {
		ਵਾਪਸ rmap->reg_update_ਬਿੱਟ(rmap, regmap_reg_addr(rmap, reg),
					     ਨਕਾਬ, val);
	} ਨਹੀਂਤਾਂ ਜੇ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		reg_val &= ~ਨਕਾਬ;
		reg_val |= val & ਨਕਾਬ;
		ਵਾਪਸ rmap->reg_write(rmap, reg, reg_val);
	}

	ਵਾਪਸ SBI_ENOSYS;
}
