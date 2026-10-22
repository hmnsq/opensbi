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

#परिभाषय CONSOLE_TBUF_MAX 256

स्थिर ध्रुव संरचना sbi_console_device *console_dev = रिक्त;
स्थिर वर्ण console_tbuf[CONSOLE_TBUF_MAX];
स्थिर u32 console_tbuf_len;
स्थिर spinlock_t console_बहिर्गमनम्_तालकम्	       = SPIN_तालकम्_INITIALIZER;

#यदि_परिभाषितम् CONFIG_CONSOLE_EARLY_बफर_आकार
#परिभाषय CONSOLE_EARLY_बफर_आकार	CONFIG_CONSOLE_EARLY_बफर_आकार
#अन्यथा
#परिभाषय CONSOLE_EARLY_बफर_आकार	256
#यदि_अन्तः
स्थिर वर्ण console_early_बफर[CONSOLE_EARLY_बफर_आकार] = { 0 };
स्थिर SBI_FIFO_DEFINE(console_early_fifo, console_early_बफर, \
		       CONSOLE_EARLY_बफर_आकार, परिमाण(वर्ण));

bool sbi_isprintable(वर्ण c)
{
	यदि (((31 < c) && (c < 127)) || (c == '\f') || (c == '\r') ||
	    (c == '\n') || (c == '\t')) {
		प्रत्यागम true;
	}
	प्रत्यागम false;
}

पूर्णाङ्क sbi_getc(शून्य)
{
	यदि (console_dev && console_dev->console_getc)
		प्रत्यागम console_dev->console_getc();
	प्रत्यागम -1;
}

स्थिर अचिह्नित दीर्घ nputs(ध्रुव वर्ण *str, अचिह्नित दीर्घ len)
{
	वर्ण ch;
	अचिह्नित दीर्घ i;

	यदि (console_dev) {
		यदि (console_dev->console_puts)
			प्रत्यागम console_dev->console_puts(str, len);
		अन्यथा यदि (console_dev->console_putc) {
			प्रति (i = 0; i < len; i++) {
				यदि (str[i] == '\n')
					console_dev->console_putc('\r');
				console_dev->console_putc(str[i]);
			}
		}
	} अन्यथा {
		प्रति (i = 0; i < len; i++) {
			ch = str[i];
			sbi_fifo_पङ्क्तौ_निक्षिप(&console_early_fifo, &ch, true);
		}
	}
	प्रत्यागम len;
}

स्थिर शून्य nputs_all(ध्रुव वर्ण *str, अचिह्नित दीर्घ len)
{
	अचिह्नित दीर्घ p = 0;

	यावत् (p < len)
		p += nputs(&str[p], len - p);
}

शून्य sbi_putc(वर्ण ch)
{
	nputs_all(&ch, 1);
}

शून्य sbi_puts(ध्रुव वर्ण *str)
{
	अचिह्नित दीर्घ len = sbi_strlen(str);

	spin_तालकम्(&console_बहिर्गमनम्_तालकम्);
	nputs_all(str, len);
	spin_unlock(&console_बहिर्गमनम्_तालकम्);
}

अचिह्नित दीर्घ sbi_nputs(ध्रुव वर्ण *str, अचिह्नित दीर्घ len)
{
	अचिह्नित दीर्घ ret;

	spin_तालकम्(&console_बहिर्गमनम्_तालकम्);
	ret = nputs(str, len);
	spin_unlock(&console_बहिर्गमनम्_तालकम्);

	प्रत्यागम ret;
}

शून्य sbi_gets(वर्ण *s, पूर्णाङ्क maxwidth, वर्ण endchar)
{
	पूर्णाङ्क ch;
	वर्ण *retval = s;

	यावत् ((ch = sbi_getc()) != endchar && ch >= 0 && maxwidth > 1) {
		*retval = (वर्ण)ch;
		retval++;
		maxwidth--;
	}
	*retval = '\0';
}

अचिह्नित दीर्घ sbi_ngets(वर्ण *str, अचिह्नित दीर्घ len)
{
	पूर्णाङ्क ch;
	अचिह्नित दीर्घ i;

	प्रति (i = 0; i < len; i++) {
		ch = sbi_getc();
		यदि (ch < 0)
			भङ्ग;
		str[i] = ch;
	}

	प्रत्यागम i;
}

#परिभाषय PAD_दक्षिणः 1
#परिभाषय PAD_ZERO 2
#परिभाषय PAD_ALTERNATE 4
#परिभाषय PAD_SIGN 8
#परिभाषय USE_TBUF 16
#परिभाषय PRINT_BUF_LEN 64

#परिभाषय va_start(v, l) __builtin_va_start((v), l)
#परिभाषय va_अन्तः __builtin_va_end
#परिभाषय va_arg __builtin_va_arg
प्रकारनिर्देश __builtin_va_list va_list;

