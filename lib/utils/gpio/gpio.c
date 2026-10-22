/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/gpio/gpio.h>

ساكن SBI_LIST_HEAD(gpio_chip_list);

بنية gpio_chip *gpio_chip_find(unsigned صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		بنية gpio_chip *chip = to_gpio_chip(pos);

		إذا (chip->id == id)
			إرجاع chip;
	}

	إرجاع عدم;
}

صحيح gpio_chip_add(بنية gpio_chip *gc)
{
	إذا (!gc)
		إرجاع SBI_EINVAL;
	إذا (gpio_chip_find(gc->id))
		إرجاع SBI_EALREADY;

	sbi_list_add(&(gc->عقدة), &(gpio_chip_list));

	إرجاع 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	إذا (!gc)
		إرجاع;

	sbi_list_del(&(gc->عقدة));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		إرجاع SBI_EINVAL;
	إذا (!gp->chip->get_direction)
		إرجاع SBI_ENOSYS;

	إرجاع gp->chip->get_direction(gp);
}

صحيح gpio_direction_إدخال(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		إرجاع SBI_EINVAL;
	إذا (!gp->chip->direction_إدخال)
		إرجاع SBI_ENOSYS;

	إرجاع gp->chip->direction_إدخال(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح قيمة)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		إرجاع SBI_EINVAL;
	إذا (!gp->chip->direction_output)
		إرجاع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_ACTIVE_أدنى)
		قيمة = قيمة == 0 ? 1 : 0;

	إرجاع gp->chip->direction_output(gp, قيمة);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح قيمة;

	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		إرجاع SBI_EINVAL;
	إذا (!gp->chip->get)
		إرجاع SBI_ENOSYS;

	قيمة = gp->chip->get(gp);

	إذا (gp->flags & GPIO_FLAG_ACTIVE_أدنى)
		قيمة = قيمة == 0 ? 1 : 0;
	إرجاع قيمة;
}

صحيح gpio_set(بنية gpio_pin *gp, صحيح قيمة)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		إرجاع SBI_EINVAL;
	إذا (!gp->chip->set)
		إرجاع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_ACTIVE_أدنى)
		قيمة = قيمة == 0 ? 1 : 0;

	gp->chip->set(gp, قيمة);
	إرجاع 0;
}
