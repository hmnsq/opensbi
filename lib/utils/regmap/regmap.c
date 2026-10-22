/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/regmap/regmap.h>

ساکن SBI_LIST_سر(regmap_list);

ساخت regmap *regmap_ڈھونڈو(غیرعلامتی صحیح id)
{
	ساخت sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		ساخت regmap *rmap = مقصد_regmap(pos);

		اگر (rmap->id == id)
			واپس rmap;
	}

	واپس تہی;
}

صحیح regmap_جوڑو(ساخت regmap *rmap)
{
	اگر (!rmap)
		واپس SBI_EINVAL;
	اگر (regmap_ڈھونڈو(rmap->id))
		واپس SBI_EALREADY;

	sbi_list_جوڑو(&(rmap->گرہ), &(regmap_list));

	واپس 0;
}

خالی regmap_remove(ساخت regmap *rmap)
{
	اگر (!rmap)
		واپس;

	sbi_list_del(&(rmap->گرہ));
}

ساکن bool regmap_reg_valid(ساخت regmap *rmap, غیرعلامتی صحیح reg)
{
	اگر ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		واپس false;
	واپس true;
}

ساکن غیرعلامتی صحیح regmap_reg_addr(ساخت regmap *rmap, غیرعلامتی صحیح reg)
{
	reg += rmap->reg_بنیاد;

	اگر (rmap->reg_سرکاؤ > 0)
		reg >>= rmap->reg_سرکاؤ;
	ورنہ اگر (rmap->reg_سرکاؤ < 0)
		reg <<= -(rmap->reg_سرکاؤ);

	واپس reg;
}

صحیح regmap_read(ساخت regmap *rmap, غیرعلامتی صحیح reg, غیرعلامتی صحیح *val)
{
	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		واپس SBI_EINVAL;
	اگر (!rmap->reg_read)
		واپس SBI_ENOSYS;

	واپس rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

صحیح regmap_write(ساخت regmap *rmap, غیرعلامتی صحیح reg, غیرعلامتی صحیح val)
{
	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		واپس SBI_EINVAL;
	اگر (!rmap->reg_write)
		واپس SBI_ENOSYS;

	واپس rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

صحیح regmap_update_بٹ(ساخت regmap *rmap, غیرعلامتی صحیح reg,
		       غیرعلامتی صحیح نقاب, غیرعلامتی صحیح val)
{
	صحیح rc;
	غیرعلامتی صحیح reg_val;

	اگر (!rmap || !regmap_reg_valid(rmap, reg))
		واپس SBI_EINVAL;

	اگر (rmap->reg_update_بٹ) {
		واپس rmap->reg_update_بٹ(rmap, regmap_reg_addr(rmap, reg),
					     نقاب, val);
	} ورنہ اگر (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		اگر (rc)
			واپس rc;

		reg_val &= ~نقاب;
		reg_val |= val & نقاب;
		واپس rmap->reg_write(rmap, reg, reg_val);
	}

	واپس SBI_ENOSYS;
}
