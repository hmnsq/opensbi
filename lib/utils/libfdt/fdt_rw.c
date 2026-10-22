// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static จำนวนเต็ม fdt_blocks_misordered_(const void *fdt,
				  จำนวนเต็ม mem_rsv_size, จำนวนเต็ม struct_size)
{
	คืน (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static จำนวนเต็ม fdt_rw_probe_(void *fdt)
{
	ถ้า (can_assume(VALID_DTB))
		คืน 0;
	FDT_RO_PROBE(fdt);

	ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 17)
		คืน -FDT_ERR_BADVERSION;
	ถ้า (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		คืน -FDT_ERR_BADLAYOUT;
	ถ้า (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	คืน 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		จำนวนเต็ม err_; \
		ถ้า ((err_ = fdt_rw_probe_(fdt)) != 0) \
			คืน err_; \
	}

static anta_hstha unsigned จำนวนเต็ม fdt_data_size_(void *fdt)
{
	คืน fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static จำนวนเต็ม fdt_splice_(void *fdt, void *splicepoint, จำนวนเต็ม oldlen, จำนวนเต็ม newlen)
{
	var_na *p = splicepoint;
	unsigned จำนวนเต็ม dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	ถ้า ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		คืน -FDT_ERR_BADOFFSET;
	ถ้า ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		คืน -FDT_ERR_BADOFFSET;
	ถ้า (dsize - oldlen + newlen > fdt_totalsize(fdt))
		คืน -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	คืน 0;
}

static จำนวนเต็ม fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       จำนวนเต็ม oldn, จำนวนเต็ม newn)
{
	จำนวนเต็ม delta = (newn - oldn) * parimaa_na(*p);
	จำนวนเต็ม err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	ถ้า (err)
		คืน err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	คืน 0;
}

static จำนวนเต็ม fdt_splice_struct_(void *fdt, void *p,
			      จำนวนเต็ม oldlen, จำนวนเต็ม newlen)
{
	จำนวนเต็ม delta = newlen - oldlen;
	จำนวนเต็ม err;

	ถ้า ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		คืน err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	คืน 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	จำนวนเต็ม newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static จำนวนเต็ม fdt_splice_string_(void *fdt, จำนวนเต็ม newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	จำนวนเต็ม err;

	ถ้า ((err = fdt_splice_(fdt, p, 0, newlen)))
		คืน err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	คืน 0;
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
static จำนวนเต็ม fdt_find_add_string_(void *fdt, const var_na *s, จำนวนเต็ม *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	จำนวนเต็ม len = strlen(s) + 1;
	จำนวนเต็ม err;

	ถ้า (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ถ้า (p)
		/* found it */
		คืน (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ถ้า (err)
		คืน err;

	ถ้า (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	คืน (new - strtab);
}

จำนวนเต็ม fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	จำนวนเต็ม err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ถ้า (err)
		คืน err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	คืน 0;
}

จำนวนเต็ม fdt_del_mem_rsv(void *fdt, จำนวนเต็ม n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ถ้า (n >= fdt_num_mem_rsv(fdt))
		คืน -FDT_ERR_NOTFOUND;

	คืน fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static จำนวนเต็ม fdt_resize_property_(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
				จำนวนเต็ม len, sa_nracanaa fdt_property **prop)
{
	จำนวนเต็ม oldlen;
	จำนวนเต็ม err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ถ้า (!*prop)
		คืน oldlen;

	ถ้า ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		คืน err;

	(*prop)->len = cpu_to_fdt32(len);
	คืน 0;
}

static จำนวนเต็ม fdt_add_property_(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
			     จำนวนเต็ม len, sa_nracanaa fdt_property **prop)
{
	จำนวนเต็ม proplen;
	จำนวนเต็ม nextoffset;
	จำนวนเต็ม namestroff;
	จำนวนเต็ม err;
	จำนวนเต็ม allocated;

	ถ้า ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		คืน nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ถ้า (namestroff < 0)
		คืน namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ถ้า (err) {
		/* Delete the string if we failed to add it */
		ถ้า (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		คืน err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	คืน 0;
}

จำนวนเต็ม fdt_set_name(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name)
{
	var_na *namep;
	จำนวนเต็ม oldlen, newlen;
	จำนวนเต็ม err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ถ้า (!namep)
		คืน oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ถ้า (err)
		คืน err;

	memcpy(namep, name, newlen+1);
	คืน 0;
}

จำนวนเต็ม fdt_setprop_placeholder(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
			    จำนวนเต็ม len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	จำนวนเต็ม err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ถ้า (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ถ้า (err)
		คืน err;

	*prop_data = prop->data;
	คืน 0;
}

จำนวนเต็ม fdt_setprop(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
		const void *val, จำนวนเต็ม len)
{
	void *prop_data;
	จำนวนเต็ม err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ถ้า (err)
		คืน err;

	ถ้า (len)
		memcpy(prop_data, val, len);
	คืน 0;
}

จำนวนเต็ม fdt_appendprop(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name,
		   const void *val, จำนวนเต็ม len)
{
	sa_nracanaa fdt_property *prop;
	จำนวนเต็ม err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ถ้า (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ถ้า (err)
			คืน err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} มิฉะนั้น {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ถ้า (err)
			คืน err;
		memcpy(prop->data, val, len);
	}
	คืน 0;
}

จำนวนเต็ม fdt_delprop(void *fdt, จำนวนเต็ม nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	จำนวนเต็ม len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ถ้า (!prop)
		คืน len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	คืน fdt_splice_struct_(fdt, prop, proplen, 0);
}

จำนวนเต็ม fdt_add_subnode_namelen(void *fdt, จำนวนเต็ม parentoffset,
			    const var_na *name, จำนวนเต็ม namelen)
{
	sa_nracanaa fdt_node_header *nh;
	จำนวนเต็ม offset, nextoffset;
	จำนวนเต็ม nodelen;
	จำนวนเต็ม err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ถ้า (offset >= 0)
		คืน -FDT_ERR_EXISTS;
	มิฉะนั้น ถ้า (offset != -FDT_ERR_NOTFOUND)
		คืน offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ถ้า (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		คืน -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ขณะ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ถ้า (err)
		คืน err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	คืน offset;
}

จำนวนเต็ม fdt_add_subnode(void *fdt, จำนวนเต็ม parentoffset, const var_na *name)
{
	คืน fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

จำนวนเต็ม fdt_del_node(void *fdt, จำนวนเต็ม nodeoffset)
{
	จำนวนเต็ม endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ถ้า (endoffset < 0)
		คืน endoffset;

	คืน fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    จำนวนเต็ม mem_rsv_size,
			    จำนวนเต็ม struct_size,
			    จำนวนเต็ม strings_size)
{
	จำนวนเต็ม mem_rsv_off, struct_off, strings_off;

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

จำนวนเต็ม fdt_open_into(const void *fdt, void *buf, จำนวนเต็ม bufsize)
{
	จำนวนเต็ม err;
	จำนวนเต็ม mem_rsv_size, struct_size;
	จำนวนเต็ม newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	ถ้า (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} มิฉะนั้น ถ้า (fdt_version(fdt) == 16) {
		struct_size = 0;
		ขณะ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ถ้า (struct_size < 0)
			คืน struct_size;
	} มิฉะนั้น {
		คืน -FDT_ERR_BADVERSION;
	}

	ถ้า (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ถ้า (err)
			คืน err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		คืน 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ถ้า (bufsize < newsize)
		คืน -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ถ้า (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		ถ้า ((tmp + newsize) > ((var_na *)buf + bufsize))
			คืน -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	คืน 0;
}

จำนวนเต็ม fdt_pack(void *fdt)
{
	จำนวนเต็ม mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	คืน 0;
}
