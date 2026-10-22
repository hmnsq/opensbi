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

#وصف_ڪريو JUART_معلومات_وٿي		0x00
# define JUART_معلومات_معلومات_نقاب		0x000000ff
# define JUART_معلومات_RVALID		(1 << 15)
# define JUART_معلومات_RAVAIL_نقاب		0xffff0000
# define JUART_معلومات_RAVAIL_سرڪايو	16
#وصف_ڪريو JUART_CTRL_وٿي		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_نقاب		0xffff0000
# define JUART_CTRL_WSPACE_سرڪايو	16

/* clang-format on */

ساكن متطاير حرف *altr_juart_بنياد;

ساكن u32 get_reg(u32 وٿي)
{
	أرجع readl(altr_juart_بنياد + وٿي);
}

ساكن فراغ سيٽ_reg(u32 وٿي, u32 val)
{
	writel(val, altr_juart_بنياد + وٿي);
}

ساكن فراغ altr_juart_putc(حرف ch)
{
	طالما(!(get_reg(JUART_CTRL_وٿي) & JUART_CTRL_WSPACE_نقاب))
		;

	سيٽ_reg(JUART_معلومات_وٿي, (بنانشان حرف)ch);
}

ساكن صحيح altr_juart_getc(فراغ)
{
	u32 reg = get_reg(JUART_معلومات_وٿي);
	إذا (reg & JUART_معلومات_RVALID)
		أرجع reg & JUART_معلومات_معلومات_نقاب;

	أرجع -1;
}

ساكن بنية sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحيح altr_juart_init(بنانشان ڊگهو بنياد_2)
{
	altr_juart_بنياد = (متطاير حرف *)بنياد_2;

	sbi_console_سيٽ_device(&altr_juart_console);

	أرجع 0;
}
