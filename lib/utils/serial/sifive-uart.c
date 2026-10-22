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

#تعریف_کرو UART_REG_TXFIFO		0
#تعریف_کرو UART_REG_RXFIFO		1
#تعریف_کرو UART_REG_TXCTRL		2
#تعریف_کرو UART_REG_RXCTRL		3
#تعریف_کرو UART_REG_IE		4
#تعریف_کرو UART_REG_IP		5
#تعریف_کرو UART_REG_DIV		6

#تعریف_کرو UART_TXFIFO_بھریا	0x80000000
#تعریف_کرو UART_RXFIFO_خالی_تھاواں	0x80000000
#تعریف_کرو UART_RXFIFO_انکڑے	0x000000ff
#تعریف_کرو UART_TXCTRL_TXEN	0x1
#تعریف_کرو UART_RXCTRL_RXEN	0x1

/* clang-format on */

ساكن متطاير حرف *uart_ادھار;
ساكن u32 uart_وچ_freq;
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
ساكن مضمن بنانشانی صحيح uart_min_clk_مقسوم_علیہ(uint64_t وچ_freq,
						uint64_t max_نشانہ_hz)
{
	uint64_t quotient = (وچ_freq + max_نشانہ_hz - 1) / (max_نشانہ_hz);

	/* Avoid underflow */
	إذا (quotient == 0)
		أرجع 0;
	وإلا
		أرجع quotient - 1;
}

ساكن u32 get_reg(u32 عدد)
{
	أرجع readl(uart_ادھار + (عدد * 0x4));
}

ساكن فراغ سیٹ_reg(u32 عدد, u32 val)
{
	writel(val, uart_ادھار + (عدد * 0x4));
}

ساكن فراغ sifive_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_بھریا)
		;

	سیٹ_reg(UART_REG_TXFIFO, ch);
}

ساكن صحيح sifive_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	إذا (!(ret & UART_RXFIFO_خالی_تھاواں))
		أرجع ret & UART_RXFIFO_انکڑے;

	أرجع -1;
}

ساكن بنية sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

صحيح sifive_uart_init(بنانشانی لما ادھار_2, u32 وچ_freq, u32 baudrate)
{
	uart_ادھار     = (متطاير حرف *)ادھار_2;
	uart_وچ_freq  = وچ_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	إذا (وچ_freq && baudrate)
		سیٹ_reg(UART_REG_DIV, uart_min_clk_مقسوم_علیہ(وچ_freq, baudrate));

	/* Disable interrupts */
	سیٹ_reg(UART_REG_IE, 0);

	/* Enable TX */
	سیٹ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	سیٹ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_سیٹ_device(&sifive_console);

	أرجع sbi_domain_جڑ_جوڑو_memrange(ادھار_2, ورقہ_ناپ, ورقہ_ناپ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
