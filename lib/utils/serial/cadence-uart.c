/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_CTRL		0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_ਬਹੁਲਕ		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_IDR		0x0C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_BRGR		0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_CSR		0x2C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_RFIFO_TFIFO	0x30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG_BDIVR		0x34

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_RXRES		0x00000001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_TXRES		0x00000002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_RXEN		0x00000004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_RXDIS		0x00000008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_TXEN		0x00000010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CTRL_TXDIS		0x00000020

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਬਹੁਲਕ_PAR_NONE	0x00000020	/* No parity set */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	UART_CSR_REMPTY		0x00000002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	UART_CSR_TFUL		0x00000010

/* clang-format on */

ਸਥਿਰ ਅਸਥਿਰ ਖਾਲੀ *uart_ਅਧਾਰ;
ਸਥਿਰ u32 uart_ਵਿੱਚ_freq;
ਸਥਿਰ u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
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

ਸਥਿਰ u32 get_reg(u32 ਵਿੱਥ)
{
	ਵਾਪਸ readl(uart_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(u32 ਵਿੱਥ, u32 val)
{
	writel(val, uart_ਅਧਾਰ + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ cadence_uart_putc(ਅੱਖਰ ch)
{
	ਜਦੋਂ (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	ਸੈੱਟ_reg(UART_REG_RFIFO_TFIFO, ch);
}

ਸਥਿਰ ਪੂਰਨ cadence_uart_getc(ਖਾਲੀ)
{
	u32 ret = get_reg(UART_REG_CSR);

	ਜੇ (!(ret & UART_CSR_REMPTY))
		ਵਾਪਸ get_reg(UART_REG_RFIFO_TFIFO);

	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

ਪੂਰਨ cadence_uart_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate)
{
	uart_ਅਧਾਰ     = (ਅਸਥਿਰ ਖਾਲੀ *)ਅਧਾਰ_2;
	uart_ਵਿੱਚ_freq  = ਵਿੱਚ_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	ਸੈੱਟ_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	ਸੈੱਟ_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	ਜੇ (ਵਿੱਚ_freq && baudrate) {
		ਸੈੱਟ_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		ਸੈੱਟ_reg(UART_REG_BDIVR,
			uart_min_clk_ਭਾਜਕ(ਵਿੱਚ_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	ਸੈੱਟ_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	ਸੈੱਟ_reg(UART_REG_ਬਹੁਲਕ, UART_ਬਹੁਲਕ_PAR_NONE);

	sbi_console_ਸੈੱਟ_device(&cadence_console);

	ਵਾਪਸ sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(ਅਧਾਰ_2, ਪੰਨਾ_ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
