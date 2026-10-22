/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi_utils/gpio/gpio.h>

স্থির SBI_LIST_HEAD(gpio_chip_list);

সংরচনা gpio_chip *gpio_chip_find(unsigned পূর্ণাঙ্ক id)
{
	সংরচনা sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		সংরচনা gpio_chip *chip = to_gpio_chip(pos);

		যদি (chip->id == id)
			ঘূৰাই chip;
	}

	ঘূৰাই রিক্ত;
}

পূর্ণাঙ্ক gpio_chip_add(সংরচনা gpio_chip *gc)
{
	যদি (!gc)
		ঘূৰাই SBI_EINVAL;
	যদি (gpio_chip_find(gc->id))
		ঘূৰাই SBI_EALREADY;

	sbi_list_add(&(gc->গ্ৰন্থি), &(gpio_chip_list));

	ঘূৰাই 0;
}

শূন্য gpio_chip_remove(সংরচনা gpio_chip *gc)
{
	যদি (!gc)
		ঘূৰাই;

	sbi_list_del(&(gc->গ্ৰন্থি));
}

পূর্ণাঙ্ক gpio_get_direction(সংরচনা gpio_pin *gp)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ঘূৰাই SBI_EINVAL;
	যদি (!gp->chip->get_direction)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই gp->chip->get_direction(gp);
}

পূর্ণাঙ্ক gpio_direction_প্ৰবেশ(সংরচনা gpio_pin *gp)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ঘূৰাই SBI_EINVAL;
	যদি (!gp->chip->direction_প্ৰবেশ)
		ঘূৰাই SBI_ENOSYS;

	ঘূৰাই gp->chip->direction_প্ৰবেশ(gp);
}

পূর্ণাঙ্ক gpio_direction_output(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ঘূৰাই SBI_EINVAL;
	যদি (!gp->chip->direction_output)
		ঘূৰাই SBI_ENOSYS;

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচেৰ)
		মান = মান == 0 ? 1 : 0;

	ঘূৰাই gp->chip->direction_output(gp, মান);
}

পূর্ণাঙ্ক gpio_get(সংরচনা gpio_pin *gp)
{
	পূর্ণাঙ্ক মান;

	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ঘূৰাই SBI_EINVAL;
	যদি (!gp->chip->get)
		ঘূৰাই SBI_ENOSYS;

	মান = gp->chip->get(gp);

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচেৰ)
		মান = মান == 0 ? 1 : 0;
	ঘূৰাই মান;
}

পূর্ণাঙ্ক gpio_set(সংরচনা gpio_pin *gp, পূর্ণাঙ্ক মান)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ঘূৰাই SBI_EINVAL;
	যদি (!gp->chip->set)
		ঘূৰাই SBI_ENOSYS;

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচেৰ)
		মান = মান == 0 ? 1 : 0;

	gp->chip->set(gp, মান);
	ঘূৰাই 0;
}
