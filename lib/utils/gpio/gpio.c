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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned puur_naa_nka id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		ᰀᰧ (chip->id == id)
			ᰀᰧᰌ chip;
	}

	ᰀᰧᰌ NULL;
}

puur_naa_nka gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ᰀᰧ (!gc)
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (gpio_chip_find(gc->id))
		ᰀᰧᰌ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ᰀᰧᰌ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ᰀᰧ (!gc)
		ᰀᰧᰌ;

	sbi_list_del(&(gc->node));
}

puur_naa_nka gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ᰀᰧ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (!gp->chip->get_direction)
		ᰀᰧᰌ SBI_ENOSYS;

	ᰀᰧᰌ gp->chip->get_direction(gp);
}

puur_naa_nka gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ᰀᰧ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (!gp->chip->direction_input)
		ᰀᰧᰌ SBI_ENOSYS;

	ᰀᰧᰌ gp->chip->direction_input(gp);
}

puur_naa_nka gpio_direction_output(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᰀᰧ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (!gp->chip->direction_output)
		ᰀᰧᰌ SBI_ENOSYS;

	ᰀᰧ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ᰀᰧᰌ gp->chip->direction_output(gp, value);
}

puur_naa_nka gpio_get(sa_nracanaa gpio_pin *gp)
{
	puur_naa_nka value;

	ᰀᰧ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (!gp->chip->get)
		ᰀᰧᰌ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ᰀᰧ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ᰀᰧᰌ value;
}

puur_naa_nka gpio_set(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᰀᰧ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᰀᰧᰌ SBI_EINVAL;
	ᰀᰧ (!gp->chip->set)
		ᰀᰧᰌ SBI_ENOSYS;

	ᰀᰧ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ᰀᰧᰌ 0;
}
