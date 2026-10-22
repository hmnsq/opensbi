/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/altr-juart.h>

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

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *altr_juart_base;

ಸ್ಥಿರ u32 get_reg(u32 offset)
{
	ಪ್ರತ್ಯಾಗಮ readl(altr_juart_base + offset);
}

ಸ್ಥಿರ ಶೂನ್ಯ set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

ಸ್ಥಿರ ಶೂನ್ಯ altr_juart_putc(ವರ್ಣ ch)
{
	ಯಾವತ್(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned ವರ್ಣ)ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ altr_juart_getc(ಶೂನ್ಯ)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	ಯದಿ (reg & JUART_DATA_RVALID)
		ಪ್ರತ್ಯಾಗಮ reg & JUART_DATA_DATA_MASK;

	ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

ಪೂರ್ಣಾಙ್ಕ altr_juart_init(unsigned long base)
{
	altr_juart_base = (ಚಞ್ಚಲ ವರ್ಣ *)base;

	sbi_console_set_device(&altr_juart_console);

	ಪ್ರತ್ಯಾಗಮ 0;
}
