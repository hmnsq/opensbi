/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_fifo.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>

#घोषणा CONSOLE_TBUF_MAX 256

अटल स्थिर काष्ठा sbi_console_device *console_dev = शून्य;
अटल अक्षर console_tbuf[CONSOLE_TBUF_MAX];
अटल u32 console_tbuf_len;
अटल spinlock_t console_out_lock	       = SPIN_LOCK_INITIALIZER;

#अगर_घोषित CONFIG_CONSOLE_EARLY_बफर_आकार
#घोषणा CONSOLE_EARLY_बफर_आकार	CONFIG_CONSOLE_EARLY_बफर_आकार
#अन्यथा
#घोषणा CONSOLE_EARLY_बफर_आकार	256
#पूर्ण_अगर
अटल अक्षर console_early_बफर[CONSOLE_EARLY_बफर_आकार] = { 0 };
अटल SBI_FIFO_DEFINE(console_early_fifo, console_early_बफर, \
		       CONSOLE_EARLY_बफर_आकार, माप(अक्षर));

bool sbi_isprintable(अक्षर c)
{
	अगर (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		वापस true;
	}
	वापस false;
}

पूर्णांक sbi_getc(व्योम)
{
	अगर (console_dev && console_dev->console_getc)
		वापस console_dev->console_getc();
	वापस -1;
}

