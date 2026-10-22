/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi_utils/serial/uart8250.h>

/* clang-format off */

#నిర్వచించు UART_RBR_స్థానభ్రంశం		0	/* In:  Receive Buffer Register */
#నిర్వచించు UART_THR_స్థానభ్రంశం		0	/* Out: Transmitter Holding Register */
#నిర్వచించు UART_DLL_స్థానభ్రంశం		0	/* Out: Divisor Latch Low */
#నిర్వచించు UART_IER_స్థానభ్రంశం		1	/* I/O: Interrupt Enable Register */
#నిర్వచించు UART_DLM_స్థానభ్రంశం		1	/* Out: Divisor Latch High */
#నిర్వచించు UART_FCR_స్థానభ్రంశం		2	/* Out: FIFO Control Register */
#నిర్వచించు UART_IIR_స్థానభ్రంశం		2	/* I/O: Interrupt Identification Register */
#నిర్వచించు UART_LCR_స్థానభ్రంశం		3	/* Out: Line Control Register */
#నిర్వచించు UART_MCR_స్థానభ్రంశం		4	/* Out: Modem Control Register */
#నిర్వచించు UART_LSR_స్థానభ్రంశం		5	/* In:  Line Status Register */
#నిర్వచించు UART_MSR_స్థానభ్రంశం		6	/* In:  Modem Status Register */
#నిర్వచించు UART_SCR_స్థానభ్రంశం		7	/* I/O: Scratch Register */
#నిర్వచించు UART_MDR1_స్థానభ్రంశం	8	/* I/O:  Mode Register */

#నిర్వచించు UART_LSR_FIFOE		0x80	/* Fifo error */
#నిర్వచించు UART_LSR_TEMT		0x40	/* Transmitter empty */
#నిర్వచించు UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#నిర్వచించు UART_LSR_BI		0x10	/* Break interrupt indicator */
#నిర్వచించు UART_LSR_FE		0x08	/* Frame error indicator */
#నిర్వచించు UART_LSR_PE		0x04	/* Parity error indicator */
#నిర్వచించు UART_LSR_OE		0x02	/* Overrun error indicator */
#నిర్వచించు UART_LSR_DR		0x01	/* Receiver data ready */
#నిర్వచించు UART_LSR_BRK_ERROR_బిట్లు	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#నిర్వచించు UART_IER_DMAE		0x80	/* DMA Requests Enable */
#నిర్వచించు UART_IER_UUE		0x40	/* UART Unit Enable */
#నిర్వచించు UART_IER_NRZE		0x20	/* NRZ coding Enable */
#నిర్వచించు UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

స్థిర నిర్మాణం uart8250_device uart8250_dev;

స్థిర u32 get_reg(నిర్మాణం uart8250_device *dev, u32 అంకె)
{
	u32 స్థానభ్రంశం = అంకె << dev->reg_జరుపు;

	ఒకవేళ (dev->reg_వెడల్పు == 1)
		తిరిగి readb(dev->పునాది_2 + స్థానభ్రంశం);
	లేకపోతే ఒకవేళ (dev->reg_వెడల్పు == 2)
		తిరిగి readw(dev->పునాది_2 + స్థానభ్రంశం);
	లేకపోతే
		తిరిగి readl(dev->పునాది_2 + స్థానభ్రంశం);
}

స్థిర శూన్య సమితి_reg(నిర్మాణం uart8250_device *dev, u32 అంకె, u32 val)
{
	u32 స్థానభ్రంశం = అంకె << dev->reg_జరుపు;

	ఒకవేళ (dev->reg_వెడల్పు == 1)
		writeb(val, dev->పునాది_2 + స్థానభ్రంశం);
	లేకపోతే ఒకవేళ (dev->reg_వెడల్పు == 2)
		writew(val, dev->పునాది_2 + స్థానభ్రంశం);
	లేకపోతే
		writel(val, dev->పునాది_2 + స్థానభ్రంశం);
}

శూన్య uart8250_device_putc(నిర్మాణం uart8250_device *dev, అక్షరం ch)
{
	అయితే ((get_reg(dev, UART_LSR_స్థానభ్రంశం) & UART_LSR_THRE) == 0)
		;

	సమితి_reg(dev, UART_THR_స్థానభ్రంశం, ch);
}

స్థిర శూన్య uart8250_putc(అక్షరం ch)
{
	తిరిగి uart8250_device_putc(&uart8250_dev, ch);
}

పూర్ణాంకం uart8250_device_getc(నిర్మాణం uart8250_device *dev)
{
	ఒకవేళ (get_reg(dev, UART_LSR_స్థానభ్రంశం) & UART_LSR_DR)
		తిరిగి get_reg(dev, UART_RBR_స్థానభ్రంశం);
	తిరిగి -1;
}

స్థిర పూర్ణాంకం uart8250_getc(శూన్య)
{
	తిరిగి uart8250_device_getc(&uart8250_dev);
}

స్థిర నిర్మాణం sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

శూన్య uart8250_device_init(నిర్మాణం uart8250_device *dev, గుర్తులేని దీర్ఘ పునాది_2,
			  u32 లోపల_freq, u32 baudrate, u32 reg_జరుపు,
			  u32 reg_వెడల్పు, u32 reg_స్థానభ్రంశం, u32 caps)
{
	u16 bdiv = 0;

	dev->పునాది_2      = (అస్థిర అక్షరం *)పునాది_2 + reg_స్థానభ్రంశం;
	dev->reg_జరుపు = reg_జరుపు;
	dev->reg_వెడల్పు = reg_వెడల్పు;
	dev->లోపల_freq   = లోపల_freq;
	dev->baudrate  = baudrate;

	ఒకవేళ (dev->baudrate) {
		bdiv = (dev->లోపల_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	సమితి_reg(dev, UART_IER_స్థానభ్రంశం, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	సమితి_reg(dev, UART_LCR_స్థానభ్రంశం, 0x80);

	ఒకవేళ (bdiv) {
		/* Set divisor low byte */
		సమితి_reg(dev, UART_DLL_స్థానభ్రంశం, bdiv & 0xff);
		/* Set divisor high byte */
		సమితి_reg(dev, UART_DLM_స్థానభ్రంశం, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	సమితి_reg(dev, UART_LCR_స్థానభ్రంశం, 0x03);
	/* Enable FIFO */
	సమితి_reg(dev, UART_FCR_స్థానభ్రంశం, 0x01);
	/* No modem control DTR RTS */
	సమితి_reg(dev, UART_MCR_స్థానభ్రంశం, 0x00);
	/* Clear line status and read receive buffer */
	ఒకవేళ (get_reg(dev, UART_LSR_స్థానభ్రంశం) & UART_LSR_DR)
		get_reg(dev, UART_RBR_స్థానభ్రంశం);
	/* Set scratchpad */
	సమితి_reg(dev, UART_SCR_స్థానభ్రంశం, 0x00);
}

పూర్ణాంకం uart8250_init(గుర్తులేని దీర్ఘ పునాది_2, u32 లోపల_freq, u32 baudrate, u32 reg_జరుపు,
		  u32 reg_వెడల్పు, u32 reg_స్థానభ్రంశం, u32 caps)
{
	uart8250_device_init(&uart8250_dev, పునాది_2, లోపల_freq, baudrate,
			     reg_జరుపు, reg_వెడల్పు, reg_స్థానభ్రంశం, caps);

	sbi_console_సమితి_device(&uart8250_console);

	తిరిగి sbi_domain_మూలం_కూడు_memrange(పునాది_2, పుట_కొలత, పుట_కొలత,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
