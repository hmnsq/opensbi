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

		אם (chip->id == id)
			החזר chip;
	}

	החזר عدم;
}

صحيح gpio_chip_add(بنية gpio_chip *gc)
{
	אם (!gc)
		החזר SBI_EINVAL;
	אם (gpio_chip_find(gc->id))
		החזר SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	החזר 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	אם (!gc)
		החזר;

	sbi_list_del(&(gc->node));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	אם (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		החזר SBI_EINVAL;
	אם (!gp->chip->get_direction)
		החזר SBI_ENOSYS;

	החזר gp->chip->get_direction(gp);
}

صحيح gpio_direction_input(بنية gpio_pin *gp)
{
	אם (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		החזר SBI_EINVAL;
	אם (!gp->chip->direction_input)
		החזר SBI_ENOSYS;

	החזר gp->chip->direction_input(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح value)
{
	אם (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		החזר SBI_EINVAL;
	אם (!gp->chip->direction_output)
		החזר SBI_ENOSYS;

	אם (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	החזר gp->chip->direction_output(gp, value);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح value;

	אם (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		החזר SBI_EINVAL;
	אם (!gp->chip->get)
		החזר SBI_ENOSYS;

	value = gp->chip->get(gp);

	אם (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	החזר value;
}

صحيح gpio_set(بنية gpio_pin *gp, صحيح value)
{
	אם (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		החזר SBI_EINVAL;
	אם (!gp->chip->set)
		החזר SBI_ENOSYS;

	אם (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	החזר 0;
}
