// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ចំនួនគត់ fdt_blocks_misordered_(const void *fdt,
				  ចំនួនគត់ mem_rsv_size, ចំនួនគត់ struct_size)
{
	ត្រឡប់ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static ចំនួនគត់ fdt_rw_probe_(void *fdt)
{
	ប្រសិនបើ (can_assume(VALID_DTB))
		ត្រឡប់ 0;
	FDT_RO_PROBE(fdt);

	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ត្រឡប់ -FDT_ERR_BADVERSION;
	ប្រសិនបើ (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ត្រឡប់ -FDT_ERR_BADLAYOUT;
	ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ត្រឡប់ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ចំនួនគត់ err_; \
		ប្រសិនបើ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ត្រឡប់ err_; \
	}

static anta_hstha unsigned ចំនួនគត់ fdt_data_size_(void *fdt)
{
	ត្រឡប់ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static ចំនួនគត់ fdt_splice_(void *fdt, void *splicepoint, ចំនួនគត់ oldlen, ចំនួនគត់ newlen)
{
	var_na *p = splicepoint;
	unsigned ចំនួនគត់ dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	ប្រសិនបើ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ត្រឡប់ -FDT_ERR_BADOFFSET;
	ប្រសិនបើ ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ត្រឡប់ -FDT_ERR_BADOFFSET;
	ប្រសិនបើ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ត្រឡប់ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       ចំនួនគត់ oldn, ចំនួនគត់ newn)
{
	ចំនួនគត់ delta = (newn - oldn) * parimaa_na(*p);
	ចំនួនគត់ err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	ប្រសិនបើ (err)
		ត្រឡប់ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_splice_struct_(void *fdt, void *p,
			      ចំនួនគត់ oldlen, ចំនួនគត់ newlen)
{
	ចំនួនគត់ delta = newlen - oldlen;
	ចំនួនគត់ err;

	ប្រសិនបើ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ត្រឡប់ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ត្រឡប់ 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ចំនួនគត់ newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static ចំនួនគត់ fdt_splice_string_(void *fdt, ចំនួនគត់ newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ចំនួនគត់ err;

	ប្រសិនបើ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ត្រឡប់ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ត្រឡប់ 0;
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
static ចំនួនគត់ fdt_find_add_string_(void *fdt, const var_na *s, ចំនួនគត់ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	ចំនួនគត់ len = strlen(s) + 1;
	ចំនួនគត់ err;

	ប្រសិនបើ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ប្រសិនបើ (p)
		/* found it */
		ត្រឡប់ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	ប្រសិនបើ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ត្រឡប់ (new - strtab);
}

ចំនួនគត់ fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ចំនួនគត់ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_del_mem_rsv(void *fdt, ចំនួនគត់ n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ប្រសិនបើ (n >= fdt_num_mem_rsv(fdt))
		ត្រឡប់ -FDT_ERR_NOTFOUND;

	ត្រឡប់ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static ចំនួនគត់ fdt_resize_property_(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
				ចំនួនគត់ len, sa_nracanaa fdt_property **prop)
{
	ចំនួនគត់ oldlen;
	ចំនួនគត់ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ប្រសិនបើ (!*prop)
		ត្រឡប់ oldlen;

	ប្រសិនបើ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ត្រឡប់ err;

	(*prop)->len = cpu_to_fdt32(len);
	ត្រឡប់ 0;
}

static ចំនួនគត់ fdt_add_property_(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
			     ចំនួនគត់ len, sa_nracanaa fdt_property **prop)
{
	ចំនួនគត់ proplen;
	ចំនួនគត់ nextoffset;
	ចំនួនគត់ namestroff;
	ចំនួនគត់ err;
	ចំនួនគត់ allocated;

	ប្រសិនបើ ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ត្រឡប់ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ប្រសិនបើ (namestroff < 0)
		ត្រឡប់ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ប្រសិនបើ (err) {
		/* Delete the string if we failed to add it */
		ប្រសិនបើ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ត្រឡប់ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_set_name(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name)
{
	var_na *namep;
	ចំនួនគត់ oldlen, newlen;
	ចំនួនគត់ err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ប្រសិនបើ (!namep)
		ត្រឡប់ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	memcpy(namep, name, newlen+1);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_setprop_placeholder(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
			    ចំនួនគត់ len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	ចំនួនគត់ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ប្រសិនបើ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	*prop_data = prop->data;
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_setprop(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
		const void *val, ចំនួនគត់ len)
{
	void *prop_data;
	ចំនួនគត់ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	ប្រសិនបើ (len)
		memcpy(prop_data, val, len);
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_appendprop(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name,
		   const void *val, ចំនួនគត់ len)
{
	sa_nracanaa fdt_property *prop;
	ចំនួនគត់ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ប្រសិនបើ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ប្រសិនបើ (err)
			ត្រឡប់ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} បើមិនដូច្នេះ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ប្រសិនបើ (err)
			ត្រឡប់ err;
		memcpy(prop->data, val, len);
	}
	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_delprop(void *fdt, ចំនួនគត់ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ចំនួនគត់ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ប្រសិនបើ (!prop)
		ត្រឡប់ len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ត្រឡប់ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ចំនួនគត់ fdt_add_subnode_namelen(void *fdt, ចំនួនគត់ parentoffset,
			    const var_na *name, ចំនួនគត់ namelen)
{
	sa_nracanaa fdt_node_header *nh;
	ចំនួនគត់ offset, nextoffset;
	ចំនួនគត់ nodelen;
	ចំនួនគត់ err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ប្រសិនបើ (offset >= 0)
		ត្រឡប់ -FDT_ERR_EXISTS;
	បើមិនដូច្នេះ ប្រសិនបើ (offset != -FDT_ERR_NOTFOUND)
		ត្រឡប់ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ប្រសិនបើ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ត្រឡប់ -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ខណៈពេល ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ប្រសិនបើ (err)
		ត្រឡប់ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_add_subnode(void *fdt, ចំនួនគត់ parentoffset, const var_na *name)
{
	ត្រឡប់ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ចំនួនគត់ fdt_del_node(void *fdt, ចំនួនគត់ nodeoffset)
{
	ចំនួនគត់ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ប្រសិនបើ (endoffset < 0)
		ត្រឡប់ endoffset;

	ត្រឡប់ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    ចំនួនគត់ mem_rsv_size,
			    ចំនួនគត់ struct_size,
			    ចំនួនគត់ strings_size)
{
	ចំនួនគត់ mem_rsv_off, struct_off, strings_off;

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

ចំនួនគត់ fdt_open_into(const void *fdt, void *buf, ចំនួនគត់ bufsize)
{
	ចំនួនគត់ err;
	ចំនួនគត់ mem_rsv_size, struct_size;
	ចំនួនគត់ newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	ប្រសិនបើ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} បើមិនដូច្នេះ ប្រសិនបើ (fdt_version(fdt) == 16) {
		struct_size = 0;
		ខណៈពេល (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ប្រសិនបើ (struct_size < 0)
			ត្រឡប់ struct_size;
	} បើមិនដូច្នេះ {
		ត្រឡប់ -FDT_ERR_BADVERSION;
	}

	ប្រសិនបើ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ប្រសិនបើ (err)
			ត្រឡប់ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ត្រឡប់ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ប្រសិនបើ (bufsize < newsize)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ប្រសិនបើ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		ប្រសិនបើ ((tmp + newsize) > ((var_na *)buf + bufsize))
			ត្រឡប់ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ត្រឡប់ 0;
}

ចំនួនគត់ fdt_pack(void *fdt)
{
	ចំនួនគត់ mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ត្រឡប់ 0;
}
