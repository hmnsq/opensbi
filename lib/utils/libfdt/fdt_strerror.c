// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

నిర్మాణం fdt_errtabent {
	స్థిరరాశి అక్షరం *str;
};

#నిర్వచించు FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

స్థిర నిర్మాణం fdt_errtabent fdt_errtable[] = {
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
	FDT_ERRTABENT(FDT_ERR_అంతర్గత),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#నిర్వచించు FDT_ERRTABSIZE	((పూర్ణాంకం)(పరిమాణం(fdt_errtable) / పరిమాణం(fdt_errtable[0])))

స్థిరరాశి అక్షరం *fdt_strerror(పూర్ణాంకం errval)
{
	ఒకవేళ (errval > 0)
		తిరిగి "<valid offset/length>";
	లేకపోతే ఒకవేళ (errval == 0)
		తిరిగి "<no error>";
	లేకపోతే ఒకవేళ (-errval < FDT_ERRTABSIZE) {
		స్థిరరాశి అక్షరం *s = fdt_errtable[-errval].str;

		ఒకవేళ (s)
			తిరిగి s;
	}

	తిరిగి "<unknown error>";
}
