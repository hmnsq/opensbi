/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi_utils/serial/uart8250.h>

/* clang-format off */

#وصف_ڪريو UART_RBR_وٿي		0	/* In:  Receive Buffer Register */
#وصف_ڪريو UART_THR_وٿي		0	/* Out: Transmitter Holding Register */
#وصف_ڪريو UART_DLL_وٿي		0	/* Out: Divisor Latch Low */
#وصف_ڪريو UART_IER_وٿي		1	/* I/O: Interrupt Enable Register */
#وصف_ڪريو UART_DLM_وٿي		1	/* Out: Divisor Latch High */
#وصف_ڪريو UART_FCR_وٿي		2	/* Out: FIFO Control Register */
#وصف_ڪريو UART_IIR_وٿي		2	/* I/O: Interrupt Identification Register */
#وصف_ڪريو UART_LCR_وٿي		3	/* Out: Line Control Register */
#وصف_ڪريو UART_MCR_وٿي		4	/* Out: Modem Control Register */
#وصف_ڪريو UART_LSR_وٿي		5	/* In:  Line Status Register */
#وصف_ڪريو UART_MSR_وٿي		6	/* In:  Modem Status Register */
#وصف_ڪريو UART_SCR_وٿي		7	/* I/O: Scratch Register */
#وصف_ڪريو UART_MDR1_وٿي	8	/* I/O:  Mode Register */

#وصف_ڪريو UART_LSR_FIFOE		0x80	/* Fifo error */
#وصف_ڪريو UART_LSR_TEMT		0x40	/* Transmitter empty */
#وصف_ڪريو UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#وصف_ڪريو UART_LSR_BI		0x10	/* Break interrupt indicator */
#وصف_ڪريو UART_LSR_FE		0x08	/* Frame error indicator */
#وصف_ڪريو UART_LSR_PE		0x04	/* Parity error indicator */
#وصف_ڪريو UART_LSR_OE		0x02	/* Overrun error indicator */
#وصف_ڪريو UART_LSR_DR		0x01	/* Receiver data ready */
#وصف_ڪريو UART_LSR_BRK_ERROR_بٽ	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#وصف_ڪريو UART_IER_DMAE		0x80	/* DMA Requests Enable */
#وصف_ڪريو UART_IER_UUE		0x40	/* UART Unit Enable */
#وصف_ڪريو UART_IER_NRZE		0x20	/* NRZ coding Enable */
#وصف_ڪريو UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ساكن بنية uart8250_device uart8250_dev;

ساكن u32 get_reg(بنية uart8250_device *dev, u32 انگ)
{
	u32 وٿي = انگ << dev->reg_سرڪايو;

	إذا (dev->reg_ويڪر == 1)
		أرجع readb(dev->بنياد_2 + وٿي);
	وإلا إذا (dev->reg_ويڪر == 2)
		أرجع readw(dev->بنياد_2 + وٿي);
	وإلا
		أرجع readl(dev->بنياد_2 + وٿي);
}

ساكن فراغ سيٽ_reg(بنية uart8250_device *dev, u32 انگ, u32 val)
{
	u32 وٿي = انگ << dev->reg_سرڪايو;

	إذا (dev->reg_ويڪر == 1)
		writeb(val, dev->بنياد_2 + وٿي);
	وإلا إذا (dev->reg_ويڪر == 2)
		writew(val, dev->بنياد_2 + وٿي);
	وإلا
		writel(val, dev->بنياد_2 + وٿي);
}

فراغ uart8250_device_putc(بنية uart8250_device *dev, حرف ch)
{
	طالما ((get_reg(dev, UART_LSR_وٿي) & UART_LSR_THRE) == 0)
		;

	سيٽ_reg(dev, UART_THR_وٿي, ch);
}

ساكن فراغ uart8250_putc(حرف ch)
{
	أرجع uart8250_device_putc(&uart8250_dev, ch);
}

صحيح uart8250_device_getc(بنية uart8250_device *dev)
{
	إذا (get_reg(dev, UART_LSR_وٿي) & UART_LSR_DR)
		أرجع get_reg(dev, UART_RBR_وٿي);
	أرجع -1;
}

ساكن صحيح uart8250_getc(فراغ)
{
	أرجع uart8250_device_getc(&uart8250_dev);
}

ساكن بنية sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

فراغ uart8250_device_init(بنية uart8250_device *dev, بنانشان ڊگهو بنياد_2,
			  u32 اندريون_freq, u32 baudrate, u32 reg_سرڪايو,
			  u32 reg_ويڪر, u32 reg_وٿي, u32 caps)
{
	u16 bdiv = 0;

	dev->بنياد_2      = (متطاير حرف *)بنياد_2 + reg_وٿي;
	dev->reg_سرڪايو = reg_سرڪايو;
	dev->reg_ويڪر = reg_ويڪر;
	dev->اندريون_freq   = اندريون_freq;
	dev->baudrate  = baudrate;

	إذا (dev->baudrate) {
		bdiv = (dev->اندريون_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	سيٽ_reg(dev, UART_IER_وٿي, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	سيٽ_reg(dev, UART_LCR_وٿي, 0x80);

	إذا (bdiv) {
		/* Set divisor low byte */
		سيٽ_reg(dev, UART_DLL_وٿي, bdiv & 0xff);
		/* Set divisor high byte */
		سيٽ_reg(dev, UART_DLM_وٿي, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	سيٽ_reg(dev, UART_LCR_وٿي, 0x03);
	/* Enable FIFO */
	سيٽ_reg(dev, UART_FCR_وٿي, 0x01);
	/* No modem control DTR RTS */
	سيٽ_reg(dev, UART_MCR_وٿي, 0x00);
	/* Clear line status and read receive buffer */
	إذا (get_reg(dev, UART_LSR_وٿي) & UART_LSR_DR)
		get_reg(dev, UART_RBR_وٿي);
	/* Set scratchpad */
	سيٽ_reg(dev, UART_SCR_وٿي, 0x00);
}

صحيح uart8250_init(بنانشان ڊگهو بنياد_2, u32 اندريون_freq, u32 baudrate, u32 reg_سرڪايو,
		  u32 reg_ويڪر, u32 reg_وٿي, u32 caps)
{
	uart8250_device_init(&uart8250_dev, بنياد_2, اندريون_freq, baudrate,
			     reg_سرڪايو, reg_ويڪر, reg_وٿي, caps);

	sbi_console_سيٽ_device(&uart8250_console);

	أرجع sbi_domain_پاڙ_جوڙيو_memrange(بنياد_2, صفحو_سائيز, صفحو_سائيز,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
