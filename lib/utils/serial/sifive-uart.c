/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#وصف_ڪريو UART_REG_TXFIFO		0
#وصف_ڪريو UART_REG_RXFIFO		1
#وصف_ڪريو UART_REG_TXCTRL		2
#وصف_ڪريو UART_REG_RXCTRL		3
#وصف_ڪريو UART_REG_IE		4
#وصف_ڪريو UART_REG_IP		5
#وصف_ڪريو UART_REG_DIV		6

#وصف_ڪريو UART_TXFIFO_ڀريل	0x80000000
#وصف_ڪريو UART_RXFIFO_خالي_جايون	0x80000000
#وصف_ڪريو UART_RXFIFO_معلومات	0x000000ff
#وصف_ڪريو UART_TXCTRL_TXEN	0x1
#وصف_ڪريو UART_RXCTRL_RXEN	0x1

/* clang-format on */

ساكن متطاير حرف *uart_بنياد;
ساكن u32 uart_اندريون_freq;
ساكن u32 uart_baudrate;

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
ساكن مضمن بنانشان صحيح uart_min_clk_ورهائيندڙ(uint64_t اندريون_freq,
						uint64_t max_هدف_hz)
{
	uint64_t quotient = (اندريون_freq + max_هدف_hz - 1) / (max_هدف_hz);

	/* Avoid underflow */
	إذا (quotient == 0)
		أرجع 0;
	وإلا
		أرجع quotient - 1;
}

ساكن u32 get_reg(u32 انگ)
{
	أرجع readl(uart_بنياد + (انگ * 0x4));
}

ساكن فراغ سيٽ_reg(u32 انگ, u32 val)
{
	writel(val, uart_بنياد + (انگ * 0x4));
}

ساكن فراغ sifive_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_ڀريل)
		;

	سيٽ_reg(UART_REG_TXFIFO, ch);
}

ساكن صحيح sifive_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	إذا (!(ret & UART_RXFIFO_خالي_جايون))
		أرجع ret & UART_RXFIFO_معلومات;

	أرجع -1;
}

ساكن بنية sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

صحيح sifive_uart_init(بنانشان ڊگهو بنياد_2, u32 اندريون_freq, u32 baudrate)
{
	uart_بنياد     = (متطاير حرف *)بنياد_2;
	uart_اندريون_freq  = اندريون_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	إذا (اندريون_freq && baudrate)
		سيٽ_reg(UART_REG_DIV, uart_min_clk_ورهائيندڙ(اندريون_freq, baudrate));

	/* Disable interrupts */
	سيٽ_reg(UART_REG_IE, 0);

	/* Enable TX */
	سيٽ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	سيٽ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_سيٽ_device(&sifive_console);

	أرجع sbi_domain_پاڙ_جوڙيو_memrange(بنياد_2, صفحو_سائيز, صفحو_سائيز,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
