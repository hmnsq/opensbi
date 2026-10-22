// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static 정수 fdt_blocks_misordered_(const void *fdt,
				  정수 mem_rsv_size, 정수 struct_size)
{
	반환 (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(sizeof(struct fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static 정수 fdt_rw_probe_(void *fdt)
{
	만약 (can_assume(VALID_DTB))
		반환 0;
	FDT_RO_PROBE(fdt);

	만약 (!can_assume(LATEST) && fdt_version(fdt) < 17)
		반환 -FDT_ERR_BADVERSION;
	만약 (fdt_blocks_misordered_(fdt, sizeof(struct fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		반환 -FDT_ERR_BADLAYOUT;
	만약 (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	반환 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		정수 err_; \
		만약 ((err_ = fdt_rw_probe_(fdt)) != 0) \
			반환 err_; \
	}

static inline unsigned 정수 fdt_data_size_(void *fdt)
{
	반환 fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static 정수 fdt_splice_(void *fdt, void *splicepoint, 정수 oldlen, 정수 newlen)
{
	char *p = splicepoint;
	unsigned 정수 dsize = fdt_data_size_(fdt);
	size_t soff = p - (char *)fdt;

	만약 ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		반환 -FDT_ERR_BADOFFSET;
	만약 ((p < (char *)fdt) || (dsize + newlen < (unsigned)oldlen))
		반환 -FDT_ERR_BADOFFSET;
	만약 (dsize - oldlen + newlen > fdt_totalsize(fdt))
		반환 -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((char *)fdt + dsize) - (p + oldlen));
	반환 0;
}

static 정수 fdt_splice_mem_rsv_(void *fdt, struct fdt_reserve_entry *p,
			       정수 oldn, 정수 newn)
{
	정수 delta = (newn - oldn) * sizeof(*p);
	정수 err;
	err = fdt_splice_(fdt, p, oldn * sizeof(*p), newn * sizeof(*p));
	만약 (err)
		반환 err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	반환 0;
}

static 정수 fdt_splice_struct_(void *fdt, void *p,
			      정수 oldlen, 정수 newlen)
{
	정수 delta = newlen - oldlen;
	정수 err;

	만약 ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		반환 err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	반환 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	정수 newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static 정수 fdt_splice_string_(void *fdt, 정수 newlen)
{
	void *p = (char *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	정수 err;

	만약 ((err = fdt_splice_(fdt, p, 0, newlen)))
		반환 err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	반환 0;
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
static 정수 fdt_find_add_string_(void *fdt, const char *s, 정수 *allocated)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	정수 len = strlen(s) + 1;
	정수 err;

	만약 (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	만약 (p)
		/* found it */
		반환 (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	만약 (err)
		반환 err;

	만약 (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	반환 (new - strtab);
}

정수 fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	struct fdt_reserve_entry *re;
	정수 err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	만약 (err)
		반환 err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	반환 0;
}

정수 fdt_del_mem_rsv(void *fdt, 정수 n)
{
	struct fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	만약 (n >= fdt_num_mem_rsv(fdt))
		반환 -FDT_ERR_NOTFOUND;

	반환 fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static 정수 fdt_resize_property_(void *fdt, 정수 nodeoffset, const char *name,
				정수 len, struct fdt_property **prop)
{
	정수 oldlen;
	정수 err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	만약 (!*prop)
		반환 oldlen;

	만약 ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		반환 err;

	(*prop)->len = cpu_to_fdt32(len);
	반환 0;
}

static 정수 fdt_add_property_(void *fdt, 정수 nodeoffset, const char *name,
			     정수 len, struct fdt_property **prop)
{
	정수 proplen;
	정수 nextoffset;
	정수 namestroff;
	정수 err;
	정수 allocated;

	만약 ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		반환 nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	만약 (namestroff < 0)
		반환 namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = sizeof(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	만약 (err) {
		/* Delete the string if we failed to add it */
		만약 (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		반환 err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	반환 0;
}

정수 fdt_set_name(void *fdt, 정수 nodeoffset, const char *name)
{
	char *namep;
	정수 oldlen, newlen;
	정수 err;

	FDT_RW_PROBE(fdt);

	namep = (char *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	만약 (!namep)
		반환 oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	만약 (err)
		반환 err;

	memcpy(namep, name, newlen+1);
	반환 0;
}

정수 fdt_setprop_placeholder(void *fdt, 정수 nodeoffset, const char *name,
			    정수 len, void **prop_data)
{
	struct fdt_property *prop;
	정수 err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	만약 (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	만약 (err)
		반환 err;

	*prop_data = prop->data;
	반환 0;
}

정수 fdt_setprop(void *fdt, 정수 nodeoffset, const char *name,
		const void *val, 정수 len)
{
	void *prop_data;
	정수 err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	만약 (err)
		반환 err;

	만약 (len)
		memcpy(prop_data, val, len);
	반환 0;
}

정수 fdt_appendprop(void *fdt, 정수 nodeoffset, const char *name,
		   const void *val, 정수 len)
{
	struct fdt_property *prop;
	정수 err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	만약 (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		만약 (err)
			반환 err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} 아니면 {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		만약 (err)
			반환 err;
		memcpy(prop->data, val, len);
	}
	반환 0;
}

정수 fdt_delprop(void *fdt, 정수 nodeoffset, const char *name)
{
	struct fdt_property *prop;
	정수 len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	만약 (!prop)
		반환 len;

	proplen = sizeof(*prop) + FDT_TAGALIGN(len);
	반환 fdt_splice_struct_(fdt, prop, proplen, 0);
}

정수 fdt_add_subnode_namelen(void *fdt, 정수 parentoffset,
			    const char *name, 정수 namelen)
{
	struct fdt_node_header *nh;
	정수 offset, nextoffset;
	정수 nodelen;
	정수 err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	만약 (offset >= 0)
		반환 -FDT_ERR_EXISTS;
	아니면 만약 (offset != -FDT_ERR_NOTFOUND)
		반환 offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	만약 (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		반환 -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} 동안 ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = sizeof(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	만약 (err)
		반환 err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((char *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	반환 offset;
}

정수 fdt_add_subnode(void *fdt, 정수 parentoffset, const char *name)
{
	반환 fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

정수 fdt_del_node(void *fdt, 정수 nodeoffset)
{
	정수 endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	만약 (endoffset < 0)
		반환 endoffset;

	반환 fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const char *old, char *new,
			    정수 mem_rsv_size,
			    정수 struct_size,
			    정수 strings_size)
{
	정수 mem_rsv_off, struct_off, strings_off;

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

정수 fdt_open_into(const void *fdt, void *buf, 정수 bufsize)
{
	정수 err;
	정수 mem_rsv_size, struct_size;
	정수 newsize;
	const char *fdtstart = fdt;
	const char *fdtend = fdtstart + fdt_totalsize(fdt);
	char *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);

	만약 (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} 아니면 만약 (fdt_version(fdt) == 16) {
		struct_size = 0;
		동안 (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		만약 (struct_size < 0)
			반환 struct_size;
	} 아니면 {
		반환 -FDT_ERR_BADVERSION;
	}

	만약 (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		만약 (err)
			반환 err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		반환 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(sizeof(struct fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	만약 (bufsize < newsize)
		반환 -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	만약 (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (char *)(uintptr_t)fdtend;
		만약 ((tmp + newsize) > ((char *)buf + bufsize))
			반환 -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	반환 0;
}

정수 fdt_pack(void *fdt)
{
	정수 mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	반환 0;
}
