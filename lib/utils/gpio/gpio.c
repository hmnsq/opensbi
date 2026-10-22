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

بنية gpio_chip *gpio_chip_پیدا_کن(بیعلامت صحيح id)
{
	بنية sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		بنية gpio_chip *chip = مقصد_gpio_chip(pos);

		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح gpio_chip_جمع_کن(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع SBI_EINVAL;
	إذا (gpio_chip_پیدا_کن(gc->id))
		أرجع SBI_EALREADY;

	sbi_list_جمع_کن(&(gc->گره), &(gpio_chip_list));

	أرجع 0;
}

فراغ gpio_chip_remove(بنية gpio_chip *gc)
{
	إذا (!gc)
		أرجع;

	sbi_list_del(&(gc->گره));
}

صحيح gpio_get_direction(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->جابه_جایی))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get_direction)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->get_direction(gp);
}

صحيح gpio_direction_درون_داد(بنية gpio_pin *gp)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->جابه_جایی))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_درون_داد)
		أرجع SBI_ENOSYS;

	أرجع gp->chip->direction_درون_داد(gp);
}

صحيح gpio_direction_output(بنية gpio_pin *gp, صحيح مقدار)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->جابه_جایی))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->direction_output)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_فعال_کم)
		مقدار = مقدار == 0 ? 1 : 0;

	أرجع gp->chip->direction_output(gp, مقدار);
}

صحيح gpio_get(بنية gpio_pin *gp)
{
	صحيح مقدار;

	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->جابه_جایی))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->get)
		أرجع SBI_ENOSYS;

	مقدار = gp->chip->get(gp);

	إذا (gp->flags & GPIO_FLAG_فعال_کم)
		مقدار = مقدار == 0 ? 1 : 0;
	أرجع مقدار;
}

صحيح gpio_مجموعه(بنية gpio_pin *gp, صحيح مقدار)
{
	إذا (!gp || !gp->chip || (gp->chip->ngpio <= gp->جابه_جایی))
		أرجع SBI_EINVAL;
	إذا (!gp->chip->مجموعه)
		أرجع SBI_ENOSYS;

	إذا (gp->flags & GPIO_FLAG_فعال_کم)
		مقدار = مقدار == 0 ? 1 : 0;

	gp->chip->مجموعه(gp, مقدار);
	أرجع 0;
}
