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

struct gpio_chip *gpio_chip_find(unsigned 整数 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		如果 (chip->id == id)
			返回 chip;
	}

	返回 NULL;
}

整数 gpio_chip_add(struct gpio_chip *gc)
{
	如果 (!gc)
		返回 SBI_EINVAL;
	如果 (gpio_chip_find(gc->id))
		返回 SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	返回 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	如果 (!gc)
		返回;

	sbi_list_del(&(gc->node));
}

整数 gpio_get_direction(struct gpio_pin *gp)
{
	如果 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		返回 SBI_EINVAL;
	如果 (!gp->chip->get_direction)
		返回 SBI_ENOSYS;

	返回 gp->chip->get_direction(gp);
}

整数 gpio_direction_input(struct gpio_pin *gp)
{
	如果 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		返回 SBI_EINVAL;
	如果 (!gp->chip->direction_input)
		返回 SBI_ENOSYS;

	返回 gp->chip->direction_input(gp);
}

整数 gpio_direction_output(struct gpio_pin *gp, 整数 value)
{
	如果 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		返回 SBI_EINVAL;
	如果 (!gp->chip->direction_output)
		返回 SBI_ENOSYS;

	如果 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	返回 gp->chip->direction_output(gp, value);
}

整数 gpio_get(struct gpio_pin *gp)
{
	整数 value;

	如果 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		返回 SBI_EINVAL;
	如果 (!gp->chip->get)
		返回 SBI_ENOSYS;

	value = gp->chip->get(gp);

	如果 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	返回 value;
}

整数 gpio_set(struct gpio_pin *gp, 整数 value)
{
	如果 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		返回 SBI_EINVAL;
	如果 (!gp->chip->set)
		返回 SBI_ENOSYS;

	如果 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	返回 0;
}
