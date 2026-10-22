/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#تعریف_کرو UART_REG_TXFIFO		0
#تعریف_کرو UART_REG_RXFIFO		1
#تعریف_کرو UART_REG_TXCTRL		2
#تعریف_کرو UART_REG_RXCTRL		3
#تعریف_کرو UART_REG_IE		4
#تعریف_کرو UART_REG_IP		5
#تعریف_کرو UART_REG_DIV		6

#تعریف_کرو UART_TXFIFO_بھرا	0x80000000
#تعریف_کرو UART_RXFIFO_خالی_خانے	0x80000000
#تعریف_کرو UART_RXFIFO_ڈیٹا	0x000000ff
#تعریف_کرو UART_TXCTRL_TXEN	0x1
#تعریف_کرو UART_RXCTRL_RXEN	0x1

/* clang-format on */

ساکن متلون حرف *uart_بنیاد;
ساکن u32 uart_اندر_freq;
ساکن u32 uart_baudrate;

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
ساکن اندرونی غیرعلامتی صحیح uart_min_clk_مقسوم_علیہ(uint64_t اندر_freq,
						uint64_t max_ہدف_hz)
{
	uint64_t quotient = (اندر_freq + max_ہدف_hz - 1) / (max_ہدف_hz);

	/* Avoid underflow */
	اگر (quotient == 0)
		واپس 0;
	ورنہ
		واپس quotient - 1;
}

ساکن u32 get_reg(u32 عدد)
{
	واپس readl(uart_بنیاد + (عدد * 0x4));
}

ساکن خالی سیٹ_reg(u32 عدد, u32 val)
{
	writel(val, uart_بنیاد + (عدد * 0x4));
}

ساکن خالی sifive_uart_putc(حرف ch)
{
	جب (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_بھرا)
		;

	سیٹ_reg(UART_REG_TXFIFO, ch);
}

ساکن صحیح sifive_uart_getc(خالی)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	اگر (!(ret & UART_RXFIFO_خالی_خانے))
		واپس ret & UART_RXFIFO_ڈیٹا;

	واپس -1;
}

ساکن ساخت sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

صحیح sifive_uart_init(غیرعلامتی طویل بنیاد_2, u32 اندر_freq, u32 baudrate)
{
	uart_بنیاد     = (متلون حرف *)بنیاد_2;
	uart_اندر_freq  = اندر_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	اگر (اندر_freq && baudrate)
		سیٹ_reg(UART_REG_DIV, uart_min_clk_مقسوم_علیہ(اندر_freq, baudrate));

	/* Disable interrupts */
	سیٹ_reg(UART_REG_IE, 0);

	/* Enable TX */
	سیٹ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	سیٹ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_سیٹ_device(&sifive_console);

	واپس sbi_domain_جڑ_جوڑو_memrange(بنیاد_2, صفحہ_سائز, صفحہ_سائز,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
