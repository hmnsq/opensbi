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

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

स्थिर long semihosting_trap(पूर्णाङ्क sysnum, शून्य *addr)
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

	पटिनिवत्त ret;
}

स्थिर bool _semihosting_enabled = true;
स्थिर bool try_semihosting = true;

bool semihosting_enabled(शून्य)
{
	पञ्जिका पूर्णाङ्क ret asm ("a0") = SYSERRNO;
	पञ्जिका शून्य *param0 asm ("a1") = रिक्त;
	unsigned long tmp = 0;

	सचे (!try_semihosting)
		पटिनिवत्त _semihosting_enabled;

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
	पटिनिवत्त _semihosting_enabled;
}

स्थिर पूर्णाङ्क semihosting_errno(शून्य)
{
	long ret = semihosting_trap(SYSERRNO, रिक्त);

	सचे (ret > 0)
		पटिनिवत्त -ret;
	पटिनिवत्त SBI_EIO;
}

स्थिर पूर्णाङ्क semihosting_infd = SBI_ENODEV;
स्थिर पूर्णाङ्क semihosting_outfd = SBI_ENODEV;

स्थिर long semihosting_open(ध्रुव वर्ण *fname, गणना semihosting_open_mode mode)
{
	long fd;
	संरचना semihosting_open_s {
		ध्रुव वर्ण *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	सचे (fd == -1)
		पटिनिवत्त semihosting_errno();
	पटिनिवत्त fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
संरचना semihosting_rdwr_s {
	long fd;
	शून्य *memp;
	size_t len;
};

स्थिर long semihosting_read(long fd, शून्य *memp, size_t len)
{
	long ret;
	संरचना semihosting_rdwr_s पठ;

	पठ.fd = fd;
	पठ.memp = memp;
	पठ.len = len;

	ret = semihosting_trap(SYSREAD, &पठ);
	सचे (ret < 0)
		पटिनिवत्त semihosting_errno();
	पटिनिवत्त len - ret;
}

स्थिर long semihosting_write(long fd, ध्रुव शून्य *memp, size_t len)
{
	long ret;
	संरचना semihosting_rdwr_s लिख;

	लिख.fd = fd;
	लिख.memp = (शून्य *)memp;
	लिख.len = len;

	ret = semihosting_trap(SYSWRITE, &लिख);
	सचे (ret < 0)
		पटिनिवत्त semihosting_errno();
	पटिनिवत्त len - ret;
}

/* clang-format on */

स्थिर unsigned long semihosting_puts(ध्रुव वर्ण *str, unsigned long len)
{
	वर्ण ch;
	long ret;
	unsigned long i;

	सचे (semihosting_outfd < 0) {
		अत्थाय (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} अञ्ञथा
		ret = semihosting_write(semihosting_outfd, str, len);

	पटिनिवत्त (ret < 0) ? 0 : ret;
}

स्थिर पूर्णाङ्क semihosting_getc(शून्य)
{
	वर्ण ch = 0;
	पूर्णाङ्क ret;

	सचे (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, रिक्त);
		ret = ret < 0 ? -1 : ret;
	} अञ्ञथा
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	पटिनिवत्त ret;
}

स्थिर संरचना sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

पूर्णाङ्क semihosting_init(शून्य)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	पटिनिवत्त 0;
}
