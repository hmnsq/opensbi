/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_locks.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_fifo.h>
#অন্তর্ভাৱ <sbi/sbi_hart.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

স্থির ধ্রুৱ সংরচনা sbi_console_device *console_dev = রিক্ত;
স্থির ৱর্ণ console_tbuf[CONSOLE_TBUF_MAX];
স্থির u32 console_tbuf_len;
স্থির spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#অন্যথা
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
স্থির ৱর্ণ console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
স্থির SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, পরিমাণ(ৱর্ণ));

bool sbi_isprintable(ৱর্ণ c)
{
	যদি (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ঘূৰাই true;
	}
	ঘূৰাই false;
}

পূর্ণাঙ্ক sbi_getc(শূন্য)
{
	যদি (console_dev && console_dev->console_getc)
		ঘূৰাই console_dev->console_getc();
	ঘূৰাই -1;
}

স্থির unsigned long nputs(ধ্রুৱ ৱর্ণ *str, unsigned long len)
{
	ৱর্ণ ch;
	unsigned long i;

	যদি (console_dev) {
		যদি (console_dev->console_puts)
			ঘূৰাই console_dev->console_puts(str, len);
		অন্যথা যদি (console_dev->console_putc) {
			বাবে (i = 0; i < len; i++) {
				যদি (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} অন্যথা {
		বাবে (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ঘূৰাই len;
}

স্থির শূন্য nputs_all(ধ্রুৱ ৱর্ণ *str, unsigned long len)
{
	unsigned long p = 0;

	যেতিয়ালৈ (p < len)
		p += nputs(&str[p], len - p);
}

শূন্য sbi_putc(ৱর্ণ ch)
{
	nputs_all(&ch, 1);
}

শূন্য sbi_puts(ধ্রুৱ ৱর্ণ *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ধ্রুৱ ৱর্ণ *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	ঘূৰাই ret;
}

শূন্য sbi_gets(ৱর্ণ *s, পূর্ণাঙ্ক maxwidth, ৱর্ণ endchar)
{
	পূর্ণাঙ্ক ch;
	ৱর্ণ *retval = s;

	যেতিয়ালৈ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (ৱর্ণ)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(ৱর্ণ *str, unsigned long len)
{
	পূর্ণাঙ্ক ch;
	unsigned long i;

	বাবে (i = 0; i < len; i++) {
		ch = sbi_getc();
		যদি (ch < 0)
			ভঙ্গ;
		str[i] = ch;
	}

	ঘূৰাই i;
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
প্রকারনির্দেশ __builtin_va_list va_list;

স্থির শূন্য printc(ৱর্ণ **out, u32 *out_len, ৱর্ণ ch, পূর্ণাঙ্ক flags)
{
	যদি (!out) {
		sbi_putc(ch);
		ঘূৰাই;
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

স্থির পূর্ণাঙ্ক prints(ৱর্ণ **out, u32 *out_len, ধ্রুৱ ৱর্ণ *string, পূর্ণাঙ্ক width,
		  পূর্ণাঙ্ক flags)
{
	পূর্ণাঙ্ক pc = 0;
	width -= sbi_strlen(string);
	যদি (!(flags & PAD_ডান)) {
		বাবে (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	বাবে (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	বাবে (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	ঘূৰাই pc;
}

স্থির পূর্ণাঙ্ক printi(ৱর্ণ **out, u32 *out_len, long long i,
		  পূর্ণাঙ্ক width, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক type)
{
	পূর্ণাঙ্ক pc = 0;
	ৱর্ণ *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
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
		যেতিয়ালৈ (u) {
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

	ঘূৰাই pc + prints(out, out_len, s, width, flags);
}

স্থির পূর্ণাঙ্ক print(ৱর্ণ **out, u32 *out_len, ধ্রুৱ ৱর্ণ *format, va_list args)
{
	bool flags_done;
	পূর্ণাঙ্ক width, flags, pc = 0;
	ৱর্ণ type, scr[2], *tout;
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

	বাবে (; *format != 0; ++format) {
		width = flags = 0;
		যদি (use_tbuf)
			flags |= USE_TBUF;
		যদি (*format == '%') {
			++format;
			যদি (*format == '\0')
				ভঙ্গ;
			যদি (*format == '%')
				গচ্ছ literal;
			/* Get flags */
			flags_done = false;
			যেতিয়ালৈ (!flags_done) {
				ৱিকল্প (*format) {
				পক্ষ '-':
					flags |= PAD_ডান;
					ভঙ্গ;
				পক্ষ '+':
					flags |= PAD_SIGN;
					ভঙ্গ;
				পক্ষ '#':
					flags |= PAD_ALTERNATE;
					ভঙ্গ;
				পক্ষ '0':
					flags |= PAD_ZERO;
					ভঙ্গ;
				পক্ষ ' ':
				পক্ষ '\'':
					/* Ignored flags, do nothing */
					ভঙ্গ;
				স্ৱভাৱ:
					flags_done = true;
					ভঙ্গ;
				}
				যদি (!flags_done)
					++format;
			}
			যদি (flags & PAD_ডান)
				flags &= ~PAD_ZERO;
			/* Get width */
			বাবে (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			যদি (*format == 's') {
				ৱর্ণ *s = va_arg(args, ৱর্ণ *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				অনুৱর্ত;
			}
			যদি ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, পূর্ণাঙ্ক),
					     width, flags, *format);
				অনুৱর্ত;
			}
			যদি ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned পূর্ণাঙ্ক),
					     width, flags, *format);
				অনুৱর্ত;
			}
			যদি ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, শূন্য*),
					     width, flags, *format);
				অনুৱর্ত;
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
					অনুৱর্ত;
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
				অনুৱর্ত;
			}
			যদি (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, পূর্ণাঙ্ক);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				অনুৱর্ত;
			}
		} অন্যথা {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	যদি (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ঘূৰাই pc;
}

পূর্ণাঙ্ক sbi_sprintf(ৱর্ণ *out, ধ্রুৱ ৱর্ণ *format, ...)
{
	va_list args;
	পূর্ণাঙ্ক retval;

	যদি (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, রিক্ত, format, args);
	va_end(args);

	ঘূৰাই retval;
}

পূর্ণাঙ্ক sbi_snprintf(ৱর্ণ *out, u32 out_sz, ধ্রুৱ ৱর্ণ *format, ...)
{
	va_list args;
	পূর্ণাঙ্ক retval;

	যদি (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ঘূৰাই retval;
}

পূর্ণাঙ্ক sbi_printf(ধ্রুৱ ৱর্ণ *format, ...)
{
	va_list args;
	পূর্ণাঙ্ক retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(রিক্ত, রিক্ত, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ঘূৰাই retval;
}

পূর্ণাঙ্ক sbi_dprintf(ধ্রুৱ ৱর্ণ *format, ...)
{
	va_list args;
	পূর্ণাঙ্ক retval = 0;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	যদি (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(রিক্ত, রিক্ত, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ঘূৰাই retval;
}

শূন্য sbi_panic(ধ্রুৱ ৱর্ণ *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(রিক্ত, রিক্ত, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ধ্রুৱ সংরচনা sbi_console_device *sbi_console_get_device(শূন্য)
{
	ঘূৰাই console_dev;
}

শূন্য sbi_console_set_device(ধ্রুৱ সংরচনা sbi_console_device *dev)
{
	ৱর্ণ ch;
	bool flush_early_fifo = false;

	যদি (!dev)
		ঘূৰাই;

	যদি (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	যদি (flush_early_fifo) {
		যেতিয়ালৈ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
