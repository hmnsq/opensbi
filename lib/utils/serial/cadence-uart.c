/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#define UART_REG_CTRL		0x00
#define UART_REG_MODE		0x04
#define UART_REG_IDR		0x0C
#define UART_REG_BRGR		0x18
#define UART_REG_CSR		0x2C
#define UART_REG_RFIFO_TFIFO	0x30
#define UART_REG_BDIVR		0x34

#define UART_CTRL_RXRES		0x00000001
#define UART_CTRL_TXRES		0x00000002
#define UART_CTRL_RXEN		0x00000004
#define UART_CTRL_RXDIS		0x00000008
#define UART_CTRL_TXEN		0x00000010
#define UART_CTRL_TXDIS		0x00000020

#define UART_MODE_PAR_NONE	0x00000020	/* No parity set */

#define UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#define	UART_CSR_REMPTY		0x00000002
#define	UART_CSR_TFUL		0x00000010

/* clang-format on */

நிலையான நிலையற்ற வெறுமை *uart_base;
நிலையான u32 uart_in_freq;
நிலையான u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
 */
நிலையான உள்வரி unsigned முழு uart_min_clk_வகுஎண்(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	எனில் (quotient == 0)
		திரும்பு 0;
	இல்லையெனில்
		திரும்பு quotient - 1;
}

நிலையான u32 get_reg(u32 offset)
{
	திரும்பு readl(uart_base + offset);
}

நிலையான வெறுமை set_reg(u32 offset, u32 val)
{
	writel(val, uart_base + offset);
}

நிலையான வெறுமை cadence_uart_putc(எழுத்து ch)
{
	போது (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	set_reg(UART_REG_RFIFO_TFIFO, ch);
}

நிலையான முழு cadence_uart_getc(வெறுமை)
{
	u32 ret = get_reg(UART_REG_CSR);

	எனில் (!(ret & UART_CSR_REMPTY))
		திரும்பு get_reg(UART_REG_RFIFO_TFIFO);

	திரும்பு -1;
}

நிலையான கட்டமைப்பு sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

முழு cadence_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	uart_base     = (நிலையற்ற வெறுமை *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	set_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	set_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	எனில் (in_freq && baudrate) {
		set_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		set_reg(UART_REG_BDIVR,
			uart_min_clk_வகுஎண்(in_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	set_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	set_reg(UART_REG_MODE, UART_MODE_PAR_NONE);

	sbi_console_set_device(&cadence_console);

	திரும்பு sbi_domain_வேர்_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
