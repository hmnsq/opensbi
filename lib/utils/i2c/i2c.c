/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 YADRO
 *
 * Authors:
 *   Nikita Shubin <n.shubin@yadro.com>
 *
 * derivate: lib/utils/gpio/gpio.c
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/i2c/i2c.h>

സ്ഥിതം SBI_LIST_HEAD(i2c_adapters_list);

ഘടന i2c_adapter *i2c_adapter_find(പൂർണ്ണസംഖ്യ id)
{
	ഘടന sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ഘടന i2c_adapter *adap = to_i2c_adapter(pos);

		എങ്കിൽ (adap->id == id)
			തിരികെ adap;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ i2c_adapter_add(ഘടന i2c_adapter *ia)
{
	എങ്കിൽ (!ia)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (i2c_adapter_find(ia->id))
		തിരികെ SBI_EALREADY;

	sbi_list_add(&(ia->കണ്ണി), &(i2c_adapters_list));

	തിരികെ 0;
}

ശൂന്യം i2c_adapter_remove(ഘടന i2c_adapter *ia)
{
	എങ്കിൽ (!ia)
		തിരികെ;

	sbi_list_del(&(ia->കണ്ണി));
}

പൂർണ്ണസംഖ്യ i2c_adapter_write(ഘടന i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	എങ്കിൽ (!ia)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!ia->ലിഖ)
		തിരികെ SBI_ENOSYS;

	തിരികെ ia->ലിഖ(ia, addr, reg, buffer, len);
}


പൂർണ്ണസംഖ്യ i2c_adapter_read(ഘടന i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, പൂർണ്ണസംഖ്യ len)
{
	എങ്കിൽ (!ia)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (!ia->പഠ)
		തിരികെ SBI_ENOSYS;

	തിരികെ ia->പഠ(ia, addr, reg, buffer, len);
}
