/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/serial/altr-juart.h>

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

நிலையான நிலையற்ற எழுத்து *altr_juart_base;

நிலையான u32 get_reg(u32 offset)
{
	திரும்பு readl(altr_juart_base + offset);
}

நிலையான வெறுமை set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

நிலையான வெறுமை altr_juart_putc(எழுத்து ch)
{
	போது(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned எழுத்து)ch);
}

நிலையான முழு altr_juart_getc(வெறுமை)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	எனில் (reg & JUART_DATA_RVALID)
		திரும்பு reg & JUART_DATA_DATA_MASK;

	திரும்பு -1;
}

நிலையான கட்டமைப்பு sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

முழு altr_juart_init(unsigned long base)
{
	altr_juart_base = (நிலையற்ற எழுத்து *)base;

	sbi_console_set_device(&altr_juart_console);

	திரும்பு 0;
}
