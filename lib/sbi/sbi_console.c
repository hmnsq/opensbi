/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_fifo.h>
#அந்தர்பாவ <sbi/sbi_hart.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

நிலையான மாறிலி கட்டமைப்பு sbi_console_device *console_dev = வெற்று;
நிலையான எழுத்து console_tbuf[CONSOLE_TBUF_MAX];
நிலையான u32 console_tbuf_len;
நிலையான spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#இல்லையெனில்
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
நிலையான எழுத்து console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
நிலையான SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, அளவு(எழுத்து));

bool sbi_isprintable(எழுத்து c)
{
	எனில் (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		திரும்பு true;
	}
	திரும்பு false;
}

முழு sbi_getc(வெறுமை)
{
	எனில் (console_dev && console_dev->console_getc)
		திரும்பு console_dev->console_getc();
	திரும்பு -1;
}

நிலையான unsigned long nputs(மாறிலி எழுத்து *str, unsigned long len)
{
	எழுத்து ch;
	unsigned long i;

	எனில் (console_dev) {
		எனில் (console_dev->console_puts)
			திரும்பு console_dev->console_puts(str, len);
		இல்லையெனில் எனில் (console_dev->console_putc) {
			க்கு (i = 0; i < len; i++) {
				எனில் (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} இல்லையெனில் {
		க்கு (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	திரும்பு len;
}

நிலையான வெறுமை nputs_all(மாறிலி எழுத்து *str, unsigned long len)
{
	unsigned long p = 0;

	போது (p < len)
		p += nputs(&str[p], len - p);
}

வெறுமை sbi_putc(எழுத்து ch)
{
	nputs_all(&ch, 1);
}

வெறுமை sbi_puts(மாறிலி எழுத்து *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(மாறிலி எழுத்து *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	திரும்பு ret;
}

வெறுமை sbi_gets(எழுத்து *s, முழு maxwidth, எழுத்து endchar)
{
	முழு ch;
	எழுத்து *retval = s;

	போது ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (எழுத்து)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(எழுத்து *str, unsigned long len)
{
	முழு ch;
	unsigned long i;

	க்கு (i = 0; i < len; i++) {
		ch = sbi_getc();
		எனில் (ch < 0)
			நிறுத்து;
		str[i] = ch;
	}

	திரும்பு i;
}

#define PAD_வலது 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
வகைவரை __builtin_va_list va_list;

நிலையான வெறுமை printc(எழுத்து **out, u32 *out_len, எழுத்து ch, முழு flags)
{
	எனில் (!out) {
		sbi_putc(ch);
		திரும்பு;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	எனில் (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		எனில் (out_len) {
			--(*out_len);
			எனில் ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

நிலையான முழு prints(எழுத்து **out, u32 *out_len, மாறிலி எழுத்து *string, முழு width,
		  முழு flags)
{
	முழு pc = 0;
	width -= sbi_strlen(string);
	எனில் (!(flags & PAD_வலது)) {
		க்கு (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	க்கு (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	க்கு (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	திரும்பு pc;
}

நிலையான முழு printi(எழுத்து **out, u32 *out_len, long long i,
		  முழு width, முழு flags, முழு type)
{
	முழு pc = 0;
	எழுத்து *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	எனில் (type == 'o')
		b = 8;
	இல்லையெனில் எனில் (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	எனில் (type == 'i' || type == 'd') {
		எனில் ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		எனில் (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	எனில் (!u) {
		*--s = '0';
	} இல்லையெனில் {
		போது (u) {
			t = u % b;
			u = u / b;
			எனில் (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	எனில் (flags & PAD_ZERO) {
		எனில் (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		எனில் (i && (flags & PAD_ALTERNATE)) {
			எனில் (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			எனில் (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} இல்லையெனில் {
		எனில் (i && (flags & PAD_ALTERNATE)) {
			எனில் (b == 16)
				*--s = 'x' - 'a' + letbase;
			எனில் (b == 16 || b == 8)
				*--s = '0';
		}
		எனில் (sign)
			*--s = sign;
	}

	திரும்பு pc + prints(out, out_len, s, width, flags);
}

நிலையான முழு print(எழுத்து **out, u32 *out_len, மாறிலி எழுத்து *format, va_list args)
{
	bool flags_done;
	முழு width, flags, pc = 0;
	எழுத்து type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	எனில் (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	எனில் (out) {
		எனில்(!out_len || *out_len)
			**out = '\0';
	}

	க்கு (; *format != 0; ++format) {
		width = flags = 0;
		எனில் (use_tbuf)
			flags |= USE_TBUF;
		எனில் (*format == '%') {
			++format;
			எனில் (*format == '\0')
				நிறுத்து;
			எனில் (*format == '%')
				செல் literal;
			/* Get flags */
			flags_done = false;
			போது (!flags_done) {
				தேர்வு (*format) {
				நிலை '-':
					flags |= PAD_வலது;
					நிறுத்து;
				நிலை '+':
					flags |= PAD_SIGN;
					நிறுத்து;
				நிலை '#':
					flags |= PAD_ALTERNATE;
					நிறுத்து;
				நிலை '0':
					flags |= PAD_ZERO;
					நிறுத்து;
				நிலை ' ':
				நிலை '\'':
					/* Ignored flags, do nothing */
					நிறுத்து;
				இயல்பு:
					flags_done = true;
					நிறுத்து;
				}
				எனில் (!flags_done)
					++format;
			}
			எனில் (flags & PAD_வலது)
				flags &= ~PAD_ZERO;
			/* Get width */
			க்கு (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			எனில் (*format == 's') {
				எழுத்து *s = va_arg(args, எழுத்து *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				தொடர்;
			}
			எனில் ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, முழு),
					     width, flags, *format);
				தொடர்;
			}
			எனில் ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned முழு),
					     width, flags, *format);
				தொடர்;
			}
			எனில் ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, வெறுமை*),
					     width, flags, *format);
				தொடர்;
			}
			எனில் (*format == 'l') {
				type = 'i';
				எனில் (format[1] == 'l') {
					++format;
					எனில் ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					தொடர்;
				}
				எனில் ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				எனில் ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				இல்லையெனில்
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				தொடர்;
			}
			எனில் (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, முழு);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				தொடர்;
			}
		} இல்லையெனில் {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	எனில் (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	திரும்பு pc;
}

முழு sbi_sprintf(எழுத்து *out, மாறிலி எழுத்து *format, ...)
{
	va_list args;
	முழு retval;

	எனில் (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, வெற்று, format, args);
	va_end(args);

	திரும்பு retval;
}

முழு sbi_snprintf(எழுத்து *out, u32 out_sz, மாறிலி எழுத்து *format, ...)
{
	va_list args;
	முழு retval;

	எனில் (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	திரும்பு retval;
}

முழு sbi_printf(மாறிலி எழுத்து *format, ...)
{
	va_list args;
	முழு retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(வெற்று, வெற்று, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	திரும்பு retval;
}

முழு sbi_dprintf(மாறிலி எழுத்து *format, ...)
{
	va_list args;
	முழு retval = 0;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	எனில் (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(வெற்று, வெற்று, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	திரும்பு retval;
}

வெறுமை sbi_panic(மாறிலி எழுத்து *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(வெற்று, வெற்று, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

மாறிலி கட்டமைப்பு sbi_console_device *sbi_console_get_device(வெறுமை)
{
	திரும்பு console_dev;
}

வெறுமை sbi_console_set_device(மாறிலி கட்டமைப்பு sbi_console_device *dev)
{
	எழுத்து ch;
	bool flush_early_fifo = false;

	எனில் (!dev)
		திரும்பு;

	எனில் (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	எனில் (flush_early_fifo) {
		போது (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
