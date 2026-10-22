/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_fifo.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_string.h>

#تعریف_کرو CONSOLE_TBUF_MAX 256

ساکن ثابت ساخت sbi_console_device *console_dev = تہی;
ساکن حرف console_tbuf[CONSOLE_TBUF_MAX];
ساکن u32 console_tbuf_len;
ساکن spinlock_t console_برآمد_تالا	       = SPIN_تالا_INITIALIZER;

#اگر_تعریف_شدہ CONFIG_CONSOLE_EARLY_بفر_سائز
#تعریف_کرو CONSOLE_EARLY_بفر_سائز	CONFIG_CONSOLE_EARLY_بفر_سائز
#ورنہ
#تعریف_کرو CONSOLE_EARLY_بفر_سائز	256
#اگر_ختم
ساکن حرف console_early_بفر[CONSOLE_EARLY_بفر_سائز] = { 0 };
ساکن SBI_FIFO_DEFINE(console_early_fifo, console_early_بفر, \
		       CONSOLE_EARLY_بفر_سائز, حجم(حرف));

bool sbi_isprintable(حرف c)
{
	اگر (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		واپس true;
	}
	واپس false;
}

صحیح sbi_getc(خالی)
{
	اگر (console_dev && console_dev->console_getc)
		واپس console_dev->console_getc();
	واپس -1;
}

