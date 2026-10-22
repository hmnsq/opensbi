/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/gpio/gpio.h>

स्थिर SBI_LIST_HEAD(gpio_chip_list);

संरचना gpio_chip *gpio_chip_find(अचिन्हित पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		संरचना gpio_chip *chip = to_gpio_chip(pos);

		यदि (chip->id == id)
			फर्काऊ chip;
	}

	फर्काऊ रिक्त;
}

पूर्णाङ्क gpio_chip_add(संरचना gpio_chip *gc)
{
	यदि (!gc)
		फर्काऊ SBI_EINVAL;
	यदि (gpio_chip_find(gc->id))
		फर्काऊ SBI_EALREADY;

	sbi_list_add(&(gc->गाँठो), &(gpio_chip_list));

	फर्काऊ 0;
}

शून्य gpio_chip_remove(संरचना gpio_chip *gc)
{
	यदि (!gc)
		फर्काऊ;

	sbi_list_del(&(gc->गाँठो));
}

पूर्णाङ्क gpio_get_direction(संरचना gpio_pin *gp)
{
	यदि (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		फर्काऊ SBI_EINVAL;
	यदि (!gp->chip->get_direction)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ gp->chip->get_direction(gp);
}

पूर्णाङ्क gpio_direction_आगत(संरचना gpio_pin *gp)
{
	यदि (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		फर्काऊ SBI_EINVAL;
	यदि (!gp->chip->direction_आगत)
		फर्काऊ SBI_ENOSYS;

	फर्काऊ gp->chip->direction_आगत(gp);
}

पूर्णाङ्क gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	यदि (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		फर्काऊ SBI_EINVAL;
	यदि (!gp->chip->direction_output)
		फर्काऊ SBI_ENOSYS;

	यदि (gp->flags & GPIO_FLAG_ACTIVE_तल्लो)
		मान = मान == 0 ? 1 : 0;

	फर्काऊ gp->chip->direction_output(gp, मान);
}

पूर्णाङ्क gpio_get(संरचना gpio_pin *gp)
{
	पूर्णाङ्क मान;

	यदि (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		फर्काऊ SBI_EINVAL;
	यदि (!gp->chip->get)
		फर्काऊ SBI_ENOSYS;

	मान = gp->chip->get(gp);

	यदि (gp->flags & GPIO_FLAG_ACTIVE_तल्लो)
		मान = मान == 0 ? 1 : 0;
	फर्काऊ मान;
}

पूर्णाङ्क gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मान)
{
	यदि (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		फर्काऊ SBI_EINVAL;
	यदि (!gp->chip->set)
		फर्काऊ SBI_ENOSYS;

	यदि (gp->flags & GPIO_FLAG_ACTIVE_तल्लो)
		मान = मान == 0 ? 1 : 0;

	gp->chip->set(gp, मान);
	फर्काऊ 0;
}
