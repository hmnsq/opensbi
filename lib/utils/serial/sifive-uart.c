/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi_utils/serial/sifive-uart.h>

/* clang-format off */

#परिभाषय UART_REG_TXFIFO		0
#परिभाषय UART_REG_RXFIFO		1
#परिभाषय UART_REG_TXCTRL		2
#परिभाषय UART_REG_RXCTRL		3
#परिभाषय UART_REG_IE		4
#परिभाषय UART_REG_IP		5
#परिभाषय UART_REG_DIV		6

#परिभाषय UART_TXFIFO_पूर्णम्	0x80000000
#परिभाषय UART_RXFIFO_रिक्तानि	0x80000000
#परिभाषय UART_RXFIFO_दत्त	0x000000ff
#परिभाषय UART_TXCTRL_TXEN	0x1
#परिभाषय UART_RXCTRL_RXEN	0x1

/* clang-format on */

स्थिर चञ्चल वर्ण *uart_आधारः;
स्थिर u32 uart_अन्तर्_freq;
स्थिर u32 uart_baudrate;

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

स्थिर u32 get_reg(u32 अङ्कः)
{
	प्रत्यागम readl(uart_आधारः + (अङ्कः * 0x4));
}

स्थिर शून्य समुच्चयः_reg(u32 अङ्कः, u32 val)
{
	writel(val, uart_आधारः + (अङ्कः * 0x4));
}

स्थिर शून्य sifive_uart_putc(वर्ण ch)
{
	यावत् (get_reg(UART_REG_TXFIFO) & UART_TXFIFO_पूर्णम्)
		;

	समुच्चयः_reg(UART_REG_TXFIFO, ch);
}

स्थिर पूर्णाङ्क sifive_uart_getc(शून्य)
{
	u32 ret = get_reg(UART_REG_RXFIFO);

	यदि (!(ret & UART_RXFIFO_रिक्तानि))
		प्रत्यागम ret & UART_RXFIFO_दत्त;

	प्रत्यागम -1;
}

स्थिर संरचना sbi_console_device sifive_console = {
	.name = "sifive_uart",
	.console_putc = sifive_uart_putc,
	.console_getc = sifive_uart_getc
};

पूर्णाङ्क sifive_uart_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate)
{
	uart_आधारः     = (चञ्चल वर्ण *)आधारः_2;
	uart_अन्तर्_freq  = अन्तर्_freq;
	uart_baudrate = baudrate;

	/* Configure baudrate */
	यदि (अन्तर्_freq && baudrate)
		समुच्चयः_reg(UART_REG_DIV, uart_min_clk_भाजकः(अन्तर्_freq, baudrate));

	/* Disable interrupts */
	समुच्चयः_reg(UART_REG_IE, 0);

	/* Enable TX */
	समुच्चयः_reg(UART_REG_TXCTRL, UART_TXCTRL_TXEN);

	/* Enable Rx */
	समुच्चयः_reg(UART_REG_RXCTRL, UART_RXCTRL_RXEN);

	sbi_console_समुच्चयः_device(&sifive_console);

	प्रत्यागम sbi_domain_मूलम्_सङ्कलय_memrange(आधारः_2, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
