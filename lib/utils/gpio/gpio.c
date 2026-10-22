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

		ᤀᤡ᤺ (chip->id == id)
			ᤀᤡᤴ chip;
	}

	ᤀᤡᤴ NULL;
}

puur_naa_nka gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ᤀᤡ᤺ (!gc)
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (gpio_chip_find(gc->id))
		ᤀᤡᤴ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ᤀᤡᤴ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ᤀᤡ᤺ (!gc)
		ᤀᤡᤴ;

	sbi_list_del(&(gc->node));
}

puur_naa_nka gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ᤀᤡ᤺ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!gp->chip->get_direction)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡᤴ gp->chip->get_direction(gp);
}

puur_naa_nka gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ᤀᤡ᤺ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!gp->chip->direction_input)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡᤴ gp->chip->direction_input(gp);
}

puur_naa_nka gpio_direction_output(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᤀᤡ᤺ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!gp->chip->direction_output)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡ᤺ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ᤀᤡᤴ gp->chip->direction_output(gp, value);
}

puur_naa_nka gpio_get(sa_nracanaa gpio_pin *gp)
{
	puur_naa_nka value;

	ᤀᤡ᤺ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!gp->chip->get)
		ᤀᤡᤴ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ᤀᤡ᤺ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ᤀᤡᤴ value;
}

puur_naa_nka gpio_set(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ᤀᤡ᤺ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ᤀᤡᤴ SBI_EINVAL;
	ᤀᤡ᤺ (!gp->chip->set)
		ᤀᤡᤴ SBI_ENOSYS;

	ᤀᤡ᤺ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ᤀᤡᤴ 0;
}
