/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi_utils/serial/uart8250.h>

/* clang-format off */

#परिभाषय UART_RBR_विस्थापनम्		0	/* In:  Receive Buffer Register */
#परिभाषय UART_THR_विस्थापनम्		0	/* Out: Transmitter Holding Register */
#परिभाषय UART_DLL_विस्थापनम्		0	/* Out: Divisor Latch Low */
#परिभाषय UART_IER_विस्थापनम्		1	/* I/O: Interrupt Enable Register */
#परिभाषय UART_DLM_विस्थापनम्		1	/* Out: Divisor Latch High */
#परिभाषय UART_FCR_विस्थापनम्		2	/* Out: FIFO Control Register */
#परिभाषय UART_IIR_विस्थापनम्		2	/* I/O: Interrupt Identification Register */
#परिभाषय UART_LCR_विस्थापनम्		3	/* Out: Line Control Register */
#परिभाषय UART_MCR_विस्थापनम्		4	/* Out: Modem Control Register */
#परिभाषय UART_LSR_विस्थापनम्		5	/* In:  Line Status Register */
#परिभाषय UART_MSR_विस्थापनम्		6	/* In:  Modem Status Register */
#परिभाषय UART_SCR_विस्थापनम्		7	/* I/O: Scratch Register */
#परिभाषय UART_MDR1_विस्थापनम्	8	/* I/O:  Mode Register */

#परिभाषय UART_LSR_FIFOE		0x80	/* Fifo error */
#परिभाषय UART_LSR_TEMT		0x40	/* Transmitter empty */
#परिभाषय UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#परिभाषय UART_LSR_BI		0x10	/* Break interrupt indicator */
#परिभाषय UART_LSR_FE		0x08	/* Frame error indicator */
#परिभाषय UART_LSR_PE		0x04	/* Parity error indicator */
#परिभाषय UART_LSR_OE		0x02	/* Overrun error indicator */
#परिभाषय UART_LSR_DR		0x01	/* Receiver data ready */
#परिभाषय UART_LSR_BRK_ERROR_द्वयङ्काः	0x1E	/* BI, FE, PE, OE bits */

/* The XScale PXA UARTs define these bits */
#परिभाषय UART_IER_DMAE		0x80	/* DMA Requests Enable */
#परिभाषय UART_IER_UUE		0x40	/* UART Unit Enable */
#परिभाषय UART_IER_NRZE		0x20	/* NRZ coding Enable */
#परिभाषय UART_IER_RTOIE		0x10	/* Receiver Time Out Interrupt Enable */

/* clang-format on */

स्थिर संरचना uart8250_device uart8250_dev;

स्थिर u32 get_reg(संरचना uart8250_device *dev, u32 अङ्कः)
{
	u32 विस्थापनम् = अङ्कः << dev->reg_सरणम्;

	यदि (dev->reg_विस्तारः == 1)
		प्रत्यागम readb(dev->आधारः_2 + विस्थापनम्);
	अन्यथा यदि (dev->reg_विस्तारः == 2)
		प्रत्यागम readw(dev->आधारः_2 + विस्थापनम्);
	अन्यथा
		प्रत्यागम readl(dev->आधारः_2 + विस्थापनम्);
}

स्थिर शून्य समुच्चयः_reg(संरचना uart8250_device *dev, u32 अङ्कः, u32 val)
{
	u32 विस्थापनम् = अङ्कः << dev->reg_सरणम्;

	यदि (dev->reg_विस्तारः == 1)
		writeb(val, dev->आधारः_2 + विस्थापनम्);
	अन्यथा यदि (dev->reg_विस्तारः == 2)
		writew(val, dev->आधारः_2 + विस्थापनम्);
	अन्यथा
		writel(val, dev->आधारः_2 + विस्थापनम्);
}

शून्य uart8250_device_putc(संरचना uart8250_device *dev, वर्ण ch)
{
	यावत् ((get_reg(dev, UART_LSR_विस्थापनम्) & UART_LSR_THRE) == 0)
		;

	समुच्चयः_reg(dev, UART_THR_विस्थापनम्, ch);
}

