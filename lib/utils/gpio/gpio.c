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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned จำนวนเต็ม id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		ถ้า (chip->id == id)
			คืน chip;
	}

	คืน NULL;
}

จำนวนเต็ม gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ถ้า (!gc)
		คืน SBI_EINVAL;
	ถ้า (gpio_chip_find(gc->id))
		คืน SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	คืน 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ถ้า (!gc)
		คืน;

	sbi_list_del(&(gc->node));
}

จำนวนเต็ม gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ถ้า (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		คืน SBI_EINVAL;
	ถ้า (!gp->chip->get_direction)
		คืน SBI_ENOSYS;

	คืน gp->chip->get_direction(gp);
}

จำนวนเต็ม gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ถ้า (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		คืน SBI_EINVAL;
	ถ้า (!gp->chip->direction_input)
		คืน SBI_ENOSYS;

	คืน gp->chip->direction_input(gp);
}

จำนวนเต็ม gpio_direction_output(sa_nracanaa gpio_pin *gp, จำนวนเต็ม value)
{
	ถ้า (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		คืน SBI_EINVAL;
	ถ้า (!gp->chip->direction_output)
		คืน SBI_ENOSYS;

	ถ้า (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	คืน gp->chip->direction_output(gp, value);
}

จำนวนเต็ม gpio_get(sa_nracanaa gpio_pin *gp)
{
	จำนวนเต็ม value;

	ถ้า (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		คืน SBI_EINVAL;
	ถ้า (!gp->chip->get)
		คืน SBI_ENOSYS;

	value = gp->chip->get(gp);

	ถ้า (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	คืน value;
}

จำนวนเต็ม gpio_set(sa_nracanaa gpio_pin *gp, จำนวนเต็ม value)
{
	ถ้า (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		คืน SBI_EINVAL;
	ถ้า (!gp->chip->set)
		คืน SBI_ENOSYS;

	ถ้า (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	คืน 0;
}
