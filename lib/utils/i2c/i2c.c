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

#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/i2c/i2c.h>

స్థిర SBI_LIST_తల(i2c_adapters_list);

నిర్మాణం i2c_adapter *i2c_adapter_కనుగొను(పూర్ణాంకం id)
{
	నిర్మాణం sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		నిర్మాణం i2c_adapter *adap = గమ్యం_i2c_adapter(pos);

		ఒకవేళ (adap->id == id)
			తిరిగి adap;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం i2c_adapter_కూడు(నిర్మాణం i2c_adapter *ia)
{
	ఒకవేళ (!ia)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (i2c_adapter_కనుగొను(ia->id))
		తిరిగి SBI_EALREADY;

	sbi_list_కూడు(&(ia->కణుపు), &(i2c_adapters_list));

	తిరిగి 0;
}

శూన్య i2c_adapter_remove(నిర్మాణం i2c_adapter *ia)
{
	ఒకవేళ (!ia)
		తిరిగి;

	sbi_list_del(&(ia->కణుపు));
}

పూర్ణాంకం i2c_adapter_write(నిర్మాణం i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *బఫరు, పూర్ణాంకం len)
{
	ఒకవేళ (!ia)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!ia->లిఖ)
		తిరిగి SBI_ENOSYS;

	తిరిగి ia->లిఖ(ia, addr, reg, బఫరు, len);
}


పూర్ణాంకం i2c_adapter_read(నిర్మాణం i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *బఫరు, పూర్ణాంకం len)
{
	ఒకవేళ (!ia)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (!ia->పఠ)
		తిరిగి SBI_ENOSYS;

	తిరిగి ia->పఠ(ia, addr, reg, బఫరు, len);
}
