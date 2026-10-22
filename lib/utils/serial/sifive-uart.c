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

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_TXFIFO		0
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_RXFIFO		1
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_TXCTRL		2
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_RXCTRL		3
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_IE		4
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_IP		5
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_DIV		6

#ವ್ಯಾಖ್ಯಾನಿಸು UART_TXFIFO_ತುಂಬಿದ	0x80000000
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RXFIFO_ಖಾಲಿಗಳು	0x80000000
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RXFIFO_ದತ್ತಾಂಶ	0x000000ff
#ವ್ಯಾಖ್ಯಾನಿಸು UART_TXCTRL_TXEN	0x1
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RXCTRL_RXEN	0x1

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಅಕ್ಷರ *uart_ತಳ;
ಸ್ಥಿರ u32 uart_ಒಳ_freq;
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uart_min_clk_ಭಾಜಕ(uint64_t ಒಳ_freq,
						uint64_t max_ಗುರಿ_hz)
{
	uint64_t quotient = (ಒಳ_freq + max_ಗುರಿ_hz - 1) / (max_ಗುರಿ_hz);

	/* Avoid underflow */
	ಆದರೆ (quotient == 0)
		ಹಿಂತಿರುಗಿ 0;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ quotient - 1;
}

ಸ್ಥಿರ u32 get_reg(u32 ಅಂಕೆ)
{
	ಹಿಂತಿರುಗಿ readl(uart_ತಳ + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u32 ಅಂಕೆ, u32 val)
{
	writel(val, uart_ತಳ + (ಅಂಕೆ * 0x4));
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_uart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_ತುಂಬಿದ)
		;

	ಗಣ_reg(UART_REG_TXFIFO, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sifive_uart_getc(ಶೂನ್ಯ)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	ಆದರೆ (!(ret & UART_RXFIFO_ಖಾಲಿಗಳು))
		ಹಿಂತಿರುಗಿ ret & UART_RXFIFO_ದತ್ತಾಂಶ;

	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

ಪೂರ್ಣಾಂಕ sifive_uart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate)
{
	uart_ತಳ     = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2;
	uart_ಒಳ_freq  = ಒಳ_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	ಆದರೆ (ಒಳ_freq && baudrate)
		ಗಣ_reg(UART_REG_DIV, uart_min_clk_ಭಾಜಕ(ಒಳ_freq, baudrate));

	/* Disable interrupts */
	ಗಣ_reg(UART_REG_IE, 0);

	/* Enable TX */
	ಗಣ_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	ಗಣ_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_ಗಣ_device(&sifive_console);

	ಹಿಂತಿರುಗಿ sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(ತಳ_2, ಪುಟ_ಗಾತ್ರ, ಪುಟ_ಗಾತ್ರ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
