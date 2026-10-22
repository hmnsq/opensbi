// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णांक fdt_blocks_misordered_(स्थिरांक शून्य *fdt,
				  पूर्णांक mem_rsv_size, पूर्णांक struct_size)
{
	परत (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(आकार(संरचना fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

स्थिर पूर्णांक fdt_rw_probe_(शून्य *fdt)
{
	जर (can_assume(VALID_DTB))
		परत 0;
	FDT_RO_PROBE(fdt);

	जर (!can_assume(LATEST) && fdt_version(fdt) < 17)
		परत -FDT_ERR_BADVERSION;
	जर (fdt_blocks_misordered_(fdt, आकार(संरचना fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		परत -FDT_ERR_BADLAYOUT;
	जर (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	परत 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		पूर्णांक err_; \
		जर ((err_ = fdt_rw_probe_(fdt)) != 0) \
			परत err_; \
	}

स्थिर ओळीत अचिन्हित पूर्णांक fdt_data_size_(शून्य *fdt)
{
	परत fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

स्थिर पूर्णांक fdt_splice_(शून्य *fdt, शून्य *splicepoint, पूर्णांक oldlen, पूर्णांक newlen)
{
	अक्षर *p = splicepoint;
	अचिन्हित पूर्णांक dsize = fdt_data_size_(fdt);
	माप_प्रकार soff = p - (अक्षर *)fdt;

	जर ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		परत -FDT_ERR_BADOFFSET;
	जर ((p < (अक्षर *)fdt) || (dsize + newlen < (अचिन्हित)oldlen))
		परत -FDT_ERR_BADOFFSET;
	जर (dsize - oldlen + newlen > fdt_totalsize(fdt))
		परत -FDT_ERR_NOSPACE;
	स_हटाओ(p + newlen, p + oldlen, ((अक्षर *)fdt + dsize) - (p + oldlen));
	परत 0;
}

स्थिर पूर्णांक fdt_splice_mem_rsv_(शून्य *fdt, संरचना fdt_reserve_entry *p,
			       पूर्णांक oldn, पूर्णांक newn)
{
	पूर्णांक delta = (newn - oldn) * आकार(*p);
	पूर्णांक err;
	err = fdt_splice_(fdt, p, oldn * आकार(*p), newn * आकार(*p));
	जर (err)
		परत err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	परत 0;
}

स्थिर पूर्णांक fdt_splice_struct_(शून्य *fdt, शून्य *p,
			      पूर्णांक oldlen, पूर्णांक newlen)
{
	पूर्णांक delta = newlen - oldlen;
	पूर्णांक err;

	जर ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		परत err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	परत 0;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_last_string_(शून्य *fdt, स्थिरांक अक्षर *s)
{
	पूर्णांक newlen = म_माप(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

स्थिर पूर्णांक fdt_splice_string_(शून्य *fdt, पूर्णांक newlen)
{
	शून्य *p = (अक्षर *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	पूर्णांक err;

	जर ((err = fdt_splice_(fdt, p, 0, newlen)))
		परत err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	परत 0;
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
स्थिर पूर्णांक fdt_find_add_string_(शून्य *fdt, स्थिरांक अक्षर *s, पूर्णांक *allocated)
{
	अक्षर *strtab = (अक्षर *)fdt + fdt_off_dt_strings(fdt);
	स्थिरांक अक्षर *p;
	अक्षर *new;
	पूर्णांक len = म_माप(s) + 1;
	पूर्णांक err;

	जर (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	जर (p)
		/* found it */
		परत (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	जर (err)
		परत err;

	जर (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	स_नकल(new, s, len);
	परत (new - strtab);
}

पूर्णांक fdt_add_mem_rsv(शून्य *fdt, uint64_t address, uint64_t size)
{
	संरचना fdt_reserve_entry *re;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_अंक_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	जर (err)
		परत err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	परत 0;
}

पूर्णांक fdt_del_mem_rsv(शून्य *fdt, पूर्णांक n)
{
	संरचना fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	जर (n >= fdt_अंक_mem_rsv(fdt))
		परत -FDT_ERR_NOTFOUND;

	परत fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

स्थिर पूर्णांक fdt_resize_property_(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
				पूर्णांक len, संरचना fdt_property **prop)
{
	पूर्णांक oldlen;
	पूर्णांक err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	जर (!*prop)
		परत oldlen;

	जर ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		परत err;

	(*prop)->len = cpu_to_fdt32(len);
	परत 0;
}

स्थिर पूर्णांक fdt_add_property_(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
			     पूर्णांक len, संरचना fdt_property **prop)
{
	पूर्णांक proplen;
	पूर्णांक nextoffset;
	पूर्णांक namestroff;
	पूर्णांक err;
	पूर्णांक allocated;

	जर ((nextoffset = fdt_check_गाठ_offset(fdt, nodeoffset)) < 0)
		परत nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	जर (namestroff < 0)
		परत namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = आकार(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	जर (err) {
		/* Delete the string if we failed to add it */
		जर (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		परत err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	परत 0;
}

पूर्णांक fdt_set_name(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name)
{
	अक्षर *namep;
	पूर्णांक oldlen, newlen;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	namep = (अक्षर *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	जर (!namep)
		परत oldlen;

	newlen = म_माप(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	जर (err)
		परत err;

	स_नकल(namep, name, newlen+1);
	परत 0;
}

पूर्णांक fdt_setprop_placeholder(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
			    पूर्णांक len, शून्य **prop_data)
{
	संरचना fdt_property *prop;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	जर (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	जर (err)
		परत err;

	*prop_data = prop->data;
	परत 0;
}

पूर्णांक fdt_setprop(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
		स्थिरांक शून्य *val, पूर्णांक len)
{
	शून्य *prop_data;
	पूर्णांक err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	जर (err)
		परत err;

	जर (len)
		स_नकल(prop_data, val, len);
	परत 0;
}

पूर्णांक fdt_appendprop(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name,
		   स्थिरांक शून्य *val, पूर्णांक len)
{
	संरचना fdt_property *prop;
	पूर्णांक err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	जर (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		जर (err)
			परत err;
		prop->len = cpu_to_fdt32(newlen);
		स_नकल(prop->data + oldlen, val, len);
	} अन्यथा {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		जर (err)
			परत err;
		स_नकल(prop->data, val, len);
	}
	परत 0;
}

पूर्णांक fdt_delprop(शून्य *fdt, पूर्णांक nodeoffset, स्थिरांक अक्षर *name)
{
	संरचना fdt_property *prop;
	पूर्णांक len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	जर (!prop)
		परत len;

	proplen = आकार(*prop) + FDT_TAGALIGN(len);
	परत fdt_splice_struct_(fdt, prop, proplen, 0);
}

पूर्णांक fdt_add_subnode_namelen(शून्य *fdt, पूर्णांक parentoffset,
			    स्थिरांक अक्षर *name, पूर्णांक namelen)
{
	संरचना fdt_गाठ_header *nh;
	पूर्णांक offset, nextoffset;
	पूर्णांक nodelen;
	पूर्णांक err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	जर (offset >= 0)
		परत -FDT_ERR_EXISTS;
	अन्यथा जर (offset != -FDT_ERR_NOTFOUND)
		परत offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	जर (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_गाठ))
		परत -FDT_ERR_INTERNAL;
	करा {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} पर्यंत ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = आकार(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	जर (err)
		परत err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_गाठ);
	स_रखो(nh->name, 0, FDT_TAGALIGN(namelen+1));
	स_नकल(nh->name, name, namelen);
	endtag = (fdt32_t *)((अक्षर *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_गाठ);

	परत offset;
}

पूर्णांक fdt_add_subnode(शून्य *fdt, पूर्णांक parentoffset, स्थिरांक अक्षर *name)
{
	परत fdt_add_subnode_namelen(fdt, parentoffset, name, म_माप(name));
}

पूर्णांक fdt_del_गाठ(शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_गाठ_end_offset(fdt, nodeoffset);
	जर (endoffset < 0)
		परत endoffset;

	परत fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

स्थिर शून्य fdt_packblocks_(स्थिरांक अक्षर *old, अक्षर *new,
			    पूर्णांक mem_rsv_size,
			    पूर्णांक struct_size,
			    पूर्णांक strings_size)
{
	पूर्णांक mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(आकार(संरचना fdt_header), 8);
	struct_off = mem_rsv_off + mem_rsv_size;
	strings_off = struct_off + struct_size;

	स_हटाओ(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_size);
	fdt_set_off_mem_rsvmap(new, mem_rsv_off);

	स_हटाओ(new + struct_off, old + fdt_off_dt_struct(old), struct_size);
	fdt_set_off_dt_struct(new, struct_off);
	fdt_set_size_dt_struct(new, struct_size);

	स_हटाओ(new + strings_off, old + fdt_off_dt_strings(old), strings_size);
	fdt_set_off_dt_strings(new, strings_off);
	fdt_set_size_dt_strings(new, fdt_size_dt_strings(old));
}

पूर्णांक fdt_open_into(स्थिरांक शून्य *fdt, शून्य *buf, पूर्णांक bufsize)
{
	पूर्णांक err;
	पूर्णांक mem_rsv_size, struct_size;
	पूर्णांक newsize;
	स्थिरांक अक्षर *fdtstart = fdt;
	स्थिरांक अक्षर *fdtend = fdtstart + fdt_totalsize(fdt);
	अक्षर *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_अंक_mem_rsv(fdt)+1)
		* आकार(संरचना fdt_reserve_entry);

	जर (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} अन्यथा जर (fdt_version(fdt) == 16) {
		struct_size = 0;
		पर्यंत (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		जर (struct_size < 0)
			परत struct_size;
	} अन्यथा {
		परत -FDT_ERR_BADVERSION;
	}

	जर (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		जर (err)
			परत err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		परत 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(आकार(संरचना fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	जर (bufsize < newsize)
		परत -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	जर (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (अक्षर *)(uintptr_t)fdtend;
		जर ((tmp + newsize) > ((अक्षर *)buf + bufsize))
			परत -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	स_हटाओ(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	परत 0;
}

पूर्णांक fdt_pack(शून्य *fdt)
{
	पूर्णांक mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_अंक_mem_rsv(fdt)+1)
		* आकार(संरचना fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	परत 0;
}
