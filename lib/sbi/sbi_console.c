/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fifo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSOLE_TBUF_MAX 256

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_console_device *console_dev = ਨਿਰਾ;
ਸਥਿਰ ਅੱਖਰ console_tbuf[CONSOLE_TBUF_MAX];
ਸਥਿਰ u32 console_tbuf_len;
ਸਥਿਰ spinlock_t console_ਬਾਹਰਲਾ_ਤਾਲਾ	       = SPIN_ਤਾਲਾ_INITIALIZER;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_CONSOLE_EARLY_ਬਫਰ_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSOLE_EARLY_ਬਫਰ_ਮਾਪ	CONFIG_CONSOLE_EARLY_ਬਫਰ_ਮਾਪ
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSOLE_EARLY_ਬਫਰ_ਮਾਪ	256
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ console_early_ਬਫਰ[CONSOLE_EARLY_ਬਫਰ_ਮਾਪ] = { 0 };
ਸਥਿਰ SBI_FIFO_DEFINE(console_early_fifo, console_early_ਬਫਰ, \
		       CONSOLE_EARLY_ਬਫਰ_ਮਾਪ, ਆਕਾਰ(ਅੱਖਰ));

bool sbi_isprintable(ਅੱਖਰ c)
{
	ਜੇ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ਵਾਪਸ true;
	}
	ਵਾਪਸ false;
}

