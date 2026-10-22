/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#परिभाषय UART_REG_CTRL		0x00
#परिभाषय UART_REG_बहुलकम्		0x04
#परिभाषय UART_REG_IDR		0x0C
#परिभाषय UART_REG_BRGR		0x18
#परिभाषय UART_REG_CSR		0x2C
#परिभाषय UART_REG_RFIFO_TFIFO	0x30
#परिभाषय UART_REG_BDIVR		0x34

#परिभाषय UART_CTRL_RXRES		0x00000001
#परिभाषय UART_CTRL_TXRES		0x00000002
#परिभाषय UART_CTRL_RXEN		0x00000004
#परिभाषय UART_CTRL_RXDIS		0x00000008
#परिभाषय UART_CTRL_TXEN		0x00000010
#परिभाषय UART_CTRL_TXDIS		0x00000020

#परिभाषय UART_बहुलकम्_PAR_NONE	0x00000020	/* No parity set */

#परिभाषय UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#परिभाषय	UART_CSR_REMPTY		0x00000002
#परिभाषय	UART_CSR_TFUL		0x00000010

/* clang-format on */

स्थिर चञ्चल शून्य *uart_आधारः;
स्थिर u32 uart_अन्तर्_freq;
स्थिर u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
 */
स्थिर अन्तःस्थ अचिह्नित पूर्णाङ्क uart_min_clk_भाजकः(uint64_t अन्तर्_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (अन्तर्_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	यदि (quotient == 0)
		प्रत्यागम 0;
	अन्यथा
		प्रत्यागम quotient - 1;
}

स्थिर u32 get_reg(u32 विस्थापनम्)
{
	प्रत्यागम readl(uart_आधारः + विस्थापनम्);
}

स्थिर शून्य समुच्चयः_reg(u32 विस्थापनम्, u32 val)
{
	writel(val, uart_आधारः + विस्थापनम्);
}

स्थिर शून्य cadence_uart_putc(वर्ण ch)
{
	यावत् (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	समुच्चयः_reg(UART_REG_RFIFO_TFIFO, ch);
}

स्थिर पूर्णाङ्क cadence_uart_getc(शून्य)
{
	u32 ret = get_reg(UART_REG_CSR);

	यदि (!(ret & UART_CSR_REMPTY))
		प्रत्यागम get_reg(UART_REG_RFIFO_TFIFO);

	प्रत्यागम -1;
}

स्थिर संरचना sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

पूर्णाङ्क cadence_uart_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate)
{
	uart_आधारः     = (चञ्चल शून्य *)आधारः_2;
	uart_अन्तर्_freq  = अन्तर्_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	समुच्चयः_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	समुच्चयः_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	यदि (अन्तर्_freq && baudrate) {
		समुच्चयः_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		समुच्चयः_reg(UART_REG_BDIVR,
			uart_min_clk_भाजकः(अन्तर्_freq, baudrate));
	}

	/* Software reset TX RX data path and enable TX RX */
	समुच्चयः_reg(UART_REG_CTRL, UART_CTRL_TXRES | UART_CTRL_RXRES
		| UART_CTRL_TXEN | UART_CTRL_RXEN);

	/*
	 * Set:
	 * 1 stop bit, bits[07:06] = 0x00,
	 * no parity set, bits[05:03] = 0x100,
	 * 8 bits character length, bits[02:01] = 0x00,
	 * sel_clk = uart_clk, bit[0] = 0x0
	 */
	समुच्चयः_reg(UART_REG_बहुलकम्, UART_बहुलकम्_PAR_NONE);

	sbi_console_समुच्चयः_device(&cadence_console);

	प्रत्यागम sbi_domain_मूलम्_सङ्कलय_memrange(आधारः_2, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
