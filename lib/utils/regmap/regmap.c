/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/regmap/regmap.h>

સ્થિર SBI_LIST_HEAD(regmap_list);

સંરચના regmap *regmap_find(unsigned પૂર્ણાંક id)
{
	સંરચના sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		સંરચના regmap *rmap = to_regmap(pos);

		જો (rmap->id == id)
			પાછા rmap;
	}

	પાછા ખાલી;
}

પૂર્ણાંક regmap_add(સંરચના regmap *rmap)
{
	જો (!rmap)
		પાછા SBI_EINVAL;
	જો (regmap_find(rmap->id))
		પાછા SBI_EALREADY;

	sbi_list_add(&(rmap->ગાંઠ), &(regmap_list));

	પાછા 0;
}

શૂન્ય regmap_remove(સંરચના regmap *rmap)
{
	જો (!rmap)
		પાછા;

	sbi_list_del(&(rmap->ગાંઠ));
}

સ્થિર bool regmap_reg_valid(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg)
{
	જો ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		પાછા false;
	પાછા true;
}

સ્થિર unsigned પૂર્ણાંક regmap_reg_addr(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg)
{
	reg += rmap->reg_base;

	જો (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	નહીંતર જો (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	પાછા reg;
}

પૂર્ણાંક regmap_read(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg, unsigned પૂર્ણાંક *val)
{
	જો (!rmap || !regmap_reg_valid(rmap, reg))
		પાછા SBI_EINVAL;
	જો (!rmap->reg_read)
		પાછા SBI_ENOSYS;

	પાછા rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

પૂર્ણાંક regmap_write(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg, unsigned પૂર્ણાંક val)
{
	જો (!rmap || !regmap_reg_valid(rmap, reg))
		પાછા SBI_EINVAL;
	જો (!rmap->reg_write)
		પાછા SBI_ENOSYS;

	પાછા rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

પૂર્ણાંક regmap_update_bits(સંરચના regmap *rmap, unsigned પૂર્ણાંક reg,
		       unsigned પૂર્ણાંક mask, unsigned પૂર્ણાંક val)
{
	પૂર્ણાંક rc;
	unsigned પૂર્ણાંક reg_val;

	જો (!rmap || !regmap_reg_valid(rmap, reg))
		પાછા SBI_EINVAL;

	જો (rmap->reg_update_bits) {
		પાછા rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} નહીંતર જો (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		જો (rc)
			પાછા rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		પાછા rmap->reg_write(rmap, reg, reg_val);
	}

	પાછા SBI_ENOSYS;
}
