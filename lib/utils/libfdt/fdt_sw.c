// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

સ્થિર પૂર્ણાંક fdt_sw_probe_(શૂન્ય *fdt)
{
	જો (!can_assume(VALID_આગત)) {
		જો (fdt_magic(fdt) == FDT_MAGIC)
			પાછા -FDT_ERR_BADSTATE;
		નહીંતર જો (fdt_magic(fdt) != FDT_SW_MAGIC)
			પાછા -FDT_ERR_BADMAGIC;
	}

	પાછા 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		પૂર્ણાંક err; \
		જો ((err = fdt_sw_probe_(fdt)) != 0) \
			પાછા err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
સ્થિર પૂર્ણાંક fdt_sw_probe_memrsv_(શૂન્ય *fdt)
{
	પૂર્ણાંક err = fdt_sw_probe_(fdt);
	જો (err)
		પાછા err;

	જો (!can_assume(VALID_આગત) && fdt_off_dt_strings(fdt) != 0)
		પાછા -FDT_ERR_BADSTATE;
	પાછા 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		પૂર્ણાંક err; \
		જો ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			પાછા err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
સ્થિર પૂર્ણાંક fdt_sw_probe_struct_(શૂન્ય *fdt)
{
	પૂર્ણાંક err = fdt_sw_probe_(fdt);
	જો (err)
		પાછા err;

	જો (!can_assume(VALID_આગત) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		પાછા -FDT_ERR_BADSTATE;
	પાછા 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		પૂર્ણાંક err; \
		જો ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			પાછા err; \
	}

સ્થિર અંતઃરેખ uint32_t sw_flags(શૂન્ય *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	પાછા fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

સ્થિર શૂન્ય *fdt_grab_space_(શૂન્ય *fdt, size_t len)
{
	unsigned પૂર્ણાંક offset = fdt_size_dt_struct(fdt);
	unsigned પૂર્ણાંક spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	જો ((offset + len < offset) || (offset + len > spaceleft))
		પાછા ખાલી;

	fdt_set_size_dt_struct(fdt, offset + len);
	પાછા fdt_offset_ptr_w_(fdt, offset);
}

પૂર્ણાંક fdt_create_with_flags(શૂન્ય *buf, પૂર્ણાંક bufsize, uint32_t flags)
{
	અચલ પૂર્ણાંક hdrsize = FDT_ALIGN(કદ(સંરચના fdt_header),
				      કદ(સંરચના fdt_reserve_entry));
	શૂન્ય *fdt = buf;

	જો (bufsize < hdrsize)
		પાછા -FDT_ERR_NOSPACE;

	જો (flags & ~FDT_CREATE_FLAGS_ALL)
		પાછા -FDT_ERR_BADFLAGS;

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

	પાછા 0;
}

પૂર્ણાંક fdt_create(શૂન્ય *buf, પૂર્ણાંક bufsize)
{
	પાછા fdt_create_with_flags(buf, bufsize, 0);
}

પૂર્ણાંક fdt_resize(શૂન્ય *fdt, શૂન્ય *buf, પૂર્ણાંક bufsize)
{
	size_t headsize, tailsize;
	અક્ષર *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	જો (bufsize < 0)
		પાછા -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	જો (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		પાછા -FDT_ERR_INTERNAL;

	જો ((headsize + tailsize) > (unsigned)bufsize)
		પાછા -FDT_ERR_NOSPACE;

	oldtail = (અક્ષર *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (અક્ષર *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	જો (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} નહીંતર {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	જો (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	પાછા 0;
}

પૂર્ણાંક fdt_add_reservemap_entry(શૂન્ય *fdt, uint64_t addr, uint64_t size)
{
	સંરચના fdt_reserve_entry *re;
	પૂર્ણાંક offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	જો ((offset + કદ(*re)) > fdt_totalsize(fdt))
		પાછા -FDT_ERR_NOSPACE;

	re = (સંરચના fdt_reserve_entry *)((અક્ષર *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + કદ(*re));

	પાછા 0;
}

પૂર્ણાંક fdt_finish_reservemap(શૂન્ય *fdt)
{
	પૂર્ણાંક err = fdt_add_reservemap_entry(fdt, 0, 0);

	જો (err)
		પાછા err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	પાછા 0;
}

પૂર્ણાંક fdt_begin_ગાંઠ(શૂન્ય *fdt, અચલ અક્ષર *name)
{
	સંરચના fdt_ગાંઠ_header *nh;
	પૂર્ણાંક namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, કદ(*nh) + FDT_TAGALIGN(namelen));
	જો (! nh)
		પાછા -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ગાંઠ);
	memcpy(nh->name, name, namelen);
	પાછા 0;
}

પૂર્ણાંક fdt_end_ગાંઠ(શૂન્ય *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	જો (! en)
		પાછા -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_ગાંઠ);
	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_add_string_(શૂન્ય *fdt, અચલ અક્ષર *s)
{
	અક્ષર *strtab = (અક્ષર *)fdt + fdt_totalsize(fdt);
	unsigned પૂર્ણાંક strtabsize = fdt_size_dt_strings(fdt);
	unsigned પૂર્ણાંક len = strlen(s) + 1;
	unsigned પૂર્ણાંક struct_ટોચ, offset;

	offset = strtabsize + len;
	struct_ટોચ = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	જો (fdt_totalsize(fdt) - offset < struct_ટોચ)
		પાછા 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	પાછા -offset;
}

/* Must only be used to roll back in case of error */
સ્થિર શૂન્ય fdt_del_last_string_(શૂન્ય *fdt, અચલ અક્ષર *s)
{
	પૂર્ણાંક strtabsize = fdt_size_dt_strings(fdt);
	પૂર્ણાંક len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

સ્થિર પૂર્ણાંક fdt_find_add_string_(શૂન્ય *fdt, અચલ અક્ષર *s, પૂર્ણાંક *allocated)
{
	અક્ષર *strtab = (અક્ષર *)fdt + fdt_totalsize(fdt);
	પૂર્ણાંક strtabsize = fdt_size_dt_strings(fdt);
	અચલ અક્ષર *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	જો (p)
		પાછા p - strtab;

	*allocated = 1;

	પાછા fdt_add_string_(fdt, s);
}

પૂર્ણાંક fdt_property_placeholder(શૂન્ય *fdt, અચલ અક્ષર *name, પૂર્ણાંક len, શૂન્ય **valp)
{
	સંરચના fdt_property *prop;
	પૂર્ણાંક nameoff;
	પૂર્ણાંક allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	જો (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} નહીંતર {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	જો (nameoff == 0)
		પાછા -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, કદ(*prop) + FDT_TAGALIGN(len));
	જો (! prop) {
		જો (allocated)
			fdt_del_last_string_(fdt, name);
		પાછા -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	પાછા 0;
}

પૂર્ણાંક fdt_property(શૂન્ય *fdt, અચલ અક્ષર *name, અચલ શૂન્ય *val, પૂર્ણાંક len)
{
	શૂન્ય *ptr;
	પૂર્ણાંક ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	જો (ret)
		પાછા ret;
	memcpy(ptr, val, len);
	પાછા 0;
}

પૂર્ણાંક fdt_finish(શૂન્ય *fdt)
{
	અક્ષર *p = (અક્ષર *)fdt;
	fdt32_t *end;
	પૂર્ણાંક oldstroffset, newstroffset;
	uint32_t tag;
	પૂર્ણાંક offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, કદ(*end));
	જો (! end)
		પાછા -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	જ્યારે ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		જો (tag == FDT_PROP) {
			સંરચના fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			પૂર્ણાંક nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	જો (nextoffset < 0)
		પાછા nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	પાછા 0;
}
