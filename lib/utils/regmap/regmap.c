/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/regmap/regmap.h>

स्थिर SBI_LIST_शीर्ष(regmap_list);

संरचना regmap *regmap_find(अचिह्नित पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		संरचना regmap *rmap = गन्तव्यम्_regmap(pos);

		यदि (rmap->id == id)
			प्रत्यागम rmap;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क regmap_सङ्कलय(संरचना regmap *rmap)
{
	यदि (!rmap)
		प्रत्यागम SBI_EINVAL;
	यदि (regmap_find(rmap->id))
		प्रत्यागम SBI_EALREADY;

	sbi_list_सङ्कलय(&(rmap->ग्रन्थिः), &(regmap_list));

	प्रत्यागम 0;
}

शून्य regmap_remove(संरचना regmap *rmap)
{
	यदि (!rmap)
		प्रत्यागम;

	sbi_list_del(&(rmap->ग्रन्थिः));
}

स्थिर bool regmap_reg_valid(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg)
{
	यदि ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		प्रत्यागम false;
	प्रत्यागम true;
}

स्थिर अचिह्नित पूर्णाङ्क regmap_reg_addr(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg)
{
	reg += rmap->reg_आधारः;

	यदि (rmap->reg_सरणम् > 0)
		reg >>= rmap->reg_सरणम्;
	अन्यथा यदि (rmap->reg_सरणम् < 0)
		reg <<= -(rmap->reg_सरणम्);

	प्रत्यागम reg;
}

पूर्णाङ्क regmap_read(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg, अचिह्नित पूर्णाङ्क *val)
{
	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;
	यदि (!rmap->reg_read)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_write(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg, अचिह्नित पूर्णाङ्क val)
{
	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;
	यदि (!rmap->reg_write)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णाङ्क regmap_update_द्वयङ्काः(संरचना regmap *rmap, अचिह्नित पूर्णाङ्क reg,
		       अचिह्नित पूर्णाङ्क आवरणम्, अचिह्नित पूर्णाङ्क val)
{
	पूर्णाङ्क rc;
	अचिह्नित पूर्णाङ्क reg_val;

	यदि (!rmap || !regmap_reg_valid(rmap, reg))
		प्रत्यागम SBI_EINVAL;

	यदि (rmap->reg_update_द्वयङ्काः) {
		प्रत्यागम rmap->reg_update_द्वयङ्काः(rmap, regmap_reg_addr(rmap, reg),
					     आवरणम्, val);
	} अन्यथा यदि (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		यदि (rc)
			प्रत्यागम rc;

		reg_val &= ~आवरणम्;
		reg_val |= val & आवरणम्;
		प्रत्यागम rmap->reg_write(rmap, reg, reg_val);
	}

	प्रत्यागम SBI_ENOSYS;
}
