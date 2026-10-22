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

		ᱡᱩᱫᱤ (chip->id == id)
			ᱨᱩᱣᱟᱹᱲ chip;
	}

	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ᱡᱩᱫᱤ (!gc)
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (gpio_chip_find(gc->id))
		ᱨᱩᱣᱟᱹᱲ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ᱨᱩᱣᱟᱹᱲ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ᱡᱩᱫᱤ (!gc)
		ᱨᱩᱣᱟᱹᱲ;

	sbi_list_del(&(gc->node));
}

puur_naa_nka gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ᱡᱩᱫᱤ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!gp->chip->get_direction)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱨᱩᱣᱟᱹᱲ gp->chip->get_direction(gp);
}

puur_naa_nka gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ᱡᱩᱫᱤ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!gp->chip->direction_input)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱨᱩᱣᱟᱹᱲ gp->chip->direction_input(gp);
}

puur_naa_nka gpio_direction_output(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᱡᱩᱫᱤ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!gp->chip->direction_output)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱡᱩᱫᱤ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ᱨᱩᱣᱟᱹᱲ gp->chip->direction_output(gp, value);
}

puur_naa_nka gpio_get(sa_nracanaa gpio_pin *gp)
{
	puur_naa_nka value;

	ᱡᱩᱫᱤ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!gp->chip->get)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ᱡᱩᱫᱤ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ᱨᱩᱣᱟᱹᱲ value;
}

puur_naa_nka gpio_set(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᱡᱩᱫᱤ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᱨᱩᱣᱟᱹᱲ SBI_EINVAL;
	ᱡᱩᱫᱤ (!gp->chip->set)
		ᱨᱩᱣᱟᱹᱲ SBI_ENOSYS;

	ᱡᱩᱫᱤ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ᱨᱩᱣᱟᱹᱲ 0;
}
