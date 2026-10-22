/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RX_ਵਿੱਥ		0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TX_ਵਿੱਥ		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਸਥਿਤੀ_ਵਿੱਥ	0x08
# define UART_ਸਥਿਤੀ_RXVALID	0x01
# define UART_ਸਥਿਤੀ_RXFULL	0x02
# define UART_ਸਥਿਤੀ_TXEMPTY	0x04
# define UART_ਸਥਿਤੀ_TXFULL	0x08
# define UART_ਸਥਿਤੀ_IE	0x10
# define UART_ਸਥਿਤੀ_OVERRUN	0x20
# define UART_ਸਥਿਤੀ_ਚੌਖਟਾ	0x40
# define UART_ਸਥਿਤੀ_PARITY	0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_ਵਿੱਥ	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *xlnx_uartlite_ਅਧਾਰ;

ਸਥਿਰ ਖਾਲੀ xlnx_uartlite_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ((readb(xlnx_uartlite_ਅਧਾਰ + UART_ਸਥਿਤੀ_ਵਿੱਥ) & UART_ਸਥਿਤੀ_TXFULL))
		;

	writeb(ch, xlnx_uartlite_ਅਧਾਰ + UART_TX_ਵਿੱਥ);
}

ਸਥਿਰ ਪੂਰਨ xlnx_uartlite_getc(ਖਾਲੀ)
{
	u16 ਸਥਿਤੀ = readb(xlnx_uartlite_ਅਧਾਰ + UART_ਸਥਿਤੀ_ਵਿੱਥ);

	ਜੇ (ਸਥਿਤੀ & UART_ਸਥਿਤੀ_RXVALID)
		ਵਾਪਸ readb(xlnx_uartlite_ਅਧਾਰ + UART_RX_ਵਿੱਥ);

	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

ਪੂਰਨ xlnx_uartlite_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2)
{
	xlnx_uartlite_ਅਧਾਰ = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;

	sbi_console_ਸੈੱਟ_device(&xlnx_uartlite_console);

	ਵਾਪਸ 0;
}
