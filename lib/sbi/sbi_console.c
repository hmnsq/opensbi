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

#تعریف_کرو CONSOLE_TBUF_MAX 256

ساكن ثابت بنية sbi_console_device *console_dev = عدم;
ساكن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساكن u32 console_tbuf_len;
ساكن spinlock_t console_باہرلا_جندرا	       = SPIN_جندرا_INITIALIZER;

#جے_تعریف_شدہ CONFIG_CONSOLE_EARLY_بفر_ناپ
#تعریف_کرو CONSOLE_EARLY_بفر_ناپ	CONFIG_CONSOLE_EARLY_بفر_ناپ
#وإلا
#تعریف_کرو CONSOLE_EARLY_بفر_ناپ	256
#جے_انت
ساكن حرف console_early_بفر[CONSOLE_EARLY_بفر_ناپ] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_بفر, \
		       CONSOLE_EARLY_بفر_ناپ, حجم(حرف));

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

ساكن بنانشانی لما nputs(ثابت حرف *str, بنانشانی لما len)
{
	حرف ch;
	بنانشانی لما i;

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
			sbi_fifo_قطار_وچ_پاؤ(&console_early_fifo, &ch, true);
		}
	}
	أرجع len;
}

ساكن فراغ nputs_all(ثابت حرف *str, بنانشانی لما len)
{
	بنانشانی لما p = 0;

	طالما (p < len)
		p += nputs(&str[p], len - p);
}

فراغ sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

فراغ sbi_puts(ثابت حرف *str)
{
	بنانشانی لما len = sbi_strlen(str);

	spin_جندرا(&console_باہرلا_جندرا);
	nputs_all(str, len);
	spin_unlock(&console_باہرلا_جندرا);
}

