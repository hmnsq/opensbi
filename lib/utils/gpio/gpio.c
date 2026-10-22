/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/gpio/gpio.h>

স্থির SBI_LIST_শীর্ষ(gpio_chip_list);

কাঠামো gpio_chip *gpio_chip_find(unsigned পূর্ণসংখ্যা id)
{
	কাঠামো sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		কাঠামো gpio_chip *chip = to_gpio_chip(pos);

		যদি (chip->id == id)
			ফেরত chip;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা gpio_chip_add(কাঠামো gpio_chip *gc)
{
	যদি (!gc)
		ফেরত SBI_EINVAL;
	যদি (gpio_chip_find(gc->id))
		ফেরত SBI_EALREADY;

	sbi_list_add(&(gc->গ্রন্থি), &(gpio_chip_list));

	ফেরত 0;
}

শূন্য gpio_chip_remove(কাঠামো gpio_chip *gc)
{
	যদি (!gc)
		ফেরত;

	sbi_list_del(&(gc->গ্রন্থি));
}

পূর্ণসংখ্যা gpio_get_direction(কাঠামো gpio_pin *gp)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ফেরত SBI_EINVAL;
	যদি (!gp->chip->get_direction)
		ফেরত SBI_ENOSYS;

	ফেরত gp->chip->get_direction(gp);
}

পূর্ণসংখ্যা gpio_direction_প্রবেশ(কাঠামো gpio_pin *gp)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ফেরত SBI_EINVAL;
	যদি (!gp->chip->direction_প্রবেশ)
		ফেরত SBI_ENOSYS;

	ফেরত gp->chip->direction_প্রবেশ(gp);
}

পূর্ণসংখ্যা gpio_direction_output(কাঠামো gpio_pin *gp, পূর্ণসংখ্যা মান)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ফেরত SBI_EINVAL;
	যদি (!gp->chip->direction_output)
		ফেরত SBI_ENOSYS;

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচের)
		মান = মান == 0 ? 1 : 0;

	ফেরত gp->chip->direction_output(gp, মান);
}

পূর্ণসংখ্যা gpio_get(কাঠামো gpio_pin *gp)
{
	পূর্ণসংখ্যা মান;

	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ফেরত SBI_EINVAL;
	যদি (!gp->chip->get)
		ফেরত SBI_ENOSYS;

	মান = gp->chip->get(gp);

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচের)
		মান = মান == 0 ? 1 : 0;
	ফেরত মান;
}

পূর্ণসংখ্যা gpio_set(কাঠামো gpio_pin *gp, পূর্ণসংখ্যা মান)
{
	যদি (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ফেরত SBI_EINVAL;
	যদি (!gp->chip->set)
		ফেরত SBI_ENOSYS;

	যদি (gp->flags & GPIO_FLAG_ACTIVE_নিচের)
		মান = মান == 0 ? 1 : 0;

	gp->chip->set(gp, মান);
	ফেরত 0;
}
