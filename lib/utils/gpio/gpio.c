/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/gpio/gpio.h>

अटल SBI_LIST_शीर्ष(gpio_chip_list);

काष्ठा gpio_chip *gpio_chip_find(अचिन्हित पूर्णांक id)
{
	काष्ठा sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		काष्ठा gpio_chip *chip = to_gpio_chip(pos);

		अगर (chip->id == id)
			वापस chip;
	}

	वापस शून्य;
}

पूर्णांक gpio_chip_add(काष्ठा gpio_chip *gc)
{
	अगर (!gc)
		वापस SBI_EINVAL;
	अगर (gpio_chip_find(gc->id))
		वापस SBI_EALREADY;

	sbi_list_add(&(gc->गाँठ), &(gpio_chip_list));

	वापस 0;
}

व्योम gpio_chip_remove(काष्ठा gpio_chip *gc)
{
	अगर (!gc)
		वापस;

	sbi_list_del(&(gc->गाँठ));
}

पूर्णांक gpio_get_direction(काष्ठा gpio_pin *gp)
{
	अगर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		वापस SBI_EINVAL;
	अगर (!gp->chip->get_direction)
		वापस SBI_ENOSYS;

	वापस gp->chip->get_direction(gp);
}

पूर्णांक gpio_direction_निवेश(काष्ठा gpio_pin *gp)
{
	अगर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		वापस SBI_EINVAL;
	अगर (!gp->chip->direction_निवेश)
		वापस SBI_ENOSYS;

	वापस gp->chip->direction_निवेश(gp);
}

पूर्णांक gpio_direction_output(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	अगर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		वापस SBI_EINVAL;
	अगर (!gp->chip->direction_output)
		वापस SBI_ENOSYS;

	अगर (gp->flags & GPIO_FLAG_ACTIVE_निचला)
		मूल्य = मूल्य == 0 ? 1 : 0;

	वापस gp->chip->direction_output(gp, मूल्य);
}

पूर्णांक gpio_get(काष्ठा gpio_pin *gp)
{
	पूर्णांक मूल्य;

	अगर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		वापस SBI_EINVAL;
	अगर (!gp->chip->get)
		वापस SBI_ENOSYS;

	मूल्य = gp->chip->get(gp);

	अगर (gp->flags & GPIO_FLAG_ACTIVE_निचला)
		मूल्य = मूल्य == 0 ? 1 : 0;
	वापस मूल्य;
}

पूर्णांक gpio_set(काष्ठा gpio_pin *gp, पूर्णांक मूल्य)
{
	अगर (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		वापस SBI_EINVAL;
	अगर (!gp->chip->set)
		वापस SBI_ENOSYS;

	अगर (gp->flags & GPIO_FLAG_ACTIVE_निचला)
		मूल्य = मूल्य == 0 ? 1 : 0;

	gp->chip->set(gp, मूल्य);
	वापस 0;
}
