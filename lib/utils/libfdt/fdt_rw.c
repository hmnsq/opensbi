// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_blocks_misordered_(ध्रुव शून्य *fdt,
				  पूर्णाङ्क mem_rsv_size, पूर्णाङ्क struct_size)
{
	प्रत्यागम (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(परिमाण(संरचना fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

स्थिर पूर्णाङ्क fdt_rw_probe_(शून्य *fdt)
{
	जइ (can_assume(VALID_DTB))
		प्रत्यागम 0;
	FDT_RO_PROBE(fdt);

	जइ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		प्रत्यागम -FDT_ERR_BADVERSION;
	जइ (fdt_blocks_misordered_(fdt, परिमाण(संरचना fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		प्रत्यागम -FDT_ERR_BADLAYOUT;
	जइ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	प्रत्यागम 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		पूर्णाङ्क err_; \
		जइ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			प्रत्यागम err_; \
	}

स्थिर अन्तःस्थ unsigned पूर्णाङ्क fdt_data_size_(शून्य *fdt)
{
	प्रत्यागम fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

स्थिर पूर्णाङ्क fdt_splice_(शून्य *fdt, शून्य *splicepoint, पूर्णाङ्क oldlen, पूर्णाङ्क newlen)
{
	वर्ण *p = splicepoint;
	unsigned पूर्णाङ्क dsize = fdt_data_size_(fdt);
	size_t soff = p - (वर्ण *)fdt;

	जइ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	जइ ((p < (वर्ण *)fdt) || (dsize + newlen < (unsigned)oldlen))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	जइ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((वर्ण *)fdt + dsize) - (p + oldlen));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_splice_mem_rsv_(शून्य *fdt, संरचना fdt_reserve_entry *p,
			       पूर्णाङ्क oldn, पूर्णाङ्क newn)
{
	पूर्णाङ्क delta = (newn - oldn) * परिमाण(*p);
	पूर्णाङ्क err;
	err = fdt_splice_(fdt, p, oldn * परिमाण(*p), newn * परिमाण(*p));
	जइ (err)
		प्रत्यागम err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_splice_struct_(शून्य *fdt, शून्य *p,
			      पूर्णाङ्क oldlen, पूर्णाङ्क newlen)
{
	पूर्णाङ्क delta = newlen - oldlen;
	पूर्णाङ्क err;

	जइ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		प्रत्यागम err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	प्रत्यागम 0;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_last_string_(शून्य *fdt, ध्रुव वर्ण *s)
{
	पूर्णाङ्क newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

स्थिर पूर्णाङ्क fdt_splice_string_(शून्य *fdt, पूर्णाङ्क newlen)
{
	शून्य *p = (वर्ण *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	पूर्णाङ्क err;

	जइ ((err = fdt_splice_(fdt, p, 0, newlen)))
		प्रत्यागम err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	प्रत्यागम 0;
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
स्थिर पूर्णाङ्क fdt_find_add_string_(शून्य *fdt, ध्रुव वर्ण *s, पूर्णाङ्क *allocated)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_off_dt_strings(fdt);
	ध्रुव वर्ण *p;
	वर्ण *new;
	पूर्णाङ्क len = strlen(s) + 1;
	पूर्णाङ्क err;

	जइ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	जइ (p)
		/* found it */
		प्रत्यागम (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	जइ (err)
		प्रत्यागम err;

	जइ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	प्रत्यागम (new - strtab);
}

पूर्णाङ्क fdt_add_mem_rsv(शून्य *fdt, uint64_t address, uint64_t size)
{
	संरचना fdt_reserve_entry *re;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_अङ्कः_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	जइ (err)
		प्रत्यागम err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_del_mem_rsv(शून्य *fdt, पूर्णाङ्क n)
{
	संरचना fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	जइ (n >= fdt_अङ्कः_mem_rsv(fdt))
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रत्यागम fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

स्थिर पूर्णाङ्क fdt_resize_property_(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
				पूर्णाङ्क len, संरचना fdt_property **prop)
{
	पूर्णाङ्क oldlen;
	पूर्णाङ्क err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	जइ (!*prop)
		प्रत्यागम oldlen;

	जइ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		प्रत्यागम err;

	(*prop)->len = cpu_to_fdt32(len);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_add_property_(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			     पूर्णाङ्क len, संरचना fdt_property **prop)
{
	पूर्णाङ्क proplen;
	पूर्णाङ्क nextoffset;
	पूर्णाङ्क namestroff;
	पूर्णाङ्क err;
	पूर्णाङ्क allocated;

	जइ ((nextoffset = fdt_check_ग्रन्थिः_offset(fdt, nodeoffset)) < 0)
		प्रत्यागम nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	जइ (namestroff < 0)
		प्रत्यागम namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = परिमाण(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	जइ (err) {
		/* Delete the string if we failed to add it */
		जइ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		प्रत्यागम err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_set_name(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	वर्ण *namep;
	पूर्णाङ्क oldlen, newlen;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	namep = (वर्ण *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	जइ (!namep)
		प्रत्यागम oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	जइ (err)
		प्रत्यागम err;

	memcpy(namep, name, newlen+1);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_setprop_placeholder(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			    पूर्णाङ्क len, शून्य **prop_data)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	जइ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	जइ (err)
		प्रत्यागम err;

	*prop_data = prop->data;
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_setprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
		ध्रुव शून्य *val, पूर्णाङ्क len)
{
	शून्य *prop_data;
	पूर्णाङ्क err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	जइ (err)
		प्रत्यागम err;

	जइ (len)
		memcpy(prop_data, val, len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_appendprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
		   ध्रुव शून्य *val, पूर्णाङ्क len)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	जइ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		जइ (err)
			प्रत्यागम err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} अण्णहा {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		जइ (err)
			प्रत्यागम err;
		memcpy(prop->data, val, len);
	}
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_delprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	जइ (!prop)
		प्रत्यागम len;

	proplen = परिमाण(*prop) + FDT_TAGALIGN(len);
	प्रत्यागम fdt_splice_struct_(fdt, prop, proplen, 0);
}

पूर्णाङ्क fdt_add_subnode_namelen(शून्य *fdt, पूर्णाङ्क parentoffset,
			    ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	संरचना fdt_ग्रन्थिः_header *nh;
	पूर्णाङ्क offset, nextoffset;
	पूर्णाङ्क nodelen;
	पूर्णाङ्क err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	जइ (offset >= 0)
		प्रत्यागम -FDT_ERR_EXISTS;
	अण्णहा जइ (offset != -FDT_ERR_NOTFOUND)
		प्रत्यागम offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	जइ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_ग्रन्थिः))
		प्रत्यागम -FDT_ERR_INTERNAL;
	करेहि {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} जाव ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = परिमाण(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	जइ (err)
		प्रत्यागम err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ग्रन्थिः);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((वर्ण *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_ग्रन्थिः);

	प्रत्यागम offset;
}

पूर्णाङ्क fdt_add_subnode(शून्य *fdt, पूर्णाङ्क parentoffset, ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

पूर्णाङ्क fdt_del_ग्रन्थिः(शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ग्रन्थिः_end_offset(fdt, nodeoffset);
	जइ (endoffset < 0)
		प्रत्यागम endoffset;

	प्रत्यागम fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

स्थिर शून्य fdt_packblocks_(ध्रुव वर्ण *old, वर्ण *new,
			    पूर्णाङ्क mem_rsv_size,
			    पूर्णाङ्क struct_size,
			    पूर्णाङ्क strings_size)
{
	पूर्णाङ्क mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(परिमाण(संरचना fdt_header), 8);
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

पूर्णाङ्क fdt_open_into(ध्रुव शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	पूर्णाङ्क err;
	पूर्णाङ्क mem_rsv_size, struct_size;
	पूर्णाङ्क newsize;
	ध्रुव वर्ण *fdtstart = fdt;
	ध्रुव वर्ण *fdtend = fdtstart + fdt_totalsize(fdt);
	वर्ण *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_अङ्कः_mem_rsv(fdt)+1)
		* परिमाण(संरचना fdt_reserve_entry);

	जइ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} अण्णहा जइ (fdt_version(fdt) == 16) {
		struct_size = 0;
		जाव (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		जइ (struct_size < 0)
			प्रत्यागम struct_size;
	} अण्णहा {
		प्रत्यागम -FDT_ERR_BADVERSION;
	}

	जइ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		जइ (err)
			प्रत्यागम err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		प्रत्यागम 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(परिमाण(संरचना fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	जइ (bufsize < newsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	जइ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (वर्ण *)(uintptr_t)fdtend;
		जइ ((tmp + newsize) > ((वर्ण *)buf + bufsize))
			प्रत्यागम -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_pack(शून्य *fdt)
{
	पूर्णाङ्क mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_अङ्कः_mem_rsv(fdt)+1)
		* परिमाण(संरचना fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	प्रत्यागम 0;
}
