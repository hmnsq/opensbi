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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned ចំនួនគត់ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		ប្រសិនបើ (chip->id == id)
			ត្រឡប់ chip;
	}

	ត្រឡប់ NULL;
}

ចំនួនគត់ gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ប្រសិនបើ (!gc)
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (gpio_chip_find(gc->id))
		ត្រឡប់ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ត្រឡប់ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ប្រសិនបើ (!gc)
		ត្រឡប់;

	sbi_list_del(&(gc->node));
}

ចំនួនគត់ gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ប្រសិនបើ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!gp->chip->get_direction)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ gp->chip->get_direction(gp);
}

ចំនួនគត់ gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ប្រសិនបើ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!gp->chip->direction_input)
		ត្រឡប់ SBI_ENOSYS;

	ត្រឡប់ gp->chip->direction_input(gp);
}

ចំនួនគត់ gpio_direction_output(sa_nracanaa gpio_pin *gp, ចំនួនគត់ value)
{
	ប្រសិនបើ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!gp->chip->direction_output)
		ត្រឡប់ SBI_ENOSYS;

	ប្រសិនបើ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ត្រឡប់ gp->chip->direction_output(gp, value);
}

ចំនួនគត់ gpio_get(sa_nracanaa gpio_pin *gp)
{
	ចំនួនគត់ value;

	ប្រសិនបើ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!gp->chip->get)
		ត្រឡប់ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ប្រសិនបើ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ត្រឡប់ value;
}

ចំនួនគត់ gpio_set(sa_nracanaa gpio_pin *gp, ចំនួនគត់ value)
{
	ប្រសិនបើ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (!gp->chip->set)
		ត្រឡប់ SBI_ENOSYS;

	ប្រសិនបើ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ត្រឡប់ 0;
}
