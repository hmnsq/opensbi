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

#تعریف_کن JUART_داده_جابه_جایی		0x00
# define JUART_داده_داده_ماسک		0x000000ff
# define JUART_داده_RVALID		(1 << 15)
# define JUART_داده_RAVAIL_ماسک		0xffff0000
# define JUART_داده_RAVAIL_لغزش	16
#تعریف_کن JUART_CTRL_جابه_جایی		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_ماسک		0xffff0000
# define JUART_CTRL_WSPACE_لغزش	16

/* clang-format on */

ساكن متطاير حرف *altr_juart_پایه;

ساكن u32 get_reg(u32 جابه_جایی)
{
	أرجع readl(altr_juart_پایه + جابه_جایی);
}

ساكن فراغ مجموعه_reg(u32 جابه_جایی, u32 val)
{
	writel(val, altr_juart_پایه + جابه_جایی);
}

ساكن فراغ altr_juart_putc(حرف ch)
{
	طالما(!(get_reg(JUART_CTRL_جابه_جایی) & JUART_CTRL_WSPACE_ماسک))
		;

	مجموعه_reg(JUART_داده_جابه_جایی, (بیعلامت حرف)ch);
}

ساكن صحيح altr_juart_getc(فراغ)
{
	u32 reg = get_reg(JUART_داده_جابه_جایی);
	إذا (reg & JUART_داده_RVALID)
		أرجع reg & JUART_داده_داده_ماسک;

	أرجع -1;
}

ساكن بنية sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحيح altr_juart_init(بیعلامت دراز پایه_2)
{
	altr_juart_پایه = (متطاير حرف *)پایه_2;

	sbi_console_مجموعه_device(&altr_juart_console);

	أرجع 0;
}
