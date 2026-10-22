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
#अञ्ञथा
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
स्थिर वर्ण console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
स्थिर SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, परिमाण(वर्ण));

bool sbi_isprintable(वर्ण c)
{
	सचे (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		पटिनिवत्त true;
	}
	पटिनिवत्त false;
}

पूर्णाङ्क sbi_getc(शून्य)
{
	सचे (console_dev && console_dev->console_getc)
		पटिनिवत्त console_dev->console_getc();
	पटिनिवत्त -1;
}

स्थिर unsigned long nputs(ध्रुव वर्ण *str, unsigned long len)
{
	वर्ण ch;
	unsigned long i;

	सचे (console_dev) {
		सचे (console_dev->console_puts)
			पटिनिवत्त console_dev->console_puts(str, len);
		अञ्ञथा सचे (console_dev->console_putc) {
			अत्थाय (i = 0; i < len; i++) {
				सचे (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अञ्ञथा {
		अत्थाय (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	पटिनिवत्त len;
}

स्थिर शून्य nputs_all(ध्रुव वर्ण *str, unsigned long len)
{
	unsigned long p = 0;

	याव (p < len)
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

	पटिनिवत्त ret;
}

शून्य sbi_gets(वर्ण *s, पूर्णाङ्क maxwidth, वर्ण endchar)
{
	पूर्णाङ्क ch;
	वर्ण *retval = s;

	याव ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
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

	अत्थाय (i = 0; i < len; i++) {
		ch = sbi_getc();
		सचे (ch < 0)
			भङ्ग;
		str[i] = ch;
	}

	पटिनिवत्त i;
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
	सचे (!out) {
		sbi_putc(ch);
		पटिनिवत्त;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	सचे (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		सचे (out_len) {
			--(*out_len);
			सचे ((flags & USE_TBUF) && *out_len == 1) {
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
	सचे (!(flags & PAD_दक्षिणः)) {
		अत्थाय (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	अत्थाय (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	अत्थाय (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	पटिनिवत्त pc;
}

स्थिर पूर्णाङ्क printi(वर्ण **out, u32 *out_len, long long i,
		  पूर्णाङ्क width, पूर्णाङ्क flags, पूर्णाङ्क type)
{
	पूर्णाङ्क pc = 0;
	वर्ण *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	सचे (type == 'o')
		b = 8;
	अञ्ञथा सचे (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	सचे (type == 'i' || type == 'd') {
		सचे ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		सचे (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	सचे (!u) {
		*--s = '0';
	} अञ्ञथा {
		याव (u) {
			t = u % b;
			u = u / b;
			सचे (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	सचे (flags & PAD_ZERO) {
		सचे (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		सचे (i && (flags & PAD_ALTERNATE)) {
			सचे (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			सचे (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} अञ्ञथा {
		सचे (i && (flags & PAD_ALTERNATE)) {
			सचे (b == 16)
				*--s = 'x' - 'a' + letbase;
			सचे (b == 16 || b == 8)
				*--s = '0';
		}
		सचे (sign)
			*--s = sign;
	}

	पटिनिवत्त pc + prints(out, out_len, s, width, flags);
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
	सचे (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	सचे (out) {
		सचे(!out_len || *out_len)
			**out = '\0';
	}

	अत्थाय (; *format != 0; ++format) {
		width = flags = 0;
		सचे (use_tbuf)
			flags |= USE_TBUF;
		सचे (*format == '%') {
			++format;
			सचे (*format == '\0')
				भङ्ग;
			सचे (*format == '%')
				गच्छ literal;
			/* Get flags */
			flags_done = false;
			याव (!flags_done) {
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
				सचे (!flags_done)
					++format;
			}
			सचे (flags & PAD_दक्षिणः)
				flags &= ~PAD_ZERO;
			/* Get width */
			अत्थाय (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			सचे (*format == 's') {
				वर्ण *s = va_arg(args, वर्ण *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				अनुवर्त;
			}
			सचे ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, va_arg(args, पूर्णाङ्क),
					     width, flags, *format);
				अनुवर्त;
			}
			सचे ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, va_arg(args, unsigned पूर्णाङ्क),
					     width, flags, *format);
				अनुवर्त;
			}
			सचे ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)va_arg(args, शून्य*),
					     width, flags, *format);
				अनुवर्त;
			}
			सचे (*format == 'l') {
				type = 'i';
				सचे (format[1] == 'l') {
					++format;
					सचे ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, va_arg(args, long long),
						width, flags, type);
					अनुवर्त;
				}
				सचे ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				सचे ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, va_arg(args, long),
					     width, flags, type);
				अञ्ञथा
					pc += printi(out, out_len, va_arg(args, unsigned long),
					     width, flags, type);
				अनुवर्त;
			}
			सचे (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, पूर्णाङ्क);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				अनुवर्त;
			}
		} अञ्ञथा {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	सचे (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	पटिनिवत्त pc;
}

पूर्णाङ्क sbi_sprintf(वर्ण *out, ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval;

	सचे (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(args, format);
	retval = print(&out, रिक्त, format, args);
	va_end(args);

	पटिनिवत्त retval;
}

पूर्णाङ्क sbi_snprintf(वर्ण *out, u32 out_sz, ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval;

	सचे (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(args, format);
	retval = print(&out, &out_sz, format, args);
	va_end(args);

	पटिनिवत्त retval;
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

	पटिनिवत्त retval;
}

पूर्णाङ्क sbi_dprintf(ध्रुव वर्ण *format, ...)
{
	va_list args;
	पूर्णाङ्क retval = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(args, format);
	सचे (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(रिक्त, रिक्त, format, args);
		spin_unlock(&console_out_lock);
	}
	va_end(args);

	पटिनिवत्त retval;
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
	पटिनिवत्त console_dev;
}

शून्य sbi_console_set_device(ध्रुव संरचना sbi_console_device *dev)
{
	वर्ण ch;
	bool flush_early_fifo = false;

	सचे (!dev)
		पटिनिवत्त;

	सचे (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	सचे (flush_early_fifo) {
		याव (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
