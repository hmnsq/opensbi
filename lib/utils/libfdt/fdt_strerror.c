// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ರಚನೆ fdt_errtabent {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
};

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

ಸ್ಥಿರ ರಚನೆ fdt_errtabent fdt_errtable[] = {
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
	FDT_ERRTABENT(FDT_ERR_ಆಂತರಿಕ),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#ವ್ಯಾಖ್ಯಾನಿಸು FDT_ERRTABSIZE	((ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(fdt_errtable) / ಗಾತ್ರ(fdt_errtable[0])))

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_strerror(ಪೂರ್ಣಾಂಕ errval)
{
	ಆದರೆ (errval > 0)
		ಹಿಂತಿರುಗಿ "<valid offset/length>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errval == 0)
		ಹಿಂತಿರುಗಿ "<no error>";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (-errval < FDT_ERRTABSIZE) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = fdt_errtable[-errval].str;

		ಆದರೆ (s)
			ಹಿಂತಿರುಗಿ s;
	}

	ಹಿಂತಿರುಗಿ "<unknown error>";
}
