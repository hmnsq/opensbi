/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/sifive-uart.h>

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

ಸ್ಥಿರ ಚಞ್ಚಲ ವರ್ಣ *uart_base;
ಸ್ಥಿರ u32 uart_in_freq;
ಸ್ಥಿರ u32 uart_baudrate;

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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ uart_min_clk_ಭಾಜಕ(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	ಯದಿ (quotient == 0)
		ಪ್ರತ್ಯಾಗಮ 0;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ quotient - 1;
}

ಸ್ಥಿರ u32 get_reg(u32 ಅಂಕೆ)
{
	ಪ್ರತ್ಯಾಗಮ readl(uart_base + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ set_reg(u32 ಅಂಕೆ, u32 val)
{
	writel(val, uart_base + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_uart_putc(ವರ್ಣ ch)
{
	ಯಾವತ್ (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sifive_uart_getc(ಶೂನ್ಯ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	ಯದಿ (!(ret & UART_RXFIFO_EMPTY))
		ಪ್ರತ್ಯಾಗಮ ret & UART_RXFIFO_DATA;

	ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

ಪೂರ್ಣಾಙ್ಕ sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (ಚಞ್ಚಲ ವರ್ಣ *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	ಯದಿ (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_ಭಾಜಕ(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	ಪ್ರತ್ಯಾಗಮ sbi_domain_ಬೇರು_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