स्थिर शून्य printc(वर्ण **बहिर्गमनम्, u32 *बहिर्गमनम्_len, वर्ण ch, पूर्णाङ्क flags)
{
	यदि (!बहिर्गमनम्) {
		sbi_putc(ch);
		प्रत्यागम;
	}

	/*
	 * The *printf entry point functions have enforced that (*out) can
	 * only be null when out_len is non-null and its value is zero.
	 */
	यदि (!बहिर्गमनम्_len || *बहिर्गमनम्_len > 1) {
		*(*बहिर्गमनम्)++ = ch;
		**बहिर्गमनम् = '\0';
		यदि (बहिर्गमनम्_len) {
			--(*बहिर्गमनम्_len);
			यदि ((flags & USE_TBUF) && *बहिर्गमनम्_len == 1) {
				nputs_all(console_tbuf, CONSOLE_TBUF_MAX - *बहिर्गमनम्_len);
				*बहिर्गमनम् = console_tbuf;
				*बहिर्गमनम्_len = CONSOLE_TBUF_MAX;
			}
		}
	}
}

स्थिर पूर्णाङ्क prints(वर्ण **बहिर्गमनम्, u32 *बहिर्गमनम्_len, ध्रुव वर्ण *string, पूर्णाङ्क विस्तारः,
		  पूर्णाङ्क flags)
{
	पूर्णाङ्क pc = 0;
	विस्तारः -= sbi_strlen(string);
	यदि (!(flags & PAD_दक्षिणः)) {
		प्रति (; विस्तारः > 0; --विस्तारः) {
			printc(बहिर्गमनम्, बहिर्गमनम्_len, flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	प्रति (; *string; ++string) {
		printc(बहिर्गमनम्, बहिर्गमनम्_len, *string, flags);
		++pc;
	}
	प्रति (; विस्तारः > 0; --विस्तारः) {
		printc(बहिर्गमनम्, बहिर्गमनम्_len, ' ', flags);
		++pc;
	}

	प्रत्यागम pc;
}

स्थिर पूर्णाङ्क printi(वर्ण **बहिर्गमनम्, u32 *बहिर्गमनम्_len, दीर्घ दीर्घ i,
		  पूर्णाङ्क विस्तारः, पूर्णाङ्क flags, पूर्णाङ्क type)
{
	पूर्णाङ्क pc = 0;
	वर्ण *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	अचिह्नित दीर्घ दीर्घ u, b, t;

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
		यावत् (u) {
			t = u % b;
			u = u / b;
			यदि (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	यदि (flags & PAD_ZERO) {
		यदि (sign) {
			printc(बहिर्गमनम्, बहिर्गमनम्_len, sign, flags);
			++pc;
			--विस्तारः;
		}
		यदि (i && (flags & PAD_ALTERNATE)) {
			यदि (b == 16 || b == 8) {
				printc(बहिर्गमनम्, बहिर्गमनम्_len, '0', flags);
				++pc;
				--विस्तारः;
			}
			यदि (b == 16) {
				printc(बहिर्गमनम्, बहिर्गमनम्_len, 'x' - 'a' + letbase, flags);
				++pc;
				--विस्तारः;
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

	प्रत्यागम pc + prints(बहिर्गमनम्, बहिर्गमनम्_len, s, विस्तारः, flags);
}

स्थिर पूर्णाङ्क print(वर्ण **बहिर्गमनम्, u32 *बहिर्गमनम्_len, ध्रुव वर्ण *format, va_list तर्काः)
{
	bool flags_समाप्तम्;
	पूर्णाङ्क विस्तारः, flags, pc = 0;
	वर्ण type, scr[2], *tout;
	bool use_tbuf = (!बहिर्गमनम्) ? true : false;

	/*
	 * The console_tbuf is protected by console_out_lock and
	 * print() is always called with console_out_lock held
	 * when out == NULL.
	 */
	यदि (use_tbuf) {
		console_tbuf_len = CONSOLE_TBUF_MAX;
		tout = console_tbuf;
		बहिर्गमनम् = &tout;
		बहिर्गमनम्_len = &console_tbuf_len;
	}

	/* handle special case: *out_len == 1*/
	यदि (बहिर्गमनम्) {
		यदि(!बहिर्गमनम्_len || *बहिर्गमनम्_len)
			**बहिर्गमनम् = '\0';
	}

	प्रति (; *format != 0; ++format) {
		विस्तारः = flags = 0;
		यदि (use_tbuf)
			flags |= USE_TBUF;
		यदि (*format == '%') {
			++format;
			यदि (*format == '\0')
				भङ्ग;
			यदि (*format == '%')
				गच्छ literal;
			/* Get flags */
			flags_समाप्तम् = false;
			यावत् (!flags_समाप्तम्) {
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
					flags_समाप्तम् = true;
					भङ्ग;
				}
				यदि (!flags_समाप्तम्)
					++format;
			}
			यदि (flags & PAD_दक्षिणः)
				flags &= ~PAD_ZERO;
			/* Get width */
			प्रति (; *format >= '0' && *format <= '9'; ++format) {
				विस्तारः *= 10;
				विस्तारः += *format - '0';
			}
			यदि (*format == 's') {
				वर्ण *s = va_arg(तर्काः, वर्ण *);
				pc += prints(बहिर्गमनम्, बहिर्गमनम्_len, s ? s : "(null)",
					     विस्तारः, flags);
				अनुवर्त;
			}
			यदि ((*format == 'd') || (*format == 'i')) {
				pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, va_arg(तर्काः, पूर्णाङ्क),
					     विस्तारः, flags, *format);
				अनुवर्त;
			}
			यदि ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, va_arg(तर्काः, अचिह्नित पूर्णाङ्क),
					     विस्तारः, flags, *format);
				अनुवर्त;
			}
			यदि ((*format == 'p') || (*format == 'P')) {
				pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, (uintptr_t)va_arg(तर्काः, शून्य*),
					     विस्तारः, flags, *format);
				अनुवर्त;
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
					pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, va_arg(तर्काः, दीर्घ दीर्घ),
						विस्तारः, flags, type);
					अनुवर्त;
				}
				यदि ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				यदि ((type == 'd') || (type == 'i'))
					pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, va_arg(तर्काः, दीर्घ),
					     विस्तारः, flags, type);
				अन्यथा
					pc += printi(बहिर्गमनम्, बहिर्गमनम्_len, va_arg(तर्काः, अचिह्नित दीर्घ),
					     विस्तारः, flags, type);
				अनुवर्त;
			}
			यदि (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(तर्काः, पूर्णाङ्क);
				scr[1] = '\0';
				pc += prints(बहिर्गमनम्, बहिर्गमनम्_len, scr, विस्तारः, flags);
				अनुवर्त;
			}
		} अन्यथा {
literal:
			printc(बहिर्गमनम्, बहिर्गमनम्_len, *format, flags);
			++pc;
		}
	}

	यदि (use_tbuf && console_tbuf_len < CONSOLE_TBUF_MAX)
		nputs_all(console_tbuf, CONSOLE_TBUF_MAX - console_tbuf_len);

	प्रत्यागम pc;
}

