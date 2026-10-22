/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_fifo.h>
#अन्तर्भाव <sbi/sbi_hart.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

#define CONSOLE_TBUF_MAX 256

स्थिर ध्रुव संरचना sbi_console_device *console_dev = रिक्त;
स्थिर वर्ण console_tbuf[CONSOLE_TBUF_MAX];
स्थिर u32 console_tbuf_len;
स्थिर spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#अण्णहा
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
स्थिर वर्ण console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
स्थिर SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, परिमाण(वर्ण));

bool sbi_isprintable(वर्ण c)
{
	जइ (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		प्रत्यागम true;
	}
	प्रत्यागम false;
}

पूर्णाङ्क sbi_getc(शून्य)
{
	जइ (console_dev && console_dev->console_getc)
		प्रत्यागम console_dev->console_getc();
	प्रत्यागम -1;
}

स्थिर unsigned long nputs(ध्रुव वर्ण *str, unsigned long len)
{
	वर्ण ch;
	unsigned long i;

	जइ (console_dev) {
		जइ (console_dev->console_puts)
			प्रत्यागम console_dev->console_puts(str, len);
		अण्णहा जइ (console_dev->console_putc) {
			प्रति (i = 0; i < len; i++) {
				जइ (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अण्णहा {
		प्रति (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	प्रत्यागम len;
}

स्थिर शून्य nputs_all(ध्रुव वर्ण *str, unsigned long len)
{
	unsigned long p = 0;

	जाव (p < len)
		p += nputs(&str[p], len - p);
}

शून्य sbi_putc(वर्ण ch)
{
	nputs_all(&ch, 1);
}

शून्य sbi_puts(ध्रुव वर्ण *str)
{
	unsigned long len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

unsigned long sbi_nputs(ध्रुव वर्ण *str, unsigned long len)
{
	unsigned long ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	प्रत्यागम ret;
}

शून्य sbi_gets(वर्ण *s, पूर्णाङ्क maxwidth, वर्ण endchar)
{
	पूर्णाङ्क ch;
	वर्ण *retval = s;

	जाव ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (वर्ण)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

unsigned long sbi_ngets(वर्ण *str, unsigned long len)
{
	पूर्णाङ्क ch;
	unsigned long i;

	प्रति (i = 0; i < len; i++) {
		ch = sbi_getc();
		जइ (ch < 0)
			भङ्ग;
		str[i] = ch;
	}

	प्रत्यागम i;
}

#define PAD_दक्षिणः 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
प्रकारनिर्देश __builtin_va_list va_list;

स्थिर शून्य printc(वर्ण **out, u32 *out_len, वर्ण ch, पूर्णाङ्क flags)
{
	जइ (!out) {
		sbi_putc(ch);
		प्रत्यागम;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	जइ (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		जइ (out_len) {
			--(*out_len);
			जइ ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

स्थिर पूर्णाङ्क prints(वर्ण **out, u32 *out_len, ध्रुव वर्ण *string, पूर्णाङ्क width,
		  पूर्णाङ्क flags)
{
	पूर्णाङ्क pc = 0;
	width -= sbi_strlen(string);
	जइ (!(flags & PAD_दक्षिणः)) {
		प्रति (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	प्रति (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	प्रति (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	प्रत्यागम pc;
}

स्थिर पूर्णाङ्क printi(वर्ण **out, u32 *out_len, long long i,
		  पूर्णाङ्क width, पूर्णाङ्क flags, पूर्णाङ्क type)
{
	पूर्णाङ्क pc = 0;
	वर्ण *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	जइ (type == 'o')
		b = 8;
	अण्णहा जइ (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	जइ (type == 'i' || type == 'd') {
		जइ ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		जइ (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	जइ (!u) {
		*--s = '0';
	} अण्णहा {
		जाव (u) {
			t = u % b;
			u = u / b;
			जइ (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	जइ (flags & PAD_ZERO) {
		जइ (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		जइ (i && (flags & PAD_ALTERNATE)) {
			जइ (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			जइ (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} अण्णहा {
		जइ (i && (flags & PAD_ALTERNATE)) {
			जइ (b == 16)
				*--s = 'x' - 'a' + letbase;
			जइ (b == 16 || b == 8)
				*--s = '0';
		}
		जइ (sign)
			*--s = sign;
	}

	प्रत्यागम pc + prints(out, out_len, s, width, flags);
}

स्थिर पूर्णाङ्क print(वर्ण **out, u32 *out_len, ध्रुव वर्ण *format, va_list args)
{
	bool flags_done;
	पूर्णाङ्क width, flags, pc = 0;
	वर्ण type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	जइ (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	जइ (out) {
		जइ(!out_len || *out_len)
			**out = '\0';
	}

	प्रति (; *format != 0; ++format) {
		width = flags = 0;
		जइ (use_tbuf)
			flags |= USE_TBUF;
		जइ (*format == '%') {
			++format;
			जइ (*format == '\0')
				भङ्ग;
			जइ (*format == '%')
				गच्छ literal;
			/* Get flags */
			flags_done = false;
			जाव (!flags_done) {
				विकल्प (*format) {
				पक्ष '-':
					flags |= PAD_दक्षिणः;
					भङ्ग;
				पक्ष '+':
					flags |= PAD_SIGN;
					भङ्ग;
				पक्ष '#':
					flags |= PAD_ALTERNATE;
					भङ्ग;
				पक्ष '0':
					flags |= PAD_ZERO;
					भङ्ग;
				पक्ष ' ':
				पक्ष '\'':
					/* Ignored flags, do nothing */
					भङ्ग;
				स्वभाव:
					flags_done = true;
					भङ्ग;
				}
				जइ (!flags_done)
					++format;
			}
			जइ (flags & PAD_दक्षिणः)
				flags &= ~PAD_ZERO;
			/* Get width */
			प्रति (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			जइ (*format == 's') {
				वर्ण *s = va_arg(args, वर्ण *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				अनुवर्त;
			}
			जइ ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, पूर्णाङ्क),
					     width, flags, *format);
				अनुवर्त;
			}
			जइ ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned पूर्णाङ्क),
					     width, flags, *format);
				अनुवर्त;
			}
			जइ ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, शून्य*),
					     width, flags, *format);
				अनुवर्त;
			}
			जइ (*format == 'l') {
				type = 'i';
				जइ (format[1] == 'l') {
					++format;
					जइ ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					अनुवर्त;
				}
				जइ ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				जइ ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				अण्णहा
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				अनुवर्त;
			}
			जइ (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, पूर्णाङ्क);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				अनुवर्त;
			}
		} अण्णहा {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	जइ (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	प्रत्यागम pc;
}

पूर्णाङ्क sbi_sprintf(वर्ण *out, ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval;

	जइ (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, रिक्त, format, args);
	va_end(args);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_snprintf(वर्ण *out, u32 out_sz, ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval;

	जइ (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_printf(ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval;

	spin_lock(&console_out_lock);
	va_start(args, format);
	retval = print(रिक्त, रिक्त, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_dprintf(ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	जइ (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(रिक्त, रिक्त, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	प्रत्यागम retval;
}

शून्य sbi_panic(ध्रुव वर्ण *format, ...)
{
	va_list args;

	spin_lock(&console_out_lock);
	va_start(args, format);
	print(रिक्त, रिक्त, format, args);
	va_end(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

ध्रुव संरचना sbi_console_device *sbi_console_get_device(शून्य)
{
	प्रत्यागम console_dev;
}

शून्य sbi_console_set_device(ध्रुव संरचना sbi_console_device *dev)
{
	वर्ण ch;
	bool flush_early_fifo = false;

	जइ (!dev)
		प्रत्यागम;

	जइ (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	जइ (flush_early_fifo) {
		जाव (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
