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

#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/i2c/i2c.h>

अटल SBI_LIST_शीर्ष(i2c_adapters_list);

काष्ठा i2c_adapter *i2c_adapter_find(पूर्णांक id)
{
	काष्ठा sbi_dlist *pos;

	sbi_list_for_each(pos, &(i2c_adapters_list)) {
		काष्ठा i2c_adapter *adap = to_i2c_adapter(pos);

		अगर (adap->id == id)
			वापस adap;
	}

	वापस शून्य;
}

पूर्णांक i2c_adapter_add(काष्ठा i2c_adapter *ia)
{
	अगर (!ia)
		वापस SBI_EINVAL;

	अगर (i2c_adapter_find(ia->id))
		वापस SBI_EALREADY;

	sbi_list_add(&(ia->गाँठ), &(i2c_adapters_list));

	वापस 0;
}

व्योम i2c_adapter_remove(काष्ठा i2c_adapter *ia)
{
	अगर (!ia)
		वापस;

	sbi_list_del(&(ia->गाँठ));
}

पूर्णांक i2c_adapter_write(काष्ठा i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *बफर, पूर्णांक len)
{
	अगर (!ia)
		वापस SBI_EINVAL;
	अगर (!ia->ग_लिखो)
		वापस SBI_ENOSYS;

	वापस ia->ग_लिखो(ia, addr, reg, बफर, len);
}


पूर्णांक i2c_adapter_read(काष्ठा i2c_adapter *ia, uint8_t addr, uint8_t reg,
		     uint8_t *बफर, पूर्णांक len)
{
	अगर (!ia)
		वापस SBI_EINVAL;
	अगर (!ia->पढ़ो)
		वापस SBI_ENOSYS;

	वापस ia->पढ़ो(ia, addr, reg, बफर, len);
}
