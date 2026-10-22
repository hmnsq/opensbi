// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static цео fdt_blocks_misordered_(const void *fdt,
				  цео mem_rsv_size, цео struct_size)
{
	врати (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(sizeof(struct fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static цео fdt_rw_probe_(void *fdt)
{
	ако (can_assume(VALID_DTB))
		врати 0;
	FDT_RO_PROBE(fdt);

	ако (!can_assume(LATEST) && fdt_version(fdt) < 17)
		врати -FDT_ERR_BADVERSION;
	ако (fdt_blocks_misordered_(fdt, sizeof(struct fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		врати -FDT_ERR_BADLAYOUT;
	ако (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	врати 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		цео err_; \
		ако ((err_ = fdt_rw_probe_(fdt)) != 0) \
			врати err_; \
	}

static inline unsigned цео fdt_data_size_(void *fdt)
{
	врати fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static цео fdt_splice_(void *fdt, void *splicepoint, цео oldlen, цео newlen)
{
	char *p = splicepoint;
	unsigned цео dsize = fdt_data_size_(fdt);
	size_t soff = p - (char *)fdt;

	ако ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		врати -FDT_ERR_BADOFFSET;
	ако ((p < (char *)fdt) || (dsize + newlen < (unsigned)oldlen))
		врати -FDT_ERR_BADOFFSET;
	ако (dsize - oldlen + newlen > fdt_totalsize(fdt))
		врати -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((char *)fdt + dsize) - (p + oldlen));
	врати 0;
}

static цео fdt_splice_mem_rsv_(void *fdt, struct fdt_reserve_entry *p,
			       цео oldn, цео newn)
{
	цео delta = (newn - oldn) * sizeof(*p);
	цео err;
	err = fdt_splice_(fdt, p, oldn * sizeof(*p), newn * sizeof(*p));
	ако (err)
		врати err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	врати 0;
}

static цео fdt_splice_struct_(void *fdt, void *p,
			      цео oldlen, цео newlen)
{
	цео delta = newlen - oldlen;
	цео err;

	ако ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		врати err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	врати 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	цео newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static цео fdt_splice_string_(void *fdt, цео newlen)
{
	void *p = (char *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	цео err;

	ако ((err = fdt_splice_(fdt, p, 0, newlen)))
		врати err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	врати 0;
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
static цео fdt_find_add_string_(void *fdt, const char *s, цео *allocated)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	цео len = strlen(s) + 1;
	цео err;

	ако (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ако (p)
		/* found it */
		врати (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ако (err)
		врати err;

	ако (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	врати (new - strtab);
}

цео fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	struct fdt_reserve_entry *re;
	цео err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ако (err)
		врати err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	врати 0;
}

цео fdt_del_mem_rsv(void *fdt, цео n)
{
	struct fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ако (n >= fdt_num_mem_rsv(fdt))
		врати -FDT_ERR_NOTFOUND;

	врати fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static цео fdt_resize_property_(void *fdt, цео nodeoffset, const char *name,
				цео len, struct fdt_property **prop)
{
	цео oldlen;
	цео err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ако (!*prop)
		врати oldlen;

	ако ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		врати err;

	(*prop)->len = cpu_to_fdt32(len);
	врати 0;
}

static цео fdt_add_property_(void *fdt, цео nodeoffset, const char *name,
			     цео len, struct fdt_property **prop)
{
	цео proplen;
	цео nextoffset;
	цео namestroff;
	цео err;
	цео allocated;

	ако ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		врати nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ако (namestroff < 0)
		врати namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = sizeof(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ако (err) {
		/* Delete the string if we failed to add it */
		ако (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		врати err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	врати 0;
}

цео fdt_set_name(void *fdt, цео nodeoffset, const char *name)
{
	char *namep;
	цео oldlen, newlen;
	цео err;

	FDT_RW_PROBE(fdt);

	namep = (char *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ако (!namep)
		врати oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ако (err)
		врати err;

	memcpy(namep, name, newlen+1);
	врати 0;
}

цео fdt_setprop_placeholder(void *fdt, цео nodeoffset, const char *name,
			    цео len, void **prop_data)
{
	struct fdt_property *prop;
	цео err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ако (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ако (err)
		врати err;

	*prop_data = prop->data;
	врати 0;
}

цео fdt_setprop(void *fdt, цео nodeoffset, const char *name,
		const void *val, цео len)
{
	void *prop_data;
	цео err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ако (err)
		врати err;

	ако (len)
		memcpy(prop_data, val, len);
	врати 0;
}

цео fdt_appendprop(void *fdt, цео nodeoffset, const char *name,
		   const void *val, цео len)
{
	struct fdt_property *prop;
	цео err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ако (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ако (err)
			врати err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} инако {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ако (err)
			врати err;
		memcpy(prop->data, val, len);
	}
	врати 0;
}

цео fdt_delprop(void *fdt, цео nodeoffset, const char *name)
{
	struct fdt_property *prop;
	цео len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ако (!prop)
		врати len;

	proplen = sizeof(*prop) + FDT_TAGALIGN(len);
	врати fdt_splice_struct_(fdt, prop, proplen, 0);
}

цео fdt_add_subnode_namelen(void *fdt, цео parentoffset,
			    const char *name, цео namelen)
{
	struct fdt_node_header *nh;
	цео offset, nextoffset;
	цео nodelen;
	цео err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ако (offset >= 0)
		врати -FDT_ERR_EXISTS;
	инако ако (offset != -FDT_ERR_NOTFOUND)
		врати offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ако (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		врати -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} док ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = sizeof(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ако (err)
		врати err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((char *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	врати offset;
}

цео fdt_add_subnode(void *fdt, цео parentoffset, const char *name)
{
	врати fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

цео fdt_del_node(void *fdt, цео nodeoffset)
{
	цео endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ако (endoffset < 0)
		врати endoffset;

	врати fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const char *old, char *new,
			    цео mem_rsv_size,
			    цео struct_size,
			    цео strings_size)
{
	цео mem_rsv_off, struct_off, strings_off;

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

цео fdt_open_into(const void *fdt, void *buf, цео bufsize)
{
	цео err;
	цео mem_rsv_size, struct_size;
	цео newsize;
	const char *fdtstart = fdt;
	const char *fdtend = fdtstart + fdt_totalsize(fdt);
	char *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);

	ако (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} инако ако (fdt_version(fdt) == 16) {
		struct_size = 0;
		док (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ако (struct_size < 0)
			врати struct_size;
	} инако {
		врати -FDT_ERR_BADVERSION;
	}

	ако (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ако (err)
			врати err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		врати 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(sizeof(struct fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ако (bufsize < newsize)
		врати -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ако (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (char *)(uintptr_t)fdtend;
		ако ((tmp + newsize) > ((char *)buf + bufsize))
			врати -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	врати 0;
}

цео fdt_pack(void *fdt)
{
	цео mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	врати 0;
}
