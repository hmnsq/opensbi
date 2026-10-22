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

ಸ್ಥಿರ SBI_LIST_ತಲೆ(gpio_chip_list);

ರಚನೆ gpio_chip *gpio_chip_ಕಂಡುಹಿಡಿ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ id)
{
	ರಚನೆ sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ರಚನೆ gpio_chip *chip = ಗಮ್ಯ_gpio_chip(pos);

		ಆದರೆ (chip->id == id)
			ಹಿಂತಿರುಗಿ chip;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ gpio_chip_ಕೂಡಿಸು(ರಚನೆ gpio_chip *gc)
{
	ಆದರೆ (!gc)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (gpio_chip_ಕಂಡುಹಿಡಿ(gc->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	sbi_list_ಕೂಡಿಸು(&(gc->ಗಂಟು), &(gpio_chip_list));

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ gpio_chip_remove(ರಚನೆ gpio_chip *gc)
{
	ಆದರೆ (!gc)
		ಹಿಂತಿರುಗಿ;

	sbi_list_del(&(gc->ಗಂಟು));
}

ಪೂರ್ಣಾಂಕ gpio_get_direction(ರಚನೆ gpio_pin *gp)
{
	ಆದರೆ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!gp->chip->get_direction)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ gp->chip->get_direction(gp);
}

ಪೂರ್ಣಾಂಕ gpio_direction_ಒಳಹರಿವು(ರಚನೆ gpio_pin *gp)
{
	ಆದರೆ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!gp->chip->direction_ಒಳಹರಿವು)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ gp->chip->direction_ಒಳಹರಿವು(gp);
}

ಪೂರ್ಣಾಂಕ gpio_direction_output(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ಆದರೆ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!gp->chip->direction_output)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಆದರೆ (gp->flags & GPIO_FLAG_ಸಕ್ರಿಯ_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;

	ಹಿಂತಿರುಗಿ gp->chip->direction_output(gp, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ gpio_get(ರಚನೆ gpio_pin *gp)
{
	ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;

	ಆದರೆ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!gp->chip->get)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಮೌಲ್ಯ = gp->chip->get(gp);

	ಆದರೆ (gp->flags & GPIO_FLAG_ಸಕ್ರಿಯ_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;
	ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಂಕ gpio_ಗಣ(ರಚನೆ gpio_pin *gp, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	ಆದರೆ (!gp || !gp->chip || (gp->chip->ngpio <= gp->ಸ್ಥಾನಾಂತರ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!gp->chip->ಗಣ)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಆದರೆ (gp->flags & GPIO_FLAG_ಸಕ್ರಿಯ_ಕೆಳಗಿನ)
		ಮೌಲ್ಯ = ಮೌಲ್ಯ == 0 ? 1 : 0;

	gp->chip->ಗಣ(gp, ಮೌಲ್ಯ);
	ಹಿಂತಿರುಗಿ 0;
}
