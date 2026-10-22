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

#تعریف_کړه UART_REG_TXFIFO		0
#تعریف_کړه UART_REG_RXFIFO		1
#تعریف_کړه UART_REG_TXCTRL		2
#تعریف_کړه UART_REG_RXCTRL		3
#تعریف_کړه UART_REG_IE		4
#تعریف_کړه UART_REG_IP		5
#تعریف_کړه UART_REG_DIV		6

#تعریف_کړه UART_TXFIFO_ډک	0x80000000
#تعریف_کړه UART_RXFIFO_تشه	0x80000000
#تعریف_کړه UART_RXFIFO_معلومات	0x000000ff
#تعریف_کړه UART_TXCTRL_TXEN	0x1
#تعریف_کړه UART_RXCTRL_RXEN	0x1

/* clang-format on */

ساكن متطاير حرف *uart_بنسټ;
ساكن u32 uart_دننه_freq;
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
ساكن مضمن بینښه صحيح uart_min_clk_وېشونکی(uint64_t دننه_freq,
						uint64_t max_موخه_hz)
{
	uint64_t quotient = (دننه_freq + max_موخه_hz - 1) / (max_موخه_hz);

	/* Avoid underflow */
	إذا (quotient == 0)
		أرجع 0;
	وإلا
		أرجع quotient - 1;
}

ساكن u32 get_reg(u32 عدد)
{
	أرجع readl(uart_بنسټ + (عدد * 0x4));
}

ساكن فراغ سېټ_reg(u32 عدد, u32 val)
{
	writel(val, uart_بنسټ + (عدد * 0x4));
}

ساكن فراغ sifive_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_ډک)
		;

	سېټ_reg(UART_REG_TXFIFO, ch);
}

ساكن صحيح sifive_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	إذا (!(ret & UART_RXFIFO_تشه))
		أرجع ret & UART_RXFIFO_معلومات;

	أرجع -1;
}

ساكن بنية sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

صحيح sifive_uart_init(بینښه اوږد بنسټ_2, u32 دننه_freq, u32 baudrate)
{
	uart_بنسټ     = (متطاير حرف *)بنسټ_2;
	uart_دننه_freq  = دننه_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	إذا (دننه_freq && baudrate)
		سېټ_reg(UART_REG_DIV, uart_min_clk_وېشونکی(دننه_freq, baudrate));

	/* Disable interrupts */
	سېټ_reg(UART_REG_IE, 0);

	/* Enable TX */
	سېټ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	سېټ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_سېټ_device(&sifive_console);

	أرجع sbi_domain_ریښه_جمع_کړه_memrange(بنسټ_2, پاڼه_اندازه, پاڼه_اندازه,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
