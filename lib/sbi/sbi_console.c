/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_fifo.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

static const sa_nracanaa sbi_console_device *console_dev = NULL;
static var_na console_tbuf[CONSOLE_TBUF_MAX];
static u32 console_tbuf_len;
static spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#ᰀᰧᰀ
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
static var_na console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
static SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, parimaa_na(var_na));

bool sbi_isprintable(var_na c)
{
	ᰀᰧ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ᰀᰧᰌ true;
	}
	ᰀᰧᰌ false;
}

puur_naa_nka sbi_getc(void)
{
	ᰀᰧ (console_dev && console_dev->console_getc)
		ᰀᰧᰌ console_dev->console_getc();
	ᰀᰧᰌ -1;
}

static unsigned long nputs(const var_na *str, unsigned long len)
{
	var_na ch;
	unsigned long i;

	ᰀᰧ (console_dev) {
		ᰀᰧ (console_dev->console_puts)
			ᰀᰧᰌ console_dev->console_puts(str, len);
		ᰀᰧᰀ ᰀᰧ (console_dev->console_putc) {
			for (i = 0; i < len; i++) {
				ᰀᰧ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ᰀᰧᰀ {
		for (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ᰀᰧᰌ len;
}

static void nputs_all(const var_na *str, unsigned long len)
{
	unsigned long p = 0;

	ᰀᰬ (p < len)
		p += nputs(&str[p], len - p);
}

void sbi_putc(var_na ch)
{
	nputs_all(&ch, 1);
}

void sbi_puts(const var_na *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(const var_na *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	ᰀᰧᰌ ret;
}

void sbi_gets(var_na *s, puur_naa_nka maxwidth, var_na endchar)
{
	puur_naa_nka ch;
	var_na *retval = s;

	ᰀᰬ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (var_na)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(var_na *str, unsigned long len)
{
	puur_naa_nka ch;
	unsigned long i;

	for (i = 0; i < len; i++) {
		ch = sbi_getc();
		ᰀᰧ (ch < 0)
			bha_nga;
		str[i] = ch;
	}

	ᰀᰧᰌ i;
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
typedef __builtin_va_list va_list;

static void printc(var_na **out, u32 *out_len, var_na ch, puur_naa_nka flags)
{
	ᰀᰧ (!out) {
		sbi_putc(ch);
		ᰀᰧᰌ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ᰀᰧ (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		ᰀᰧ (out_len) {
			--(*out_len);
			ᰀᰧ ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

static puur_naa_nka prints(var_na **out, u32 *out_len, const var_na *string, puur_naa_nka width,
		  puur_naa_nka flags)
{
	puur_naa_nka pc = 0;
	width -= sbi_strlen(string);
	ᰀᰧ (!(flags & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	for (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	for (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	ᰀᰧᰌ pc;
}

static puur_naa_nka printi(var_na **out, u32 *out_len, long long i,
		  puur_naa_nka width, puur_naa_nka flags, puur_naa_nka type)
{
	puur_naa_nka pc = 0;
	var_na *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	ᰀᰧ (type == 'o')
		b = 8;
	ᰀᰧᰀ ᰀᰧ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ᰀᰧ (type == 'i' || type == 'd') {
		ᰀᰧ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ᰀᰧ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ᰀᰧ (!u) {
		*--s = '0';
	} ᰀᰧᰀ {
		ᰀᰬ (u) {
			t = u % b;
			u = u / b;
			ᰀᰧ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ᰀᰧ (flags & PAD_ZERO) {
		ᰀᰧ (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		ᰀᰧ (i && (flags & PAD_ALTERNATE)) {
			ᰀᰧ (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			ᰀᰧ (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} ᰀᰧᰀ {
		ᰀᰧ (i && (flags & PAD_ALTERNATE)) {
			ᰀᰧ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ᰀᰧ (b == 16 || b == 8)
				*--s = '0';
		}
		ᰀᰧ (sign)
			*--s = sign;
	}

	ᰀᰧᰌ pc + prints(out, out_len, s, width, flags);
}

static puur_naa_nka print(var_na **out, u32 *out_len, const var_na *format, va_list args)
{
	bool flags_done;
	puur_naa_nka width, flags, pc = 0;
	var_na type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ᰀᰧ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ᰀᰧ (out) {
		ᰀᰧ(!out_len || *out_len)
			**out = '\0';
	}

	for (; *format != 0; ++format) {
		width = flags = 0;
		ᰀᰧ (use_tbuf)
			flags |= USE_TBUF;
		ᰀᰧ (*format == '%') {
			++format;
			ᰀᰧ (*format == '\0')
				bha_nga;
			ᰀᰧ (*format == '%')
				goto literal;
			/* Get flags */
			flags_done = false;
			ᰀᰬ (!flags_done) {
				switch (*format) {
				pak_sa '-':
					flags |= PAD_RIGHT;
					bha_nga;
				pak_sa '+':
					flags |= PAD_SIGN;
					bha_nga;
				pak_sa '#':
					flags |= PAD_ALTERNATE;
					bha_nga;
				pak_sa '0':
					flags |= PAD_ZERO;
					bha_nga;
				pak_sa ' ':
				pak_sa '\'':
					/* Ignored flags, do nothing */
					bha_nga;
				default:
					flags_done = true;
					bha_nga;
				}
				ᰀᰧ (!flags_done)
					++format;
			}
			ᰀᰧ (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			ᰀᰧ (*format == 's') {
				var_na *s = va_arg(args, var_na *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				continue;
			}
			ᰀᰧ ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, puur_naa_nka),
					     width, flags, *format);
				continue;
			}
			ᰀᰧ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned puur_naa_nka),
					     width, flags, *format);
				continue;
			}
			ᰀᰧ ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, void*),
					     width, flags, *format);
				continue;
			}
			ᰀᰧ (*format == 'l') {
				type = 'i';
				ᰀᰧ (format[1] == 'l') {
					++format;
					ᰀᰧ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					continue;
				}
				ᰀᰧ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ᰀᰧ ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				ᰀᰧᰀ
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				continue;
			}
			ᰀᰧ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, puur_naa_nka);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				continue;
			}
		} ᰀᰧᰀ {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	ᰀᰧ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ᰀᰧᰌ pc;
}

puur_naa_nka sbi_sprintf(var_na *out, const var_na *format, ...)
{
	va_list args;
	puur_naa_nka retval;

	ᰀᰧ (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, NULL, format, args);
	va_end(args);

	ᰀᰧᰌ retval;
}

puur_naa_nka sbi_snprintf(var_na *out, u32 out_sz, const var_na *format, ...)
{
	va_list args;
	puur_naa_nka retval;

	ᰀᰧ (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ᰀᰧᰌ retval;
}

puur_naa_nka sbi_printf(const var_na *format, ...)
{
	va_list args;
	puur_naa_nka retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ᰀᰧᰌ retval;
}

puur_naa_nka sbi_dprintf(const var_na *format, ...)
{
	va_list args;
	puur_naa_nka retval = 0;
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	ᰀᰧ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(NULL, NULL, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ᰀᰧᰌ retval;
}

void sbi_panic(const var_na *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

const sa_nracanaa sbi_console_device *sbi_console_get_device(void)
{
	ᰀᰧᰌ console_dev;
}

void sbi_console_set_device(const sa_nracanaa sbi_console_device *dev)
{
	var_na ch;
	bool flush_early_fifo = false;

	ᰀᰧ (!dev)
		ᰀᰧᰌ;

	ᰀᰧ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ᰀᰧ (flush_early_fifo) {
		ᰀᰬ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
