/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/altr-juart.h>

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

ସ୍ଥିର ଅସ୍ଥିର ଅକ୍ଷର *altr_juart_base;

ସ୍ଥିର u32 get_reg(u32 offset)
{
	ଫେରନ୍ତୁ readl(altr_juart_base + offset);
}

ସ୍ଥିର ଶୂନ୍ୟ set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

ସ୍ଥିର ଶୂନ୍ୟ altr_juart_putc(ଅକ୍ଷର ch)
{
	ଯେତେବେଳେ(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned ଅକ୍ଷର)ch);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା altr_juart_getc(ଶୂନ୍ୟ)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	ଯଦି (reg & JUART_DATA_RVALID)
		ଫେରନ୍ତୁ reg & JUART_DATA_DATA_MASK;

	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା altr_juart_init(unsigned long base)
{
	altr_juart_base = (ଅସ୍ଥିର ଅକ୍ଷର *)base;

	sbi_console_set_device(&altr_juart_console);

	ଫେରନ୍ତୁ 0;
}
