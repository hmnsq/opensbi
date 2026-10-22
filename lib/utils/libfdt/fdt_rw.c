// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static số_nguyên fdt_blocks_misordered_(const void *fdt,
				  số_nguyên mem_rsv_size, số_nguyên struct_size)
{
	trả_về (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(sizeof(struct fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static số_nguyên fdt_rw_probe_(void *fdt)
{
	nếu (can_assume(VALID_DTB))
		trả_về 0;
	FDT_RO_PROBE(fdt);

	nếu (!can_assume(LATEST) && fdt_version(fdt) < 17)
		trả_về -FDT_ERR_BADVERSION;
	nếu (fdt_blocks_misordered_(fdt, sizeof(struct fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		trả_về -FDT_ERR_BADLAYOUT;
	nếu (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	trả_về 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		số_nguyên err_; \
		nếu ((err_ = fdt_rw_probe_(fdt)) != 0) \
			trả_về err_; \
	}

static inline unsigned số_nguyên fdt_data_size_(void *fdt)
{
	trả_về fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static số_nguyên fdt_splice_(void *fdt, void *splicepoint, số_nguyên oldlen, số_nguyên newlen)
{
	char *p = splicepoint;
	unsigned số_nguyên dsize = fdt_data_size_(fdt);
	size_t soff = p - (char *)fdt;

	nếu ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		trả_về -FDT_ERR_BADOFFSET;
	nếu ((p < (char *)fdt) || (dsize + newlen < (unsigned)oldlen))
		trả_về -FDT_ERR_BADOFFSET;
	nếu (dsize - oldlen + newlen > fdt_totalsize(fdt))
		trả_về -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((char *)fdt + dsize) - (p + oldlen));
	trả_về 0;
}

static số_nguyên fdt_splice_mem_rsv_(void *fdt, struct fdt_reserve_entry *p,
			       số_nguyên oldn, số_nguyên newn)
{
	số_nguyên delta = (newn - oldn) * sizeof(*p);
	số_nguyên err;
	err = fdt_splice_(fdt, p, oldn * sizeof(*p), newn * sizeof(*p));
	nếu (err)
		trả_về err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	trả_về 0;
}

static số_nguyên fdt_splice_struct_(void *fdt, void *p,
			      số_nguyên oldlen, số_nguyên newlen)
{
	số_nguyên delta = newlen - oldlen;
	số_nguyên err;

	nếu ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		trả_về err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	trả_về 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	số_nguyên newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static số_nguyên fdt_splice_string_(void *fdt, số_nguyên newlen)
{
	void *p = (char *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	số_nguyên err;

	nếu ((err = fdt_splice_(fdt, p, 0, newlen)))
		trả_về err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	trả_về 0;
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
static số_nguyên fdt_find_add_string_(void *fdt, const char *s, số_nguyên *allocated)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	số_nguyên len = strlen(s) + 1;
	số_nguyên err;

	nếu (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	nếu (p)
		/* found it */
		trả_về (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	nếu (err)
		trả_về err;

	nếu (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	trả_về (new - strtab);
}

số_nguyên fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	struct fdt_reserve_entry *re;
	số_nguyên err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	nếu (err)
		trả_về err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	trả_về 0;
}

số_nguyên fdt_del_mem_rsv(void *fdt, số_nguyên n)
{
	struct fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	nếu (n >= fdt_num_mem_rsv(fdt))
		trả_về -FDT_ERR_NOTFOUND;

	trả_về fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static số_nguyên fdt_resize_property_(void *fdt, số_nguyên nodeoffset, const char *name,
				số_nguyên len, struct fdt_property **prop)
{
	số_nguyên oldlen;
	số_nguyên err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	nếu (!*prop)
		trả_về oldlen;

	nếu ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		trả_về err;

	(*prop)->len = cpu_to_fdt32(len);
	trả_về 0;
}

static số_nguyên fdt_add_property_(void *fdt, số_nguyên nodeoffset, const char *name,
			     số_nguyên len, struct fdt_property **prop)
{
	số_nguyên proplen;
	số_nguyên nextoffset;
	số_nguyên namestroff;
	số_nguyên err;
	số_nguyên allocated;

	nếu ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		trả_về nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	nếu (namestroff < 0)
		trả_về namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = sizeof(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	nếu (err) {
		/* Delete the string if we failed to add it */
		nếu (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		trả_về err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	trả_về 0;
}

số_nguyên fdt_set_name(void *fdt, số_nguyên nodeoffset, const char *name)
{
	char *namep;
	số_nguyên oldlen, newlen;
	số_nguyên err;

	FDT_RW_PROBE(fdt);

	namep = (char *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	nếu (!namep)
		trả_về oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	nếu (err)
		trả_về err;

	memcpy(namep, name, newlen+1);
	trả_về 0;
}

số_nguyên fdt_setprop_placeholder(void *fdt, số_nguyên nodeoffset, const char *name,
			    số_nguyên len, void **prop_data)
{
	struct fdt_property *prop;
	số_nguyên err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	nếu (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	nếu (err)
		trả_về err;

	*prop_data = prop->data;
	trả_về 0;
}

số_nguyên fdt_setprop(void *fdt, số_nguyên nodeoffset, const char *name,
		const void *val, số_nguyên len)
{
	void *prop_data;
	số_nguyên err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	nếu (err)
		trả_về err;

	nếu (len)
		memcpy(prop_data, val, len);
	trả_về 0;
}

số_nguyên fdt_appendprop(void *fdt, số_nguyên nodeoffset, const char *name,
		   const void *val, số_nguyên len)
{
	struct fdt_property *prop;
	số_nguyên err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	nếu (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		nếu (err)
			trả_về err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} không_thì {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		nếu (err)
			trả_về err;
		memcpy(prop->data, val, len);
	}
	trả_về 0;
}

số_nguyên fdt_delprop(void *fdt, số_nguyên nodeoffset, const char *name)
{
	struct fdt_property *prop;
	số_nguyên len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	nếu (!prop)
		trả_về len;

	proplen = sizeof(*prop) + FDT_TAGALIGN(len);
	trả_về fdt_splice_struct_(fdt, prop, proplen, 0);
}

số_nguyên fdt_add_subnode_namelen(void *fdt, số_nguyên parentoffset,
			    const char *name, số_nguyên namelen)
{
	struct fdt_node_header *nh;
	số_nguyên offset, nextoffset;
	số_nguyên nodelen;
	số_nguyên err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	nếu (offset >= 0)
		trả_về -FDT_ERR_EXISTS;
	không_thì nếu (offset != -FDT_ERR_NOTFOUND)
		trả_về offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	nếu (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		trả_về -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} trong_khi ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = sizeof(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	nếu (err)
		trả_về err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((char *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	trả_về offset;
}

số_nguyên fdt_add_subnode(void *fdt, số_nguyên parentoffset, const char *name)
{
	trả_về fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

số_nguyên fdt_del_node(void *fdt, số_nguyên nodeoffset)
{
	số_nguyên endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	nếu (endoffset < 0)
		trả_về endoffset;

	trả_về fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const char *old, char *new,
			    số_nguyên mem_rsv_size,
			    số_nguyên struct_size,
			    số_nguyên strings_size)
{
	số_nguyên mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(sizeof(struct fdt_header), 8);
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

số_nguyên fdt_open_into(const void *fdt, void *buf, số_nguyên bufsize)
{
	số_nguyên err;
	số_nguyên mem_rsv_size, struct_size;
	số_nguyên newsize;
	const char *fdtstart = fdt;
	const char *fdtend = fdtstart + fdt_totalsize(fdt);
	char *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);

	nếu (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} không_thì nếu (fdt_version(fdt) == 16) {
		struct_size = 0;
		trong_khi (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		nếu (struct_size < 0)
			trả_về struct_size;
	} không_thì {
		trả_về -FDT_ERR_BADVERSION;
	}

	nếu (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		nếu (err)
			trả_về err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		trả_về 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(sizeof(struct fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	nếu (bufsize < newsize)
		trả_về -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	nếu (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (char *)(uintptr_t)fdtend;
		nếu ((tmp + newsize) > ((char *)buf + bufsize))
			trả_về -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	trả_về 0;
}

số_nguyên fdt_pack(void *fdt)
{
	số_nguyên mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	trả_về 0;
}
