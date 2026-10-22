/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 ISCAS
 *
 * Authors:
 *   Icenowy Zheng <zhengxingda@iscas.ac.cn>
 */

#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi_utils/serial/altr-juart.h>

/* clang-format off */

#परिभाषय JUART_दत्त_विस्थापनम्		0x00
# define JUART_दत्त_दत्त_आवरणम्		0x000000ff
# define JUART_दत्त_RVALID		(1 << 15)
# define JUART_दत्त_RAVAIL_आवरणम्		0xffff0000
# define JUART_दत्त_RAVAIL_सरणम्	16
#परिभाषय JUART_CTRL_विस्थापनम्		0x04
# define JUART_CTRL_RE			(1 << 0)
# define JUART_CTRL_WE			(1 << 1)
# define JUART_CTRL_RI			(1 << 8)
# define JUART_CTRL_WI			(1 << 9)
# define JUART_CTRL_AC			(1 << 10)
# define JUART_CTRL_WSPACE_आवरणम्		0xffff0000
# define JUART_CTRL_WSPACE_सरणम्	16

/* clang-format on */

स्थिर चञ्चल वर्ण *altr_juart_आधारः;

स्थिर u32 get_reg(u32 विस्थापनम्)
{
	प्रत्यागम readl(altr_juart_आधारः + विस्थापनम्);
}

स्थिर शून्य समुच्चयः_reg(u32 विस्थापनम्, u32 val)
{
	writel(val, altr_juart_आधारः + विस्थापनम्);
}

स्थिर शून्य altr_juart_putc(वर्ण ch)
{
	यावत्(!(get_reg(JUART_CTRL_विस्थापनम्) & JUART_CTRL_WSPACE_आवरणम्))
		;

	समुच्चयः_reg(JUART_दत्त_विस्थापनम्, (अचिह्नित वर्ण)ch);
}

स्थिर पूर्णाङ्क altr_juart_getc(शून्य)
{
	u32 reg = get_reg(JUART_दत्त_विस्थापनम्);
	यदि (reg & JUART_दत्त_RVALID)
		प्रत्यागम reg & JUART_दत्त_दत्त_आवरणम्;

	प्रत्यागम -1;
}

स्थिर संरचना sbi_console_device altr_juart_console = {
	.name = "altr-juart",
	.console_putc = altr_juart_putc,
	.console_getc = altr_juart_getc
};

पूर्णाङ्क altr_juart_init(अचिह्नित दीर्घ आधारः_2)
{
	altr_juart_आधारः = (चञ्चल वर्ण *)आधारः_2;

	sbi_console_समुच्चयः_device(&altr_juart_console);

	प्रत्यागम 0;
}
