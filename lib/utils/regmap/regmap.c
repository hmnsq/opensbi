/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/regmap/regmap.h>

സ്ഥിതം SBI_LIST_HEAD(regmap_list);

ഘടന regmap *regmap_find(unsigned പൂർണ്ണസംഖ്യ id)
{
	ഘടന sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ഘടന regmap *rmap = to_regmap(pos);

		എങ്കിൽ (rmap->id == id)
			തിരികെ rmap;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ regmap_add(ഘടന regmap *rmap)
{
	എങ്കിൽ (!rmap)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (regmap_find(rmap->id))
		തിരികെ SBI_EALREADY;

	sbi_list_add(&(rmap->കണ്ണി), &(regmap_list));

	തിരികെ 0;
}

ശൂന്യം regmap_remove(ഘടന regmap *rmap)
{
	എങ്കിൽ (!rmap)
		തിരികെ;

	sbi_list_del(&(rmap->കണ്ണി));
}

സ്ഥിതം bool regmap_reg_valid(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg)
{
	എങ്കിൽ ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		തിരികെ false;
	തിരികെ true;
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ regmap_reg_addr(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg)
{
	reg += rmap->reg_base;

	എങ്കിൽ (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	അല്ലെങ്കിൽ എങ്കിൽ (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	തിരികെ reg;
}

പൂർണ്ണസംഖ്യ regmap_read(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg, unsigned പൂർണ്ണസംഖ്യ *val)
{
	എങ്കിൽ (!rmap || !regmap_reg_valid(rmap, reg))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!rmap->reg_read)
		തിരികെ SBI_ENOSYS;

	തിരികെ rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

പൂർണ്ണസംഖ്യ regmap_write(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg, unsigned പൂർണ്ണസംഖ്യ val)
{
	എങ്കിൽ (!rmap || !regmap_reg_valid(rmap, reg))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!rmap->reg_write)
		തിരികെ SBI_ENOSYS;

	തിരികെ rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

പൂർണ്ണസംഖ്യ regmap_update_bits(ഘടന regmap *rmap, unsigned പൂർണ്ണസംഖ്യ reg,
		       unsigned പൂർണ്ണസംഖ്യ mask, unsigned പൂർണ്ണസംഖ്യ val)
{
	പൂർണ്ണസംഖ്യ rc;
	unsigned പൂർണ്ണസംഖ്യ reg_val;

	എങ്കിൽ (!rmap || !regmap_reg_valid(rmap, reg))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (rmap->reg_update_bits) {
		തിരികെ rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} അല്ലെങ്കിൽ എങ്കിൽ (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		എങ്കിൽ (rc)
			തിരികെ rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		തിരികെ rmap->reg_write(rmap, reg, reg_val);
	}

	തിരികെ SBI_ENOSYS;
}
