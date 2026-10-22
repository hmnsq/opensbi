/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_fifo.h>
#അന്തര്ഭാവ <sbi/sbi_hart.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

സ്ഥിതം സ്ഥിരം ഘടന sbi_console_device *console_dev = ശൂന്യ;
സ്ഥിതം അക്ഷരം console_tbuf[CONSOLE_TBUF_MAX];
സ്ഥിതം u32 console_tbuf_len;
സ്ഥിതം spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#അല്ലെങ്കിൽ
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
സ്ഥിതം അക്ഷരം console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
സ്ഥിതം SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, വലുപ്പം(അക്ഷരം));

bool sbi_isprintable(അക്ഷരം c)
{
	എങ്കിൽ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		തിരികെ true;
	}
	തിരികെ false;
}

പൂർണ്ണസംഖ്യ sbi_getc(ശൂന്യം)
{
	എങ്കിൽ (console_dev && console_dev->console_getc)
		തിരികെ console_dev->console_getc();
	തിരികെ -1;
}

സ്ഥിതം unsigned long nputs(സ്ഥിരം അക്ഷരം *str, unsigned long len)
{
	അക്ഷരം ch;
	unsigned long i;

	എങ്കിൽ (console_dev) {
		എങ്കിൽ (console_dev->console_puts)
			തിരികെ console_dev->console_puts(str, len);
		അല്ലെങ്കിൽ എങ്കിൽ (console_dev->console_putc) {
			വേണ്ടി (i = 0; i < len; i++) {
				എങ്കിൽ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} അല്ലെങ്കിൽ {
		വേണ്ടി (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	തിരികെ len;
}

സ്ഥിതം ശൂന്യം nputs_all(സ്ഥിരം അക്ഷരം *str, unsigned long len)
{
	unsigned long p = 0;

	ആകുമ്പോൾ (p < len)
		p += nputs(&str[p], len - p);
}

ശൂന്യം sbi_putc(അക്ഷരം ch)
{
	nputs_all(&ch, 1);
}

ശൂന്യം sbi_puts(സ്ഥിരം അക്ഷരം *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(സ്ഥിരം അക്ഷരം *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	തിരികെ ret;
}

ശൂന്യം sbi_gets(അക്ഷരം *s, പൂർണ്ണസംഖ്യ maxwidth, അക്ഷരം endchar)
{
	പൂർണ്ണസംഖ്യ ch;
	അക്ഷരം *retval = s;

	ആകുമ്പോൾ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (അക്ഷരം)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(അക്ഷരം *str, unsigned long len)
{
	പൂർണ്ണസംഖ്യ ch;
	unsigned long i;

	വേണ്ടി (i = 0; i < len; i++) {
		ch = sbi_getc();
		എങ്കിൽ (ch < 0)
			നിർത്തുക;
		str[i] = ch;
	}

	തിരികെ i;
}

#define PAD_വലത് 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
തരനിർവചനം __builtin_va_list va_list;

സ്ഥിതം ശൂന്യം printc(അക്ഷരം **out, u32 *out_len, അക്ഷരം ch, പൂർണ്ണസംഖ്യ flags)
{
	എങ്കിൽ (!out) {
		sbi_putc(ch);
		തിരികെ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	എങ്കിൽ (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		എങ്കിൽ (out_len) {
			--(*out_len);
			എങ്കിൽ ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ prints(അക്ഷരം **out, u32 *out_len, സ്ഥിരം അക്ഷരം *string, പൂർണ്ണസംഖ്യ width,
		  പൂർണ്ണസംഖ്യ flags)
{
	പൂർണ്ണസംഖ്യ pc = 0;
	width -= sbi_strlen(string);
	എങ്കിൽ (!(flags & PAD_വലത്)) {
		വേണ്ടി (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	വേണ്ടി (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	വേണ്ടി (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	തിരികെ pc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ printi(അക്ഷരം **out, u32 *out_len, long long i,
		  പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ type)
{
	പൂർണ്ണസംഖ്യ pc = 0;
	അക്ഷരം *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	എങ്കിൽ (type == 'o')
		b = 8;
	അല്ലെങ്കിൽ എങ്കിൽ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	എങ്കിൽ (type == 'i' || type == 'd') {
		എങ്കിൽ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		എങ്കിൽ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	എങ്കിൽ (!u) {
		*--s = '0';
	} അല്ലെങ്കിൽ {
		ആകുമ്പോൾ (u) {
			t = u % b;
			u = u / b;
			എങ്കിൽ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	എങ്കിൽ (flags & PAD_ZERO) {
		എങ്കിൽ (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		എങ്കിൽ (i && (flags & PAD_ALTERNATE)) {
			എങ്കിൽ (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			എങ്കിൽ (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (i && (flags & PAD_ALTERNATE)) {
			എങ്കിൽ (b == 16)
				*--s = 'x' - 'a' + letbase;
			എങ്കിൽ (b == 16 || b == 8)
				*--s = '0';
		}
		എങ്കിൽ (sign)
			*--s = sign;
	}

	തിരികെ pc + prints(out, out_len, s, width, flags);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ print(അക്ഷരം **out, u32 *out_len, സ്ഥിരം അക്ഷരം *format, va_list args)
{
	bool flags_done;
	പൂർണ്ണസംഖ്യ width, flags, pc = 0;
	അക്ഷരം type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	എങ്കിൽ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	എങ്കിൽ (out) {
		എങ്കിൽ(!out_len || *out_len)
			**out = '\0';
	}

	വേണ്ടി (; *format != 0; ++format) {
		width = flags = 0;
		എങ്കിൽ (use_tbuf)
			flags |= USE_TBUF;
		എങ്കിൽ (*format == '%') {
			++format;
			എങ്കിൽ (*format == '\0')
				നിർത്തുക;
			എങ്കിൽ (*format == '%')
				പോകുക literal;
			/* Get flags */
			flags_done = false;
			ആകുമ്പോൾ (!flags_done) {
				തിരഞ്ഞെടുപ്പ് (*format) {
				അവസ്ഥ '-':
					flags |= PAD_വലത്;
					നിർത്തുക;
				അവസ്ഥ '+':
					flags |= PAD_SIGN;
					നിർത്തുക;
				അവസ്ഥ '#':
					flags |= PAD_ALTERNATE;
					നിർത്തുക;
				അവസ്ഥ '0':
					flags |= PAD_ZERO;
					നിർത്തുക;
				അവസ്ഥ ' ':
				അവസ്ഥ '\'':
					/* Ignored flags, do nothing */
					നിർത്തുക;
				സ്ഥിരസ്ഥിതി:
					flags_done = true;
					നിർത്തുക;
				}
				എങ്കിൽ (!flags_done)
					++format;
			}
			എങ്കിൽ (flags & PAD_വലത്)
				flags &= ~PAD_ZERO;
			/* Get width */
			വേണ്ടി (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			എങ്കിൽ (*format == 's') {
				അക്ഷരം *s = va_arg(args, അക്ഷരം *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				തുടരുക;
			}
			എങ്കിൽ ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, പൂർണ്ണസംഖ്യ),
					     width, flags, *format);
				തുടരുക;
			}
			എങ്കിൽ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned പൂർണ്ണസംഖ്യ),
					     width, flags, *format);
				തുടരുക;
			}
			എങ്കിൽ ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, ശൂന്യം*),
					     width, flags, *format);
				തുടരുക;
			}
			എങ്കിൽ (*format == 'l') {
				type = 'i';
				എങ്കിൽ (format[1] == 'l') {
					++format;
					എങ്കിൽ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					തുടരുക;
				}
				എങ്കിൽ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				എങ്കിൽ ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				അല്ലെങ്കിൽ
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				തുടരുക;
			}
			എങ്കിൽ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, പൂർണ്ണസംഖ്യ);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				തുടരുക;
			}
		} അല്ലെങ്കിൽ {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	എങ്കിൽ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	തിരികെ pc;
}

പൂർണ്ണസംഖ്യ sbi_sprintf(അക്ഷരം *out, സ്ഥിരം അക്ഷരം *format, ...)
{
	va_list args;
	പൂർണ്ണസംഖ്യ retval;

	എങ്കിൽ (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, ശൂന്യ, format, args);
	va_end(args);

	തിരികെ retval;
}

പൂർണ്ണസംഖ്യ sbi_snprintf(അക്ഷരം *out, u32 out_sz, സ്ഥിരം അക്ഷരം *format, ...)
{
	va_list args;
	പൂർണ്ണസംഖ്യ retval;

	എങ്കിൽ (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	തിരികെ retval;
}

പൂർണ്ണസംഖ്യ sbi_printf(സ്ഥിരം അക്ഷരം *format, ...)
{
	va_list args;
	പൂർണ്ണസംഖ്യ retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(ശൂന്യ, ശൂന്യ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	തിരികെ retval;
}

പൂർണ്ണസംഖ്യ sbi_dprintf(സ്ഥിരം അക്ഷരം *format, ...)
{
	va_list args;
	പൂർണ്ണസംഖ്യ retval = 0;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	എങ്കിൽ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(ശൂന്യ, ശൂന്യ, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	തിരികെ retval;
}

ശൂന്യം sbi_panic(സ്ഥിരം അക്ഷരം *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(ശൂന്യ, ശൂന്യ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

സ്ഥിരം ഘടന sbi_console_device *sbi_console_get_device(ശൂന്യം)
{
	തിരികെ console_dev;
}

ശൂന്യം sbi_console_set_device(സ്ഥിരം ഘടന sbi_console_device *dev)
{
	അക്ഷരം ch;
	bool flush_early_fifo = false;

	എങ്കിൽ (!dev)
		തിരികെ;

	എങ്കിൽ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	എങ്കിൽ (flush_early_fifo) {
		ആകുമ്പോൾ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
