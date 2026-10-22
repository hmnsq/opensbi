// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_blocks_misordered_(സ്ഥിരം ശൂന്യം *fdt,
				  പൂർണ്ണസംഖ്യ mem_rsv_size, പൂർണ്ണസംഖ്യ struct_size)
{
	തിരികെ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(വലുപ്പം(ഘടന fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_rw_probe_(ശൂന്യം *fdt)
{
	എങ്കിൽ (can_assume(VALID_DTB))
		തിരികെ 0;
	FDT_RO_PROBE(fdt);

	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		തിരികെ -FDT_ERR_BADVERSION;
	എങ്കിൽ (fdt_blocks_misordered_(fdt, വലുപ്പം(ഘടന fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		തിരികെ -FDT_ERR_BADLAYOUT;
	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	തിരികെ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		പൂർണ്ണസംഖ്യ err_; \
		എങ്കിൽ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			തിരികെ err_; \
	}

സ്ഥിതം വരിയിൽ unsigned പൂർണ്ണസംഖ്യ fdt_data_size_(ശൂന്യം *fdt)
{
	തിരികെ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_splice_(ശൂന്യം *fdt, ശൂന്യം *splicepoint, പൂർണ്ണസംഖ്യ oldlen, പൂർണ്ണസംഖ്യ newlen)
{
	അക്ഷരം *p = splicepoint;
	unsigned പൂർണ്ണസംഖ്യ dsize = fdt_data_size_(fdt);
	size_t soff = p - (അക്ഷരം *)fdt;

	എങ്കിൽ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		തിരികെ -FDT_ERR_BADOFFSET;
	എങ്കിൽ ((p < (അക്ഷരം *)fdt) || (dsize + newlen < (unsigned)oldlen))
		തിരികെ -FDT_ERR_BADOFFSET;
	എങ്കിൽ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		തിരികെ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((അക്ഷരം *)fdt + dsize) - (p + oldlen));
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_splice_mem_rsv_(ശൂന്യം *fdt, ഘടന fdt_reserve_entry *p,
			       പൂർണ്ണസംഖ്യ oldn, പൂർണ്ണസംഖ്യ newn)
{
	പൂർണ്ണസംഖ്യ delta = (newn - oldn) * വലുപ്പം(*p);
	പൂർണ്ണസംഖ്യ err;
	err = fdt_splice_(fdt, p, oldn * വലുപ്പം(*p), newn * വലുപ്പം(*p));
	എങ്കിൽ (err)
		തിരികെ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_splice_struct_(ശൂന്യം *fdt, ശൂന്യം *p,
			      പൂർണ്ണസംഖ്യ oldlen, പൂർണ്ണസംഖ്യ newlen)
{
	പൂർണ്ണസംഖ്യ delta = newlen - oldlen;
	പൂർണ്ണസംഖ്യ err;

	എങ്കിൽ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		തിരികെ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	തിരികെ 0;
}

/* Must only be used to roll back in case of error */
സ്ഥിതം ശൂന്യം fdt_del_last_string_(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *s)
{
	പൂർണ്ണസംഖ്യ newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_splice_string_(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ newlen)
{
	ശൂന്യം *p = (അക്ഷരം *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	പൂർണ്ണസംഖ്യ err;

	എങ്കിൽ ((err = fdt_splice_(fdt, p, 0, newlen)))
		തിരികെ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	തിരികെ 0;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_find_add_string_(ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *allocated)
{
	അക്ഷരം *strtab = (അക്ഷരം *)fdt + fdt_off_dt_strings(fdt);
	സ്ഥിരം അക്ഷരം *p;
	അക്ഷരം *new;
	പൂർണ്ണസംഖ്യ len = strlen(s) + 1;
	പൂർണ്ണസംഖ്യ err;

	എങ്കിൽ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	എങ്കിൽ (p)
		/* found it */
		തിരികെ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	എങ്കിൽ (err)
		തിരികെ err;

	എങ്കിൽ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	തിരികെ (new - strtab);
}

പൂർണ്ണസംഖ്യ fdt_add_mem_rsv(ശൂന്യം *fdt, uint64_t address, uint64_t size)
{
	ഘടന fdt_reserve_entry *re;
	പൂർണ്ണസംഖ്യ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_അക്കം_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	എങ്കിൽ (err)
		തിരികെ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_del_mem_rsv(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ n)
{
	ഘടന fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	എങ്കിൽ (n >= fdt_അക്കം_mem_rsv(fdt))
		തിരികെ -FDT_ERR_NOTFOUND;

	തിരികെ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_resize_property_(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
				പൂർണ്ണസംഖ്യ len, ഘടന fdt_property **prop)
{
	പൂർണ്ണസംഖ്യ oldlen;
	പൂർണ്ണസംഖ്യ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	എങ്കിൽ (!*prop)
		തിരികെ oldlen;

	എങ്കിൽ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		തിരികെ err;

	(*prop)->len = cpu_to_fdt32(len);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_add_property_(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
			     പൂർണ്ണസംഖ്യ len, ഘടന fdt_property **prop)
{
	പൂർണ്ണസംഖ്യ proplen;
	പൂർണ്ണസംഖ്യ nextoffset;
	പൂർണ്ണസംഖ്യ namestroff;
	പൂർണ്ണസംഖ്യ err;
	പൂർണ്ണസംഖ്യ allocated;

	എങ്കിൽ ((nextoffset = fdt_check_കണ്ണി_offset(fdt, nodeoffset)) < 0)
		തിരികെ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	എങ്കിൽ (namestroff < 0)
		തിരികെ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = വലുപ്പം(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	എങ്കിൽ (err) {
		/* Delete the string if we failed to add it */
		എങ്കിൽ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		തിരികെ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_set_name(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name)
{
	അക്ഷരം *namep;
	പൂർണ്ണസംഖ്യ oldlen, newlen;
	പൂർണ്ണസംഖ്യ err;

	FDT_RW_PROBE(fdt);

	namep = (അക്ഷരം *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	എങ്കിൽ (!namep)
		തിരികെ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	എങ്കിൽ (err)
		തിരികെ err;

	memcpy(namep, name, newlen+1);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_setprop_placeholder(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
			    പൂർണ്ണസംഖ്യ len, ശൂന്യം **prop_data)
{
	ഘടന fdt_property *prop;
	പൂർണ്ണസംഖ്യ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	എങ്കിൽ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	എങ്കിൽ (err)
		തിരികെ err;

	*prop_data = prop->data;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_setprop(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
		സ്ഥിരം ശൂന്യം *val, പൂർണ്ണസംഖ്യ len)
{
	ശൂന്യം *prop_data;
	പൂർണ്ണസംഖ്യ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	എങ്കിൽ (err)
		തിരികെ err;

	എങ്കിൽ (len)
		memcpy(prop_data, val, len);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_appendprop(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name,
		   സ്ഥിരം ശൂന്യം *val, പൂർണ്ണസംഖ്യ len)
{
	ഘടന fdt_property *prop;
	പൂർണ്ണസംഖ്യ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	എങ്കിൽ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		എങ്കിൽ (err)
			തിരികെ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} അല്ലെങ്കിൽ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		എങ്കിൽ (err)
			തിരികെ err;
		memcpy(prop->data, val, len);
	}
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_delprop(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *name)
{
	ഘടന fdt_property *prop;
	പൂർണ്ണസംഖ്യ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	എങ്കിൽ (!prop)
		തിരികെ len;

	proplen = വലുപ്പം(*prop) + FDT_TAGALIGN(len);
	തിരികെ fdt_splice_struct_(fdt, prop, proplen, 0);
}

പൂർണ്ണസംഖ്യ fdt_add_subnode_namelen(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ parentoffset,
			    സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen)
{
	ഘടന fdt_കണ്ണി_header *nh;
	പൂർണ്ണസംഖ്യ offset, nextoffset;
	പൂർണ്ണസംഖ്യ nodelen;
	പൂർണ്ണസംഖ്യ err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	എങ്കിൽ (offset >= 0)
		തിരികെ -FDT_ERR_EXISTS;
	അല്ലെങ്കിൽ എങ്കിൽ (offset != -FDT_ERR_NOTFOUND)
		തിരികെ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	എങ്കിൽ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_കണ്ണി))
		തിരികെ -FDT_ERR_INTERNAL;
	ചെയ്യുക {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ആകുമ്പോൾ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = വലുപ്പം(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	എങ്കിൽ (err)
		തിരികെ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_കണ്ണി);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((അക്ഷരം *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_കണ്ണി);

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_add_subnode(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ parentoffset, സ്ഥിരം അക്ഷരം *name)
{
	തിരികെ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

പൂർണ്ണസംഖ്യ fdt_del_കണ്ണി(ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_കണ്ണി_end_offset(fdt, nodeoffset);
	എങ്കിൽ (endoffset < 0)
		തിരികെ endoffset;

	തിരികെ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

സ്ഥിതം ശൂന്യം fdt_packblocks_(സ്ഥിരം അക്ഷരം *old, അക്ഷരം *new,
			    പൂർണ്ണസംഖ്യ mem_rsv_size,
			    പൂർണ്ണസംഖ്യ struct_size,
			    പൂർണ്ണസംഖ്യ strings_size)
{
	പൂർണ്ണസംഖ്യ mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(വലുപ്പം(ഘടന fdt_header), 8);
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

പൂർണ്ണസംഖ്യ fdt_open_into(സ്ഥിരം ശൂന്യം *fdt, ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize)
{
	പൂർണ്ണസംഖ്യ err;
	പൂർണ്ണസംഖ്യ mem_rsv_size, struct_size;
	പൂർണ്ണസംഖ്യ newsize;
	സ്ഥിരം അക്ഷരം *fdtstart = fdt;
	സ്ഥിരം അക്ഷരം *fdtend = fdtstart + fdt_totalsize(fdt);
	അക്ഷരം *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_അക്കം_mem_rsv(fdt)+1)
		* വലുപ്പം(ഘടന fdt_reserve_entry);

	എങ്കിൽ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} അല്ലെങ്കിൽ എങ്കിൽ (fdt_version(fdt) == 16) {
		struct_size = 0;
		ആകുമ്പോൾ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		എങ്കിൽ (struct_size < 0)
			തിരികെ struct_size;
	} അല്ലെങ്കിൽ {
		തിരികെ -FDT_ERR_BADVERSION;
	}

	എങ്കിൽ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		എങ്കിൽ (err)
			തിരികെ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		തിരികെ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(വലുപ്പം(ഘടന fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	എങ്കിൽ (bufsize < newsize)
		തിരികെ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	എങ്കിൽ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (അക്ഷരം *)(uintptr_t)fdtend;
		എങ്കിൽ ((tmp + newsize) > ((അക്ഷരം *)buf + bufsize))
			തിരികെ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_pack(ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_അക്കം_mem_rsv(fdt)+1)
		* വലുപ്പം(ഘടന fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	തിരികെ 0;
}
