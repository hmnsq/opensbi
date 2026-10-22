/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_fifo.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_console_device *console_dev = ଖାଲି;
ସ୍ଥିର ଅକ୍ଷର console_tbuf[CONSOLE_TBUF_MAX];
ସ୍ଥିର u32 console_tbuf_len;
ସ୍ଥିର spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#ଅନ୍ୟଥା
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
ସ୍ଥିର ଅକ୍ଷର console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
ସ୍ଥିର SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, ଆକାର(ଅକ୍ଷର));

bool sbi_isprintable(ଅକ୍ଷର c)
{
	ଯଦି (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ଫେରନ୍ତୁ true;
	}
	ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_getc(ଶୂନ୍ୟ)
{
	ଯଦି (console_dev && console_dev->console_getc)
		ଫେରନ୍ତୁ console_dev->console_getc();
	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର unsigned long nputs(ଅଚଳ ଅକ୍ଷର *str, unsigned long len)
{
	ଅକ୍ଷର ch;
	unsigned long i;

	ଯଦି (console_dev) {
		ଯଦି (console_dev->console_puts)
			ଫେରନ୍ତୁ console_dev->console_puts(str, len);
		ଅନ୍ୟଥା ଯଦି (console_dev->console_putc) {
			ପାଇଁ (i = 0; i < len; i++) {
				ଯଦି (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ଅନ୍ୟଥା {
		ପାଇଁ (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ଫେରନ୍ତୁ len;
}

ସ୍ଥିର ଶୂନ୍ୟ nputs_all(ଅଚଳ ଅକ୍ଷର *str, unsigned long len)
{
	unsigned long p = 0;

	ଯେତେବେଳେ (p < len)
		p += nputs(&str[p], len - p);
}

ଶୂନ୍ୟ sbi_putc(ଅକ୍ଷର ch)
{
	nputs_all(&ch, 1);
}

ଶୂନ୍ୟ sbi_puts(ଅଚଳ ଅକ୍ଷର *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ଅଚଳ ଅକ୍ଷର *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ sbi_gets(ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxwidth, ଅକ୍ଷର endchar)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch;
	ଅକ୍ଷର *retval = s;

	ଯେତେବେଳେ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (ଅକ୍ଷର)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(ଅକ୍ଷର *str, unsigned long len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch;
	unsigned long i;

	ପାଇଁ (i = 0; i < len; i++) {
		ch = sbi_getc();
		ଯଦି (ch < 0)
			ଭାଙ୍ଗ;
		str[i] = ch;
	}

	ଫେରନ୍ତୁ i;
}

#define PAD_ଡାହାଣ 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
ପ୍ରକାରବ୍ୟାଖ୍ୟା __builtin_va_list va_list;

ସ୍ଥିର ଶୂନ୍ୟ printc(ଅକ୍ଷର **out, u32 *out_len, ଅକ୍ଷର ch, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
	ଯଦି (!out) {
		sbi_putc(ch);
		ଫେରନ୍ତୁ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ଯଦି (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		ଯଦି (out_len) {
			--(*out_len);
			ଯଦି ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prints(ଅକ୍ଷର **out, u32 *out_len, ଅଚଳ ଅକ୍ଷର *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width,
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pc = 0;
	width -= sbi_strlen(string);
	ଯଦି (!(flags & PAD_ଡାହାଣ)) {
		ପାଇଁ (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	ପାଇଁ (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	ପାଇଁ (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	ଫେରନ୍ତୁ pc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା printi(ଅକ୍ଷର **out, u32 *out_len, long long i,
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pc = 0;
	ଅକ୍ଷର *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	ଯଦି (type == 'o')
		b = 8;
	ଅନ୍ୟଥା ଯଦି (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ଯଦି (type == 'i' || type == 'd') {
		ଯଦି ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ଯଦି (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ଯଦି (!u) {
		*--s = '0';
	} ଅନ୍ୟଥା {
		ଯେତେବେଳେ (u) {
			t = u % b;
			u = u / b;
			ଯଦି (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ଯଦି (flags & PAD_ZERO) {
		ଯଦି (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		ଯଦି (i && (flags & PAD_ALTERNATE)) {
			ଯଦି (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			ଯଦି (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} ଅନ୍ୟଥା {
		ଯଦି (i && (flags & PAD_ALTERNATE)) {
			ଯଦି (b == 16)
				*--s = 'x' - 'a' + letbase;
			ଯଦି (b == 16 || b == 8)
				*--s = '0';
		}
		ଯଦି (sign)
			*--s = sign;
	}

	ଫେରନ୍ତୁ pc + prints(out, out_len, s, width, flags);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା print(ଅକ୍ଷର **out, u32 *out_len, ଅଚଳ ଅକ୍ଷର *format, va_list args)
{
	bool flags_done;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, flags, pc = 0;
	ଅକ୍ଷର type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ଯଦି (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ଯଦି (out) {
		ଯଦି(!out_len || *out_len)
			**out = '\0';
	}

	ପାଇଁ (; *format != 0; ++format) {
		width = flags = 0;
		ଯଦି (use_tbuf)
			flags |= USE_TBUF;
		ଯଦି (*format == '%') {
			++format;
			ଯଦି (*format == '\0')
				ଭାଙ୍ଗ;
			ଯଦି (*format == '%')
				ଯାଅ literal;
			/* Get flags */
			flags_done = false;
			ଯେତେବେଳେ (!flags_done) {
				ବାଛ (*format) {
				ସ୍ଥିତି '-':
					flags |= PAD_ଡାହାଣ;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି '+':
					flags |= PAD_SIGN;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି '#':
					flags |= PAD_ALTERNATE;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି '0':
					flags |= PAD_ZERO;
					ଭାଙ୍ଗ;
				ସ୍ଥିତି ' ':
				ସ୍ଥିତି '\'':
					/* Ignored flags, do nothing */
					ଭାଙ୍ଗ;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					flags_done = true;
					ଭାଙ୍ଗ;
				}
				ଯଦି (!flags_done)
					++format;
			}
			ଯଦି (flags & PAD_ଡାହାଣ)
				flags &= ~PAD_ZERO;
			/* Get width */
			ପାଇଁ (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			ଯଦି (*format == 's') {
				ଅକ୍ଷର *s = va_arg(args, ଅକ୍ଷର *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				ଜାରି;
			}
			ଯଦି ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
					     width, flags, *format);
				ଜାରି;
			}
			ଯଦି ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
					     width, flags, *format);
				ଜାରି;
			}
			ଯଦି ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, ଶୂନ୍ୟ*),
					     width, flags, *format);
				ଜାରି;
			}
			ଯଦି (*format == 'l') {
				type = 'i';
				ଯଦି (format[1] == 'l') {
					++format;
					ଯଦି ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					ଜାରି;
				}
				ଯଦି ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ଯଦି ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				ଅନ୍ୟଥା
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				ଜାରି;
			}
			ଯଦି (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				ଜାରି;
			}
		} ଅନ୍ୟଥା {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	ଯଦି (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ଫେରନ୍ତୁ pc;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_sprintf(ଅକ୍ଷର *out, ଅଚଳ ଅକ୍ଷର *format, ...)
{
	va_list args;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval;

	ଯଦି (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, ଖାଲି, format, args);
	va_end(args);

	ଫେରନ୍ତୁ retval;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_snprintf(ଅକ୍ଷର *out, u32 out_sz, ଅଚଳ ଅକ୍ଷର *format, ...)
{
	va_list args;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval;

	ଯଦି (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ଫେରନ୍ତୁ retval;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_printf(ଅଚଳ ଅକ୍ଷର *format, ...)
{
	va_list args;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(ଖାଲି, ଖାଲି, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ଫେରନ୍ତୁ retval;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_dprintf(ଅଚଳ ଅକ୍ଷର *format, ...)
{
	va_list args;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = 0;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	ଯଦି (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(ଖାଲି, ଖାଲି, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ଫେରନ୍ତୁ retval;
}

ଶୂନ୍ୟ sbi_panic(ଅଚଳ ଅକ୍ଷର *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(ଖାଲି, ଖାଲି, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ଅଚଳ ସଂରଚନା sbi_console_device *sbi_console_get_device(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ console_dev;
}

ଶୂନ୍ୟ sbi_console_set_device(ଅଚଳ ସଂରଚନା sbi_console_device *dev)
{
	ଅକ୍ଷର ch;
	bool flush_early_fifo = false;

	ଯଦି (!dev)
		ଫେରନ୍ତୁ;

	ଯଦି (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ଯଦି (flush_early_fifo) {
		ଯେତେବେଳେ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
