/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/uart8250.h>

/* clang-format off */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RBR_ਵਿੱਥ		0	/* In:  Receive Buffer Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_THR_ਵਿੱਥ		0	/* Out: Transmitter Holding Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_DLL_ਵਿੱਥ		0	/* Out: Divisor Latch Low */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IER_ਵਿੱਥ		1	/* I/O: Interrupt Enable Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_DLM_ਵਿੱਥ		1	/* Out: Divisor Latch High */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_FCR_ਵਿੱਥ		2	/* Out: FIFO Control Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IIR_ਵਿੱਥ		2	/* I/O: Interrupt Identification Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LCR_ਵਿੱਥ		3	/* Out: Line Control Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_MCR_ਵਿੱਥ		4	/* Out: Modem Control Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_ਵਿੱਥ		5	/* In:  Line Status Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_MSR_ਵਿੱਥ		6	/* In:  Modem Status Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_SCR_ਵਿੱਥ		7	/* I/O: Scratch Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_MDR1_ਵਿੱਥ	8	/* I/O:  Mode Register */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_FIFOE		0x80	/* Fifo error */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_TEMT		0x40	/* Transmitter empty */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_BI		0x10	/* Break interrupt indicator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_FE		0x08	/* Frame error indicator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_PE		0x04	/* Parity error indicator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_OE		0x02	/* Overrun error indicator */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_DR		0x01	/* Receiver data ready */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_LSR_BRK_ERROR_ਬਿੱਟ	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IER_DMAE		0x80	/* DMA Requests Enable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IER_UUE		0x40	/* UART Unit Enable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IER_NRZE		0x20	/* NRZ coding Enable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ਸਥਿਰ ਬਣਤਰ uart8250_device uart8250_dev;

ਸਥਿਰ u32 get_reg(ਬਣਤਰ uart8250_device *dev, u32 ਅੰਕ)
{
	u32 ਵਿੱਥ = ਅੰਕ << dev->reg_ਖਿਸਕਾਓ;

	ਜੇ (dev->reg_ਚੌੜਾਈ == 1)
		ਵਾਪਸ readb(dev->ਅਧਾਰ_2 + ਵਿੱਥ);
	ਨਹੀਂਤਾਂ ਜੇ (dev->reg_ਚੌੜਾਈ == 2)
		ਵਾਪਸ readw(dev->ਅਧਾਰ_2 + ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ readl(dev->ਅਧਾਰ_2 + ਵਿੱਥ);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_reg(ਬਣਤਰ uart8250_device *dev, u32 ਅੰਕ, u32 val)
{
	u32 ਵਿੱਥ = ਅੰਕ << dev->reg_ਖਿਸਕਾਓ;

	ਜੇ (dev->reg_ਚੌੜਾਈ == 1)
		writeb(val, dev->ਅਧਾਰ_2 + ਵਿੱਥ);
	ਨਹੀਂਤਾਂ ਜੇ (dev->reg_ਚੌੜਾਈ == 2)
		writew(val, dev->ਅਧਾਰ_2 + ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		writel(val, dev->ਅਧਾਰ_2 + ਵਿੱਥ);
}

ਖਾਲੀ uart8250_device_putc(ਬਣਤਰ uart8250_device *dev, ਅੱਖਰ ch)
{
	ਜਦੋਂ ((get_reg(dev, UART_LSR_ਵਿੱਥ) & UART_LSR_THRE) == 0)
		;

	ਸੈੱਟ_reg(dev, UART_THR_ਵਿੱਥ, ch);
}

ਸਥਿਰ ਖਾਲੀ uart8250_putc(ਅੱਖਰ ch)
{
	ਵਾਪਸ uart8250_device_putc(&uart8250_dev, ch);
}

ਪੂਰਨ uart8250_device_getc(ਬਣਤਰ uart8250_device *dev)
{
	ਜੇ (get_reg(dev, UART_LSR_ਵਿੱਥ) & UART_LSR_DR)
		ਵਾਪਸ get_reg(dev, UART_RBR_ਵਿੱਥ);
	ਵਾਪਸ -1;
}

ਸਥਿਰ ਪੂਰਨ uart8250_getc(ਖਾਲੀ)
{
	ਵਾਪਸ uart8250_device_getc(&uart8250_dev);
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

ਖਾਲੀ uart8250_device_init(ਬਣਤਰ uart8250_device *dev, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2,
			  u32 ਵਿੱਚ_freq, u32 baudrate, u32 reg_ਖਿਸਕਾਓ,
			  u32 reg_ਚੌੜਾਈ, u32 reg_ਵਿੱਥ, u32 caps)
{
	u16 bdiv = 0;

	dev->ਅਧਾਰ_2      = (ਅਸਥਿਰ ਅੱਖਰ *)ਅਧਾਰ_2 + reg_ਵਿੱਥ;
	dev->reg_ਖਿਸਕਾਓ = reg_ਖਿਸਕਾਓ;
	dev->reg_ਚੌੜਾਈ = reg_ਚੌੜਾਈ;
	dev->ਵਿੱਚ_freq   = ਵਿੱਚ_freq;
	dev->baudrate  = baudrate;

	ਜੇ (dev->baudrate) {
		bdiv = (dev->ਵਿੱਚ_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	ਸੈੱਟ_reg(dev, UART_IER_ਵਿੱਥ, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	ਸੈੱਟ_reg(dev, UART_LCR_ਵਿੱਥ, 0x80);

	ਜੇ (bdiv) {
		/* Set divisor low byte */
		ਸੈੱਟ_reg(dev, UART_DLL_ਵਿੱਥ, bdiv & 0xff);
		/* Set divisor high byte */
		ਸੈੱਟ_reg(dev, UART_DLM_ਵਿੱਥ, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	ਸੈੱਟ_reg(dev, UART_LCR_ਵਿੱਥ, 0x03);
	/* Enable FIFO */
	ਸੈੱਟ_reg(dev, UART_FCR_ਵਿੱਥ, 0x01);
	/* No modem control DTR RTS */
	ਸੈੱਟ_reg(dev, UART_MCR_ਵਿੱਥ, 0x00);
	/* Clear line status and read receive buffer */
	ਜੇ (get_reg(dev, UART_LSR_ਵਿੱਥ) & UART_LSR_DR)
		get_reg(dev, UART_RBR_ਵਿੱਥ);
	/* Set scratchpad */
	ਸੈੱਟ_reg(dev, UART_SCR_ਵਿੱਥ, 0x00);
}

ਪੂਰਨ uart8250_init(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅਧਾਰ_2, u32 ਵਿੱਚ_freq, u32 baudrate, u32 reg_ਖਿਸਕਾਓ,
		  u32 reg_ਚੌੜਾਈ, u32 reg_ਵਿੱਥ, u32 caps)
{
	uart8250_device_init(&uart8250_dev, ਅਧਾਰ_2, ਵਿੱਚ_freq, baudrate,
			     reg_ਖਿਸਕਾਓ, reg_ਚੌੜਾਈ, reg_ਵਿੱਥ, caps);

	sbi_console_ਸੈੱਟ_device(&uart8250_console);

	ਵਾਪਸ sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(ਅਧਾਰ_2, ਪੰਨਾ_ਮਾਪ, ਪੰਨਾ_ਮਾਪ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
