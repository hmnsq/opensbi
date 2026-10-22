/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_fifo.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_string.h>

#నిర్వచించు CONSOLE_TBUF_MAX 256

స్థిర స్థిరరాశి నిర్మాణం sbi_console_device *console_dev = ఖాళీ;
స్థిర అక్షరం console_tbuf[CONSOLE_TBUF_MAX];
స్థిర u32 console_tbuf_len;
స్థిర spinlock_t console_బహిర్గతం_తాళం	       = SPIN_తాళం_INITIALIZER;

#నిర్వచితమైతే CONFIG_CONSOLE_EARLY_బఫరు_కొలత
#నిర్వచించు CONSOLE_EARLY_బఫరు_కొలత	CONFIG_CONSOLE_EARLY_బఫరు_కొలత
#లేకపోతే
#నిర్వచించు CONSOLE_EARLY_బఫరు_కొలత	256
#అయితే_ముగింపు
స్థిర అక్షరం console_early_బఫరు[CONSOLE_EARLY_బఫరు_కొలత] = { 0 };
స్థిర SBI_FIFO_DEFINE(console_early_fifo, console_early_బఫరు, \
		       CONSOLE_EARLY_బఫరు_కొలత, పరిమాణం(అక్షరం));

bool sbi_isprintable(అక్షరం c)
{
	ఒకవేళ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		తిరిగి true;
	}
	తిరిగి false;
}

పూర్ణాంకం sbi_getc(శూన్య)
{
	ఒకవేళ (console_dev && console_dev->console_getc)
		తిరిగి console_dev->console_getc();
	తిరిగి -1;
}

