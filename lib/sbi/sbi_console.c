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
#иначе
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
static char console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
static SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, sizeof(char));

bool sbi_isprintable(char c)
{
	если (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		вернуть true;
	}
	вернуть false;
}

целое sbi_getc(void)
{
	если (console_dev && console_dev->console_getc)
		вернуть console_dev->console_getc();
	вернуть -1;
}

static unsigned long nputs(const char *str, unsigned long len)
{
	char ch;
	unsigned long i;

	если (console_dev) {
		если (console_dev->console_puts)
			вернуть console_dev->console_puts(str, len);
		иначе если (console_dev->console_putc) {
			для (i = 0; i < len; i++) {
				если (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} иначе {
		для (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	вернуть len;
}

static void nputs_all(const char *str, unsigned long len)
{
	unsigned long p = 0;

	пока (p < len)
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

	вернуть ret;
}

void sbi_gets(char *s, целое maxwidth, char endchar)
{
	целое ch;
	char *retval = s;

	пока ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (char)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(char *str, unsigned long len)
{
	целое ch;
	unsigned long i;

	для (i = 0; i < len; i++) {
		ch = sbi_getc();
		если (ch < 0)
			break;
		str[i] = ch;
	}

	вернуть i;
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

static void printc(char **out, u32 *out_len, char ch, целое flags)
{
	если (!out) {
		sbi_putc(ch);
		вернуть;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	если (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		если (out_len) {
			--(*out_len);
			если ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

static целое prints(char **out, u32 *out_len, const char *string, целое width,
		  целое flags)
{
	целое pc = 0;
	width -= sbi_strlen(string);
	если (!(flags & PAD_RIGHT)) {
		для (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	для (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	для (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	вернуть pc;
}

static целое printi(char **out, u32 *out_len, long long i,
		  целое width, целое flags, целое type)
{
	целое pc = 0;
	char *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	если (type == 'o')
		b = 8;
	иначе если (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	если (type == 'i' || type == 'd') {
		если ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		если (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	если (!u) {
		*--s = '0';
	} иначе {
		пока (u) {
			t = u % b;
			u = u / b;
			если (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	если (flags & PAD_ZERO) {
		если (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		если (i && (flags & PAD_ALTERNATE)) {
			если (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			если (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} иначе {
		если (i && (flags & PAD_ALTERNATE)) {
			если (b == 16)
				*--s = 'x' - 'a' + letbase;
			если (b == 16 || b == 8)
				*--s = '0';
		}
		если (sign)
			*--s = sign;
	}

	вернуть pc + prints(out, out_len, s, width, flags);
}

static целое print(char **out, u32 *out_len, const char *format, va_list args)
{
	bool flags_done;
	целое width, flags, pc = 0;
	char type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	если (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	если (out) {
		если(!out_len || *out_len)
			**out = '\0';
	}

	для (; *format != 0; ++format) {
		width = flags = 0;
		если (use_tbuf)
			flags |= USE_TBUF;
		если (*format == '%') {
			++format;
			если (*format == '\0')
				break;
			если (*format == '%')
				goto literal;
			/* Get flags */
			flags_done = false;
			пока (!flags_done) {
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
				если (!flags_done)
					++format;
			}
			если (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			для (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			если (*format == 's') {
				char *s = va_arg(args, char *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				continue;
			}
			если ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, целое),
					     width, flags, *format);
				continue;
			}
			если ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned целое),
					     width, flags, *format);
				continue;
			}
			если ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, void*),
					     width, flags, *format);
				continue;
			}
			если (*format == 'l') {
				type = 'i';
				если (format[1] == 'l') {
					++format;
					если ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					continue;
				}
				если ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				если ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				иначе
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				continue;
			}
			если (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, целое);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				continue;
			}
		} иначе {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	если (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	вернуть pc;
}

целое sbi_sprintf(char *out, const char *format, ...)
{
	va_list args;
	целое retval;

	если (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, NULL, format, args);
	va_end(args);

	вернуть retval;
}

целое sbi_snprintf(char *out, u32 out_sz, const char *format, ...)
{
	va_list args;
	целое retval;

	если (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	вернуть retval;
}

целое sbi_printf(const char *format, ...)
{
	va_list args;
	целое retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	вернуть retval;
}

целое sbi_dprintf(const char *format, ...)
{
	va_list args;
	целое retval = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	если (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(NULL, NULL, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	вернуть retval;
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
	вернуть console_dev;
}

void sbi_console_set_device(const struct sbi_console_device *dev)
{
	char ch;
	bool flush_early_fifo = false;

	если (!dev)
		вернуть;

	если (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	если (flush_early_fifo) {
		пока (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