स्थिर शून्य uart8250_putc(वर्ण ch)
{
	प्रत्यागम uart8250_device_putc(&uart8250_dev, ch);
}

पूर्णाङ्क uart8250_device_getc(संरचना uart8250_device *dev)
{
	यदि (get_reg(dev, UART_LSR_विस्थापनम्) & UART_LSR_DR)
		प्रत्यागम get_reg(dev, UART_RBR_विस्थापनम्);
	प्रत्यागम -1;
}

स्थिर पूर्णाङ्क uart8250_getc(शून्य)
{
	प्रत्यागम uart8250_device_getc(&uart8250_dev);
}

स्थिर संरचना sbi_console_device uart8250_console = {
	.name = "uart8250",
	.console_putc = uart8250_putc,
	.console_getc = uart8250_getc
};

शून्य uart8250_device_init(संरचना uart8250_device *dev, अचिह्नित दीर्घ आधारः_2,
			  u32 अन्तर्_freq, u32 baudrate, u32 reg_सरणम्,
			  u32 reg_विस्तारः, u32 reg_विस्थापनम्, u32 caps)
{
	u16 bdiv = 0;

	dev->आधारः_2      = (चञ्चल वर्ण *)आधारः_2 + reg_विस्थापनम्;
	dev->reg_सरणम् = reg_सरणम्;
	dev->reg_विस्तारः = reg_विस्तारः;
	dev->अन्तर्_freq   = अन्तर्_freq;
	dev->baudrate  = baudrate;

	यदि (dev->baudrate) {
		bdiv = (dev->अन्तर्_freq + 8 * dev->baudrate) /
		       (16 * dev->baudrate);
	}

	/* Disable all interrupts */
	समुच्चयः_reg(dev, UART_IER_विस्थापनम्, (caps & UART_CAP_UUE) ?
				 UART_IER_UUE : 0x00);
	/* Enable DLAB */
	समुच्चयः_reg(dev, UART_LCR_विस्थापनम्, 0x80);

	यदि (bdiv) {
		/* Set divisor low byte */
		समुच्चयः_reg(dev, UART_DLL_विस्थापनम्, bdiv & 0xff);
		/* Set divisor high byte */
		समुच्चयः_reg(dev, UART_DLM_विस्थापनम्, (bdiv >> 8) & 0xff);
	}

	/* 8 bits, no parity, one stop bit */
	समुच्चयः_reg(dev, UART_LCR_विस्थापनम्, 0x03);
	/* Enable FIFO */
	समुच्चयः_reg(dev, UART_FCR_विस्थापनम्, 0x01);
	/* No modem control DTR RTS */
	समुच्चयः_reg(dev, UART_MCR_विस्थापनम्, 0x00);
	/* Clear line status and read receive buffer */
	यदि (get_reg(dev, UART_LSR_विस्थापनम्) & UART_LSR_DR)
		get_reg(dev, UART_RBR_विस्थापनम्);
	/* Set scratchpad */
	समुच्चयः_reg(dev, UART_SCR_विस्थापनम्, 0x00);
}

पूर्णाङ्क uart8250_init(अचिह्नित दीर्घ आधारः_2, u32 अन्तर्_freq, u32 baudrate, u32 reg_सरणम्,
		  u32 reg_विस्तारः, u32 reg_विस्थापनम्, u32 caps)
{
	uart8250_device_init(&uart8250_dev, आधारः_2, अन्तर्_freq, baudrate,
			     reg_सरणम्, reg_विस्तारः, reg_विस्थापनम्, caps);

	sbi_console_समुच्चयः_device(&uart8250_console);

	प्रत्यागम sbi_domain_मूलम्_सङ्कलय_memrange(आधारः_2, PAGE_आकार, PAGE_आकार,
					    (SBI_DOMAIN_MEMREGION_MMIO |
					    SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW));
}
