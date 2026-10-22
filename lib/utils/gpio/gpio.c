/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi_utils/gpio/gpio.h>

static SBI_LIST_HEAD(gpio_chip_list);

sa_nracanaa gpio_chip *gpio_chip_find(unsigned ཧྲིལ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		གལ་ཏེ (chip->id == id)
			ཕྱིར chip;
	}

	ཕྱིར NULL;
}

ཧྲིལ gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	གལ་ཏེ (!gc)
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (gpio_chip_find(gc->id))
		ཕྱིར SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ཕྱིར 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	གལ་ཏེ (!gc)
		ཕྱིར;

	sbi_list_del(&(gc->node));
}

ཧྲིལ gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	གལ་ཏེ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (!gp->chip->get_direction)
		ཕྱིར SBI_ENOSYS;

	ཕྱིར gp->chip->get_direction(gp);
}

ཧྲིལ gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	གལ་ཏེ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (!gp->chip->direction_input)
		ཕྱིར SBI_ENOSYS;

	ཕྱིར gp->chip->direction_input(gp);
}

ཧྲིལ gpio_direction_output(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	གལ་ཏེ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (!gp->chip->direction_output)
		ཕྱིར SBI_ENOSYS;

	གལ་ཏེ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ཕྱིར gp->chip->direction_output(gp, value);
}

ཧྲིལ gpio_get(sa_nracanaa gpio_pin *gp)
{
	ཧྲིལ value;

	གལ་ཏེ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (!gp->chip->get)
		ཕྱིར SBI_ENOSYS;

	value = gp->chip->get(gp);

	གལ་ཏེ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ཕྱིར value;
}

ཧྲིལ gpio_set(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	གལ་ཏེ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ཕྱིར SBI_EINVAL;
	གལ་ཏེ (!gp->chip->set)
		ཕྱིར SBI_ENOSYS;

	གལ་ཏེ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ཕྱིར 0;
}
