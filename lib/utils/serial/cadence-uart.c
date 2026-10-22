/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#నిర్వచించు UART_REG_CTRL		0x00
#నిర్వచించు UART_REG_బహుళకం		0x04
#నిర్వచించు UART_REG_IDR		0x0C
#నిర్వచించు UART_REG_BRGR		0x18
#నిర్వచించు UART_REG_CSR		0x2C
#నిర్వచించు UART_REG_RFIFO_TFIFO	0x30
#నిర్వచించు UART_REG_BDIVR		0x34

#నిర్వచించు UART_CTRL_RXRES		0x00000001
#నిర్వచించు UART_CTRL_TXRES		0x00000002
#నిర్వచించు UART_CTRL_RXEN		0x00000004
#నిర్వచించు UART_CTRL_RXDIS		0x00000008
#నిర్వచించు UART_CTRL_TXEN		0x00000010
#నిర్వచించు UART_CTRL_TXDIS		0x00000020

#నిర్వచించు UART_బహుళకం_PAR_NONE	0x00000020	/* No parity set */

#నిర్వచించు UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#నిర్వచించు	UART_CSR_REMPTY		0x00000002
#నిర్వచించు	UART_CSR_TFUL		0x00000010

/* clang-format on */

స్థిర అస్థిర శూన్య *uart_పునాది;
స్థిర u32 uart_లోపల_freq;
స్థిర u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
 */
స్థిర అంతఃవరుస గుర్తులేని పూర్ణాంకం uart_min_clk_భాజకం(uint64_t లోపల_freq,
						uint64_t max_లక్ష్యం_hz)
{
	uint64_t quotient = (లోపల_freq + max_లక్ష్యం_hz - 1) / (max_లక్ష్యం_hz);

	/* Avoid underflow */
	ఒకవేళ (quotient == 0)
		తిరిగి 0;
	లేకపోతే
		తిరిగి quotient - 1;
}

స్థిర u32 get_reg(u32 స్థానభ్రంశం)
{
	తిరిగి readl(uart_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య సమితి_reg(u32 స్థానభ్రంశం, u32 val)
{
	writel(val, uart_పునాది + స్థానభ్రంశం);
}

స్థిర శూన్య cadence_uart_putc(అక్షరం ch)
{
	అయితే (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	సమితి_reg(UART_REG_RFIFO_TFIFO, ch);
}

స్థిర పూర్ణాంకం cadence_uart_getc(శూన్య)
{
	u32 ret = get_reg(UART_REG_CSR);

	ఒకవేళ (!(ret & UART_CSR_REMPTY))
		తిరిగి get_reg(UART_REG_RFIFO_TFIFO);

	తిరిగి -1;
}

స్థిర నిర్మాణం sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

పూర్ణాంకం cadence_uart_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate)
{
	uart_పునాది     = (అస్థిర శూన్య *)పునాది_2;
	uart_లోపల_freq  = లోపల_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	సమితి_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	సమితి_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	ఒకవేళ (లోపల_freq && baudrate) {
		సమితి_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		సమితి_reg(UART_REG_BDIVR,
			uart_min_clk_భాజకం(లోపల_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	సమితి_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	సమితి_reg(UART_REG_బహుళకం, UART_బహుళకం_PAR_NONE);

	sbi_console_సమితి_device(&cadence_console);

	తిరిగి sbi_domain_మూలం_కూడు_memrange(పునాది_2, పుట_కొలత, పుట_కొలత,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
