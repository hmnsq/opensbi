/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 */

#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi_utils/serial/cadence-uart.h>

/* clang-format off */

#घोषणा UART_REG_CTRL		0x00
#घोषणा UART_REG_MODE		0x04
#घोषणा UART_REG_IDR		0x0C
#घोषणा UART_REG_BRGR		0x18
#घोषणा UART_REG_CSR		0x2C
#घोषणा UART_REG_RFIFO_TFIFO	0x30
#घोषणा UART_REG_BDIVR		0x34

#घोषणा UART_CTRL_RXRES		0x00000001
#घोषणा UART_CTRL_TXRES		0x00000002
#घोषणा UART_CTRL_RXEN		0x00000004
#घोषणा UART_CTRL_RXDIS		0x00000008
#घोषणा UART_CTRL_TXEN		0x00000010
#घोषणा UART_CTRL_TXDIS		0x00000020

#घोषणा UART_MODE_PAR_NONE	0x00000020	/* No parity set */

#घोषणा UART_BRGR_CD_CLKDIVISOR	0x00000001	/* baud_sample = sel_clk */

#घोषणा	UART_CSR_REMPTY		0x00000002
#घोषणा	UART_CSR_TFUL		0x00000010

/* clang-format on */

अटल अस्थिर व्योम *uart_base;
अटल u32 uart_in_freq;
अटल u32 uart_baudrate;

/*
 * Find minimum divisor divides in_freq to max_target_hz;
 * Based on SiFive UART driver (sifive-uart.c)
 */
अटल अंतरभूत अचिन्हित पूर्णांक uart_min_clk_भाजक(uint64_t in_freq,
						uint64_t max_target_hz)
{
	uint64_t quotient = (in_freq + max_target_hz - 1) / (max_target_hz);

	/* Avoid underflow */
	अगर (quotient == 0)
		वापस 0;
	अन्यथा
		वापस quotient - 1;
}

अटल u32 get_reg(u32 offset)
{
	वापस readl(uart_base + offset);
}

अटल व्योम set_reg(u32 offset, u32 val)
{
	writel(val, uart_base + offset);
}

अटल व्योम cadence_uart_putc(अक्षर ch)
{
	जबतक (get_reg(UART_REG_CSR) & UART_CSR_TFUL)
		;

	set_reg(UART_REG_RFIFO_TFIFO, ch);
}

अटल पूर्णांक cadence_uart_getc(व्योम)
{
	u32 ret = get_reg(UART_REG_CSR);

	अगर (!(ret & UART_CSR_REMPTY))
		वापस get_reg(UART_REG_RFIFO_TFIFO);

	वापस -1;
}

अटल काष्ठा sbi_console_device cadence_console = {
	.name = "cadence_uart",
	.console_putc = cadence_uart_putc,
	.console_getc = cadence_uart_getc
};

पूर्णांक cadence_uart_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate)
{
	uart_base     = (अस्थिर व्योम *)base;
	uart_in_freq  = in_freq;
	uart_baudrate = baudrate;

	/* Disable interrupts */
	set_reg(UART_REG_IDR, 0xFFFFFFFF);

	/* Disable TX RX */
	set_reg(UART_REG_CTRL, UART_CTRL_TXDIS | UART_CTRL_RXDIS);

	/* Configure baudrate */
	अगर (in_freq && baudrate) {
		set_reg(UART_REG_BRGR, UART_BRGR_CD_CLKDIVISOR);
		set_reg(UART_REG_BDIVR,
			uart_min_clk_भाजक(in_freq, baudrate));
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

	वापस sbi_domain_मूल_add_memrange(base, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
