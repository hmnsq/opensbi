/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/gpio/gpio.h>

ساكن SBI_LIST_HEAD(gpio_chip_list);

بنية gpio_chip *gpio_chip_find(unsigned صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		بنية gpio_chip *chip = to_gpio_chip(pos);

		ܐܢ (chip->id == id)
			ܦܢܐ chip;
	}

	ܦܢܐ عدم;
}

صحيح gpio_chip_add(بنية gpio_chip *gc)
{
	ܐܢ (!gc)
		ܦܢܐ SBI_EINVAL;
	ܐܢ (gpio_chip_find(gc->id))
		ܦܢܐ SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ܦܢܐ 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	ܐܢ (!gc)
		ܦܢܐ;

	sbi_list_del(&(gc->node));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	ܐܢ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!gp->chip->get_direction)
		ܦܢܐ SBI_ENOSYS;

	ܦܢܐ gp->chip->get_direction(gp);
}

صحيح gpio_direction_input(بنية gpio_pin *gp)
{
	ܐܢ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!gp->chip->direction_input)
		ܦܢܐ SBI_ENOSYS;

	ܦܢܐ gp->chip->direction_input(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح value)
{
	ܐܢ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!gp->chip->direction_output)
		ܦܢܐ SBI_ENOSYS;

	ܐܢ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ܦܢܐ gp->chip->direction_output(gp, value);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح value;

	ܐܢ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!gp->chip->get)
		ܦܢܐ SBI_ENOSYS;

	value = gp->chip->get(gp);

	ܐܢ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ܦܢܐ value;
}

صحيح gpio_set(بنية gpio_pin *gp, صحيح value)
{
	ܐܢ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ܦܢܐ SBI_EINVAL;
	ܐܢ (!gp->chip->set)
		ܦܢܐ SBI_ENOSYS;

	ܐܢ (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ܦܢܐ 0;
}
