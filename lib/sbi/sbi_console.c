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

स्थिर स्थिरांक संरचना sbi_console_device *console_dev = रिक्त;
स्थिर अक्षर console_tbuf[CONSOLE_TBUF_MAX];
स्थिर u32 console_tbuf_len;
स्थिर spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#ifdef CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#define CONSOLE_EARLY_BUFFER_SIZE	CONFIG_CONSOLE_EARLY_BUFFER_SIZE
#अन्यथा
#define CONSOLE_EARLY_BUFFER_SIZE	256
#endif
स्थिर अक्षर console_early_buffer[CONSOLE_EARLY_BUFFER_SIZE] = { 0 };
स्थिर SBI_FIFO_DEFINE(console_early_fifo, console_early_buffer, \
		       CONSOLE_EARLY_BUFFER_SIZE, आकार(अक्षर));

bool sbi_isprintable(अक्षर c)
{
	जर (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		परत true;
	}
	परत false;
}

पूर्णांक sbi_getc(शून्य)
{
	जर (console_dev && console_dev->console_getc)
		परत console_dev->console_getc();
	परत -1;
}

स्थिर अचिन्हित दीर्घ nputs(स्थिरांक अक्षर *str, अचिन्हित दीर्घ len)
{
	अक्षर ch;
	अचिन्हित दीर्घ i;

	जर (console_dev) {
		जर (console_dev->console_puts)
			परत console_dev->console_puts(str, len);
		अन्यथा जर (console_dev->console_putc) {
			साठी (i = 0; i < len; i++) {
				जर (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अन्यथा {
		साठी (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	परत len;
}

स्थिर शून्य nputs_all(स्थिरांक अक्षर *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ p = 0;

	पर्यंत (p < len)
		p += nputs(&str[p], len - p);
}

शून्य sbi_putc(अक्षर ch)
{
	nputs_all(&ch, 1);
}

शून्य sbi_puts(स्थिरांक अक्षर *str)
{
	अचिन्हित दीर्घ len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

अचिन्हित दीर्घ sbi_nputs(स्थिरांक अक्षर *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	परत ret;
}

शून्य sbi_gets(अक्षर *s, पूर्णांक maxwidth, अक्षर endchar)
{
	पूर्णांक ch;
	अक्षर *retval = s;

	पर्यंत ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (अक्षर)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

अचिन्हित दीर्घ sbi_ngets(अक्षर *str, अचिन्हित दीर्घ len)
{
	पूर्णांक ch;
	अचिन्हित दीर्घ i;

	साठी (i = 0; i < len; i++) {
		ch = sbi_getc();
		जर (ch < 0)
			थांबा;
		str[i] = ch;
	}

	परत i;
}

#define PAD_उजवा 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8
#define USE_TBUF 16
#define PRINT_BUF_LEN 64

#define बहु_शुरू(v, l) __builtin_va_start((v), l)
#define बहु_पूर्ण __builtin_va_end
#define बहु_तर्क __builtin_va_arg
प्रकारव्याख्या __builtin_va_list बहु_सूची;

स्थिर शून्य printc(अक्षर **out, u32 *out_len, अक्षर ch, पूर्णांक flags)
{
	जर (!out) {
		sbi_putc(ch);
		परत;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	जर (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		जर (out_len) {
			--(*out_len);
			जर ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

स्थिर पूर्णांक prints(अक्षर **out, u32 *out_len, स्थिरांक अक्षर *string, पूर्णांक width,
		  पूर्णांक flags)
{
	पूर्णांक pc = 0;
	width -= sbi_strlen(string);
	जर (!(flags & PAD_उजवा)) {
		साठी (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	साठी (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	साठी (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	परत pc;
}

स्थिर पूर्णांक printi(अक्षर **out, u32 *out_len, दीर्घ दीर्घ i,
		  पूर्णांक width, पूर्णांक flags, पूर्णांक type)
{
	पूर्णांक pc = 0;
	अक्षर *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	अचिन्हित दीर्घ दीर्घ u, b, t;

	b = 10;
	letbase = 'a';
	जर (type == 'o')
		b = 8;
	अन्यथा जर (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	जर (type == 'i' || type == 'd') {
		जर ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		जर (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	जर (!u) {
		*--s = '0';
	} अन्यथा {
		पर्यंत (u) {
			t = u % b;
			u = u / b;
			जर (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	जर (flags & PAD_ZERO) {
		जर (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		जर (i && (flags & PAD_ALTERNATE)) {
			जर (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			जर (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} अन्यथा {
		जर (i && (flags & PAD_ALTERNATE)) {
			जर (b == 16)
				*--s = 'x' - 'a' + letbase;
			जर (b == 16 || b == 8)
				*--s = '0';
		}
		जर (sign)
			*--s = sign;
	}

	परत pc + prints(out, out_len, s, width, flags);
}

स्थिर पूर्णांक print(अक्षर **out, u32 *out_len, स्थिरांक अक्षर *format, बहु_सूची args)
{
	bool flags_done;
	पूर्णांक width, flags, pc = 0;
	अक्षर type, scr[2], *tout;
	bool use_tbuf = (!out) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	जर (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	जर (out) {
		जर(!out_len || *out_len)
			**out = '\0';
	}

	साठी (; *format != 0; ++format) {
		width = flags = 0;
		जर (use_tbuf)
			flags |= USE_TBUF;
		जर (*format == '%') {
			++format;
			जर (*format == '\0')
				थांबा;
			जर (*format == '%')
				जा literal;
			/* Get flags */
			flags_done = false;
			पर्यंत (!flags_done) {
				निवड (*format) {
				स्थिती '-':
					flags |= PAD_उजवा;
					थांबा;
				स्थिती '+':
					flags |= PAD_SIGN;
					थांबा;
				स्थिती '#':
					flags |= PAD_ALTERNATE;
					थांबा;
				स्थिती '0':
					flags |= PAD_ZERO;
					थांबा;
				स्थिती ' ':
				स्थिती '\'':
					/* Ignored flags, do nothing */
					थांबा;
				पूर्वनिर्धारित:
					flags_done = true;
					थांबा;
				}
				जर (!flags_done)
					++format;
			}
			जर (flags & PAD_उजवा)
				flags &= ~PAD_ZERO;
			/* Get width */
			साठी (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			जर (*format == 's') {
				अक्षर *s = बहु_तर्क(args, अक्षर *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				सुरूठेवा;
			}
			जर ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, बहु_तर्क(args, पूर्णांक),
					     width, flags, *format);
				सुरूठेवा;
			}
			जर ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित पूर्णांक),
					     width, flags, *format);
				सुरूठेवा;
			}
			जर ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)बहु_तर्क(args, शून्य*),
					     width, flags, *format);
				सुरूठेवा;
			}
			जर (*format == 'l') {
				type = 'i';
				जर (format[1] == 'l') {
					++format;
					जर ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ दीर्घ),
						width, flags, type);
					सुरूठेवा;
				}
				जर ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				जर ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ),
					     width, flags, type);
				अन्यथा
					pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित दीर्घ),
					     width, flags, type);
				सुरूठेवा;
			}
			जर (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = बहु_तर्क(args, पूर्णांक);
				scr[1] = '\0';
				pc += prints(out, out_len, scr, width, flags);
				सुरूठेवा;
			}
		} अन्यथा {
literal:
			printc(out, out_len, *format, flags);
			++pc;
		}
	}

	जर (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	परत pc;
}

पूर्णांक sbi_sprintf(अक्षर *out, स्थिरांक अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	जर (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	बहु_शुरू(args, format);
	retval = print(&out, रिक्त, format, args);
	बहु_पूर्ण(args);

	परत retval;
}

पूर्णांक sbi_snprintf(अक्षर *out, u32 out_sz, स्थिरांक अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	जर (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	बहु_शुरू(args, format);
	retval = print(&out, &out_sz, format, args);
	बहु_पूर्ण(args);

	परत retval;
}

पूर्णांक sbi_printf(स्थिरांक अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	retval = print(रिक्त, रिक्त, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	परत retval;
}

पूर्णांक sbi_dprintf(स्थिरांक अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	बहु_शुरू(args, format);
	जर (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(रिक्त, रिक्त, format, args);
		spin_unlock(&console_out_lock);
	}
	बहु_पूर्ण(args);

	परत retval;
}

शून्य sbi_panic(स्थिरांक अक्षर *format, ...)
{
	बहु_सूची args;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	print(रिक्त, रिक्त, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

स्थिरांक संरचना sbi_console_device *sbi_console_get_device(शून्य)
{
	परत console_dev;
}

शून्य sbi_console_set_device(स्थिरांक संरचना sbi_console_device *dev)
{
	अक्षर ch;
	bool flush_early_fifo = false;

	जर (!dev)
		परत;

	जर (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	जर (flush_early_fifo) {
		पर्यंत (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
