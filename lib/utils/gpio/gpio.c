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

		𐡄𐡍 (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح gpio_chip_add(بنية gpio_chip *gc)
{
	𐡄𐡍 (!gc)
		أرجع SBI_EINVAL;
	𐡄𐡍 (gpio_chip_find(gc->id))
		أرجع SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	أرجع 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	𐡄𐡍 (!gc)
		أرجع;

	sbi_list_del(&(gc->node));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	𐡄𐡍 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!gp->chip->get_direction)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->get_direction(gp);
}

صحيح gpio_direction_input(بنية gpio_pin *gp)
{
	𐡄𐡍 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!gp->chip->direction_input)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->direction_input(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح value)
{
	𐡄𐡍 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!gp->chip->direction_output)
		أرجع SBI_ENOSYS;

	𐡄𐡍 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	أرجع gp->chip->direction_output(gp, value);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح value;

	𐡄𐡍 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!gp->chip->get)
		أرجع SBI_ENOSYS;

	value = gp->chip->get(gp);

	𐡄𐡍 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	أرجع value;
}

صحيح gpio_set(بنية gpio_pin *gp, صحيح value)
{
	𐡄𐡍 (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		أرجع SBI_EINVAL;
	𐡄𐡍 (!gp->chip->set)
		أرجع SBI_ENOSYS;

	𐡄𐡍 (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	أرجع 0;
}
