// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ཧྲིལ fdt_blocks_misordered_(const void *fdt,
				  ཧྲིལ mem_rsv_size, ཧྲིལ struct_size)
{
	ལོག (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static ཧྲིལ fdt_rw_probe_(void *fdt)
{
	གལ་སྲིད (can_assume(VALID_DTB))
		ལོག 0;
	FDT_RO_PROBE(fdt);

	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ལོག -FDT_ERR_BADVERSION;
	གལ་སྲིད (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ལོག -FDT_ERR_BADLAYOUT;
	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ལོག 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ཧྲིལ err_; \
		གལ་སྲིད ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ལོག err_; \
	}

static anta_hstha unsigned ཧྲིལ fdt_data_size_(void *fdt)
{
	ལོག fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static ཧྲིལ fdt_splice_(void *fdt, void *splicepoint, ཧྲིལ oldlen, ཧྲིལ newlen)
{
	var_na *p = splicepoint;
	unsigned ཧྲིལ dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	གལ་སྲིད ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ལོག -FDT_ERR_BADOFFSET;
	གལ་སྲིད ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ལོག -FDT_ERR_BADOFFSET;
	གལ་སྲིད (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ལོག -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ལོག 0;
}

static ཧྲིལ fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       ཧྲིལ oldn, ཧྲིལ newn)
{
	ཧྲིལ delta = (newn - oldn) * parimaa_na(*p);
	ཧྲིལ err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	གལ་སྲིད (err)
		ལོག err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ལོག 0;
}

static ཧྲིལ fdt_splice_struct_(void *fdt, void *p,
			      ཧྲིལ oldlen, ཧྲིལ newlen)
{
	ཧྲིལ delta = newlen - oldlen;
	ཧྲིལ err;

	གལ་སྲིད ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ལོག err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ལོག 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ཧྲིལ newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static ཧྲིལ fdt_splice_string_(void *fdt, ཧྲིལ newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ཧྲིལ err;

	གལ་སྲིད ((err = fdt_splice_(fdt, p, 0, newlen)))
		ལོག err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ལོག 0;
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
static ཧྲིལ fdt_find_add_string_(void *fdt, const var_na *s, ཧྲིལ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	ཧྲིལ len = strlen(s) + 1;
	ཧྲིལ err;

	གལ་སྲིད (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	གལ་སྲིད (p)
		/* found it */
		ལོག (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	གལ་སྲིད (err)
		ལོག err;

	གལ་སྲིད (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ལོག (new - strtab);
}

ཧྲིལ fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ཧྲིལ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	གལ་སྲིད (err)
		ལོག err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ལོག 0;
}

ཧྲིལ fdt_del_mem_rsv(void *fdt, ཧྲིལ n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	གལ་སྲིད (n >= fdt_num_mem_rsv(fdt))
		ལོག -FDT_ERR_NOTFOUND;

	ལོག fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static ཧྲིལ fdt_resize_property_(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
				ཧྲིལ len, sa_nracanaa fdt_property **prop)
{
	ཧྲིལ oldlen;
	ཧྲིལ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	གལ་སྲིད (!*prop)
		ལོག oldlen;

	གལ་སྲིད ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ལོག err;

	(*prop)->len = cpu_to_fdt32(len);
	ལོག 0;
}

static ཧྲིལ fdt_add_property_(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
			     ཧྲིལ len, sa_nracanaa fdt_property **prop)
{
	ཧྲིལ proplen;
	ཧྲིལ nextoffset;
	ཧྲིལ namestroff;
	ཧྲིལ err;
	ཧྲིལ allocated;

	གལ་སྲིད ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ལོག nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	གལ་སྲིད (namestroff < 0)
		ལོག namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	གལ་སྲིད (err) {
		/* Delete the string if we failed to add it */
		གལ་སྲིད (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ལོག err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ལོག 0;
}

ཧྲིལ fdt_set_name(void *fdt, ཧྲིལ nodeoffset, const var_na *name)
{
	var_na *namep;
	ཧྲིལ oldlen, newlen;
	ཧྲིལ err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	གལ་སྲིད (!namep)
		ལོག oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	གལ་སྲིད (err)
		ལོག err;

	memcpy(namep, name, newlen+1);
	ལོག 0;
}

ཧྲིལ fdt_setprop_placeholder(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
			    ཧྲིལ len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	ཧྲིལ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	གལ་སྲིད (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	གལ་སྲིད (err)
		ལོག err;

	*prop_data = prop->data;
	ལོག 0;
}

ཧྲིལ fdt_setprop(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
		const void *val, ཧྲིལ len)
{
	void *prop_data;
	ཧྲིལ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	གལ་སྲིད (err)
		ལོག err;

	གལ་སྲིད (len)
		memcpy(prop_data, val, len);
	ལོག 0;
}

ཧྲིལ fdt_appendprop(void *fdt, ཧྲིལ nodeoffset, const var_na *name,
		   const void *val, ཧྲིལ len)
{
	sa_nracanaa fdt_property *prop;
	ཧྲིལ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	གལ་སྲིད (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		གལ་སྲིད (err)
			ལོག err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} དེ་མིན {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		གལ་སྲིད (err)
			ལོག err;
		memcpy(prop->data, val, len);
	}
	ལོག 0;
}

ཧྲིལ fdt_delprop(void *fdt, ཧྲིལ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ཧྲིལ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	གལ་སྲིད (!prop)
		ལོག len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ལོག fdt_splice_struct_(fdt, prop, proplen, 0);
}

ཧྲིལ fdt_add_subnode_namelen(void *fdt, ཧྲིལ parentoffset,
			    const var_na *name, ཧྲིལ namelen)
{
	sa_nracanaa fdt_node_header *nh;
	ཧྲིལ offset, nextoffset;
	ཧྲིལ nodelen;
	ཧྲིལ err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	གལ་སྲིད (offset >= 0)
		ལོག -FDT_ERR_EXISTS;
	དེ་མིན གལ་སྲིད (offset != -FDT_ERR_NOTFOUND)
		ལོག offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	གལ་སྲིད (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ལོག -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} སྐབས ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	གལ་སྲིད (err)
		ལོག err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ལོག offset;
}

ཧྲིལ fdt_add_subnode(void *fdt, ཧྲིལ parentoffset, const var_na *name)
{
	ལོག fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ཧྲིལ fdt_del_node(void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	གལ་སྲིད (endoffset < 0)
		ལོག endoffset;

	ལོག fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    ཧྲིལ mem_rsv_size,
			    ཧྲིལ struct_size,
			    ཧྲིལ strings_size)
{
	ཧྲིལ mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8);
	struct_off = mem_rsv_off + mem_rsv_size;
	strings_off = struct_off + struct_size;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_size);
	fdt_set_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_size);
	fdt_set_off_dt_struct(new, struct_off);
	fdt_set_size_dt_struct(new, struct_size);

	memmove(new + strings_off, old + fdt_off_dt_strings(old), strings_size);
	fdt_set_off_dt_strings(new, strings_off);
	fdt_set_size_dt_strings(new, fdt_size_dt_strings(old));
}

ཧྲིལ fdt_open_into(const void *fdt, void *buf, ཧྲིལ bufsize)
{
	ཧྲིལ err;
	ཧྲིལ mem_rsv_size, struct_size;
	ཧྲིལ newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	གལ་སྲིད (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} དེ་མིན གལ་སྲིད (fdt_version(fdt) == 16) {
		struct_size = 0;
		སྐབས (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		གལ་སྲིད (struct_size < 0)
			ལོག struct_size;
	} དེ་མིན {
		ལོག -FDT_ERR_BADVERSION;
	}

	གལ་སྲིད (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		གལ་སྲིད (err)
			ལོག err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ལོག 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	གལ་སྲིད (bufsize < newsize)
		ལོག -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	གལ་སྲིད (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		གལ་སྲིད ((tmp + newsize) > ((var_na *)buf + bufsize))
			ལོག -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ལོག 0;
}

ཧྲིལ fdt_pack(void *fdt)
{
	ཧྲིལ mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ལོག 0;
}
