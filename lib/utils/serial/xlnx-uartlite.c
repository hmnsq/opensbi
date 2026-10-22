/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Alistair Francis <alistair.francis@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/xlnx_uartlite.h>

/* clang-format off */

#परिभाषय UART_RX_विस्थापनम्		0x00
#परिभाषय UART_TX_विस्थापनम्		0x04
#परिभाषय UART_स्थितिः_विस्थापनम्	0x08
# define UART_स्थितिः_RXVALID	0x01
# define UART_स्थितिः_RXFULL	0x02
# define UART_स्थितिः_TXEMPTY	0x04
# define UART_स्थितिः_TXFULL	0x08
# define UART_स्थितिः_IE	0x10
# define UART_स्थितिः_OVERRUN	0x20
# define UART_स्थितिः_FRAME	0x40
# define UART_स्थितिः_PARITY	0x80
#परिभाषय UART_CTRL_विस्थापनम्	0x0C
# define UART_CTRL_RST_TX	0x01
# define UART_CTRL_RST_RX	0x02
# define UART_CTRL_IE		0x10

/* clang-format on */

स्थिर चञ्चल वर्ण *xlnx_uartlite_आधारः;

स्थिर शून्य xlnx_uartlite_putc(वर्ण ch)
{
	यावत्((readb(xlnx_uartlite_आधारः + UART_स्थितिः_विस्थापनम्) & UART_स्थितिः_TXFULL))
		;

	writeb(ch, xlnx_uartlite_आधारः + UART_TX_विस्थापनम्);
}

स्थिर पूर्णाङ्क xlnx_uartlite_getc(शून्य)
{
	u16 स्थितिः = readb(xlnx_uartlite_आधारः + UART_स्थितिः_विस्थापनम्);

	यदि (स्थितिः & UART_स्थितिः_RXVALID)
		प्रत्यागम readb(xlnx_uartlite_आधारः + UART_RX_विस्थापनम्);

	प्रत्यागम -1;
}

स्थिर संरचना sbi_console_device xlnx_uartlite_console = {
	.name = "xlnx-uartlite",
	.console_putc = xlnx_uartlite_putc,
	.console_getc = xlnx_uartlite_getc
};

पूर्णाङ्क xlnx_uartlite_init(अचिह्नित दीर्घ आधारः_2)
{
	xlnx_uartlite_आधारः = (चञ्चल वर्ण *)आधारः_2;

	sbi_console_समुच्चयः_device(&xlnx_uartlite_console);

	प्रत्यागम 0;
}
