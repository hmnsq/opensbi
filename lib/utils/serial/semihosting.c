/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi_utils/serial/semihosting.h>

#घोषणा SYSOPEN     0x01
#घोषणा SYSWRITEC   0x03
#घोषणा SYSWRITE    0x05
#घोषणा SYSREAD     0x06
#घोषणा SYSREADC    0x07
#घोषणा SYSERRNO	0x13

अटल दीर्घ semihosting_trap(पूर्णांक sysnum, व्योम *addr)
{
	रेजिस्टर पूर्णांक ret यंत्र ("a0") = sysnum;
	रेजिस्टर व्योम *param0 यंत्र ("a1") = addr;

	यंत्र अस्थिर (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	वापस ret;
}

अटल bool _semihosting_enabled = true;
अटल bool try_semihosting = true;

bool semihosting_enabled(व्योम)
{
	रेजिस्टर पूर्णांक ret यंत्र ("a0") = SYSERRNO;
	रेजिस्टर व्योम *param0 यंत्र ("a1") = शून्य;
	अचिन्हित दीर्घ tmp = 0;

	अगर (!try_semihosting)
		वापस _semihosting_enabled;

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
	वापस _semihosting_enabled;
}

अटल पूर्णांक semihosting_errno(व्योम)
{
	दीर्घ ret = semihosting_trap(SYSERRNO, शून्य);

	अगर (ret > 0)
		वापस -ret;
	वापस SBI_EIO;
}

अटल पूर्णांक semihosting_infd = SBI_ENODEV;
अटल पूर्णांक semihosting_outfd = SBI_ENODEV;

अटल दीर्घ semihosting_open(स्थिर अक्षर *fname, क्रमागत semihosting_open_mode mode)
{
	दीर्घ fd;
	काष्ठा semihosting_open_s {
		स्थिर अक्षर *fname;
		अचिन्हित दीर्घ mode;
		माप_प्रकार len;
	} खोलो;

	खोलो.fname = fname;
	खोलो.len = sbi_strlen(fname);
	खोलो.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &खोलो);
	अगर (fd == -1)
		वापस semihosting_errno();
	वापस fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
काष्ठा semihosting_rdwr_s {
	दीर्घ fd;
	व्योम *memp;
	माप_प्रकार len;
};

अटल दीर्घ semihosting_read(दीर्घ fd, व्योम *memp, माप_प्रकार len)
{
	दीर्घ ret;
	काष्ठा semihosting_rdwr_s पढ़ो;

	पढ़ो.fd = fd;
	पढ़ो.memp = memp;
	पढ़ो.len = len;

	ret = semihosting_trap(SYSREAD, &पढ़ो);
	अगर (ret < 0)
		वापस semihosting_errno();
	वापस len - ret;
}

अटल दीर्घ semihosting_write(दीर्घ fd, स्थिर व्योम *memp, माप_प्रकार len)
{
	दीर्घ ret;
	काष्ठा semihosting_rdwr_s ग_लिखो;

	ग_लिखो.fd = fd;
	ग_लिखो.memp = (व्योम *)memp;
	ग_लिखो.len = len;

	ret = semihosting_trap(SYSWRITE, &ग_लिखो);
	अगर (ret < 0)
		वापस semihosting_errno();
	वापस len - ret;
}

/* clang-format on */

अटल अचिन्हित दीर्घ semihosting_puts(स्थिर अक्षर *str, अचिन्हित दीर्घ len)
{
	अक्षर ch;
	दीर्घ ret;
	अचिन्हित दीर्घ i;

	अगर (semihosting_outfd < 0) {
		क्रम (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} अन्यथा
		ret = semihosting_write(semihosting_outfd, str, len);

	वापस (ret < 0) ? 0 : ret;
}

अटल पूर्णांक semihosting_getc(व्योम)
{
	अक्षर ch = 0;
	पूर्णांक ret;

	अगर (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, शून्य);
		ret = ret < 0 ? -1 : ret;
	} अन्यथा
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	वापस ret;
}

अटल काष्ठा sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

पूर्णांक semihosting_init(व्योम)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	वापस 0;
}
