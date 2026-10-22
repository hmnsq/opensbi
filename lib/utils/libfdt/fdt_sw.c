// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ຈຳນວນເຕັມ fdt_sw_probe_(void *fdt)
{
	ຖ້າ (!can_assume(VALID_INPUT)) {
		ຖ້າ (fdt_magic(fdt) == FDT_MAGIC)
			ສົ່ງຄືນ -FDT_ERR_BADSTATE;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ສົ່ງຄືນ -FDT_ERR_BADMAGIC;
	}

	ສົ່ງຄືນ 0;
}

#define FDT_SW_PROBE(fdt) \
	{ \
		ຈຳນວນເຕັມ err; \
		ຖ້າ ((err = fdt_sw_probe_(fdt)) != 0) \
			ສົ່ງຄືນ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
static ຈຳນວນເຕັມ fdt_sw_probe_memrsv_(void *fdt)
{
	ຈຳນວນເຕັມ err = fdt_sw_probe_(fdt);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	ຖ້າ (!can_assume(VALID_INPUT) && fdt_off_dt_strings(fdt) != 0)
		ສົ່ງຄືນ -FDT_ERR_BADSTATE;
	ສົ່ງຄືນ 0;
}

#define FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ຈຳນວນເຕັມ err; \
		ຖ້າ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ສົ່ງຄືນ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
static ຈຳນວນເຕັມ fdt_sw_probe_struct_(void *fdt)
{
	ຈຳນວນເຕັມ err = fdt_sw_probe_(fdt);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	ຖ້າ (!can_assume(VALID_INPUT) &&
	    fdt_off_dt_strings(fdt) != fdt_totalsize(fdt))
		ສົ່ງຄືນ -FDT_ERR_BADSTATE;
	ສົ່ງຄືນ 0;
}

#define FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ຈຳນວນເຕັມ err; \
		ຖ້າ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ສົ່ງຄືນ err; \
	}

