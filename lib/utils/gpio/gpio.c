/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/gpio/gpio.h>

સ્થિર SBI_LIST_HEAD(gpio_chip_list);

સંરચના gpio_chip *gpio_chip_find(unsigned પૂર્ણાંક id)
{
	સંરચના sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		સંરચના gpio_chip *chip = to_gpio_chip(pos);

		જો (chip->id == id)
			પાછા chip;
	}

	પાછા ખાલી;
}

પૂર્ણાંક gpio_chip_add(સંરચના gpio_chip *gc)
{
	જો (!gc)
		પાછા SBI_EINVAL;
	જો (gpio_chip_find(gc->id))
		પાછા SBI_EALREADY;

	sbi_list_add(&(gc->ગાંઠ), &(gpio_chip_list));

	પાછા 0;
}

શૂન્ય gpio_chip_remove(સંરચના gpio_chip *gc)
{
	જો (!gc)
		પાછા;

	sbi_list_del(&(gc->ગાંઠ));
}

પૂર્ણાંક gpio_get_direction(સંરચના gpio_pin *gp)
{
	જો (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		પાછા SBI_EINVAL;
	જો (!gp->chip->get_direction)
		પાછા SBI_ENOSYS;

	પાછા gp->chip->get_direction(gp);
}

પૂર્ણાંક gpio_direction_આગત(સંરચના gpio_pin *gp)
{
	જો (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		પાછા SBI_EINVAL;
	જો (!gp->chip->direction_આગત)
		પાછા SBI_ENOSYS;

	પાછા gp->chip->direction_આગત(gp);
}

પૂર્ણાંક gpio_direction_output(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	જો (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		પાછા SBI_EINVAL;
	જો (!gp->chip->direction_output)
		પાછા SBI_ENOSYS;

	જો (gp->flags & GPIO_FLAG_ACTIVE_નીચલું)
		કિંમત = કિંમત == 0 ? 1 : 0;

	પાછા gp->chip->direction_output(gp, કિંમત);
}

પૂર્ણાંક gpio_get(સંરચના gpio_pin *gp)
{
	પૂર્ણાંક કિંમત;

	જો (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		પાછા SBI_EINVAL;
	જો (!gp->chip->get)
		પાછા SBI_ENOSYS;

	કિંમત = gp->chip->get(gp);

	જો (gp->flags & GPIO_FLAG_ACTIVE_નીચલું)
		કિંમત = કિંમત == 0 ? 1 : 0;
	પાછા કિંમત;
}

પૂર્ણાંક gpio_set(સંરચના gpio_pin *gp, પૂર્ણાંક કિંમત)
{
	જો (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		પાછા SBI_EINVAL;
	જો (!gp->chip->set)
		પાછા SBI_ENOSYS;

	જો (gp->flags & GPIO_FLAG_ACTIVE_નીચલું)
		કિંમત = કિંમત == 0 ? 1 : 0;

	gp->chip->set(gp, કિંમત);
	પાછા 0;
}
