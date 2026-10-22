/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#define UART_REG_TXFIFO		0
#define UART_REG_RXFIFO		1
#define UART_REG_TXCTRL		2
#define UART_REG_RXCTRL		3
#define UART_REG_IE		4
#define UART_REG_IP		5
#define UART_REG_DIV		6

#define UART_TXFIFO_FULL	0x80000000
#define UART_RXFIFO_EMPTY	0x80000000
#define UART_RXFIFO_DATA	0x000000ff
#define UART_TXCTRL_TXEN	0x1
#define UART_RXCTRL_RXEN	0x1

/* clang-format on */

ସ୍ଥିର ଅସ୍ଥିର ଅକ୍ଷର *uart_base;
ସ୍ଥିର u32 uart_in_freq;
ସ୍ଥିର u32 uart_baudrate;

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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା uart_min_clk_ଭାଜକ(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	ଯଦି (quotient == 0)
		ଫେରନ୍ତୁ 0;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ quotient - 1;
}

ସ୍ଥିର u32 get_reg(u32 ଅଙ୍କ)
{
	ଫେରନ୍ତୁ readl(uart_base + (ଅଙ୍କ * 0x4));
}

ସ୍ଥିର ଶୂନ୍ୟ set_reg(u32 ଅଙ୍କ, u32 val)
{
	writel(val, uart_base + (ଅଙ୍କ * 0x4));
}

ସ୍ଥିର ଶୂନ୍ୟ sifive_uart_putc(ଅକ୍ଷର ch)
{
	ଯେତେବେଳେ (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_uart_getc(ଶୂନ୍ୟ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	ଯଦି (!(ret & UART_RXFIFO_EMPTY))
		ଫେରନ୍ତୁ ret & UART_RXFIFO_DATA;

	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (ଅସ୍ଥିର ଅକ୍ଷର *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	ଯଦି (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_ଭାଜକ(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	ଫେରନ୍ତୁ sbi_domain_ମୂଳ_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
