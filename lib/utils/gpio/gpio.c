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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned ကိန်းပြည့် id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		အကယ်၍ (chip->id == id)
			ပြန်ပေး chip;
	}

	ပြန်ပေး NULL;
}

ကိန်းပြည့် gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	အကယ်၍ (!gc)
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (gpio_chip_find(gc->id))
		ပြန်ပေး SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ပြန်ပေး 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	အကယ်၍ (!gc)
		ပြန်ပေး;

	sbi_list_del(&(gc->node));
}

ကိန်းပြည့် gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	အကယ်၍ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!gp->chip->get_direction)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး gp->chip->get_direction(gp);
}

ကိန်းပြည့် gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	အကယ်၍ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!gp->chip->direction_input)
		ပြန်ပေး SBI_ENOSYS;

	ပြန်ပေး gp->chip->direction_input(gp);
}

ကိန်းပြည့် gpio_direction_output(sa_nracanaa gpio_pin *gp, ကိန်းပြည့် value)
{
	အကယ်၍ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!gp->chip->direction_output)
		ပြန်ပေး SBI_ENOSYS;

	အကယ်၍ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ပြန်ပေး gp->chip->direction_output(gp, value);
}

ကိန်းပြည့် gpio_get(sa_nracanaa gpio_pin *gp)
{
	ကိန်းပြည့် value;

	အကယ်၍ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!gp->chip->get)
		ပြန်ပေး SBI_ENOSYS;

	value = gp->chip->get(gp);

	အကယ်၍ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ပြန်ပေး value;
}

ကိန်းပြည့် gpio_set(sa_nracanaa gpio_pin *gp, ကိန်းပြည့် value)
{
	အကယ်၍ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပြန်ပေး SBI_EINVAL;
	အကယ်၍ (!gp->chip->set)
		ပြန်ပေး SBI_ENOSYS;

	အကယ်၍ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ပြန်ပေး 0;
}
