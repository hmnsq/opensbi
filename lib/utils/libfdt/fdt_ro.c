// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ایستا صحیح fdt_nodename_eq_(ثابت تهی *fdt, صحیح offset,
			    ثابت نویسه *s, صحیح len)
{
	صحیح olen;
	ثابت نویسه *p = fdt_get_name(fdt, offset, &olen);

	اگر (!p || olen < len)
		/* short match */
		بازگشت 0;

	اگر (memcmp(p, s, len) != 0)
		بازگشت 0;

	اگر (p[len] == '\0')
		بازگشت 1;
	وگرنه اگر (!memchr(s, '@', len) && (p[len] == '@'))
		بازگشت 1;
	وگرنه
		بازگشت 0;
}

ثابت نویسه *fdt_get_string(ثابت تهی *fdt, صحیح stroffset, صحیح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحیح err;
	ثابت نویسه *s, *n;

	اگر (can_assume(VALID_درون_داد)) {
		s = (ثابت نویسه *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		اگر (lenp)
			*lenp = strlen(s);
		بازگشت s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	اگر (totalsize < 0)
		برو fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	اگر (absoffset >= (unsigned)totalsize)
		برو fail;
	len = totalsize - absoffset;

	اگر (fdt_magic(fdt) == FDT_MAGIC) {
		اگر (stroffset < 0)
			برو fail;
		اگر (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			اگر ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				برو fail;
			اگر ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} وگرنه اگر (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned صحیح sw_stroffset = -stroffset;

		اگر ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			برو fail;
		اگر (sw_stroffset < len)
			len = sw_stroffset;
	} وگرنه {
		err = -FDT_ERR_INTERNAL;
		برو fail;
	}

	s = (ثابت نویسه *)fdt + absoffset;
	n = memchr(s, '\0', len);
	اگر (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		برو fail;
	}

	اگر (lenp)
		*lenp = n - s;
	بازگشت s;

fail:
	اگر (lenp)
		*lenp = err;
	بازگشت پوچ;
}

ثابت نویسه *fdt_string(ثابت تهی *fdt, صحیح stroffset)
{
	بازگشت fdt_get_string(fdt, stroffset, پوچ);
}

ایستا صحیح fdt_string_eq_(ثابت تهی *fdt, صحیح stroffset,
			  ثابت نویسه *s, صحیح len)
{
	صحیح slen;
	ثابت نویسه *p = fdt_get_string(fdt, stroffset, &slen);

	بازگشت p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحیح fdt_find_max_phandle(ثابت تهی *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحیح offset = -1;

	وقتی (true) {
		uint32_t مقدار;

		offset = fdt_next_گره(fdt, offset, پوچ);
		اگر (offset < 0) {
			اگر (offset == -FDT_ERR_NOTFOUND)
				بشکن;

			بازگشت offset;
		}

		مقدار = fdt_get_phandle(fdt, offset);

		اگر (مقدار > max)
			max = مقدار;
	}

	اگر (phandle)
		*phandle = max;

	بازگشت 0;
}

صحیح fdt_generate_phandle(ثابت تهی *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحیح err;

	err = fdt_find_max_phandle(fdt, &max);
	اگر (err < 0)
		بازگشت err;

	اگر (max == FDT_MAX_PHANDLE)
		بازگشت -FDT_ERR_NOPHANDLES;

	اگر (phandle)
		*phandle = max + 1;

	بازگشت 0;
}

ایستا ثابت ساختار fdt_reserve_entry *fdt_mem_rsv(ثابت تهی *fdt, صحیح n)
{
	unsigned صحیح offset = n * اندازه(ساختار fdt_reserve_entry);
	unsigned صحیح absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	اگر (!can_assume(VALID_درون_داد)) {
		اگر (absoffset < fdt_off_mem_rsvmap(fdt))
			بازگشت پوچ;
		اگر (absoffset > fdt_totalsize(fdt) -
		    اندازه(ساختار fdt_reserve_entry))
			بازگشت پوچ;
	}
	بازگشت fdt_mem_rsv_(fdt, n);
}

صحیح fdt_get_mem_rsv(ثابت تهی *fdt, صحیح n, uint64_t *address, uint64_t *size)
{
	ثابت ساختار fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	اگر (!can_assume(VALID_درون_داد) && !re)
		بازگشت -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	بازگشت 0;
}

صحیح fdt_رقم_mem_rsv(ثابت تهی *fdt)
{
	صحیح i;
	ثابت ساختار fdt_reserve_entry *re;

	برای (i = 0; (re = fdt_mem_rsv(fdt, i)) != پوچ; i++) {
		اگر (fdt64_ld_(&re->size) == 0)
			بازگشت i;
	}
	بازگشت -FDT_ERR_TRUNCATED;
}

ایستا صحیح nextprop_(ثابت تهی *fdt, صحیح offset)
{
	uint32_t tag;
	صحیح nextoffset;

	انجامبده {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		انتخاب (tag) {
		حالت FDT_END:
			اگر (nextoffset >= 0)
				بازگشت -FDT_ERR_BADSTRUCTURE;
			وگرنه
				بازگشت nextoffset;

		حالت FDT_PROP:
			بازگشت offset;
		}
		offset = nextoffset;
	} وقتی (tag == FDT_NOP);

	بازگشت -FDT_ERR_NOTFOUND;
}

صحیح fdt_subnode_offset_namelen(ثابت تهی *fdt, صحیح offset,
			       ثابت نویسه *name, صحیح namelen)
{
	صحیح depth;

	FDT_RO_PROBE(fdt);

	برای (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_گره(fdt, offset, &depth))
		اگر ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			بازگشت offset;

	اگر (depth < 0)
		بازگشت -FDT_ERR_NOTFOUND;
	بازگشت offset; /* error */
}

صحیح fdt_subnode_offset(ثابت تهی *fdt, صحیح parentoffset,
		       ثابت نویسه *name)
{
	بازگشت fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

صحیح fdt_path_offset_namelen(ثابت تهی *fdt, ثابت نویسه *path, صحیح namelen)
{
	ثابت نویسه *end = path + namelen;
	ثابت نویسه *p = path;
	صحیح offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	اگر (*path != '/') {
		ثابت نویسه *q = memchr(path, '/', end - p);

		اگر (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		اگر (!p)
			بازگشت -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	وقتی (p < end) {
		ثابت نویسه *q;

		وقتی (*p == '/') {
			p++;
			اگر (p == end)
				بازگشت offset;
		}
		q = memchr(p, '/', end - p);
		اگر (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		اگر (offset < 0)
			بازگشت offset;

		p = q;
	}

	بازگشت offset;
}

صحیح fdt_path_offset(ثابت تهی *fdt, ثابت نویسه *path)
{
	بازگشت fdt_path_offset_namelen(fdt, path, strlen(path));
}

ثابت نویسه *fdt_get_name(ثابت تهی *fdt, صحیح nodeoffset, صحیح *len)
{
	ثابت ساختار fdt_گره_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ثابت نویسه *nameptr;
	صحیح err;

	اگر (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_گره_offset(fdt, nodeoffset)) < 0))
			برو fail;

	nameptr = nh->name;

	اگر (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت نویسه *leaf;
		leaf = strrchr(nameptr, '/');
		اگر (leaf == پوچ) {
			err = -FDT_ERR_BADSTRUCTURE;
			برو fail;
		}
		nameptr = leaf+1;
	}

	اگر (len)
		*len = strlen(nameptr);

	بازگشت nameptr;

 fail:
	اگر (len)
		*len = err;
	بازگشت پوچ;
}

صحیح fdt_first_property_offset(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح offset;

	اگر ((offset = fdt_check_گره_offset(fdt, nodeoffset)) < 0)
		بازگشت offset;

	بازگشت nextprop_(fdt, offset);
}

صحیح fdt_next_property_offset(ثابت تهی *fdt, صحیح offset)
{
	اگر ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		بازگشت offset;

	بازگشت nextprop_(fdt, offset);
}

ایستا ثابت ساختار fdt_property *fdt_get_property_by_offset_(ثابت تهی *fdt,
						              صحیح offset,
						              صحیح *lenp)
{
	صحیح err;
	ثابت ساختار fdt_property *prop;

	اگر (!can_assume(VALID_درون_داد) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		اگر (lenp)
			*lenp = err;
		بازگشت پوچ;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	اگر (lenp)
		*lenp = fdt32_ld_(&prop->len);

	بازگشت prop;
}

ثابت ساختار fdt_property *fdt_get_property_by_offset(ثابت تهی *fdt,
						      صحیح offset,
						      صحیح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	اگر (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		اگر (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		بازگشت پوچ;
	}

	بازگشت fdt_get_property_by_offset_(fdt, offset, lenp);
}

ایستا ثابت ساختار fdt_property *fdt_get_property_namelen_(ثابت تهی *fdt,
						            صحیح offset,
						            ثابت نویسه *name,
						            صحیح namelen,
							    صحیح *lenp,
							    صحیح *poffset)
{
	برای (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ثابت ساختار fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		اگر (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			بشکن;
		}
		اگر (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			اگر (poffset)
				*poffset = offset;
			بازگشت prop;
		}
	}

	اگر (lenp)
		*lenp = offset;
	بازگشت پوچ;
}


ثابت ساختار fdt_property *fdt_get_property_namelen(ثابت تهی *fdt,
						    صحیح offset,
						    ثابت نویسه *name,
						    صحیح namelen, صحیح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	اگر (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		اگر (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		بازگشت پوچ;
	}

	بازگشت fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 پوچ);
}


ثابت ساختار fdt_property *fdt_get_property(ثابت تهی *fdt,
					    صحیح nodeoffset,
					    ثابت نویسه *name, صحیح *lenp)
{
	بازگشت fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ثابت تهی *fdt_getprop_namelen(ثابت تهی *fdt, صحیح nodeoffset,
				ثابت نویسه *name, صحیح namelen, صحیح *lenp)
{
	صحیح poffset;
	ثابت ساختار fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	اگر (!prop)
		بازگشت پوچ;

	/* Handle realignment */
	اگر (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + اندازه(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		بازگشت prop->data + 4;
	بازگشت prop->data;
}

ثابت تهی *fdt_getprop_by_offset(ثابت تهی *fdt, صحیح offset,
				  ثابت نویسه **namep, صحیح *lenp)
{
	ثابت ساختار fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	اگر (!prop)
		بازگشت پوچ;
	اگر (namep) {
		ثابت نویسه *name;
		صحیح namelen;

		اگر (!can_assume(VALID_درون_داد)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			اگر (!name) {
				اگر (lenp)
					*lenp = namelen;
				بازگشت پوچ;
			}
			*namep = name;
		} وگرنه {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	اگر (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + اندازه(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		بازگشت prop->data + 4;
	بازگشت prop->data;
}

ثابت تهی *fdt_getprop(ثابت تهی *fdt, صحیح nodeoffset,
			ثابت نویسه *name, صحیح *lenp)
{
	بازگشت fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ثابت تهی *fdt, صحیح nodeoffset)
{
	ثابت fdt32_t *php;
	صحیح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	اگر (!php || (len != اندازه(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		اگر (!php || (len != اندازه(*php)))
			بازگشت 0;
	}

	بازگشت fdt32_ld_(php);
}

ثابت نویسه *fdt_get_alias_namelen(ثابت تهی *fdt,
				  ثابت نویسه *name, صحیح namelen)
{
	صحیح aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	اگر (aliasoffset < 0)
		بازگشت پوچ;

	بازگشت fdt_getprop_namelen(fdt, aliasoffset, name, namelen, پوچ);
}

ثابت نویسه *fdt_get_alias(ثابت تهی *fdt, ثابت نویسه *name)
{
	بازگشت fdt_get_alias_namelen(fdt, name, strlen(name));
}

صحیح fdt_get_path(ثابت تهی *fdt, صحیح nodeoffset, نویسه *buf, صحیح buflen)
{
	صحیح pdepth = 0, p = 0;
	صحیح offset, depth, namelen;
	ثابت نویسه *name;

	FDT_RO_PROBE(fdt);

	اگر (buflen < 2)
		بازگشت -FDT_ERR_NOSPACE;

	برای (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_گره(fdt, offset, &depth)) {
		وقتی (pdepth > depth) {
			انجامبده {
				p--;
			} وقتی (buf[p-1] != '/');
			pdepth--;
		}

		اگر (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			اگر (!name)
				بازگشت namelen;
			اگر ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		اگر (offset == nodeoffset) {
			اگر (pdepth < (depth + 1))
				بازگشت -FDT_ERR_NOSPACE;

			اگر (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			بازگشت 0;
		}
	}

	اگر ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		بازگشت -FDT_ERR_BADOFFSET;
	وگرنه اگر (offset == -FDT_ERR_BADOFFSET)
		بازگشت -FDT_ERR_BADSTRUCTURE;

	بازگشت offset; /* error from fdt_next_node() */
}

صحیح fdt_supernode_atdepth_offset(ثابت تهی *fdt, صحیح nodeoffset,
				 صحیح supernodedepth, صحیح *nodedepth)
{
	صحیح offset, depth;
	صحیح supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	اگر (supernodedepth < 0)
		بازگشت -FDT_ERR_NOTFOUND;

	برای (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_گره(fdt, offset, &depth)) {
		اگر (depth == supernodedepth)
			supernodeoffset = offset;

		اگر (offset == nodeoffset) {
			اگر (nodedepth)
				*nodedepth = depth;

			اگر (supernodedepth > depth)
				بازگشت -FDT_ERR_NOTFOUND;
			وگرنه
				بازگشت supernodeoffset;
		}
	}

	اگر (!can_assume(VALID_درون_داد)) {
		اگر ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			بازگشت -FDT_ERR_BADOFFSET;
		وگرنه اگر (offset == -FDT_ERR_BADOFFSET)
			بازگشت -FDT_ERR_BADSTRUCTURE;
	}

	بازگشت offset; /* error from fdt_next_node() */
}

صحیح fdt_گره_depth(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح nodedepth;
	صحیح err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	اگر (err)
		بازگشت (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	بازگشت nodedepth;
}

صحیح fdt_parent_offset(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح nodedepth = fdt_گره_depth(fdt, nodeoffset);

	اگر (nodedepth < 0)
		بازگشت nodedepth;
	بازگشت fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, پوچ);
}

صحیح fdt_گره_offset_by_prop_مقدار(ثابت تهی *fdt, صحیح startoffset,
				  ثابت نویسه *propname,
				  ثابت تهی *propval, صحیح proplen)
{
	صحیح offset;
	ثابت تهی *val;
	صحیح len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	برای (offset = fdt_next_گره(fdt, startoffset, پوچ);
	     offset >= 0;
	     offset = fdt_next_گره(fdt, offset, پوچ)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		اگر (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			بازگشت offset;
	}

	بازگشت offset; /* error from fdt_next_node() */
}

صحیح fdt_گره_offset_by_phandle(ثابت تهی *fdt, uint32_t phandle)
{
	صحیح offset;

	اگر ((phandle == 0) || (phandle == ~0U))
		بازگشت -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	برای (offset = fdt_next_گره(fdt, -1, پوچ);
	     offset >= 0;
	     offset = fdt_next_گره(fdt, offset, پوچ)) {
		اگر (fdt_get_phandle(fdt, offset) == phandle)
			بازگشت offset;
	}

	بازگشت offset; /* error from fdt_next_node() */
}

صحیح fdt_stringlist_contains(ثابت نویسه *strlist, صحیح listlen, ثابت نویسه *str)
{
	صحیح len = strlen(str);
	ثابت نویسه *p;

	وقتی (listlen >= len) {
		اگر (memcmp(str, strlist, len+1) == 0)
			بازگشت 1;
		p = memchr(strlist, '\0', listlen);
		اگر (!p)
			بازگشت 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	بازگشت 0;
}

صحیح fdt_stringlist_شمار(ثابت تهی *fdt, صحیح nodeoffset, ثابت نویسه *property)
{
	ثابت نویسه *list, *end;
	صحیح length, شمار = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	اگر (!list)
		بازگشت length;

	end = list + length;

	وقتی (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + length > end)
			بازگشت -FDT_ERR_BADVALUE;

		list += length;
		شمار++;
	}

	بازگشت شمار;
}

صحیح fdt_stringlist_جستجو(ثابت تهی *fdt, صحیح nodeoffset, ثابت نویسه *property,
			  ثابت نویسه *string)
{
	صحیح length, len, idx = 0;
	ثابت نویسه *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	اگر (!list)
		بازگشت length;

	len = strlen(string) + 1;
	end = list + length;

	وقتی (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + length > end)
			بازگشت -FDT_ERR_BADVALUE;

		اگر (length == len && memcmp(list, string, length) == 0)
			بازگشت idx;

		list += length;
		idx++;
	}

	بازگشت -FDT_ERR_NOTFOUND;
}

ثابت نویسه *fdt_stringlist_get(ثابت تهی *fdt, صحیح nodeoffset,
			       ثابت نویسه *property, صحیح idx,
			       صحیح *lenp)
{
	ثابت نویسه *list, *end;
	صحیح length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	اگر (!list) {
		اگر (lenp)
			*lenp = length;

		بازگشت پوچ;
	}

	end = list + length;

	وقتی (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		اگر (list + length > end) {
			اگر (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			بازگشت پوچ;
		}

		اگر (idx == 0) {
			اگر (lenp)
				*lenp = length - 1;

			بازگشت list;
		}

		list += length;
		idx--;
	}

	اگر (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	بازگشت پوچ;
}

صحیح fdt_گره_check_compatible(ثابت تهی *fdt, صحیح nodeoffset,
			      ثابت نویسه *compatible)
{
	ثابت تهی *prop;
	صحیح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	اگر (!prop)
		بازگشت len;

	بازگشت !fdt_stringlist_contains(prop, len, compatible);
}

صحیح fdt_گره_offset_by_compatible(ثابت تهی *fdt, صحیح startoffset,
				  ثابت نویسه *compatible)
{
	صحیح offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	برای (offset = fdt_next_گره(fdt, startoffset, پوچ);
	     offset >= 0;
	     offset = fdt_next_گره(fdt, offset, پوچ)) {
		err = fdt_گره_check_compatible(fdt, offset, compatible);
		اگر ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			بازگشت err;
		وگرنه اگر (err == 0)
			بازگشت offset;
	}

	بازگشت offset; /* error from fdt_next_node() */
}
