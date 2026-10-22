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

ایستا فرار نویسه *altr_juart_base;

ایستا u32 get_reg(u32 offset)
{
	بازگشت readl(altr_juart_base + offset);
}

ایستا تهی set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

ایستا تهی altr_juart_putc(نویسه ch)
{
	وقتی(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned نویسه)ch);
}

ایستا صحیح altr_juart_getc(تهی)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	اگر (reg & JUART_DATA_RVALID)
		بازگشت reg & JUART_DATA_DATA_MASK;

	بازگشت -1;
}

ایستا ساختار sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

صحیح altr_juart_init(unsigned long base)
{
	altr_juart_base = (فرار نویسه *)base;

	sbi_console_set_device(&altr_juart_console);

	بازگشت 0;
}
