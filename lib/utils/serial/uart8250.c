/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi_utils/serial/uart8250.h>

/* clang-format off */

#घोषणा UART_RBR_OFFSET		0	/* In:  Receive Buffer Register */
#घोषणा UART_THR_OFFSET		0	/* Out: Transmitter Holding Register */
#घोषणा UART_DLL_OFFSET		0	/* Out: Divisor Latch Low */
#घोषणा UART_IER_OFFSET		1	/* I/O: Interrupt Enable Register */
#घोषणा UART_DLM_OFFSET		1	/* Out: Divisor Latch High */
#घोषणा UART_FCR_OFFSET		2	/* Out: FIFO Control Register */
#घोषणा UART_IIR_OFFSET		2	/* I/O: Interrupt Identification Register */
#घोषणा UART_LCR_OFFSET		3	/* Out: Line Control Register */
#घोषणा UART_MCR_OFFSET		4	/* Out: Modem Control Register */
#घोषणा UART_LSR_OFFSET		5	/* In:  Line Status Register */
#घोषणा UART_MSR_OFFSET		6	/* In:  Modem Status Register */
#घोषणा UART_SCR_OFFSET		7	/* I/O: Scratch Register */
#घोषणा UART_MDR1_OFFSET	8	/* I/O:  Mode Register */

#घोषणा UART_LSR_FIFOE		0x80	/* Fifo error */
#घोषणा UART_LSR_TEMT		0x40	/* Transmitter empty */
#घोषणा UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#घोषणा UART_LSR_BI		0x10	/* Break interrupt indicator */
#घोषणा UART_LSR_FE		0x08	/* Frame error indicator */
#घोषणा UART_LSR_PE		0x04	/* Parity error indicator */
#घोषणा UART_LSR_OE		0x02	/* Overrun error indicator */
#घोषणा UART_LSR_DR		0x01	/* Receiver data ready */
#घोषणा UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#घोषणा UART_IER_DMAE		0x80	/* DMA Requests Enable */
#घोषणा UART_IER_UUE		0x40	/* UART Unit Enable */
#घोषणा UART_IER_NRZE		0x20	/* NRZ coding Enable */
#घोषणा UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

अटल काष्ठा uart8250_device uart8250_dev;

अटल u32 get_reg(काष्ठा uart8250_device *dev, u32 अंक)
{
	u32 offset = अंक << dev->reg_shift;

	अगर (dev->reg_width == 1)
		वापस readb(dev->base + offset);
	अन्यथा अगर (dev->reg_width == 2)
		वापस readw(dev->base + offset);
	अन्यथा
		वापस readl(dev->base + offset);
}

अटल व्योम set_reg(काष्ठा uart8250_device *dev, u32 अंक, u32 val)
{
	u32 offset = अंक << dev->reg_shift;

	अगर (dev->reg_width == 1)
		writeb(val, dev->base + offset);
	अन्यथा अगर (dev->reg_width == 2)
		writew(val, dev->base + offset);
	अन्यथा
		writel(val, dev->base + offset);
}

व्योम uart8250_device_putc(काष्ठा uart8250_device *dev, अक्षर ch)
{
	जबतक ((get_reg(dev, UART_LSR_OFFSET) & UART_LSR_THRE) == 0)
		;

	set_reg(dev, UART_THR_OFFSET, ch);
}

अटल व्योम uart8250_putc(अक्षर ch)
{
	वापस uart8250_device_putc(&uart8250_dev, ch);
}

पूर्णांक uart8250_device_getc(काष्ठा uart8250_device *dev)
{
	अगर (get_reg(dev, UART_LSR_OFFSET) & UART_LSR_DR)
		वापस get_reg(dev, UART_RBR_OFFSET);
	वापस -1;
}

अटल पूर्णांक uart8250_getc(व्योम)
{
	वापस uart8250_device_getc(&uart8250_dev);
}

अटल काष्ठा sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

व्योम uart8250_device_init(काष्ठा uart8250_device *dev, अचिन्हित दीर्घ base,
			  u32 in_freq, u32 baudrate, u32 reg_shift,
			  u32 reg_width, u32 reg_offset, u32 caps)
{
	u16 bdiv = 0;

	dev->base      = (अस्थिर अक्षर *)base + reg_offset;
	dev->reg_shift = reg_shift;
	dev->reg_width = reg_width;
	dev->in_freq   = in_freq;
	dev->baudrate  = baudrate;

	अगर (dev->baudrate) {
		bdiv = (dev->in_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	set_reg(dev, UART_IER_OFFSET, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	set_reg(dev, UART_LCR_OFFSET, 0x80);

	अगर (bdiv) {
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
	अगर (get_reg(dev, UART_LSR_OFFSET) & UART_LSR_DR)
		get_reg(dev, UART_RBR_OFFSET);
	/* Set scratchpad */
	set_reg(dev, UART_SCR_OFFSET, 0x00);
}

पूर्णांक uart8250_init(अचिन्हित दीर्घ base, u32 in_freq, u32 baudrate, u32 reg_shift,
		  u32 reg_width, u32 reg_offset, u32 caps)
{
	uart8250_device_init(&uart8250_dev, base, in_freq, baudrate,
			     reg_shift, reg_width, reg_offset, caps);

	sbi_console_set_device(&uart8250_console);

	वापस sbi_domain_मूल_add_memrange(base, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
