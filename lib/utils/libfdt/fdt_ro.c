// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_nodename_eq_(ثابت فراغ *fdt, صحيح offset,
			    ثابت حرف *s, صحيح len)
{
	صحيح olen;
	ثابت حرف *p = fdt_get_name(fdt, offset, &olen);

	إذا (!p || olen < len)
		/* short match */
		إرجاع 0;

	إذا (memcmp(p, s, len) != 0)
		إرجاع 0;

	إذا (p[len] == '\0')
		إرجاع 1;
	وإلا إذا (!memchr(s, '@', len) && (p[len] == '@'))
		إرجاع 1;
	وإلا
		إرجاع 0;
}

ثابت حرف *fdt_get_string(ثابت فراغ *fdt, صحيح stroffset, صحيح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحيح err;
	ثابت حرف *s, *n;

	إذا (can_assume(VALID_إدخال)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		إذا (lenp)
			*lenp = strlen(s);
		إرجاع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	إذا (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	إذا (absoffset >= (unsigned)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	إذا (fdt_magic(fdt) == FDT_MAGIC) {
		إذا (stroffset < 0)
			اذهب fail;
		إذا (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			إذا ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				اذهب fail;
			إذا ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} وإلا إذا (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned صحيح sw_stroffset = -stroffset;

		إذا ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			اذهب fail;
		إذا (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_INTERNAL;
		اذهب fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	إذا (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		اذهب fail;
	}

	إذا (lenp)
		*lenp = n - s;
	إرجاع s;

fail:
	إذا (lenp)
		*lenp = err;
	إرجاع عدم;
}

ثابت حرف *fdt_string(ثابت فراغ *fdt, صحيح stroffset)
{
	إرجاع fdt_get_string(fdt, stroffset, عدم);
}

ساكن صحيح fdt_string_eq_(ثابت فراغ *fdt, صحيح stroffset,
			  ثابت حرف *s, صحيح len)
{
	صحيح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	إرجاع p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحيح fdt_find_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح offset = -1;

	بينما (true) {
		uint32_t قيمة;

		offset = fdt_next_عقدة(fdt, offset, عدم);
		إذا (offset < 0) {
			إذا (offset == -FDT_ERR_NOTFOUND)
				اكسر;

			إرجاع offset;
		}

		قيمة = fdt_get_phandle(fdt, offset);

		إذا (قيمة > max)
			max = قيمة;
	}

	إذا (phandle)
		*phandle = max;

	إرجاع 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_find_max_phandle(fdt, &max);
	إذا (err < 0)
		إرجاع err;

	إذا (max == FDT_MAX_PHANDLE)
		إرجاع -FDT_ERR_NOPHANDLES;

	إذا (phandle)
		*phandle = max + 1;

	إرجاع 0;
}

ساكن ثابت بنية fdt_reserve_entry *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	unsigned صحيح offset = n * حجم(بنية fdt_reserve_entry);
	unsigned صحيح absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	إذا (!can_assume(VALID_إدخال)) {
		إذا (absoffset < fdt_off_mem_rsvmap(fdt))
			إرجاع عدم;
		إذا (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_entry))
			إرجاع عدم;
	}
	إرجاع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *address, uint64_t *size)
{
	ثابت بنية fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	إذا (!can_assume(VALID_إدخال) && !re)
		إرجاع -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	إرجاع 0;
}

صحيح fdt_رقم_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_entry *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		إذا (fdt64_ld_(&re->size) == 0)
			إرجاع i;
	}
	إرجاع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح offset)
{
	uint32_t tag;
	صحيح nextoffset;

	افعل {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_END:
			إذا (nextoffset >= 0)
				إرجاع -FDT_ERR_BADSTRUCTURE;
			وإلا
				إرجاع nextoffset;

		حالة FDT_PROP:
			إرجاع offset;
		}
		offset = nextoffset;
	} بينما (tag == FDT_NOP);

	إرجاع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_offset_namelen(ثابت فراغ *fdt, صحيح offset,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح depth;

	FDT_RO_PROBE(fdt);

	لكل (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_عقدة(fdt, offset, &depth))
		إذا ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			إرجاع offset;

	إذا (depth < 0)
		إرجاع -FDT_ERR_NOTFOUND;
	إرجاع offset; /* error */
}

صحيح fdt_subnode_offset(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	إرجاع fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

صحيح fdt_path_offset_namelen(ثابت فراغ *fdt, ثابت حرف *path, صحيح namelen)
{
	ثابت حرف *end = path + namelen;
	ثابت حرف *p = path;
	صحيح offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	إذا (*path != '/') {
		ثابت حرف *q = memchr(path, '/', end - p);

		إذا (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		إذا (!p)
			إرجاع -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	بينما (p < end) {
		ثابت حرف *q;

		بينما (*p == '/') {
			p++;
			إذا (p == end)
				إرجاع offset;
		}
		q = memchr(p, '/', end - p);
		إذا (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		إذا (offset < 0)
			إرجاع offset;

		p = q;
	}

	إرجاع offset;
}

صحيح fdt_path_offset(ثابت فراغ *fdt, ثابت حرف *path)
{
	إرجاع fdt_path_offset_namelen(fdt, path, strlen(path));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_عقدة_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	إذا (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_عقدة_offset(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	إذا (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *leaf;
		leaf = strrchr(nameptr, '/');
		إذا (leaf == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = leaf+1;
	}

	إذا (len)
		*len = strlen(nameptr);

	إرجاع nameptr;

 fail:
	إذا (len)
		*len = err;
	إرجاع عدم;
}

صحيح fdt_first_property_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح offset;

	إذا ((offset = fdt_check_عقدة_offset(fdt, nodeoffset)) < 0)
		إرجاع offset;

	إرجاع nextprop_(fdt, offset);
}

صحيح fdt_next_property_offset(ثابت فراغ *fdt, صحيح offset)
{
	إذا ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		إرجاع offset;

	إرجاع nextprop_(fdt, offset);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_offset_(ثابت فراغ *fdt,
						              صحيح offset,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	إذا (!can_assume(VALID_إدخال) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		إذا (lenp)
			*lenp = err;
		إرجاع عدم;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	إذا (lenp)
		*lenp = fdt32_ld_(&prop->len);

	إرجاع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_offset(ثابت فراغ *fdt,
						      صحيح offset,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	إذا (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		إرجاع عدم;
	}

	إرجاع fdt_get_property_by_offset_(fdt, offset, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح offset,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	لكل (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		إذا (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			اكسر;
		}
		إذا (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			إذا (poffset)
				*poffset = offset;
			إرجاع prop;
		}
	}

	إذا (lenp)
		*lenp = offset;
	إرجاع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح offset,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	إذا (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		إذا (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		إرجاع عدم;
	}

	إرجاع fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	إرجاع fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ثابت فراغ *fdt_getprop_namelen(ثابت فراغ *fdt, صحيح nodeoffset,
				ثابت حرف *name, صحيح namelen, صحيح *lenp)
{
	صحيح poffset;
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	إذا (!prop)
		إرجاع عدم;

	/* Handle realignment */
	إذا (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		إرجاع prop->data + 4;
	إرجاع prop->data;
}

ثابت فراغ *fdt_getprop_by_offset(ثابت فراغ *fdt, صحيح offset,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	إذا (!prop)
		إرجاع عدم;
	إذا (namep) {
		ثابت حرف *name;
		صحيح namelen;

		إذا (!can_assume(VALID_إدخال)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			إذا (!name) {
				إذا (lenp)
					*lenp = namelen;
				إرجاع عدم;
			}
			*namep = name;
		} وإلا {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	إذا (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		إرجاع prop->data + 4;
	إرجاع prop->data;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	إرجاع fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ثابت فراغ *fdt, صحيح nodeoffset)
{
	ثابت fdt32_t *php;
	صحيح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	إذا (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		إذا (!php || (len != حجم(*php)))
			إرجاع 0;
	}

	إرجاع fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت فراغ *fdt,
				  ثابت حرف *name, صحيح namelen)
{
	صحيح aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	إذا (aliasoffset < 0)
		إرجاع عدم;

	إرجاع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	إرجاع fdt_get_alias_namelen(fdt, name, strlen(name));
}

صحيح fdt_get_path(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح offset, depth, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	إذا (buflen < 2)
		إرجاع -FDT_ERR_NOSPACE;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_عقدة(fdt, offset, &depth)) {
		بينما (pdepth > depth) {
			افعل {
				p--;
			} بينما (buf[p-1] != '/');
			pdepth--;
		}

		إذا (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			إذا (!name)
				إرجاع namelen;
			إذا ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		إذا (offset == nodeoffset) {
			إذا (pdepth < (depth + 1))
				إرجاع -FDT_ERR_NOSPACE;

			إذا (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			إرجاع 0;
		}
	}

	إذا ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		إرجاع -FDT_ERR_BADOFFSET;
	وإلا إذا (offset == -FDT_ERR_BADOFFSET)
		إرجاع -FDT_ERR_BADSTRUCTURE;

	إرجاع offset; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_offset(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح offset, depth;
	صحيح supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	إذا (supernodedepth < 0)
		إرجاع -FDT_ERR_NOTFOUND;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_عقدة(fdt, offset, &depth)) {
		إذا (depth == supernodedepth)
			supernodeoffset = offset;

		إذا (offset == nodeoffset) {
			إذا (nodedepth)
				*nodedepth = depth;

			إذا (supernodedepth > depth)
				إرجاع -FDT_ERR_NOTFOUND;
			وإلا
				إرجاع supernodeoffset;
		}
	}

	إذا (!can_assume(VALID_إدخال)) {
		إذا ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			إرجاع -FDT_ERR_BADOFFSET;
		وإلا إذا (offset == -FDT_ERR_BADOFFSET)
			إرجاع -FDT_ERR_BADSTRUCTURE;
	}

	إرجاع offset; /* error from fdt_next_node() */
}

صحيح fdt_عقدة_depth(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	إذا (err)
		إرجاع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	إرجاع nodedepth;
}

صحيح fdt_parent_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_عقدة_depth(fdt, nodeoffset);

	إذا (nodedepth < 0)
		إرجاع nodedepth;
	إرجاع fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_عقدة_offset_by_prop_قيمة(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *propname,
				  ثابت فراغ *propval, صحيح proplen)
{
	صحيح offset;
	ثابت فراغ *val;
	صحيح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	لكل (offset = fdt_next_عقدة(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_عقدة(fdt, offset, عدم)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		إذا (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			إرجاع offset;
	}

	إرجاع offset; /* error from fdt_next_node() */
}

صحيح fdt_عقدة_offset_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح offset;

	إذا ((phandle == 0) || (phandle == ~0U))
		إرجاع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (offset = fdt_next_عقدة(fdt, -1, عدم);
	     offset >= 0;
	     offset = fdt_next_عقدة(fdt, offset, عدم)) {
		إذا (fdt_get_phandle(fdt, offset) == phandle)
			إرجاع offset;
	}

	إرجاع offset; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_contains(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = strlen(str);
	ثابت حرف *p;

	بينما (listlen >= len) {
		إذا (memcmp(str, strlist, len+1) == 0)
			إرجاع 1;
		p = memchr(strlist, '\0', listlen);
		إذا (!p)
			إرجاع 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	إرجاع 0;
}

صحيح fdt_stringlist_تعداد(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *end;
	صحيح length, تعداد_2 = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	إذا (!list)
		إرجاع length;

	end = list + length;

	بينما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + length > end)
			إرجاع -FDT_ERR_BADVALUE;

		list += length;
		تعداد_2++;
	}

	إرجاع تعداد_2;
}

صحيح fdt_stringlist_بحث(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح length, len, idx = 0;
	ثابت حرف *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	إذا (!list)
		إرجاع length;

	len = strlen(string) + 1;
	end = list + length;

	بينما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + length > end)
			إرجاع -FDT_ERR_BADVALUE;

		إذا (length == len && memcmp(list, string, length) == 0)
			إرجاع idx;

		list += length;
		idx++;
	}

	إرجاع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *end;
	صحيح length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	إذا (!list) {
		إذا (lenp)
			*lenp = length;

		إرجاع عدم;
	}

	end = list + length;

	بينما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		إذا (list + length > end) {
			إذا (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			إرجاع عدم;
		}

		إذا (idx == 0) {
			إذا (lenp)
				*lenp = length - 1;

			إرجاع list;
		}

		list += length;
		idx--;
	}

	إذا (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	إرجاع عدم;
}

صحيح fdt_عقدة_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	إذا (!prop)
		إرجاع len;

	إرجاع !fdt_stringlist_contains(prop, len, compatible);
}

صحيح fdt_عقدة_offset_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	لكل (offset = fdt_next_عقدة(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_عقدة(fdt, offset, عدم)) {
		err = fdt_عقدة_check_compatible(fdt, offset, compatible);
		إذا ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			إرجاع err;
		وإلا إذا (err == 0)
			إرجاع offset;
	}

	إرجاع offset; /* error from fdt_next_node() */
}
