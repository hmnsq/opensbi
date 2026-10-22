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

static const struct sbi_console_device *console_dev = NULL;
static char console_tbuf[CONSOLE_TBUF_MAX];
static u32 console_tbuf_len;
static spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#아니면
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
static char console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
static SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, sizeof(char));

bool sbi_isprintable(char c)
{
	만약 (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		반환 true;
	}
	반환 false;
}

정수 sbi_getc(void)
{
	만약 (console_dev && console_dev->console_getc)
		반환 console_dev->console_getc();
	반환 -1;
}

static unsigned long nputs(const char *str, unsigned long len)
{
	char ch;
	unsigned long i;

	만약 (console_dev) {
		만약 (console_dev->console_puts)
			반환 console_dev->console_puts(str, len);
		아니면 만약 (console_dev->console_putc) {
			위해 (i = 0; i < len; i++) {
				만약 (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} 아니면 {
		위해 (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	반환 len;
}

static void nputs_all(const char *str, unsigned long len)
{
	unsigned long p = 0;

	동안 (p < len)
		p += nputs(&str[p], len - p);
}

void sbi_putc(char ch)
{
	nputs_all(&ch, 1);
}

void sbi_puts(const char *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(const char *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	반환 ret;
}

void sbi_gets(char *s, 정수 maxwidth, char endchar)
{
	정수 ch;
	char *retval = s;

	동안 ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (char)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(char *str, unsigned long len)
{
	정수 ch;
	unsigned long i;

	위해 (i = 0; i < len; i++) {
		ch = sbi_getc();
		만약 (ch < 0)
			break;
		str[i] = ch;
	}

	반환 i;
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

static void printc(char **out, u32 *out_len, char ch, 정수 flags)
{
	만약 (!out) {
		sbi_putc(ch);
		반환;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	만약 (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		만약 (out_len) {
			--(*out_len);
			만약 ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

static 정수 prints(char **out, u32 *out_len, const char *string, 정수 width,
		  정수 flags)
{
	정수 pc = 0;
	width -= sbi_strlen(string);
	만약 (!(flags & PAD_RIGHT)) {
		위해 (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	위해 (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	위해 (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	반환 pc;
}

static 정수 printi(char **out, u32 *out_len, long long i,
		  정수 width, 정수 flags, 정수 type)
{
	정수 pc = 0;
	char *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	만약 (type == 'o')
		b = 8;
	아니면 만약 (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	만약 (type == 'i' || type == 'd') {
		만약 ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		만약 (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	만약 (!u) {
		*--s = '0';
	} 아니면 {
		동안 (u) {
			t = u % b;
			u = u / b;
			만약 (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	만약 (flags & PAD_ZERO) {
		만약 (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		만약 (i && (flags & PAD_ALTERNATE)) {
			만약 (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			만약 (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} 아니면 {
		만약 (i && (flags & PAD_ALTERNATE)) {
			만약 (b == 16)
				*--s = 'x' - 'a' + letbase;
			만약 (b == 16 || b == 8)
				*--s = '0';
		}
		만약 (sign)
			*--s = sign;
	}

	반환 pc + prints(out, out_len, s, width, flags);
}

static 정수 print(char **out, u32 *out_len, const char *format, va_list args)
{
	bool flags_done;
	정수 width, flags, pc = 0;
	char type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	만약 (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	만약 (out) {
		만약(!out_len || *out_len)
			**out = '\0';
	}

	위해 (; *format != 0; ++format) {
		width = flags = 0;
		만약 (use_tbuf)
			flags |= USE_TBUF;
		만약 (*format == '%') {
			++format;
			만약 (*format == '\0')
				break;
			만약 (*format == '%')
				goto literal;
			/* Get flags */
			flags_done = false;
			동안 (!flags_done) {
				switch (*format) {
				case '-':
					flags |= PAD_RIGHT;
					break;
				case '+':
					flags |= PAD_SIGN;
					break;
				case '#':
					flags |= PAD_ALTERNATE;
					break;
				case '0':
					flags |= PAD_ZERO;
					break;
				case ' ':
				case '\'':
					/* Ignored flags, do nothing */
					break;
				default:
					flags_done = true;
					break;
				}
				만약 (!flags_done)
					++format;
			}
			만약 (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			위해 (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			만약 (*format == 's') {
				char *s = va_arg(args, char *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				continue;
			}
			만약 ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, 정수),
					     width, flags, *format);
				continue;
			}
			만약 ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned 정수),
					     width, flags, *format);
				continue;
			}
			만약 ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, void*),
					     width, flags, *format);
				continue;
			}
			만약 (*format == 'l') {
				type = 'i';
				만약 (format[1] == 'l') {
					++format;
					만약 ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					continue;
				}
				만약 ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				만약 ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				아니면
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				continue;
			}
			만약 (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, 정수);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				continue;
			}
		} 아니면 {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	만약 (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	반환 pc;
}

정수 sbi_sprintf(char *out, const char *format, ...)
{
	va_list args;
	정수 retval;

	만약 (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, NULL, format, args);
	va_end(args);

	반환 retval;
}

정수 sbi_snprintf(char *out, u32 out_sz, const char *format, ...)
{
	va_list args;
	정수 retval;

	만약 (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	반환 retval;
}

정수 sbi_printf(const char *format, ...)
{
	va_list args;
	정수 retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	반환 retval;
}

정수 sbi_dprintf(const char *format, ...)
{
	va_list args;
	정수 retval = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	만약 (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(NULL, NULL, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	반환 retval;
}

void sbi_panic(const char *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

const struct sbi_console_device *sbi_console_get_device(void)
{
	반환 console_dev;
}

void sbi_console_set_device(const struct sbi_console_device *dev)
{
	char ch;
	bool flush_early_fifo = false;

	만약 (!dev)
		반환;

	만약 (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	만약 (flush_early_fifo) {
		동안 (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
