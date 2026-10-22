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

		सचे (chip->id == id)
			पटिनिवत्त chip;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क gpio_chip_add(संरचना gpio_chip *gc)
{
	सचे (!gc)
		पटिनिवत्त SBI_EINVAL;
	सचे (gpio_chip_find(gc->id))
		पटिनिवत्त SBI_EALREADY;

	sbi_list_add(&(gc->ग्रन्थिः), &(gpio_chip_list));

	पटिनिवत्त 0;
}

शून्य gpio_chip_remove(संरचना gpio_chip *gc)
{
	सचे (!gc)
		पटिनिवत्त;

	sbi_list_del(&(gc->ग्रन्थिः));
}

पूर्णाङ्क gpio_get_direction(संरचना gpio_pin *gp)
{
	सचे (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		पटिनिवत्त SBI_EINVAL;
	सचे (!gp->chip->get_direction)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त gp->chip->get_direction(gp);
}

पूर्णाङ्क gpio_direction_निवेशः(संरचना gpio_pin *gp)
{
	सचे (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		पटिनिवत्त SBI_EINVAL;
	सचे (!gp->chip->direction_निवेशः)
		पटिनिवत्त SBI_ENOSYS;

	पटिनिवत्त gp->chip->direction_निवेशः(gp);
}

पूर्णाङ्क gpio_direction_output(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	सचे (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		पटिनिवत्त SBI_EINVAL;
	सचे (!gp->chip->direction_output)
		पटिनिवत्त SBI_ENOSYS;

	सचे (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;

	पटिनिवत्त gp->chip->direction_output(gp, मानम्);
}

पूर्णाङ्क gpio_get(संरचना gpio_pin *gp)
{
	पूर्णाङ्क मानम्;

	सचे (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		पटिनिवत्त SBI_EINVAL;
	सचे (!gp->chip->get)
		पटिनिवत्त SBI_ENOSYS;

	मानम् = gp->chip->get(gp);

	सचे (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;
	पटिनिवत्त मानम्;
}

पूर्णाङ्क gpio_set(संरचना gpio_pin *gp, पूर्णाङ्क मानम्)
{
	सचे (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		पटिनिवत्त SBI_EINVAL;
	सचे (!gp->chip->set)
		पटिनिवत्त SBI_ENOSYS;

	सचे (gp->flags & GPIO_FLAG_ACTIVE_नीचः)
		मानम् = मानम् == 0 ? 1 : 0;

	gp->chip->set(gp, मानम्);
	पटिनिवत्त 0;
}
