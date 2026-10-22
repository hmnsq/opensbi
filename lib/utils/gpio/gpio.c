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

struct gpio_chip *gpio_chip_find(unsigned số_nguyên id)
{
	struct sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		struct gpio_chip *chip = to_gpio_chip(pos);

		nếu (chip->id == id)
			trả_về chip;
	}

	trả_về NULL;
}

số_nguyên gpio_chip_add(struct gpio_chip *gc)
{
	nếu (!gc)
		trả_về SBI_EINVAL;
	nếu (gpio_chip_find(gc->id))
		trả_về SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	trả_về 0;
}

void gpio_chip_remove(struct gpio_chip *gc)
{
	nếu (!gc)
		trả_về;

	sbi_list_del(&(gc->node));
}

số_nguyên gpio_get_direction(struct gpio_pin *gp)
{
	nếu (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		trả_về SBI_EINVAL;
	nếu (!gp->chip->get_direction)
		trả_về SBI_ENOSYS;

	trả_về gp->chip->get_direction(gp);
}

số_nguyên gpio_direction_input(struct gpio_pin *gp)
{
	nếu (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		trả_về SBI_EINVAL;
	nếu (!gp->chip->direction_input)
		trả_về SBI_ENOSYS;

	trả_về gp->chip->direction_input(gp);
}

số_nguyên gpio_direction_output(struct gpio_pin *gp, số_nguyên value)
{
	nếu (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		trả_về SBI_EINVAL;
	nếu (!gp->chip->direction_output)
		trả_về SBI_ENOSYS;

	nếu (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	trả_về gp->chip->direction_output(gp, value);
}

số_nguyên gpio_get(struct gpio_pin *gp)
{
	số_nguyên value;

	nếu (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		trả_về SBI_EINVAL;
	nếu (!gp->chip->get)
		trả_về SBI_ENOSYS;

	value = gp->chip->get(gp);

	nếu (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	trả_về value;
}

số_nguyên gpio_set(struct gpio_pin *gp, số_nguyên value)
{
	nếu (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		trả_về SBI_EINVAL;
	nếu (!gp->chip->set)
		trả_về SBI_ENOSYS;

	nếu (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	trả_về 0;
}
