/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi_utils/serial/uart8250.h>

/* clang-format off */

#define UART_RBR_OFFSET		0	/* In:  Receive Buffer Register */
#define UART_THR_OFFSET		0	/* Out: Transmitter Holding Register */
#define UART_DLL_OFFSET		0	/* Out: Divisor Latch Low */
#define UART_IER_OFFSET		1	/* I/O: Interrupt Enable Register */
#define UART_DLM_OFFSET		1	/* Out: Divisor Latch High */
#define UART_FCR_OFFSET		2	/* Out: FIFO Control Register */
#define UART_IIR_OFFSET		2	/* I/O: Interrupt Identification Register */
#define UART_LCR_OFFSET		3	/* Out: Line Control Register */
#define UART_MCR_OFFSET		4	/* Out: Modem Control Register */
#define UART_LSR_OFFSET		5	/* In:  Line Status Register */
#define UART_MSR_OFFSET		6	/* In:  Modem Status Register */
#define UART_SCR_OFFSET		7	/* I/O: Scratch Register */
#define UART_MDR1_OFFSET	8	/* I/O:  Mode Register */

#define UART_LSR_FIFOE		0x80	/* Fifo error */
#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#define UART_LSR_BI		0x10	/* Break interrupt indicator */
#define UART_LSR_FE		0x08	/* Frame error indicator */
#define UART_LSR_PE		0x04	/* Parity error indicator */
#define UART_LSR_OE		0x02	/* Overrun error indicator */
#define UART_LSR_DR		0x01	/* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#define UART_IER_DMAE		0x80	/* DMA Requests Enable */
#define UART_IER_UUE		0x40	/* UART Unit Enable */
#define UART_IER_NRZE		0x20	/* NRZ coding Enable */
#define UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

സ്ഥിതം ഘടന uart8250_device uart8250_dev;

സ്ഥിതം u32 get_reg(ഘടന uart8250_device *dev, u32 അക്കം)
{
	u32 offset = അക്കം << dev->reg_shift;

	എങ്കിൽ (dev->reg_width == 1)
		തിരികെ readb(dev->base + offset);
	അല്ലെങ്കിൽ എങ്കിൽ (dev->reg_width == 2)
		തിരികെ readw(dev->base + offset);
	അല്ലെങ്കിൽ
		തിരികെ readl(dev->base + offset);
}

സ്ഥിതം ശൂന്യം set_reg(ഘടന uart8250_device *dev, u32 അക്കം, u32 val)
{
	u32 offset = അക്കം << dev->reg_shift;

	എങ്കിൽ (dev->reg_width == 1)
		writeb(val, dev->base + offset);
	അല്ലെങ്കിൽ എങ്കിൽ (dev->reg_width == 2)
		writew(val, dev->base + offset);
	അല്ലെങ്കിൽ
		writel(val, dev->base + offset);
}

ശൂന്യം uart8250_device_putc(ഘടന uart8250_device *dev, അക്ഷരം ch)
{
	ആകുമ്പോൾ ((get_reg(dev, UART_LSR_OFFSET) & UART_LSR_THRE) == 0)
		;

	set_reg(dev, UART_THR_OFFSET, ch);
}

സ്ഥിതം ശൂന്യം uart8250_putc(അക്ഷരം ch)
{
	തിരികെ uart8250_device_putc(&uart8250_dev, ch);
}

പൂർണ്ണസംഖ്യ uart8250_device_getc(ഘടന uart8250_device *dev)
{
	എങ്കിൽ (get_reg(dev, UART_LSR_OFFSET) & UART_LSR_DR)
		തിരികെ get_reg(dev, UART_RBR_OFFSET);
	തിരികെ -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ uart8250_getc(ശൂന്യം)
{
	തിരികെ uart8250_device_getc(&uart8250_dev);
}

സ്ഥിതം ഘടന sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

ശൂന്യം uart8250_device_init(ഘടന uart8250_device *dev, unsigned long base,
			  u32 in_freq, u32 baudrate, u32 reg_shift,
			  u32 reg_width, u32 reg_offset, u32 caps)
{
	u16 bdiv = 0;

	dev->base      = (അസ്ഥിര അക്ഷരം *)base + reg_offset;
	dev->reg_shift = reg_shift;
	dev->reg_width = reg_width;
	dev->in_freq   = in_freq;
	dev->baudrate  = baudrate;

	എങ്കിൽ (dev->baudrate) {
		bdiv = (dev->in_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	set_reg(dev, UART_IER_OFFSET, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	set_reg(dev, UART_LCR_OFFSET, 0x80);

	എങ്കിൽ (bdiv) {
		/* Set divisor low byte */
		set_reg(dev, UART_DLL_OFFSET, bdiv & 0xff);
		/* Set divisor high byte */
		set_reg(dev, UART_DLM_OFFSET, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	set_reg(dev, UART_LCR_OFFSET, 0x03);
	/* Enable FIFO */
	set_reg(dev, UART_FCR_OFFSET, 0x01);
	/* No modem control DTR RTS */
	set_reg(dev, UART_MCR_OFFSET, 0x00);
	/* Clear line status and read receive buffer */
	എങ്കിൽ (get_reg(dev, UART_LSR_OFFSET) & UART_LSR_DR)
		get_reg(dev, UART_RBR_OFFSET);
	/* Set scratchpad */
	set_reg(dev, UART_SCR_OFFSET, 0x00);
}

പൂർണ്ണസംഖ്യ uart8250_init(unsigned long base, u32 in_freq, u32 baudrate, u32 reg_shift,
		  u32 reg_width, u32 reg_offset, u32 caps)
{
	uart8250_device_init(&uart8250_dev, base, in_freq, baudrate,
			     reg_shift, reg_width, reg_offset, caps);

	sbi_console_set_device(&uart8250_console);

	തിരികെ sbi_domain_വേര്_add_memrange(base, PAGE_SIZE, PAGE_SIZE,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
