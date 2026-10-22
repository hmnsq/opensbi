// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_ಖಂಡಗಳು_misordered(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
				  ಪೂರ್ಣಾಂಕ mem_rsv_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ struct_ಗಾತ್ರ)
{
	ಹಿಂತಿರುಗಿ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(ಗಾತ್ರ(ರಚನೆ fdt_ಶೀರ್ಷಿಕೆ), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_ಗಾತ್ರ))
		|| (fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_ಗಾತ್ರ))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt)));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_rw_probe_(ಶೂನ್ಯ *fdt)
{
	ಆದರೆ (can_assume(VALID_DTB))
		ಹಿಂತಿರುಗಿ 0;
	FDT_RO_PROBE(fdt);

	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 17)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
	ಆದರೆ (fdt_ಖಂಡಗಳು_misordered(fdt, ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು),
				   fdt_ಗಾತ್ರ_dt_struct(fdt)))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADLAYOUT;
	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) > 17)
		fdt_ಗಣ_ಆವೃತ್ತಿ(fdt, 17);

	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_RW_PROBE(fdt) \
	{ \
		ಪೂರ್ಣಾಂಕ err_; \
		ಆದರೆ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ಹಿಂತಿರುಗಿ err_; \
	}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ fdt_ದತ್ತಾಂಶ_ಗಾತ್ರ(ಶೂನ್ಯ *fdt)
{
	ಹಿಂತಿರುಗಿ fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_splice_(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *splicepoint, ಪೂರ್ಣಾಂಕ oldlen, ಪೂರ್ಣಾಂಕ newlen)
{
	ಅಕ್ಷರ *p = splicepoint;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dsize = fdt_ದತ್ತಾಂಶ_ಗಾತ್ರ(fdt);
	size_t soff = p - (ಅಕ್ಷರ *)fdt;

	ಆದರೆ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;
	ಆದರೆ ((p < (ಅಕ್ಷರ *)fdt) || (dsize + newlen < (ಚಿಹ್ನೆರಹಿತ)oldlen))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;
	ಆದರೆ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((ಅಕ್ಷರ *)fdt + dsize) - (p + oldlen));
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_splice_mem_rsv_(ಶೂನ್ಯ *fdt, ರಚನೆ fdt_reserve_ನಮೂದು *p,
			       ಪೂರ್ಣಾಂಕ oldn, ಪೂರ್ಣಾಂಕ newn)
{
	ಪೂರ್ಣಾಂಕ delta = (newn - oldn) * ಗಾತ್ರ(*p);
	ಪೂರ್ಣಾಂಕ err;
	err = fdt_splice_(fdt, p, oldn * ಗಾತ್ರ(*p), newn * ಗಾತ್ರ(*p));
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;
	fdt_ಗಣ_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + delta);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_splice_struct_(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *p,
			      ಪೂರ್ಣಾಂಕ oldlen, ಪೂರ್ಣಾಂಕ newlen)
{
	ಪೂರ್ಣಾಂಕ delta = newlen - oldlen;
	ಪೂರ್ಣಾಂಕ err;

	ಆದರೆ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ಹಿಂತಿರುಗಿ err;

	fdt_ಗಣ_ಗಾತ್ರ_dt_struct(fdt, fdt_ಗಾತ್ರ_dt_struct(fdt) + delta);
	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + delta);
	ಹಿಂತಿರುಗಿ 0;
}

/* Must only be used to roll back in case of error */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_del_ಕೊನೆಯ_string(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
	ಪೂರ್ಣಾಂಕ newlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1;

	fdt_ಗಣ_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) - newlen);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_splice_string_(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ newlen)
{
	ಶೂನ್ಯ *p = (ಅಕ್ಷರ *)fdt
		+ fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಪೂರ್ಣಾಂಕ err;

	ಆದರೆ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ಹಿಂತಿರುಗಿ err;

	fdt_ಗಣ_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + newlen);
	ಹಿಂತಿರುಗಿ 0;
}

