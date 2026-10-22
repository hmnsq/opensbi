/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_string.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi_utils/serial/semihosting.h>

#परिभाषय SYSOPEN     0x01
#परिभाषय SYSWRITEC   0x03
#परिभाषय SYSWRITE    0x05
#परिभाषय SYSREAD     0x06
#परिभाषय SYSREADC    0x07
#परिभाषय SYSERRNO	0x13

स्थिर दीर्घ semihosting_trap(पूर्णाङ्क sysnum, शून्य *addr)
{
	पञ्जिका पूर्णाङ्क ret asm ("a0") = sysnum;
	पञ्जिका शून्य *param0 asm ("a1") = addr;

	asm चञ्चल (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	प्रत्यागम ret;
}

स्थिर bool _semihosting_enabled = true;
स्थिर bool try_semihosting = true;

bool semihosting_enabled(शून्य)
{
	पञ्जिका पूर्णाङ्क ret asm ("a0") = SYSERRNO;
	पञ्जिका शून्य *param0 asm ("a1") = रिक्त;
	अचिह्नित दीर्घ tmp = 0;

	यदि (!try_semihosting)
		प्रत्यागम _semihosting_enabled;

	asm चञ्चल (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	j _semihost_test_vector_next\n"
		"	.align 4\n"
		"_semihost_test_vector:\n"
		"	csrr %[en], mepc\n"
		"	addi %[en], %[en], 4\n"
		"	csrw mepc, %[en]\n"
		"	add %[en], zero, zero\n"
		"	mret\n"
		"_semihost_test_vector_next:\n"

		"	lla %[tmp], _semihost_test_vector\n"
		"	csrrw %[tmp], mtvec, %[tmp]\n"
		"	.align 4\n"
		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"
		"	csrw mtvec, %[tmp]\n"

		"	.option pop\n"
		: [tmp] "+r" (tmp), [en] "+r" (_semihosting_enabled),
		  [ret] "+r" (ret)
		: "r" (param0) : "memory");

	try_semihosting = false;
	प्रत्यागम _semihosting_enabled;
}

स्थिर पूर्णाङ्क semihosting_errno(शून्य)
{
	दीर्घ ret = semihosting_trap(SYSERRNO, रिक्त);

	यदि (ret > 0)
		प्रत्यागम -ret;
	प्रत्यागम SBI_EIO;
}

स्थिर पूर्णाङ्क semihosting_infd = SBI_ENODEV;
स्थिर पूर्णाङ्क semihosting_outfd = SBI_ENODEV;

स्थिर दीर्घ semihosting_open(ध्रुव वर्ण *fname, गणना semihosting_open_बहुलकम् बहुलकम्)
{
	दीर्घ fd;
	संरचना semihosting_open_s {
		ध्रुव वर्ण *fname;
		अचिह्नित दीर्घ बहुलकम्;
		size_t len;
	} उद्घाटय;

	उद्घाटय.fname = fname;
	उद्घाटय.len = sbi_strlen(fname);
	उद्घाटय.बहुलकम् = बहुलकम्;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &उद्घाटय);
	यदि (fd == -1)
		प्रत्यागम semihosting_errno();
	प्रत्यागम fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
संरचना semihosting_rdwr_s {
	दीर्घ fd;
	शून्य *memp;
	size_t len;
};

स्थिर दीर्घ semihosting_read(दीर्घ fd, शून्य *memp, size_t len)
{
	दीर्घ ret;
	संरचना semihosting_rdwr_s पठ;

	पठ.fd = fd;
	पठ.memp = memp;
	पठ.len = len;

	ret = semihosting_trap(SYSREAD, &पठ);
	यदि (ret < 0)
		प्रत्यागम semihosting_errno();
	प्रत्यागम len - ret;
}

स्थिर दीर्घ semihosting_write(दीर्घ fd, ध्रुव शून्य *memp, size_t len)
{
	दीर्घ ret;
	संरचना semihosting_rdwr_s लिख;

	लिख.fd = fd;
	लिख.memp = (शून्य *)memp;
	लिख.len = len;

	ret = semihosting_trap(SYSWRITE, &लिख);
	यदि (ret < 0)
		प्रत्यागम semihosting_errno();
	प्रत्यागम len - ret;
}

/* clang-format on */

स्थिर अचिह्नित दीर्घ semihosting_puts(ध्रुव वर्ण *str, अचिह्नित दीर्घ len)
{
	वर्ण ch;
	दीर्घ ret;
	अचिह्नित दीर्घ i;

	यदि (semihosting_outfd < 0) {
		प्रति (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} अन्यथा
		ret = semihosting_write(semihosting_outfd, str, len);

	प्रत्यागम (ret < 0) ? 0 : ret;
}

स्थिर पूर्णाङ्क semihosting_getc(शून्य)
{
	वर्ण ch = 0;
	पूर्णाङ्क ret;

	यदि (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, रिक्त);
		ret = ret < 0 ? -1 : ret;
	} अन्यथा
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	प्रत्यागम ret;
}

स्थिर संरचना sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

पूर्णाङ्क semihosting_init(शून्य)
{
	semihosting_infd = semihosting_open(":tt", बहुलकम्_READ);
	semihosting_outfd = semihosting_open(":tt", बहुलकम्_WRITE);

	sbi_console_समुच्चयः_device(&semihosting_console);

	प्रत्यागम 0;
}
