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

#ವ್ಯಾಖ್ಯಾನಿಸು CONSOLE_TBUF_MAX 256

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *console_dev = ಖಾಲಿ;
ಸ್ಥಿರ ಅಕ್ಷರ console_tbuf[CONSOLE_TBUF_MAX];
ಸ್ಥಿರ u32 console_tbuf_len;
ಸ್ಥಿರ spinlock_t console_ಹೊರಹೋಗು_ಬೀಗ	       = SPIN_ಬೀಗ_INITIALIZER;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ	CONFIG_CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ	256
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ console_early_ಬಫರ್[CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ] = { 0 };
ಸ್ಥಿರ SBI_FIFO_DEFINE(console_early_fifo, console_early_ಬಫರ್, \
		       CONSOLE_EARLY_ಬಫರ್_ಗಾತ್ರ, ಗಾತ್ರ(ಅಕ್ಷರ));

bool sbi_isprintable(ಅಕ್ಷರ c)
{
	ಆದರೆ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		ಹಿಂತಿರುಗಿ true;
	}
	ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ sbi_getc(ಶೂನ್ಯ)
{
	ಆದರೆ (console_dev && console_dev->console_getc)
		ಹಿಂತಿರುಗಿ console_dev->console_getc();
	ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ nputs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
	ಅಕ್ಷರ ch;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	ಆದರೆ (console_dev) {
		ಆದರೆ (console_dev->console_puts)
			ಹಿಂತಿರುಗಿ console_dev->console_puts(str, len);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (console_dev->console_putc) {
			ಗಾಗಿ (i = 0; i < len; i++) {
				ಆದರೆ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಗಾಗಿ (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_ಸರತಿಗೆ_ಸೇರಿಸು(&console_early_fifo, &ch, true);
		}
	}
	ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರ ಶೂನ್ಯ nputs_all(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p = 0;

	ಆಗ (p < len)
		p += nputs(&str[p], len - p);
}

ಶೂನ್ಯ sbi_putc(ಅಕ್ಷರ ch)
{
	nputs_all(&ch, 1);
}

ಶೂನ್ಯ sbi_puts(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len = sbi_strlen(str);

	spin_ಬೀಗ(&console_ಹೊರಹೋಗು_ಬೀಗ);
	nputs_all(str, len);
	spin_unlock(&console_ಹೊರಹೋಗು_ಬೀಗ);
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_nputs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ret;

	spin_ಬೀಗ(&console_ಹೊರಹೋಗು_ಬೀಗ);
	ret = nputs(str, len);
	spin_unlock(&console_ಹೊರಹೋಗು_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ sbi_gets(ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ maxwidth, ಅಕ್ಷರ endchar)
{
	ಪೂರ್ಣಾಂಕ ch;
	ಅಕ್ಷರ *retval = s;

	ಆಗ ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (ಅಕ್ಷರ)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ngets(ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
	ಪೂರ್ಣಾಂಕ ch;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	ಗಾಗಿ (i = 0; i < len; i++) {
		ch = sbi_getc();
		ಆದರೆ (ch < 0)
			ನಿಲ್ಲಿಸು;
		str[i] = ch;
	}

	ಹಿಂತಿರುಗಿ i;
}

#ವ್ಯಾಖ್ಯಾನಿಸು PAD_ಬಲ 1
#ವ್ಯಾಖ್ಯಾನಿಸು PAD_ZERO 2
#ವ್ಯಾಖ್ಯಾನಿಸು PAD_ALTERNATE 4
#ವ್ಯಾಖ್ಯಾನಿಸು PAD_SIGN 8
#ವ್ಯಾಖ್ಯಾನಿಸು USE_TBUF 16
#ವ್ಯಾಖ್ಯಾನಿಸು PRINT_BUF_LEN 64

#ವ್ಯಾಖ್ಯಾನಿಸು va_ಆರಂಭ(v, l) __builtin_va_start((v), l)
#ವ್ಯಾಖ್ಯಾನಿಸು va_ಕೊನೆ __builtin_va_end
#ವ್ಯಾಖ್ಯಾನಿಸು va_arg __builtin_va_arg
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ __builtin_va_list va_list;

ಸ್ಥಿರ ಶೂನ್ಯ printc(ಅಕ್ಷರ **ಹೊರಹೋಗು, u32 *ಹೊರಹೋಗು_len, ಅಕ್ಷರ ch, ಪೂರ್ಣಾಂಕ flags)
{
	ಆದರೆ (!ಹೊರಹೋಗು) {
		sbi_putc(ch);
		ಹಿಂತಿರುಗಿ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	ಆದರೆ (!ಹೊರಹೋಗು_len || *ಹೊರಹೋಗು_len > 1) {
		*(*ಹೊರಹೋಗು)++ = ch;
		**ಹೊರಹೋಗು = '\0';
		ಆದರೆ (ಹೊರಹೋಗು_len) {
			--(*ಹೊರಹೋಗು_len);
			ಆದರೆ ((flags & USE_TBUF) && *ಹೊರಹೋಗು_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *ಹೊರಹೋಗು_len);
				*ಹೊರಹೋಗು = console_tbuf;
				*ಹೊರಹೋಗು_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prints(ಅಕ್ಷರ **ಹೊರಹೋಗು, u32 *ಹೊರಹೋಗು_len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ ಅಗಲ,
		  ಪೂರ್ಣಾಂಕ flags)
{
	ಪೂರ್ಣಾಂಕ pc = 0;
	ಅಗಲ -= sbi_strlen(string);
	ಆದರೆ (!(flags & PAD_ಬಲ)) {
		ಗಾಗಿ (; ಅಗಲ > 0; --ಅಗಲ) {
			printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	ಗಾಗಿ (; *string; ++string) {
		printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, *string, flags);
		++pc;
	}
	ಗಾಗಿ (; ಅಗಲ > 0; --ಅಗಲ) {
		printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, ' ', flags);
		++pc;
	}

	ಹಿಂತಿರುಗಿ pc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ printi(ಅಕ್ಷರ **ಹೊರಹೋಗು, u32 *ಹೊರಹೋಗು_len, ದೀರ್ಘ ದೀರ್ಘ i,
		  ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ type)
{
	ಪೂರ್ಣಾಂಕ pc = 0;
	ಅಕ್ಷರ *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ u, b, t;

	b = 10;
	letbase = 'a';
	ಆದರೆ (type == 'o')
		b = 8;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	ಆದರೆ (type == 'i' || type == 'd') {
		ಆದರೆ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		ಆದರೆ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	ಆದರೆ (!u) {
		*--s = '0';
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆಗ (u) {
			t = u % b;
			u = u / b;
			ಆದರೆ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	ಆದರೆ (flags & PAD_ZERO) {
		ಆದರೆ (sign) {
			printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, sign, flags);
			++pc;
			--ಅಗಲ;
		}
		ಆದರೆ (i && (flags & PAD_ALTERNATE)) {
			ಆದರೆ (b == 16 || b == 8) {
				printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, '0', flags);
				++pc;
				--ಅಗಲ;
			}
			ಆದರೆ (b == 16) {
				printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, 'x' - 'a' + letbase, flags);
				++pc;
				--ಅಗಲ;
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (i && (flags & PAD_ALTERNATE)) {
			ಆದರೆ (b == 16)
				*--s = 'x' - 'a' + letbase;
			ಆದರೆ (b == 16 || b == 8)
				*--s = '0';
		}
		ಆದರೆ (sign)
			*--s = sign;
	}

	ಹಿಂತಿರುಗಿ pc + prints(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, s, ಅಗಲ, flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ print(ಅಕ್ಷರ **ಹೊರಹೋಗು, u32 *ಹೊರಹೋಗು_len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು)
{
	bool flags_ಮುಗಿದ;
	ಪೂರ್ಣಾಂಕ ಅಗಲ, flags, pc = 0;
	ಅಕ್ಷರ type, scr[2], *tout;
	bool use_tbuf = (!ಹೊರಹೋಗು) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	ಆದರೆ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		ಹೊರಹೋಗು = &tout;
		ಹೊರಹೋಗು_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	ಆದರೆ (ಹೊರಹೋಗು) {
		ಆದರೆ(!ಹೊರಹೋಗು_len || *ಹೊರಹೋಗು_len)
			**ಹೊರಹೋಗು = '\0';
	}

	ಗಾಗಿ (; *format != 0; ++format) {
		ಅಗಲ = flags = 0;
		ಆದರೆ (use_tbuf)
			flags |= USE_TBUF;
		ಆದರೆ (*format == '%') {
			++format;
			ಆದರೆ (*format == '\0')
				ನಿಲ್ಲಿಸು;
			ಆದರೆ (*format == '%')
				ಹೋಗು literal;
			/* Get flags */
			flags_ಮುಗಿದ = false;
			ಆಗ (!flags_ಮುಗಿದ) {
				ಆಯ್ಕೆ (*format) {
				ಸ್ಥಿತಿ '-':
					flags |= PAD_ಬಲ;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ '+':
					flags |= PAD_SIGN;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ '#':
					flags |= PAD_ALTERNATE;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ '0':
					flags |= PAD_ZERO;
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ ' ':
				ಸ್ಥಿತಿ '\'':
					/* Ignored flags, do nothing */
					ನಿಲ್ಲಿಸು;
				ಪೂರ್ವನಿಯೋಜಿತ:
					flags_ಮುಗಿದ = true;
					ನಿಲ್ಲಿಸು;
				}
				ಆದರೆ (!flags_ಮುಗಿದ)
					++format;
			}
			ಆದರೆ (flags & PAD_ಬಲ)
				flags &= ~PAD_ZERO;
			/* Get width */
			ಗಾಗಿ (; *format >= '0' && *format <= '9'; ++format) {
				ಅಗಲ *= 10;
				ಅಗಲ += *format - '0';
			}
			ಆದರೆ (*format == 's') {
				ಅಕ್ಷರ *s = va_arg(ವಾದಗಳು, ಅಕ್ಷರ *);
				pc += prints(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, s ? s : "(null)",
					     ಅಗಲ, flags);
				ಮುಂದುವರಿ;
			}
			ಆದರೆ ((*format == 'd') || (*format == 'i')) {
				pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, va_arg(ವಾದಗಳು, ಪೂರ್ಣಾಂಕ),
					     ಅಗಲ, flags, *format);
				ಮುಂದುವರಿ;
			}
			ಆದರೆ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, va_arg(ವಾದಗಳು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ),
					     ಅಗಲ, flags, *format);
				ಮುಂದುವರಿ;
			}
			ಆದರೆ ((*format == 'p') || (*format == 'P')) {
				pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, (uintptr_t)va_arg(ವಾದಗಳು, ಶೂನ್ಯ*),
					     ಅಗಲ, flags, *format);
				ಮುಂದುವರಿ;
			}
			ಆದರೆ (*format == 'l') {
				type = 'i';
				ಆದರೆ (format[1] == 'l') {
					++format;
					ಆದರೆ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, va_arg(ವಾದಗಳು, ದೀರ್ಘ ದೀರ್ಘ),
						ಅಗಲ, flags, type);
					ಮುಂದುವರಿ;
				}
				ಆದರೆ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				ಆದರೆ ((type == 'd') || (type == 'i'))
					pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, va_arg(ವಾದಗಳು, ದೀರ್ಘ),
					     ಅಗಲ, flags, type);
				ಇಲ್ಲದಿದ್ದರೆ
					pc += printi(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, va_arg(ವಾದಗಳು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ),
					     ಅಗಲ, flags, type);
				ಮುಂದುವರಿ;
			}
			ಆದರೆ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(ವಾದಗಳು, ಪೂರ್ಣಾಂಕ);
				scr[1] = '\0';
				pc += prints(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, scr, ಅಗಲ, flags);
				ಮುಂದುವರಿ;
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
literal:
			printc(ಹೊರಹೋಗು, ಹೊರಹೋಗು_len, *format, flags);
			++pc;
		}
	}

	ಆದರೆ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	ಹಿಂತಿರುಗಿ pc;
}

ಪೂರ್ಣಾಂಕ sbi_sprintf(ಅಕ್ಷರ *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
	va_list ವಾದಗಳು;
	ಪೂರ್ಣಾಂಕ retval;

	ಆದರೆ (unlikely(!ಹೊರಹೋಗು))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_ಆರಂಭ(ವಾದಗಳು, format);
	retval = print(&ಹೊರಹೋಗು, ಖಾಲಿ, format, ವಾದಗಳು);
	va_ಕೊನೆ(ವಾದಗಳು);

	ಹಿಂತಿರುಗಿ retval;
}

ಪೂರ್ಣಾಂಕ sbi_snprintf(ಅಕ್ಷರ *ಹೊರಹೋಗು, u32 ಹೊರಹೋಗು_sz, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
	va_list ವಾದಗಳು;
	ಪೂರ್ಣಾಂಕ retval;

	ಆದರೆ (unlikely(!ಹೊರಹೋಗು && ಹೊರಹೋಗು_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_ಆರಂಭ(ವಾದಗಳು, format);
	retval = print(&ಹೊರಹೋಗು, &ಹೊರಹೋಗು_sz, format, ವಾದಗಳು);
	va_ಕೊನೆ(ವಾದಗಳು);

	ಹಿಂತಿರುಗಿ retval;
}

ಪೂರ್ಣಾಂಕ sbi_printf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
	va_list ವಾದಗಳು;
	ಪೂರ್ಣಾಂಕ retval;

	spin_ಬೀಗ(&console_ಹೊರಹೋಗು_ಬೀಗ);
	va_ಆರಂಭ(ವಾದಗಳು, format);
	retval = print(ಖಾಲಿ, ಖಾಲಿ, format, ವಾದಗಳು);
	va_ಕೊನೆ(ವಾದಗಳು);
	spin_unlock(&console_ಹೊರಹೋಗು_ಬೀಗ);

	ಹಿಂತಿರುಗಿ retval;
}

ಪೂರ್ಣಾಂಕ sbi_dprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
	va_list ವಾದಗಳು;
	ಪೂರ್ಣಾಂಕ retval = 0;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_ಆರಂಭ(ವಾದಗಳು, format);
	ಆದರೆ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_ಬೀಗ(&console_ಹೊರಹೋಗು_ಬೀಗ);
		retval = print(ಖಾಲಿ, ಖಾಲಿ, format, ವಾದಗಳು);
		spin_unlock(&console_ಹೊರಹೋಗು_ಬೀಗ);
	}
	va_ಕೊನೆ(ವಾದಗಳು);

	ಹಿಂತಿರುಗಿ retval;
}

ಶೂನ್ಯ sbi_panic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
	va_list ವಾದಗಳು;

	spin_ಬೀಗ(&console_ಹೊರಹೋಗು_ಬೀಗ);
	va_ಆರಂಭ(ವಾದಗಳು, format);
	print(ಖಾಲಿ, ಖಾಲಿ, format, ವಾದಗಳು);
	va_ಕೊನೆ(ವಾದಗಳು);
	spin_unlock(&console_ಹೊರಹೋಗು_ಬೀಗ);

	sbi_hart_hang();
}

ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *sbi_console_get_device(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ console_dev;
}

ಶೂನ್ಯ sbi_console_ಗಣ_device(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_console_device *dev)
{
	ಅಕ್ಷರ ch;
	bool flush_early_fifo = false;

	ಆದರೆ (!dev)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	ಆದರೆ (flush_early_fifo) {
		ಆಗ (!sbi_fifo_ಸರತಿಯಿಂದ_ತೆಗೆ(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