पूर्णाङ्क sbi_sprintf(वर्ण *बहिर्गमनम्, ध्रुव वर्ण *format, ...)
{
	va_list तर्काः;
	पूर्णाङ्क retval;

	यदि (unlikely(!बहिर्गमनम्))
		sbi_panic("sbi_sprintf called with NULL output string\n");

	va_start(तर्काः, format);
	retval = print(&बहिर्गमनम्, रिक्त, format, तर्काः);
	va_अन्तः(तर्काः);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_snprintf(वर्ण *बहिर्गमनम्, u32 बहिर्गमनम्_sz, ध्रुव वर्ण *format, ...)
{
	va_list तर्काः;
	पूर्णाङ्क retval;

	यदि (unlikely(!बहिर्गमनम् && बहिर्गमनम्_sz != 0))
		sbi_panic("sbi_snprintf called with NULL output string and "
			  "output size is not zero\n");

	va_start(तर्काः, format);
	retval = print(&बहिर्गमनम्, &बहिर्गमनम्_sz, format, तर्काः);
	va_अन्तः(तर्काः);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_printf(ध्रुव वर्ण *format, ...)
{
	va_list तर्काः;
	पूर्णाङ्क retval;

	spin_तालकम्(&console_बहिर्गमनम्_तालकम्);
	va_start(तर्काः, format);
	retval = print(रिक्त, रिक्त, format, तर्काः);
	va_अन्तः(तर्काः);
	spin_unlock(&console_बहिर्गमनम्_तालकम्);

	प्रत्यागम retval;
}

पूर्णाङ्क sbi_dprintf(ध्रुव वर्ण *format, ...)
{
	va_list तर्काः;
	पूर्णाङ्क retval = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	va_start(तर्काः, format);
	यदि (scratch->options & SBI_SCRATCH_DEBUG_PRINTS) {
		spin_तालकम्(&console_बहिर्गमनम्_तालकम्);
		retval = print(रिक्त, रिक्त, format, तर्काः);
		spin_unlock(&console_बहिर्गमनम्_तालकम्);
	}
	va_अन्तः(तर्काः);

	प्रत्यागम retval;
}

शून्य sbi_panic(ध्रुव वर्ण *format, ...)
{
	va_list तर्काः;

	spin_तालकम्(&console_बहिर्गमनम्_तालकम्);
	va_start(तर्काः, format);
	print(रिक्त, रिक्त, format, तर्काः);
	va_अन्तः(तर्काः);
	spin_unlock(&console_बहिर्गमनम्_तालकम्);

	sbi_hart_hang();
}

ध्रुव संरचना sbi_console_device *sbi_console_get_device(शून्य)
{
	प्रत्यागम console_dev;
}

शून्य sbi_console_समुच्चयः_device(ध्रुव संरचना sbi_console_device *dev)
{
	वर्ण ch;
	bool flush_early_fifo = false;

	यदि (!dev)
		प्रत्यागम;

	यदि (!console_dev)
		flush_early_fifo = true;

	console_dev = dev;

	यदि (flush_early_fifo) {
		यावत् (!sbi_fifo_पङ्क्तेः_निष्कासय(&console_early_fifo, &ch))
			sbi_putc(ch);
	}
}
