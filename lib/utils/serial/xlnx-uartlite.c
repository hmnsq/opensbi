/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_RX_ಸ್ಥಾನಾಂತರ		0x00
#ವ್ಯಾಖ್ಯಾನಿಸು UART_TX_ಸ್ಥಾನಾಂತರ		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ	0x08
# define UART_ಸ್ಥಿತಿ_RXVALID	0x01
# define UART_ಸ್ಥಿತಿ_RXFULL	0x02
# define UART_ಸ್ಥಿತಿ_TXEMPTY	0x04
# define UART_ಸ್ಥಿತಿ_TXFULL	0x08
# define UART_ಸ್ಥಿತಿ_IE	0x10
# define UART_ಸ್ಥಿತಿ_OVERRUN	0x20
# define UART_ಸ್ಥಿತಿ_ಚೌಕಟ್ಟು	0x40
# define UART_ಸ್ಥಿತಿ_PARITY	0x80
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_ಸ್ಥಾನಾಂತರ	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *xlnx_uartlite_ತಳ;

ಸ್ಥಿರ ಶೂನ್ಯ xlnx_uartlite_putc(ಅಕ್ಷರ ch)
{
	ಆಗ((readb(xlnx_uartlite_ತಳ + UART_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ) & UART_ಸ್ಥಿತಿ_TXFULL))
		;

	writeb(ch, xlnx_uartlite_ತಳ + UART_TX_ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xlnx_uartlite_getc(ಶೂನ್ಯ)
{
	u16 ಸ್ಥಿತಿ_2 = readb(xlnx_uartlite_ತಳ + UART_ಸ್ಥಿತಿ_ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (ಸ್ಥಿತಿ_2 & UART_ಸ್ಥಿತಿ_RXVALID)
		ಹಿಂತಿರುಗಿ readb(xlnx_uartlite_ತಳ + UART_RX_ಸ್ಥಾನಾಂತರ);

	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

ಪೂರ್ಣಾಂಕ xlnx_uartlite_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2)
{
	xlnx_uartlite_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;

	sbi_console_ಗಣ_device(&xlnx_uartlite_console);

	ಹಿಂತಿರುಗಿ 0;
}
