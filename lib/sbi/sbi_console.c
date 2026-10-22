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

#define CONSOLE_TBUF_MAX 256

ساكن ثابت بنية sbi_console_device *console_dev = عدم;
ساكن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساكن u32 console_tbuf_len;
ساكن spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#وإلا
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
ساكن حرف console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, حجم(حرف));

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

ساكن unsigned long nputs(ثابت حرف *str, unsigned long len)
{
	حرف ch;
	unsigned long i;

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
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	أرجع len;
}

ساكن فراغ nputs_all(ثابت حرف *str, unsigned long len)
{
	unsigned long p = 0;

	طالما (p < len)
		p += nputs(&str[p], len - p);
}

فراغ sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

فراغ sbi_puts(ثابت حرف *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ثابت حرف *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

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

unsigned long sbi_ngets(حرف *str, unsigned long len)
{
	صحيح ch;
	unsigned long i;

	لكل (i = 0; i < len; i++) {
		ch = sbi_getc();
		إذا (ch < 0)
			اكسر;
		str[i] = ch;
	}

	أرجع i;
}

#define PAD_يمين 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define اطبع_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **out, u32 *out_len, حرف ch, صحيح flags)
{
	إذا (!out) {
		sbi_putc(ch);
		أرجع;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	إذا (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		إذا (out_len) {
			--(*out_len);
			إذا ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساكن صحيح prints(حرف **out, u32 *out_len, ثابت حرف *string, صحيح width,
		  صحيح flags)
{
	صحيح pc = 0;
	width -= sbi_strlen(string);
	إذا (!(flags & PAD_يمين)) {
		لكل (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	لكل (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	لكل (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	أرجع pc;
}

ساكن صحيح printi(حرف **out, u32 *out_len, long long i,
		  صحيح width, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, اطبع_buf[اطبع_BUF_LEN];
	unsigned long long u, b, t;

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

	s  = اطبع_buf + اطبع_BUF_LEN - 1;
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
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		إذا (i && (flags & PAD_ALTERNATE)) {
			إذا (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			إذا (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
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

	أرجع pc + prints(out, out_len, s, width, flags);
}

ساكن صحيح اطبع(حرف **out, u32 *out_len, ثابت حرف *format, va_list وسائط)
{
	bool flags_done;
	صحيح width, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	إذا (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	إذا (out) {
		إذا(!out_len || *out_len)
			**out = '\0';
	}

	لكل (; *format != 0; ++format) {
		width = flags = 0;
		إذا (use_tbuf)
			flags |= USE_TBUF;
		إذا (*format == '%') {
			++format;
			إذا (*format == '\0')
				اكسر;
			إذا (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_done = false;
			طالما (!flags_done) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_يمين;
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
					flags_done = true;
					اكسر;
				}
				إذا (!flags_done)
					++format;
			}
			إذا (flags & PAD_يمين)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			إذا (*format == 's') {
				حرف *s = va_arg(وسائط, حرف *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				استمر;
			}
			إذا ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(وسائط, صحيح),
					     width, flags, *format);
				استمر;
			}
			إذا ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(وسائط, unsigned صحيح),
					     width, flags, *format);
				استمر;
			}
			إذا ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(وسائط, فراغ*),
					     width, flags, *format);
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
					pc += printi(out, out_len, va_arg(وسائط, long long),
						width, flags, type);
					استمر;
				}
				إذا ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				إذا ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(وسائط, long),
					     width, flags, type);
				وإلا
					pc += printi(out, out_len, va_arg(وسائط, unsigned long),
					     width, flags, type);
				استمر;
			}
			إذا (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(وسائط, صحيح);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				استمر;
			}
		} وإلا {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	إذا (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	أرجع pc;
}

صحيح sbi_sprintf(حرف *out, ثابت حرف *format, ...)
{
	va_list وسائط;
	صحيح retval;

	إذا (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(وسائط, format);
	retval = اطبع(&out, عدم, format, وسائط);
	va_end(وسائط);

	أرجع retval;
}

صحيح sbi_snprintf(حرف *out, u32 out_sz, ثابت حرف *format, ...)
{
	va_list وسائط;
	صحيح retval;

	إذا (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(وسائط, format);
	retval = اطبع(&out, &out_sz, format, وسائط);
	va_end(وسائط);

	أرجع retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list وسائط;
	صحيح retval;

	spin_lock(&console_out_lock);
	va_start(وسائط, format);
	retval = اطبع(عدم, عدم, format, وسائط);
	va_end(وسائط);
	spin_unlock(&console_out_lock);

	أرجع retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list وسائط;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(وسائط, format);
	إذا (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = اطبع(عدم, عدم, format, وسائط);
		spin_unlock(&console_out_lock);
	}
	va_end(وسائط);

	أرجع retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list وسائط;

	spin_lock(&console_out_lock);
	va_start(وسائط, format);
	اطبع(عدم, عدم, format, وسائط);
	va_end(وسائط);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	أرجع console_dev;
}

فراغ sbi_console_set_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	إذا (!dev)
		أرجع;

	إذا (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	إذا (flush_early_fifo) {
		طالما (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
