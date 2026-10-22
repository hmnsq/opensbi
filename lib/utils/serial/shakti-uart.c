/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/shakti-uart.h>

#ವ್ಯಾಖ್ಯಾನಿಸು REG_BAUD	0x00
#ವ್ಯಾಖ್ಯಾನಿಸು REG_TX		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು REG_RX		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು REG_ಸ್ಥಿತಿ	0x0C
#ವ್ಯಾಖ್ಯಾನಿಸು REG_DELAY	0x10
#ವ್ಯಾಖ್ಯಾನಿಸು REG_CONTROL	0x14
#ವ್ಯಾಖ್ಯಾನಿಸು REG_INT_EN	0x18
#ವ್ಯಾಖ್ಯಾನಿಸು REG_IQ_ಚಕ್ರಗಳು	0x1C
#ವ್ಯಾಖ್ಯಾನಿಸು REG_RX_THRES	0x20

#ವ್ಯಾಖ್ಯಾನಿಸು UART_TX_ತುಂಬಿದ  0x2
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RX_NOT_ಖಾಲಿಗಳು 0x4
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RX_ತುಂಬಿದ  0x8

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *uart_ತಳ;

ಸ್ಥಿರ ಶೂನ್ಯ shakti_uart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ ((readb(uart_ತಳ + REG_ಸ್ಥಿತಿ) & UART_TX_ತುಂಬಿದ))
		;
	writeb(ch, uart_ತಳ + REG_TX);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ shakti_uart_getc(ಶೂನ್ಯ)
{
	ಆದರೆ (readb(uart_ತಳ + REG_ಸ್ಥಿತಿ) & UART_RX_NOT_ಖಾಲಿಗಳು)
		ಹಿಂತಿರುಗಿ readb(uart_ತಳ + REG_RX);
	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

ಪೂರ್ಣಾಂಕ shakti_uart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate)
{
	uart_ತಳ = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;
	u16 baud;

	ಆದರೆ (baudrate) {
		baud = (u16)(ಒಳ_freq / (16 * baudrate));
		writew(baud, uart_ತಳ + REG_BAUD);
	}

	sbi_console_ಗಣ_device(&shakti_console);

	ಹಿಂತಿರುಗಿ 0;
}
