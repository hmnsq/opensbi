/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_fifo.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

ایستا ثابت ساختار sbi_console_device *console_dev = پوچ;
ایستا نویسه console_tbuf[CONSOLE_TBUF_MAX];
ایستا u32 console_tbuf_len;
ایستا spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#وگرنه
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
ایستا نویسه console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
ایستا SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, اندازه(نویسه));

bool sbi_isprintable(نویسه c)
{
	اگر (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		بازگشت true;
	}
	بازگشت false;
}

صحیح sbi_getc(تهی)
{
	اگر (console_dev && console_dev->console_getc)
		بازگشت console_dev->console_getc();
	بازگشت -1;
}

ایستا unsigned long nputs(ثابت نویسه *str, unsigned long len)
{
	نویسه ch;
	unsigned long i;

	اگر (console_dev) {
		اگر (console_dev->console_puts)
			بازگشت console_dev->console_puts(str, len);
		وگرنه اگر (console_dev->console_putc) {
			برای (i = 0; i < len; i++) {
				اگر (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} وگرنه {
		برای (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	بازگشت len;
}

ایستا تهی nputs_all(ثابت نویسه *str, unsigned long len)
{
	unsigned long p = 0;

	وقتی (p < len)
		p += nputs(&str[p], len - p);
}

تهی sbi_putc(نویسه ch)
{
	nputs_all(&ch, 1);
}

تهی sbi_puts(ثابت نویسه *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ثابت نویسه *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	بازگشت ret;
}

تهی sbi_gets(نویسه *s, صحیح maxwidth, نویسه endchar)
{
	صحیح ch;
	نویسه *retval = s;

	وقتی ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (نویسه)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(نویسه *str, unsigned long len)
{
	صحیح ch;
	unsigned long i;

	برای (i = 0; i < len; i++) {
		ch = sbi_getc();
		اگر (ch < 0)
			بشکن;
		str[i] = ch;
	}

	بازگشت i;
}

#define PAD_راست 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
تعریفنوع __builtin_va_list va_list;

ایستا تهی printc(نویسه **out, u32 *out_len, نویسه ch, صحیح flags)
{
	اگر (!out) {
		sbi_putc(ch);
		بازگشت;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	اگر (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		اگر (out_len) {
			--(*out_len);
			اگر ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ایستا صحیح prints(نویسه **out, u32 *out_len, ثابت نویسه *string, صحیح width,
		  صحیح flags)
{
	صحیح pc = 0;
	width -= sbi_strlen(string);
	اگر (!(flags & PAD_راست)) {
		برای (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	برای (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	برای (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	بازگشت pc;
}

ایستا صحیح printi(نویسه **out, u32 *out_len, long long i,
		  صحیح width, صحیح flags, صحیح type)
{
	صحیح pc = 0;
	نویسه *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	اگر (type == 'o')
		b = 8;
	وگرنه اگر (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	اگر (type == 'i' || type == 'd') {
		اگر ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		اگر (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	اگر (!u) {
		*--s = '0';
	} وگرنه {
		وقتی (u) {
			t = u % b;
			u = u / b;
			اگر (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	اگر (flags & PAD_ZERO) {
		اگر (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		اگر (i && (flags & PAD_ALTERNATE)) {
			اگر (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			اگر (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} وگرنه {
		اگر (i && (flags & PAD_ALTERNATE)) {
			اگر (b == 16)
				*--s = 'x' - 'a' + letbase;
			اگر (b == 16 || b == 8)
				*--s = '0';
		}
		اگر (sign)
			*--s = sign;
	}

	بازگشت pc + prints(out, out_len, s, width, flags);
}

ایستا صحیح print(نویسه **out, u32 *out_len, ثابت نویسه *format, va_list args)
{
	bool flags_done;
	صحیح width, flags, pc = 0;
	نویسه type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	اگر (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	اگر (out) {
		اگر(!out_len || *out_len)
			**out = '\0';
	}

	برای (; *format != 0; ++format) {
		width = flags = 0;
		اگر (use_tbuf)
			flags |= USE_TBUF;
		اگر (*format == '%') {
			++format;
			اگر (*format == '\0')
				بشکن;
			اگر (*format == '%')
				برو literal;
			/* Get flags */
			flags_done = false;
			وقتی (!flags_done) {
				انتخاب (*format) {
				حالت '-':
					flags |= PAD_راست;
					بشکن;
				حالت '+':
					flags |= PAD_SIGN;
					بشکن;
				حالت '#':
					flags |= PAD_ALTERNATE;
					بشکن;
				حالت '0':
					flags |= PAD_ZERO;
					بشکن;
				حالت ' ':
				حالت '\'':
					/* Ignored flags, do nothing */
					بشکن;
				پیشفرض:
					flags_done = true;
					بشکن;
				}
				اگر (!flags_done)
					++format;
			}
			اگر (flags & PAD_راست)
				flags &= ~PAD_ZERO;
			/* Get width */
			برای (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			اگر (*format == 's') {
				نویسه *s = va_arg(args, نویسه *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				ادامه;
			}
			اگر ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, صحیح),
					     width, flags, *format);
				ادامه;
			}
			اگر ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned صحیح),
					     width, flags, *format);
				ادامه;
			}
			اگر ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, تهی*),
					     width, flags, *format);
				ادامه;
			}
			اگر (*format == 'l') {
				type = 'i';
				اگر (format[1] == 'l') {
					++format;
					اگر ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					ادامه;
				}
				اگر ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				اگر ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				وگرنه
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				ادامه;
			}
			اگر (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, صحیح);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				ادامه;
			}
		} وگرنه {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	اگر (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	بازگشت pc;
}

صحیح sbi_sprintf(نویسه *out, ثابت نویسه *format, ...)
{
	va_list args;
	صحیح retval;

	اگر (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, پوچ, format, args);
	va_end(args);

	بازگشت retval;
}

صحیح sbi_snprintf(نویسه *out, u32 out_sz, ثابت نویسه *format, ...)
{
	va_list args;
	صحیح retval;

	اگر (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	بازگشت retval;
}

صحیح sbi_printf(ثابت نویسه *format, ...)
{
	va_list args;
	صحیح retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(پوچ, پوچ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	بازگشت retval;
}

صحیح sbi_dprintf(ثابت نویسه *format, ...)
{
	va_list args;
	صحیح retval = 0;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	اگر (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(پوچ, پوچ, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	بازگشت retval;
}

تهی sbi_panic(ثابت نویسه *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(پوچ, پوچ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ثابت ساختار sbi_console_device *sbi_console_get_device(تهی)
{
	بازگشت console_dev;
}

تهی sbi_console_set_device(ثابت ساختار sbi_console_device *dev)
{
	نویسه ch;
	bool flush_early_fifo = false;

	اگر (!dev)
		بازگشت;

	اگر (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	اگر (flush_early_fifo) {
		وقتی (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
