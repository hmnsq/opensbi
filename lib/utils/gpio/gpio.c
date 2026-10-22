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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned ຈຳນວນເຕັມ id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		ຖ້າ (chip->id == id)
			ສົ່ງຄືນ chip;
	}

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	ຖ້າ (!gc)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (gpio_chip_find(gc->id))
		ສົ່ງຄືນ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ສົ່ງຄືນ 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	ຖ້າ (!gc)
		ສົ່ງຄືນ;

	sbi_list_del(&(gc->node));
}

ຈຳນວນເຕັມ gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	ຖ້າ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!gp->chip->get_direction)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ gp->chip->get_direction(gp);
}

ຈຳນວນເຕັມ gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	ຖ້າ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!gp->chip->direction_input)
		ສົ່ງຄືນ SBI_ENOSYS;

	ສົ່ງຄືນ gp->chip->direction_input(gp);
}

ຈຳນວນເຕັມ gpio_direction_output(sa_nracanaa gpio_pin *gp, ຈຳນວນເຕັມ value)
{
	ຖ້າ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!gp->chip->direction_output)
		ສົ່ງຄືນ SBI_ENOSYS;

	ຖ້າ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ສົ່ງຄືນ gp->chip->direction_output(gp, value);
}

ຈຳນວນເຕັມ gpio_get(sa_nracanaa gpio_pin *gp)
{
	ຈຳນວນເຕັມ value;

	ຖ້າ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!gp->chip->get)
		ສົ່ງຄືນ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ຖ້າ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ສົ່ງຄືນ value;
}

ຈຳນວນເຕັມ gpio_set(sa_nracanaa gpio_pin *gp, ຈຳນວນເຕັມ value)
{
	ຖ້າ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (!gp->chip->set)
		ສົ່ງຄືນ SBI_ENOSYS;

	ຖ້າ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ສົ່ງຄືນ 0;
}
