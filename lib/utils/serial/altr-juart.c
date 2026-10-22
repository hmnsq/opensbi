/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#నిర్వచించు JUART_దత్తాంశం_స్థానభ్రంశం		0x00
# define JUART_దత్తాంశం_దత్తాంశం_ముసుగు		0x000000ff
# define JUART_దత్తాంశం_RVALID		(1 << 15)
# define JUART_దత్తాంశం_RAVAIL_ముసుగు		0xffff0000
# define JUART_దత్తాంశం_RAVAIL_జరుపు	16
#నిర్వచించు JUART_CTRL_స్థానభ్రంశం		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_ముసుగు		0xffff0000
# define JUART_CTRL_WSPACE_జరుపు	16

/* clang-format on */

స్థిర అస్థిర అక్షరం *altr_juart_పునాది;

స్థిర u32 get_reg(u32 స్థానభ్రంశం)
{
	తిరిగి readl(altr_juart_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య సమితి_reg(u32 స్థానభ్రంశం, u32 val)
{
	writel(val, altr_juart_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య altr_juart_putc(అక్షరం ch)
{
	అయితే(!(get_reg(JUART_CTRL_స్థానభ్రంశం) & JUART_CTRL_WSPACE_ముసుగు))
		;

	సమితి_reg(JUART_దత్తాంశం_స్థానభ్రంశం, (గుర్తులేని అక్షరం)ch);
}

స్థిర పూర్ణాంకం altr_juart_getc(శూన్య)
{
	u32 reg = get_reg(JUART_దత్తాంశం_స్థానభ్రంశం);
	ఒకవేళ (reg & JUART_దత్తాంశం_RVALID)
		తిరిగి reg & JUART_దత్తాంశం_దత్తాంశం_ముసుగు;

	తిరిగి -1;
}

స్థిర నిర్మాణం sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

పూర్ణాంకం altr_juart_init(గుర్తులేని దీర్ఘ పునాది_2)
{
	altr_juart_పునాది = (అస్థిర అక్షరం *)పునాది_2;

	sbi_console_సమితి_device(&altr_juart_console);

	తిరిగి 0;
}
