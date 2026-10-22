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

struct gpio_chip *gpio_chip_find(unsigned цяло id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		ако (chip->id == id)
			върни chip;
	}

	върни NULL;
}

цяло gpio_chip_add(struct gpio_chip *gc)
{
	ако (!gc)
		върни SBI_EINVAL;
	ако (gpio_chip_find(gc->id))
		върни SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	върни 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	ако (!gc)
		върни;

	sbi_list_del(&(gc->node));
}

цяло gpio_get_direction(struct gpio_pin *gp)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		върни SBI_EINVAL;
	ако (!gp->chip->get_direction)
		върни SBI_ENOSYS;

	върни gp->chip->get_direction(gp);
}

цяло gpio_direction_input(struct gpio_pin *gp)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		върни SBI_EINVAL;
	ако (!gp->chip->direction_input)
		върни SBI_ENOSYS;

	върни gp->chip->direction_input(gp);
}

цяло gpio_direction_output(struct gpio_pin *gp, цяло value)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		върни SBI_EINVAL;
	ако (!gp->chip->direction_output)
		върни SBI_ENOSYS;

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	върни gp->chip->direction_output(gp, value);
}

цяло gpio_get(struct gpio_pin *gp)
{
	цяло value;

	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		върни SBI_EINVAL;
	ако (!gp->chip->get)
		върни SBI_ENOSYS;

	value = gp->chip->get(gp);

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	върни value;
}

цяло gpio_set(struct gpio_pin *gp, цяло value)
{
	ако (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		върни SBI_EINVAL;
	ако (!gp->chip->set)
		върни SBI_ENOSYS;

	ако (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	върни 0;
}
