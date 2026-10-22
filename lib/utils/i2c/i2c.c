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

ಸ್ಥಿರ SBI_LIST_ತಲೆ(i2c_adapters_list);

ರಚನೆ i2c_adapter *i2c_adapter_ಕಂಡುಹಿಡಿ(ಪೂರ್ಣಾಂಕ id)
{
	ರಚನೆ sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ರಚನೆ i2c_adapter *adap = ಗಮ್ಯ_i2c_adapter(pos);

		ಆದರೆ (adap->id == id)
			ಹಿಂತಿರುಗಿ adap;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ i2c_adapter_ಕೂಡಿಸು(ರಚನೆ i2c_adapter *ia)
{
	ಆದರೆ (!ia)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (i2c_adapter_ಕಂಡುಹಿಡಿ(ia->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	sbi_list_ಕೂಡಿಸು(&(ia->ಗಂಟು), &(i2c_adapters_list));

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ i2c_adapter_remove(ರಚನೆ i2c_adapter *ia)
{
	ಆದರೆ (!ia)
		ಹಿಂತಿರುಗಿ;

	sbi_list_del(&(ia->ಗಂಟು));
}

ಪೂರ್ಣಾಂಕ i2c_adapter_write(ರಚನೆ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ಆದರೆ (!ia)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!ia->ಲಿಖ)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ ia->ಲಿಖ(ia, addr, reg, ಬಫರ್, len);
}


ಪೂರ್ಣಾಂಕ i2c_adapter_read(ರಚನೆ i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *ಬಫರ್, ಪೂರ್ಣಾಂಕ len)
{
	ಆದರೆ (!ia)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (!ia->ಪಠ)
		ಹಿಂತಿರುಗಿ SBI_ENOSYS;

	ಹಿಂತಿರುಗಿ ia->ಪಠ(ia, addr, reg, ಬಫರ್, len);
}