ਪੂਰਨ sbi_getc(ਖਾਲੀ)
{
	ਜੇ (console_dev && console_dev->console_getc)
		ਵਾਪਸ console_dev->console_getc();
	ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ nputs(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
	ਅੱਖਰ ch;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	ਜੇ (console_dev) {
		ਜੇ (console_dev->console_puts)
			ਵਾਪਸ console_dev->console_puts(str, len);
		ਨਹੀਂਤਾਂ ਜੇ (console_dev->console_putc) {
			ਲਈ (i = 0; i < len; i++) {
				ਜੇ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ਨਹੀਂਤਾਂ {
		ਲਈ (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(&console_early_fifo, &ch, true);
		}
	}
	ਵਾਪਸ len;
}

ਸਥਿਰ ਖਾਲੀ nputs_all(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p = 0;

	ਜਦੋਂ (p < len)
		p += nputs(&str[p], len - p);
}

ਖਾਲੀ sbi_putc(ਅੱਖਰ ch)
{
	nputs_all(&ch, 1);
}

ਖਾਲੀ sbi_puts(ਅਚਲ ਅੱਖਰ *str)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len = sbi_strlen(str);

	spin_ਤਾਲਾ(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
	nputs_all(str, len);
	spin_unlock(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_nputs(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ret;

	spin_ਤਾਲਾ(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
	ret = nputs(str, len);
	spin_unlock(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);

	ਵਾਪਸ ret;
}

ਖਾਲੀ sbi_gets(ਅੱਖਰ *s, ਪੂਰਨ maxwidth, ਅੱਖਰ endchar)
{
	ਪੂਰਨ ch;
	ਅੱਖਰ *retval = s;

	ਜਦੋਂ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (ਅੱਖਰ)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ngets(ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
	ਪੂਰਨ ch;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	ਲਈ (i = 0; i < len; i++) {
		ch = sbi_getc();
		ਜੇ (ch < 0)
			ਰੋਕੋ;
		str[i] = ch;
	}

	ਵਾਪਸ i;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_ਸੱਜਾ 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_ZERO 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_ALTERNATE 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_SIGN 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_TBUF 16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINT_BUF_LEN 64

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ va_ਸ਼ੁਰੂ(v, l) __builtin_va_start((v), l)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ va_ਸਿਰਾ __builtin_va_end
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ va_arg __builtin_va_arg
ਕਿਸਮਪਰਿਭਾਸ਼ਾ __builtin_va_list va_list;

ਸਥਿਰ ਖਾਲੀ printc(ਅੱਖਰ **ਬਾਹਰਲਾ, u32 *ਬਾਹਰਲਾ_len, ਅੱਖਰ ch, ਪੂਰਨ flags)
{
	ਜੇ (!ਬਾਹਰਲਾ) {
		sbi_putc(ch);
		ਵਾਪਸ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ਜੇ (!ਬਾਹਰਲਾ_len || *ਬਾਹਰਲਾ_len > 1) {
		*(*ਬਾਹਰਲਾ)++ = ch;
		**ਬਾਹਰਲਾ = '\0';
		ਜੇ (ਬਾਹਰਲਾ_len) {
			--(*ਬਾਹਰਲਾ_len);
			ਜੇ ((flags & USE_TBUF) && *ਬਾਹਰਲਾ_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *ਬਾਹਰਲਾ_len);
				*ਬਾਹਰਲਾ = console_tbuf;
				*ਬਾਹਰਲਾ_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ਸਥਿਰ ਪੂਰਨ prints(ਅੱਖਰ **ਬਾਹਰਲਾ, u32 *ਬਾਹਰਲਾ_len, ਅਚਲ ਅੱਖਰ *string, ਪੂਰਨ ਚੌੜਾਈ,
		  ਪੂਰਨ flags)
{
	ਪੂਰਨ pc = 0;
	ਚੌੜਾਈ -= sbi_strlen(string);
	ਜੇ (!(flags & PAD_ਸੱਜਾ)) {
		ਲਈ (; ਚੌੜਾਈ > 0; --ਚੌੜਾਈ) {
			printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	ਲਈ (; *string; ++string) {
		printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, *string, flags);
		++pc;
	}
	ਲਈ (; ਚੌੜਾਈ > 0; --ਚੌੜਾਈ) {
		printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, ' ', flags);
		++pc;
	}

	ਵਾਪਸ pc;
}

ਸਥਿਰ ਪੂਰਨ printi(ਅੱਖਰ **ਬਾਹਰਲਾ, u32 *ਬਾਹਰਲਾ_len, ਲੰਮਾ ਲੰਮਾ i,
		  ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ flags, ਪੂਰਨ type)
{
	ਪੂਰਨ pc = 0;
	ਅੱਖਰ *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ u, b, t;

	b = 10;
	letbase = 'a';
	ਜੇ (type == 'o')
		b = 8;
	ਨਹੀਂਤਾਂ ਜੇ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ਜੇ (type == 'i' || type == 'd') {
		ਜੇ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ਜੇ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ਜੇ (!u) {
		*--s = '0';
	} ਨਹੀਂਤਾਂ {
		ਜਦੋਂ (u) {
			t = u % b;
			u = u / b;
			ਜੇ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ਜੇ (flags & PAD_ZERO) {
		ਜੇ (sign) {
			printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, sign, flags);
			++pc;
			--ਚੌੜਾਈ;
		}
		ਜੇ (i && (flags & PAD_ALTERNATE)) {
			ਜੇ (b == 16 || b == 8) {
				printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, '0', flags);
				++pc;
				--ਚੌੜਾਈ;
			}
			ਜੇ (b == 16) {
				printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, 'x' - 'a' + letbase, flags);
				++pc;
				--ਚੌੜਾਈ;
			}
		}
	} ਨਹੀਂਤਾਂ {
		ਜੇ (i && (flags & PAD_ALTERNATE)) {
			ਜੇ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ਜੇ (b == 16 || b == 8)
				*--s = '0';
		}
		ਜੇ (sign)
			*--s = sign;
	}

	ਵਾਪਸ pc + prints(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, s, ਚੌੜਾਈ, flags);
}

ਸਥਿਰ ਪੂਰਨ print(ਅੱਖਰ **ਬਾਹਰਲਾ, u32 *ਬਾਹਰਲਾ_len, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ)
{
	bool flags_ਪੂਰਾ;
	ਪੂਰਨ ਚੌੜਾਈ, flags, pc = 0;
	ਅੱਖਰ type, scr[2], *tout;
	bool use_tbuf = (!ਬਾਹਰਲਾ) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ਜੇ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		ਬਾਹਰਲਾ = &tout;
		ਬਾਹਰਲਾ_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ਜੇ (ਬਾਹਰਲਾ) {
		ਜੇ(!ਬਾਹਰਲਾ_len || *ਬਾਹਰਲਾ_len)
			**ਬਾਹਰਲਾ = '\0';
	}

	ਲਈ (; *format != 0; ++format) {
		ਚੌੜਾਈ = flags = 0;
		ਜੇ (use_tbuf)
			flags |= USE_TBUF;
		ਜੇ (*format == '%') {
			++format;
			ਜੇ (*format == '\0')
				ਰੋਕੋ;
			ਜੇ (*format == '%')
				ਜਾਓ literal;
			/* Get flags */
			flags_ਪੂਰਾ = false;
			ਜਦੋਂ (!flags_ਪੂਰਾ) {
				ਚੋਣ (*format) {
				ਹਾਲਤ '-':
					flags |= PAD_ਸੱਜਾ;
					ਰੋਕੋ;
				ਹਾਲਤ '+':
					flags |= PAD_SIGN;
					ਰੋਕੋ;
				ਹਾਲਤ '#':
					flags |= PAD_ALTERNATE;
					ਰੋਕੋ;
				ਹਾਲਤ '0':
					flags |= PAD_ZERO;
					ਰੋਕੋ;
				ਹਾਲਤ ' ':
				ਹਾਲਤ '\'':
					/* Ignored flags, do nothing */
					ਰੋਕੋ;
				ਮੂਲ:
					flags_ਪੂਰਾ = true;
					ਰੋਕੋ;
				}
				ਜੇ (!flags_ਪੂਰਾ)
					++format;
			}
			ਜੇ (flags & PAD_ਸੱਜਾ)
				flags &= ~PAD_ZERO;
			/* Get width */
			ਲਈ (; *format >= '0' && *format <= '9'; ++format) {
				ਚੌੜਾਈ *= 10;
				ਚੌੜਾਈ += *format - '0';
			}
			ਜੇ (*format == 's') {
				ਅੱਖਰ *s = va_arg(ਦਲੀਲਾਂ, ਅੱਖਰ *);
				pc += prints(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, s ? s : "(null)",
					     ਚੌੜਾਈ, flags);
				ਜਾਰੀ;
			}
			ਜੇ ((*format == 'd') || (*format == 'i')) {
				pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, va_arg(ਦਲੀਲਾਂ, ਪੂਰਨ),
					     ਚੌੜਾਈ, flags, *format);
				ਜਾਰੀ;
			}
			ਜੇ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, va_arg(ਦਲੀਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ),
					     ਚੌੜਾਈ, flags, *format);
				ਜਾਰੀ;
			}
			ਜੇ ((*format == 'p') || (*format == 'P')) {
				pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, (uintptr_t)va_arg(ਦਲੀਲਾਂ, ਖਾਲੀ*),
					     ਚੌੜਾਈ, flags, *format);
				ਜਾਰੀ;
			}
			ਜੇ (*format == 'l') {
				type = 'i';
				ਜੇ (format[1] == 'l') {
					++format;
					ਜੇ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, va_arg(ਦਲੀਲਾਂ, ਲੰਮਾ ਲੰਮਾ),
						ਚੌੜਾਈ, flags, type);
					ਜਾਰੀ;
				}
				ਜੇ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ਜੇ ((type == 'd') || (type == 'i'))
					pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, va_arg(ਦਲੀਲਾਂ, ਲੰਮਾ),
					     ਚੌੜਾਈ, flags, type);
				ਨਹੀਂਤਾਂ
					pc += printi(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, va_arg(ਦਲੀਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ),
					     ਚੌੜਾਈ, flags, type);
				ਜਾਰੀ;
			}
			ਜੇ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(ਦਲੀਲਾਂ, ਪੂਰਨ);
				scr[1] = '\0';
				pc += prints(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, scr, ਚੌੜਾਈ, flags);
				ਜਾਰੀ;
			}
		} ਨਹੀਂਤਾਂ {
literal:
			printc(ਬਾਹਰਲਾ, ਬਾਹਰਲਾ_len, *format, flags);
			++pc;
		}
	}

	ਜੇ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ਵਾਪਸ pc;
}

ਪੂਰਨ sbi_sprintf(ਅੱਖਰ *ਬਾਹਰਲਾ, ਅਚਲ ਅੱਖਰ *format, ...)
{
	va_list ਦਲੀਲਾਂ;
	ਪੂਰਨ retval;

	ਜੇ (unlikely(!ਬਾਹਰਲਾ))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_ਸ਼ੁਰੂ(ਦਲੀਲਾਂ, format);
	retval = print(&ਬਾਹਰਲਾ, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
	va_ਸਿਰਾ(ਦਲੀਲਾਂ);

	ਵਾਪਸ retval;
}

ਪੂਰਨ sbi_snprintf(ਅੱਖਰ *ਬਾਹਰਲਾ, u32 ਬਾਹਰਲਾ_sz, ਅਚਲ ਅੱਖਰ *format, ...)
{
	va_list ਦਲੀਲਾਂ;
	ਪੂਰਨ retval;

	ਜੇ (unlikely(!ਬਾਹਰਲਾ && ਬਾਹਰਲਾ_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_ਸ਼ੁਰੂ(ਦਲੀਲਾਂ, format);
	retval = print(&ਬਾਹਰਲਾ, &ਬਾਹਰਲਾ_sz, format, ਦਲੀਲਾਂ);
	va_ਸਿਰਾ(ਦਲੀਲਾਂ);

	ਵਾਪਸ retval;
}

ਪੂਰਨ sbi_printf(ਅਚਲ ਅੱਖਰ *format, ...)
{
	va_list ਦਲੀਲਾਂ;
	ਪੂਰਨ retval;

	spin_ਤਾਲਾ(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
	va_ਸ਼ੁਰੂ(ਦਲੀਲਾਂ, format);
	retval = print(ਨਿਰਾ, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
	va_ਸਿਰਾ(ਦਲੀਲਾਂ);
	spin_unlock(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);

	ਵਾਪਸ retval;
}

ਪੂਰਨ sbi_dprintf(ਅਚਲ ਅੱਖਰ *format, ...)
{
	va_list ਦਲੀਲਾਂ;
	ਪੂਰਨ retval = 0;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_ਸ਼ੁਰੂ(ਦਲੀਲਾਂ, format);
	ਜੇ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_ਤਾਲਾ(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
		retval = print(ਨਿਰਾ, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
		spin_unlock(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
	}
	va_ਸਿਰਾ(ਦਲੀਲਾਂ);

	ਵਾਪਸ retval;
}

ਖਾਲੀ sbi_panic(ਅਚਲ ਅੱਖਰ *format, ...)
{
	va_list ਦਲੀਲਾਂ;

	spin_ਤਾਲਾ(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);
	va_ਸ਼ੁਰੂ(ਦਲੀਲਾਂ, format);
	print(ਨਿਰਾ, ਨਿਰਾ, format, ਦਲੀਲਾਂ);
	va_ਸਿਰਾ(ਦਲੀਲਾਂ);
	spin_unlock(&console_ਬਾਹਰਲਾ_ਤਾਲਾ);

	sbi_hart_hang();
}

ਅਚਲ ਬਣਤਰ sbi_console_device *sbi_console_get_device(ਖਾਲੀ)
{
	ਵਾਪਸ console_dev;
}

ਖਾਲੀ sbi_console_ਸੈੱਟ_device(ਅਚਲ ਬਣਤਰ sbi_console_device *dev)
{
	ਅੱਖਰ ch;
	bool flush_early_fifo = false;

	ਜੇ (!dev)
		ਵਾਪਸ;

	ਜੇ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ਜੇ (flush_early_fifo) {
		ਜਦੋਂ (!sbi_fifo_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
