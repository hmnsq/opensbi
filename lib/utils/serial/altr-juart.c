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

#define JUART_তথ্য_OFFSET		0x00
# define JUART_তথ্য_তথ্য_MASK		0x000000ff
# define JUART_তথ্য_RVALID		(1 << 15)
# define JUART_তথ্য_RAVAIL_MASK		0xffff0000
# define JUART_তথ্য_RAVAIL_SHIFT	16
#define JUART_CTRL_OFFSET		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_MASK		0xffff0000
# define JUART_CTRL_WSPACE_SHIFT	16

/* clang-format on */

স্থির অস্থির অক্ষর *altr_juart_base;

স্থির u32 get_reg(u32 offset)
{
	ফেরত readl(altr_juart_base + offset);
}

স্থির শূন্য set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

স্থির শূন্য altr_juart_putc(অক্ষর ch)
{
	যখন(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_তথ্য_OFFSET, (unsigned অক্ষর)ch);
}

স্থির পূর্ণসংখ্যা altr_juart_getc(শূন্য)
{
	u32 reg = get_reg(JUART_তথ্য_OFFSET);
	যদি (reg & JUART_তথ্য_RVALID)
		ফেরত reg & JUART_তথ্য_তথ্য_MASK;

	ফেরত -1;
}

স্থির কাঠামো sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

পূর্ণসংখ্যা altr_juart_init(unsigned long base)
{
	altr_juart_base = (অস্থির অক্ষর *)base;

	sbi_console_set_device(&altr_juart_console);

	ফেরত 0;
}
