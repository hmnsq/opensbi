/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/regmap/regmap.h>

अटल SBI_LIST_शीर्ष(regmap_list);

काष्ठा regmap *regmap_find(अचिन्हित पूर्णांक id)
{
	काष्ठा sbi_dlist *pos;

	sbi_list_for_each(pos, &(regmap_list)) {
		काष्ठा regmap *rmap = to_regmap(pos);

		अगर (rmap->id == id)
			वापस rmap;
	}

	वापस शून्य;
}

पूर्णांक regmap_add(काष्ठा regmap *rmap)
{
	अगर (!rmap)
		वापस SBI_EINVAL;
	अगर (regmap_find(rmap->id))
		वापस SBI_EALREADY;

	sbi_list_add(&(rmap->गाँठ), &(regmap_list));

	वापस 0;
}

व्योम regmap_remove(काष्ठा regmap *rmap)
{
	अगर (!rmap)
		वापस;

	sbi_list_del(&(rmap->गाँठ));
}

अटल bool regmap_reg_valid(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg)
{
	अगर ((reg >= rmap->reg_max) ||
	    (reg & (rmap->reg_stride - 1)))
		वापस false;
	वापस true;
}

अटल अचिन्हित पूर्णांक regmap_reg_addr(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg)
{
	reg += rmap->reg_base;

	अगर (rmap->reg_shift > 0)
		reg >>= rmap->reg_shift;
	अन्यथा अगर (rmap->reg_shift < 0)
		reg <<= -(rmap->reg_shift);

	वापस reg;
}

पूर्णांक regmap_read(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg, अचिन्हित पूर्णांक *val)
{
	अगर (!rmap || !regmap_reg_valid(rmap, reg))
		वापस SBI_EINVAL;
	अगर (!rmap->reg_read)
		वापस SBI_ENOSYS;

	वापस rmap->reg_read(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णांक regmap_write(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg, अचिन्हित पूर्णांक val)
{
	अगर (!rmap || !regmap_reg_valid(rmap, reg))
		वापस SBI_EINVAL;
	अगर (!rmap->reg_write)
		वापस SBI_ENOSYS;

	वापस rmap->reg_write(rmap, regmap_reg_addr(rmap, reg), val);
}

पूर्णांक regmap_update_bits(काष्ठा regmap *rmap, अचिन्हित पूर्णांक reg,
		       अचिन्हित पूर्णांक mask, अचिन्हित पूर्णांक val)
{
	पूर्णांक rc;
	अचिन्हित पूर्णांक reg_val;

	अगर (!rmap || !regmap_reg_valid(rmap, reg))
		वापस SBI_EINVAL;

	अगर (rmap->reg_update_bits) {
		वापस rmap->reg_update_bits(rmap, regmap_reg_addr(rmap, reg),
					     mask, val);
	} अन्यथा अगर (rmap->reg_read && rmap->reg_write) {
		reg = regmap_reg_addr(rmap, reg);

		rc = rmap->reg_read(rmap, reg, &reg_val);
		अगर (rc)
			वापस rc;

		reg_val &= ~mask;
		reg_val |= val & mask;
		वापस rmap->reg_write(rmap, reg, reg_val);
	}

	वापस SBI_ENOSYS;
}
