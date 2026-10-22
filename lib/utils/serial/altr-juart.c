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

#تعریف_کړه JUART_معلومات_دننه_واټن		0x00
# define JUART_معلومات_معلومات_ماسک		0x000000ff
# define JUART_معلومات_RVALID		(1 << 15)
# define JUART_معلومات_RAVAIL_ماسک		0xffff0000
# define JUART_معلومات_RAVAIL_ښويدنه	16
#تعریف_کړه JUART_CTRL_دننه_واټن		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_ماسک		0xffff0000
# define JUART_CTRL_WSPACE_ښويدنه	16

/* clang-format on */

ساكن متطاير حرف *altr_juart_بنسټ;

ساكن u32 get_reg(u32 دننه_واټن)
{
	أرجع readl(altr_juart_بنسټ + دننه_واټن);
}

ساكن فراغ سېټ_reg(u32 دننه_واټن, u32 val)
{
	writel(val, altr_juart_بنسټ + دننه_واټن);
}

ساكن فراغ altr_juart_putc(حرف ch)
{
	طالما(!(get_reg(JUART_CTRL_دننه_واټن) & JUART_CTRL_WSPACE_ماسک))
		;

	سېټ_reg(JUART_معلومات_دننه_واټن, (بینښه حرف)ch);
}

ساكن صحيح altr_juart_getc(فراغ)
{
	u32 reg = get_reg(JUART_معلومات_دننه_واټن);
	إذا (reg & JUART_معلومات_RVALID)
		أرجع reg & JUART_معلومات_معلومات_ماسک;

	أرجع -1;
}

ساكن بنية sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحيح altr_juart_init(بینښه اوږد بنسټ_2)
{
	altr_juart_بنسټ = (متطاير حرف *)بنسټ_2;

	sbi_console_سېټ_device(&altr_juart_console);

	أرجع 0;
}
