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
#ܐܚܪܢܐ
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
ساكن حرف console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
ساكن SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, حجم(حرف));

bool sbi_isprintable(حرف c)
{
	ܐܢ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ܦܢܐ true;
	}
	ܦܢܐ false;
}

صحيح sbi_getc(فراغ)
{
	ܐܢ (console_dev && console_dev->console_getc)
		ܦܢܐ console_dev->console_getc();
	ܦܢܐ -1;
}

ساكن unsigned long nputs(ثابت حرف *str, unsigned long len)
{
	حرف ch;
	unsigned long i;

	ܐܢ (console_dev) {
		ܐܢ (console_dev->console_puts)
			ܦܢܐ console_dev->console_puts(str, len);
		ܐܚܪܢܐ ܐܢ (console_dev->console_putc) {
			لكل (i = 0; i < len; i++) {
				ܐܢ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ܐܚܪܢܐ {
		لكل (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ܦܢܐ len;
}

ساكن فراغ nputs_all(ثابت حرف *str, unsigned long len)
{
	unsigned long p = 0;

	ܟܡܐ (p < len)
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

	ܦܢܐ ret;
}

فراغ sbi_gets(حرف *s, صحيح maxwidth, حرف endchar)
{
	صحيح ch;
	حرف *retval = s;

	ܟܡܐ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
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
		ܐܢ (ch < 0)
			اكسر;
		str[i] = ch;
	}

	ܦܢܐ i;
}

#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
تعريفنوع __builtin_va_list va_list;

ساكن فراغ printc(حرف **out, u32 *out_len, حرف ch, صحيح flags)
{
	ܐܢ (!out) {
		sbi_putc(ch);
		ܦܢܐ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ܐܢ (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		ܐܢ (out_len) {
			--(*out_len);
			ܐܢ ((flags & USE_TBUF) && *out_len == 1) {
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
	ܐܢ (!(flags & PAD_RIGHT)) {
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

	ܦܢܐ pc;
}

ساكن صحيح printi(حرف **out, u32 *out_len, long long i,
		  صحيح width, صحيح flags, صحيح type)
{
	صحيح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	ܐܢ (type == 'o')
		b = 8;
	ܐܚܪܢܐ ܐܢ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ܐܢ (type == 'i' || type == 'd') {
		ܐܢ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ܐܢ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ܐܢ (!u) {
		*--s = '0';
	} ܐܚܪܢܐ {
		ܟܡܐ (u) {
			t = u % b;
			u = u / b;
			ܐܢ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ܐܢ (flags & PAD_ZERO) {
		ܐܢ (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		ܐܢ (i && (flags & PAD_ALTERNATE)) {
			ܐܢ (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			ܐܢ (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} ܐܚܪܢܐ {
		ܐܢ (i && (flags & PAD_ALTERNATE)) {
			ܐܢ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ܐܢ (b == 16 || b == 8)
				*--s = '0';
		}
		ܐܢ (sign)
			*--s = sign;
	}

	ܦܢܐ pc + prints(out, out_len, s, width, flags);
}

ساكن صحيح print(حرف **out, u32 *out_len, ثابت حرف *format, va_list args)
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
	ܐܢ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ܐܢ (out) {
		ܐܢ(!out_len || *out_len)
			**out = '\0';
	}

	لكل (; *format != 0; ++format) {
		width = flags = 0;
		ܐܢ (use_tbuf)
			flags |= USE_TBUF;
		ܐܢ (*format == '%') {
			++format;
			ܐܢ (*format == '\0')
				اكسر;
			ܐܢ (*format == '%')
				اذهب literal;
			/* Get flags */
			flags_done = false;
			ܟܡܐ (!flags_done) {
				بدل (*format) {
				حالة '-':
					flags |= PAD_RIGHT;
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
				ܐܢ (!flags_done)
					++format;
			}
			ܐܢ (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			لكل (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			ܐܢ (*format == 's') {
				حرف *s = va_arg(args, حرف *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				استمر;
			}
			ܐܢ ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, صحيح),
					     width, flags, *format);
				استمر;
			}
			ܐܢ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned صحيح),
					     width, flags, *format);
				استمر;
			}
			ܐܢ ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, فراغ*),
					     width, flags, *format);
				استمر;
			}
			ܐܢ (*format == 'l') {
				type = 'i';
				ܐܢ (format[1] == 'l') {
					++format;
					ܐܢ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					استمر;
				}
				ܐܢ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ܐܢ ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				ܐܚܪܢܐ
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				استمر;
			}
			ܐܢ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, صحيح);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				استمر;
			}
		} ܐܚܪܢܐ {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	ܐܢ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ܦܢܐ pc;
}

صحيح sbi_sprintf(حرف *out, ثابت حرف *format, ...)
{
	va_list args;
	صحيح retval;

	ܐܢ (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, عدم, format, args);
	va_end(args);

	ܦܢܐ retval;
}

صحيح sbi_snprintf(حرف *out, u32 out_sz, ثابت حرف *format, ...)
{
	va_list args;
	صحيح retval;

	ܐܢ (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ܦܢܐ retval;
}

صحيح sbi_printf(ثابت حرف *format, ...)
{
	va_list args;
	صحيح retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(عدم, عدم, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ܦܢܐ retval;
}

صحيح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list args;
	صحيح retval = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	ܐܢ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(عدم, عدم, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ܦܢܐ retval;
}

فراغ sbi_panic(ثابت حرف *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(عدم, عدم, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ثابت بنية sbi_console_device *sbi_console_get_device(فراغ)
{
	ܦܢܐ console_dev;
}

فراغ sbi_console_set_device(ثابت بنية sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	ܐܢ (!dev)
		ܦܢܐ;

	ܐܢ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ܐܢ (flush_early_fifo) {
		ܟܡܐ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
