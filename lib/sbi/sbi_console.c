/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_fifo.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_string.h>

#تعریف_کن CONSOLE_TBUF_MAX 256

ساكن ثابت بنية sbi_console_device *console_dev = عدم;
ساكن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساكن u32 console_tbuf_len;
ساكن spinlock_t console_بیرون_قفل	       = SPIN_قفل_INITIALIZER;

#اگر_تعریف_شده CONFIG_CONSOLE_EARLY_میانگیر_اندازه
#تعریف_کن CONSOLE_EARLY_میانگیر_اندازه	CONFIG_CONSOLE_EARLY_میانگیر_اندازه
#وإلا
#تعریف_کن CONSOLE_EARLY_میانگیر_اندازه	256
#پایان_اگر
ساكن حرف console_early_میانگیر[CONSOLE_EARLY_میانگیر_اندازه] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_میانگیر, \
		       CONSOLE_EARLY_میانگیر_اندازه, حجم(حرف));

bool sbi_isprintable(حرف c)
{
	إذا (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		أرجع true;
	}
	أرجع false;
}

صحيح sbi_getc(فراغ)
{
	إذا (console_dev && console_dev->console_getc)
		أرجع console_dev->console_getc();
	أرجع -1;
}

ساكن بیعلامت دراز nputs(ثابت حرف *str, بیعلامت دراز len)
{
	حرف ch;
	بیعلامت دراز i;

	إذا (console_dev) {
		إذا (console_dev->console_puts)
			أرجع console_dev->console_puts(str, len);
		وإلا إذا (console_dev->console_putc) {
			لكل (i = 0; i < len; i++) {
				إذا (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} وإلا {
		لكل (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_افزودن_به_صف(&console_early_fifo, &ch, true);
		}
	}
	أرجع len;
}

ساكن فراغ nputs_all(ثابت حرف *str, بیعلامت دراز len)
{
	بیعلامت دراز p = 0;

	طالما (p < len)
		p += nputs(&str[p], len - p);
}

فراغ sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

فراغ sbi_puts(ثابت حرف *str)
{
	بیعلامت دراز len = sbi_strlen(str);

	spin_قفل(&console_بیرون_قفل);
	nputs_all(str, len);
	spin_unlock(&console_بیرون_قفل);
}

بیعلامت دراز sbi_nputs(ثابت حرف *str, بیعلامت دراز len)
{
	بیعلامت دراز ret;

	spin_قفل(&console_بیرون_قفل);
	ret = nputs(str, len);
	spin_unlock(&console_بیرون_قفل);

	أرجع ret;
}

فراغ sbi_gets(حرف *s, صحيح maxwidth, حرف endchar)
{
	صحيح ch;
	حرف *retval = s;

	طالما ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (حرف)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

بیعلامت دراز sbi_ngets(حرف *str, بیعلامت دراز len)
{
	صحيح ch;
	بیعلامت دراز i;

	لكل (i = 0; i < len; i++) {
		ch = sbi_getc();
		إذا (ch < 0)
			اكسر;
		str[i] = ch;
	}

	أرجع i;
}

#تعریف_کن PAD_راست 1
#تعریف_کن PAD_ZERO 2
#تعریف_کن PAD_ALTERNATE 4
#تعریف_کن PAD_SIGN 8
#تعریف_کن USE_TBUF 16
#تعریف_کن PRINT_BUF_LEN 64

#تعریف_کن va_شروع(v, l) __builtin_va_start((v), l)
#تعریف_کن va_انتها __builtin_va_end
#تعریف_کن va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **بیرون, u32 *بیرون_len, حرف ch, صحيح flags)
{
	إذا (!بیرون) {
		sbi_putc(ch);
		أرجع;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	إذا (!بیرون_len || *بیرون_len > 1) {
		*(*بیرون)++ = ch;
		**بیرون = '\0';
		إذا (بیرون_len) {
			--(*بیرون_len);
			إذا ((flags & USE_TBUF) && *بیرون_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *بیرون_len);
				*بیرون = console_tbuf;
				*بیرون_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساكن صحيح prints(حرف **بیرون, u32 *بیرون_len, ثابت حرف *string, صحيح عرض,
		  صحيح flags)
{
	صحيح pc = 0;
	عرض -= sbi_strlen(string);
	إذا (!(flags & PAD_راست)) {
		لكل (; عرض > 0; --عرض) {
			printc(بیرون, بیرون_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	لكل (; *string; ++string) {
		printc(بیرون, بیرون_len, *string, flags);
		++pc;
	}
	لكل (; عرض > 0; --عرض) {
		printc(بیرون, بیرون_len, ' ', flags);
		++pc;
	}

	أرجع pc;
}

ساكن صحيح printi(حرف **بیرون, u32 *بیرون_len, دراز دراز i,
		  صحيح عرض, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	بیعلامت دراز دراز u, b, t;

	b = 10;
	letbase = 'a';
	إذا (type == 'o')
		b = 8;
	وإلا إذا (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	إذا (type == 'i' || type == 'd') {
		إذا ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		إذا (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	إذا (!u) {
		*--s = '0';
	} وإلا {
		طالما (u) {
			t = u % b;
			u = u / b;
			إذا (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	إذا (flags & PAD_ZERO) {
		إذا (sign) {
			printc(بیرون, بیرون_len, sign, flags);
			++pc;
			--عرض;
		}
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16 || b == 8) {
				printc(بیرون, بیرون_len, '0', flags);
				++pc;
				--عرض;
			}
			إذا (b == 16) {
				printc(بیرون, بیرون_len, 'x' - 'a' + letbase, flags);
				++pc;
				--عرض;
			}
		}
	} وإلا {
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16)
				*--s = 'x' - 'a' + letbase;
			إذا (b == 16 || b == 8)
				*--s = '0';
		}
		إذا (sign)
			*--s = sign;
	}

	أرجع pc + prints(بیرون, بیرون_len, s, عرض, flags);
}

ساكن صحيح print(حرف **بیرون, u32 *بیرون_len, ثابت حرف *format, va_list آرگومانها)
{
	bool flags_انجامشده;
	صحيح عرض, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!بیرون) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	إذا (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		بیرون = &tout;
		بیرون_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	إذا (بیرون) {
		إذا(!بیرون_len || *بیرون_len)
			**بیرون = '\0';
	}

	لكل (; *format != 0; ++format) {
		عرض = flags = 0;
		إذا (use_tbuf)
			flags |= USE_TBUF;
		إذا (*format == '%') {
			++format;
			إذا (*format == '\0')
				اكسر;
			إذا (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_انجامشده = false;
			طالما (!flags_انجامشده) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_راست;
					اكسر;
				حالة '+':
					flags |= PAD_SIGN;
					اكسر;
				حالة '#':
					flags |= PAD_ALTERNATE;
					اكسر;
				حالة '0':
					flags |= PAD_ZERO;
					اكسر;
				حالة ' ':
				حالة '\'':
					/* Ignored flags, do nothing */
					اكسر;
				افتراضي:
					flags_انجامشده = true;
					اكسر;
				}
				إذا (!flags_انجامشده)
					++format;
			}
			إذا (flags & PAD_راست)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				عرض *= 10;
				عرض += *format - '0';
			}
			إذا (*format == 's') {
				حرف *s = va_arg(آرگومانها, حرف *);
				pc += prints(بیرون, بیرون_len, s ? s : "(null)",
					     عرض, flags);
				استمر;
			}
			إذا ((*format == 'd') || (*format == 'i')) {
				pc += printi(بیرون, بیرون_len, va_arg(آرگومانها, صحيح),
					     عرض, flags, *format);
				استمر;
			}
			إذا ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(بیرون, بیرون_len, va_arg(آرگومانها, بیعلامت صحيح),
					     عرض, flags, *format);
				استمر;
			}
			إذا ((*format == 'p') || (*format == 'P')) {
				pc += printi(بیرون, بیرون_len, (uintptr_t)va_arg(آرگومانها, فراغ*),
					     عرض, flags, *format);
				استمر;
			}
			إذا (*format == 'l') {
				type = 'i';
				إذا (format[1] == 'l') {
					++format;
					إذا ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(بیرون, بیرون_len, va_arg(آرگومانها, دراز دراز),
						عرض, flags, type);
					استمر;
				}
				إذا ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				إذا ((type == 'd') || (type == 'i'))
					pc += printi(بیرون, بیرون_len, va_arg(آرگومانها, دراز),
					     عرض, flags, type);
				وإلا
					pc += printi(بیرون, بیرون_len, va_arg(آرگومانها, بیعلامت دراز),
					     عرض, flags, type);
				استمر;
			}
			إذا (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(آرگومانها, صحيح);
				scr[1] = '\0';
				pc += prints(بیرون, بیرون_len, scr, عرض, flags);
				استمر;
			}
		} وإلا {
literal:
			printc(بیرون, بیرون_len, *format, flags);
			++pc;
		}
	}

	إذا (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	أرجع pc;
}

صحيح sbi_sprintf(حرف *بیرون, ثابت حرف *format, ...)
{
	va_list آرگومانها;
	صحيح retval;

	إذا (unlikely(!بیرون))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_شروع(آرگومانها, format);
	retval = print(&بیرون, عدم, format, آرگومانها);
	va_انتها(آرگومانها);

	أرجع retval;
}

صحيح sbi_snprintf(حرف *بیرون, u32 بیرون_sz, ثابت حرف *format, ...)
{
	va_list آرگومانها;
	صحيح retval;

	إذا (unlikely(!بیرون && بیرون_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_شروع(آرگومانها, format);
	retval = print(&بیرون, &بیرون_sz, format, آرگومانها);
	va_انتها(آرگومانها);

	أرجع retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list آرگومانها;
	صحيح retval;

	spin_قفل(&console_بیرون_قفل);
	va_شروع(آرگومانها, format);
	retval = print(عدم, عدم, format, آرگومانها);
	va_انتها(آرگومانها);
	spin_unlock(&console_بیرون_قفل);

	أرجع retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list آرگومانها;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_شروع(آرگومانها, format);
	إذا (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_قفل(&console_بیرون_قفل);
		retval = print(عدم, عدم, format, آرگومانها);
		spin_unlock(&console_بیرون_قفل);
	}
	va_انتها(آرگومانها);

	أرجع retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list آرگومانها;

	spin_قفل(&console_بیرون_قفل);
	va_شروع(آرگومانها, format);
	print(عدم, عدم, format, آرگومانها);
	va_انتها(آرگومانها);
	spin_unlock(&console_بیرون_قفل);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	أرجع console_dev;
}

فراغ sbi_console_مجموعه_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	إذا (!dev)
		أرجع;

	إذا (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	إذا (flush_early_fifo) {
		طالما (!sbi_fifo_خارج_از_صف(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
