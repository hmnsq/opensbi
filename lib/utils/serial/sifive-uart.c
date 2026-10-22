/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_io.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi_utils/serial/sifive-uart.h>

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

static ca_ncala var_na *uart_base;
static u32 uart_in_freq;
static u32 uart_baudrate;

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
static anta_hstha unsigned ཧྲིལ uart_min_clk_divisor(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	གལ་ཏེ (quotient == 0)
		ཕྱིར 0;
	དེ་མིན
		ཕྱིར quotient - 1;
}

static u32 get_reg(u32 num)
{
	ཕྱིར readl(uart_base + (num * 0x4));
}

static void set_reg(u32 num, u32 val)
{
	writel(val, uart_base + (num * 0x4));
}

static void sifive_uart_putc(var_na ch)
{
	སྐབས (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

static ཧྲིལ sifive_uart_getc(void)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	གལ་ཏེ (!(ret & UART_RXFIFO_EMPTY))
		ཕྱིར ret & UART_RXFIFO_DATA;

	ཕྱིར -1;
}

static sa_nracanaa sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

ཧྲིལ sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (ca_ncala var_na *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	གལ་ཏེ (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_divisor(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	ཕྱིར sbi_domain_root_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
