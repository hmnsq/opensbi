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

स्थिर अचर संरचना sbi_console_device *console_dev = रिक्त;
स्थिर वर्ण console_tbuf[CONSOLE_TBUF_MAX];
स्थिर u32 console_tbuf_len;
स्थिर spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#अन्यथा
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
स्थिर वर्ण console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
स्थिर SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, परिमाण(वर्ण));

bool sbi_isprintable(वर्ण c)
{
	यदि (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		फर्काऊ true;
	}
	फर्काऊ false;
}

पूर्णाङ्क sbi_getc(शून्य)
{
	यदि (console_dev && console_dev->console_getc)
		फर्काऊ console_dev->console_getc();
	फर्काऊ -1;
}

स्थिर अचिन्हित दीर्घ nputs(अचर वर्ण *str, अचिन्हित दीर्घ len)
{
	वर्ण ch;
	अचिन्हित दीर्घ i;

	यदि (console_dev) {
		यदि (console_dev->console_puts)
			फर्काऊ console_dev->console_puts(str, len);
		अन्यथा यदि (console_dev->console_putc) {
			लागि (i = 0; i < len; i++) {
				यदि (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अन्यथा {
		लागि (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	फर्काऊ len;
}

स्थिर शून्य nputs_all(अचर वर्ण *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ p = 0;

	जबसम्म (p < len)
		p += nputs(&str[p], len - p);
}

शून्य sbi_putc(वर्ण ch)
{
	nputs_all(&ch, 1);
}

शून्य sbi_puts(अचर वर्ण *str)
{
	अचिन्हित दीर्घ len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

अचिन्हित दीर्घ sbi_nputs(अचर वर्ण *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	फर्काऊ ret;
}

शून्य sbi_gets(वर्ण *s, पूर्णाङ्क maxwidth, वर्ण endchar)
{
	पूर्णाङ्क ch;
	वर्ण *retval = s;

	जबसम्म ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (वर्ण)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

अचिन्हित दीर्घ sbi_ngets(वर्ण *str, अचिन्हित दीर्घ len)
{
	पूर्णाङ्क ch;
	अचिन्हित दीर्घ i;

	लागि (i = 0; i < len; i++) {
		ch = sbi_getc();
		यदि (ch < 0)
			रोक;
		str[i] = ch;
	}

	फर्काऊ i;
}

#define PAD_दायाँ 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define बहु_शुरू(v, l) __builtin_va_start((v), l)
#define बहु_पूर्ण __builtin_va_end
#define बहु_तर्क __builtin_va_arg
प्रकारनिर्देश __builtin_va_list बहु_सूची;

स्थिर शून्य printc(वर्ण **out, u32 *out_len, वर्ण ch, पूर्णाङ्क flags)
{
	यदि (!out) {
		sbi_putc(ch);
		फर्काऊ;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	यदि (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		यदि (out_len) {
			--(*out_len);
			यदि ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

स्थिर पूर्णाङ्क prints(वर्ण **out, u32 *out_len, अचर वर्ण *string, पूर्णाङ्क width,
		  पूर्णाङ्क flags)
{
	पूर्णाङ्क pc = 0;
	width -= sbi_strlen(string);
	यदि (!(flags & PAD_दायाँ)) {
		लागि (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	लागि (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	लागि (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	फर्काऊ pc;
}

स्थिर पूर्णाङ्क printi(वर्ण **out, u32 *out_len, दीर्घ दीर्घ i,
		  पूर्णाङ्क width, पूर्णाङ्क flags, पूर्णाङ्क type)
{
	पूर्णाङ्क pc = 0;
	वर्ण *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	अचिन्हित दीर्घ दीर्घ u, b, t;

	b = 10;
	letbase = 'a';
	यदि (type == 'o')
		b = 8;
	अन्यथा यदि (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	यदि (type == 'i' || type == 'd') {
		यदि ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		यदि (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	यदि (!u) {
		*--s = '0';
	} अन्यथा {
		जबसम्म (u) {
			t = u % b;
			u = u / b;
			यदि (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	यदि (flags & PAD_ZERO) {
		यदि (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		यदि (i && (flags & PAD_ALTERNATE)) {
			यदि (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			यदि (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} अन्यथा {
		यदि (i && (flags & PAD_ALTERNATE)) {
			यदि (b == 16)
				*--s = 'x' - 'a' + letbase;
			यदि (b == 16 || b == 8)
				*--s = '0';
		}
		यदि (sign)
			*--s = sign;
	}

	फर्काऊ pc + prints(out, out_len, s, width, flags);
}

स्थिर पूर्णाङ्क print(वर्ण **out, u32 *out_len, अचर वर्ण *format, बहु_सूची args)
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
	यदि (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	यदि (out) {
		यदि(!out_len || *out_len)
			**out = '\0';
	}

	लागि (; *format != 0; ++format) {
		width = flags = 0;
		यदि (use_tbuf)
			flags |= USE_TBUF;
		यदि (*format == '%') {
			++format;
			यदि (*format == '\0')
				रोक;
			यदि (*format == '%')
				गच्छ literal;
			/* Get flags */
			flags_done = false;
			जबसम्म (!flags_done) {
				विकल्प (*format) {
				पक्ष '-':
					flags |= PAD_दायाँ;
					रोक;
				पक्ष '+':
					flags |= PAD_SIGN;
					रोक;
				पक्ष '#':
					flags |= PAD_ALTERNATE;
					रोक;
				पक्ष '0':
					flags |= PAD_ZERO;
					रोक;
				पक्ष ' ':
				पक्ष '\'':
					/* Ignored flags, do nothing */
					रोक;
				स्वभाव:
					flags_done = true;
					रोक;
				}
				यदि (!flags_done)
					++format;
			}
			यदि (flags & PAD_दायाँ)
				flags &= ~PAD_ZERO;
			/* Get width */
			लागि (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			यदि (*format == 's') {
				वर्ण *s = बहु_तर्क(args, वर्ण *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				जारी;
			}
			यदि ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, बहु_तर्क(args, पूर्णाङ्क),
					     width, flags, *format);
				जारी;
			}
			यदि ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित पूर्णाङ्क),
					     width, flags, *format);
				जारी;
			}
			यदि ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)बहु_तर्क(args, शून्य*),
					     width, flags, *format);
				जारी;
			}
			यदि (*format == 'l') {
				type = 'i';
				यदि (format[1] == 'l') {
					++format;
					यदि ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ दीर्घ),
						width, flags, type);
					जारी;
				}
				यदि ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				यदि ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ),
					     width, flags, type);
				अन्यथा
					pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित दीर्घ),
					     width, flags, type);
				जारी;
			}
			यदि (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = बहु_तर्क(args, पूर्णाङ्क);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				जारी;
			}
		} अन्यथा {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	यदि (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	फर्काऊ pc;
}

पूर्णाङ्क sbi_sprintf(वर्ण *out, अचर वर्ण *format, ...)
{
	बहु_सूची args;
	पूर्णाङ्क retval;

	यदि (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	बहु_शुरू(args, format);
	retval = print(&out, रिक्त, format, args);
	बहु_पूर्ण(args);

	फर्काऊ retval;
}

पूर्णाङ्क sbi_snprintf(वर्ण *out, u32 out_sz, अचर वर्ण *format, ...)
{
	बहु_सूची args;
	पूर्णाङ्क retval;

	यदि (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	बहु_शुरू(args, format);
	retval = print(&out, &out_sz, format, args);
	बहु_पूर्ण(args);

	फर्काऊ retval;
}

पूर्णाङ्क sbi_printf(अचर वर्ण *format, ...)
{
	बहु_सूची args;
	पूर्णाङ्क retval;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	retval = print(रिक्त, रिक्त, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	फर्काऊ retval;
}

पूर्णाङ्क sbi_dprintf(अचर वर्ण *format, ...)
{
	बहु_सूची args;
	पूर्णाङ्क retval = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	बहु_शुरू(args, format);
	यदि (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(रिक्त, रिक्त, format, args);
		spin_unlock(&console_out_lock);
	}
	बहु_पूर्ण(args);

	फर्काऊ retval;
}

शून्य sbi_panic(अचर वर्ण *format, ...)
{
	बहु_सूची args;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	print(रिक्त, रिक्त, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

अचर संरचना sbi_console_device *sbi_console_get_device(शून्य)
{
	फर्काऊ console_dev;
}

शून्य sbi_console_set_device(अचर संरचना sbi_console_device *dev)
{
	वर्ण ch;
	bool flush_early_fifo = false;

	यदि (!dev)
		फर्काऊ;

	यदि (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	यदि (flush_early_fifo) {
		जबसम्म (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
