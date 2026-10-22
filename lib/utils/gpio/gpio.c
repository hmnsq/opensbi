/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/gpio/gpio.h>

ಸ್ಥಿರ SBI_LIST_HEAD(gpio_chip_list);

ಸಂರಚನಾ gpio_chip *gpio_chip_find(unsigned ಪೂರ್ಣಾಙ್ಕ id)
{
	ಸಂರಚನಾ sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ಸಂರಚನಾ gpio_chip *chip = to_gpio_chip(pos);

		ಯದಿ (chip->id == id)
			ಪ್ರತ್ಯಾಗಮ chip;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ gpio_chip_add(ಸಂರಚನಾ gpio_chip *gc)
{
	ಯದಿ (!gc)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (gpio_chip_find(gc->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	sbi_list_add(&(gc->ಗಂಟು), &(gpio_chip_list));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ gpio_chip_remove(ಸಂರಚನಾ gpio_chip *gc)
{
	ಯದಿ (!gc)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_del(&(gc->ಗಂಟು));
}

ಪೂರ್ಣಾಙ್ಕ gpio_get_direction(ಸಂರಚನಾ gpio_pin *gp)
{
	ಯದಿ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!gp->chip->get_direction)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ gp->chip->get_direction(gp);
}

ಪೂರ್ಣಾಙ್ಕ gpio_direction_ಒಳಹರಿವು(ಸಂರಚನಾ gpio_pin *gp)
{
	ಯದಿ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!gp->chip->direction_ಒಳಹರಿವು)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ gp->chip->direction_ಒಳಹರಿವು(gp);
}

ಪೂರ್ಣಾಙ್ಕ gpio_direction_output(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	ಯದಿ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!gp->chip->direction_output)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಯದಿ (gp->flags & GPIO_FLAG_ACTIVE_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;

	ಪ್ರತ್ಯಾಗಮ gp->chip->direction_output(gp, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ gpio_get(ಸಂರಚನಾ gpio_pin *gp)
{
	ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;

	ಯದಿ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!gp->chip->get)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಮೌಲ್ಯ = gp->chip->get(gp);

	ಯದಿ (gp->flags & GPIO_FLAG_ACTIVE_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;
	ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ gpio_set(ಸಂರಚನಾ gpio_pin *gp, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	ಯದಿ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!gp->chip->set)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಯದಿ (gp->flags & GPIO_FLAG_ACTIVE_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;

	gp->chip->set(gp, ಮೌಲ್ಯ);
	ಪ್ರತ್ಯಾಗಮ 0;
}
