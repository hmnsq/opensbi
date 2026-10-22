/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi_utils/serial/sifive-uart.h>

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

நிலையான நிலையற்ற எழுத்து *uart_base;
நிலையான u32 uart_in_freq;
நிலையான u32 uart_baudrate;

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
நிலையான உள்வரி unsigned முழு uart_min_clk_வகுஎண்(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	எனில் (quotient == 0)
		திரும்பு 0;
	இல்லையெனில்
		திரும்பு quotient - 1;
}

நிலையான u32 get_reg(u32 இலக்கம்)
{
	திரும்பு readl(uart_base + (இலக்கம் * 0x4));
}

நிலையான வெறுமை set_reg(u32 இலக்கம், u32 val)
{
	writel(val, uart_base + (இலக்கம் * 0x4));
}

நிலையான வெறுமை sifive_uart_putc(எழுத்து ch)
{
	போது (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_FULL)
		;

	set_reg(UART_REG_TXFIFO, ch);
}

நிலையான முழு sifive_uart_getc(வெறுமை)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	எனில் (!(ret & UART_RXFIFO_EMPTY))
		திரும்பு ret & UART_RXFIFO_DATA;

	திரும்பு -1;
}

நிலையான கட்டமைப்பு sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

முழு sifive_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (நிலையற்ற எழுத்து *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	எனில் (in_freq && baudrate)
		set_reg(UART_REG_DIV, uart_min_clk_வகுஎண்(in_freq, baudrate));

	/* Disable interrupts */
	set_reg(UART_REG_IE, 0);

	/* Enable TX */
	set_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	set_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_set_device(&sifive_console);

	திரும்பு sbi_domain_வேர்_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
