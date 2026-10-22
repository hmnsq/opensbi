/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/gpio/gpio.h>

ایستا SBI_LIST_HEAD(gpio_chip_list);

ساختار gpio_chip *gpio_chip_find(unsigned صحیح id)
{
	ساختار sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ساختار gpio_chip *chip = to_gpio_chip(pos);

		اگر (chip->id == id)
			بازگشت chip;
	}

	بازگشت پوچ;
}

صحیح gpio_chip_add(ساختار gpio_chip *gc)
{
	اگر (!gc)
		بازگشت SBI_EINVAL;
	اگر (gpio_chip_find(gc->id))
		بازگشت SBI_EALREADY;

	sbi_list_add(&(gc->گره), &(gpio_chip_list));

	بازگشت 0;
}

تهی gpio_chip_remove(ساختار gpio_chip *gc)
{
	اگر (!gc)
		بازگشت;

	sbi_list_del(&(gc->گره));
}

صحیح gpio_get_direction(ساختار gpio_pin *gp)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		بازگشت SBI_EINVAL;
	اگر (!gp->chip->get_direction)
		بازگشت SBI_ENOSYS;

	بازگشت gp->chip->get_direction(gp);
}

صحیح gpio_direction_درون_داد(ساختار gpio_pin *gp)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		بازگشت SBI_EINVAL;
	اگر (!gp->chip->direction_درون_داد)
		بازگشت SBI_ENOSYS;

	بازگشت gp->chip->direction_درون_داد(gp);
}

صحیح gpio_direction_output(ساختار gpio_pin *gp, صحیح مقدار)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		بازگشت SBI_EINVAL;
	اگر (!gp->chip->direction_output)
		بازگشت SBI_ENOSYS;

	اگر (gp->flags & GPIO_FLAG_ACTIVE_کم)
		مقدار = مقدار == 0 ? 1 : 0;

	بازگشت gp->chip->direction_output(gp, مقدار);
}

صحیح gpio_get(ساختار gpio_pin *gp)
{
	صحیح مقدار;

	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		بازگشت SBI_EINVAL;
	اگر (!gp->chip->get)
		بازگشت SBI_ENOSYS;

	مقدار = gp->chip->get(gp);

	اگر (gp->flags & GPIO_FLAG_ACTIVE_کم)
		مقدار = مقدار == 0 ? 1 : 0;
	بازگشت مقدار;
}

صحیح gpio_set(ساختار gpio_pin *gp, صحیح مقدار)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		بازگشت SBI_EINVAL;
	اگر (!gp->chip->set)
		بازگشت SBI_ENOSYS;

	اگر (gp->flags & GPIO_FLAG_ACTIVE_کم)
		مقدار = مقدار == 0 ? 1 : 0;

	gp->chip->set(gp, مقدار);
	بازگشت 0;
}
