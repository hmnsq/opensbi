/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#شامل <sbi/sbi_domain.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#تعریف_کرو UART_REG_CTRL		0x00
#تعریف_کرو UART_REG_منوال		0x04
#تعریف_کرو UART_REG_IDR		0x0C
#تعریف_کرو UART_REG_BRGR		0x18
#تعریف_کرو UART_REG_CSR		0x2C
#تعریف_کرو UART_REG_RFIFO_TFIFO	0x30
#تعریف_کرو UART_REG_BDIVR		0x34

#تعریف_کرو UART_CTRL_RXRES		0x00000001
#تعریف_کرو UART_CTRL_TXRES		0x00000002
#تعریف_کرو UART_CTRL_RXEN		0x00000004
#تعریف_کرو UART_CTRL_RXDIS		0x00000008
#تعریف_کرو UART_CTRL_TXEN		0x00000010
#تعریف_کرو UART_CTRL_TXDIS		0x00000020

#تعریف_کرو UART_منوال_PAR_NONE	0x00000020	/* No parity set */

#تعریف_کرو UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#تعریف_کرو	UART_CSR_REMPTY		0x00000002
#تعریف_کرو	UART_CSR_TFUL		0x00000010

/* clang-format on */

ساکن متلون خالی *uart_بنیاد;
ساکن u32 uart_اندر_freq;
ساکن u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
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

ساکن u32 get_reg(u32 انحراف)
{
	واپس readl(uart_بنیاد + انحراف);
}

ساکن خالی سیٹ_reg(u32 انحراف, u32 val)
{
	writel(val, uart_بنیاد + انحراف);
}

ساکن خالی cadence_uart_putc(حرف ch)
{
	جب (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	سیٹ_reg(UART_REG_RFIFO_TFIFO, ch);
}

ساکن صحیح cadence_uart_getc(خالی)
{
	u32 ret = get_reg(UART_REG_CSR);

	اگر (!(ret & UART_CSR_REMPTY))
		واپس get_reg(UART_REG_RFIFO_TFIFO);

	واپس -1;
}

ساکن ساخت sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

صحیح cadence_uart_init(غیرعلامتی طویل بنیاد_2, u32 اندر_freq, u32 baudrate)
{
	uart_بنیاد     = (متلون خالی *)بنیاد_2;
	uart_اندر_freq  = اندر_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	سیٹ_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	سیٹ_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	اگر (اندر_freq && baudrate) {
		سیٹ_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		سیٹ_reg(UART_REG_BDIVR,
			uart_min_clk_مقسوم_علیہ(اندر_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	سیٹ_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	سیٹ_reg(UART_REG_منوال, UART_منوال_PAR_NONE);

	sbi_console_سیٹ_device(&cadence_console);

	واپس sbi_domain_جڑ_جوڑو_memrange(بنیاد_2, صفحہ_سائز, صفحہ_سائز,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
