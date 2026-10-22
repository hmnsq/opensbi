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

sa_nracanaa gpio_chip *gpio_chip_find(unsigned පූර්ණ_සංඛ්‍යා id)
{
	sa_nracanaa sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		sa_nracanaa gpio_chip *chip = to_gpio_chip(pos);

		නම් (chip->id == id)
			ආපසු chip;
	}

	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා gpio_chip_add(sa_nracanaa gpio_chip *gc)
{
	නම් (!gc)
		ආපසු SBI_EINVAL;
	නම් (gpio_chip_find(gc->id))
		ආපසු SBI_EALREADY;

	sbi_list_add(&(gc->node), &(gpio_chip_list));

	ආපසු 0;
}

void gpio_chip_remove(sa_nracanaa gpio_chip *gc)
{
	නම් (!gc)
		ආපසු;

	sbi_list_del(&(gc->node));
}

පූර්ණ_සංඛ්‍යා gpio_get_direction(sa_nracanaa gpio_pin *gp)
{
	නම් (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ආපසු SBI_EINVAL;
	නම් (!gp->chip->get_direction)
		ආපසු SBI_ENOSYS;

	ආපසු gp->chip->get_direction(gp);
}

පූර්ණ_සංඛ්‍යා gpio_direction_input(sa_nracanaa gpio_pin *gp)
{
	නම් (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ආපසු SBI_EINVAL;
	නම් (!gp->chip->direction_input)
		ආපසු SBI_ENOSYS;

	ආපසු gp->chip->direction_input(gp);
}

පූර්ණ_සංඛ්‍යා gpio_direction_output(sa_nracanaa gpio_pin *gp, පූර්ණ_සංඛ්‍යා value)
{
	නම් (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ආපසු SBI_EINVAL;
	නම් (!gp->chip->direction_output)
		ආපසු SBI_ENOSYS;

	නම් (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	ආපසු gp->chip->direction_output(gp, value);
}

පූර්ණ_සංඛ්‍යා gpio_get(sa_nracanaa gpio_pin *gp)
{
	පූර්ණ_සංඛ්‍යා value;

	නම් (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ආපසු SBI_EINVAL;
	නම් (!gp->chip->get)
		ආපසු SBI_ENOSYS;

	value = gp->chip->get(gp);

	නම් (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;
	ආපසු value;
}

පූර්ණ_සංඛ්‍යා gpio_set(sa_nracanaa gpio_pin *gp, පූර්ණ_සංඛ්‍යා value)
{
	නම් (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ආපසු SBI_EINVAL;
	නම් (!gp->chip->set)
		ආපසු SBI_ENOSYS;

	නම් (gp->flags & GPIO_FLAG_ACTIVE_LOW)
		value = value == 0 ? 1 : 0;

	gp->chip->set(gp, value);
	ආපසු 0;
}
