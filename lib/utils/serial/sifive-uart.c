/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi_utils/serial/sifive-uart.h>

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
#define UART_RXFIFO_তথ্য	0x000000ff
#define UART_TXCTRL_TXEN	0x1
#define UART_RXCTRL_RXEN	0x1

/* clang-format on */

স্থির অস্থির অক্ষর *uart_base;
স্থির u32 uart_in_freq;
স্থির u32 uart_baudrate;

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
স্থির অন্তঃরেখ unsigned পূর্ণসংখ্যা uart_min_clk_ভাজক(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	যদি (quotient == 0)
		ফেরত 0;
	অন্যথা
		ফেরত quotient - 1;
}

স্থির u32 get_reg(u32 অঙ্ক)
{
	ফেরত readl(uart_base + (অঙ্ক * 0x4));
}

স্থির শূন্য set_reg(u32 অঙ্ক, u32 val)
{
	writel(val, uart_base + (অঙ্ক * 0x4));
}

স্থির শূন্য sifive_uart_putc(অক্ষর ch)
{
	যখন (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

স্থির পূর্ণসংখ্যা sifive_uart_getc(শূন্য)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	যদি (!(ret & UART_RXFIFO_EMPTY))
		ফেরত ret & UART_RXFIFO_তথ্য;

	ফেরত -1;
}

স্থির কাঠামো sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

পূর্ণসংখ্যা sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (অস্থির অক্ষর *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	যদি (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_ভাজক(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	ফেরত sbi_domain_মূল_add_memrange(base, PAGE_আকার, PAGE_আকার,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
