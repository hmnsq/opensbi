// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static целое fdt_blocks_misordered_(const void *fdt,
				  целое mem_rsv_size, целое struct_size)
{
	вернуть (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(sizeof(struct fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static целое fdt_rw_probe_(void *fdt)
{
	если (can_assume(VALID_DTB))
		вернуть 0;
	FDT_RO_PROBE(fdt);

	если (!can_assume(LATEST) && fdt_version(fdt) < 17)
		вернуть -FDT_ERR_BADVERSION;
	если (fdt_blocks_misordered_(fdt, sizeof(struct fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		вернуть -FDT_ERR_BADLAYOUT;
	если (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	вернуть 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		целое err_; \
		если ((err_ = fdt_rw_probe_(fdt)) != 0) \
			вернуть err_; \
	}

static inline unsigned целое fdt_data_size_(void *fdt)
{
	вернуть fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static целое fdt_splice_(void *fdt, void *splicepoint, целое oldlen, целое newlen)
{
	char *p = splicepoint;
	unsigned целое dsize = fdt_data_size_(fdt);
	size_t soff = p - (char *)fdt;

	если ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		вернуть -FDT_ERR_BADOFFSET;
	если ((p < (char *)fdt) || (dsize + newlen < (unsigned)oldlen))
		вернуть -FDT_ERR_BADOFFSET;
	если (dsize - oldlen + newlen > fdt_totalsize(fdt))
		вернуть -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((char *)fdt + dsize) - (p + oldlen));
	вернуть 0;
}

static целое fdt_splice_mem_rsv_(void *fdt, struct fdt_reserve_entry *p,
			       целое oldn, целое newn)
{
	целое delta = (newn - oldn) * sizeof(*p);
	целое err;
	err = fdt_splice_(fdt, p, oldn * sizeof(*p), newn * sizeof(*p));
	если (err)
		вернуть err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	вернуть 0;
}

static целое fdt_splice_struct_(void *fdt, void *p,
			      целое oldlen, целое newlen)
{
	целое delta = newlen - oldlen;
	целое err;

	если ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		вернуть err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	вернуть 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const char *s)
{
	целое newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static целое fdt_splice_string_(void *fdt, целое newlen)
{
	void *p = (char *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	целое err;

	если ((err = fdt_splice_(fdt, p, 0, newlen)))
		вернуть err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	вернуть 0;
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
static целое fdt_find_add_string_(void *fdt, const char *s, целое *allocated)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	целое len = strlen(s) + 1;
	целое err;

	если (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	если (p)
		/* found it */
		вернуть (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	если (err)
		вернуть err;

	если (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	вернуть (new - strtab);
}

целое fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	struct fdt_reserve_entry *re;
	целое err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	если (err)
		вернуть err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	вернуть 0;
}

целое fdt_del_mem_rsv(void *fdt, целое n)
{
	struct fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	если (n >= fdt_num_mem_rsv(fdt))
		вернуть -FDT_ERR_NOTFOUND;

	вернуть fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static целое fdt_resize_property_(void *fdt, целое nodeoffset, const char *name,
				целое len, struct fdt_property **prop)
{
	целое oldlen;
	целое err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	если (!*prop)
		вернуть oldlen;

	если ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		вернуть err;

	(*prop)->len = cpu_to_fdt32(len);
	вернуть 0;
}

static целое fdt_add_property_(void *fdt, целое nodeoffset, const char *name,
			     целое len, struct fdt_property **prop)
{
	целое proplen;
	целое nextoffset;
	целое namestroff;
	целое err;
	целое allocated;

	если ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		вернуть nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	если (namestroff < 0)
		вернуть namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = sizeof(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	если (err) {
		/* Delete the string if we failed to add it */
		если (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		вернуть err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	вернуть 0;
}

целое fdt_set_name(void *fdt, целое nodeoffset, const char *name)
{
	char *namep;
	целое oldlen, newlen;
	целое err;

	FDT_RW_PROBE(fdt);

	namep = (char *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	если (!namep)
		вернуть oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	если (err)
		вернуть err;

	memcpy(namep, name, newlen+1);
	вернуть 0;
}

целое fdt_setprop_placeholder(void *fdt, целое nodeoffset, const char *name,
			    целое len, void **prop_data)
{
	struct fdt_property *prop;
	целое err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	если (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	если (err)
		вернуть err;

	*prop_data = prop->data;
	вернуть 0;
}

целое fdt_setprop(void *fdt, целое nodeoffset, const char *name,
		const void *val, целое len)
{
	void *prop_data;
	целое err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	если (err)
		вернуть err;

	если (len)
		memcpy(prop_data, val, len);
	вернуть 0;
}

целое fdt_appendprop(void *fdt, целое nodeoffset, const char *name,
		   const void *val, целое len)
{
	struct fdt_property *prop;
	целое err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	если (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		если (err)
			вернуть err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} иначе {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		если (err)
			вернуть err;
		memcpy(prop->data, val, len);
	}
	вернуть 0;
}

целое fdt_delprop(void *fdt, целое nodeoffset, const char *name)
{
	struct fdt_property *prop;
	целое len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	если (!prop)
		вернуть len;

	proplen = sizeof(*prop) + FDT_TAGALIGN(len);
	вернуть fdt_splice_struct_(fdt, prop, proplen, 0);
}

целое fdt_add_subnode_namelen(void *fdt, целое parentoffset,
			    const char *name, целое namelen)
{
	struct fdt_node_header *nh;
	целое offset, nextoffset;
	целое nodelen;
	целое err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	если (offset >= 0)
		вернуть -FDT_ERR_EXISTS;
	иначе если (offset != -FDT_ERR_NOTFOUND)
		вернуть offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	если (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		вернуть -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} пока ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = sizeof(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	если (err)
		вернуть err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((char *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	вернуть offset;
}

целое fdt_add_subnode(void *fdt, целое parentoffset, const char *name)
{
	вернуть fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

целое fdt_del_node(void *fdt, целое nodeoffset)
{
	целое endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	если (endoffset < 0)
		вернуть endoffset;

	вернуть fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const char *old, char *new,
			    целое mem_rsv_size,
			    целое struct_size,
			    целое strings_size)
{
	целое mem_rsv_off, struct_off, strings_off;

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

целое fdt_open_into(const void *fdt, void *buf, целое bufsize)
{
	целое err;
	целое mem_rsv_size, struct_size;
	целое newsize;
	const char *fdtstart = fdt;
	const char *fdtend = fdtstart + fdt_totalsize(fdt);
	char *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);

	если (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} иначе если (fdt_version(fdt) == 16) {
		struct_size = 0;
		пока (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		если (struct_size < 0)
			вернуть struct_size;
	} иначе {
		вернуть -FDT_ERR_BADVERSION;
	}

	если (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		если (err)
			вернуть err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		вернуть 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(sizeof(struct fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	если (bufsize < newsize)
		вернуть -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	если (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (char *)(uintptr_t)fdtend;
		если ((tmp + newsize) > ((char *)buf + bufsize))
			вернуть -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	вернуть 0;
}

целое fdt_pack(void *fdt)
{
	целое mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* sizeof(struct fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	вернуть 0;
}
