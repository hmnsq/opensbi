// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_sw_probe_(ಶೂನ್ಯ *fdt)
{
	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಆದರೆ (fdt_magic(fdt) == FDT_MAGIC)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTATE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADMAGIC;
	}

	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_SW_PROBE(fdt) \
	{ \
		ಪೂರ್ಣಾಂಕ err; \
		ಆದರೆ ((err = fdt_sw_probe_(fdt)) != 0) \
			ಹಿಂತಿರುಗಿ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_sw_probe_memrsv_(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ err = fdt_sw_probe_(fdt);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) && fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) != 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTATE;
	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ಪೂರ್ಣಾಂಕ err; \
		ಆದರೆ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ಹಿಂತಿರುಗಿ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_sw_probe_struct_(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ err = fdt_sw_probe_(fdt);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) &&
	    fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) != fdt_totalsize(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTATE;
	ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ಪೂರ್ಣಾಂಕ err; \
		ಆದರೆ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ಹಿಂತಿರುಗಿ err; \
	}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t sw_flags(ಶೂನ್ಯ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ಹಿಂತಿರುಗಿ fdt_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ಸ್ಥಿರ ಶೂನ್ಯ *fdt_grab_space_(ಶೂನ್ಯ *fdt, size_t len)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = fdt_ಗಾತ್ರ_dt_struct(fdt);
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);

	ಆದರೆ ((ಸ್ಥಾನಾಂತರ + len < ಸ್ಥಾನಾಂತರ) || (ಸ್ಥಾನಾಂತರ + len > spaceleft))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	fdt_ಗಣ_ಗಾತ್ರ_dt_struct(fdt, ಸ್ಥಾನಾಂತರ + len);
	ಹಿಂತಿರುಗಿ fdt_ಸ್ಥಾನಾಂತರ_ptr_w(fdt, ಸ್ಥಾನಾಂತರ);
}

ಪೂರ್ಣಾಂಕ fdt_create_with_flags(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize, uint32_t flags)
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ hdrsize = FDT_ALIGN(ಗಾತ್ರ(ರಚನೆ fdt_ಶೀರ್ಷಿಕೆ),
				      ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು));
	ಶೂನ್ಯ *fdt = buf;

	ಆದರೆ (bufsize < hdrsize)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	ಆದರೆ (flags & ~FDT_CREATE_FLAGS_ALL)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADFLAGS;

	memset(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_ಗಣ_magic(fdt, FDT_SW_MAGIC);
	fdt_ಗಣ_ಆವೃತ್ತಿ(fdt, FDT_ಕೊನೆಯ_SUPPORTED_ಆವೃತ್ತಿ);
	fdt_ಗಣ_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt, flags);

	fdt_ಗಣ_totalsize(fdt,  bufsize);

	fdt_ಗಣ_off_mem_rsvmap(fdt, hdrsize);
	fdt_ಗಣ_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, 0);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_create(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
	ಹಿಂತಿರುಗಿ fdt_create_with_flags(buf, bufsize, 0);
}

