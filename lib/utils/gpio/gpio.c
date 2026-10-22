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

		གལ་སྲིད (chip->id == id)
			ལོག chip;
	}

	ལོག NULL;
}

ཧྲིལ gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	གལ་སྲིད (!gc)
		ལོག SBI_EINVAL;
	གལ་སྲིད (gpio_chip_find(gc->id))
		ལོག SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ལོག 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	གལ་སྲིད (!gc)
		ལོག;

	sbi_list_del(&(gc->node));
}

ཧྲིལ gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	གལ་སྲིད (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!gp->chip->get_direction)
		ལོག SBI_ENOSYS;

	ལོག gp->chip->get_direction(gp);
}

ཧྲིལ gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	གལ་སྲིད (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!gp->chip->direction_input)
		ལོག SBI_ENOSYS;

	ལོག gp->chip->direction_input(gp);
}

ཧྲིལ gpio_direction_output(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	གལ་སྲིད (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!gp->chip->direction_output)
		ལོག SBI_ENOSYS;

	གལ་སྲིད (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ལོག gp->chip->direction_output(gp, value);
}

ཧྲིལ gpio_get(sa_nracanaa gpio_pin *gp)
{
	ཧྲིལ value;

	གལ་སྲིད (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!gp->chip->get)
		ལོག SBI_ENOSYS;

	value = gp->chip->get(gp);

	གལ་སྲིད (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ལོག value;
}

ཧྲིལ gpio_set(sa_nracanaa gpio_pin *gp, ཧྲིལ value)
{
	གལ་སྲིད (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ལོག SBI_EINVAL;
	གལ་སྲིད (!gp->chip->set)
		ལོག SBI_ENOSYS;

	གལ་སྲིད (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ལོག 0;
}
