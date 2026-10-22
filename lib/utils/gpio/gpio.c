/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/gpio/gpio.h>

സ്ഥിതം SBI_LIST_HEAD(gpio_chip_list);

ഘടന gpio_chip *gpio_chip_find(unsigned പൂർണ്ണസംഖ്യ id)
{
	ഘടന sbi_dlist *pos;

	sbi_list_for_each(pos, &(gpio_chip_list)) {
		ഘടന gpio_chip *chip = to_gpio_chip(pos);

		എങ്കിൽ (chip->id == id)
			തിരികെ chip;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ gpio_chip_add(ഘടന gpio_chip *gc)
{
	എങ്കിൽ (!gc)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (gpio_chip_find(gc->id))
		തിരികെ SBI_EALREADY;

	sbi_list_add(&(gc->കണ്ണി), &(gpio_chip_list));

	തിരികെ 0;
}

ശൂന്യം gpio_chip_remove(ഘടന gpio_chip *gc)
{
	എങ്കിൽ (!gc)
		തിരികെ;

	sbi_list_del(&(gc->കണ്ണി));
}

പൂർണ്ണസംഖ്യ gpio_get_direction(ഘടന gpio_pin *gp)
{
	എങ്കിൽ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!gp->chip->get_direction)
		തിരികെ SBI_ENOSYS;

	തിരികെ gp->chip->get_direction(gp);
}

പൂർണ്ണസംഖ്യ gpio_direction_നിവേശം(ഘടന gpio_pin *gp)
{
	എങ്കിൽ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!gp->chip->direction_നിവേശം)
		തിരികെ SBI_ENOSYS;

	തിരികെ gp->chip->direction_നിവേശം(gp);
}

പൂർണ്ണസംഖ്യ gpio_direction_output(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	എങ്കിൽ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!gp->chip->direction_output)
		തിരികെ SBI_ENOSYS;

	എങ്കിൽ (gp->flags & GPIO_FLAG_ACTIVE_താഴെയുള്ള)
		മൂല്യം = മൂല്യം == 0 ? 1 : 0;

	തിരികെ gp->chip->direction_output(gp, മൂല്യം);
}

പൂർണ്ണസംഖ്യ gpio_get(ഘടന gpio_pin *gp)
{
	പൂർണ്ണസംഖ്യ മൂല്യം;

	എങ്കിൽ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!gp->chip->get)
		തിരികെ SBI_ENOSYS;

	മൂല്യം = gp->chip->get(gp);

	എങ്കിൽ (gp->flags & GPIO_FLAG_ACTIVE_താഴെയുള്ള)
		മൂല്യം = മൂല്യം == 0 ? 1 : 0;
	തിരികെ മൂല്യം;
}

പൂർണ്ണസംഖ്യ gpio_set(ഘടന gpio_pin *gp, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	എങ്കിൽ (!gp || !gp->chip || (gp->chip->ngpio <= gp->offset))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!gp->chip->set)
		തിരികെ SBI_ENOSYS;

	എങ്കിൽ (gp->flags & GPIO_FLAG_ACTIVE_താഴെയുള്ള)
		മൂല്യം = മൂല്യം == 0 ? 1 : 0;

	gp->chip->set(gp, മൂല്യം);
	തിരികെ 0;
}
