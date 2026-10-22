/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_string.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

સ્થિર long semihosting_trap(પૂર્ણાંક sysnum, શૂન્ય *addr)
{
	નોંધણી પૂર્ણાંક ret asm ("a0") = sysnum;
	નોંધણી શૂન્ય *param0 asm ("a1") = addr;

	asm અસ્થિર (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	પાછા ret;
}

સ્થિર bool _semihosting_enabled = true;
સ્થિર bool try_semihosting = true;

bool semihosting_enabled(શૂન્ય)
{
	નોંધણી પૂર્ણાંક ret asm ("a0") = SYSERRNO;
	નોંધણી શૂન્ય *param0 asm ("a1") = ખાલી;
	unsigned long tmp = 0;

	જો (!try_semihosting)
		પાછા _semihosting_enabled;

	asm અસ્થિર (
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
	પાછા _semihosting_enabled;
}

સ્થિર પૂર્ણાંક semihosting_errno(શૂન્ય)
{
	long ret = semihosting_trap(SYSERRNO, ખાલી);

	જો (ret > 0)
		પાછા -ret;
	પાછા SBI_EIO;
}

સ્થિર પૂર્ણાંક semihosting_infd = SBI_ENODEV;
સ્થિર પૂર્ણાંક semihosting_outfd = SBI_ENODEV;

સ્થિર long semihosting_open(અચલ અક્ષર *fname, ગણના semihosting_open_mode mode)
{
	long fd;
	સંરચના semihosting_open_s {
		અચલ અક્ષર *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	જો (fd == -1)
		પાછા semihosting_errno();
	પાછા fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
સંરચના semihosting_rdwr_s {
	long fd;
	શૂન્ય *memp;
	size_t len;
};

સ્થિર long semihosting_read(long fd, શૂન્ય *memp, size_t len)
{
	long ret;
	સંરચના semihosting_rdwr_s પઠ;

	પઠ.fd = fd;
	પઠ.memp = memp;
	પઠ.len = len;

	ret = semihosting_trap(SYSREAD, &પઠ);
	જો (ret < 0)
		પાછા semihosting_errno();
	પાછા len - ret;
}

સ્થિર long semihosting_write(long fd, અચલ શૂન્ય *memp, size_t len)
{
	long ret;
	સંરચના semihosting_rdwr_s લિખ;

	લિખ.fd = fd;
	લિખ.memp = (શૂન્ય *)memp;
	લિખ.len = len;

	ret = semihosting_trap(SYSWRITE, &લિખ);
	જો (ret < 0)
		પાછા semihosting_errno();
	પાછા len - ret;
}

/* clang-format on */

સ્થિર unsigned long semihosting_puts(અચલ અક્ષર *str, unsigned long len)
{
	અક્ષર ch;
	long ret;
	unsigned long i;

	જો (semihosting_outfd < 0) {
		માટે (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} નહીંતર
		ret = semihosting_write(semihosting_outfd, str, len);

	પાછા (ret < 0) ? 0 : ret;
}

સ્થિર પૂર્ણાંક semihosting_getc(શૂન્ય)
{
	અક્ષર ch = 0;
	પૂર્ણાંક ret;

	જો (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ખાલી);
		ret = ret < 0 ? -1 : ret;
	} નહીંતર
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	પાછા ret;
}

સ્થિર સંરચના sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

પૂર્ણાંક semihosting_init(શૂન્ય)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	પાછા 0;
}
