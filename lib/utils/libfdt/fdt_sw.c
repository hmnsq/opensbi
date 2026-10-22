// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_sw_probe_(ಶೂನ್ಯ *fdt)
{
	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಯದಿ (fdt_magic(fdt) == FDT_MAGIC)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTATE;
		ಅನ್ಯಥಾ ಯದಿ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADMAGIC;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ಪೂರ್ಣಾಙ್ಕ err; \
		ಯದಿ ((err = fdt_sw_probe_(fdt)) != 0) \
			ಪ್ರತ್ಯಾಗಮ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_sw_probe_memrsv_(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ err = fdt_sw_probe_(fdt);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) && fdt_off_dt_strings(fdt) != 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTATE;
	ಪ್ರತ್ಯಾಗಮ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ಪೂರ್ಣಾಙ್ಕ err; \
		ಯದಿ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ಪ್ರತ್ಯಾಗಮ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_sw_probe_struct_(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ err = fdt_sw_probe_(fdt);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTATE;
	ಪ್ರತ್ಯಾಗಮ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ಪೂರ್ಣಾಙ್ಕ err; \
		ಯದಿ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ಪ್ರತ್ಯಾಗಮ err; \
	}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint32_t sw_flags(ಶೂನ್ಯ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ಪ್ರತ್ಯಾಗಮ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ಸ್ಥಿರ ಶೂನ್ಯ *fdt_grab_space_(ಶೂನ್ಯ *fdt, size_t len)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ offset = fdt_size_dt_struct(fdt);
	unsigned ಪೂರ್ಣಾಙ್ಕ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ಯದಿ ((offset + len < offset) || (offset + len > spaceleft))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	fdt_set_size_dt_struct(fdt, offset + len);
	ಪ್ರತ್ಯಾಗಮ fdt_offset_ptr_w_(fdt, offset);
}

ಪೂರ್ಣಾಙ್ಕ fdt_create_with_flags(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize, uint32_t flags)
{
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ hdrsize = FDT_ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_header),
				      ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry));
	ಶೂನ್ಯ *fdt = buf;

	ಯದಿ (bufsize < hdrsize)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	ಯದಿ (flags & ~FDT_CREATE_FLAGS_ALL)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADFLAGS;

	memset(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_set_magic(fdt, FDT_SW_MAGIC);
	fdt_set_version(fdt, FDT_LAST_SUPPORTED_VERSION);
	fdt_set_last_comp_version(fdt, flags);

	fdt_set_totalsize(fdt,  bufsize);

	fdt_set_off_mem_rsvmap(fdt, hdrsize);
	fdt_set_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_set_off_dt_strings(fdt, 0);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_create(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
	ಪ್ರತ್ಯಾಗಮ fdt_create_with_flags(buf, bufsize, 0);
}

ಪೂರ್ಣಾಙ್ಕ fdt_resize(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
	size_t headsize, tailsize;
	ವರ್ಣ *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ಯದಿ (bufsize < 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ಯದಿ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;

	ಯದಿ ((headsize + tailsize) > (unsigned)bufsize)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	oldtail = (ವರ್ಣ *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (ವರ್ಣ *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ಯದಿ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ಅನ್ಯಥಾ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ಯದಿ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_add_reservemap_entry(ಶೂನ್ಯ *fdt, uint64_t addr, uint64_t size)
{
	ಸಂರಚನಾ fdt_reserve_entry *re;
	ಪೂರ್ಣಾಙ್ಕ offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ಯದಿ ((offset + ಪರಿಮಾಣ(*re)) > fdt_totalsize(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	re = (ಸಂರಚನಾ fdt_reserve_entry *)((ವರ್ಣ *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + ಪರಿಮಾಣ(*re));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_finish_reservemap(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ err = fdt_add_reservemap_entry(fdt, 0, 0);

	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_begin_ಗಂಟು(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *name)
{
	ಸಂರಚನಾ fdt_ಗಂಟು_header *nh;
	ಪೂರ್ಣಾಙ್ಕ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, ಪರಿಮಾಣ(*nh) + FDT_TAGALIGN(namelen));
	ಯದಿ (! nh)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ಗಂಟು);
	memcpy(nh->name, name, namelen);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_end_ಗಂಟು(ಶೂನ್ಯ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ಯದಿ (! en)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_ಗಂಟು);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_add_string_(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *s)
{
	ವರ್ಣ *strtab = (ವರ್ಣ *)fdt + fdt_totalsize(fdt);
	unsigned ಪೂರ್ಣಾಙ್ಕ strtabsize = fdt_size_dt_strings(fdt);
	unsigned ಪೂರ್ಣಾಙ್ಕ len = strlen(s) + 1;
	unsigned ಪೂರ್ಣಾಙ್ಕ struct_ತುದಿ, offset;

	offset = strtabsize + len;
	struct_ತುದಿ = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ಯದಿ (fdt_totalsize(fdt) - offset < struct_ತುದಿ)
		ಪ್ರತ್ಯಾಗಮ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ಪ್ರತ್ಯಾಗಮ -offset;
}

/* Must only be used to roll back in case of error */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_del_last_string_(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *s)
{
	ಪೂರ್ಣಾಙ್ಕ strtabsize = fdt_size_dt_strings(fdt);
	ಪೂರ್ಣಾಙ್ಕ len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_find_add_string_(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *allocated)
{
	ವರ್ಣ *strtab = (ವರ್ಣ *)fdt + fdt_totalsize(fdt);
	ಪೂರ್ಣಾಙ್ಕ strtabsize = fdt_size_dt_strings(fdt);
	ಧ್ರುವ ವರ್ಣ *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ಯದಿ (p)
		ಪ್ರತ್ಯಾಗಮ p - strtab;

	*allocated = 1;

	ಪ್ರತ್ಯಾಗಮ fdt_add_string_(fdt, s);
}

ಪೂರ್ಣಾಙ್ಕ fdt_property_placeholder(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ len, ಶೂನ್ಯ **valp)
{
	ಸಂರಚನಾ fdt_property *prop;
	ಪೂರ್ಣಾಙ್ಕ nameoff;
	ಪೂರ್ಣಾಙ್ಕ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ಯದಿ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} ಅನ್ಯಥಾ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ಯದಿ (nameoff == 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, ಪರಿಮಾಣ(*prop) + FDT_TAGALIGN(len));
	ಯದಿ (! prop) {
		ಯದಿ (allocated)
			fdt_del_last_string_(fdt, name);
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_property(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಶೂನ್ಯ *ptr;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;
	memcpy(ptr, val, len);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_finish(ಶೂನ್ಯ *fdt)
{
	ವರ್ಣ *p = (ವರ್ಣ *)fdt;
	fdt32_t *end;
	ಪೂರ್ಣಾಙ್ಕ oldstroffset, newstroffset;
	uint32_t tag;
	ಪೂರ್ಣಾಙ್ಕ offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, ಪರಿಮಾಣ(*end));
	ಯದಿ (! end)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ಯಾವತ್ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ಯದಿ (tag == FDT_PROP) {
			ಸಂರಚನಾ fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ಪೂರ್ಣಾಙ್ಕ nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ಯದಿ (nextoffset < 0)
		ಪ್ರತ್ಯಾಗಮ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ಪ್ರತ್ಯಾಗಮ 0;
}
