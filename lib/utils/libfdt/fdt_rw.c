// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

स्थिर पूर्णाङ्क fdt_खण्डानि_misordered(ध्रुव शून्य *fdt,
				  पूर्णाङ्क mem_rsv_आकार, पूर्णाङ्क struct_आकार)
{
	प्रत्यागम (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(परिमाण(संरचना fdt_शीर्षकम्), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_आकार))
		|| (fdt_off_dt_वाक्यानि(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_आकार))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_वाक्यानि(fdt) + fdt_आकार_dt_वाक्यानि(fdt)));
}

स्थिर पूर्णाङ्क fdt_rw_probe_(शून्य *fdt)
{
	यदि (can_assume(VALID_DTB))
		प्रत्यागम 0;
	FDT_RO_PROBE(fdt);

	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) < 17)
		प्रत्यागम -FDT_ERR_BADVERSION;
	यदि (fdt_खण्डानि_misordered(fdt, परिमाण(संरचना fdt_reserve_प्रविष्टिः),
				   fdt_आकार_dt_struct(fdt)))
		प्रत्यागम -FDT_ERR_BADLAYOUT;
	यदि (!can_assume(LATEST) && fdt_संस्करणम्(fdt) > 17)
		fdt_समुच्चयः_संस्करणम्(fdt, 17);

	प्रत्यागम 0;
}

#परिभाषय FDT_RW_PROBE(fdt) \
	{ \
		पूर्णाङ्क err_; \
		यदि ((err_ = fdt_rw_probe_(fdt)) != 0) \
			प्रत्यागम err_; \
	}

स्थिर अन्तःस्थ अचिह्नित पूर्णाङ्क fdt_दत्त_आकार(शून्य *fdt)
{
	प्रत्यागम fdt_off_dt_वाक्यानि(fdt) + fdt_आकार_dt_वाक्यानि(fdt);
}

