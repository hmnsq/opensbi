/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#घोषणा JUART_तथ्य_OFFSET		0x00
# define JUART_तथ्य_तथ्य_MASK		0x000000ff
# define JUART_तथ्य_RVALID		(1 << 15)
# define JUART_तथ्य_RAVAIL_MASK		0xffff0000
# define JUART_तथ्य_RAVAIL_SHIFT	16
#घोषणा JUART_CTRL_OFFSET		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_MASK		0xffff0000
# define JUART_CTRL_WSPACE_SHIFT	16

/* clang-format on */

अटल अस्थिर अक्षर *altr_juart_base;

अटल u32 get_reg(u32 offset)
{
	वापस readl(altr_juart_base + offset);
}

अटल व्योम set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

अटल व्योम altr_juart_putc(अक्षर ch)
{
	जबतक(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_तथ्य_OFFSET, (अचिन्हित अक्षर)ch);
}

अटल पूर्णांक altr_juart_getc(व्योम)
{
	u32 reg = get_reg(JUART_तथ्य_OFFSET);
	अगर (reg & JUART_तथ्य_RVALID)
		वापस reg & JUART_तथ्य_तथ्य_MASK;

	वापस -1;
}

अटल काष्ठा sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

पूर्णांक altr_juart_init(अचिन्हित दीर्घ base)
{
	altr_juart_base = (अस्थिर अक्षर *)base;

	sbi_console_set_device(&altr_juart_console);

	वापस 0;
}
