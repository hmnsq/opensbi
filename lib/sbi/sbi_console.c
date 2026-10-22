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
#否则
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
static char console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
static SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, sizeof(char));

bool sbi_isprintable(char c)
{
	もし (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		戻る true;
	}
	戻る false;
}

整数 sbi_getc(void)
{
	もし (console_dev && console_dev->console_getc)
		戻る console_dev->console_getc();
	戻る -1;
}

static unsigned long nputs(const char *str, unsigned long len)
{
	char ch;
	unsigned long i;

	もし (console_dev) {
		もし (console_dev->console_puts)
			戻る console_dev->console_puts(str, len);
		否则 もし (console_dev->console_putc) {
			ために (i = 0; i < len; i++) {
				もし (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} 否则 {
		ために (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	戻る len;
}

static void nputs_all(const char *str, unsigned long len)
{
	unsigned long p = 0;

	ながら (p < len)
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

	戻る ret;
}

void sbi_gets(char *s, 整数 maxwidth, char endchar)
{
	整数 ch;
	char *retval = s;

	ながら ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (char)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(char *str, unsigned long len)
{
	整数 ch;
	unsigned long i;

	ために (i = 0; i < len; i++) {
		ch = sbi_getc();
		もし (ch < 0)
			break;
		str[i] = ch;
	}

	戻る i;
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

static void printc(char **out, u32 *out_len, char ch, 整数 flags)
{
	もし (!out) {
		sbi_putc(ch);
		戻る;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	もし (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		もし (out_len) {
			--(*out_len);
			もし ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

static 整数 prints(char **out, u32 *out_len, const char *string, 整数 width,
		  整数 flags)
{
	整数 pc = 0;
	width -= sbi_strlen(string);
	もし (!(flags & PAD_RIGHT)) {
		ために (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	ために (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	ために (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	戻る pc;
}

static 整数 printi(char **out, u32 *out_len, long long i,
		  整数 width, 整数 flags, 整数 type)
{
	整数 pc = 0;
	char *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	もし (type == 'o')
		b = 8;
	否则 もし (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	もし (type == 'i' || type == 'd') {
		もし ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		もし (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	もし (!u) {
		*--s = '0';
	} 否则 {
		ながら (u) {
			t = u % b;
			u = u / b;
			もし (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	もし (flags & PAD_ZERO) {
		もし (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		もし (i && (flags & PAD_ALTERNATE)) {
			もし (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			もし (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} 否则 {
		もし (i && (flags & PAD_ALTERNATE)) {
			もし (b == 16)
				*--s = 'x' - 'a' + letbase;
			もし (b == 16 || b == 8)
				*--s = '0';
		}
		もし (sign)
			*--s = sign;
	}

	戻る pc + prints(out, out_len, s, width, flags);
}

static 整数 print(char **out, u32 *out_len, const char *format, va_list args)
{
	bool flags_done;
	整数 width, flags, pc = 0;
	char type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	もし (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	もし (out) {
		もし(!out_len || *out_len)
			**out = '\0';
	}

	ために (; *format != 0; ++format) {
		width = flags = 0;
		もし (use_tbuf)
			flags |= USE_TBUF;
		もし (*format == '%') {
			++format;
			もし (*format == '\0')
				break;
			もし (*format == '%')
				goto literal;
			/* Get flags */
			flags_done = false;
			ながら (!flags_done) {
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
				もし (!flags_done)
					++format;
			}
			もし (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			ために (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			もし (*format == 's') {
				char *s = va_arg(args, char *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				continue;
			}
			もし ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, 整数),
					     width, flags, *format);
				continue;
			}
			もし ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned 整数),
					     width, flags, *format);
				continue;
			}
			もし ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, void*),
					     width, flags, *format);
				continue;
			}
			もし (*format == 'l') {
				type = 'i';
				もし (format[1] == 'l') {
					++format;
					もし ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					continue;
				}
				もし ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				もし ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				否则
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				continue;
			}
			もし (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, 整数);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				continue;
			}
		} 否则 {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	もし (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	戻る pc;
}

整数 sbi_sprintf(char *out, const char *format, ...)
{
	va_list args;
	整数 retval;

	もし (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, NULL, format, args);
	va_end(args);

	戻る retval;
}

整数 sbi_snprintf(char *out, u32 out_sz, const char *format, ...)
{
	va_list args;
	整数 retval;

	もし (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	戻る retval;
}

整数 sbi_printf(const char *format, ...)
{
	va_list args;
	整数 retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(NULL, NULL, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	戻る retval;
}

整数 sbi_dprintf(const char *format, ...)
{
	va_list args;
	整数 retval = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	もし (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(NULL, NULL, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	戻る retval;
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
	戻る console_dev;
}

void sbi_console_set_device(const struct sbi_console_device *dev)
{
	char ch;
	bool flush_early_fifo = false;

	もし (!dev)
		戻る;

	もし (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	もし (flush_early_fifo) {
		ながら (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
