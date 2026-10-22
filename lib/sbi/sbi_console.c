/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_fifo.h>
#અન્તર્ભાવ <sbi/sbi_hart.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

સ્થિર અચલ સંરચના sbi_console_device *console_dev = ખાલી;
સ્થિર અક્ષર console_tbuf[CONSOLE_TBUF_MAX];
સ્થિર u32 console_tbuf_len;
સ્થિર spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#નહીંતર
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
સ્થિર અક્ષર console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
સ્થિર SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, કદ(અક્ષર));

bool sbi_isprintable(અક્ષર c)
{
	જો (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		પાછા true;
	}
	પાછા false;
}

પૂર્ણાંક sbi_getc(શૂન્ય)
{
	જો (console_dev && console_dev->console_getc)
		પાછા console_dev->console_getc();
	પાછા -1;
}

સ્થિર unsigned long nputs(અચલ અક્ષર *str, unsigned long len)
{
	અક્ષર ch;
	unsigned long i;

	જો (console_dev) {
		જો (console_dev->console_puts)
			પાછા console_dev->console_puts(str, len);
		નહીંતર જો (console_dev->console_putc) {
			માટે (i = 0; i < len; i++) {
				જો (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} નહીંતર {
		માટે (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	પાછા len;
}

સ્થિર શૂન્ય nputs_all(અચલ અક્ષર *str, unsigned long len)
{
	unsigned long p = 0;

	જ્યારે (p < len)
		p += nputs(&str[p], len - p);
}

શૂન્ય sbi_putc(અક્ષર ch)
{
	nputs_all(&ch, 1);
}

શૂન્ય sbi_puts(અચલ અક્ષર *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(અચલ અક્ષર *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	પાછા ret;
}

શૂન્ય sbi_gets(અક્ષર *s, પૂર્ણાંક maxwidth, અક્ષર endchar)
{
	પૂર્ણાંક ch;
	અક્ષર *retval = s;

	જ્યારે ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (અક્ષર)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(અક્ષર *str, unsigned long len)
{
	પૂર્ણાંક ch;
	unsigned long i;

	માટે (i = 0; i < len; i++) {
		ch = sbi_getc();
		જો (ch < 0)
			તોડો;
		str[i] = ch;
	}

	પાછા i;
}

#define PAD_જમણું 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
પ્રકારવ્યાખ્યા __builtin_va_list va_list;

સ્થિર શૂન્ય printc(અક્ષર **out, u32 *out_len, અક્ષર ch, પૂર્ણાંક flags)
{
	જો (!out) {
		sbi_putc(ch);
		પાછા;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	જો (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		જો (out_len) {
			--(*out_len);
			જો ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

સ્થિર પૂર્ણાંક prints(અક્ષર **out, u32 *out_len, અચલ અક્ષર *string, પૂર્ણાંક width,
		  પૂર્ણાંક flags)
{
	પૂર્ણાંક pc = 0;
	width -= sbi_strlen(string);
	જો (!(flags & PAD_જમણું)) {
		માટે (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	માટે (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	માટે (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	પાછા pc;
}

સ્થિર પૂર્ણાંક printi(અક્ષર **out, u32 *out_len, long long i,
		  પૂર્ણાંક width, પૂર્ણાંક flags, પૂર્ણાંક type)
{
	પૂર્ણાંક pc = 0;
	અક્ષર *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	જો (type == 'o')
		b = 8;
	નહીંતર જો (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	જો (type == 'i' || type == 'd') {
		જો ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		જો (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	જો (!u) {
		*--s = '0';
	} નહીંતર {
		જ્યારે (u) {
			t = u % b;
			u = u / b;
			જો (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	જો (flags & PAD_ZERO) {
		જો (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		જો (i && (flags & PAD_ALTERNATE)) {
			જો (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			જો (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} નહીંતર {
		જો (i && (flags & PAD_ALTERNATE)) {
			જો (b == 16)
				*--s = 'x' - 'a' + letbase;
			જો (b == 16 || b == 8)
				*--s = '0';
		}
		જો (sign)
			*--s = sign;
	}

	પાછા pc + prints(out, out_len, s, width, flags);
}

સ્થિર પૂર્ણાંક print(અક્ષર **out, u32 *out_len, અચલ અક્ષર *format, va_list args)
{
	bool flags_done;
	પૂર્ણાંક width, flags, pc = 0;
	અક્ષર type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	જો (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	જો (out) {
		જો(!out_len || *out_len)
			**out = '\0';
	}

	માટે (; *format != 0; ++format) {
		width = flags = 0;
		જો (use_tbuf)
			flags |= USE_TBUF;
		જો (*format == '%') {
			++format;
			જો (*format == '\0')
				તોડો;
			જો (*format == '%')
				જાઓ literal;
			/* Get flags */
			flags_done = false;
			જ્યારે (!flags_done) {
				પસંદગી (*format) {
				સ્થિતિ '-':
					flags |= PAD_જમણું;
					તોડો;
				સ્થિતિ '+':
					flags |= PAD_SIGN;
					તોડો;
				સ્થિતિ '#':
					flags |= PAD_ALTERNATE;
					તોડો;
				સ્થિતિ '0':
					flags |= PAD_ZERO;
					તોડો;
				સ્થિતિ ' ':
				સ્થિતિ '\'':
					/* Ignored flags, do nothing */
					તોડો;
				મૂળભૂત:
					flags_done = true;
					તોડો;
				}
				જો (!flags_done)
					++format;
			}
			જો (flags & PAD_જમણું)
				flags &= ~PAD_ZERO;
			/* Get width */
			માટે (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			જો (*format == 's') {
				અક્ષર *s = va_arg(args, અક્ષર *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				ચાલુ;
			}
			જો ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, પૂર્ણાંક),
					     width, flags, *format);
				ચાલુ;
			}
			જો ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned પૂર્ણાંક),
					     width, flags, *format);
				ચાલુ;
			}
			જો ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, શૂન્ય*),
					     width, flags, *format);
				ચાલુ;
			}
			જો (*format == 'l') {
				type = 'i';
				જો (format[1] == 'l') {
					++format;
					જો ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					ચાલુ;
				}
				જો ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				જો ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				નહીંતર
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				ચાલુ;
			}
			જો (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, પૂર્ણાંક);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				ચાલુ;
			}
		} નહીંતર {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	જો (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	પાછા pc;
}

પૂર્ણાંક sbi_sprintf(અક્ષર *out, અચલ અક્ષર *format, ...)
{
	va_list args;
	પૂર્ણાંક retval;

	જો (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, ખાલી, format, args);
	va_end(args);

	પાછા retval;
}

પૂર્ણાંક sbi_snprintf(અક્ષર *out, u32 out_sz, અચલ અક્ષર *format, ...)
{
	va_list args;
	પૂર્ણાંક retval;

	જો (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	પાછા retval;
}

પૂર્ણાંક sbi_printf(અચલ અક્ષર *format, ...)
{
	va_list args;
	પૂર્ણાંક retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(ખાલી, ખાલી, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	પાછા retval;
}

પૂર્ણાંક sbi_dprintf(અચલ અક્ષર *format, ...)
{
	va_list args;
	પૂર્ણાંક retval = 0;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	જો (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(ખાલી, ખાલી, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	પાછા retval;
}

શૂન્ય sbi_panic(અચલ અક્ષર *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(ખાલી, ખાલી, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

અચલ સંરચના sbi_console_device *sbi_console_get_device(શૂન્ય)
{
	પાછા console_dev;
}

શૂન્ય sbi_console_set_device(અચલ સંરચના sbi_console_device *dev)
{
	અક્ષર ch;
	bool flush_early_fifo = false;

	જો (!dev)
		પાછા;

	જો (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	જો (flush_early_fifo) {
		જ્યારે (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
