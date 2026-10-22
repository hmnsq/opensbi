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

		ᬬᬾᬦ᭄ (chip->id == id)
			ᬯᬮᬶ chip;
	}

	ᬯᬮᬶ NULL;
}

puur_naa_nka gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ᬬᬾᬦ᭄ (!gc)
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (gpio_chip_find(gc->id))
		ᬯᬮᬶ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ᬯᬮᬶ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ᬬᬾᬦ᭄ (!gc)
		ᬯᬮᬶ;

	sbi_list_del(&(gc->node));
}

puur_naa_nka gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ᬬᬾᬦ᭄ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (!gp->chip->get_direction)
		ᬯᬮᬶ SBI_ENOSYS;

	ᬯᬮᬶ gp->chip->get_direction(gp);
}

puur_naa_nka gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ᬬᬾᬦ᭄ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (!gp->chip->direction_input)
		ᬯᬮᬶ SBI_ENOSYS;

	ᬯᬮᬶ gp->chip->direction_input(gp);
}

puur_naa_nka gpio_direction_output(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᬬᬾᬦ᭄ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (!gp->chip->direction_output)
		ᬯᬮᬶ SBI_ENOSYS;

	ᬬᬾᬦ᭄ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ᬯᬮᬶ gp->chip->direction_output(gp, value);
}

puur_naa_nka gpio_get(sa_nracanaa gpio_pin *gp)
{
	puur_naa_nka value;

	ᬬᬾᬦ᭄ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (!gp->chip->get)
		ᬯᬮᬶ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ᬬᬾᬦ᭄ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ᬯᬮᬶ value;
}

puur_naa_nka gpio_set(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᬬᬾᬦ᭄ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᬯᬮᬶ SBI_EINVAL;
	ᬬᬾᬦ᭄ (!gp->chip->set)
		ᬯᬮᬶ SBI_ENOSYS;

	ᬬᬾᬦ᭄ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ᬯᬮᬶ 0;
}
