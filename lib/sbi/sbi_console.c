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

#وصف_ڪريو CONSOLE_TBUF_MAX 256

ساكن ثابت بنية sbi_console_device *console_dev = عدم;
ساكن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساكن u32 console_tbuf_len;
ساكن spinlock_t console_ٻاهريون_تالو	       = SPIN_تالو_INITIALIZER;

#جيڪڏهن_وصف_ٿيل CONFIG_CONSOLE_EARLY_بفر_سائيز
#وصف_ڪريو CONSOLE_EARLY_بفر_سائيز	CONFIG_CONSOLE_EARLY_بفر_سائيز
#وإلا
#وصف_ڪريو CONSOLE_EARLY_بفر_سائيز	256
#جيڪڏهن_پورو
ساكن حرف console_early_بفر[CONSOLE_EARLY_بفر_سائيز] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_بفر, \
		       CONSOLE_EARLY_بفر_سائيز, حجم(حرف));

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

ساكن بنانشان ڊگهو nputs(ثابت حرف *str, بنانشان ڊگهو len)
{
	حرف ch;
	بنانشان ڊگهو i;

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
			sbi_fifo_قطار_اندر_وجهو(&console_early_fifo, &ch, true);
		}
	}
	أرجع len;
}

ساكن فراغ nputs_all(ثابت حرف *str, بنانشان ڊگهو len)
{
	بنانشان ڊگهو p = 0;

	طالما (p < len)
		p += nputs(&str[p], len - p);
}

فراغ sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

فراغ sbi_puts(ثابت حرف *str)
{
	بنانشان ڊگهو len = sbi_strlen(str);

	spin_تالو(&console_ٻاهريون_تالو);
	nputs_all(str, len);
	spin_unlock(&console_ٻاهريون_تالو);
}

