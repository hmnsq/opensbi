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

ساکن SBI_LIST_سر(gpio_chip_list);

ساخت gpio_chip *gpio_chip_ڈھونڈو(غیرعلامتی صحیح id)
{
	ساخت sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ساخت gpio_chip *chip = مقصد_gpio_chip(pos);

		اگر (chip->id == id)
			واپس chip;
	}

	واپس تہی;
}

صحیح gpio_chip_جوڑو(ساخت gpio_chip *gc)
{
	اگر (!gc)
		واپس SBI_EINVAL;
	اگر (gpio_chip_ڈھونڈو(gc->id))
		واپس SBI_EALREADY;

	sbi_list_جوڑو(&(gc->گرہ), &(gpio_chip_list));

	واپس 0;
}

خالی gpio_chip_remove(ساخت gpio_chip *gc)
{
	اگر (!gc)
		واپس;

	sbi_list_del(&(gc->گرہ));
}

صحیح gpio_get_direction(ساخت gpio_pin *gp)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->انحراف))
		واپس SBI_EINVAL;
	اگر (!gp->chip->get_direction)
		واپس SBI_ENOSYS;

	واپس gp->chip->get_direction(gp);
}

صحیح gpio_direction_داخلہ(ساخت gpio_pin *gp)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->انحراف))
		واپس SBI_EINVAL;
	اگر (!gp->chip->direction_داخلہ)
		واپس SBI_ENOSYS;

	واپس gp->chip->direction_داخلہ(gp);
}

صحیح gpio_direction_output(ساخت gpio_pin *gp, صحیح قدر)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->انحراف))
		واپس SBI_EINVAL;
	اگر (!gp->chip->direction_output)
		واپس SBI_ENOSYS;

	اگر (gp->flags & GPIO_FLAG_فعال_کم)
		قدر = قدر == 0 ? 1 : 0;

	واپس gp->chip->direction_output(gp, قدر);
}

صحیح gpio_get(ساخت gpio_pin *gp)
{
	صحیح قدر;

	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->انحراف))
		واپس SBI_EINVAL;
	اگر (!gp->chip->get)
		واپس SBI_ENOSYS;

	قدر = gp->chip->get(gp);

	اگر (gp->flags & GPIO_FLAG_فعال_کم)
		قدر = قدر == 0 ? 1 : 0;
	واپس قدر;
}

صحیح gpio_سیٹ(ساخت gpio_pin *gp, صحیح قدر)
{
	اگر (!gp || !gp->chip || (gp->chip->ngpio <= gp->انحراف))
		واپس SBI_EINVAL;
	اگر (!gp->chip->سیٹ)
		واپس SBI_ENOSYS;

	اگر (gp->flags & GPIO_FLAG_فعال_کم)
		قدر = قدر == 0 ? 1 : 0;

	gp->chip->سیٹ(gp, قدر);
	واپس 0;
}
