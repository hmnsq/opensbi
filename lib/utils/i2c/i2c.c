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

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/i2c/i2c.h>

ಸ್ಥಿರ SBI_LIST_HEAD(i2c_adapters_list);

ಸಂರಚನಾ i2c_adapter *i2c_adapter_find(ಪೂರ್ಣಾಙ್ಕ id)
{
	ಸಂರಚನಾ sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ಸಂರಚನಾ i2c_adapter *adap = to_i2c_adapter(pos);

		ಯದಿ (adap->id == id)
			ಪ್ರತ್ಯಾಗಮ adap;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ i2c_adapter_add(ಸಂರಚನಾ i2c_adapter *ia)
{
	ಯದಿ (!ia)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (i2c_adapter_find(ia->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	sbi_list_add(&(ia->ಗಂಟು), &(i2c_adapters_list));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ i2c_adapter_remove(ಸಂರಚನಾ i2c_adapter *ia)
{
	ಯದಿ (!ia)
		ಪ್ರತ್ಯಾಗಮ;

	sbi_list_del(&(ia->ಗಂಟು));
}

ಪೂರ್ಣಾಙ್ಕ i2c_adapter_write(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಯದಿ (!ia)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!ia->ಲಿಖ)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ ia->ಲಿಖ(ia, addr, reg, buffer, len);
}


ಪೂರ್ಣಾಙ್ಕ i2c_adapter_read(ಸಂರಚನಾ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಯದಿ (!ia)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (!ia->ಪಠ)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSYS;

	ಪ್ರತ್ಯಾಗಮ ia->ಪಠ(ia, addr, reg, buffer, len);
}
