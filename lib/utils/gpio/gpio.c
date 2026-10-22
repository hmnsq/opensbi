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
			أرجع chip;
	}

	أرجع عدم;
}

صحيح gpio_chip_add(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع SBI_EINVAL;
	إذا (gpio_chip_find(gc->id))
		أرجع SBI_EALREADY;

	sbi_list_add(&(gc->تۈگۈن), &(gpio_chip_list));

	أرجع 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع;

	sbi_list_del(&(gc->تۈگۈن));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get_direction)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->get_direction(gp);
}

صحيح gpio_direction_كىرگۈزۈش(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_كىرگۈزۈش)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->direction_كىرگۈزۈش(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح قىممەت)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_output)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_ACTIVE_تۆۋەن)
		قىممەت = قىممەت == 0 ? 1 : 0;

	أرجع gp->chip->direction_output(gp, قىممەت);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح قىممەت;

	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get)
		أرجع SBI_ENOSYS;

	قىممەت = gp->chip->get(gp);

	إذا (gp->flags & GPIO_FLAG_ACTIVE_تۆۋەن)
		قىممەت = قىممەت == 0 ? 1 : 0;
	أرجع قىممەت;
}

صحيح gpio_set(بنية gpio_pin *gp, صحيح قىممەت)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->set)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_ACTIVE_تۆۋەن)
		قىممەت = قىممەت == 0 ? 1 : 0;

	gp->chip->set(gp, قىممەت);
	أرجع 0;
}
