/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/gpio/gpio.h>

ସ୍ଥିର SBI_LIST_HEAD(gpio_chip_list);

ସଂରଚନା gpio_chip *gpio_chip_find(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
	ସଂରଚନା sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ସଂରଚନା gpio_chip *chip = to_gpio_chip(pos);

		ଯଦି (chip->id == id)
			ଫେରନ୍ତୁ chip;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_chip_add(ସଂରଚନା gpio_chip *gc)
{
	ଯଦି (!gc)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (gpio_chip_find(gc->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	sbi_list_add(&(gc->ଗଣ୍ଠି), &(gpio_chip_list));

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ gpio_chip_remove(ସଂରଚନା gpio_chip *gc)
{
	ଯଦି (!gc)
		ଫେରନ୍ତୁ;

	sbi_list_del(&(gc->ଗଣ୍ଠି));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_get_direction(ସଂରଚନା gpio_pin *gp)
{
	ଯଦି (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!gp->chip->get_direction)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ gp->chip->get_direction(gp);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_direction_ନିବେଶ(ସଂରଚନା gpio_pin *gp)
{
	ଯଦି (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!gp->chip->direction_ନିବେଶ)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ gp->chip->direction_ନିବେଶ(gp);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_direction_output(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	ଯଦି (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!gp->chip->direction_output)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଯଦି (gp->flags & GPIO_FLAG_ACTIVE_ତଳ)
		ମୂଲ୍ୟ = ମୂଲ୍ୟ == 0 ? 1 : 0;

	ଫେରନ୍ତୁ gp->chip->direction_output(gp, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_get(ସଂରଚନା gpio_pin *gp)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;

	ଯଦି (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!gp->chip->get)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ମୂଲ୍ୟ = gp->chip->get(gp);

	ଯଦି (gp->flags & GPIO_FLAG_ACTIVE_ତଳ)
		ମୂଲ୍ୟ = ମୂଲ୍ୟ == 0 ? 1 : 0;
	ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpio_set(ସଂରଚନା gpio_pin *gp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	ଯଦି (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!gp->chip->set)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଯଦି (gp->flags & GPIO_FLAG_ACTIVE_ତଳ)
		ମୂଲ୍ୟ = ମୂଲ୍ୟ == 0 ? 1 : 0;

	gp->chip->set(gp, ମୂଲ୍ୟ);
	ଫେରନ୍ତୁ 0;
}
