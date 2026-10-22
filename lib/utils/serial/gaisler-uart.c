/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Cobham Gaisler AB.
 *
 * Authors:
 *   Daniel Cederman <cederman@gaisler.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/gaisler-uart.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_ದತ್ತಾಂಶ		0
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_ಸ್ಥಿತಿ		1
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_CTRL		2
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_SCALER		3

#ವ್ಯಾಖ್ಯಾನಿಸು UART_ದತ್ತಾಂಶ_ದತ್ತಾಂಶ		0x000000ff
#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಸ್ಥಿತಿ_FIFOFULL	0x00000200
#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಸ್ಥಿತಿ_DATAREADY	0x00000001

#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_DB		(1<<11)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_FL		(1<<6)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_TE		(1<<1)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_RE		(1<<0)

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *uart_ತಳ;

ಸ್ಥಿರ u32 get_reg(u32 ಅಂಕೆ)
{
	ಹಿಂತಿರುಗಿ readl(uart_ತಳ + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u32 ಅಂಕೆ, u32 val)
{
	writel(val, uart_ತಳ + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ gaisler_uart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ (get_reg(UART_REG_ಸ್ಥಿತಿ) & UART_ಸ್ಥಿತಿ_FIFOFULL)
		;

	ಗಣ_reg(UART_REG_ದತ್ತಾಂಶ, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gaisler_uart_getc(ಶೂನ್ಯ)
{
	u32 ret = get_reg(UART_REG_ಸ್ಥಿತಿ);
	ಆದರೆ (!(ret & UART_ಸ್ಥಿತಿ_DATAREADY))
		ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ get_reg(UART_REG_ದತ್ತಾಂಶ) & UART_ದತ್ತಾಂಶ_ದತ್ತಾಂಶ;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device gaisler_console = {
	.name	      = "gaisler_uart",
	.console_putc = gaisler_uart_putc,
	.console_getc = gaisler_uart_getc
};

ಪೂರ್ಣಾಂಕ gaisler_uart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate)
{
	u32 ctrl;

	uart_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;

	/* Configure baudrate */
	ಆದರೆ (ಒಳ_freq && baudrate)
		ಗಣ_reg(UART_REG_SCALER, ಒಳ_freq / (baudrate * 8 + 7));

	ctrl = get_reg(UART_REG_CTRL);
	/* Preserve debug mode and flow control */
	ctrl &= (UART_CTRL_DB | UART_CTRL_FL);
	/* Enable TX and RX */
	ctrl |= UART_CTRL_TE | UART_CTRL_RE;
	ಗಣ_reg(UART_REG_CTRL, ctrl);

	sbi_console_ಗಣ_device(&gaisler_console);

	ಹಿಂತಿರುಗಿ 0;
}
