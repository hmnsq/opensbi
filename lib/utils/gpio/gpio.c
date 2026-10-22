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

ساكن SBI_LIST_سر(gpio_chip_list);

بنية gpio_chip *gpio_chip_ڳوليو(بنانشان صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		بنية gpio_chip *chip = منزل_gpio_chip(pos);

		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح gpio_chip_جوڙيو(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع SBI_EINVAL;
	إذا (gpio_chip_ڳوليو(gc->id))
		أرجع SBI_EALREADY;

	sbi_list_جوڙيو(&(gc->ڳنڍ), &(gpio_chip_list));

	أرجع 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع;

	sbi_list_del(&(gc->ڳنڍ));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->وٿي))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get_direction)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->get_direction(gp);
}

صحيح gpio_direction_داخلا(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->وٿي))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_داخلا)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->direction_داخلا(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح قدر)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->وٿي))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_output)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_سرگرم_هيٺيون)
		قدر = قدر == 0 ? 1 : 0;

	أرجع gp->chip->direction_output(gp, قدر);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح قدر;

	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->وٿي))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get)
		أرجع SBI_ENOSYS;

	قدر = gp->chip->get(gp);

	إذا (gp->flags & GPIO_FLAG_سرگرم_هيٺيون)
		قدر = قدر == 0 ? 1 : 0;
	أرجع قدر;
}

صحيح gpio_سيٽ(بنية gpio_pin *gp, صحيح قدر)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->وٿي))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->سيٽ)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_سرگرم_هيٺيون)
		قدر = قدر == 0 ? 1 : 0;

	gp->chip->سيٽ(gp, قدر);
	أرجع 0;
}