స్థిర గుర్తులేని దీర్ఘ nputs(స్థిరరాశి అక్షరం *str, గుర్తులేని దీర్ఘ len)
{
	అక్షరం ch;
	గుర్తులేని దీర్ఘ i;

	ఒకవేళ (console_dev) {
		ఒకవేళ (console_dev->console_puts)
			తిరిగి console_dev->console_puts(str, len);
		లేకపోతే ఒకవేళ (console_dev->console_putc) {
			కొరకు (i = 0; i < len; i++) {
				ఒకవేళ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} లేకపోతే {
		కొరకు (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_వరుసలో_చేర్చు(&console_early_fifo, &ch, true);
		}
	}
	తిరిగి len;
}

స్థిర శూన్య nputs_all(స్థిరరాశి అక్షరం *str, గుర్తులేని దీర్ఘ len)
{
	గుర్తులేని దీర్ఘ p = 0;

	అయితే (p < len)
		p += nputs(&str[p], len - p);
}

శూన్య sbi_putc(అక్షరం ch)
{
	nputs_all(&ch, 1);
}

శూన్య sbi_puts(స్థిరరాశి అక్షరం *str)
{
	గుర్తులేని దీర్ఘ len = sbi_strlen(str);

	spin_తాళం(&console_బహిర్గతం_తాళం);
	nputs_all(str, len);
	spin_unlock(&console_బహిర్గతం_తాళం);
}

గుర్తులేని దీర్ఘ sbi_nputs(స్థిరరాశి అక్షరం *str, గుర్తులేని దీర్ఘ len)
{
	గుర్తులేని దీర్ఘ ret;

	spin_తాళం(&console_బహిర్గతం_తాళం);
	ret = nputs(str, len);
	spin_unlock(&console_బహిర్గతం_తాళం);

	తిరిగి ret;
}

శూన్య sbi_gets(అక్షరం *s, పూర్ణాంకం maxwidth, అక్షరం endchar)
{
	పూర్ణాంకం ch;
	అక్షరం *retval = s;

	అయితే ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (అక్షరం)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

గుర్తులేని దీర్ఘ sbi_ngets(అక్షరం *str, గుర్తులేని దీర్ఘ len)
{
	పూర్ణాంకం ch;
	గుర్తులేని దీర్ఘ i;

	కొరకు (i = 0; i < len; i++) {
		ch = sbi_getc();
		ఒకవేళ (ch < 0)
			ఆపు;
		str[i] = ch;
	}

	తిరిగి i;
}

#నిర్వచించు PAD_కుడి 1
#నిర్వచించు PAD_ZERO 2
#నిర్వచించు PAD_ALTERNATE 4
#నిర్వచించు PAD_SIGN 8
#నిర్వచించు USE_TBUF 16
#నిర్వచించు PRINT_BUF_LEN 64

#నిర్వచించు va_ప్రారంభం(v, l) __builtin_va_start((v), l)
#నిర్వచించు va_చివర __builtin_va_end
#నిర్వచించు va_arg __builtin_va_arg
రకనిర్వచనం __builtin_va_list va_list;

స్థిర శూన్య printc(అక్షరం **బహిర్గతం, u32 *బహిర్గతం_len, అక్షరం ch, పూర్ణాంకం flags)
{
	ఒకవేళ (!బహిర్గతం) {
		sbi_putc(ch);
		తిరిగి;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ఒకవేళ (!బహిర్గతం_len || *బహిర్గతం_len > 1) {
		*(*బహిర్గతం)++ = ch;
		**బహిర్గతం = '\0';
		ఒకవేళ (బహిర్గతం_len) {
			--(*బహిర్గతం_len);
			ఒకవేళ ((flags & USE_TBUF) && *బహిర్గతం_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *బహిర్గతం_len);
				*బహిర్గతం = console_tbuf;
				*బహిర్గతం_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

స్థిర పూర్ణాంకం prints(అక్షరం **బహిర్గతం, u32 *బహిర్గతం_len, స్థిరరాశి అక్షరం *string, పూర్ణాంకం వెడల్పు,
		  పూర్ణాంకం flags)
{
	పూర్ణాంకం pc = 0;
	వెడల్పు -= sbi_strlen(string);
	ఒకవేళ (!(flags & PAD_కుడి)) {
		కొరకు (; వెడల్పు > 0; --వెడల్పు) {
			printc(బహిర్గతం, బహిర్గతం_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	కొరకు (; *string; ++string) {
		printc(బహిర్గతం, బహిర్గతం_len, *string, flags);
		++pc;
	}
	కొరకు (; వెడల్పు > 0; --వెడల్పు) {
		printc(బహిర్గతం, బహిర్గతం_len, ' ', flags);
		++pc;
	}

	తిరిగి pc;
}

స్థిర పూర్ణాంకం printi(అక్షరం **బహిర్గతం, u32 *బహిర్గతం_len, దీర్ఘ దీర్ఘ i,
		  పూర్ణాంకం వెడల్పు, పూర్ణాంకం flags, పూర్ణాంకం type)
{
	పూర్ణాంకం pc = 0;
	అక్షరం *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	గుర్తులేని దీర్ఘ దీర్ఘ u, b, t;

	b = 10;
	letbase = 'a';
	ఒకవేళ (type == 'o')
		b = 8;
	లేకపోతే ఒకవేళ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ఒకవేళ (type == 'i' || type == 'd') {
		ఒకవేళ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ఒకవేళ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ఒకవేళ (!u) {
		*--s = '0';
	} లేకపోతే {
		అయితే (u) {
			t = u % b;
			u = u / b;
			ఒకవేళ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ఒకవేళ (flags & PAD_ZERO) {
		ఒకవేళ (sign) {
			printc(బహిర్గతం, బహిర్గతం_len, sign, flags);
			++pc;
			--వెడల్పు;
		}
		ఒకవేళ (i && (flags & PAD_ALTERNATE)) {
			ఒకవేళ (b == 16 || b == 8) {
				printc(బహిర్గతం, బహిర్గతం_len, '0', flags);
				++pc;
				--వెడల్పు;
			}
			ఒకవేళ (b == 16) {
				printc(బహిర్గతం, బహిర్గతం_len, 'x' - 'a' + letbase, flags);
				++pc;
				--వెడల్పు;
			}
		}
	} లేకపోతే {
		ఒకవేళ (i && (flags & PAD_ALTERNATE)) {
			ఒకవేళ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ఒకవేళ (b == 16 || b == 8)
				*--s = '0';
		}
		ఒకవేళ (sign)
			*--s = sign;
	}

	తిరిగి pc + prints(బహిర్గతం, బహిర్గతం_len, s, వెడల్పు, flags);
}

స్థిర పూర్ణాంకం print(అక్షరం **బహిర్గతం, u32 *బహిర్గతం_len, స్థిరరాశి అక్షరం *format, va_list వాదనలు)
{
	bool flags_పూర్తి;
	పూర్ణాంకం వెడల్పు, flags, pc = 0;
	అక్షరం type, scr[2], *tout;
	bool use_tbuf = (!బహిర్గతం) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ఒకవేళ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		బహిర్గతం = &tout;
		బహిర్గతం_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ఒకవేళ (బహిర్గతం) {
		ఒకవేళ(!బహిర్గతం_len || *బహిర్గతం_len)
			**బహిర్గతం = '\0';
	}

	కొరకు (; *format != 0; ++format) {
		వెడల్పు = flags = 0;
		ఒకవేళ (use_tbuf)
			flags |= USE_TBUF;
		ఒకవేళ (*format == '%') {
			++format;
			ఒకవేళ (*format == '\0')
				ఆపు;
			ఒకవేళ (*format == '%')
				వెళ్ళు literal;
			/* Get flags */
			flags_పూర్తి = false;
			అయితే (!flags_పూర్తి) {
				ఎంపిక (*format) {
				స్థితి '-':
					flags |= PAD_కుడి;
					ఆపు;
				స్థితి '+':
					flags |= PAD_SIGN;
					ఆపు;
				స్థితి '#':
					flags |= PAD_ALTERNATE;
					ఆపు;
				స్థితి '0':
					flags |= PAD_ZERO;
					ఆపు;
				స్థితి ' ':
				స్థితి '\'':
					/* Ignored flags, do nothing */
					ఆపు;
				అప్రమేయ:
					flags_పూర్తి = true;
					ఆపు;
				}
				ఒకవేళ (!flags_పూర్తి)
					++format;
			}
			ఒకవేళ (flags & PAD_కుడి)
				flags &= ~PAD_ZERO;
			/* Get width */
			కొరకు (; *format >= '0' && *format <= '9'; ++format) {
				వెడల్పు *= 10;
				వెడల్పు += *format - '0';
			}
			ఒకవేళ (*format == 's') {
				అక్షరం *s = va_arg(వాదనలు, అక్షరం *);
				pc += prints(బహిర్గతం, బహిర్గతం_len, s ? s : "(null)",
					     వెడల్పు, flags);
				కొనసాగు;
			}
			ఒకవేళ ((*format == 'd') || (*format == 'i')) {
				pc += printi(బహిర్గతం, బహిర్గతం_len, va_arg(వాదనలు, పూర్ణాంకం),
					     వెడల్పు, flags, *format);
				కొనసాగు;
			}
			ఒకవేళ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(బహిర్గతం, బహిర్గతం_len, va_arg(వాదనలు, గుర్తులేని పూర్ణాంకం),
					     వెడల్పు, flags, *format);
				కొనసాగు;
			}
			ఒకవేళ ((*format == 'p') || (*format == 'P')) {
				pc += printi(బహిర్గతం, బహిర్గతం_len, (uintptr_t)va_arg(వాదనలు, శూన్య*),
					     వెడల్పు, flags, *format);
				కొనసాగు;
			}
			ఒకవేళ (*format == 'l') {
				type = 'i';
				ఒకవేళ (format[1] == 'l') {
					++format;
					ఒకవేళ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(బహిర్గతం, బహిర్గతం_len, va_arg(వాదనలు, దీర్ఘ దీర్ఘ),
						వెడల్పు, flags, type);
					కొనసాగు;
				}
				ఒకవేళ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ఒకవేళ ((type == 'd') || (type == 'i'))
					pc += printi(బహిర్గతం, బహిర్గతం_len, va_arg(వాదనలు, దీర్ఘ),
					     వెడల్పు, flags, type);
				లేకపోతే
					pc += printi(బహిర్గతం, బహిర్గతం_len, va_arg(వాదనలు, గుర్తులేని దీర్ఘ),
					     వెడల్పు, flags, type);
				కొనసాగు;
			}
			ఒకవేళ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(వాదనలు, పూర్ణాంకం);
				scr[1] = '\0';
				pc += prints(బహిర్గతం, బహిర్గతం_len, scr, వెడల్పు, flags);
				కొనసాగు;
			}
		} లేకపోతే {
literal:
			printc(బహిర్గతం, బహిర్గతం_len, *format, flags);
			++pc;
		}
	}

	ఒకవేళ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	తిరిగి pc;
}

పూర్ణాంకం sbi_sprintf(అక్షరం *బహిర్గతం, స్థిరరాశి అక్షరం *format, ...)
{
	va_list వాదనలు;
	పూర్ణాంకం retval;

	ఒకవేళ (unlikely(!బహిర్గతం))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_ప్రారంభం(వాదనలు, format);
	retval = print(&బహిర్గతం, ఖాళీ, format, వాదనలు);
	va_చివర(వాదనలు);

	తిరిగి retval;
}

పూర్ణాంకం sbi_snprintf(అక్షరం *బహిర్గతం, u32 బహిర్గతం_sz, స్థిరరాశి అక్షరం *format, ...)
{
	va_list వాదనలు;
	పూర్ణాంకం retval;

	ఒకవేళ (unlikely(!బహిర్గతం && బహిర్గతం_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_ప్రారంభం(వాదనలు, format);
	retval = print(&బహిర్గతం, &బహిర్గతం_sz, format, వాదనలు);
	va_చివర(వాదనలు);

	తిరిగి retval;
}

పూర్ణాంకం sbi_printf(స్థిరరాశి అక్షరం *format, ...)
{
	va_list వాదనలు;
	పూర్ణాంకం retval;

	spin_తాళం(&console_బహిర్గతం_తాళం);
	va_ప్రారంభం(వాదనలు, format);
	retval = print(ఖాళీ, ఖాళీ, format, వాదనలు);
	va_చివర(వాదనలు);
	spin_unlock(&console_బహిర్గతం_తాళం);

	తిరిగి retval;
}

పూర్ణాంకం sbi_dprintf(స్థిరరాశి అక్షరం *format, ...)
{
	va_list వాదనలు;
	పూర్ణాంకం retval = 0;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_ప్రారంభం(వాదనలు, format);
	ఒకవేళ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_తాళం(&console_బహిర్గతం_తాళం);
		retval = print(ఖాళీ, ఖాళీ, format, వాదనలు);
		spin_unlock(&console_బహిర్గతం_తాళం);
	}
	va_చివర(వాదనలు);

	తిరిగి retval;
}

శూన్య sbi_panic(స్థిరరాశి అక్షరం *format, ...)
{
	va_list వాదనలు;

	spin_తాళం(&console_బహిర్గతం_తాళం);
	va_ప్రారంభం(వాదనలు, format);
	print(ఖాళీ, ఖాళీ, format, వాదనలు);
	va_చివర(వాదనలు);
	spin_unlock(&console_బహిర్గతం_తాళం);

	sbi_hart_hang();
}

స్థిరరాశి నిర్మాణం sbi_console_device *sbi_console_get_device(శూన్య)
{
	తిరిగి console_dev;
}

శూన్య sbi_console_సమితి_device(స్థిరరాశి నిర్మాణం sbi_console_device *dev)
{
	అక్షరం ch;
	bool flush_early_fifo = false;

	ఒకవేళ (!dev)
		తిరిగి;

	ఒకవేళ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ఒకవేళ (flush_early_fifo) {
		అయితే (!sbi_fifo_వరుస_నుండి_తీయి(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
