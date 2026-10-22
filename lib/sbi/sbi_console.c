/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fifo.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_console_device *console_dev = ರಿಕ್ತ;
ಸ್ಥಿರ ವರ್ಣ console_tbuf[CONSOLE_TBUF_MAX];
ಸ್ಥಿರ u32 console_tbuf_len;
ಸ್ಥಿರ spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#ಅನ್ಯಥಾ
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
ಸ್ಥಿರ ವರ್ಣ console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
ಸ್ಥಿರ SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, ಪರಿಮಾಣ(ವರ್ಣ));

bool sbi_isprintable(ವರ್ಣ c)
{
	ಯದಿ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ಪ್ರತ್ಯಾಗಮ true;
	}
	ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ sbi_getc(ಶೂನ್ಯ)
{
	ಯದಿ (console_dev && console_dev->console_getc)
		ಪ್ರತ್ಯಾಗಮ console_dev->console_getc();
	ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ unsigned long nputs(ಧ್ರುವ ವರ್ಣ *str, unsigned long len)
{
	ವರ್ಣ ch;
	unsigned long i;

	ಯದಿ (console_dev) {
		ಯದಿ (console_dev->console_puts)
			ಪ್ರತ್ಯಾಗಮ console_dev->console_puts(str, len);
		ಅನ್ಯಥಾ ಯದಿ (console_dev->console_putc) {
			ಪ್ರತಿ (i = 0; i < len; i++) {
				ಯದಿ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ಅನ್ಯಥಾ {
		ಪ್ರತಿ (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	ಪ್ರತ್ಯಾಗಮ len;
}

ಸ್ಥಿರ ಶೂನ್ಯ nputs_all(ಧ್ರುವ ವರ್ಣ *str, unsigned long len)
{
	unsigned long p = 0;

	ಯಾವತ್ (p < len)
		p += nputs(&str[p], len - p);
}

ಶೂನ್ಯ sbi_putc(ವರ್ಣ ch)
{
	nputs_all(&ch, 1);
}

ಶೂನ್ಯ sbi_puts(ಧ್ರುವ ವರ್ಣ *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ಧ್ರುವ ವರ್ಣ *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ sbi_gets(ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ maxwidth, ವರ್ಣ endchar)
{
	ಪೂರ್ಣಾಙ್ಕ ch;
	ವರ್ಣ *retval = s;

	ಯಾವತ್ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (ವರ್ಣ)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(ವರ್ಣ *str, unsigned long len)
{
	ಪೂರ್ಣಾಙ್ಕ ch;
	unsigned long i;

	ಪ್ರತಿ (i = 0; i < len; i++) {
		ch = sbi_getc();
		ಯದಿ (ch < 0)
			ಭಙ್ಗ;
		str[i] = ch;
	}

	ಪ್ರತ್ಯಾಗಮ i;
}

#define PAD_ಬಲ 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
ಪ್ರಕಾರನಿರ್ದೇಶ __builtin_va_list va_list;

ಸ್ಥಿರ ಶೂನ್ಯ printc(ವರ್ಣ **out, u32 *out_len, ವರ್ಣ ch, ಪೂರ್ಣಾಙ್ಕ flags)
{
	ಯದಿ (!out) {
		sbi_putc(ch);
		ಪ್ರತ್ಯಾಗಮ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ಯದಿ (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		ಯದಿ (out_len) {
			--(*out_len);
			ಯದಿ ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prints(ವರ್ಣ **out, u32 *out_len, ಧ್ರುವ ವರ್ಣ *string, ಪೂರ್ಣಾಙ್ಕ width,
		  ಪೂರ್ಣಾಙ್ಕ flags)
{
	ಪೂರ್ಣಾಙ್ಕ pc = 0;
	width -= sbi_strlen(string);
	ಯದಿ (!(flags & PAD_ಬಲ)) {
		ಪ್ರತಿ (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	ಪ್ರತಿ (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	ಪ್ರತಿ (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	ಪ್ರತ್ಯಾಗಮ pc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ printi(ವರ್ಣ **out, u32 *out_len, long long i,
		  ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ type)
{
	ಪೂರ್ಣಾಙ್ಕ pc = 0;
	ವರ್ಣ *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	ಯದಿ (type == 'o')
		b = 8;
	ಅನ್ಯಥಾ ಯದಿ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ಯದಿ (type == 'i' || type == 'd') {
		ಯದಿ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ಯದಿ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ಯದಿ (!u) {
		*--s = '0';
	} ಅನ್ಯಥಾ {
		ಯಾವತ್ (u) {
			t = u % b;
			u = u / b;
			ಯದಿ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ಯದಿ (flags & PAD_ZERO) {
		ಯದಿ (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		ಯದಿ (i && (flags & PAD_ALTERNATE)) {
			ಯದಿ (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			ಯದಿ (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} ಅನ್ಯಥಾ {
		ಯದಿ (i && (flags & PAD_ALTERNATE)) {
			ಯದಿ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ಯದಿ (b == 16 || b == 8)
				*--s = '0';
		}
		ಯದಿ (sign)
			*--s = sign;
	}

	ಪ್ರತ್ಯಾಗಮ pc + prints(out, out_len, s, width, flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ print(ವರ್ಣ **out, u32 *out_len, ಧ್ರುವ ವರ್ಣ *format, va_list args)
{
	bool flags_done;
	ಪೂರ್ಣಾಙ್ಕ width, flags, pc = 0;
	ವರ್ಣ type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ಯದಿ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ಯದಿ (out) {
		ಯದಿ(!out_len || *out_len)
			**out = '\0';
	}

	ಪ್ರತಿ (; *format != 0; ++format) {
		width = flags = 0;
		ಯದಿ (use_tbuf)
			flags |= USE_TBUF;
		ಯದಿ (*format == '%') {
			++format;
			ಯದಿ (*format == '\0')
				ಭಙ್ಗ;
			ಯದಿ (*format == '%')
				ಗಚ್ಛ literal;
			/* Get flags */
			flags_done = false;
			ಯಾವತ್ (!flags_done) {
				ವಿಕಲ್ಪ (*format) {
				ಪಕ್ಷ '-':
					flags |= PAD_ಬಲ;
					ಭಙ್ಗ;
				ಪಕ್ಷ '+':
					flags |= PAD_SIGN;
					ಭಙ್ಗ;
				ಪಕ್ಷ '#':
					flags |= PAD_ALTERNATE;
					ಭಙ್ಗ;
				ಪಕ್ಷ '0':
					flags |= PAD_ZERO;
					ಭಙ್ಗ;
				ಪಕ್ಷ ' ':
				ಪಕ್ಷ '\'':
					/* Ignored flags, do nothing */
					ಭಙ್ಗ;
				ಸ್ವಭಾವ:
					flags_done = true;
					ಭಙ್ಗ;
				}
				ಯದಿ (!flags_done)
					++format;
			}
			ಯದಿ (flags & PAD_ಬಲ)
				flags &= ~PAD_ZERO;
			/* Get width */
			ಪ್ರತಿ (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			ಯದಿ (*format == 's') {
				ವರ್ಣ *s = va_arg(args, ವರ್ಣ *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				ಅನುವರ್ತ;
			}
			ಯದಿ ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, ಪೂರ್ಣಾಙ್ಕ),
					     width, flags, *format);
				ಅನುವರ್ತ;
			}
			ಯದಿ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned ಪೂರ್ಣಾಙ್ಕ),
					     width, flags, *format);
				ಅನುವರ್ತ;
			}
			ಯದಿ ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, ಶೂನ್ಯ*),
					     width, flags, *format);
				ಅನುವರ್ತ;
			}
			ಯದಿ (*format == 'l') {
				type = 'i';
				ಯದಿ (format[1] == 'l') {
					++format;
					ಯದಿ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					ಅನುವರ್ತ;
				}
				ಯದಿ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ಯದಿ ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				ಅನ್ಯಥಾ
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				ಅನುವರ್ತ;
			}
			ಯದಿ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, ಪೂರ್ಣಾಙ್ಕ);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				ಅನುವರ್ತ;
			}
		} ಅನ್ಯಥಾ {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	ಯದಿ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ಪ್ರತ್ಯಾಗಮ pc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_sprintf(ವರ್ಣ *out, ಧ್ರುವ ವರ್ಣ *format, ...)
{
	va_list args;
	ಪೂರ್ಣಾಙ್ಕ retval;

	ಯದಿ (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, ರಿಕ್ತ, format, args);
	va_end(args);

	ಪ್ರತ್ಯಾಗಮ retval;
}

ಪೂರ್ಣಾಙ್ಕ sbi_snprintf(ವರ್ಣ *out, u32 out_sz, ಧ್ರುವ ವರ್ಣ *format, ...)
{
	va_list args;
	ಪೂರ್ಣಾಙ್ಕ retval;

	ಯದಿ (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	ಪ್ರತ್ಯಾಗಮ retval;
}

ಪೂರ್ಣಾಙ್ಕ sbi_printf(ಧ್ರುವ ವರ್ಣ *format, ...)
{
	va_list args;
	ಪೂರ್ಣಾಙ್ಕ retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(ರಿಕ್ತ, ರಿಕ್ತ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	ಪ್ರತ್ಯಾಗಮ retval;
}

ಪೂರ್ಣಾಙ್ಕ sbi_dprintf(ಧ್ರುವ ವರ್ಣ *format, ...)
{
	va_list args;
	ಪೂರ್ಣಾಙ್ಕ retval = 0;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	ಯದಿ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(ರಿಕ್ತ, ರಿಕ್ತ, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	ಪ್ರತ್ಯಾಗಮ retval;
}

ಶೂನ್ಯ sbi_panic(ಧ್ರುವ ವರ್ಣ *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(ರಿಕ್ತ, ರಿಕ್ತ, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ಧ್ರುವ ಸಂರಚನಾ sbi_console_device *sbi_console_get_device(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ console_dev;
}

ಶೂನ್ಯ sbi_console_set_device(ಧ್ರುವ ಸಂರಚನಾ sbi_console_device *dev)
{
	ವರ್ಣ ch;
	bool flush_early_fifo = false;

	ಯದಿ (!dev)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ಯದಿ (flush_early_fifo) {
		ಯಾವತ್ (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
