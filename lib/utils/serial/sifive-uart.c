/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#घोषणा UART_REG_TXFIFO		0
#घोषणा UART_REG_RXFIFO		1
#घोषणा UART_REG_TXCTRL		2
#घोषणा UART_REG_RXCTRL		3
#घोषणा UART_REG_IE		4
#घोषणा UART_REG_IP		5
#घोषणा UART_REG_DIV		6

#घोषणा UART_TXFIFO_FULL	0x80000000
#घोषणा UART_RXFIFO_EMPTY	0x80000000
#घोषणा UART_RXFIFO_तथ्य	0x000000ff
#घोषणा UART_TXCTRL_TXEN	0x1
#घोषणा UART_RXCTRL_RXEN	0x1

/* clang-format on */

अटल अस्थिर अक्षर *uart_base;
अटल u32 uart_in_freq;
अटल u32 uart_baudrate;

/**
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on uart driver n SiFive FSBL.
 *
 * f_baud = f_in / (div + 1) => div = (f_in / f_baud) - 1
 * The nearest integer solution requires rounding up as to not exceed max_target_hz.
 * div  = ceil(f_in / f_baud) - 1
 *	= floor((f_in - 1 + f_baud) / f_baud) - 1
 * This should not overflow as long as (f_in - 1 + f_baud) does not exceed
 * 2^32 - 1, which is unlikely since we represent frequencies in kHz.
 */
अटल अंतरभूत अचिन्हित पूर्णांक uart_min_clk_भाजक(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	अगर (quotient == 0)
		वापस 0;
	अन्यथा
		वापस quotient - 1;
}

अटल u32 get_reg(u32 अंक)
{
	वापस readl(uart_base + (अंक * 0x4));
}

अटल व्योम set_reg(u32 अंक, u32 val)
{
	writel(val, uart_base + (अंक * 0x4));
}

अटल व्योम sifive_uart_putc(अक्षर ch)
{
	जबतक (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

अटल पूर्णांक sifive_uart_getc(व्योम)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	अगर (!(ret & UART_RXFIFO_EMPTY))
		वापस ret & UART_RXFIFO_तथ्य;

	वापस -1;
}

अटल काष्ठा sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

पूर्णांक sifive_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	uart_base     = (अस्थिर अक्षर *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	अगर (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_भाजक(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	वापस sbi_domain_मूल_add_memrange(base, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
