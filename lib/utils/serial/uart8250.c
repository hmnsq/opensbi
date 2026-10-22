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

#تعریف_کن UART_RBR_جابه_جایی		0	/* In:  Receive Buffer Register */
#تعریف_کن UART_THR_جابه_جایی		0	/* Out: Transmitter Holding Register */
#تعریف_کن UART_DLL_جابه_جایی		0	/* Out: Divisor Latch Low */
#تعریف_کن UART_IER_جابه_جایی		1	/* I/O: Interrupt Enable Register */
#تعریف_کن UART_DLM_جابه_جایی		1	/* Out: Divisor Latch High */
#تعریف_کن UART_FCR_جابه_جایی		2	/* Out: FIFO Control Register */
#تعریف_کن UART_IIR_جابه_جایی		2	/* I/O: Interrupt Identification Register */
#تعریف_کن UART_LCR_جابه_جایی		3	/* Out: Line Control Register */
#تعریف_کن UART_MCR_جابه_جایی		4	/* Out: Modem Control Register */
#تعریف_کن UART_LSR_جابه_جایی		5	/* In:  Line Status Register */
#تعریف_کن UART_MSR_جابه_جایی		6	/* In:  Modem Status Register */
#تعریف_کن UART_SCR_جابه_جایی		7	/* I/O: Scratch Register */
#تعریف_کن UART_MDR1_جابه_جایی	8	/* I/O:  Mode Register */

#تعریف_کن UART_LSR_FIFOE		0x80	/* Fifo error */
#تعریف_کن UART_LSR_TEMT		0x40	/* Transmitter empty */
#تعریف_کن UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#تعریف_کن UART_LSR_BI		0x10	/* Break interrupt indicator */
#تعریف_کن UART_LSR_FE		0x08	/* Frame error indicator */
#تعریف_کن UART_LSR_PE		0x04	/* Parity error indicator */
#تعریف_کن UART_LSR_OE		0x02	/* Overrun error indicator */
#تعریف_کن UART_LSR_DR		0x01	/* Receiver data ready */
#تعریف_کن UART_LSR_BRK_ERROR_بیتها	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#تعریف_کن UART_IER_DMAE		0x80	/* DMA Requests Enable */
#تعریف_کن UART_IER_UUE		0x40	/* UART Unit Enable */
#تعریف_کن UART_IER_NRZE		0x20	/* NRZ coding Enable */
#تعریف_کن UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ساكن بنية uart8250_device uart8250_dev;

ساكن u32 get_reg(بنية uart8250_device *dev, u32 رقم)
{
	u32 جابه_جایی = رقم << dev->reg_لغزش;

	إذا (dev->reg_عرض == 1)
		أرجع readb(dev->پایه_2 + جابه_جایی);
	وإلا إذا (dev->reg_عرض == 2)
		أرجع readw(dev->پایه_2 + جابه_جایی);
	وإلا
		أرجع readl(dev->پایه_2 + جابه_جایی);
}

ساكن فراغ مجموعه_reg(بنية uart8250_device *dev, u32 رقم, u32 val)
{
	u32 جابه_جایی = رقم << dev->reg_لغزش;

	إذا (dev->reg_عرض == 1)
		writeb(val, dev->پایه_2 + جابه_جایی);
	وإلا إذا (dev->reg_عرض == 2)
		writew(val, dev->پایه_2 + جابه_جایی);
	وإلا
		writel(val, dev->پایه_2 + جابه_جایی);
}

فراغ uart8250_device_putc(بنية uart8250_device *dev, حرف ch)
{
	طالما ((get_reg(dev, UART_LSR_جابه_جایی) & UART_LSR_THRE) == 0)
		;

	مجموعه_reg(dev, UART_THR_جابه_جایی, ch);
}

ساكن فراغ uart8250_putc(حرف ch)
{
	أرجع uart8250_device_putc(&uart8250_dev, ch);
}

صحيح uart8250_device_getc(بنية uart8250_device *dev)
{
	إذا (get_reg(dev, UART_LSR_جابه_جایی) & UART_LSR_DR)
		أرجع get_reg(dev, UART_RBR_جابه_جایی);
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

فراغ uart8250_device_init(بنية uart8250_device *dev, بیعلامت دراز پایه_2,
			  u32 درون_freq, u32 baudrate, u32 reg_لغزش,
			  u32 reg_عرض, u32 reg_جابه_جایی, u32 caps)
{
	u16 bdiv = 0;

	dev->پایه_2      = (متطاير حرف *)پایه_2 + reg_جابه_جایی;
	dev->reg_لغزش = reg_لغزش;
	dev->reg_عرض = reg_عرض;
	dev->درون_freq   = درون_freq;
	dev->baudrate  = baudrate;

	إذا (dev->baudrate) {
		bdiv = (dev->درون_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	مجموعه_reg(dev, UART_IER_جابه_جایی, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	مجموعه_reg(dev, UART_LCR_جابه_جایی, 0x80);

	إذا (bdiv) {
		/* Set divisor low byte */
		مجموعه_reg(dev, UART_DLL_جابه_جایی, bdiv & 0xff);
		/* Set divisor high byte */
		مجموعه_reg(dev, UART_DLM_جابه_جایی, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	مجموعه_reg(dev, UART_LCR_جابه_جایی, 0x03);
	/* Enable FIFO */
	مجموعه_reg(dev, UART_FCR_جابه_جایی, 0x01);
	/* No modem control DTR RTS */
	مجموعه_reg(dev, UART_MCR_جابه_جایی, 0x00);
	/* Clear line status and read receive buffer */
	إذا (get_reg(dev, UART_LSR_جابه_جایی) & UART_LSR_DR)
		get_reg(dev, UART_RBR_جابه_جایی);
	/* Set scratchpad */
	مجموعه_reg(dev, UART_SCR_جابه_جایی, 0x00);
}

صحيح uart8250_init(بیعلامت دراز پایه_2, u32 درون_freq, u32 baudrate, u32 reg_لغزش,
		  u32 reg_عرض, u32 reg_جابه_جایی, u32 caps)
{
	uart8250_device_init(&uart8250_dev, پایه_2, درون_freq, baudrate,
			     reg_لغزش, reg_عرض, reg_جابه_جایی, caps);

	sbi_console_مجموعه_device(&uart8250_console);

	أرجع sbi_domain_ریشه_جمع_کن_memrange(پایه_2, صفحه_اندازه, صفحه_اندازه,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
