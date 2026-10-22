// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ціле fdt_blocks_misordered_(const void *fdt,
				  ціле mem_rsv_size, ціле struct_size)
{
	повернути (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(sizeof(struct fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static ціле fdt_rw_probe_(void *fdt)
{
	якщо (can_assume(VALID_DTB))
		повернути 0;
	FDT_RO_PROBE(fdt);

	якщо (!can_assume(LATEST) && fdt_version(fdt) < 17)
		повернути -FDT_ERR_BADVERSION;
	якщо (fdt_blocks_misordered_(fdt, sizeof(struct fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		повернути -FDT_ERR_BADLAYOUT;
	якщо (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	повернути 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ціле err_; \
		якщо ((err_ = fdt_rw_probe_(fdt)) != 0) \
			повернути err_; \
	}

static inline unsigned ціле fdt_data_size_(void *fdt)
{
	повернути fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static ціле fdt_splice_(void *fdt, void *splicepoint, ціле oldlen, ціле newlen)
{
	char *p = splicepoint;
	unsigned ціле dsize = fdt_data_size_(fdt);
	size_t soff = p - (char *)fdt;

	якщо ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		повернути -FDT_ERR_BADOFFSET;
	якщо ((p < (char *)fdt) || (dsize + newlen < (unsigned)oldlen))
		повернути -FDT_ERR_BADOFFSET;
	якщо (dsize - oldlen + newlen > fdt_totalsize(fdt))
		повернути -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((char *)fdt + dsize) - (p + oldlen));
	повернути 0;
}

static ціле fdt_splice_mem_rsv_(void *fdt, struct fdt_reserve_entry *p,
			       ціле oldn, ціле newn)
{
	ціле delta = (newn - oldn) * sizeof(*p);
	ціле err;
	err = fdt_splice_(fdt, p, oldn * sizeof(*p), newn * sizeof(*p));
	якщо (err)
		повернути err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	повернути 0;
}

static ціле fdt_splice_struct_(void *fdt, void *p,
			      ціле oldlen, ціле newlen)
{
	ціле delta = newlen - oldlen;
	ціле err;

	якщо ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		повернути err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	повернути 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	ціле newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static ціле fdt_splice_string_(void *fdt, ціле newlen)
{
	void *p = (char *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ціле err;

	якщо ((err = fdt_splice_(fdt, p, 0, newlen)))
		повернути err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	повернути 0;
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
static ціле fdt_find_add_string_(void *fdt, const char *s, ціле *allocated)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	ціле len = strlen(s) + 1;
	ціле err;

	якщо (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	якщо (p)
		/* found it */
		повернути (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	якщо (err)
		повернути err;

	якщо (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	повернути (new - strtab);
}

ціле fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	struct fdt_reserve_entry *re;
	ціле err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	якщо (err)
		повернути err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	повернути 0;
}

ціле fdt_del_mem_rsv(void *fdt, ціле n)
{
	struct fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	якщо (n >= fdt_num_mem_rsv(fdt))
		повернути -FDT_ERR_NOTFOUND;

	повернути fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static ціле fdt_resize_property_(void *fdt, ціле nodeoffset, const char *name,
				ціле len, struct fdt_property **prop)
{
	ціле oldlen;
	ціле err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	якщо (!*prop)
		повернути oldlen;

	якщо ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		повернути err;

	(*prop)->len = cpu_to_fdt32(len);
	повернути 0;
}

static ціле fdt_add_property_(void *fdt, ціле nodeoffset, const char *name,
			     ціле len, struct fdt_property **prop)
{
	ціле proplen;
	ціле nextoffset;
	ціле namestroff;
	ціле err;
	ціле allocated;

	якщо ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		повернути nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	якщо (namestroff < 0)
		повернути namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = sizeof(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	якщо (err) {
		/* Delete the string if we failed to add it */
		якщо (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		повернути err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	повернути 0;
}

ціле fdt_set_name(void *fdt, ціле nodeoffset, const char *name)
{
	char *namep;
	ціле oldlen, newlen;
	ціле err;

	FDT_RW_PROBE(fdt);

	namep = (char *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	якщо (!namep)
		повернути oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	якщо (err)
		повернути err;

	memcpy(namep, name, newlen+1);
	повернути 0;
}

ціле fdt_setprop_placeholder(void *fdt, ціле nodeoffset, const char *name,
			    ціле len, void **prop_data)
{
	struct fdt_property *prop;
	ціле err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	якщо (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	якщо (err)
		повернути err;

	*prop_data = prop->data;
	повернути 0;
}

ціле fdt_setprop(void *fdt, ціле nodeoffset, const char *name,
		const void *val, ціле len)
{
	void *prop_data;
	ціле err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	якщо (err)
		повернути err;

	якщо (len)
		memcpy(prop_data, val, len);
	повернути 0;
}

ціле fdt_appendprop(void *fdt, ціле nodeoffset, const char *name,
		   const void *val, ціле len)
{
	struct fdt_property *prop;
	ціле err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	якщо (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		якщо (err)
			повернути err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} інакше {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		якщо (err)
			повернути err;
		memcpy(prop->data, val, len);
	}
	повернути 0;
}

ціле fdt_delprop(void *fdt, ціле nodeoffset, const char *name)
{
	struct fdt_property *prop;
	ціле len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	якщо (!prop)
		повернути len;

	proplen = sizeof(*prop) + FDT_TAGALIGN(len);
	повернути fdt_splice_struct_(fdt, prop, proplen, 0);
}

ціле fdt_add_subnode_namelen(void *fdt, ціле parentoffset,
			    const char *name, ціле namelen)
{
	struct fdt_node_header *nh;
	ціле offset, nextoffset;
	ціле nodelen;
	ціле err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	якщо (offset >= 0)
		повернути -FDT_ERR_EXISTS;
	інакше якщо (offset != -FDT_ERR_NOTFOUND)
		повернути offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	якщо (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		повернути -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} поки ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = sizeof(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	якщо (err)
		повернути err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((char *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	повернути offset;
}

ціле fdt_add_subnode(void *fdt, ціле parentoffset, const char *name)
{
	повернути fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ціле fdt_del_node(void *fdt, ціле nodeoffset)
{
	ціле endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	якщо (endoffset < 0)
		повернути endoffset;

	повернути fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const char *old, char *new,
			    ціле mem_rsv_size,
			    ціле struct_size,
			    ціле strings_size)
{
	ціле mem_rsv_off, struct_off, strings_off;

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

ціле fdt_open_into(const void *fdt, void *buf, ціле bufsize)
{
	ціле err;
	ціле mem_rsv_size, struct_size;
	ціле newsize;
	const char *fdtstart = fdt;
	const char *fdtend = fdtstart + fdt_totalsize(fdt);
	char *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);

	якщо (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} інакше якщо (fdt_version(fdt) == 16) {
		struct_size = 0;
		поки (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		якщо (struct_size < 0)
			повернути struct_size;
	} інакше {
		повернути -FDT_ERR_BADVERSION;
	}

	якщо (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		якщо (err)
			повернути err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		повернути 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(sizeof(struct fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	якщо (bufsize < newsize)
		повернути -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	якщо (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (char *)(uintptr_t)fdtend;
		якщо ((tmp + newsize) > ((char *)buf + bufsize))
			повернути -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	повернути 0;
}

ціле fdt_pack(void *fdt)
{
	ціле mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	повернути 0;
}