بنانشانی لما sbi_nputs(ثابت حرف *str, بنانشانی لما len)
{
	بنانشانی لما ret;

	spin_جندرا(&console_باہرلا_جندرا);
	ret = nputs(str, len);
	spin_unlock(&console_باہرلا_جندرا);

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

بنانشانی لما sbi_ngets(حرف *str, بنانشانی لما len)
{
	صحيح ch;
	بنانشانی لما i;

	لكل (i = 0; i < len; i++) {
		ch = sbi_getc();
		إذا (ch < 0)
			اكسر;
		str[i] = ch;
	}

	أرجع i;
}

#تعریف_کرو PAD_دایاں 1
#تعریف_کرو PAD_ZERO 2
#تعریف_کرو PAD_ALTERNATE 4
#تعریف_کرو PAD_SIGN 8
#تعریف_کرو USE_TBUF 16
#تعریف_کرو PRINT_BUF_LEN 64

#تعریف_کرو va_شروع(v, l) __builtin_va_start((v), l)
#تعریف_کرو va_سرا __builtin_va_end
#تعریف_کرو va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **باہرلا, u32 *باہرلا_len, حرف ch, صحيح flags)
{
	إذا (!باہرلا) {
		sbi_putc(ch);
		أرجع;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	إذا (!باہرلا_len || *باہرلا_len > 1) {
		*(*باہرلا)++ = ch;
		**باہرلا = '\0';
		إذا (باہرلا_len) {
			--(*باہرلا_len);
			إذا ((flags & USE_TBUF) && *باہرلا_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *باہرلا_len);
				*باہرلا = console_tbuf;
				*باہرلا_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساكن صحيح prints(حرف **باہرلا, u32 *باہرلا_len, ثابت حرف *string, صحيح چوڑائی,
		  صحيح flags)
{
	صحيح pc = 0;
	چوڑائی -= sbi_strlen(string);
	إذا (!(flags & PAD_دایاں)) {
		لكل (; چوڑائی > 0; --چوڑائی) {
			printc(باہرلا, باہرلا_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	لكل (; *string; ++string) {
		printc(باہرلا, باہرلا_len, *string, flags);
		++pc;
	}
	لكل (; چوڑائی > 0; --چوڑائی) {
		printc(باہرلا, باہرلا_len, ' ', flags);
		++pc;
	}

	أرجع pc;
}

ساكن صحيح printi(حرف **باہرلا, u32 *باہرلا_len, لما لما i,
		  صحيح چوڑائی, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	بنانشانی لما لما u, b, t;

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
			printc(باہرلا, باہرلا_len, sign, flags);
			++pc;
			--چوڑائی;
		}
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16 || b == 8) {
				printc(باہرلا, باہرلا_len, '0', flags);
				++pc;
				--چوڑائی;
			}
			إذا (b == 16) {
				printc(باہرلا, باہرلا_len, 'x' - 'a' + letbase, flags);
				++pc;
				--چوڑائی;
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

	أرجع pc + prints(باہرلا, باہرلا_len, s, چوڑائی, flags);
}

ساكن صحيح print(حرف **باہرلا, u32 *باہرلا_len, ثابت حرف *format, va_list دلیلاں)
{
	bool flags_پورا;
	صحيح چوڑائی, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!باہرلا) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	إذا (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		باہرلا = &tout;
		باہرلا_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	إذا (باہرلا) {
		إذا(!باہرلا_len || *باہرلا_len)
			**باہرلا = '\0';
	}

	لكل (; *format != 0; ++format) {
		چوڑائی = flags = 0;
		إذا (use_tbuf)
			flags |= USE_TBUF;
		إذا (*format == '%') {
			++format;
			إذا (*format == '\0')
				اكسر;
			إذا (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_پورا = false;
			طالما (!flags_پورا) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_دایاں;
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
					flags_پورا = true;
					اكسر;
				}
				إذا (!flags_پورا)
					++format;
			}
			إذا (flags & PAD_دایاں)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				چوڑائی *= 10;
				چوڑائی += *format - '0';
			}
			إذا (*format == 's') {
				حرف *s = va_arg(دلیلاں, حرف *);
				pc += prints(باہرلا, باہرلا_len, s ? s : "(null)",
					     چوڑائی, flags);
				استمر;
			}
			إذا ((*format == 'd') || (*format == 'i')) {
				pc += printi(باہرلا, باہرلا_len, va_arg(دلیلاں, صحيح),
					     چوڑائی, flags, *format);
				استمر;
			}
			إذا ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(باہرلا, باہرلا_len, va_arg(دلیلاں, بنانشانی صحيح),
					     چوڑائی, flags, *format);
				استمر;
			}
			إذا ((*format == 'p') || (*format == 'P')) {
				pc += printi(باہرلا, باہرلا_len, (uintptr_t)va_arg(دلیلاں, فراغ*),
					     چوڑائی, flags, *format);
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
					pc += printi(باہرلا, باہرلا_len, va_arg(دلیلاں, لما لما),
						چوڑائی, flags, type);
					استمر;
				}
				إذا ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				إذا ((type == 'd') || (type == 'i'))
					pc += printi(باہرلا, باہرلا_len, va_arg(دلیلاں, لما),
					     چوڑائی, flags, type);
				وإلا
					pc += printi(باہرلا, باہرلا_len, va_arg(دلیلاں, بنانشانی لما),
					     چوڑائی, flags, type);
				استمر;
			}
			إذا (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(دلیلاں, صحيح);
				scr[1] = '\0';
				pc += prints(باہرلا, باہرلا_len, scr, چوڑائی, flags);
				استمر;
			}
		} وإلا {
literal:
			printc(باہرلا, باہرلا_len, *format, flags);
			++pc;
		}
	}

	إذا (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	أرجع pc;
}

صحيح sbi_sprintf(حرف *باہرلا, ثابت حرف *format, ...)
{
	va_list دلیلاں;
	صحيح retval;

	إذا (unlikely(!باہرلا))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_شروع(دلیلاں, format);
	retval = print(&باہرلا, عدم, format, دلیلاں);
	va_سرا(دلیلاں);

	أرجع retval;
}

صحيح sbi_snprintf(حرف *باہرلا, u32 باہرلا_sz, ثابت حرف *format, ...)
{
	va_list دلیلاں;
	صحيح retval;

	إذا (unlikely(!باہرلا && باہرلا_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_شروع(دلیلاں, format);
	retval = print(&باہرلا, &باہرلا_sz, format, دلیلاں);
	va_سرا(دلیلاں);

	أرجع retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list دلیلاں;
	صحيح retval;

	spin_جندرا(&console_باہرلا_جندرا);
	va_شروع(دلیلاں, format);
	retval = print(عدم, عدم, format, دلیلاں);
	va_سرا(دلیلاں);
	spin_unlock(&console_باہرلا_جندرا);

	أرجع retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list دلیلاں;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_شروع(دلیلاں, format);
	إذا (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_جندرا(&console_باہرلا_جندرا);
		retval = print(عدم, عدم, format, دلیلاں);
		spin_unlock(&console_باہرلا_جندرا);
	}
	va_سرا(دلیلاں);

	أرجع retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list دلیلاں;

	spin_جندرا(&console_باہرلا_جندرا);
	va_شروع(دلیلاں, format);
	print(عدم, عدم, format, دلیلاں);
	va_سرا(دلیلاں);
	spin_unlock(&console_باہرلا_جندرا);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	أرجع console_dev;
}

فراغ sbi_console_سیٹ_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	إذا (!dev)
		أرجع;

	إذا (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	إذا (flush_early_fifo) {
		طالما (!sbi_fifo_قطار_توں_کڈھو(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
