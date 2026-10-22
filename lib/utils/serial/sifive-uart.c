/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#నిర్వచించు UART_REG_TXFIFO		0
#నిర్వచించు UART_REG_RXFIFO		1
#నిర్వచించు UART_REG_TXCTRL		2
#నిర్వచించు UART_REG_RXCTRL		3
#నిర్వచించు UART_REG_IE		4
#నిర్వచించు UART_REG_IP		5
#నిర్వచించు UART_REG_DIV		6

#నిర్వచించు UART_TXFIFO_నిండిన	0x80000000
#నిర్వచించు UART_RXFIFO_ఖాళీలు	0x80000000
#నిర్వచించు UART_RXFIFO_దత్తాంశం	0x000000ff
#నిర్వచించు UART_TXCTRL_TXEN	0x1
#నిర్వచించు UART_RXCTRL_RXEN	0x1

/* clang-format on */

స్థిర అస్థిర అక్షరం *uart_పునాది;
స్థిర u32 uart_లోపల_freq;
స్థిర u32 uart_baudrate;

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

స్థిర u32 get_reg(u32 అంకె)
{
	తిరిగి readl(uart_పునాది + (అంకె * 0x4));
}

స్థిర శూన్య సమితి_reg(u32 అంకె, u32 val)
{
	writel(val, uart_పునాది + (అంకె * 0x4));
}

స్థిర శూన్య sifive_uart_putc(అక్షరం ch)
{
	అయితే (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_నిండిన)
		;

	సమితి_reg(UART_REG_TXFIFO, ch);
}

స్థిర పూర్ణాంకం sifive_uart_getc(శూన్య)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	ఒకవేళ (!(ret & UART_RXFIFO_ఖాళీలు))
		తిరిగి ret & UART_RXFIFO_దత్తాంశం;

	తిరిగి -1;
}

స్థిర నిర్మాణం sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

పూర్ణాంకం sifive_uart_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate)
{
	uart_పునాది     = (అస్థిర అక్షరం *)పునాది_2;
	uart_లోపల_freq  = లోపల_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	ఒకవేళ (లోపల_freq && baudrate)
		సమితి_reg(UART_REG_DIV, uart_min_clk_భాజకం(లోపల_freq, baudrate));

	/* Disable interrupts */
	సమితి_reg(UART_REG_IE, 0);

	/* Enable TX */
	సమితి_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	సమితి_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_సమితి_device(&sifive_console);

	తిరిగి sbi_domain_మూలం_కూడు_memrange(పునాది_2, పుట_కొలత, పుట_కొలత,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
