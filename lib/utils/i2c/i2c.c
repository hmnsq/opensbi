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

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/i2c/i2c.h>

ਸਥਿਰ SBI_LIST_ਸਿਰ(i2c_adapters_list);

ਬਣਤਰ i2c_adapter *i2c_adapter_ਲੱਭੋ(ਪੂਰਨ id)
{
	ਬਣਤਰ sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ਬਣਤਰ i2c_adapter *adap = ਮੰਜ਼ਿਲ_i2c_adapter(pos);

		ਜੇ (adap->id == id)
			ਵਾਪਸ adap;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ i2c_adapter_ਜੋੜੋ(ਬਣਤਰ i2c_adapter *ia)
{
	ਜੇ (!ia)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (i2c_adapter_ਲੱਭੋ(ia->id))
		ਵਾਪਸ SBI_EALREADY;

	sbi_list_ਜੋੜੋ(&(ia->ਗੰਢ), &(i2c_adapters_list));

	ਵਾਪਸ 0;
}

ਖਾਲੀ i2c_adapter_remove(ਬਣਤਰ i2c_adapter *ia)
{
	ਜੇ (!ia)
		ਵਾਪਸ;

	sbi_list_del(&(ia->ਗੰਢ));
}

ਪੂਰਨ i2c_adapter_write(ਬਣਤਰ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਜੇ (!ia)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!ia->ਲਿਖ)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ ia->ਲਿਖ(ia, addr, reg, ਬਫਰ, len);
}


ਪੂਰਨ i2c_adapter_read(ਬਣਤਰ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *ਬਫਰ, ਪੂਰਨ len)
{
	ਜੇ (!ia)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (!ia->ਪਠ)
		ਵਾਪਸ SBI_ENOSYS;

	ਵਾਪਸ ia->ਪਠ(ia, addr, reg, ਬਫਰ, len);
}