अटल अचिन्हित दीर्घ nputs(स्थिर अक्षर *str, अचिन्हित दीर्घ len)
{
	अक्षर ch;
	अचिन्हित दीर्घ i;

	अगर (console_dev) {
		अगर (console_dev->console_puts)
			वापस console_dev->console_puts(str, len);
		अन्यथा अगर (console_dev->console_putc) {
			क्रम (i = 0; i < len; i++) {
				अगर (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अन्यथा {
		क्रम (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_enqueue(&console_early_fifo, &ch, true);
		}
	}
	वापस len;
}

अटल व्योम nputs_all(स्थिर अक्षर *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ p = 0;

	जबतक (p < len)
		p += nputs(&str[p], len - p);
}

व्योम sbi_putc(अक्षर ch)
{
	nputs_all(&ch, 1);
}

व्योम sbi_puts(स्थिर अक्षर *str)
{
	अचिन्हित दीर्घ len = sbi_strlen(str);

	spin_lock(&console_out_lock);
	nputs_all(str, len);
	spin_unlock(&console_out_lock);
}

अचिन्हित दीर्घ sbi_nputs(स्थिर अक्षर *str, अचिन्हित दीर्घ len)
{
	अचिन्हित दीर्घ ret;

	spin_lock(&console_out_lock);
	ret = nputs(str, len);
	spin_unlock(&console_out_lock);

	वापस ret;
}

व्योम sbi_gets(अक्षर *s, पूर्णांक maxwidth, अक्षर endchar)
{
	पूर्णांक ch;
	अक्षर *retval = s;

	जबतक ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
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

	क्रम (i = 0; i < len; i++) {
		ch = sbi_getc();
		अगर (ch < 0)
			अवरोध;
		str[i] = ch;
	}

	वापस i;
}

#घोषणा PAD_दायाँ 1
#घोषणा PAD_ZERO 2
#घोषणा PAD_ALTERNATE 4
#घोषणा PAD_SIGN 8
#घोषणा USE_TBUF 16
#घोषणा PRINT_BUF_LEN 64

#घोषणा बहु_शुरू(v, l) __builtin_va_start((v), l)
#घोषणा बहु_पूर्ण __builtin_va_end
#घोषणा बहु_तर्क __builtin_va_arg
प्रकार __builtin_va_list बहु_सूची;

अटल व्योम printc(अक्षर **out, u32 *out_len, अक्षर ch, पूर्णांक flags)
{
	अगर (!out) {
		sbi_putc(ch);
		वापस;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	अगर (!out_len || *out_len > 1) {
		*(*out)++ = ch;
		**out = '\0';
		अगर (out_len) {
			--(*out_len);
			अगर ((flags & USE_TBUF) && *out_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *out_len);
				*out = console_tbuf;
				*out_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

अटल पूर्णांक prints(अक्षर **out, u32 *out_len, स्थिर अक्षर *string, पूर्णांक width,
		  पूर्णांक flags)
{
	पूर्णांक pc = 0;
	width -= sbi_strlen(string);
	अगर (!(flags & PAD_दायाँ)) {
		क्रम (; width > 0; --width) {
			printc(out, out_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	क्रम (; *string; ++string) {
		printc(out, out_len, *string, flags);
		++pc;
	}
	क्रम (; width > 0; --width) {
		printc(out, out_len, ' ', flags);
		++pc;
	}

	वापस pc;
}

अटल पूर्णांक printi(अक्षर **out, u32 *out_len, दीर्घ दीर्घ i,
		  पूर्णांक width, पूर्णांक flags, पूर्णांक type)
{
	पूर्णांक pc = 0;
	अक्षर *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	अचिन्हित दीर्घ दीर्घ u, b, t;

	b = 10;
	letbase = 'a';
	अगर (type == 'o')
		b = 8;
	अन्यथा अगर (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	अगर (type == 'i' || type == 'd') {
		अगर ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		अगर (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	अगर (!u) {
		*--s = '0';
	} अन्यथा {
		जबतक (u) {
			t = u % b;
			u = u / b;
			अगर (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	अगर (flags & PAD_ZERO) {
		अगर (sign) {
			printc(out, out_len, sign, flags);
			++pc;
			--width;
		}
		अगर (i && (flags & PAD_ALTERNATE)) {
			अगर (b == 16 || b == 8) {
				printc(out, out_len, '0', flags);
				++pc;
				--width;
			}
			अगर (b == 16) {
				printc(out, out_len, 'x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} अन्यथा {
		अगर (i && (flags & PAD_ALTERNATE)) {
			अगर (b == 16)
				*--s = 'x' - 'a' + letbase;
			अगर (b == 16 || b == 8)
				*--s = '0';
		}
		अगर (sign)
			*--s = sign;
	}

	वापस pc + prints(out, out_len, s, width, flags);
}

अटल पूर्णांक print(अक्षर **out, u32 *out_len, स्थिर अक्षर *format, बहु_सूची args)
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
	अगर (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		out = &tout;
		out_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	अगर (out) {
		अगर(!out_len || *out_len)
			**out = '\0';
	}

	क्रम (; *format != 0; ++format) {
		width = flags = 0;
		अगर (use_tbuf)
			flags |= USE_TBUF;
		अगर (*format == '%') {
			++format;
			अगर (*format == '\0')
				अवरोध;
			अगर (*format == '%')
				जाओ literal;
			/* Get flags */
			flags_done = false;
			जबतक (!flags_done) {
				चयन (*format) {
				हाल '-':
					flags |= PAD_दायाँ;
					अवरोध;
				हाल '+':
					flags |= PAD_SIGN;
					अवरोध;
				हाल '#':
					flags |= PAD_ALTERNATE;
					अवरोध;
				हाल '0':
					flags |= PAD_ZERO;
					अवरोध;
				हाल ' ':
				हाल '\'':
					/* Ignored flags, do nothing */
					अवरोध;
				शेष:
					flags_done = true;
					अवरोध;
				}
				अगर (!flags_done)
					++format;
			}
			अगर (flags & PAD_दायाँ)
				flags &= ~PAD_ZERO;
			/* Get width */
			क्रम (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			अगर (*format == 's') {
				अक्षर *s = बहु_तर्क(args, अक्षर *);
				pc += prints(out, out_len, s ? s : "(null)",
					     width, flags);
				जारी;
			}
			अगर ((*format == 'd') || (*format == 'i')) {
				pc += printi(out, out_len, बहु_तर्क(args, पूर्णांक),
					     width, flags, *format);
				जारी;
			}
			अगर ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित पूर्णांक),
					     width, flags, *format);
				जारी;
			}
			अगर ((*format == 'p') || (*format == 'P')) {
				pc += printi(out, out_len, (uintptr_t)बहु_तर्क(args, व्योम*),
					     width, flags, *format);
				जारी;
			}
			अगर (*format == 'l') {
				type = 'i';
				अगर (format[1] == 'l') {
					++format;
					अगर ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ दीर्घ),
						width, flags, type);
					जारी;
				}
				अगर ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				अगर ((type == 'd') || (type == 'i'))
					pc += printi(out, out_len, बहु_तर्क(args, दीर्घ),
					     width, flags, type);
				अन्यथा
					pc += printi(out, out_len, बहु_तर्क(args, अचिन्हित दीर्घ),
					     width, flags, type);
				जारी;
			}
			अगर (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = बहु_तर्क(args, पूर्णांक);
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

	अगर (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	वापस pc;
}

पूर्णांक sbi_sprintf(अक्षर *out, स्थिर अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	अगर (unlikely(!out))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	बहु_शुरू(args, format);
	retval = print(&out, शून्य, format, args);
	बहु_पूर्ण(args);

	वापस retval;
}

पूर्णांक sbi_snprintf(अक्षर *out, u32 out_sz, स्थिर अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	अगर (unlikely(!out && out_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	बहु_शुरू(args, format);
	retval = print(&out, &out_sz, format, args);
	बहु_पूर्ण(args);

	वापस retval;
}

पूर्णांक sbi_printf(स्थिर अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	retval = print(शून्य, शून्य, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	वापस retval;
}

पूर्णांक sbi_dprintf(स्थिर अक्षर *format, ...)
{
	बहु_सूची args;
	पूर्णांक retval = 0;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	बहु_शुरू(args, format);
	अगर (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_lock(&console_out_lock);
		retval = print(शून्य, शून्य, format, args);
		spin_unlock(&console_out_lock);
	}
	बहु_पूर्ण(args);

	वापस retval;
}

व्योम sbi_panic(स्थिर अक्षर *format, ...)
{
	बहु_सूची args;

	spin_lock(&console_out_lock);
	बहु_शुरू(args, format);
	print(शून्य, शून्य, format, args);
	बहु_पूर्ण(args);
	spin_unlock(&console_out_lock);

	sbi_hart_hang();
}

स्थिर काष्ठा sbi_console_device *sbi_console_get_device(व्योम)
{
	वापस console_dev;
}

व्योम sbi_console_set_device(स्थिर काष्ठा sbi_console_device *dev)
{
	अक्षर ch;
	bool flush_early_fifo = false;

	अगर (!dev)
		वापस;

	अगर (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	अगर (flush_early_fifo) {
		जबतक (!sbi_fifo_dequeue(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
