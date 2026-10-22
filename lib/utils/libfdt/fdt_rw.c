// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ຈຳນວນເຕັມ fdt_blocks_misordered_(const void *fdt,
				  ຈຳນວນເຕັມ mem_rsv_size, ຈຳນວນເຕັມ struct_size)
{
	ສົ່ງຄືນ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static ຈຳນວນເຕັມ fdt_rw_probe_(void *fdt)
{
	ຖ້າ (can_assume(VALID_DTB))
		ສົ່ງຄືນ 0;
	FDT_RO_PROBE(fdt);

	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ສົ່ງຄືນ -FDT_ERR_BADVERSION;
	ຖ້າ (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ສົ່ງຄືນ -FDT_ERR_BADLAYOUT;
	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ສົ່ງຄືນ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ຈຳນວນເຕັມ err_; \
		ຖ້າ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ສົ່ງຄືນ err_; \
	}

static anta_hstha unsigned ຈຳນວນເຕັມ fdt_data_size_(void *fdt)
{
	ສົ່ງຄືນ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static ຈຳນວນເຕັມ fdt_splice_(void *fdt, void *splicepoint, ຈຳນວນເຕັມ oldlen, ຈຳນວນເຕັມ newlen)
{
	var_na *p = splicepoint;
	unsigned ຈຳນວນເຕັມ dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	ຖ້າ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;
	ຖ້າ ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;
	ຖ້າ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       ຈຳນວນເຕັມ oldn, ຈຳນວນເຕັມ newn)
{
	ຈຳນວນເຕັມ delta = (newn - oldn) * parimaa_na(*p);
	ຈຳນວນເຕັມ err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	ຖ້າ (err)
		ສົ່ງຄືນ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_splice_struct_(void *fdt, void *p,
			      ຈຳນວນເຕັມ oldlen, ຈຳນວນເຕັມ newlen)
{
	ຈຳນວນເຕັມ delta = newlen - oldlen;
	ຈຳນວນເຕັມ err;

	ຖ້າ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ສົ່ງຄືນ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ສົ່ງຄືນ 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	ຈຳນວນເຕັມ newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static ຈຳນວນເຕັມ fdt_splice_string_(void *fdt, ຈຳນວນເຕັມ newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ຈຳນວນເຕັມ err;

	ຖ້າ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ສົ່ງຄືນ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ສົ່ງຄືນ 0;
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
static ຈຳນວນເຕັມ fdt_find_add_string_(void *fdt, const var_na *s, ຈຳນວນເຕັມ *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	ຈຳນວນເຕັມ len = strlen(s) + 1;
	ຈຳນວນເຕັມ err;

	ຖ້າ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ຖ້າ (p)
		/* found it */
		ສົ່ງຄືນ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	ຖ້າ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ສົ່ງຄືນ (new - strtab);
}

ຈຳນວນເຕັມ fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	ຈຳນວນເຕັມ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_del_mem_rsv(void *fdt, ຈຳນວນເຕັມ n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ຖ້າ (n >= fdt_num_mem_rsv(fdt))
		ສົ່ງຄືນ -FDT_ERR_NOTFOUND;

	ສົ່ງຄືນ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static ຈຳນວນເຕັມ fdt_resize_property_(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
				ຈຳນວນເຕັມ len, sa_nracanaa fdt_property **prop)
{
	ຈຳນວນເຕັມ oldlen;
	ຈຳນວນເຕັມ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ຖ້າ (!*prop)
		ສົ່ງຄືນ oldlen;

	ຖ້າ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ສົ່ງຄືນ err;

	(*prop)->len = cpu_to_fdt32(len);
	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ fdt_add_property_(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
			     ຈຳນວນເຕັມ len, sa_nracanaa fdt_property **prop)
{
	ຈຳນວນເຕັມ proplen;
	ຈຳນວນເຕັມ nextoffset;
	ຈຳນວນເຕັມ namestroff;
	ຈຳນວນເຕັມ err;
	ຈຳນວນເຕັມ allocated;

	ຖ້າ ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ສົ່ງຄືນ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ຖ້າ (namestroff < 0)
		ສົ່ງຄືນ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ຖ້າ (err) {
		/* Delete the string if we failed to add it */
		ຖ້າ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ສົ່ງຄືນ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_set_name(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name)
{
	var_na *namep;
	ຈຳນວນເຕັມ oldlen, newlen;
	ຈຳນວນເຕັມ err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ຖ້າ (!namep)
		ສົ່ງຄືນ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	memcpy(namep, name, newlen+1);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_setprop_placeholder(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
			    ຈຳນວນເຕັມ len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	ຈຳນວນເຕັມ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ຖ້າ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	*prop_data = prop->data;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_setprop(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
		const void *val, ຈຳນວນເຕັມ len)
{
	void *prop_data;
	ຈຳນວນເຕັມ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	ຖ້າ (len)
		memcpy(prop_data, val, len);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_appendprop(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name,
		   const void *val, ຈຳນວນເຕັມ len)
{
	sa_nracanaa fdt_property *prop;
	ຈຳນວນເຕັມ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ຖ້າ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ຖ້າ (err)
			ສົ່ງຄືນ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} ບໍ່ດັ່ງນັ້ນ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ຖ້າ (err)
			ສົ່ງຄືນ err;
		memcpy(prop->data, val, len);
	}
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_delprop(void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	ຈຳນວນເຕັມ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ຖ້າ (!prop)
		ສົ່ງຄືນ len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ສົ່ງຄືນ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ຈຳນວນເຕັມ fdt_add_subnode_namelen(void *fdt, ຈຳນວນເຕັມ parentoffset,
			    const var_na *name, ຈຳນວນເຕັມ namelen)
{
	sa_nracanaa fdt_node_header *nh;
	ຈຳນວນເຕັມ offset, nextoffset;
	ຈຳນວນເຕັມ nodelen;
	ຈຳນວນເຕັມ err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ຖ້າ (offset >= 0)
		ສົ່ງຄືນ -FDT_ERR_EXISTS;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (offset != -FDT_ERR_NOTFOUND)
		ສົ່ງຄືນ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ຖ້າ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ສົ່ງຄືນ -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ໃນຂະນະທີ່ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ຖ້າ (err)
		ສົ່ງຄືນ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_add_subnode(void *fdt, ຈຳນວນເຕັມ parentoffset, const var_na *name)
{
	ສົ່ງຄືນ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ຈຳນວນເຕັມ fdt_del_node(void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ຖ້າ (endoffset < 0)
		ສົ່ງຄືນ endoffset;

	ສົ່ງຄືນ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    ຈຳນວນເຕັມ mem_rsv_size,
			    ຈຳນວນເຕັມ struct_size,
			    ຈຳນວນເຕັມ strings_size)
{
	ຈຳນວນເຕັມ mem_rsv_off, struct_off, strings_off;

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

ຈຳນວນເຕັມ fdt_open_into(const void *fdt, void *buf, ຈຳນວນເຕັມ bufsize)
{
	ຈຳນວນເຕັມ err;
	ຈຳນວນເຕັມ mem_rsv_size, struct_size;
	ຈຳນວນເຕັມ newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	ຖ້າ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_version(fdt) == 16) {
		struct_size = 0;
		ໃນຂະນະທີ່ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ຖ້າ (struct_size < 0)
			ສົ່ງຄືນ struct_size;
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ -FDT_ERR_BADVERSION;
	}

	ຖ້າ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ຖ້າ (err)
			ສົ່ງຄືນ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ສົ່ງຄືນ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ຖ້າ (bufsize < newsize)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ຖ້າ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		ຖ້າ ((tmp + newsize) > ((var_na *)buf + bufsize))
			ສົ່ງຄືນ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_pack(void *fdt)
{
	ຈຳນວນເຕັມ mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ສົ່ງຄືນ 0;
}
