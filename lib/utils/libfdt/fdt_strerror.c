// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ସଂରଚନା fdt_errtabent {
	ଅଚଳ ଅକ୍ଷର *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

ସ୍ଥିର ସଂରଚନା fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଆକାର(fdt_errtable) / ଆକାର(fdt_errtable[0])))

ଅଚଳ ଅକ୍ଷର *fdt_strerror(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errval)
{
	ଯଦି (errval > 0)
		ଫେରନ୍ତୁ "<valid offset/length>";
	ଅନ୍ୟଥା ଯଦି (errval == 0)
		ଫେରନ୍ତୁ "<no error>";
	ଅନ୍ୟଥା ଯଦି (-errval < FDT_ERRTABSIZE) {
		ଅଚଳ ଅକ୍ଷର *s = fdt_errtable[-errval].str;

		ଯଦି (s)
			ଫେରନ୍ତୁ s;
	}

	ଫେରନ୍ତୁ "<unknown error>";
}
