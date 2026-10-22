/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#تعریف_کن UART_REG_CTRL		0x00
#تعریف_کن UART_REG_نما		0x04
#تعریف_کن UART_REG_IDR		0x0C
#تعریف_کن UART_REG_BRGR		0x18
#تعریف_کن UART_REG_CSR		0x2C
#تعریف_کن UART_REG_RFIFO_TFIFO	0x30
#تعریف_کن UART_REG_BDIVR		0x34

#تعریف_کن UART_CTRL_RXRES		0x00000001
#تعریف_کن UART_CTRL_TXRES		0x00000002
#تعریف_کن UART_CTRL_RXEN		0x00000004
#تعریف_کن UART_CTRL_RXDIS		0x00000008
#تعریف_کن UART_CTRL_TXEN		0x00000010
#تعریف_کن UART_CTRL_TXDIS		0x00000020

#تعریف_کن UART_نما_PAR_NONE	0x00000020	/* No parity set */

#تعریف_کن UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#تعریف_کن	UART_CSR_REMPTY		0x00000002
#تعریف_کن	UART_CSR_TFUL		0x00000010

/* clang-format on */

ساكن متطاير فراغ *uart_پایه;
ساكن u32 uart_درون_freq;
ساكن u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
 */
ساكن مضمن بیعلامت صحيح uart_min_clk_مقسوم_علیه(uint64_t درون_freq,
						uint64_t max_هدف_hz)
{
	uint64_t quotient = (درون_freq + max_هدف_hz - 1) / (max_هدف_hz);

	/* Avoid underflow */
	إذا (quotient == 0)
		أرجع 0;
	وإلا
		أرجع quotient - 1;
}

ساكن u32 get_reg(u32 جابه_جایی)
{
	أرجع readl(uart_پایه + جابه_جایی);
}

ساكن فراغ مجموعه_reg(u32 جابه_جایی, u32 val)
{
	writel(val, uart_پایه + جابه_جایی);
}

ساكن فراغ cadence_uart_putc(حرف ch)
{
	طالما (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	مجموعه_reg(UART_REG_RFIFO_TFIFO, ch);
}

ساكن صحيح cadence_uart_getc(فراغ)
{
	u32 ret = get_reg(UART_REG_CSR);

	إذا (!(ret & UART_CSR_REMPTY))
		أرجع get_reg(UART_REG_RFIFO_TFIFO);

	أرجع -1;
}

ساكن بنية sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

صحيح cadence_uart_init(بیعلامت دراز پایه_2, u32 درون_freq, u32 baudrate)
{
	uart_پایه     = (متطاير فراغ *)پایه_2;
	uart_درون_freq  = درون_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	مجموعه_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	مجموعه_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	إذا (درون_freq && baudrate) {
		مجموعه_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		مجموعه_reg(UART_REG_BDIVR,
			uart_min_clk_مقسوم_علیه(درون_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	مجموعه_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	مجموعه_reg(UART_REG_نما, UART_نما_PAR_NONE);

	sbi_console_مجموعه_device(&cadence_console);

	أرجع sbi_domain_ریشه_جمع_کن_memrange(پایه_2, صفحه_اندازه, صفحه_اندازه,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
