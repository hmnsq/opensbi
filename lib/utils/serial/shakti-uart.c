/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/shakti-uart.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_BAUD	0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_TX		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_RX		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_ਸਥਿਤੀ	0x0C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_DELAY	0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_CONTROL	0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_INT_EN	0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_IQ_ਚੱਕਰਾਂ	0x1C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_RX_THRES	0x20

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TX_ਭਰਿਆ  0x2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RX_NOT_ਖਾਲੀ_ਥਾਵਾਂ 0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RX_ਭਰਿਆ  0x8

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *uart_ਅਧਾਰ;

ਸਥਿਰ ਖਾਲੀ shakti_uart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ ((readb(uart_ਅਧਾਰ + REG_ਸਥਿਤੀ) & UART_TX_ਭਰਿਆ))
		;
	writeb(ch, uart_ਅਧਾਰ + REG_TX);
}

ਸਥਿਰ ਪੂਰਨ shakti_uart_getc(ਖਾਲੀ)
{
	ਜੇ (readb(uart_ਅਧਾਰ + REG_ਸਥਿਤੀ) & UART_RX_NOT_ਖਾਲੀ_ਥਾਵਾਂ)
		ਵਾਪਸ readb(uart_ਅਧਾਰ + REG_RX);
	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

ਪੂਰਨ shakti_uart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate)
{
	uart_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;
	u16 baud;

	ਜੇ (baudrate) {
		baud = (u16)(ਵਿੱਚ_freq / (16 * baudrate));
		writew(baud, uart_ਅਧਾਰ + REG_BAUD);
	}

	sbi_console_ਸੈੱਟ_device(&shakti_console);

	ਵਾਪਸ 0;
}
