/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/altr-juart.h>

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

स्थिर अस्थिर अक्षर *altr_juart_base;

स्थिर u32 get_reg(u32 offset)
{
	परत readl(altr_juart_base + offset);
}

स्थिर शून्य set_reg(u32 offset, u32 val)
{
	writel(val, altr_juart_base + offset);
}

स्थिर शून्य altr_juart_putc(अक्षर ch)
{
	पर्यंत(!(get_reg(JUART_CTRL_OFFSET) & JUART_CTRL_WSPACE_MASK))
		;

	set_reg(JUART_DATA_OFFSET, (अचिन्हित अक्षर)ch);
}

स्थिर पूर्णांक altr_juart_getc(शून्य)
{
	u32 reg = get_reg(JUART_DATA_OFFSET);
	जर (reg & JUART_DATA_RVALID)
		परत reg & JUART_DATA_DATA_MASK;

	परत -1;
}

स्थिर संरचना sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

पूर्णांक altr_juart_init(अचिन्हित दीर्घ base)
{
	altr_juart_base = (अस्थिर अक्षर *)base;

	sbi_console_set_device(&altr_juart_console);

	परत 0;
}