ಪೂರ್ಣಾಂಕ fdt_resize(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ bufsize)
{
	size_t headsize, tailsize;
	ಅಕ್ಷರ *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ಆದರೆ (bufsize < 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_ಗಾತ್ರ_dt_struct(fdt);
	tailsize = fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);

	ಆದರೆ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_ಆಂತರಿಕ;

	ಆದರೆ ((headsize + tailsize) > (ಚಿಹ್ನೆರಹಿತ)bufsize)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	oldtail = (ಅಕ್ಷರ *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (ಅಕ್ಷರ *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ಆದರೆ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ಇಲ್ಲದಿದ್ದರೆ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_ಗಣ_totalsize(buf, bufsize);
	ಆದರೆ (fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(buf))
		fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(buf, bufsize);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_reservemap_ನಮೂದು(ಶೂನ್ಯ *fdt, uint64_t addr, uint64_t ಗಾತ್ರ_2)
{
	ರಚನೆ fdt_reserve_ನಮೂದು *re;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	FDT_SW_PROBE_MEMRSV(fdt);

	ಸ್ಥಾನಾಂತರ = fdt_off_dt_struct(fdt);
	ಆದರೆ ((ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ(*re)) > fdt_totalsize(fdt))
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	re = (ರಚನೆ fdt_reserve_ನಮೂದು *)((ಅಕ್ಷರ *)fdt + ಸ್ಥಾನಾಂತರ);
	re->ವಿಳಾಸ = cpu_ಗಮ್ಯ_fdt64(addr);
	re->ಗಾತ್ರ_2 = cpu_ಗಮ್ಯ_fdt64(ಗಾತ್ರ_2);

	fdt_ಗಣ_off_dt_struct(fdt, ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ(*re));

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಕ್ತಾಯ_reservemap(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ err = fdt_ಕೂಡಿಸು_reservemap_ನಮೂದು(fdt, 0, 0);

	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ err;

	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, fdt_totalsize(fdt));
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_begin_ಗಂಟು(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ರಚನೆ fdt_ಗಂಟು_ಶೀರ್ಷಿಕೆ *nh;
	ಪೂರ್ಣಾಂಕ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) + 1;
	nh = fdt_grab_space_(fdt, ಗಾತ್ರ(*nh) + FDT_TAGALIGN(namelen));
	ಆದರೆ (! nh)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	nh->ಟ್ಯಾಗ್ = cpu_ಗಮ್ಯ_fdt32(FDT_BEGIN_ಗಂಟು);
	ಸ್ಮರಣೆ_ನಕಲು(nh->name, name, namelen);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಕೊನೆ_ಗಂಟು(ಶೂನ್ಯ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ಆದರೆ (! en)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	*en = cpu_ಗಮ್ಯ_fdt32(FDT_ಕೊನೆ_ಗಂಟು);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_ಕೂಡಿಸು_string(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
	ಅಕ್ಷರ *strtab = (ಅಕ್ಷರ *)fdt + fdt_totalsize(fdt);
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ strtabsize = fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ struct_ತುದಿ, ಸ್ಥಾನಾಂತರ;

	ಸ್ಥಾನಾಂತರ = strtabsize + len;
	struct_ತುದಿ = fdt_off_dt_struct(fdt) + fdt_ಗಾತ್ರ_dt_struct(fdt);
	ಆದರೆ (fdt_totalsize(fdt) - ಸ್ಥಾನಾಂತರ < struct_ತುದಿ)
		ಹಿಂತಿರುಗಿ 0; /* no more room :( */

	ಸ್ಮರಣೆ_ನಕಲು(strtab - ಸ್ಥಾನಾಂತರ, s, len);
	fdt_ಗಣ_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, strtabsize + len);
	ಹಿಂತಿರುಗಿ -ಸ್ಥಾನಾಂತರ;
}

/* Must only be used to roll back in case of error */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_del_ಕೊನೆಯ_string(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
	ಪೂರ್ಣಾಂಕ strtabsize = fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) + 1;

	fdt_ಗಣ_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, strtabsize - len);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_ಕಂಡುಹಿಡಿ_ಕೂಡಿಸು_string(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *allocated)
{
	ಅಕ್ಷರ *strtab = (ಅಕ್ಷರ *)fdt + fdt_totalsize(fdt);
	ಪೂರ್ಣಾಂಕ strtabsize = fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

	*allocated = 0;

	p = fdt_ಕಂಡುಹಿಡಿ_string(strtab - strtabsize, strtabsize, s);
	ಆದರೆ (p)
		ಹಿಂತಿರುಗಿ p - strtab;

	*allocated = 1;

	ಹಿಂತಿರುಗಿ fdt_ಕೂಡಿಸು_string(fdt, s);
}

ಪೂರ್ಣಾಂಕ fdt_property_placeholder(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ len, ಶೂನ್ಯ **valp)
{
	ರಚನೆ fdt_property *prop;
	ಪೂರ್ಣಾಂಕ nameoff;
	ಪೂರ್ಣಾಂಕ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ಆದರೆ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_ಕೂಡಿಸು_string(fdt, name);
	} ಇಲ್ಲದಿದ್ದರೆ {
		nameoff = fdt_ಕಂಡುಹಿಡಿ_ಕೂಡಿಸು_string(fdt, name, &allocated);
	}
	ಆದರೆ (nameoff == 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, ಗಾತ್ರ(*prop) + FDT_TAGALIGN(len));
	ಆದರೆ (! prop) {
		ಆದರೆ (allocated)
			fdt_del_ಕೊನೆಯ_string(fdt, name);
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;
	}

	prop->ಟ್ಯಾಗ್ = cpu_ಗಮ್ಯ_fdt32(FDT_PROP);
	prop->nameoff = cpu_ಗಮ್ಯ_fdt32(nameoff);
	prop->len = cpu_ಗಮ್ಯ_fdt32(len);
	*valp = prop->ದತ್ತಾಂಶ;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_property(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಂಕ len)
{
	ಶೂನ್ಯ *ptr;
	ಪೂರ್ಣಾಂಕ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;
	ಸ್ಮರಣೆ_ನಕಲು(ptr, val, len);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಕ್ತಾಯ(ಶೂನ್ಯ *fdt)
{
	ಅಕ್ಷರ *p = (ಅಕ್ಷರ *)fdt;
	fdt32_t *ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ oldstroffset, newstroffset;
	uint32_t ಟ್ಯಾಗ್;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	ಕೊನೆ = fdt_grab_space_(fdt, ಗಾತ್ರ(*ಕೊನೆ));
	ಆದರೆ (! ಕೊನೆ)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;
	*ಕೊನೆ = cpu_ಗಮ್ಯ_fdt32(FDT_ಕೊನೆ);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_ಗಾತ್ರ_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt));
	fdt_ಗಣ_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	ಸ್ಥಾನಾಂತರ = 0;
	ಆಗ ((ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &nextoffset)) != FDT_ಕೊನೆ) {
		ಆದರೆ (ಟ್ಯಾಗ್ == FDT_PROP) {
			ರಚನೆ fdt_property *prop =
				fdt_ಸ್ಥಾನಾಂತರ_ptr_w(fdt, ಸ್ಥಾನಾಂತರ);
			ಪೂರ್ಣಾಂಕ nameoff;

			nameoff = fdt32_ಗಮ್ಯ_cpu(prop->nameoff);
			nameoff += fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
			prop->nameoff = cpu_ಗಮ್ಯ_fdt32(nameoff);
		}
		ಸ್ಥಾನಾಂತರ = nextoffset;
	}
	ಆದರೆ (nextoffset < 0)
		ಹಿಂತಿರುಗಿ nextoffset;

	/* Finally, adjust the header */
	fdt_ಗಣ_totalsize(fdt, newstroffset + fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_ಗಣ_ಕೊನೆಯ_comp_ಆವೃತ್ತಿ(fdt, FDT_ಕೊನೆಯ_COMPATIBLE_ಆವೃತ್ತಿ);
	fdt_ಗಣ_magic(fdt, FDT_MAGIC);

	ಹಿಂತಿರುಗಿ 0;
}
