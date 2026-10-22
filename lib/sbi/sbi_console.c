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

#تعریف_کړه CONSOLE_TBUF_MAX 256

ساكن ثابت بنية sbi_console_device *console_dev = عدم;
ساكن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساكن u32 console_tbuf_len;
ساكن spinlock_t console_بهر_قلف	       = SPIN_قلف_INITIALIZER;

#که_تعریف_شوی CONFIG_CONSOLE_EARLY_بفر_اندازه
#تعریف_کړه CONSOLE_EARLY_بفر_اندازه	CONFIG_CONSOLE_EARLY_بفر_اندازه
#وإلا
#تعریف_کړه CONSOLE_EARLY_بفر_اندازه	256
#د_که_پای
ساكن حرف console_early_بفر[CONSOLE_EARLY_بفر_اندازه] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_بفر, \
		       CONSOLE_EARLY_بفر_اندازه, حجم(حرف));

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

ساكن بینښه اوږد nputs(ثابت حرف *str, بینښه اوږد len)
{
	حرف ch;
	بینښه اوږد i;

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
			sbi_fifo_کتار_ته_ورزیاتول(&console_early_fifo, &ch, true);
		}
	}
	أرجع len;
}

ساكن فراغ nputs_all(ثابت حرف *str, بینښه اوږد len)
{
	بینښه اوږد p = 0;

	طالما (p < len)
		p += nputs(&str[p], len - p);
}

فراغ sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

فراغ sbi_puts(ثابت حرف *str)
{
	بینښه اوږد len = sbi_strlen(str);

	spin_قلف(&console_بهر_قلف);
	nputs_all(str, len);
	spin_unlock(&console_بهر_قلف);
}