ساکن غیرعلامتی طویل nputs(ثابت حرف *str, غیرعلامتی طویل len)
{
	حرف ch;
	غیرعلامتی طویل i;

	اگر (console_dev) {
		اگر (console_dev->console_puts)
			واپس console_dev->console_puts(str, len);
		ورنہ اگر (console_dev->console_putc) {
			برائے (i = 0; i < len; i++) {
				اگر (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} ورنہ {
		برائے (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_قطار_میں_ڈالو(&console_early_fifo, &ch, true);
		}
	}
	واپس len;
}

ساکن خالی nputs_all(ثابت حرف *str, غیرعلامتی طویل len)
{
	غیرعلامتی طویل p = 0;

	جب (p < len)
		p += nputs(&str[p], len - p);
}

خالی sbi_putc(حرف ch)
{
	nputs_all(&ch, 1);
}

خالی sbi_puts(ثابت حرف *str)
{
	غیرعلامتی طویل len = sbi_strlen(str);

	spin_تالا(&console_برآمد_تالا);
	nputs_all(str, len);
	spin_unlock(&console_برآمد_تالا);
}

غیرعلامتی طویل sbi_nputs(ثابت حرف *str, غیرعلامتی طویل len)
{
	غیرعلامتی طویل ret;

	spin_تالا(&console_برآمد_تالا);
	ret = nputs(str, len);
	spin_unlock(&console_برآمد_تالا);

	واپس ret;
}

خالی sbi_gets(حرف *s, صحیح maxwidth, حرف endchar)
{
	صحیح ch;
	حرف *retval = s;

	جب ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (حرف)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

غیرعلامتی طویل sbi_ngets(حرف *str, غیرعلامتی طویل len)
{
	صحیح ch;
	غیرعلامتی طویل i;

	برائے (i = 0; i < len; i++) {
		ch = sbi_getc();
		اگر (ch < 0)
			توڑو;
		str[i] = ch;
	}

	واپس i;
}

#تعریف_کرو PAD_دایاں 1
#تعریف_کرو PAD_ZERO 2
#تعریف_کرو PAD_ALTERNATE 4
#تعریف_کرو PAD_SIGN 8
#تعریف_کرو USE_TBUF 16
#تعریف_کرو PRINT_BUF_LEN 64

#تعریف_کرو va_آغاز(v, l) __builtin_va_start((v), l)
#تعریف_کرو va_انتہا __builtin_va_end
#تعریف_کرو va_arg __builtin_va_arg
قسمتعریف __builtin_va_list va_list;

ساکن خالی printc(حرف **برآمد, u32 *برآمد_len, حرف ch, صحیح flags)
{
	اگر (!برآمد) {
		sbi_putc(ch);
		واپس;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	اگر (!برآمد_len || *برآمد_len > 1) {
		*(*برآمد)++ = ch;
		**برآمد = '\0';
		اگر (برآمد_len) {
			--(*برآمد_len);
			اگر ((flags & USE_TBUF) && *برآمد_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *برآمد_len);
				*برآمد = console_tbuf;
				*برآمد_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

ساکن صحیح prints(حرف **برآمد, u32 *برآمد_len, ثابت حرف *string, صحیح چوڑائی,
		  صحیح flags)
{
	صحیح pc = 0;
	چوڑائی -= sbi_strlen(string);
	اگر (!(flags & PAD_دایاں)) {
		برائے (; چوڑائی > 0; --چوڑائی) {
			printc(برآمد, برآمد_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	برائے (; *string; ++string) {
		printc(برآمد, برآمد_len, *string, flags);
		++pc;
	}
	برائے (; چوڑائی > 0; --چوڑائی) {
		printc(برآمد, برآمد_len, ' ', flags);
		++pc;
	}

	واپس pc;
}

ساکن صحیح printi(حرف **برآمد, u32 *برآمد_len, طویل طویل i,
		  صحیح چوڑائی, صحیح flags, صحیح type)
{
	صحیح pc = 0;
	حرف *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	غیرعلامتی طویل طویل u, b, t;

	b = 10;
	letbase = 'a';
	اگر (type == 'o')
		b = 8;
	ورنہ اگر (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	اگر (type == 'i' || type == 'd') {
		اگر ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		اگر (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	اگر (!u) {
		*--s = '0';
	} ورنہ {
		جب (u) {
			t = u % b;
			u = u / b;
			اگر (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	اگر (flags & PAD_ZERO) {
		اگر (sign) {
			printc(برآمد, برآمد_len, sign, flags);
			++pc;
			--چوڑائی;
		}
		اگر (i && (flags & PAD_ALTERNATE)) {
			اگر (b == 16 || b == 8) {
				printc(برآمد, برآمد_len, '0', flags);
				++pc;
				--چوڑائی;
			}
			اگر (b == 16) {
				printc(برآمد, برآمد_len, 'x' - 'a' + letbase, flags);
				++pc;
				--چوڑائی;
			}
		}
	} ورنہ {
		اگر (i && (flags & PAD_ALTERNATE)) {
			اگر (b == 16)
				*--s = 'x' - 'a' + letbase;
			اگر (b == 16 || b == 8)
				*--s = '0';
		}
		اگر (sign)
			*--s = sign;
	}

	واپس pc + prints(برآمد, برآمد_len, s, چوڑائی, flags);
}

ساکن صحیح print(حرف **برآمد, u32 *برآمد_len, ثابت حرف *format, va_list دلائل)
{
	bool flags_مکمل;
	صحیح چوڑائی, flags, pc = 0;
	حرف type, scr[2], *tout;
	bool use_tbuf = (!برآمد) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	اگر (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		برآمد = &tout;
		برآمد_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	اگر (برآمد) {
		اگر(!برآمد_len || *برآمد_len)
			**برآمد = '\0';
	}

	برائے (; *format != 0; ++format) {
		چوڑائی = flags = 0;
		اگر (use_tbuf)
			flags |= USE_TBUF;
		اگر (*format == '%') {
			++format;
			اگر (*format == '\0')
				توڑو;
			اگر (*format == '%')
				جاؤ literal;
			/* Get flags */
			flags_مکمل = false;
			جب (!flags_مکمل) {
				انتخاب (*format) {
				حالت '-':
					flags |= PAD_دایاں;
					توڑو;
				حالت '+':
					flags |= PAD_SIGN;
					توڑو;
				حالت '#':
					flags |= PAD_ALTERNATE;
					توڑو;
				حالت '0':
					flags |= PAD_ZERO;
					توڑو;
				حالت ' ':
				حالت '\'':
					/* Ignored flags, do nothing */
					توڑو;
				طےشدہ:
					flags_مکمل = true;
					توڑو;
				}
				اگر (!flags_مکمل)
					++format;
			}
			اگر (flags & PAD_دایاں)
				flags &= ~PAD_ZERO;
			/* Get width */
			برائے (; *format >= '0' && *format <= '9'; ++format) {
				چوڑائی *= 10;
				چوڑائی += *format - '0';
			}
			اگر (*format == 's') {
				حرف *s = va_arg(دلائل, حرف *);
				pc += prints(برآمد, برآمد_len, s ? s : "(null)",
					     چوڑائی, flags);
				جاری;
			}
			اگر ((*format == 'd') || (*format == 'i')) {
				pc += printi(برآمد, برآمد_len, va_arg(دلائل, صحیح),
					     چوڑائی, flags, *format);
				جاری;
			}
			اگر ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(برآمد, برآمد_len, va_arg(دلائل, غیرعلامتی صحیح),
					     چوڑائی, flags, *format);
				جاری;
			}
			اگر ((*format == 'p') || (*format == 'P')) {
				pc += printi(برآمد, برآمد_len, (uintptr_t)va_arg(دلائل, خالی*),
					     چوڑائی, flags, *format);
				جاری;
			}
			اگر (*format == 'l') {
				type = 'i';
				اگر (format[1] == 'l') {
					++format;
					اگر ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(برآمد, برآمد_len, va_arg(دلائل, طویل طویل),
						چوڑائی, flags, type);
					جاری;
				}
				اگر ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				اگر ((type == 'd') || (type == 'i'))
					pc += printi(برآمد, برآمد_len, va_arg(دلائل, طویل),
					     چوڑائی, flags, type);
				ورنہ
					pc += printi(برآمد, برآمد_len, va_arg(دلائل, غیرعلامتی طویل),
					     چوڑائی, flags, type);
				جاری;
			}
			اگر (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(دلائل, صحیح);
				scr[1] = '\0';
				pc += prints(برآمد, برآمد_len, scr, چوڑائی, flags);
				جاری;
			}
		} ورنہ {
literal:
			printc(برآمد, برآمد_len, *format, flags);
			++pc;
		}
	}

	اگر (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	واپس pc;
}

صحیح sbi_sprintf(حرف *برآمد, ثابت حرف *format, ...)
{
	va_list دلائل;
	صحیح retval;

	اگر (unlikely(!برآمد))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_آغاز(دلائل, format);
	retval = print(&برآمد, تہی, format, دلائل);
	va_انتہا(دلائل);

	واپس retval;
}

صحیح sbi_snprintf(حرف *برآمد, u32 برآمد_sz, ثابت حرف *format, ...)
{
	va_list دلائل;
	صحیح retval;

	اگر (unlikely(!برآمد && برآمد_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_آغاز(دلائل, format);
	retval = print(&برآمد, &برآمد_sz, format, دلائل);
	va_انتہا(دلائل);

	واپس retval;
}

صحیح sbi_printf(ثابت حرف *format, ...)
{
	va_list دلائل;
	صحیح retval;

	spin_تالا(&console_برآمد_تالا);
	va_آغاز(دلائل, format);
	retval = print(تہی, تہی, format, دلائل);
	va_انتہا(دلائل);
	spin_unlock(&console_برآمد_تالا);

	واپس retval;
}

صحیح sbi_dprintf(ثابت حرف *format, ...)
{
	va_list دلائل;
	صحیح retval = 0;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_آغاز(دلائل, format);
	اگر (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_تالا(&console_برآمد_تالا);
		retval = print(تہی, تہی, format, دلائل);
		spin_unlock(&console_برآمد_تالا);
	}
	va_انتہا(دلائل);

	واپس retval;
}

خالی sbi_panic(ثابت حرف *format, ...)
{
	va_list دلائل;

	spin_تالا(&console_برآمد_تالا);
	va_آغاز(دلائل, format);
	print(تہی, تہی, format, دلائل);
	va_انتہا(دلائل);
	spin_unlock(&console_برآمد_تالا);

	sbi_hart_hang();
}

ثابت ساخت sbi_console_device *sbi_console_get_device(خالی)
{
	واپس console_dev;
}

خالی sbi_console_سیٹ_device(ثابت ساخت sbi_console_device *dev)
{
	حرف ch;
	bool flush_early_fifo = false;

	اگر (!dev)
		واپس;

	اگر (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	اگر (flush_early_fifo) {
		جب (!sbi_fifo_قطار_سے_نکالو(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