static anta_hstha uint32_t sw_flags(void *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ສົ່ງຄືນ fdt_last_comp_version(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

static void *fdt_grab_space_(void *fdt, size_t len)
{
	unsigned ຈຳນວນເຕັມ offset = fdt_size_dt_struct(fdt);
	unsigned ຈຳນວນເຕັມ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_size_dt_strings(fdt);

	ຖ້າ ((offset + len < offset) || (offset + len > spaceleft))
		ສົ່ງຄືນ NULL;

	fdt_set_size_dt_struct(fdt, offset + len);
	ສົ່ງຄືນ fdt_offset_ptr_w_(fdt, offset);
}

ຈຳນວນເຕັມ fdt_create_with_flags(void *buf, ຈຳນວນເຕັມ bufsize, uint32_t flags)
{
	const ຈຳນວນເຕັມ hdrsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header),
				      parimaa_na(sa_nracanaa fdt_reserve_entry));
	void *fdt = buf;

	ຖ້າ (bufsize < hdrsize)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	ຖ້າ (flags & ~FDT_CREATE_FLAGS_ALL)
		ສົ່ງຄືນ -FDT_ERR_BADFLAGS;

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

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_create(void *buf, ຈຳນວນເຕັມ bufsize)
{
	ສົ່ງຄືນ fdt_create_with_flags(buf, bufsize, 0);
}

ຈຳນວນເຕັມ fdt_resize(void *fdt, void *buf, ຈຳນວນເຕັມ bufsize)
{
	size_t headsize, tailsize;
	var_na *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ຖ້າ (bufsize < 0)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	tailsize = fdt_size_dt_strings(fdt);

	ຖ້າ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ສົ່ງຄືນ -FDT_ERR_INTERNAL;

	ຖ້າ ((headsize + tailsize) > (unsigned)bufsize)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	oldtail = (var_na *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (var_na *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ຖ້າ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ບໍ່ດັ່ງນັ້ນ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_set_totalsize(buf, bufsize);
	ຖ້າ (fdt_off_dt_strings(buf))
		fdt_set_off_dt_strings(buf, bufsize);

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ຈຳນວນເຕັມ offset;

	FDT_SW_PROBE_MEMRSV(fdt);

	offset = fdt_off_dt_struct(fdt);
	ຖ້າ ((offset + parimaa_na(*re)) > fdt_totalsize(fdt))
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	re = (sa_nracanaa fdt_reserve_entry *)((var_na *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt_set_off_dt_struct(fdt, offset + parimaa_na(*re));

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_finish_reservemap(void *fdt)
{
	ຈຳນວນເຕັມ err = fdt_add_reservemap_entry(fdt, 0, 0);

	ຖ້າ (err)
		ສົ່ງຄືນ err;

	fdt_set_off_dt_strings(fdt, fdt_totalsize(fdt));
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_begin_node(void *fdt, const var_na *name)
{
	sa_nracanaa fdt_node_header *nh;
	ຈຳນວນເຕັມ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = strlen(name) + 1;
	nh = fdt_grab_space_(fdt, parimaa_na(*nh) + FDT_TAGALIGN(namelen));
	ຖ້າ (! nh)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_end_node(void *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ຖ້າ (! en)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_add_string_(void *fdt, const var_na *s)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	unsigned ຈຳນວນເຕັມ strtabsize = fdt_size_dt_strings(fdt);
	unsigned ຈຳນວນເຕັມ len = strlen(s) + 1;
	unsigned ຈຳນວນເຕັມ struct_top, offset;

	offset = strtabsize + len;
	struct_top = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	ຖ້າ (fdt_totalsize(fdt) - offset < struct_top)
		ສົ່ງຄືນ 0; /* no more room :( */

	memcpy(strtab - offset, s, len);
	fdt_set_size_dt_strings(fdt, strtabsize + len);
	ສົ່ງຄືນ -offset;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ຈຳນວນເຕັມ strtabsize = fdt_size_dt_strings(fdt);
	ຈຳນວນເຕັມ len = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, strtabsize - len);
}

static ຈຳນວນເຕັມ fdt_find_add_string_(void *fdt, const var_na *s, ຈຳນວນເຕັມ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_totalsize(fdt);
	ຈຳນວນເຕັມ strtabsize = fdt_size_dt_strings(fdt);
	const var_na *p;

	*allocated = 0;

	p = fdt_find_string_(strtab - strtabsize, strtabsize, s);
	ຖ້າ (p)
		ສົ່ງຄືນ p - strtab;

	*allocated = 1;

	ສົ່ງຄືນ fdt_add_string_(fdt, s);
}

ຈຳນວນເຕັມ fdt_property_placeholder(void *fdt, const var_na *name, ຈຳນວນເຕັມ len, void **valp)
{
	sa_nracanaa fdt_property *prop;
	ຈຳນວນເຕັມ nameoff;
	ຈຳນວນເຕັມ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ຖ້າ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_add_string_(fdt, name);
	} ບໍ່ດັ່ງນັ້ນ {
		nameoff = fdt_find_add_string_(fdt, name, &allocated);
	}
	ຖ້າ (nameoff == 0)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, parimaa_na(*prop) + FDT_TAGALIGN(len));
	ຖ້າ (! prop) {
		ຖ້າ (allocated)
			fdt_del_last_string_(fdt, name);
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;
	}

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	*valp = prop->data;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_property(void *fdt, const var_na *name, const void *val, ຈຳນວນເຕັມ len)
{
	void *ptr;
	ຈຳນວນເຕັມ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ຖ້າ (ret)
		ສົ່ງຄືນ ret;
	memcpy(ptr, val, len);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_finish(void *fdt)
{
	var_na *p = (var_na *)fdt;
	fdt32_t *end;
	ຈຳນວນເຕັມ oldstroffset, newstroffset;
	uint32_t tag;
	ຈຳນວນເຕັມ offset, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	end = fdt_grab_space_(fdt, parimaa_na(*end));
	ຖ້າ (! end)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_size_dt_strings(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_size_dt_strings(fdt));
	fdt_set_off_dt_strings(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	ໃນຂະນະທີ່ ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		ຖ້າ (tag == FDT_PROP) {
			sa_nracanaa fdt_property *prop =
				fdt_offset_ptr_w_(fdt, offset);
			ຈຳນວນເຕັມ nameoff;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += fdt_size_dt_strings(fdt);
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}
	ຖ້າ (nextoffset < 0)
		ສົ່ງຄືນ nextoffset;

	/* Finally, adjust the header */
	fdt_set_totalsize(fdt, newstroffset + fdt_size_dt_strings(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_set_last_comp_version(fdt, FDT_LAST_COMPATIBLE_VERSION);
	fdt_set_magic(fdt, FDT_MAGIC);

	ສົ່ງຄືນ 0;
}
