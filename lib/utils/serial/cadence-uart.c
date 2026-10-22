/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_CTRL		0x00
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_ಬಹುಲಕ		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_IDR		0x0C
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_BRGR		0x18
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_CSR		0x2C
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_RFIFO_TFIFO	0x30
#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG_BDIVR		0x34

#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_RXRES		0x00000001
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_TXRES		0x00000002
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_RXEN		0x00000004
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_RXDIS		0x00000008
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_TXEN		0x00000010
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CTRL_TXDIS		0x00000020

#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಬಹುಲಕ_PAR_NONE	0x00000020	/* No parity set */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#ವ್ಯಾಖ್ಯಾನಿಸು	UART_CSR_REMPTY		0x00000002
#ವ್ಯಾಖ್ಯಾನಿಸು	UART_CSR_TFUL		0x00000010

/* clang-format on */

ಸ್ಥಿರ ಅಸ್ಥಿರ ಶೂನ್ಯ *uart_ತಳ;
ಸ್ಥಿರ u32 uart_ಒಳ_freq;
ಸ್ಥಿರ u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
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

ಸ್ಥಿರ u32 get_reg(u32 ಸ್ಥಾನಾಂತರ)
{
	ಹಿಂತಿರುಗಿ readl(uart_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(u32 ಸ್ಥಾನಾಂತರ, u32 val)
{
	writel(val, uart_ತಳ + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ cadence_uart_putc(ಅಕ್ಷರ ch)
{
	ಆಗ (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	ಗಣ_reg(UART_REG_RFIFO_TFIFO, ch);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cadence_uart_getc(ಶೂನ್ಯ)
{
	u32 ret = get_reg(UART_REG_CSR);

	ಆದರೆ (!(ret & UART_CSR_REMPTY))
		ಹಿಂತಿರುಗಿ get_reg(UART_REG_RFIFO_TFIFO);

	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

ಪೂರ್ಣಾಂಕ cadence_uart_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate)
{
	uart_ತಳ     = (ಅಸ್ಥಿರ ಶೂನ್ಯ *)ತಳ_2;
	uart_ಒಳ_freq  = ಒಳ_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	ಗಣ_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	ಗಣ_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	ಆದರೆ (ಒಳ_freq && baudrate) {
		ಗಣ_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		ಗಣ_reg(UART_REG_BDIVR,
			uart_min_clk_ಭಾಜಕ(ಒಳ_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	ಗಣ_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	ಗಣ_reg(UART_REG_ಬಹುಲಕ, UART_ಬಹುಲಕ_PAR_NONE);

	sbi_console_ಗಣ_device(&cadence_console);

	ಹಿಂತಿರುಗಿ sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(ತಳ_2, ಪುಟ_ಗಾತ್ರ, ಪುಟ_ಗಾತ್ರ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
