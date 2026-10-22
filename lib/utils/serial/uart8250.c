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

#تعریف_کړه UART_RBR_دننه_واټن		0	/* In:  Receive Buffer Register */
#تعریف_کړه UART_THR_دننه_واټن		0	/* Out: Transmitter Holding Register */
#تعریف_کړه UART_DLL_دننه_واټن		0	/* Out: Divisor Latch Low */
#تعریف_کړه UART_IER_دننه_واټن		1	/* I/O: Interrupt Enable Register */
#تعریف_کړه UART_DLM_دننه_واټن		1	/* Out: Divisor Latch High */
#تعریف_کړه UART_FCR_دننه_واټن		2	/* Out: FIFO Control Register */
#تعریف_کړه UART_IIR_دننه_واټن		2	/* I/O: Interrupt Identification Register */
#تعریف_کړه UART_LCR_دننه_واټن		3	/* Out: Line Control Register */
#تعریف_کړه UART_MCR_دننه_واټن		4	/* Out: Modem Control Register */
#تعریف_کړه UART_LSR_دننه_واټن		5	/* In:  Line Status Register */
#تعریف_کړه UART_MSR_دننه_واټن		6	/* In:  Modem Status Register */
#تعریف_کړه UART_SCR_دننه_واټن		7	/* I/O: Scratch Register */
#تعریف_کړه UART_MDR1_دننه_واټن	8	/* I/O:  Mode Register */

#تعریف_کړه UART_LSR_FIFOE		0x80	/* Fifo error */
#تعریف_کړه UART_LSR_TEMT		0x40	/* Transmitter empty */
#تعریف_کړه UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#تعریف_کړه UART_LSR_BI		0x10	/* Break interrupt indicator */
#تعریف_کړه UART_LSR_FE		0x08	/* Frame error indicator */
#تعریف_کړه UART_LSR_PE		0x04	/* Parity error indicator */
#تعریف_کړه UART_LSR_OE		0x02	/* Overrun error indicator */
#تعریف_کړه UART_LSR_DR		0x01	/* Receiver data ready */
#تعریف_کړه UART_LSR_BRK_ERROR_بټونه	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#تعریف_کړه UART_IER_DMAE		0x80	/* DMA Requests Enable */
#تعریف_کړه UART_IER_UUE		0x40	/* UART Unit Enable */
#تعریف_کړه UART_IER_NRZE		0x20	/* NRZ coding Enable */
#تعریف_کړه UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

ساكن بنية uart8250_device uart8250_dev;

ساكن u32 get_reg(بنية uart8250_device *dev, u32 عدد)
{
	u32 دننه_واټن = عدد << dev->reg_ښويدنه;

	إذا (dev->reg_پلنوالی == 1)
		أرجع readb(dev->بنسټ_2 + دننه_واټن);
	وإلا إذا (dev->reg_پلنوالی == 2)
		أرجع readw(dev->بنسټ_2 + دننه_واټن);
	وإلا
		أرجع readl(dev->بنسټ_2 + دننه_واټن);
}

ساكن فراغ سېټ_reg(بنية uart8250_device *dev, u32 عدد, u32 val)
{
	u32 دننه_واټن = عدد << dev->reg_ښويدنه;

	إذا (dev->reg_پلنوالی == 1)
		writeb(val, dev->بنسټ_2 + دننه_واټن);
	وإلا إذا (dev->reg_پلنوالی == 2)
		writew(val, dev->بنسټ_2 + دننه_واټن);
	وإلا
		writel(val, dev->بنسټ_2 + دننه_واټن);
}

فراغ uart8250_device_putc(بنية uart8250_device *dev, حرف ch)
{
	طالما ((get_reg(dev, UART_LSR_دننه_واټن) & UART_LSR_THRE) == 0)
		;

	سېټ_reg(dev, UART_THR_دننه_واټن, ch);
}

ساكن فراغ uart8250_putc(حرف ch)
{
	أرجع uart8250_device_putc(&uart8250_dev, ch);
}

صحيح uart8250_device_getc(بنية uart8250_device *dev)
{
	إذا (get_reg(dev, UART_LSR_دننه_واټن) & UART_LSR_DR)
		أرجع get_reg(dev, UART_RBR_دننه_واټن);
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

فراغ uart8250_device_init(بنية uart8250_device *dev, بینښه اوږد بنسټ_2,
			  u32 دننه_freq, u32 baudrate, u32 reg_ښويدنه,
			  u32 reg_پلنوالی, u32 reg_دننه_واټن, u32 caps)
{
	u16 bdiv = 0;

	dev->بنسټ_2      = (متطاير حرف *)بنسټ_2 + reg_دننه_واټن;
	dev->reg_ښويدنه = reg_ښويدنه;
	dev->reg_پلنوالی = reg_پلنوالی;
	dev->دننه_freq   = دننه_freq;
	dev->baudrate  = baudrate;

	إذا (dev->baudrate) {
		bdiv = (dev->دننه_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	سېټ_reg(dev, UART_IER_دننه_واټن, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	سېټ_reg(dev, UART_LCR_دننه_واټن, 0x80);

	إذا (bdiv) {
		/* Set divisor low byte */
		سېټ_reg(dev, UART_DLL_دننه_واټن, bdiv & 0xff);
		/* Set divisor high byte */
		سېټ_reg(dev, UART_DLM_دننه_واټن, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	سېټ_reg(dev, UART_LCR_دننه_واټن, 0x03);
	/* Enable FIFO */
	سېټ_reg(dev, UART_FCR_دننه_واټن, 0x01);
	/* No modem control DTR RTS */
	سېټ_reg(dev, UART_MCR_دننه_واټن, 0x00);
	/* Clear line status and read receive buffer */
	إذا (get_reg(dev, UART_LSR_دننه_واټن) & UART_LSR_DR)
		get_reg(dev, UART_RBR_دننه_واټن);
	/* Set scratchpad */
	سېټ_reg(dev, UART_SCR_دننه_واټن, 0x00);
}

صحيح uart8250_init(بینښه اوږد بنسټ_2, u32 دننه_freq, u32 baudrate, u32 reg_ښويدنه,
		  u32 reg_پلنوالی, u32 reg_دننه_واټن, u32 caps)
{
	uart8250_device_init(&uart8250_dev, بنسټ_2, دننه_freq, baudrate,
			     reg_ښويدنه, reg_پلنوالی, reg_دننه_واټن, caps);

	sbi_console_سېټ_device(&uart8250_console);

	أرجع sbi_domain_ریښه_جمع_کړه_memrange(بنسټ_2, پاڼه_اندازه, پاڼه_اندازه,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
