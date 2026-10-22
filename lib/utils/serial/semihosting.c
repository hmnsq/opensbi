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

स्थिर दीर्घ semihosting_trap(पूर्णांक sysnum, शून्य *addr)
{
	नोंदणी पूर्णांक ret यंत्र ("a0") = sysnum;
	नोंदणी शून्य *param0 यंत्र ("a1") = addr;

	यंत्र अस्थिर (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	परत ret;
}

स्थिर bool _semihosting_enabled = true;
स्थिर bool try_semihosting = true;

bool semihosting_enabled(शून्य)
{
	नोंदणी पूर्णांक ret यंत्र ("a0") = SYSERRNO;
	नोंदणी शून्य *param0 यंत्र ("a1") = रिक्त;
	अचिन्हित दीर्घ tmp = 0;

	जर (!try_semihosting)
		परत _semihosting_enabled;

	यंत्र अस्थिर (
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
	परत _semihosting_enabled;
}

स्थिर पूर्णांक semihosting_errno(शून्य)
{
	दीर्घ ret = semihosting_trap(SYSERRNO, रिक्त);

	जर (ret > 0)
		परत -ret;
	परत SBI_EIO;
}

स्थिर पूर्णांक semihosting_infd = SBI_ENODEV;
स्थिर पूर्णांक semihosting_outfd = SBI_ENODEV;

स्थिर दीर्घ semihosting_open(स्थिरांक अक्षर *fname, गणना semihosting_open_mode mode)
{
	दीर्घ fd;
	संरचना semihosting_open_s {
		स्थिरांक अक्षर *fname;
		अचिन्हित दीर्घ mode;
		माप_प्रकार len;
	} खोलो;

	खोलो.fname = fname;
	खोलो.len = sbi_strlen(fname);
	खोलो.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &खोलो);
	जर (fd == -1)
		परत semihosting_errno();
	परत fd;
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
	माप_प्रकार len;
};

स्थिर दीर्घ semihosting_read(दीर्घ fd, शून्य *memp, माप_प्रकार len)
{
	दीर्घ ret;
	संरचना semihosting_rdwr_s पठ;

	पठ.fd = fd;
	पठ.memp = memp;
	पठ.len = len;

	ret = semihosting_trap(SYSREAD, &पठ);
	जर (ret < 0)
		परत semihosting_errno();
	परत len - ret;
}

स्थिर दीर्घ semihosting_write(दीर्घ fd, स्थिरांक शून्य *memp, माप_प्रकार len)
{
	दीर्घ ret;
	संरचना semihosting_rdwr_s लिख;

	लिख.fd = fd;
	लिख.memp = (शून्य *)memp;
	लिख.len = len;

	ret = semihosting_trap(SYSWRITE, &लिख);
	जर (ret < 0)
		परत semihosting_errno();
	परत len - ret;
}

/* clang-format on */

स्थिर अचिन्हित दीर्घ semihosting_puts(स्थिरांक अक्षर *str, अचिन्हित दीर्घ len)
{
	अक्षर ch;
	दीर्घ ret;
	अचिन्हित दीर्घ i;

	जर (semihosting_outfd < 0) {
		साठी (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} अन्यथा
		ret = semihosting_write(semihosting_outfd, str, len);

	परत (ret < 0) ? 0 : ret;
}

स्थिर पूर्णांक semihosting_getc(शून्य)
{
	अक्षर ch = 0;
	पूर्णांक ret;

	जर (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, रिक्त);
		ret = ret < 0 ? -1 : ret;
	} अन्यथा
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	परत ret;
}

स्थिर संरचना sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

पूर्णांक semihosting_init(शून्य)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	परत 0;
}
