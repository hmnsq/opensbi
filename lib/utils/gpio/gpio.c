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

		ပေႃး (chip->id == id)
			ပွၵ်ႈ chip;
	}

	ပွၵ်ႈ NULL;
}

puur_naa_nka gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ပေႃး (!gc)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (gpio_chip_find(gc->id))
		ပွၵ်ႈ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ပွၵ်ႈ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ပေႃး (!gc)
		ပွၵ်ႈ;

	sbi_list_del(&(gc->node));
}

puur_naa_nka gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ပေႃး (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!gp->chip->get_direction)
		ပွၵ်ႈ SBI_ENOSYS;

	ပွၵ်ႈ gp->chip->get_direction(gp);
}

puur_naa_nka gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ပေႃး (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!gp->chip->direction_input)
		ပွၵ်ႈ SBI_ENOSYS;

	ပွၵ်ႈ gp->chip->direction_input(gp);
}

puur_naa_nka gpio_direction_output(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ပေႃး (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!gp->chip->direction_output)
		ပွၵ်ႈ SBI_ENOSYS;

	ပေႃး (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ပွၵ်ႈ gp->chip->direction_output(gp, value);
}

puur_naa_nka gpio_get(sa_nracanaa gpio_pin *gp)
{
	puur_naa_nka value;

	ပေႃး (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!gp->chip->get)
		ပွၵ်ႈ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ပေႃး (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ပွၵ်ႈ value;
}

puur_naa_nka gpio_set(sa_nracanaa gpio_pin *gp, puur_naa_nka value)
{
	ပေႃး (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (!gp->chip->set)
		ပွၵ်ႈ SBI_ENOSYS;

	ပေႃး (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ပွၵ်ႈ 0;
}
