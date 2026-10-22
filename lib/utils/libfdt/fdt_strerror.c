// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

সংরচনা fdt_errtabent {
	ধ্রুৱ ৱর্ণ *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

স্থির সংরচনা fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((পূর্ণাঙ্ক)(পরিমাণ(fdt_errtable) / পরিমাণ(fdt_errtable[0])))

ধ্রুৱ ৱর্ণ *fdt_strerror(পূর্ণাঙ্ক errval)
{
	যদি (errval > 0)
		ঘূৰাই "<valid offset/length>";
	অন্যথা যদি (errval == 0)
		ঘূৰাই "<no error>";
	অন্যথা যদি (-errval < FDT_ERRTABSIZE) {
		ধ্রুৱ ৱর্ণ *s = fdt_errtable[-errval].str;

		যদি (s)
			ঘূৰাই s;
	}

	ঘূৰাই "<unknown error>";
}
