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

		もし (chip->id == id)
			戻る chip;
	}

	戻る NULL;
}

整数 gpio_chip_add(struct gpio_chip *gc)
{
	もし (!gc)
		戻る SBI_EINVAL;
	もし (gpio_chip_find(gc->id))
		戻る SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	戻る 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	もし (!gc)
		戻る;

	sbi_list_del(&(gc->node));
}

整数 gpio_get_direction(struct gpio_pin *gp)
{
	もし (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		戻る SBI_EINVAL;
	もし (!gp->chip->get_direction)
		戻る SBI_ENOSYS;

	戻る gp->chip->get_direction(gp);
}

整数 gpio_direction_input(struct gpio_pin *gp)
{
	もし (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		戻る SBI_EINVAL;
	もし (!gp->chip->direction_input)
		戻る SBI_ENOSYS;

	戻る gp->chip->direction_input(gp);
}

整数 gpio_direction_output(struct gpio_pin *gp, 整数 value)
{
	もし (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		戻る SBI_EINVAL;
	もし (!gp->chip->direction_output)
		戻る SBI_ENOSYS;

	もし (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	戻る gp->chip->direction_output(gp, value);
}

整数 gpio_get(struct gpio_pin *gp)
{
	整数 value;

	もし (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		戻る SBI_EINVAL;
	もし (!gp->chip->get)
		戻る SBI_ENOSYS;

	value = gp->chip->get(gp);

	もし (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	戻る value;
}

整数 gpio_set(struct gpio_pin *gp, 整数 value)
{
	もし (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		戻る SBI_EINVAL;
	もし (!gp->chip->set)
		戻る SBI_ENOSYS;

	もし (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	戻る 0;
}
