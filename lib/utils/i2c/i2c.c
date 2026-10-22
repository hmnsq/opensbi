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

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/i2c/i2c.h>

ସ୍ଥିର SBI_LIST_HEAD(i2c_adapters_list);

ସଂରଚନା i2c_adapter *i2c_adapter_find(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
	ସଂରଚନା sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		ସଂରଚନା i2c_adapter *adap = to_i2c_adapter(pos);

		ଯଦି (adap->id == id)
			ଫେରନ୍ତୁ adap;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା i2c_adapter_add(ସଂରଚନା i2c_adapter *ia)
{
	ଯଦି (!ia)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ଯଦି (i2c_adapter_find(ia->id))
		ଫେରନ୍ତୁ SBI_EALREADY;

	sbi_list_add(&(ia->ଗଣ୍ଠି), &(i2c_adapters_list));

	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ i2c_adapter_remove(ସଂରଚନା i2c_adapter *ia)
{
	ଯଦି (!ia)
		ଫେରନ୍ତୁ;

	sbi_list_del(&(ia->ଗଣ୍ଠି));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା i2c_adapter_write(ସଂରଚନା i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଯଦି (!ia)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!ia->ଲିଖ)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ ia->ଲିଖ(ia, addr, reg, buffer, len);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା i2c_adapter_read(ସଂରଚନା i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଯଦି (!ia)
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (!ia->ପଠ)
		ଫେରନ୍ତୁ SBI_ENOSYS;

	ଫେରନ୍ତୁ ia->ପଠ(ia, addr, reg, buffer, len);
}
