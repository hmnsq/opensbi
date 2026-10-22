/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#define JUART_DATA_OFFSET		0x00
# define JUART_DATA_DATA_MASK		0x000000ff
# define JUART_DATA_RVALID		(1 << 15)
# define JUART_DATA_RAVAIL_MASK		0xffff0000
# define JUART_DATA_RAVAIL_SHIFT	16
#define JUART_CTRL_OFFSET		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_MASK		0xffff0000
# define JUART_CTRL_WSPACE_SHIFT	16

/* clang-format on */

સ્થિર અસ્થિર અક્ષર *altr_juart_base;

સ્થિર u32 get_reg(u32 offset)
{
	પાછા readl(altr_juart_base + offset);
}

સ્થિર શૂન્ય set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

સ્થિર શૂન્ય altr_juart_putc(અક્ષર ch)
{
	જ્યારે(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned અક્ષર)ch);
}

સ્થિર પૂર્ણાંક altr_juart_getc(શૂન્ય)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	જો (reg & JUART_DATA_RVALID)
		પાછા reg & JUART_DATA_DATA_MASK;

	પાછા -1;
}

સ્થિર સંરચના sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

પૂર્ણાંક altr_juart_init(unsigned long base)
{
	altr_juart_base = (અસ્થિર અક્ષર *)base;

	sbi_console_set_device(&altr_juart_console);

	પાછા 0;
}
