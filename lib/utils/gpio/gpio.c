/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/gpio/gpio.h>

स्थिर SBI_LIST_HEAD(gpio_chip_list);

संरचना gpio_chip *gpio_chip_find(unsigned पूर्णाङ्क id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		संरचना gpio_chip *chip = to_gpio_chip(pos);

		जइ (chip->id == id)
			प्रत्यागम chip;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क gpio_chip_add(संरचना gpio_chip *gc)
{
	जइ (!gc)
		प्रत्यागम SBI_EINVAL;
	जइ (gpio_chip_find(gc->id))
		प्रत्यागम SBI_EALREADY;

	sbi_list_add(&(gc->ग्रन्थिः), &(gpio_chip_list));

	प्रत्यागम 0;
}

शून्य gpio_chip_remove(संरचना gpio_chip *gc)
{
	जइ (!gc)
		प्रत्यागम;

	sbi_list_del(&(gc->ग्रन्थिः));
}

पूर्णाङ्क gpio_get_direction(संरचना gpio_pin *gp)
{
	जइ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		प्रत्यागम SBI_EINVAL;
	जइ (!gp->chip->get_direction)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम gp->chip->get_direction(gp);
}

पूर्णाङ्क gpio_direction_निवेशः(संरचना gpio_pin *gp)
{
	जइ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		प्रत्यागम SBI_EINVAL;
	जइ (!gp->chip->direction_निवेशः)
		प्रत्यागम SBI_ENOSYS;

	प्रत्यागम gp->chip->direction_निवेशः(gp);
}

पूर्णाङ्क gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	जइ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		प्रत्यागम SBI_EINVAL;
	जइ (!gp->chip->direction_output)
		प्रत्यागम SBI_ENOSYS;

	जइ (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;

	प्रत्यागम gp->chip->direction_output(gp, मानम्);
}

पूर्णाङ्क gpio_get(संरचना gpio_pin *gp)
{
	पूर्णाङ्क मानम्;

	जइ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		प्रत्यागम SBI_EINVAL;
	जइ (!gp->chip->get)
		प्रत्यागम SBI_ENOSYS;

	मानम् = gp->chip->get(gp);

	जइ (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;
	प्रत्यागम मानम्;
}

पूर्णाङ्क gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	जइ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		प्रत्यागम SBI_EINVAL;
	जइ (!gp->chip->set)
		प्रत्यागम SBI_ENOSYS;

	जइ (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;

	gp->chip->set(gp, मानम्);
	प्रत्यागम 0;
}
