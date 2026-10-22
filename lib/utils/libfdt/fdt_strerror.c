// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

কাঠামো fdt_errtabent {
	ধ্রুবক অক্ষর *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

স্থির কাঠামো fdt_errtabent fdt_errtable[] = {
	FDT_ERRTABENT(FDT_ERR_NOTFOUND),
	FDT_ERRTABENT(FDT_ERR_EXISTS),
	FDT_ERRTABENT(FDT_ERR_NOSPACE),

	FDT_ERRTABENT(FDT_ERR_BADOFFSET),
	FDT_ERRTABENT(FDT_ERR_BADPATH),
	FDT_ERRTABENT(FDT_ERR_BADPHANDLE),
	FDT_ERRTABENT(FDT_ERR_BADSTATE),

	FDT_ERRTABENT(FDT_ERR_TRUNCATED),
	FDT_ERRTABENT(FDT_ERR_BADMAGIC),
	FDT_ERRTABENT(FDT_ERR_BADVERSION),
	FDT_ERRTABENT(FDT_ERR_BADSTRUCTURE),
	FDT_ERRTABENT(FDT_ERR_BADLAYOUT),
	FDT_ERRTABENT(FDT_ERR_INTERNAL),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#define FDT_ERRTABSIZE	((পূর্ণসংখ্যা)(আকার(fdt_errtable) / আকার(fdt_errtable[0])))

ধ্রুবক অক্ষর *fdt_strerror(পূর্ণসংখ্যা errval)
{
	যদি (errval > 0)
		ফেরত "<valid offset/length>";
	অন্যথা যদি (errval == 0)
		ফেরত "<no error>";
	অন্যথা যদি (-errval < FDT_ERRTABSIZE) {
		ধ্রুবক অক্ষর *s = fdt_errtable[-errval].str;

		যদি (s)
			ফেরত s;
	}

	ফেরত "<unknown error>";
}
