/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_TXFIFO		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_RXFIFO		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_TXCTRL		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_RXCTRL		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_IE		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_IP		5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_DIV		6

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TXFIFO_ਭਰਿਆ	0x80000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RXFIFO_ਖਾਲੀ_ਥਾਵਾਂ	0x80000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RXFIFO_ਅੰਕੜੇ	0x000000ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TXCTRL_TXEN	0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RXCTRL_RXEN	0x1

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਅੱਖਰ *uart_ਅਧਾਰ;
ਸਥਿਰ u32 uart_ਵਿੱਚ_freq;
ਸਥਿਰ u32 uart_baudrate;

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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uart_min_clk_ਭਾਜਕ(uint64_t ਵਿੱਚ_freq,
						uint64_t max_ਨਿਸ਼ਾਨਾ_hz)
{
	uint64_t quotient = (ਵਿੱਚ_freq + max_ਨਿਸ਼ਾਨਾ_hz - 1) / (max_ਨਿਸ਼ਾਨਾ_hz);

	/* Avoid underflow */
	ਜੇ (quotient == 0)
		ਵਾਪਸ 0;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ quotient - 1;
}

ਸਥਿਰ u32 get_reg(u32 ਅੰਕ)
{
	ਵਾਪਸ readl(uart_ਅਧਾਰ + (ਅੰਕ * 0x4));
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u32 ਅੰਕ, u32 val)
{
	writel(val, uart_ਅਧਾਰ + (ਅੰਕ * 0x4));
}

ਸਥਿਰ ਖਾਲੀ sifive_uart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_ਭਰਿਆ)
		;

	ਸੈੱਟ_reg(UART_REG_TXFIFO, ch);
}

ਸਥਿਰ ਪੂਰਨ sifive_uart_getc(ਖਾਲੀ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	ਜੇ (!(ret & UART_RXFIFO_ਖਾਲੀ_ਥਾਵਾਂ))
		ਵਾਪਸ ret & UART_RXFIFO_ਅੰਕੜੇ;

	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

ਪੂਰਨ sifive_uart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate)
{
	uart_ਅਧਾਰ     = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2;
	uart_ਵਿੱਚ_freq  = ਵਿੱਚ_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	ਜੇ (ਵਿੱਚ_freq && baudrate)
		ਸੈੱਟ_reg(UART_REG_DIV, uart_min_clk_ਭਾਜਕ(ਵਿੱਚ_freq, baudrate));

	/* Disable interrupts */
	ਸੈੱਟ_reg(UART_REG_IE, 0);

	/* Enable TX */
	ਸੈੱਟ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	ਸੈੱਟ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_ਸੈੱਟ_device(&sifive_console);

	ਵਾਪਸ sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(ਅਧਾਰ_2, ਪੰਨਾ_ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
