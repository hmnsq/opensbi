/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/gpio/gpio.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(gpio_chip_list);

ਬਣਤਰ gpio_chip *gpio_chip_ਲੱਭੋ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ id)
{
	ਬਣਤਰ sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ਬਣਤਰ gpio_chip *chip = ਮੰਜ਼ਿਲ_gpio_chip(pos);

		ਜੇ (chip->id == id)
			ਵਾਪਸ chip;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ gpio_chip_ਜੋੜੋ(ਬਣਤਰ gpio_chip *gc)
{
	ਜੇ (!gc)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (gpio_chip_ਲੱਭੋ(gc->id))
		ਵਾਪਸ SBI_EALREADY;

	sbi_list_ਜੋੜੋ(&(gc->ਗੰਢ), &(gpio_chip_list));

	ਵਾਪਸ 0;
}

ਖਾਲੀ gpio_chip_remove(ਬਣਤਰ gpio_chip *gc)
{
	ਜੇ (!gc)
		ਵਾਪਸ;

	sbi_list_del(&(gc->ਗੰਢ));
}

ਪੂਰਨ gpio_get_direction(ਬਣਤਰ gpio_pin *gp)
{
	ਜੇ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ਵਿੱਥ))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!gp->chip->get_direction)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ gp->chip->get_direction(gp);
}

ਪੂਰਨ gpio_direction_ਨਿਵੇਸ਼(ਬਣਤਰ gpio_pin *gp)
{
	ਜੇ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ਵਿੱਥ))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!gp->chip->direction_ਨਿਵੇਸ਼)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ gp->chip->direction_ਨਿਵੇਸ਼(gp);
}

ਪੂਰਨ gpio_direction_output(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਜੇ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ਵਿੱਥ))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!gp->chip->direction_output)
		ਵਾਪਸ SBI_ENOSYS;

	ਜੇ (gp->flags & GPIO_FLAG_ਸਰਗਰਮ_ਨੀਵਾਂ)
		ਮੁੱਲ = ਮੁੱਲ == 0 ? 1 : 0;

	ਵਾਪਸ gp->chip->direction_output(gp, ਮੁੱਲ);
}

ਪੂਰਨ gpio_get(ਬਣਤਰ gpio_pin *gp)
{
	ਪੂਰਨ ਮੁੱਲ;

	ਜੇ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ਵਿੱਥ))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!gp->chip->get)
		ਵਾਪਸ SBI_ENOSYS;

	ਮੁੱਲ = gp->chip->get(gp);

	ਜੇ (gp->flags & GPIO_FLAG_ਸਰਗਰਮ_ਨੀਵਾਂ)
		ਮੁੱਲ = ਮੁੱਲ == 0 ? 1 : 0;
	ਵਾਪਸ ਮੁੱਲ;
}

ਪੂਰਨ gpio_ਸੈੱਟ(ਬਣਤਰ gpio_pin *gp, ਪੂਰਨ ਮੁੱਲ)
{
	ਜੇ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ਵਿੱਥ))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!gp->chip->ਸੈੱਟ)
		ਵਾਪਸ SBI_ENOSYS;

	ਜੇ (gp->flags & GPIO_FLAG_ਸਰਗਰਮ_ਨੀਵਾਂ)
		ਮੁੱਲ = ਮੁੱਲ == 0 ? 1 : 0;

	gp->chip->ਸੈੱਟ(gp, ਮੁੱਲ);
	ਵਾਪਸ 0;
}
