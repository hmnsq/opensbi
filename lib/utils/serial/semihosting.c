/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

static long semihosting_trap(ціле sysnum, void *addr)
{
	register ціле ret asm ("a0") = sysnum;
	register void *param0 asm ("a1") = addr;

	asm volatile (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	повернути ret;
}

static bool _semihosting_enabled = true;
static bool try_semihosting = true;

bool semihosting_enabled(void)
{
	register ціле ret asm ("a0") = SYSERRNO;
	register void *param0 asm ("a1") = NULL;
	unsigned long tmp = 0;

	якщо (!try_semihosting)
		повернути _semihosting_enabled;

	asm volatile (
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
	повернути _semihosting_enabled;
}

static ціле semihosting_errno(void)
{
	long ret = semihosting_trap(SYSERRNO, NULL);

	якщо (ret > 0)
		повернути -ret;
	повернути SBI_EIO;
}

static ціле semihosting_infd = SBI_ENODEV;
static ціле semihosting_outfd = SBI_ENODEV;

static long semihosting_open(const char *fname, enum semihosting_open_mode mode)
{
	long fd;
	struct semihosting_open_s {
		const char *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	якщо (fd == -1)
		повернути semihosting_errno();
	повернути fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
struct semihosting_rdwr_s {
	long fd;
	void *memp;
	size_t len;
};

static long semihosting_read(long fd, void *memp, size_t len)
{
	long ret;
	struct semihosting_rdwr_s read;

	read.fd = fd;
	read.memp = memp;
	read.len = len;

	ret = semihosting_trap(SYSREAD, &read);
	якщо (ret < 0)
		повернути semihosting_errno();
	повернути len - ret;
}

static long semihosting_write(long fd, const void *memp, size_t len)
{
	long ret;
	struct semihosting_rdwr_s write;

	write.fd = fd;
	write.memp = (void *)memp;
	write.len = len;

	ret = semihosting_trap(SYSWRITE, &write);
	якщо (ret < 0)
		повернути semihosting_errno();
	повернути len - ret;
}

/* clang-format on */

static unsigned long semihosting_puts(const char *str, unsigned long len)
{
	char ch;
	long ret;
	unsigned long i;

	якщо (semihosting_outfd < 0) {
		для (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} інакше
		ret = semihosting_write(semihosting_outfd, str, len);

	повернути (ret < 0) ? 0 : ret;
}

static ціле semihosting_getc(void)
{
	char ch = 0;
	ціле ret;

	якщо (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, NULL);
		ret = ret < 0 ? -1 : ret;
	} інакше
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	повернути ret;
}

static struct sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

ціле semihosting_init(void)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	повернути 0;
}
