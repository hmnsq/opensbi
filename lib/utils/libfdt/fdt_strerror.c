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
#define FDT_ERRTABSIZE	((puur_naa_nka)(parimaa_na(fdt_errtable) / parimaa_na(fdt_errtable[0])))

const var_na *fdt_strerror(puur_naa_nka errval)
{
	ᬬᬾᬦ᭄ (errval > 0)
		ᬯᬮᬶ "<valid offset/length>";
	ᬮᬶᬬ ᬬᬾᬦ᭄ (errval == 0)
		ᬯᬮᬶ "<no error>";
	ᬮᬶᬬ ᬬᬾᬦ᭄ (-errval < FDT_ERRTABSIZE) {
		const var_na *s = fdt_errtable[-errval].str;

		ᬬᬾᬦ᭄ (s)
			ᬯᬮᬶ s;
	}

	ᬯᬮᬶ "<unknown error>";
}
