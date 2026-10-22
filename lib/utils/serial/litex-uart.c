/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Gabriel Somlo
 *
 * Authors:
 *   Gabriel Somlo <gsomlo@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/litex-uart.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_RXTX		0
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_TXFULL		1
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_RXEMPTY	2
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_EV_ಸ್ಥಿತಿ	3
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_EV_PENDING	4
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_EV_ENABLE	5

#ವ್ಯಾಖ್ಯಾನಿಸು UART_EV_RX		0x2

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ u32 *uart_ತಳ;

ಸ್ಥಿರ u8 get_reg(u8 reg)
{
	ಹಿಂತಿರುಗಿ readb(uart_ತಳ + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u8 reg, u8 val)
{
	writeb(val, uart_ತಳ + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ litex_uart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ (get_reg(UART_REG_TXFULL));
	ಗಣ_reg(UART_REG_RXTX, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ litex_uart_getc(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (get_reg(UART_REG_RXEMPTY))
		ಹಿಂತಿರುಗಿ -1;

	ret = get_reg(UART_REG_RXTX);
	ಗಣ_reg(UART_REG_EV_PENDING, UART_EV_RX); /* ack. char read */
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device litex_console = {
	.name = "litex_uart",
	.console_putc = litex_uart_putc,
	.console_getc = litex_uart_getc
};

ಪೂರ್ಣಾಂಕ litex_uart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2)
{
	uart_ತಳ = (ಅಸ್ಥಿರ u32 *)ತಳ_2;
	ಗಣ_reg(UART_REG_EV_ENABLE, 0); /* UART in polling mode: disable IRQ */
	sbi_console_ಗಣ_device(&litex_console);
	ಹಿಂತಿರುಗಿ 0;
}
