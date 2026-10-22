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

struct gpio_chip *gpio_chip_find(unsigned 정수 id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		만약 (chip->id == id)
			반환 chip;
	}

	반환 NULL;
}

정수 gpio_chip_add(struct gpio_chip *gc)
{
	만약 (!gc)
		반환 SBI_EINVAL;
	만약 (gpio_chip_find(gc->id))
		반환 SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	반환 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	만약 (!gc)
		반환;

	sbi_list_del(&(gc->node));
}

정수 gpio_get_direction(struct gpio_pin *gp)
{
	만약 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		반환 SBI_EINVAL;
	만약 (!gp->chip->get_direction)
		반환 SBI_ENOSYS;

	반환 gp->chip->get_direction(gp);
}

정수 gpio_direction_input(struct gpio_pin *gp)
{
	만약 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		반환 SBI_EINVAL;
	만약 (!gp->chip->direction_input)
		반환 SBI_ENOSYS;

	반환 gp->chip->direction_input(gp);
}

정수 gpio_direction_output(struct gpio_pin *gp, 정수 value)
{
	만약 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		반환 SBI_EINVAL;
	만약 (!gp->chip->direction_output)
		반환 SBI_ENOSYS;

	만약 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	반환 gp->chip->direction_output(gp, value);
}

정수 gpio_get(struct gpio_pin *gp)
{
	정수 value;

	만약 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		반환 SBI_EINVAL;
	만약 (!gp->chip->get)
		반환 SBI_ENOSYS;

	value = gp->chip->get(gp);

	만약 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	반환 value;
}

정수 gpio_set(struct gpio_pin *gp, 정수 value)
{
	만약 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		반환 SBI_EINVAL;
	만약 (!gp->chip->set)
		반환 SBI_ENOSYS;

	만약 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	반환 0;
}
