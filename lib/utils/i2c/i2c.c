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

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/i2c/i2c.h>

સ્થિર SBI_LIST_HEAD(i2c_adapters_list);

સંરચના i2c_adapter *i2c_adapter_find(પૂર્ણાંક id)
{
	સંરચના sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		સંરચના i2c_adapter *adap = to_i2c_adapter(pos);

		જો (adap->id == id)
			પાછા adap;
	}

	પાછા ખાલી;
}

પૂર્ણાંક i2c_adapter_add(સંરચના i2c_adapter *ia)
{
	જો (!ia)
		પાછા SBI_EINVAL;

	જો (i2c_adapter_find(ia->id))
		પાછા SBI_EALREADY;

	sbi_list_add(&(ia->ગાંઠ), &(i2c_adapters_list));

	પાછા 0;
}

શૂન્ય i2c_adapter_remove(સંરચના i2c_adapter *ia)
{
	જો (!ia)
		પાછા;

	sbi_list_del(&(ia->ગાંઠ));
}

પૂર્ણાંક i2c_adapter_write(સંરચના i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, પૂર્ણાંક len)
{
	જો (!ia)
		પાછા SBI_EINVAL;
	જો (!ia->લિખ)
		પાછા SBI_ENOSYS;

	પાછા ia->લિખ(ia, addr, reg, buffer, len);
}


પૂર્ણાંક i2c_adapter_read(સંરચના i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *buffer, પૂર્ણાંક len)
{
	જો (!ia)
		પાછા SBI_EINVAL;
	જો (!ia->પઠ)
		પાછા SBI_ENOSYS;

	પાછા ia->પઠ(ia, addr, reg, buffer, len);
}
