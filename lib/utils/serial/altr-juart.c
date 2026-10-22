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

#ವ್ಯಾಖ್ಯಾನಿಸು JUART_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ		0x00
# define JUART_ದತ್ತಾಂಶ_ದತ್ತಾಂಶ_ಮುಖವಾಡ		0x000000ff
# define JUART_ದತ್ತಾಂಶ_RVALID		(1 << 15)
# define JUART_ದತ್ತಾಂಶ_RAVAIL_ಮುಖವಾಡ		0xffff0000
# define JUART_ದತ್ತಾಂಶ_RAVAIL_ಸರಿಸು	16
#ವ್ಯಾಖ್ಯಾನಿಸು JUART_CTRL_ಸ್ಥಾನಾಂತರ		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_ಮುಖವಾಡ		0xffff0000
# define JUART_CTRL_WSPACE_ಸರಿಸು	16

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *altr_juart_ತಳ;

ಸ್ಥಿರ u32 get_reg(u32 ಸ್ಥಾನಾಂತರ)
{
	ಹಿಂತಿರುಗಿ readl(altr_juart_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u32 ಸ್ಥಾನಾಂತರ, u32 val)
{
	writel(val, altr_juart_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ altr_juart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ(!(get_reg(JUART_CTRL_ಸ್ಥಾನಾಂತರ) & JUART_CTRL_WSPACE_ಮುಖವಾಡ))
		;

	ಗಣ_reg(JUART_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ altr_juart_getc(ಶೂನ್ಯ)
{
	u32 reg = get_reg(JUART_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
	ಆದರೆ (reg & JUART_ದತ್ತಾಂಶ_RVALID)
		ಹಿಂತಿರುಗಿ reg & JUART_ದತ್ತಾಂಶ_ದತ್ತಾಂಶ_ಮುಖವಾಡ;

	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

ಪೂರ್ಣಾಂಕ altr_juart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2)
{
	altr_juart_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;

	sbi_console_ಗಣ_device(&altr_juart_console);

	ಹಿಂತಿರುಗಿ 0;
}
