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

struct gpio_chip *gpio_chip_find(unsigned цео id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		ако (chip->id == id)
			врати chip;
	}

	врати NULL;
}

цео gpio_chip_add(struct gpio_chip *gc)
{
	ако (!gc)
		врати SBI_EINVAL;
	ако (gpio_chip_find(gc->id))
		врати SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	врати 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	ако (!gc)
		врати;

	sbi_list_del(&(gc->node));
}

цео gpio_get_direction(struct gpio_pin *gp)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		врати SBI_EINVAL;
	ако (!gp->chip->get_direction)
		врати SBI_ENOSYS;

	врати gp->chip->get_direction(gp);
}

цео gpio_direction_input(struct gpio_pin *gp)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		врати SBI_EINVAL;
	ако (!gp->chip->direction_input)
		врати SBI_ENOSYS;

	врати gp->chip->direction_input(gp);
}

цео gpio_direction_output(struct gpio_pin *gp, цео value)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		врати SBI_EINVAL;
	ако (!gp->chip->direction_output)
		врати SBI_ENOSYS;

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	врати gp->chip->direction_output(gp, value);
}

цео gpio_get(struct gpio_pin *gp)
{
	цео value;

	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		врати SBI_EINVAL;
	ако (!gp->chip->get)
		врати SBI_ENOSYS;

	value = gp->chip->get(gp);

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	врати value;
}

цео gpio_set(struct gpio_pin *gp, цео value)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		врати SBI_EINVAL;
	ако (!gp->chip->set)
		врати SBI_ENOSYS;

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	врати 0;
}
