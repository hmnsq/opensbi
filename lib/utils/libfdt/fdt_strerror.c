// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

sa_nracanaa fdt_errtabent {
	const var_na *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

static sa_nracanaa fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((ကိန်းပြည့်)(parimaa_na(fdt_errtable) / parimaa_na(fdt_errtable[0])))

const var_na *fdt_strerror(ကိန်းပြည့် errval)
{
	အကယ်၍ (errval > 0)
		ပြန်ပေး "<valid offset/length>";
	မဟုတ်ပါက အကယ်၍ (errval == 0)
		ပြန်ပေး "<no error>";
	မဟုတ်ပါက အကယ်၍ (-errval < FDT_ERRTABSIZE) {
		const var_na *s = fdt_errtable[-errval].str;

		အကယ်၍ (s)
			ပြန်ပေး s;
	}

	ပြန်ပေး "<unknown error>";
}
