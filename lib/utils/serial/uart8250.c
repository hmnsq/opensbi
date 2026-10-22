/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/uart8250.h>

/* clang-format off */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_RBR_ಸ್ಥಾನಾಂತರ		0	/* In:  Receive Buffer Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_THR_ಸ್ಥಾನಾಂತರ		0	/* Out: Transmitter Holding Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_DLL_ಸ್ಥಾನಾಂತರ		0	/* Out: Divisor Latch Low */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IER_ಸ್ಥಾನಾಂತರ		1	/* I/O: Interrupt Enable Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_DLM_ಸ್ಥಾನಾಂತರ		1	/* Out: Divisor Latch High */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_FCR_ಸ್ಥಾನಾಂತರ		2	/* Out: FIFO Control Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IIR_ಸ್ಥಾನಾಂತರ		2	/* I/O: Interrupt Identification Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LCR_ಸ್ಥಾನಾಂತರ		3	/* Out: Line Control Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_MCR_ಸ್ಥಾನಾಂತರ		4	/* Out: Modem Control Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_ಸ್ಥಾನಾಂತರ		5	/* In:  Line Status Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_MSR_ಸ್ಥಾನಾಂತರ		6	/* In:  Modem Status Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_SCR_ಸ್ಥಾನಾಂತರ		7	/* I/O: Scratch Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_MDR1_ಸ್ಥಾನಾಂತರ	8	/* I/O:  Mode Register */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_FIFOE		0x80	/* Fifo error */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_TEMT		0x40	/* Transmitter empty */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_BI		0x10	/* Break interrupt indicator */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_FE		0x08	/* Frame error indicator */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_PE		0x04	/* Parity error indicator */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_OE		0x02	/* Overrun error indicator */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_DR		0x01	/* Receiver data ready */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_LSR_BRK_ERROR_ಬಿಟ್ಟುಗಳು	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IER_DMAE		0x80	/* DMA Requests Enable */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IER_UUE		0x40	/* UART Unit Enable */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IER_NRZE		0x20	/* NRZ coding Enable */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ಸ್ಥಿರ ರಚನೆ uart8250_device uart8250_dev;

ಸ್ಥಿರ u32 get_reg(ರಚನೆ uart8250_device *dev, u32 ಅಂಕೆ)
{
	u32 ಸ್ಥಾನಾಂತರ = ಅಂಕೆ << dev->reg_ಸರಿಸು;

	ಆದರೆ (dev->reg_ಅಗಲ == 1)
		ಹಿಂತಿರುಗಿ readb(dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dev->reg_ಅಗಲ == 2)
		ಹಿಂತಿರುಗಿ readw(dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ readl(dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_reg(ರಚನೆ uart8250_device *dev, u32 ಅಂಕೆ, u32 val)
{
	u32 ಸ್ಥಾನಾಂತರ = ಅಂಕೆ << dev->reg_ಸರಿಸು;

	ಆದರೆ (dev->reg_ಅಗಲ == 1)
		writeb(val, dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dev->reg_ಅಗಲ == 2)
		writew(val, dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		writel(val, dev->ತಳ_2 + ಸ್ಥಾನಾಂತರ);
}

ಶೂನ್ಯ uart8250_device_putc(ರಚನೆ uart8250_device *dev, ಅಕ್ಷರ ch)
{
	ಆಗ ((get_reg(dev, UART_LSR_ಸ್ಥಾನಾಂತರ) & UART_LSR_THRE) == 0)
		;

	ಗಣ_reg(dev, UART_THR_ಸ್ಥಾನಾಂತರ, ch);
}

ಸ್ಥಿರ ಶೂನ್ಯ uart8250_putc(ಅಕ್ಷರ ch)
{
	ಹಿಂತಿರುಗಿ uart8250_device_putc(&uart8250_dev, ch);
}

ಪೂರ್ಣಾಂಕ uart8250_device_getc(ರಚನೆ uart8250_device *dev)
{
	ಆದರೆ (get_reg(dev, UART_LSR_ಸ್ಥಾನಾಂತರ) & UART_LSR_DR)
		ಹಿಂತಿರುಗಿ get_reg(dev, UART_RBR_ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ uart8250_getc(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ uart8250_device_getc(&uart8250_dev);
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

ಶೂನ್ಯ uart8250_device_init(ರಚನೆ uart8250_device *dev, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2,
			  u32 ಒಳ_freq, u32 baudrate, u32 reg_ಸರಿಸು,
			  u32 reg_ಅಗಲ, u32 reg_ಸ್ಥಾನಾಂತರ, u32 caps)
{
	u16 bdiv = 0;

	dev->ತಳ_2      = (ಅಸ್ಥಿರ ಅಕ್ಷರ *)ತಳ_2 + reg_ಸ್ಥಾನಾಂತರ;
	dev->reg_ಸರಿಸು = reg_ಸರಿಸು;
	dev->reg_ಅಗಲ = reg_ಅಗಲ;
	dev->ಒಳ_freq   = ಒಳ_freq;
	dev->baudrate  = baudrate;

	ಆದರೆ (dev->baudrate) {
		bdiv = (dev->ಒಳ_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	ಗಣ_reg(dev, UART_IER_ಸ್ಥಾನಾಂತರ, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	ಗಣ_reg(dev, UART_LCR_ಸ್ಥಾನಾಂತರ, 0x80);

	ಆದರೆ (bdiv) {
		/* Set divisor low byte */
		ಗಣ_reg(dev, UART_DLL_ಸ್ಥಾನಾಂತರ, bdiv & 0xff);
		/* Set divisor high byte */
		ಗಣ_reg(dev, UART_DLM_ಸ್ಥಾನಾಂತರ, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	ಗಣ_reg(dev, UART_LCR_ಸ್ಥಾನಾಂತರ, 0x03);
	/* Enable FIFO */
	ಗಣ_reg(dev, UART_FCR_ಸ್ಥಾನಾಂತರ, 0x01);
	/* No modem control DTR RTS */
	ಗಣ_reg(dev, UART_MCR_ಸ್ಥಾನಾಂತರ, 0x00);
	/* Clear line status and read receive buffer */
	ಆದರೆ (get_reg(dev, UART_LSR_ಸ್ಥಾನಾಂತರ) & UART_LSR_DR)
		get_reg(dev, UART_RBR_ಸ್ಥಾನಾಂತರ);
	/* Set scratchpad */
	ಗಣ_reg(dev, UART_SCR_ಸ್ಥಾನಾಂತರ, 0x00);
}

ಪೂರ್ಣಾಂಕ uart8250_init(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ತಳ_2, u32 ಒಳ_freq, u32 baudrate, u32 reg_ಸರಿಸು,
		  u32 reg_ಅಗಲ, u32 reg_ಸ್ಥಾನಾಂತರ, u32 caps)
{
	uart8250_device_init(&uart8250_dev, ತಳ_2, ಒಳ_freq, baudrate,
			     reg_ಸರಿಸು, reg_ಅಗಲ, reg_ಸ್ಥಾನಾಂತರ, caps);

	sbi_console_ಗಣ_device(&uart8250_console);

	ಹಿಂತಿರುಗಿ sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(ತಳ_2, ಪುಟ_ಗಾತ್ರ, ಪುಟ_ಗಾತ್ರ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
