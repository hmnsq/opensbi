// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#समावेश "libfdt_env.h"

#समावेश <fdt.h>
#समावेश <libfdt.h>

#समावेश "libfdt_internal.h"

अटल पूर्णांक fdt_blocks_misordered_(स्थिर व्योम *fdt,
				  पूर्णांक mem_rsv_आकार, पूर्णांक struct_आकार)
{
	वापस (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(माप(काष्ठा fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_आकार))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_आकार))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_आकार_dt_strings(fdt)));
}

अटल पूर्णांक fdt_rw_probe_(व्योम *fdt)
{
	अगर (can_assume(VALID_DTB))
		वापस 0;
	FDT_RO_PROBE(fdt);

	अगर (!can_assume(LATEST) && fdt_version(fdt) < 17)
		वापस -FDT_ERR_BADVERSION;
	अगर (fdt_blocks_misordered_(fdt, माप(काष्ठा fdt_reserve_entry),
				   fdt_आकार_dt_struct(fdt)))
		वापस -FDT_ERR_BADLAYOUT;
	अगर (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	वापस 0;
}

#घोषणा FDT_RW_PROBE(fdt) \
	{ \
		पूर्णांक err_; \
		अगर ((err_ = fdt_rw_probe_(fdt)) != 0) \
			वापस err_; \
	}

अटल अंतरभूत अचिन्हित पूर्णांक fdt_तथ्य_आकार(व्योम *fdt)
{
	वापस fdt_off_dt_strings(fdt) + fdt_आकार_dt_strings(fdt);
}

अटल पूर्णांक fdt_splice_(व्योम *fdt, व्योम *splicepoint, पूर्णांक oldlen, पूर्णांक newlen)
{
	अक्षर *p = splicepoint;
	अचिन्हित पूर्णांक dsize = fdt_तथ्य_आकार(fdt);
	माप_प्रकार soff = p - (अक्षर *)fdt;

	अगर ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		वापस -FDT_ERR_BADOFFSET;
	अगर ((p < (अक्षर *)fdt) || (dsize + newlen < (अचिन्हित)oldlen))
		वापस -FDT_ERR_BADOFFSET;
	अगर (dsize - oldlen + newlen > fdt_totalsize(fdt))
		वापस -FDT_ERR_NOSPACE;
	स_हटाओ(p + newlen, p + oldlen, ((अक्षर *)fdt + dsize) - (p + oldlen));
	वापस 0;
}

अटल पूर्णांक fdt_splice_mem_rsv_(व्योम *fdt, काष्ठा fdt_reserve_entry *p,
			       पूर्णांक oldn, पूर्णांक newn)
{
	पूर्णांक delta = (newn - oldn) * माप(*p);
	पूर्णांक err;
	err = fdt_splice_(fdt, p, oldn * माप(*p), newn * माप(*p));
	अगर (err)
		वापस err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	वापस 0;
}

अटल पूर्णांक fdt_splice_struct_(व्योम *fdt, व्योम *p,
			      पूर्णांक oldlen, पूर्णांक newlen)
{
	पूर्णांक delta = newlen - oldlen;
	पूर्णांक err;

	अगर ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		वापस err;

	fdt_set_आकार_dt_struct(fdt, fdt_आकार_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	वापस 0;
}

/* Must only be used to roll back in case of error */
अटल व्योम fdt_del_last_string_(व्योम *fdt, स्थिर अक्षर *s)
{
	पूर्णांक newlen = म_माप(s) + 1;

	fdt_set_आकार_dt_strings(fdt, fdt_आकार_dt_strings(fdt) - newlen);
}

अटल पूर्णांक fdt_splice_string_(व्योम *fdt, पूर्णांक newlen)
{
	व्योम *p = (अक्षर *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_आकार_dt_strings(fdt);
	पूर्णांक err;

	अगर ((err = fdt_splice_(fdt, p, 0, newlen)))
		वापस err;

	fdt_set_आकार_dt_strings(fdt, fdt_आकार_dt_strings(fdt) + newlen);
	वापस 0;
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
अटल पूर्णांक fdt_find_add_string_(व्योम *fdt, स्थिर अक्षर *s, पूर्णांक *allocated)
{
	अक्षर *strtab = (अक्षर *)fdt + fdt_off_dt_strings(fdt);
	स्थिर अक्षर *p;
	अक्षर *new;
	पूर्णांक len = म_माप(s) + 1;
	पूर्णांक err;

	अगर (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_आकार_dt_strings(fdt), s);
	अगर (p)
		/* found it */
		वापस (p - strtab);

	new = strtab + fdt_आकार_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	अगर (err)
		वापस err;

	अगर (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	स_नकल(new, s, len);
	वापस (new - strtab);
}

पूर्णांक fdt_add_mem_rsv(व्योम *fdt, uint64_t address, uint64_t आकार)
{
	काष्ठा fdt_reserve_entry *re;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_अंक_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	अगर (err)
		वापस err;

	re->address = cpu_to_fdt64(address);
	re->आकार = cpu_to_fdt64(आकार);
	वापस 0;
}

पूर्णांक fdt_del_mem_rsv(व्योम *fdt, पूर्णांक n)
{
	काष्ठा fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	अगर (n >= fdt_अंक_mem_rsv(fdt))
		वापस -FDT_ERR_NOTFOUND;

	वापस fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

अटल पूर्णांक fdt_resize_property_(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name,
				पूर्णांक len, काष्ठा fdt_property **prop)
{
	पूर्णांक oldlen;
	पूर्णांक err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	अगर (!*prop)
		वापस oldlen;

	अगर ((err = fdt_splice_struct_(fdt, (*prop)->तथ्य, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		वापस err;

	(*prop)->len = cpu_to_fdt32(len);
	वापस 0;
}

अटल पूर्णांक fdt_add_property_(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name,
			     पूर्णांक len, काष्ठा fdt_property **prop)
{
	पूर्णांक proplen;
	पूर्णांक nextoffset;
	पूर्णांक namestroff;
	पूर्णांक err;
	पूर्णांक allocated;

	अगर ((nextoffset = fdt_check_गाँठ_offset(fdt, nodeoffset)) < 0)
		वापस nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	अगर (namestroff < 0)
		वापस namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = माप(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	अगर (err) {
		/* Delete the string if we failed to add it */
		अगर (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		वापस err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	वापस 0;
}

पूर्णांक fdt_set_name(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name)
{
	अक्षर *namep;
	पूर्णांक oldlen, newlen;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	namep = (अक्षर *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	अगर (!namep)
		वापस oldlen;

	newlen = म_माप(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	अगर (err)
		वापस err;

	स_नकल(namep, name, newlen+1);
	वापस 0;
}

पूर्णांक fdt_setprop_placeholder(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name,
			    पूर्णांक len, व्योम **prop_तथ्य)
{
	काष्ठा fdt_property *prop;
	पूर्णांक err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	अगर (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	अगर (err)
		वापस err;

	*prop_तथ्य = prop->तथ्य;
	वापस 0;
}

पूर्णांक fdt_setprop(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name,
		स्थिर व्योम *val, पूर्णांक len)
{
	व्योम *prop_तथ्य;
	पूर्णांक err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_तथ्य);
	अगर (err)
		वापस err;

	अगर (len)
		स_नकल(prop_तथ्य, val, len);
	वापस 0;
}

पूर्णांक fdt_appendprop(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name,
		   स्थिर व्योम *val, पूर्णांक len)
{
	काष्ठा fdt_property *prop;
	पूर्णांक err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	अगर (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->तथ्य,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		अगर (err)
			वापस err;
		prop->len = cpu_to_fdt32(newlen);
		स_नकल(prop->तथ्य + oldlen, val, len);
	} अन्यथा {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		अगर (err)
			वापस err;
		स_नकल(prop->तथ्य, val, len);
	}
	वापस 0;
}

पूर्णांक fdt_delprop(व्योम *fdt, पूर्णांक nodeoffset, स्थिर अक्षर *name)
{
	काष्ठा fdt_property *prop;
	पूर्णांक len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	अगर (!prop)
		वापस len;

	proplen = माप(*prop) + FDT_TAGALIGN(len);
	वापस fdt_splice_struct_(fdt, prop, proplen, 0);
}

पूर्णांक fdt_add_subnode_namelen(व्योम *fdt, पूर्णांक parentoffset,
			    स्थिर अक्षर *name, पूर्णांक namelen)
{
	काष्ठा fdt_गाँठ_header *nh;
	पूर्णांक offset, nextoffset;
	पूर्णांक nodelen;
	पूर्णांक err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	अगर (offset >= 0)
		वापस -FDT_ERR_EXISTS;
	अन्यथा अगर (offset != -FDT_ERR_NOTFOUND)
		वापस offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_अग्र_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	अगर (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_गाँठ))
		वापस -FDT_ERR_INTERNAL;
	करो {
		offset = nextoffset;
		tag = fdt_अग्र_tag(fdt, offset, &nextoffset);
	} जबतक ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = माप(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	अगर (err)
		वापस err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_गाँठ);
	स_रखो(nh->name, 0, FDT_TAGALIGN(namelen+1));
	स_नकल(nh->name, name, namelen);
	endtag = (fdt32_t *)((अक्षर *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_गाँठ);

	वापस offset;
}

पूर्णांक fdt_add_subnode(व्योम *fdt, पूर्णांक parentoffset, स्थिर अक्षर *name)
{
	वापस fdt_add_subnode_namelen(fdt, parentoffset, name, म_माप(name));
}

पूर्णांक fdt_del_गाँठ(व्योम *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_गाँठ_end_offset(fdt, nodeoffset);
	अगर (endoffset < 0)
		वापस endoffset;

	वापस fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

अटल व्योम fdt_packblocks_(स्थिर अक्षर *old, अक्षर *new,
			    पूर्णांक mem_rsv_आकार,
			    पूर्णांक struct_आकार,
			    पूर्णांक strings_आकार)
{
	पूर्णांक mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(माप(काष्ठा fdt_header), 8);
	struct_off = mem_rsv_off + mem_rsv_आकार;
	strings_off = struct_off + struct_आकार;

	स_हटाओ(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_आकार);
	fdt_set_off_mem_rsvmap(new, mem_rsv_off);

	स_हटाओ(new + struct_off, old + fdt_off_dt_struct(old), struct_आकार);
	fdt_set_off_dt_struct(new, struct_off);
	fdt_set_आकार_dt_struct(new, struct_आकार);

	स_हटाओ(new + strings_off, old + fdt_off_dt_strings(old), strings_आकार);
	fdt_set_off_dt_strings(new, strings_off);
	fdt_set_आकार_dt_strings(new, fdt_आकार_dt_strings(old));
}

पूर्णांक fdt_open_into(स्थिर व्योम *fdt, व्योम *buf, पूर्णांक bufsize)
{
	पूर्णांक err;
	पूर्णांक mem_rsv_आकार, struct_आकार;
	पूर्णांक newsize;
	स्थिर अक्षर *fdtstart = fdt;
	स्थिर अक्षर *fdtend = fdtstart + fdt_totalsize(fdt);
	अक्षर *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_आकार = (fdt_अंक_mem_rsv(fdt)+1)
		* माप(काष्ठा fdt_reserve_entry);

	अगर (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_आकार = fdt_आकार_dt_struct(fdt);
	} अन्यथा अगर (fdt_version(fdt) == 16) {
		struct_आकार = 0;
		जबतक (fdt_अग्र_tag(fdt, struct_आकार, &struct_आकार) != FDT_END)
			;
		अगर (struct_आकार < 0)
			वापस struct_आकार;
	} अन्यथा {
		वापस -FDT_ERR_BADVERSION;
	}

	अगर (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_आकार, struct_आकार)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		अगर (err)
			वापस err;
		fdt_set_version(buf, 17);
		fdt_set_आकार_dt_struct(buf, struct_आकार);
		fdt_set_totalsize(buf, bufsize);
		वापस 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(माप(काष्ठा fdt_header), 8) + mem_rsv_आकार
		+ struct_आकार + fdt_आकार_dt_strings(fdt);

	अगर (bufsize < newsize)
		वापस -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	अगर (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (अक्षर *)(uintptr_t)fdtend;
		अगर ((tmp + newsize) > ((अक्षर *)buf + bufsize))
			वापस -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_आकार, struct_आकार,
			fdt_आकार_dt_strings(fdt));
	स_हटाओ(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	वापस 0;
}

पूर्णांक fdt_pack(व्योम *fdt)
{
	पूर्णांक mem_rsv_आकार;

	FDT_RW_PROBE(fdt);

	mem_rsv_आकार = (fdt_अंक_mem_rsv(fdt)+1)
		* माप(काष्ठा fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_आकार, fdt_आकार_dt_struct(fdt),
			fdt_आकार_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_तथ्य_आकार(fdt));

	वापस 0;
}
