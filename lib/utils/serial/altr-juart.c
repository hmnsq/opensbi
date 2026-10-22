/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi_utils/serial/altr-juart.h>

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

സ്ഥിതം അസ്ഥിര അക്ഷരം *altr_juart_base;

സ്ഥിതം u32 get_reg(u32 offset)
{
	തിരികെ readl(altr_juart_base + offset);
}

സ്ഥിതം ശൂന്യം set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

സ്ഥിതം ശൂന്യം altr_juart_putc(അക്ഷരം ch)
{
	ആകുമ്പോൾ(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (unsigned അക്ഷരം)ch);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ altr_juart_getc(ശൂന്യം)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	എങ്കിൽ (reg & JUART_DATA_RVALID)
		തിരികെ reg & JUART_DATA_DATA_MASK;

	തിരികെ -1;
}

സ്ഥിതം ഘടന sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

പൂർണ്ണസംഖ്യ altr_juart_init(unsigned long base)
{
	altr_juart_base = (അസ്ഥിര അക്ഷരം *)base;

	sbi_console_set_device(&altr_juart_console);

	തിരികെ 0;
}
