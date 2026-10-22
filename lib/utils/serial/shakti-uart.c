/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Vijai Kumar K <vijai@behindbytes.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/shakti-uart.h>

#घोषणा REG_BAUD	0x00
#घोषणा REG_TX		0x04
#घोषणा REG_RX		0x08
#घोषणा REG_STATUS	0x0C
#घोषणा REG_DELAY	0x10
#घोषणा REG_CONTROL	0x14
#घोषणा REG_INT_EN	0x18
#घोषणा REG_IQ_CYCLES	0x1C
#घोषणा REG_RX_THRES	0x20

#घोषणा UART_TX_FULL  0x2
#घोषणा UART_RX_NOT_EMPTY 0x4
#घोषणा UART_RX_FULL  0x8

अटल अस्थिर अक्षर *uart_base;

अटल व्योम shakti_uart_putc(अक्षर ch)
{
	जबतक ((readb(uart_base + REG_STATUS) & UART_TX_FULL))
		;
	writeb(ch, uart_base + REG_TX);
}

अटल पूर्णांक shakti_uart_getc(व्योम)
{
	अगर (readb(uart_base + REG_STATUS) & UART_RX_NOT_EMPTY)
		वापस readb(uart_base + REG_RX);
	वापस -1;
}

अटल काष्ठा sbi_console_device shakti_console = {
	.name = "shakti_uart",
	.console_putc = shakti_uart_putc,
	.console_getc = shakti_uart_getc
};

पूर्णांक shakti_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	uart_base = (अस्थिर अक्षर *)base;
	u16 baud;

	अगर (baudrate) {
		baud = (u16)(in_freq / (16 * baudrate));
		writew(baud, uart_base + REG_BAUD);
	}

	sbi_console_set_device(&shakti_console);

	वापस 0;
}
