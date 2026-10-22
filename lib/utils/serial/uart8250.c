/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi_utils/serial/uart8250.h>

/* clang-format off */

#تعریف_کرو UART_RBR_انحراف		0	/* In:  Receive Buffer Register */
#تعریف_کرو UART_THR_انحراف		0	/* Out: Transmitter Holding Register */
#تعریف_کرو UART_DLL_انحراف		0	/* Out: Divisor Latch Low */
#تعریف_کرو UART_IER_انحراف		1	/* I/O: Interrupt Enable Register */
#تعریف_کرو UART_DLM_انحراف		1	/* Out: Divisor Latch High */
#تعریف_کرو UART_FCR_انحراف		2	/* Out: FIFO Control Register */
#تعریف_کرو UART_IIR_انحراف		2	/* I/O: Interrupt Identification Register */
#تعریف_کرو UART_LCR_انحراف		3	/* Out: Line Control Register */
#تعریف_کرو UART_MCR_انحراف		4	/* Out: Modem Control Register */
#تعریف_کرو UART_LSR_انحراف		5	/* In:  Line Status Register */
#تعریف_کرو UART_MSR_انحراف		6	/* In:  Modem Status Register */
#تعریف_کرو UART_SCR_انحراف		7	/* I/O: Scratch Register */
#تعریف_کرو UART_MDR1_انحراف	8	/* I/O:  Mode Register */

#تعریف_کرو UART_LSR_FIFOE		0x80	/* Fifo error */
#تعریف_کرو UART_LSR_TEMT		0x40	/* Transmitter empty */
#تعریف_کرو UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#تعریف_کرو UART_LSR_BI		0x10	/* Break interrupt indicator */
#تعریف_کرو UART_LSR_FE		0x08	/* Frame error indicator */
#تعریف_کرو UART_LSR_PE		0x04	/* Parity error indicator */
#تعریف_کرو UART_LSR_OE		0x02	/* Overrun error indicator */
#تعریف_کرو UART_LSR_DR		0x01	/* Receiver data ready */
#تعریف_کرو UART_LSR_BRK_ERROR_بٹ	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#تعریف_کرو UART_IER_DMAE		0x80	/* DMA Requests Enable */
#تعریف_کرو UART_IER_UUE		0x40	/* UART Unit Enable */
#تعریف_کرو UART_IER_NRZE		0x20	/* NRZ coding Enable */
#تعریف_کرو UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ساکن ساخت uart8250_device uart8250_dev;

ساکن u32 get_reg(ساخت uart8250_device *dev, u32 عدد)
{
	u32 انحراف = عدد << dev->reg_سرکاؤ;

	اگر (dev->reg_چوڑائی == 1)
		واپس readb(dev->بنیاد_2 + انحراف);
	ورنہ اگر (dev->reg_چوڑائی == 2)
		واپس readw(dev->بنیاد_2 + انحراف);
	ورنہ
		واپس readl(dev->بنیاد_2 + انحراف);
}

ساکن خالی سیٹ_reg(ساخت uart8250_device *dev, u32 عدد, u32 val)
{
	u32 انحراف = عدد << dev->reg_سرکاؤ;

	اگر (dev->reg_چوڑائی == 1)
		writeb(val, dev->بنیاد_2 + انحراف);
	ورنہ اگر (dev->reg_چوڑائی == 2)
		writew(val, dev->بنیاد_2 + انحراف);
	ورنہ
		writel(val, dev->بنیاد_2 + انحراف);
}

خالی uart8250_device_putc(ساخت uart8250_device *dev, حرف ch)
{
	جب ((get_reg(dev, UART_LSR_انحراف) & UART_LSR_THRE) == 0)
		;

	سیٹ_reg(dev, UART_THR_انحراف, ch);
}

ساکن خالی uart8250_putc(حرف ch)
{
	واپس uart8250_device_putc(&uart8250_dev, ch);
}

صحیح uart8250_device_getc(ساخت uart8250_device *dev)
{
	اگر (get_reg(dev, UART_LSR_انحراف) & UART_LSR_DR)
		واپس get_reg(dev, UART_RBR_انحراف);
	واپس -1;
}

ساکن صحیح uart8250_getc(خالی)
{
	واپس uart8250_device_getc(&uart8250_dev);
}

ساکن ساخت sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

خالی uart8250_device_init(ساخت uart8250_device *dev, غیرعلامتی طویل بنیاد_2,
			  u32 اندر_freq, u32 baudrate, u32 reg_سرکاؤ,
			  u32 reg_چوڑائی, u32 reg_انحراف, u32 caps)
{
	u16 bdiv = 0;

	dev->بنیاد_2      = (متلون حرف *)بنیاد_2 + reg_انحراف;
	dev->reg_سرکاؤ = reg_سرکاؤ;
	dev->reg_چوڑائی = reg_چوڑائی;
	dev->اندر_freq   = اندر_freq;
	dev->baudrate  = baudrate;

	اگر (dev->baudrate) {
		bdiv = (dev->اندر_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	سیٹ_reg(dev, UART_IER_انحراف, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	سیٹ_reg(dev, UART_LCR_انحراف, 0x80);

	اگر (bdiv) {
		/* Set divisor low byte */
		سیٹ_reg(dev, UART_DLL_انحراف, bdiv & 0xff);
		/* Set divisor high byte */
		سیٹ_reg(dev, UART_DLM_انحراف, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	سیٹ_reg(dev, UART_LCR_انحراف, 0x03);
	/* Enable FIFO */
	سیٹ_reg(dev, UART_FCR_انحراف, 0x01);
	/* No modem control DTR RTS */
	سیٹ_reg(dev, UART_MCR_انحراف, 0x00);
	/* Clear line status and read receive buffer */
	اگر (get_reg(dev, UART_LSR_انحراف) & UART_LSR_DR)
		get_reg(dev, UART_RBR_انحراف);
	/* Set scratchpad */
	سیٹ_reg(dev, UART_SCR_انحراف, 0x00);
}

صحیح uart8250_init(غیرعلامتی طویل بنیاد_2, u32 اندر_freq, u32 baudrate, u32 reg_سرکاؤ,
		  u32 reg_چوڑائی, u32 reg_انحراف, u32 caps)
{
	uart8250_device_init(&uart8250_dev, بنیاد_2, اندر_freq, baudrate,
			     reg_سرکاؤ, reg_چوڑائی, reg_انحراف, caps);

	sbi_console_سیٹ_device(&uart8250_console);

	واپس sbi_domain_جڑ_جوڑو_memrange(بنیاد_2, صفحہ_سائز, صفحہ_سائز,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