/**
 * fdt_find_add_string_() - Find or allocate a string
 *
 * @fdt: pointer to the device tree to check/adjust
 * @s: string to find/add
 * @allocated: Set to 0 if the string was found, 1 if not found and so
 *	allocated. Ignored if can_assume(NO_ROLLBACK)
 * @return offset of string in the string table (whether found or added)
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_ಕಂಡುಹಿಡಿ_ಕೂಡಿಸು_string(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *allocated)
{
	ಅಕ್ಷರ *strtab = (ಅಕ್ಷರ *)fdt + fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
	ಅಕ್ಷರ *new;
	ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1;
	ಪೂರ್ಣಾಂಕ err;

	ಆದರೆ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_ಕಂಡುಹಿಡಿ_string(strtab, fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt), s);
	ಆದರೆ (p)
		/* found it */
		ಹಿಂತಿರುಗಿ (p - strtab);

	new = strtab + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	err = fdt_splice_string_(fdt, len);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಆದರೆ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	ಸ್ಮರಣೆ_ನಕಲು(new, s, len);
	ಹಿಂತಿರುಗಿ (new - strtab);
}

ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_mem_rsv(ಶೂನ್ಯ *fdt, uint64_t ವಿಳಾಸ, uint64_t ಗಾತ್ರ_2)
{
	ರಚನೆ fdt_reserve_ನಮೂದು *re;
	ಪೂರ್ಣಾಂಕ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_ಅಂಕೆ_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	re->ವಿಳಾಸ = cpu_ಗಮ್ಯ_fdt64(ವಿಳಾಸ);
	re->ಗಾತ್ರ_2 = cpu_ಗಮ್ಯ_fdt64(ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_del_mem_rsv(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ n)
{
	ರಚನೆ fdt_reserve_ನಮೂದು *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ಆದರೆ (n >= fdt_ಅಂಕೆ_mem_rsv(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;

	ಹಿಂತಿರುಗಿ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_resize_property_(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
				ಪೂರ್ಣಾಂಕ len, ರಚನೆ fdt_property **prop)
{
	ಪೂರ್ಣಾಂಕ oldlen;
	ಪೂರ್ಣಾಂಕ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ಆದರೆ (!*prop)
		ಹಿಂತಿರುಗಿ oldlen;

	ಆದರೆ ((err = fdt_splice_struct_(fdt, (*prop)->ದತ್ತಾಂಶ, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ಹಿಂತಿರುಗಿ err;

	(*prop)->len = cpu_ಗಮ್ಯ_fdt32(len);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_property(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
			     ಪೂರ್ಣಾಂಕ len, ರಚನೆ fdt_property **prop)
{
	ಪೂರ್ಣಾಂಕ proplen;
	ಪೂರ್ಣಾಂಕ nextoffset;
	ಪೂರ್ಣಾಂಕ namestroff;
	ಪೂರ್ಣಾಂಕ err;
	ಪೂರ್ಣಾಂಕ allocated;

	ಆದರೆ ((nextoffset = fdt_check_ಗಂಟು_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset)) < 0)
		ಹಿಂತಿರುಗಿ nextoffset;

	namestroff = fdt_ಕಂಡುಹಿಡಿ_ಕೂಡಿಸು_string(fdt, name, &allocated);
	ಆದರೆ (namestroff < 0)
		ಹಿಂತಿರುಗಿ namestroff;

	*prop = fdt_ಸ್ಥಾನಾಂತರ_ptr_w(fdt, nextoffset);
	proplen = ಗಾತ್ರ(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ಆದರೆ (err) {
		/* Delete the string if we failed to add it */
		ಆದರೆ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_ಕೊನೆಯ_string(fdt, name);
		ಹಿಂತಿರುಗಿ err;
	}

	(*prop)->ಟ್ಯಾಗ್ = cpu_ಗಮ್ಯ_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_ಗಮ್ಯ_fdt32(namestroff);
	(*prop)->len = cpu_ಗಮ್ಯ_fdt32(len);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಗಣ_name(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಅಕ್ಷರ *namep;
	ಪೂರ್ಣಾಂಕ oldlen, newlen;
	ಪೂರ್ಣಾಂಕ err;

	FDT_RW_PROBE(fdt);

	namep = (ಅಕ್ಷರ *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ಆದರೆ (!namep)
		ಹಿಂತಿರುಗಿ oldlen;

	newlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಸ್ಮರಣೆ_ನಕಲು(namep, name, newlen+1);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_setprop_placeholder(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
			    ಪೂರ್ಣಾಂಕ len, ಶೂನ್ಯ **prop_ದತ್ತಾಂಶ)
{
	ರಚನೆ fdt_property *prop;
	ಪೂರ್ಣಾಂಕ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ಆದರೆ (err == -FDT_ERR_NOTFOUND)
		err = fdt_ಕೂಡಿಸು_property(fdt, nodeoffset, name, len, &prop);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	*prop_ದತ್ತಾಂಶ = prop->ದತ್ತಾಂಶ;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_setprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
		ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಂಕ len)
{
	ಶೂನ್ಯ *prop_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_ದತ್ತಾಂಶ);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಆದರೆ (len)
		ಸ್ಮರಣೆ_ನಕಲು(prop_ದತ್ತಾಂಶ, val, len);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_appendprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
		   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಂಕ len)
{
	ರಚನೆ fdt_property *prop;
	ಪೂರ್ಣಾಂಕ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ಆದರೆ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->ದತ್ತಾಂಶ,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ಆದರೆ (err)
			ಹಿಂತಿರುಗಿ err;
		prop->len = cpu_ಗಮ್ಯ_fdt32(newlen);
		ಸ್ಮರಣೆ_ನಕಲು(prop->ದತ್ತಾಂಶ + oldlen, val, len);
	} ಇಲ್ಲದಿದ್ದರೆ {
		err = fdt_ಕೂಡಿಸು_property(fdt, nodeoffset, name, len, &prop);
		ಆದರೆ (err)
			ಹಿಂತಿರುಗಿ err;
		ಸ್ಮರಣೆ_ನಕಲು(prop->ದತ್ತಾಂಶ, val, len);
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_delprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ರಚನೆ fdt_property *prop;
	ಪೂರ್ಣಾಂಕ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ len;

	proplen = ಗಾತ್ರ(*prop) + FDT_TAGALIGN(len);
	ಹಿಂತಿರುಗಿ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_subnode_namelen(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ parentoffset,
			    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen)
{
	ರಚನೆ fdt_ಗಂಟು_ಶೀರ್ಷಿಕೆ *nh;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, nextoffset;
	ಪೂರ್ಣಾಂಕ nodelen;
	ಪೂರ್ಣಾಂಕ err;
	uint32_t ಟ್ಯಾಗ್;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	ಸ್ಥಾನಾಂತರ = fdt_subnode_ಸ್ಥಾನಾಂತರ_namelen(fdt, parentoffset, name, namelen);
	ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_EXISTS;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಾನಾಂತರ != -FDT_ERR_NOTFOUND)
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

	/* Try to place the new node after the parent's properties */
	ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ಆದರೆ (!can_assume(LIBFDT_FLAWLESS) && (ಟ್ಯಾಗ್ != FDT_BEGIN_ಗಂಟು))
		ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;
	ಮಾಡು {
		ಸ್ಥಾನಾಂತರ = nextoffset;
		ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &nextoffset);
	} ಆಗ ((ಟ್ಯಾಗ್ == FDT_PROP) || (ಟ್ಯಾಗ್ == FDT_NOP));

	nh = fdt_ಸ್ಥಾನಾಂತರ_ptr_w(fdt, ಸ್ಥಾನಾಂತರ);
	nodelen = ಗಾತ್ರ(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	nh->ಟ್ಯಾಗ್ = cpu_ಗಮ್ಯ_fdt32(FDT_BEGIN_ಗಂಟು);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	ಸ್ಮರಣೆ_ನಕಲು(nh->name, name, namelen);
	endtag = (fdt32_t *)((ಅಕ್ಷರ *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_ಗಮ್ಯ_fdt32(FDT_ಕೊನೆ_ಗಂಟು);

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_subnode(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ parentoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಹಿಂತಿರುಗಿ fdt_ಕೂಡಿಸು_subnode_namelen(fdt, parentoffset, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

ಪೂರ್ಣಾಂಕ fdt_del_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ಗಂಟು_ಕೊನೆ_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset);
	ಆದರೆ (endoffset < 0)
		ಹಿಂತಿರುಗಿ endoffset;

	ಹಿಂತಿರುಗಿ fdt_splice_struct_(fdt, fdt_ಸ್ಥಾನಾಂತರ_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_packblocks_(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *old, ಅಕ್ಷರ *new,
			    ಪೂರ್ಣಾಂಕ mem_rsv_ಗಾತ್ರ,
			    ಪೂರ್ಣಾಂಕ struct_ಗಾತ್ರ,
			    ಪೂರ್ಣಾಂಕ ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ mem_rsv_off, struct_off, ಅಕ್ಷರಸರಣಿಗಳು_off;

	mem_rsv_off = FDT_ALIGN(ಗಾತ್ರ(ರಚನೆ fdt_ಶೀರ್ಷಿಕೆ), 8);
	struct_off = mem_rsv_off + mem_rsv_ಗಾತ್ರ;
	ಅಕ್ಷರಸರಣಿಗಳು_off = struct_off + struct_ಗಾತ್ರ;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_ಗಾತ್ರ);
	fdt_ಗಣ_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_ಗಾತ್ರ);
	fdt_ಗಣ_off_dt_struct(new, struct_off);
	fdt_ಗಣ_ಗಾತ್ರ_dt_struct(new, struct_ಗಾತ್ರ);

	memmove(new + ಅಕ್ಷರಸರಣಿಗಳು_off, old + fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(old), ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ);
	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(new, ಅಕ್ಷರಸರಣಿಗಳು_off);
	fdt_ಗಣ_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(new, fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(old));
}

ಪೂರ್ಣಾಂಕ fdt_open_into(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
	ಪೂರ್ಣಾಂಕ err;
	ಪೂರ್ಣಾಂಕ mem_rsv_ಗಾತ್ರ, struct_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ newsize;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdtstart = fdt;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdtend = fdtstart + fdt_totalsize(fdt);
	ಅಕ್ಷರ *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_ಗಾತ್ರ = (fdt_ಅಂಕೆ_mem_rsv(fdt)+1)
		* ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು);

	ಆದರೆ (can_assume(LATEST) || fdt_ಆವೃತ್ತಿ(fdt) >= 17) {
		struct_ಗಾತ್ರ = fdt_ಗಾತ್ರ_dt_struct(fdt);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_ಆವೃತ್ತಿ(fdt) == 16) {
		struct_ಗಾತ್ರ = 0;
		ಆಗ (fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, struct_ಗಾತ್ರ, &struct_ಗಾತ್ರ) != FDT_ಕೊನೆ)
			;
		ಆದರೆ (struct_ಗಾತ್ರ < 0)
			ಹಿಂತಿರುಗಿ struct_ಗಾತ್ರ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADVERSION;
	}

	ಆದರೆ (can_assume(LIBFDT_ಕ್ರಮ) ||
	    !fdt_ಖಂಡಗಳು_misordered(fdt, mem_rsv_ಗಾತ್ರ, struct_ಗಾತ್ರ)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ಆದರೆ (err)
			ಹಿಂತಿರುಗಿ err;
		fdt_ಗಣ_ಆವೃತ್ತಿ(buf, 17);
		fdt_ಗಣ_ಗಾತ್ರ_dt_struct(buf, struct_ಗಾತ್ರ);
		fdt_ಗಣ_totalsize(buf, bufsize);
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(ಗಾತ್ರ(ರಚನೆ fdt_ಶೀರ್ಷಿಕೆ), 8) + mem_rsv_ಗಾತ್ರ
		+ struct_ಗಾತ್ರ + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);

	ಆದರೆ (bufsize < newsize)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ಆದರೆ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (ಅಕ್ಷರ *)(uintptr_t)fdtend;
		ಆದರೆ ((tmp + newsize) > ((ಅಕ್ಷರ *)buf + bufsize))
			ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_ಗಾತ್ರ, struct_ಗಾತ್ರ,
			fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt));
	memmove(buf, tmp, newsize);

	fdt_ಗಣ_magic(buf, FDT_MAGIC);
	fdt_ಗಣ_totalsize(buf, bufsize);
	fdt_ಗಣ_ಆವೃತ್ತಿ(buf, 17);
	fdt_ಗಣ_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(buf, 16);
	fdt_ಗಣ_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_pack(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ mem_rsv_ಗಾತ್ರ;

	FDT_RW_PROBE(fdt);

	mem_rsv_ಗಾತ್ರ = (fdt_ಅಂಕೆ_mem_rsv(fdt)+1)
		* ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು);
	fdt_packblocks_(fdt, fdt, mem_rsv_ಗಾತ್ರ, fdt_ಗಾತ್ರ_dt_struct(fdt),
			fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt));
	fdt_ಗಣ_totalsize(fdt, fdt_ದತ್ತಾಂಶ_ಗಾತ್ರ(fdt));

	ಹಿಂತಿರುಗಿ 0;
}