بنانشان ڊگهو sbi_nputs(ثابت حرف *str, بنانشان ڊگهو len)
{
	بنانشان ڊگهو ret;

	spin_تالو(&console_ٻاهريون_تالو);
	ret = nputs(str, len);
	spin_unlock(&console_ٻاهريون_تالو);

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

بنانشان ڊگهو sbi_ngets(حرف *str, بنانشان ڊگهو len)
{
	صحيح ch;
	بنانشان ڊگهو i;

	لكل (i = 0; i < len; i++) {
		ch = sbi_getc();
		إذا (ch < 0)
			اكسر;
		str[i] = ch;
	}

	أرجع i;
}

#وصف_ڪريو PAD_ساڄو 1
#وصف_ڪريو PAD_ZERO 2
#وصف_ڪريو PAD_ALTERNATE 4
#وصف_ڪريو PAD_SIGN 8
#وصف_ڪريو USE_TBUF 16
#وصف_ڪريو PRINT_BUF_LEN 64

#وصف_ڪريو va_شروعات(v, l) __builtin_va_start((v), l)
#وصف_ڪريو va_پڇاڙي __builtin_va_end
#وصف_ڪريو va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **ٻاهريون, u32 *ٻاهريون_len, حرف ch, صحيح flags)
{
	إذا (!ٻاهريون) {
		sbi_putc(ch);
		أرجع;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	إذا (!ٻاهريون_len || *ٻاهريون_len > 1) {
		*(*ٻاهريون)++ = ch;
		**ٻاهريون = '\0';
		إذا (ٻاهريون_len) {
			--(*ٻاهريون_len);
			إذا ((flags & USE_TBUF) && *ٻاهريون_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *ٻاهريون_len);
				*ٻاهريون = console_tbuf;
				*ٻاهريون_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساكن صحيح prints(حرف **ٻاهريون, u32 *ٻاهريون_len, ثابت حرف *string, صحيح ويڪر,
		  صحيح flags)
{
	صحيح pc = 0;
	ويڪر -= sbi_strlen(string);
	إذا (!(flags & PAD_ساڄو)) {
		لكل (; ويڪر > 0; --ويڪر) {
			printc(ٻاهريون, ٻاهريون_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	لكل (; *string; ++string) {
		printc(ٻاهريون, ٻاهريون_len, *string, flags);
		++pc;
	}
	لكل (; ويڪر > 0; --ويڪر) {
		printc(ٻاهريون, ٻاهريون_len, ' ', flags);
		++pc;
	}

	أرجع pc;
}

ساكن صحيح printi(حرف **ٻاهريون, u32 *ٻاهريون_len, ڊگهو ڊگهو i,
		  صحيح ويڪر, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	بنانشان ڊگهو ڊگهو u, b, t;

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
			printc(ٻاهريون, ٻاهريون_len, sign, flags);
			++pc;
			--ويڪر;
		}
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16 || b == 8) {
				printc(ٻاهريون, ٻاهريون_len, '0', flags);
				++pc;
				--ويڪر;
			}
			إذا (b == 16) {
				printc(ٻاهريون, ٻاهريون_len, 'x' - 'a' + letbase, flags);
				++pc;
				--ويڪر;
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

	أرجع pc + prints(ٻاهريون, ٻاهريون_len, s, ويڪر, flags);
}

ساكن صحيح print(حرف **ٻاهريون, u32 *ٻاهريون_len, ثابت حرف *format, va_list دليلون)
{
	bool flags_پورو;
	صحيح ويڪر, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!ٻاهريون) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	إذا (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		ٻاهريون = &tout;
		ٻاهريون_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	إذا (ٻاهريون) {
		إذا(!ٻاهريون_len || *ٻاهريون_len)
			**ٻاهريون = '\0';
	}

	لكل (; *format != 0; ++format) {
		ويڪر = flags = 0;
		إذا (use_tbuf)
			flags |= USE_TBUF;
		إذا (*format == '%') {
			++format;
			إذا (*format == '\0')
				اكسر;
			إذا (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_پورو = false;
			طالما (!flags_پورو) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_ساڄو;
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
					flags_پورو = true;
					اكسر;
				}
				إذا (!flags_پورو)
					++format;
			}
			إذا (flags & PAD_ساڄو)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				ويڪر *= 10;
				ويڪر += *format - '0';
			}
			إذا (*format == 's') {
				حرف *s = va_arg(دليلون, حرف *);
				pc += prints(ٻاهريون, ٻاهريون_len, s ? s : "(null)",
					     ويڪر, flags);
				استمر;
			}
			إذا ((*format == 'd') || (*format == 'i')) {
				pc += printi(ٻاهريون, ٻاهريون_len, va_arg(دليلون, صحيح),
					     ويڪر, flags, *format);
				استمر;
			}
			إذا ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(ٻاهريون, ٻاهريون_len, va_arg(دليلون, بنانشان صحيح),
					     ويڪر, flags, *format);
				استمر;
			}
			إذا ((*format == 'p') || (*format == 'P')) {
				pc += printi(ٻاهريون, ٻاهريون_len, (uintptr_t)va_arg(دليلون, فراغ*),
					     ويڪر, flags, *format);
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
					pc += printi(ٻاهريون, ٻاهريون_len, va_arg(دليلون, ڊگهو ڊگهو),
						ويڪر, flags, type);
					استمر;
				}
				إذا ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				إذا ((type == 'd') || (type == 'i'))
					pc += printi(ٻاهريون, ٻاهريون_len, va_arg(دليلون, ڊگهو),
					     ويڪر, flags, type);
				وإلا
					pc += printi(ٻاهريون, ٻاهريون_len, va_arg(دليلون, بنانشان ڊگهو),
					     ويڪر, flags, type);
				استمر;
			}
			إذا (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(دليلون, صحيح);
				scr[1] = '\0';
				pc += prints(ٻاهريون, ٻاهريون_len, scr, ويڪر, flags);
				استمر;
			}
		} وإلا {
literal:
			printc(ٻاهريون, ٻاهريون_len, *format, flags);
			++pc;
		}
	}

	إذا (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	أرجع pc;
}

صحيح sbi_sprintf(حرف *ٻاهريون, ثابت حرف *format, ...)
{
	va_list دليلون;
	صحيح retval;

	إذا (unlikely(!ٻاهريون))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_شروعات(دليلون, format);
	retval = print(&ٻاهريون, عدم, format, دليلون);
	va_پڇاڙي(دليلون);

	أرجع retval;
}

صحيح sbi_snprintf(حرف *ٻاهريون, u32 ٻاهريون_sz, ثابت حرف *format, ...)
{
	va_list دليلون;
	صحيح retval;

	إذا (unlikely(!ٻاهريون && ٻاهريون_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_شروعات(دليلون, format);
	retval = print(&ٻاهريون, &ٻاهريون_sz, format, دليلون);
	va_پڇاڙي(دليلون);

	أرجع retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list دليلون;
	صحيح retval;

	spin_تالو(&console_ٻاهريون_تالو);
	va_شروعات(دليلون, format);
	retval = print(عدم, عدم, format, دليلون);
	va_پڇاڙي(دليلون);
	spin_unlock(&console_ٻاهريون_تالو);

	أرجع retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list دليلون;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_شروعات(دليلون, format);
	إذا (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_تالو(&console_ٻاهريون_تالو);
		retval = print(عدم, عدم, format, دليلون);
		spin_unlock(&console_ٻاهريون_تالو);
	}
	va_پڇاڙي(دليلون);

	أرجع retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list دليلون;

	spin_تالو(&console_ٻاهريون_تالو);
	va_شروعات(دليلون, format);
	print(عدم, عدم, format, دليلون);
	va_پڇاڙي(دليلون);
	spin_unlock(&console_ٻاهريون_تالو);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	أرجع console_dev;
}

فراغ sbi_console_سيٽ_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	إذا (!dev)
		أرجع;

	إذا (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	إذا (flush_early_fifo) {
		طالما (!sbi_fifo_قطار_مان_ڪڍو(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
