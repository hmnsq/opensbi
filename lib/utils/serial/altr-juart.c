/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JUART_ਅੰਕੜੇ_ਵਿੱਥ		0x00
# define JUART_ਅੰਕੜੇ_ਅੰਕੜੇ_ਨਕਾਬ		0x000000ff
# define JUART_ਅੰਕੜੇ_RVALID		(1 << 15)
# define JUART_ਅੰਕੜੇ_RAVAIL_ਨਕਾਬ		0xffff0000
# define JUART_ਅੰਕੜੇ_RAVAIL_ਖਿਸਕਾਓ	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JUART_CTRL_ਵਿੱਥ		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_ਨਕਾਬ		0xffff0000
# define JUART_CTRL_WSPACE_ਖਿਸਕਾਓ	16

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *altr_juart_ਅਧਾਰ;

ਸਥਿਰ u32 get_reg(u32 ਵਿੱਥ)
{
	ਵਾਪਸ readl(altr_juart_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u32 ਵਿੱਥ, u32 val)
{
	writel(val, altr_juart_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ altr_juart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ(!(get_reg(JUART_CTRL_ਵਿੱਥ) & JUART_CTRL_WSPACE_ਨਕਾਬ))
		;

	ਸੈੱਟ_reg(JUART_ਅੰਕੜੇ_ਵਿੱਥ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ch);
}

ਸਥਿਰ ਪੂਰਨ altr_juart_getc(ਖਾਲੀ)
{
	u32 reg = get_reg(JUART_ਅੰਕੜੇ_ਵਿੱਥ);
	ਜੇ (reg & JUART_ਅੰਕੜੇ_RVALID)
		ਵਾਪਸ reg & JUART_ਅੰਕੜੇ_ਅੰਕੜੇ_ਨਕਾਬ;

	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

ਪੂਰਨ altr_juart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2)
{
	altr_juart_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;

	sbi_console_ਸੈੱਟ_device(&altr_juart_console);

	ਵਾਪਸ 0;
}
