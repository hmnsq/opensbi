/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/gpio/gpio.h>

స్థిర SBI_LIST_తల(gpio_chip_list);

నిర్మాణం gpio_chip *gpio_chip_కనుగొను(గుర్తులేని పూర్ణాంకం id)
{
	నిర్మాణం sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		నిర్మాణం gpio_chip *chip = గమ్యం_gpio_chip(pos);

		ఒకవేళ (chip->id == id)
			తిరిగి chip;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం gpio_chip_కూడు(నిర్మాణం gpio_chip *gc)
{
	ఒకవేళ (!gc)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (gpio_chip_కనుగొను(gc->id))
		తిరిగి SBI_EALREADY;

	sbi_list_కూడు(&(gc->కణుపు), &(gpio_chip_list));

	తిరిగి 0;
}

శూన్య gpio_chip_remove(నిర్మాణం gpio_chip *gc)
{
	ఒకవేళ (!gc)
		తిరిగి;

	sbi_list_del(&(gc->కణుపు));
}

పూర్ణాంకం gpio_get_direction(నిర్మాణం gpio_pin *gp)
{
	ఒకవేళ (!gp || !gp->chip || (gp->chip->ngpio <= gp->స్థానభ్రంశం))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!gp->chip->get_direction)
		తిరిగి SBI_ENOSYS;

	తిరిగి gp->chip->get_direction(gp);
}

పూర్ణాంకం gpio_direction_ఆదానం(నిర్మాణం gpio_pin *gp)
{
	ఒకవేళ (!gp || !gp->chip || (gp->chip->ngpio <= gp->స్థానభ్రంశం))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!gp->chip->direction_ఆదానం)
		తిరిగి SBI_ENOSYS;

	తిరిగి gp->chip->direction_ఆదానం(gp);
}

పూర్ణాంకం gpio_direction_output(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	ఒకవేళ (!gp || !gp->chip || (gp->chip->ngpio <= gp->స్థానభ్రంశం))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!gp->chip->direction_output)
		తిరిగి SBI_ENOSYS;

	ఒకవేళ (gp->flags & GPIO_FLAG_చురుకైన_దిగువ)
		విలువ = విలువ == 0 ? 1 : 0;

	తిరిగి gp->chip->direction_output(gp, విలువ);
}

పూర్ణాంకం gpio_get(నిర్మాణం gpio_pin *gp)
{
	పూర్ణాంకం విలువ;

	ఒకవేళ (!gp || !gp->chip || (gp->chip->ngpio <= gp->స్థానభ్రంశం))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!gp->chip->get)
		తిరిగి SBI_ENOSYS;

	విలువ = gp->chip->get(gp);

	ఒకవేళ (gp->flags & GPIO_FLAG_చురుకైన_దిగువ)
		విలువ = విలువ == 0 ? 1 : 0;
	తిరిగి విలువ;
}

పూర్ణాంకం gpio_సమితి(నిర్మాణం gpio_pin *gp, పూర్ణాంకం విలువ)
{
	ఒకవేళ (!gp || !gp->chip || (gp->chip->ngpio <= gp->స్థానభ్రంశం))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!gp->chip->సమితి)
		తిరిగి SBI_ENOSYS;

	ఒకవేళ (gp->flags & GPIO_FLAG_చురుకైన_దిగువ)
		విలువ = విలువ == 0 ? 1 : 0;

	gp->chip->సమితి(gp, విలువ);
	తిరిగి 0;
}
