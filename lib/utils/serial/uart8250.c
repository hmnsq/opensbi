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

#تعریف_کرو UART_RBR_وتھ		0	/* In:  Receive Buffer Register */
#تعریف_کرو UART_THR_وتھ		0	/* Out: Transmitter Holding Register */
#تعریف_کرو UART_DLL_وتھ		0	/* Out: Divisor Latch Low */
#تعریف_کرو UART_IER_وتھ		1	/* I/O: Interrupt Enable Register */
#تعریف_کرو UART_DLM_وتھ		1	/* Out: Divisor Latch High */
#تعریف_کرو UART_FCR_وتھ		2	/* Out: FIFO Control Register */
#تعریف_کرو UART_IIR_وتھ		2	/* I/O: Interrupt Identification Register */
#تعریف_کرو UART_LCR_وتھ		3	/* Out: Line Control Register */
#تعریف_کرو UART_MCR_وتھ		4	/* Out: Modem Control Register */
#تعریف_کرو UART_LSR_وتھ		5	/* In:  Line Status Register */
#تعریف_کرو UART_MSR_وتھ		6	/* In:  Modem Status Register */
#تعریف_کرو UART_SCR_وتھ		7	/* I/O: Scratch Register */
#تعریف_کرو UART_MDR1_وتھ	8	/* I/O:  Mode Register */

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

ساكن بنية uart8250_device uart8250_dev;

ساكن u32 get_reg(بنية uart8250_device *dev, u32 عدد)
{
	u32 وتھ = عدد << dev->reg_کھسکاؤ;

	إذا (dev->reg_چوڑائی == 1)
		أرجع readb(dev->ادھار_2 + وتھ);
	وإلا إذا (dev->reg_چوڑائی == 2)
		أرجع readw(dev->ادھار_2 + وتھ);
	وإلا
		أرجع readl(dev->ادھار_2 + وتھ);
}

ساكن فراغ سیٹ_reg(بنية uart8250_device *dev, u32 عدد, u32 val)
{
	u32 وتھ = عدد << dev->reg_کھسکاؤ;

	إذا (dev->reg_چوڑائی == 1)
		writeb(val, dev->ادھار_2 + وتھ);
	وإلا إذا (dev->reg_چوڑائی == 2)
		writew(val, dev->ادھار_2 + وتھ);
	وإلا
		writel(val, dev->ادھار_2 + وتھ);
}

فراغ uart8250_device_putc(بنية uart8250_device *dev, حرف ch)
{
	طالما ((get_reg(dev, UART_LSR_وتھ) & UART_LSR_THRE) == 0)
		;

	سیٹ_reg(dev, UART_THR_وتھ, ch);
}

ساكن فراغ uart8250_putc(حرف ch)
{
	أرجع uart8250_device_putc(&uart8250_dev, ch);
}

صحيح uart8250_device_getc(بنية uart8250_device *dev)
{
	إذا (get_reg(dev, UART_LSR_وتھ) & UART_LSR_DR)
		أرجع get_reg(dev, UART_RBR_وتھ);
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

فراغ uart8250_device_init(بنية uart8250_device *dev, بنانشانی لما ادھار_2,
			  u32 وچ_freq, u32 baudrate, u32 reg_کھسکاؤ,
			  u32 reg_چوڑائی, u32 reg_وتھ, u32 caps)
{
	u16 bdiv = 0;

	dev->ادھار_2      = (متطاير حرف *)ادھار_2 + reg_وتھ;
	dev->reg_کھسکاؤ = reg_کھسکاؤ;
	dev->reg_چوڑائی = reg_چوڑائی;
	dev->وچ_freq   = وچ_freq;
	dev->baudrate  = baudrate;

	إذا (dev->baudrate) {
		bdiv = (dev->وچ_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	سیٹ_reg(dev, UART_IER_وتھ, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	سیٹ_reg(dev, UART_LCR_وتھ, 0x80);

	إذا (bdiv) {
		/* Set divisor low byte */
		سیٹ_reg(dev, UART_DLL_وتھ, bdiv & 0xff);
		/* Set divisor high byte */
		سیٹ_reg(dev, UART_DLM_وتھ, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	سیٹ_reg(dev, UART_LCR_وتھ, 0x03);
	/* Enable FIFO */
	سیٹ_reg(dev, UART_FCR_وتھ, 0x01);
	/* No modem control DTR RTS */
	سیٹ_reg(dev, UART_MCR_وتھ, 0x00);
	/* Clear line status and read receive buffer */
	إذا (get_reg(dev, UART_LSR_وتھ) & UART_LSR_DR)
		get_reg(dev, UART_RBR_وتھ);
	/* Set scratchpad */
	سیٹ_reg(dev, UART_SCR_وتھ, 0x00);
}

صحيح uart8250_init(بنانشانی لما ادھار_2, u32 وچ_freq, u32 baudrate, u32 reg_کھسکاؤ,
		  u32 reg_چوڑائی, u32 reg_وتھ, u32 caps)
{
	uart8250_device_init(&uart8250_dev, ادھار_2, وچ_freq, baudrate,
			     reg_کھسکاؤ, reg_چوڑائی, reg_وتھ, caps);

	sbi_console_سیٹ_device(&uart8250_console);

	أرجع sbi_domain_جڑ_جوڑو_memrange(ادھار_2, ورقہ_ناپ, ورقہ_ناپ,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
