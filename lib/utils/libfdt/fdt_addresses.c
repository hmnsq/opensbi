// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2014 David Gibson <david@gibson.dropbear.id.au>
 * Copyright (C) 2018 embedded brains GmbH
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_cells(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *c;
	uint32_t val;
	ಪೂರ್ಣಾಂಕ len;

	c = fdt_getprop(fdt, nodeoffset, name, &len);
	ಆದರೆ (!c)
		ಹಿಂತಿರುಗಿ len;

	ಆದರೆ (len != ಗಾತ್ರ(*c))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADNCELLS;

	val = fdt32_ಗಮ್ಯ_cpu(*c);
	ಆದರೆ (val > FDT_MAX_NCELLS)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADNCELLS;

	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)val;
}

ಪೂರ್ಣಾಂಕ fdt_ವಿಳಾಸ_cells(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ val;

	val = fdt_cells(fdt, nodeoffset, "#address-cells");
	ಆದರೆ (val == 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADNCELLS;
	ಆದರೆ (val == -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ 2;
	ಹಿಂತಿರುಗಿ val;
}

ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ_cells(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ val;

	val = fdt_cells(fdt, nodeoffset, "#size-cells");
	ಆದರೆ (val == -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ 1;
	ಹಿಂತಿರುಗಿ val;
}

/* This function assumes that [address|size]_cells is 1 or 2 */
ಪೂರ್ಣಾಂಕ fdt_appendprop_addrrange(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಪೋಷಕ, ಪೂರ್ಣಾಂಕ nodeoffset,
			     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, uint64_t addr, uint64_t ಗಾತ್ರ_2)
{
	ಪೂರ್ಣಾಂಕ addr_cells, ಗಾತ್ರ_cells, ret;
	uint8_t ದತ್ತಾಂಶ[ಗಾತ್ರ(fdt64_t) * 2], *prop;

	ret = fdt_ವಿಳಾಸ_cells(fdt, ಪೋಷಕ);
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ ret;
	addr_cells = ret;

	ret = fdt_ಗಾತ್ರ_cells(fdt, ಪೋಷಕ);
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ ret;
	ಗಾತ್ರ_cells = ret;

	/* check validity of address */
	prop = ದತ್ತಾಂಶ;
	ಆದರೆ (addr_cells == 1) {
		ಆದರೆ ((addr > UINT32_MAX) || ((UINT32_MAX + 1 - addr) < ಗಾತ್ರ_2))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)addr);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (addr_cells == 2) {
		fdt64_st(prop, addr);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADNCELLS;
	}

	/* check validity of size */
	prop += addr_cells * ಗಾತ್ರ(fdt32_t);
	ಆದರೆ (ಗಾತ್ರ_cells == 1) {
		ಆದರೆ (ಗಾತ್ರ_2 > UINT32_MAX)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		fdt32_st(prop, (uint32_t)ಗಾತ್ರ_2);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_cells == 2) {
		fdt64_st(prop, ಗಾತ್ರ_2);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADNCELLS;
	}

	ಹಿಂತಿರುಗಿ fdt_appendprop(fdt, nodeoffset, name, ದತ್ತಾಂಶ,
			      (addr_cells + ಗಾತ್ರ_cells) * ಗಾತ್ರ(fdt32_t));
}
