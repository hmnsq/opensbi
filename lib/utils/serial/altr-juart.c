/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#تعریف_کرو JUART_ڈیٹا_انحراف		0x00
# define JUART_ڈیٹا_ڈیٹا_نقاب		0x000000ff
# define JUART_ڈیٹا_RVALID		(1 << 15)
# define JUART_ڈیٹا_RAVAIL_نقاب		0xffff0000
# define JUART_ڈیٹا_RAVAIL_سرکاؤ	16
#تعریف_کرو JUART_CTRL_انحراف		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_نقاب		0xffff0000
# define JUART_CTRL_WSPACE_سرکاؤ	16

/* clang-format on */

ساکن متلون حرف *altr_juart_بنیاد;

ساکن u32 get_reg(u32 انحراف)
{
	واپس readl(altr_juart_بنیاد + انحراف);
}

ساکن خالی سیٹ_reg(u32 انحراف, u32 val)
{
	writel(val, altr_juart_بنیاد + انحراف);
}

ساکن خالی altr_juart_putc(حرف ch)
{
	جب(!(get_reg(JUART_CTRL_انحراف) & JUART_CTRL_WSPACE_نقاب))
		;

	سیٹ_reg(JUART_ڈیٹا_انحراف, (غیرعلامتی حرف)ch);
}

ساکن صحیح altr_juart_getc(خالی)
{
	u32 reg = get_reg(JUART_ڈیٹا_انحراف);
	اگر (reg & JUART_ڈیٹا_RVALID)
		واپس reg & JUART_ڈیٹا_ڈیٹا_نقاب;

	واپس -1;
}

ساکن ساخت sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحیح altr_juart_init(غیرعلامتی طویل بنیاد_2)
{
	altr_juart_بنیاد = (متلون حرف *)بنیاد_2;

	sbi_console_سیٹ_device(&altr_juart_console);

	واپس 0;
}
