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

संरचना gpio_chip *gpio_chip_find(अचिन्हित पूर्णांक id)
{
	संरचना sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		संरचना gpio_chip *chip = to_gpio_chip(pos);

		जर (chip->id == id)
			परत chip;
	}

	परत रिक्त;
}

पूर्णांक gpio_chip_add(संरचना gpio_chip *gc)
{
	जर (!gc)
		परत SBI_EINVAL;
	जर (gpio_chip_find(gc->id))
		परत SBI_EALREADY;

	sbi_list_add(&(gc->गाठ), &(gpio_chip_list));

	परत 0;
}

शून्य gpio_chip_remove(संरचना gpio_chip *gc)
{
	जर (!gc)
		परत;

	sbi_list_del(&(gc->गाठ));
}

पूर्णांक gpio_get_direction(संरचना gpio_pin *gp)
{
	जर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		परत SBI_EINVAL;
	जर (!gp->chip->get_direction)
		परत SBI_ENOSYS;

	परत gp->chip->get_direction(gp);
}

पूर्णांक gpio_direction_आदान(संरचना gpio_pin *gp)
{
	जर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		परत SBI_EINVAL;
	जर (!gp->chip->direction_आदान)
		परत SBI_ENOSYS;

	परत gp->chip->direction_आदान(gp);
}

पूर्णांक gpio_direction_output(संरचना gpio_pin *gp, पूर्णांक मूल्य)
{
	जर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		परत SBI_EINVAL;
	जर (!gp->chip->direction_output)
		परत SBI_ENOSYS;

	जर (gp->flags & GPIO_FLAG_ACTIVE_खालचा)
		मूल्य = मूल्य == 0 ? 1 : 0;

	परत gp->chip->direction_output(gp, मूल्य);
}

पूर्णांक gpio_get(संरचना gpio_pin *gp)
{
	पूर्णांक मूल्य;

	जर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		परत SBI_EINVAL;
	जर (!gp->chip->get)
		परत SBI_ENOSYS;

	मूल्य = gp->chip->get(gp);

	जर (gp->flags & GPIO_FLAG_ACTIVE_खालचा)
		मूल्य = मूल्य == 0 ? 1 : 0;
	परत मूल्य;
}

पूर्णांक gpio_set(संरचना gpio_pin *gp, पूर्णांक मूल्य)
{
	जर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		परत SBI_EINVAL;
	जर (!gp->chip->set)
		परत SBI_ENOSYS;

	जर (gp->flags & GPIO_FLAG_ACTIVE_खालचा)
		मूल्य = मूल्य == 0 ? 1 : 0;

	gp->chip->set(gp, मूल्य);
	परत 0;
}
