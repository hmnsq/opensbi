/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#تعریف_کرو JUART_انکڑے_وتھ		0x00
# define JUART_انکڑے_انکڑے_نقاب		0x000000ff
# define JUART_انکڑے_RVALID		(1 << 15)
# define JUART_انکڑے_RAVAIL_نقاب		0xffff0000
# define JUART_انکڑے_RAVAIL_کھسکاؤ	16
#تعریف_کرو JUART_CTRL_وتھ		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_نقاب		0xffff0000
# define JUART_CTRL_WSPACE_کھسکاؤ	16

/* clang-format on */

ساكن متطاير حرف *altr_juart_ادھار;

ساكن u32 get_reg(u32 وتھ)
{
	أرجع readl(altr_juart_ادھار + وتھ);
}

ساكن فراغ سیٹ_reg(u32 وتھ, u32 val)
{
	writel(val, altr_juart_ادھار + وتھ);
}

ساكن فراغ altr_juart_putc(حرف ch)
{
	طالما(!(get_reg(JUART_CTRL_وتھ) & JUART_CTRL_WSPACE_نقاب))
		;

	سیٹ_reg(JUART_انکڑے_وتھ, (بنانشانی حرف)ch);
}

ساكن صحيح altr_juart_getc(فراغ)
{
	u32 reg = get_reg(JUART_انکڑے_وتھ);
	إذا (reg & JUART_انکڑے_RVALID)
		أرجع reg & JUART_انکڑے_انکڑے_نقاب;

	أرجع -1;
}

ساكن بنية sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحيح altr_juart_init(بنانشانی لما ادھار_2)
{
	altr_juart_ادھار = (متطاير حرف *)ادھار_2;

	sbi_console_سیٹ_device(&altr_juart_console);

	أرجع 0;
}