بینښه اوږد sbi_nputs(ثابت حرف *str, بینښه اوږد len)
{
	بینښه اوږد ret;

	spin_قلف(&console_بهر_قلف);
	ret = nputs(str, len);
	spin_unlock(&console_بهر_قلف);

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

بینښه اوږد sbi_ngets(حرف *str, بینښه اوږد len)
{
	صحيح ch;
	بینښه اوږد i;

	لكل (i = 0; i < len; i++) {
		ch = sbi_getc();
		إذا (ch < 0)
			اكسر;
		str[i] = ch;
	}

	أرجع i;
}

#تعریف_کړه PAD_ښی 1
#تعریف_کړه PAD_ZERO 2
#تعریف_کړه PAD_ALTERNATE 4
#تعریف_کړه PAD_SIGN 8
#تعریف_کړه USE_TBUF 16
#تعریف_کړه PRINT_BUF_LEN 64

#تعریف_کړه va_پیل(v, l) __builtin_va_start((v), l)
#تعریف_کړه va_پای_ټکی __builtin_va_end
#تعریف_کړه va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **بهر, u32 *بهر_len, حرف ch, صحيح flags)
{
	إذا (!بهر) {
		sbi_putc(ch);
		أرجع;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	إذا (!بهر_len || *بهر_len > 1) {
		*(*بهر)++ = ch;
		**بهر = '\0';
		إذا (بهر_len) {
			--(*بهر_len);
			إذا ((flags & USE_TBUF) && *بهر_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *بهر_len);
				*بهر = console_tbuf;
				*بهر_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساكن صحيح prints(حرف **بهر, u32 *بهر_len, ثابت حرف *string, صحيح پلنوالی,
		  صحيح flags)
{
	صحيح pc = 0;
	پلنوالی -= sbi_strlen(string);
	إذا (!(flags & PAD_ښی)) {
		لكل (; پلنوالی > 0; --پلنوالی) {
			printc(بهر, بهر_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	لكل (; *string; ++string) {
		printc(بهر, بهر_len, *string, flags);
		++pc;
	}
	لكل (; پلنوالی > 0; --پلنوالی) {
		printc(بهر, بهر_len, ' ', flags);
		++pc;
	}

	أرجع pc;
}

ساكن صحيح printi(حرف **بهر, u32 *بهر_len, اوږد اوږد i,
		  صحيح پلنوالی, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	بینښه اوږد اوږد u, b, t;

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
			printc(بهر, بهر_len, sign, flags);
			++pc;
			--پلنوالی;
		}
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16 || b == 8) {
				printc(بهر, بهر_len, '0', flags);
				++pc;
				--پلنوالی;
			}
			إذا (b == 16) {
				printc(بهر, بهر_len, 'x' - 'a' + letbase, flags);
				++pc;
				--پلنوالی;
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

	أرجع pc + prints(بهر, بهر_len, s, پلنوالی, flags);
}

ساكن صحيح print(حرف **بهر, u32 *بهر_len, ثابت حرف *format, va_list دلیلونه)
{
	bool flags_بشپړ;
	صحيح پلنوالی, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!بهر) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	إذا (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		بهر = &tout;
		بهر_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	إذا (بهر) {
		إذا(!بهر_len || *بهر_len)
			**بهر = '\0';
	}

	لكل (; *format != 0; ++format) {
		پلنوالی = flags = 0;
		إذا (use_tbuf)
			flags |= USE_TBUF;
		إذا (*format == '%') {
			++format;
			إذا (*format == '\0')
				اكسر;
			إذا (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_بشپړ = false;
			طالما (!flags_بشپړ) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_ښی;
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
					flags_بشپړ = true;
					اكسر;
				}
				إذا (!flags_بشپړ)
					++format;
			}
			إذا (flags & PAD_ښی)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				پلنوالی *= 10;
				پلنوالی += *format - '0';
			}
			إذا (*format == 's') {
				حرف *s = va_arg(دلیلونه, حرف *);
				pc += prints(بهر, بهر_len, s ? s : "(null)",
					     پلنوالی, flags);
				استمر;
			}
			إذا ((*format == 'd') || (*format == 'i')) {
				pc += printi(بهر, بهر_len, va_arg(دلیلونه, صحيح),
					     پلنوالی, flags, *format);
				استمر;
			}
			إذا ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(بهر, بهر_len, va_arg(دلیلونه, بینښه صحيح),
					     پلنوالی, flags, *format);
				استمر;
			}
			إذا ((*format == 'p') || (*format == 'P')) {
				pc += printi(بهر, بهر_len, (uintptr_t)va_arg(دلیلونه, فراغ*),
					     پلنوالی, flags, *format);
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
					pc += printi(بهر, بهر_len, va_arg(دلیلونه, اوږد اوږد),
						پلنوالی, flags, type);
					استمر;
				}
				إذا ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				إذا ((type == 'd') || (type == 'i'))
					pc += printi(بهر, بهر_len, va_arg(دلیلونه, اوږد),
					     پلنوالی, flags, type);
				وإلا
					pc += printi(بهر, بهر_len, va_arg(دلیلونه, بینښه اوږد),
					     پلنوالی, flags, type);
				استمر;
			}
			إذا (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(دلیلونه, صحيح);
				scr[1] = '\0';
				pc += prints(بهر, بهر_len, scr, پلنوالی, flags);
				استمر;
			}
		} وإلا {
literal:
			printc(بهر, بهر_len, *format, flags);
			++pc;
		}
	}

	إذا (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	أرجع pc;
}

صحيح sbi_sprintf(حرف *بهر, ثابت حرف *format, ...)
{
	va_list دلیلونه;
	صحيح retval;

	إذا (unlikely(!بهر))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_پیل(دلیلونه, format);
	retval = print(&بهر, عدم, format, دلیلونه);
	va_پای_ټکی(دلیلونه);

	أرجع retval;
}

صحيح sbi_snprintf(حرف *بهر, u32 بهر_sz, ثابت حرف *format, ...)
{
	va_list دلیلونه;
	صحيح retval;

	إذا (unlikely(!بهر && بهر_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_پیل(دلیلونه, format);
	retval = print(&بهر, &بهر_sz, format, دلیلونه);
	va_پای_ټکی(دلیلونه);

	أرجع retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list دلیلونه;
	صحيح retval;

	spin_قلف(&console_بهر_قلف);
	va_پیل(دلیلونه, format);
	retval = print(عدم, عدم, format, دلیلونه);
	va_پای_ټکی(دلیلونه);
	spin_unlock(&console_بهر_قلف);

	أرجع retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list دلیلونه;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_پیل(دلیلونه, format);
	إذا (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_قلف(&console_بهر_قلف);
		retval = print(عدم, عدم, format, دلیلونه);
		spin_unlock(&console_بهر_قلف);
	}
	va_پای_ټکی(دلیلونه);

	أرجع retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list دلیلونه;

	spin_قلف(&console_بهر_قلف);
	va_پیل(دلیلونه, format);
	print(عدم, عدم, format, دلیلونه);
	va_پای_ټکی(دلیلونه);
	spin_unlock(&console_بهر_قلف);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	أرجع console_dev;
}

فراغ sbi_console_سېټ_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	إذا (!dev)
		أرجع;

	إذا (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	إذا (flush_early_fifo) {
		طالما (!sbi_fifo_له_کتار_ایستل(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
