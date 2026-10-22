/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/gpio/gpio.h>

நிலையான SBI_LIST_HEAD(gpio_chip_list);

கட்டமைப்பு gpio_chip *gpio_chip_find(unsigned முழு id)
{
	கட்டமைப்பு sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		கட்டமைப்பு gpio_chip *chip = to_gpio_chip(pos);

		எனில் (chip->id == id)
			திரும்பு chip;
	}

	திரும்பு வெற்று;
}

முழு gpio_chip_add(கட்டமைப்பு gpio_chip *gc)
{
	எனில் (!gc)
		திரும்பு SBI_EINVAL;
	எனில் (gpio_chip_find(gc->id))
		திரும்பு SBI_EALREADY;

	sbi_list_add(&(gc->முனை), &(gpio_chip_list));

	திரும்பு 0;
}

வெறுமை gpio_chip_remove(கட்டமைப்பு gpio_chip *gc)
{
	எனில் (!gc)
		திரும்பு;

	sbi_list_del(&(gc->முனை));
}

முழு gpio_get_direction(கட்டமைப்பு gpio_pin *gp)
{
	எனில் (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		திரும்பு SBI_EINVAL;
	எனில் (!gp->chip->get_direction)
		திரும்பு SBI_ENOSYS;

	திரும்பு gp->chip->get_direction(gp);
}

முழு gpio_direction_உள்ளீடு(கட்டமைப்பு gpio_pin *gp)
{
	எனில் (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		திரும்பு SBI_EINVAL;
	எனில் (!gp->chip->direction_உள்ளீடு)
		திரும்பு SBI_ENOSYS;

	திரும்பு gp->chip->direction_உள்ளீடு(gp);
}

முழு gpio_direction_output(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	எனில் (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		திரும்பு SBI_EINVAL;
	எனில் (!gp->chip->direction_output)
		திரும்பு SBI_ENOSYS;

	எனில் (gp->flags & GPIO_FLAG_ACTIVE_கீழ்)
		மதிப்பு = மதிப்பு == 0 ? 1 : 0;

	திரும்பு gp->chip->direction_output(gp, மதிப்பு);
}

முழு gpio_get(கட்டமைப்பு gpio_pin *gp)
{
	முழு மதிப்பு;

	எனில் (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		திரும்பு SBI_EINVAL;
	எனில் (!gp->chip->get)
		திரும்பு SBI_ENOSYS;

	மதிப்பு = gp->chip->get(gp);

	எனில் (gp->flags & GPIO_FLAG_ACTIVE_கீழ்)
		மதிப்பு = மதிப்பு == 0 ? 1 : 0;
	திரும்பு மதிப்பு;
}

முழு gpio_set(கட்டமைப்பு gpio_pin *gp, முழு மதிப்பு)
{
	எனில் (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		திரும்பு SBI_EINVAL;
	எனில் (!gp->chip->set)
		திரும்பு SBI_ENOSYS;

	எனில் (gp->flags & GPIO_FLAG_ACTIVE_கீழ்)
		மதிப்பு = மதிப்பு == 0 ? 1 : 0;

	gp->chip->set(gp, மதிப்பு);
	திரும்பு 0;
}
