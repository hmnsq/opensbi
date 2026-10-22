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

struct gpio_chip *gpio_chip_find(unsigned целое id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		если (chip->id == id)
			вернуть chip;
	}

	вернуть NULL;
}

целое gpio_chip_add(struct gpio_chip *gc)
{
	если (!gc)
		вернуть SBI_EINVAL;
	если (gpio_chip_find(gc->id))
		вернуть SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	вернуть 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	если (!gc)
		вернуть;

	sbi_list_del(&(gc->node));
}

целое gpio_get_direction(struct gpio_pin *gp)
{
	если (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		вернуть SBI_EINVAL;
	если (!gp->chip->get_direction)
		вернуть SBI_ENOSYS;

	вернуть gp->chip->get_direction(gp);
}

целое gpio_direction_input(struct gpio_pin *gp)
{
	если (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		вернуть SBI_EINVAL;
	если (!gp->chip->direction_input)
		вернуть SBI_ENOSYS;

	вернуть gp->chip->direction_input(gp);
}

целое gpio_direction_output(struct gpio_pin *gp, целое value)
{
	если (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		вернуть SBI_EINVAL;
	если (!gp->chip->direction_output)
		вернуть SBI_ENOSYS;

	если (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	вернуть gp->chip->direction_output(gp, value);
}

целое gpio_get(struct gpio_pin *gp)
{
	целое value;

	если (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		вернуть SBI_EINVAL;
	если (!gp->chip->get)
		вернуть SBI_ENOSYS;

	value = gp->chip->get(gp);

	если (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	вернуть value;
}

целое gpio_set(struct gpio_pin *gp, целое value)
{
	если (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		вернуть SBI_EINVAL;
	если (!gp->chip->set)
		вернуть SBI_ENOSYS;

	если (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	вернуть 0;
}
