/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi_utils/serial/altr-juart.h>

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

স্থির চঞ্চল বর্ণ *altr_juart_base;

স্থির u32 get_reg(u32 offset)
{
	প্রত্যাগম readl(altr_juart_base + offset);
}

স্থির শূন্য set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

স্থির শূন্য altr_juart_putc(বর্ণ ch)
{
	যাবত্(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned বর্ণ)ch);
}

স্থির পূর্ণাঙ্ক altr_juart_getc(শূন্য)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	যদি (reg & JUART_DATA_RVALID)
		প্রত্যাগম reg & JUART_DATA_DATA_MASK;

	প্রত্যাগম -1;
}

স্থির সংরচনা sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

পূর্ণাঙ্ক altr_juart_init(unsigned long base)
{
	altr_juart_base = (চঞ্চল বর্ণ *)base;

	sbi_console_set_device(&altr_juart_console);

	প্রত্যাগম 0;
}
