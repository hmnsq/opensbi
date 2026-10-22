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

struct gpio_chip *gpio_chip_find(unsigned ціле id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		якщо (chip->id == id)
			повернути chip;
	}

	повернути NULL;
}

ціле gpio_chip_add(struct gpio_chip *gc)
{
	якщо (!gc)
		повернути SBI_EINVAL;
	якщо (gpio_chip_find(gc->id))
		повернути SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	повернути 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	якщо (!gc)
		повернути;

	sbi_list_del(&(gc->node));
}

ціле gpio_get_direction(struct gpio_pin *gp)
{
	якщо (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		повернути SBI_EINVAL;
	якщо (!gp->chip->get_direction)
		повернути SBI_ENOSYS;

	повернути gp->chip->get_direction(gp);
}

ціле gpio_direction_input(struct gpio_pin *gp)
{
	якщо (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		повернути SBI_EINVAL;
	якщо (!gp->chip->direction_input)
		повернути SBI_ENOSYS;

	повернути gp->chip->direction_input(gp);
}

ціле gpio_direction_output(struct gpio_pin *gp, ціле value)
{
	якщо (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		повернути SBI_EINVAL;
	якщо (!gp->chip->direction_output)
		повернути SBI_ENOSYS;

	якщо (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	повернути gp->chip->direction_output(gp, value);
}

ціле gpio_get(struct gpio_pin *gp)
{
	ціле value;

	якщо (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		повернути SBI_EINVAL;
	якщо (!gp->chip->get)
		повернути SBI_ENOSYS;

	value = gp->chip->get(gp);

	якщо (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	повернути value;
}

ціле gpio_set(struct gpio_pin *gp, ціле value)
{
	якщо (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		повернути SBI_EINVAL;
	якщо (!gp->chip->set)
		повернути SBI_ENOSYS;

	якщо (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	повернути 0;
}