स्थिर पूर्णाङ्क fdt_splice_(शून्य *fdt, शून्य *splicepoint, पूर्णाङ्क oldlen, पूर्णाङ्क newlen)
{
	वर्ण *p = splicepoint;
	अचिह्नित पूर्णाङ्क dsize = fdt_दत्त_आकार(fdt);
	size_t soff = p - (वर्ण *)fdt;

	यदि ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	यदि ((p < (वर्ण *)fdt) || (dsize + newlen < (अचिह्नित)oldlen))
		प्रत्यागम -FDT_ERR_BADOFFSET;
	यदि (dsize - oldlen + newlen > fdt_totalsize(fdt))
		प्रत्यागम -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((वर्ण *)fdt + dsize) - (p + oldlen));
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_splice_mem_rsv_(शून्य *fdt, संरचना fdt_reserve_प्रविष्टिः *p,
			       पूर्णाङ्क oldn, पूर्णाङ्क newn)
{
	पूर्णाङ्क delta = (newn - oldn) * परिमाण(*p);
	पूर्णाङ्क err;
	err = fdt_splice_(fdt, p, oldn * परिमाण(*p), newn * परिमाण(*p));
	यदि (err)
		प्रत्यागम err;
	fdt_समुच्चयः_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_समुच्चयः_off_dt_वाक्यानि(fdt, fdt_off_dt_वाक्यानि(fdt) + delta);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_splice_struct_(शून्य *fdt, शून्य *p,
			      पूर्णाङ्क oldlen, पूर्णाङ्क newlen)
{
	पूर्णाङ्क delta = newlen - oldlen;
	पूर्णाङ्क err;

	यदि ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		प्रत्यागम err;

	fdt_समुच्चयः_आकार_dt_struct(fdt, fdt_आकार_dt_struct(fdt) + delta);
	fdt_समुच्चयः_off_dt_वाक्यानि(fdt, fdt_off_dt_वाक्यानि(fdt) + delta);
	प्रत्यागम 0;
}

/* Must only be used to roll back in case of error */
स्थिर शून्य fdt_del_चरमः_string(शून्य *fdt, ध्रुव वर्ण *s)
{
	पूर्णाङ्क newlen = वाक्यदैर्घ्यम्(s) + 1;

	fdt_समुच्चयः_आकार_dt_वाक्यानि(fdt, fdt_आकार_dt_वाक्यानि(fdt) - newlen);
}

स्थिर पूर्णाङ्क fdt_splice_string_(शून्य *fdt, पूर्णाङ्क newlen)
{
	शून्य *p = (वर्ण *)fdt
		+ fdt_off_dt_वाक्यानि(fdt) + fdt_आकार_dt_वाक्यानि(fdt);
	पूर्णाङ्क err;

	यदि ((err = fdt_splice_(fdt, p, 0, newlen)))
		प्रत्यागम err;

	fdt_समुच्चयः_आकार_dt_वाक्यानि(fdt, fdt_आकार_dt_वाक्यानि(fdt) + newlen);
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
स्थिर पूर्णाङ्क fdt_find_सङ्कलय_string(शून्य *fdt, ध्रुव वर्ण *s, पूर्णाङ्क *allocated)
{
	वर्ण *strtab = (वर्ण *)fdt + fdt_off_dt_वाक्यानि(fdt);
	ध्रुव वर्ण *p;
	वर्ण *new;
	पूर्णाङ्क len = वाक्यदैर्घ्यम्(s) + 1;
	पूर्णाङ्क err;

	यदि (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_आकार_dt_वाक्यानि(fdt), s);
	यदि (p)
		/* found it */
		प्रत्यागम (p - strtab);

	new = strtab + fdt_आकार_dt_वाक्यानि(fdt);
	err = fdt_splice_string_(fdt, len);
	यदि (err)
		प्रत्यागम err;

	यदि (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	स्मृतिं_प्रतिलिख(new, s, len);
	प्रत्यागम (new - strtab);
}

पूर्णाङ्क fdt_सङ्कलय_mem_rsv(शून्य *fdt, uint64_t स्थानाङ्कः, uint64_t आकार)
{
	संरचना fdt_reserve_प्रविष्टिः *re;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_अङ्कः_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	यदि (err)
		प्रत्यागम err;

	re->स्थानाङ्कः = cpu_गन्तव्यम्_fdt64(स्थानाङ्कः);
	re->आकार = cpu_गन्तव्यम्_fdt64(आकार);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_del_mem_rsv(शून्य *fdt, पूर्णाङ्क n)
{
	संरचना fdt_reserve_प्रविष्टिः *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	यदि (n >= fdt_अङ्कः_mem_rsv(fdt))
		प्रत्यागम -FDT_ERR_NOTFOUND;

	प्रत्यागम fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

स्थिर पूर्णाङ्क fdt_resize_property_(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
				पूर्णाङ्क len, संरचना fdt_property **prop)
{
	पूर्णाङ्क oldlen;
	पूर्णाङ्क err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	यदि (!*prop)
		प्रत्यागम oldlen;

	यदि ((err = fdt_splice_struct_(fdt, (*prop)->दत्त, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		प्रत्यागम err;

	(*prop)->len = cpu_गन्तव्यम्_fdt32(len);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क fdt_सङ्कलय_property(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			     पूर्णाङ्क len, संरचना fdt_property **prop)
{
	पूर्णाङ्क proplen;
	पूर्णाङ्क nextoffset;
	पूर्णाङ्क namestroff;
	पूर्णाङ्क err;
	पूर्णाङ्क allocated;

	यदि ((nextoffset = fdt_check_ग्रन्थिः_विस्थापनम्(fdt, nodeoffset)) < 0)
		प्रत्यागम nextoffset;

	namestroff = fdt_find_सङ्कलय_string(fdt, name, &allocated);
	यदि (namestroff < 0)
		प्रत्यागम namestroff;

	*prop = fdt_विस्थापनम्_ptr_w(fdt, nextoffset);
	proplen = परिमाण(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	यदि (err) {
		/* Delete the string if we failed to add it */
		यदि (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_चरमः_string(fdt, name);
		प्रत्यागम err;
	}

	(*prop)->चिह्नकम् = cpu_गन्तव्यम्_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_गन्तव्यम्_fdt32(namestroff);
	(*prop)->len = cpu_गन्तव्यम्_fdt32(len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_समुच्चयः_name(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	वर्ण *namep;
	पूर्णाङ्क oldlen, newlen;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	namep = (वर्ण *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	यदि (!namep)
		प्रत्यागम oldlen;

	newlen = वाक्यदैर्घ्यम्(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	यदि (err)
		प्रत्यागम err;

	स्मृतिं_प्रतिलिख(namep, name, newlen+1);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_setprop_placeholder(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
			    पूर्णाङ्क len, शून्य **prop_दत्त)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	यदि (err == -FDT_ERR_NOTFOUND)
		err = fdt_सङ्कलय_property(fdt, nodeoffset, name, len, &prop);
	यदि (err)
		प्रत्यागम err;

	*prop_दत्त = prop->दत्त;
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_setprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
		ध्रुव शून्य *val, पूर्णाङ्क len)
{
	शून्य *prop_दत्त;
	पूर्णाङ्क err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_दत्त);
	यदि (err)
		प्रत्यागम err;

	यदि (len)
		स्मृतिं_प्रतिलिख(prop_दत्त, val, len);
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_appendprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name,
		   ध्रुव शून्य *val, पूर्णाङ्क len)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	यदि (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->दत्त,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		यदि (err)
			प्रत्यागम err;
		prop->len = cpu_गन्तव्यम्_fdt32(newlen);
		स्मृतिं_प्रतिलिख(prop->दत्त + oldlen, val, len);
	} अन्यथा {
		err = fdt_सङ्कलय_property(fdt, nodeoffset, name, len, &prop);
		यदि (err)
			प्रत्यागम err;
		स्मृतिं_प्रतिलिख(prop->दत्त, val, len);
	}
	प्रत्यागम 0;
}

पूर्णाङ्क fdt_delprop(शून्य *fdt, पूर्णाङ्क nodeoffset, ध्रुव वर्ण *name)
{
	संरचना fdt_property *prop;
	पूर्णाङ्क len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	यदि (!prop)
		प्रत्यागम len;

	proplen = परिमाण(*prop) + FDT_TAGALIGN(len);
	प्रत्यागम fdt_splice_struct_(fdt, prop, proplen, 0);
}

पूर्णाङ्क fdt_सङ्कलय_subnode_namelen(शून्य *fdt, पूर्णाङ्क parentoffset,
			    ध्रुव वर्ण *name, पूर्णाङ्क namelen)
{
	संरचना fdt_ग्रन्थिः_शीर्षकम् *nh;
	पूर्णाङ्क विस्थापनम्, nextoffset;
	पूर्णाङ्क nodelen;
	पूर्णाङ्क err;
	uint32_t चिह्नकम्;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	विस्थापनम् = fdt_subnode_विस्थापनम्_namelen(fdt, parentoffset, name, namelen);
	यदि (विस्थापनम् >= 0)
		प्रत्यागम -FDT_ERR_EXISTS;
	अन्यथा यदि (विस्थापनम् != -FDT_ERR_NOTFOUND)
		प्रत्यागम विस्थापनम्;

	/* Try to place the new node after the parent's properties */
	चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	यदि (!can_assume(LIBFDT_FLAWLESS) && (चिह्नकम् != FDT_BEGIN_ग्रन्थिः))
		प्रत्यागम -FDT_ERR_आन्तरिकम्;
	कुरु {
		विस्थापनम् = nextoffset;
		चिह्नकम् = fdt_अग्र_चिह्नकम्(fdt, विस्थापनम्, &nextoffset);
	} यावत् ((चिह्नकम् == FDT_PROP) || (चिह्नकम् == FDT_NOP));

	nh = fdt_विस्थापनम्_ptr_w(fdt, विस्थापनम्);
	nodelen = परिमाण(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	यदि (err)
		प्रत्यागम err;

	nh->चिह्नकम् = cpu_गन्तव्यम्_fdt32(FDT_BEGIN_ग्रन्थिः);
	स्मृतिं_पूरय(nh->name, 0, FDT_TAGALIGN(namelen+1));
	स्मृतिं_प्रतिलिख(nh->name, name, namelen);
	endtag = (fdt32_t *)((वर्ण *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_गन्तव्यम्_fdt32(FDT_अन्तः_ग्रन्थिः);

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क fdt_सङ्कलय_subnode(शून्य *fdt, पूर्णाङ्क parentoffset, ध्रुव वर्ण *name)
{
	प्रत्यागम fdt_सङ्कलय_subnode_namelen(fdt, parentoffset, name, वाक्यदैर्घ्यम्(name));
}

पूर्णाङ्क fdt_del_ग्रन्थिः(शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ग्रन्थिः_अन्तः_विस्थापनम्(fdt, nodeoffset);
	यदि (endoffset < 0)
		प्रत्यागम endoffset;

	प्रत्यागम fdt_splice_struct_(fdt, fdt_विस्थापनम्_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

स्थिर शून्य fdt_packblocks_(ध्रुव वर्ण *old, वर्ण *new,
			    पूर्णाङ्क mem_rsv_आकार,
			    पूर्णाङ्क struct_आकार,
			    पूर्णाङ्क वाक्यानि_आकार)
{
	पूर्णाङ्क mem_rsv_off, struct_off, वाक्यानि_off;

	mem_rsv_off = FDT_ALIGN(परिमाण(संरचना fdt_शीर्षकम्), 8);
	struct_off = mem_rsv_off + mem_rsv_आकार;
	वाक्यानि_off = struct_off + struct_आकार;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_आकार);
	fdt_समुच्चयः_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_आकार);
	fdt_समुच्चयः_off_dt_struct(new, struct_off);
	fdt_समुच्चयः_आकार_dt_struct(new, struct_आकार);

	memmove(new + वाक्यानि_off, old + fdt_off_dt_वाक्यानि(old), वाक्यानि_आकार);
	fdt_समुच्चयः_off_dt_वाक्यानि(new, वाक्यानि_off);
	fdt_समुच्चयः_आकार_dt_वाक्यानि(new, fdt_आकार_dt_वाक्यानि(old));
}

पूर्णाङ्क fdt_open_into(ध्रुव शून्य *fdt, शून्य *buf, पूर्णाङ्क bufsize)
{
	पूर्णाङ्क err;
	पूर्णाङ्क mem_rsv_आकार, struct_आकार;
	पूर्णाङ्क newsize;
	ध्रुव वर्ण *fdtstart = fdt;
	ध्रुव वर्ण *fdtend = fdtstart + fdt_totalsize(fdt);
	वर्ण *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_आकार = (fdt_अङ्कः_mem_rsv(fdt)+1)
		* परिमाण(संरचना fdt_reserve_प्रविष्टिः);

	यदि (can_assume(LATEST) || fdt_संस्करणम्(fdt) >= 17) {
		struct_आकार = fdt_आकार_dt_struct(fdt);
	} अन्यथा यदि (fdt_संस्करणम्(fdt) == 16) {
		struct_आकार = 0;
		यावत् (fdt_अग्र_चिह्नकम्(fdt, struct_आकार, &struct_आकार) != FDT_अन्तः)
			;
		यदि (struct_आकार < 0)
			प्रत्यागम struct_आकार;
	} अन्यथा {
		प्रत्यागम -FDT_ERR_BADVERSION;
	}

	यदि (can_assume(LIBFDT_क्रमः) ||
	    !fdt_खण्डानि_misordered(fdt, mem_rsv_आकार, struct_आकार)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		यदि (err)
			प्रत्यागम err;
		fdt_समुच्चयः_संस्करणम्(buf, 17);
		fdt_समुच्चयः_आकार_dt_struct(buf, struct_आकार);
		fdt_समुच्चयः_totalsize(buf, bufsize);
		प्रत्यागम 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(परिमाण(संरचना fdt_शीर्षकम्), 8) + mem_rsv_आकार
		+ struct_आकार + fdt_आकार_dt_वाक्यानि(fdt);

	यदि (bufsize < newsize)
		प्रत्यागम -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	यदि (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (वर्ण *)(uintptr_t)fdtend;
		यदि ((tmp + newsize) > ((वर्ण *)buf + bufsize))
			प्रत्यागम -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_आकार, struct_आकार,
			fdt_आकार_dt_वाक्यानि(fdt));
	memmove(buf, tmp, newsize);

	fdt_समुच्चयः_magic(buf, FDT_MAGIC);
	fdt_समुच्चयः_totalsize(buf, bufsize);
	fdt_समुच्चयः_संस्करणम्(buf, 17);
	fdt_समुच्चयः_चरमः_comp_संस्करणम्(buf, 16);
	fdt_समुच्चयः_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_pack(शून्य *fdt)
{
	पूर्णाङ्क mem_rsv_आकार;

	FDT_RW_PROBE(fdt);

	mem_rsv_आकार = (fdt_अङ्कः_mem_rsv(fdt)+1)
		* परिमाण(संरचना fdt_reserve_प्रविष्टिः);
	fdt_packblocks_(fdt, fdt, mem_rsv_आकार, fdt_आकार_dt_struct(fdt),
			fdt_आकार_dt_वाक्यानि(fdt));
	fdt_समुच्चयः_totalsize(fdt, fdt_दत्त_आकार(fdt));

	प्रत्यागम 0;
}
