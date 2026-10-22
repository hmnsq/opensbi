/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_fifo.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

স্থির ধ্রুবক কাঠামো sbi_console_device *console_dev = ফাঁকা;
স্থির অক্ষর console_tbuf[CONSOLE_TBUF_MAX];
স্থির u32 console_tbuf_len;
স্থির spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_বাফার_আকার
#define CONSOLE_EARLY_বাফার_আকার	CONFIG_CONSOLE_EARLY_বাফার_আকার
#অন্যথা
#define CONSOLE_EARLY_বাফার_আকার	256
#endif
স্থির অক্ষর console_early_বাফার[CONSOLE_EARLY_বাফার_আকার] = { 0 };
স্থির SBI_FIFO_DEFINE(console_early_fifo, console_early_বাফার, \
		       CONSOLE_EARLY_বাফার_আকার, আকার(অক্ষর));

bool sbi_isprintable(অক্ষর c)
{
	যদি (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ফেরত true;
	}
	ফেরত false;
}

পূর্ণসংখ্যা sbi_getc(শূন্য)
{
	যদি (console_dev && console_dev->console_getc)
		ফেরত console_dev->console_getc();
	ফেরত -1;
}

স্থির unsigned long nputs(ধ্রুবক অক্ষর *str, unsigned long len)
{
	অক্ষর ch;
	unsigned long i;

	যদি (console_dev) {
		যদি (console_dev->console_puts)
			ফেরত console_dev->console_puts(str, len);
		অন্যথা যদি (console_dev->console_putc) {
			জন্য (i = 0; i < len; i++) {
				যদি (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} অন্যথা {
		জন্য (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ফেরত len;
}

স্থির শূন্য nputs_all(ধ্রুবক অক্ষর *str, unsigned long len)
{
	unsigned long p = 0;

	যখন (p < len)
		p += nputs(&str[p], len - p);
}

শূন্য sbi_putc(অক্ষর ch)
{
	nputs_all(&ch, 1);
}

শূন্য sbi_puts(ধ্রুবক অক্ষর *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ধ্রুবক অক্ষর *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	ফেরত ret;
}

শূন্য sbi_gets(অক্ষর *s, পূর্ণসংখ্যা maxwidth, অক্ষর endchar)
{
	পূর্ণসংখ্যা ch;
	অক্ষর *retval = s;

	যখন ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (অক্ষর)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(অক্ষর *str, unsigned long len)
{
	পূর্ণসংখ্যা ch;
	unsigned long i;

	জন্য (i = 0; i < len; i++) {
		ch = sbi_getc();
		যদি (ch < 0)
			বিরতি;
		str[i] = ch;
	}

	ফেরত i;
}

#define PAD_ডান 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
প্রকারলেখ __builtin_va_list va_list;

স্থির শূন্য printc(অক্ষর **out, u32 *out_len, অক্ষর ch, পূর্ণসংখ্যা flags)
{
	যদি (!out) {
		sbi_putc(ch);
		ফেরত;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	যদি (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		যদি (out_len) {
			--(*out_len);
			যদি ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

স্থির পূর্ণসংখ্যা prints(অক্ষর **out, u32 *out_len, ধ্রুবক অক্ষর *string, পূর্ণসংখ্যা width,
		  পূর্ণসংখ্যা flags)
{
	পূর্ণসংখ্যা pc = 0;
	width -= sbi_strlen(string);
	যদি (!(flags & PAD_ডান)) {
		জন্য (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	জন্য (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	জন্য (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	ফেরত pc;
}

স্থির পূর্ণসংখ্যা printi(অক্ষর **out, u32 *out_len, long long i,
		  পূর্ণসংখ্যা width, পূর্ণসংখ্যা flags, পূর্ণসংখ্যা type)
{
	পূর্ণসংখ্যা pc = 0;
	অক্ষর *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	যদি (type == 'o')
		b = 8;
	অন্যথা যদি (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	যদি (type == 'i' || type == 'd') {
		যদি ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		যদি (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	যদি (!u) {
		*--s = '0';
	} অন্যথা {
		যখন (u) {
			t = u % b;
			u = u / b;
			যদি (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	যদি (flags & PAD_ZERO) {
		যদি (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		যদি (i && (flags & PAD_ALTERNATE)) {
			যদি (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			যদি (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} অন্যথা {
		যদি (i && (flags & PAD_ALTERNATE)) {
			যদি (b == 16)
				*--s = 'x' - 'a' + letbase;
			যদি (b == 16 || b == 8)
				*--s = '0';
		}
		যদি (sign)
			*--s = sign;
	}

	ফেরত pc + prints(out, out_len, s, width, flags);
}

স্থির পূর্ণসংখ্যা print(অক্ষর **out, u32 *out_len, ধ্রুবক অক্ষর *format, va_list args)
{
	bool flags_done;
	পূর্ণসংখ্যা width, flags, pc = 0;
	অক্ষর type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	যদি (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	যদি (out) {
		যদি(!out_len || *out_len)
			**out = '\0';
	}

	জন্য (; *format != 0; ++format) {
		width = flags = 0;
		যদি (use_tbuf)
			flags |= USE_TBUF;
		যদি (*format == '%') {
			++format;
			যদি (*format == '\0')
				বিরতি;
			যদি (*format == '%')
				যাও literal;
			/* Get flags */
			flags_done = false;
			যখন (!flags_done) {
				নির্বাচন (*format) {
				ক্ষেত্র '-':
					flags |= PAD_ডান;
					বিরতি;
				ক্ষেত্র '+':
					flags |= PAD_SIGN;
					বিরতি;
				ক্ষেত্র '#':
					flags |= PAD_ALTERNATE;
					বিরতি;
				ক্ষেত্র '0':
					flags |= PAD_ZERO;
					বিরতি;
				ক্ষেত্র ' ':
				ক্ষেত্র '\'':
					/* Ignored flags, do nothing */
					বিরতি;
				পূর্বনির্ধারিত:
					flags_done = true;
					বিরতি;
				}
				যদি (!flags_done)
					++format;
			}
			যদি (flags & PAD_ডান)
				flags &= ~PAD_ZERO;
			/* Get width */
			জন্য (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			যদি (*format == 's') {
				অক্ষর *s = va_arg(args, অক্ষর *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				চালু;
			}
			যদি ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, পূর্ণসংখ্যা),
					     width, flags, *format);
				চালু;
			}
			যদি ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned পূর্ণসংখ্যা),
					     width, flags, *format);
				চালু;
			}
			যদি ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, শূন্য*),
					     width, flags, *format);
				চালু;
			}
			যদি (*format == 'l') {
				type = 'i';
				যদি (format[1] == 'l') {
					++format;
					যদি ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					চালু;
				}
				যদি ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				যদি ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				অন্যথা
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				চালু;
			}
			যদি (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, পূর্ণসংখ্যা);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				চালু;
			}
		} অন্যথা {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	যদি (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ফেরত pc;
}

পূর্ণসংখ্যা sbi_sprintf(অক্ষর *out, ধ্রুবক অক্ষর *format, ...)
{
	va_list args;
	পূর্ণসংখ্যা retval;

	যদি (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, ফাঁকা, format, args);
	va_end(args);

	ফেরত retval;
}

পূর্ণসংখ্যা sbi_snprintf(অক্ষর *out, u32 out_sz, ধ্রুবক অক্ষর *format, ...)
{
	va_list args;
	পূর্ণসংখ্যা retval;

	যদি (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ফেরত retval;
}

পূর্ণসংখ্যা sbi_printf(ধ্রুবক অক্ষর *format, ...)
{
	va_list args;
	পূর্ণসংখ্যা retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(ফাঁকা, ফাঁকা, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ফেরত retval;
}

পূর্ণসংখ্যা sbi_dprintf(ধ্রুবক অক্ষর *format, ...)
{
	va_list args;
	পূর্ণসংখ্যা retval = 0;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	যদি (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(ফাঁকা, ফাঁকা, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ফেরত retval;
}

শূন্য sbi_panic(ধ্রুবক অক্ষর *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(ফাঁকা, ফাঁকা, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ধ্রুবক কাঠামো sbi_console_device *sbi_console_get_device(শূন্য)
{
	ফেরত console_dev;
}

শূন্য sbi_console_set_device(ধ্রুবক কাঠামো sbi_console_device *dev)
{
	অক্ষর ch;
	bool flush_early_fifo = false;

	যদি (!dev)
		ফেরত;

	যদি (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	যদি (flush_early_fifo) {
		যখন (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
