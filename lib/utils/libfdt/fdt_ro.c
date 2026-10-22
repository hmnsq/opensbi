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

	אם (!p || olen < len)
		/* short match */
		החזר 0;

	אם (memcmp(p, s, len) != 0)
		החזר 0;

	אם (p[len] == '\0')
		החזר 1;
	אחרת אם (!memchr(s, '@', len) && (p[len] == '@'))
		החזר 1;
	אחרת
		החזר 0;
}

ثابت حرف *fdt_get_string(ثابت فراغ *fdt, صحيح stroffset, صحيح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحيح err;
	ثابت حرف *s, *n;

	אם (can_assume(VALID_INPUT)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		אם (lenp)
			*lenp = strlen(s);
		החזר s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	אם (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	אם (absoffset >= (unsigned)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	אם (fdt_magic(fdt) == FDT_MAGIC) {
		אם (stroffset < 0)
			اذهب fail;
		אם (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			אם ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				اذهب fail;
			אם ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} אחרת אם (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned صحيح sw_stroffset = -stroffset;

		אם ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			اذهب fail;
		אם (sw_stroffset < len)
			len = sw_stroffset;
	} אחרת {
		err = -FDT_ERR_INTERNAL;
		اذهب fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	אם (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		اذهب fail;
	}

	אם (lenp)
		*lenp = n - s;
	החזר s;

fail:
	אם (lenp)
		*lenp = err;
	החזר عدم;
}

ثابت حرف *fdt_string(ثابت فراغ *fdt, صحيح stroffset)
{
	החזר fdt_get_string(fdt, stroffset, عدم);
}

ساكن صحيح fdt_string_eq_(ثابت فراغ *fdt, صحيح stroffset,
			  ثابت حرف *s, صحيح len)
{
	صحيح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	החזר p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحيح fdt_find_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح offset = -1;

	כלעוד (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, عدم);
		אם (offset < 0) {
			אם (offset == -FDT_ERR_NOTFOUND)
				اكسر;

			החזר offset;
		}

		value = fdt_get_phandle(fdt, offset);

		אם (value > max)
			max = value;
	}

	אם (phandle)
		*phandle = max;

	החזר 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_find_max_phandle(fdt, &max);
	אם (err < 0)
		החזר err;

	אם (max == FDT_MAX_PHANDLE)
		החזר -FDT_ERR_NOPHANDLES;

	אם (phandle)
		*phandle = max + 1;

	החזר 0;
}

ساكن ثابت بنية fdt_reserve_entry *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	unsigned صحيح offset = n * حجم(بنية fdt_reserve_entry);
	unsigned صحيح absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	אם (!can_assume(VALID_INPUT)) {
		אם (absoffset < fdt_off_mem_rsvmap(fdt))
			החזר عدم;
		אם (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_entry))
			החזר عدم;
	}
	החזר fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *address, uint64_t *size)
{
	ثابت بنية fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	אם (!can_assume(VALID_INPUT) && !re)
		החזר -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	החזר 0;
}

صحيح fdt_num_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_entry *re;

	עבור (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		אם (fdt64_ld_(&re->size) == 0)
			החזר i;
	}
	החזר -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح offset)
{
	uint32_t tag;
	صحيح nextoffset;

	עשה {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_END:
			אם (nextoffset >= 0)
				החזר -FDT_ERR_BADSTRUCTURE;
			אחרת
				החזר nextoffset;

		حالة FDT_PROP:
			החזר offset;
		}
		offset = nextoffset;
	} כלעוד (tag == FDT_NOP);

	החזר -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_offset_namelen(ثابت فراغ *fdt, صحيح offset,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح depth;

	FDT_RO_PROBE(fdt);

	עבור (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		אם ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			החזר offset;

	אם (depth < 0)
		החזר -FDT_ERR_NOTFOUND;
	החזר offset; /* error */
}

صحيح fdt_subnode_offset(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	החזר fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

صحيح fdt_path_offset_namelen(ثابت فراغ *fdt, ثابت حرف *path, صحيح namelen)
{
	ثابت حرف *end = path + namelen;
	ثابت حرف *p = path;
	صحيح offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	אם (*path != '/') {
		ثابت حرف *q = memchr(path, '/', end - p);

		אם (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		אם (!p)
			החזר -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	כלעוד (p < end) {
		ثابت حرف *q;

		כלעוד (*p == '/') {
			p++;
			אם (p == end)
				החזר offset;
		}
		q = memchr(p, '/', end - p);
		אם (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		אם (offset < 0)
			החזר offset;

		p = q;
	}

	החזר offset;
}

صحيح fdt_path_offset(ثابت فراغ *fdt, ثابت حرف *path)
{
	החזר fdt_path_offset_namelen(fdt, path, strlen(path));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	אם (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	אם (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *leaf;
		leaf = strrchr(nameptr, '/');
		אם (leaf == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = leaf+1;
	}

	אם (len)
		*len = strlen(nameptr);

	החזר nameptr;

 fail:
	אם (len)
		*len = err;
	החזר عدم;
}

صحيح fdt_first_property_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح offset;

	אם ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		החזר offset;

	החזר nextprop_(fdt, offset);
}

صحيح fdt_next_property_offset(ثابت فراغ *fdt, صحيح offset)
{
	אם ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		החזר offset;

	החזר nextprop_(fdt, offset);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_offset_(ثابت فراغ *fdt,
						              صحيح offset,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	אם (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		אם (lenp)
			*lenp = err;
		החזר عدم;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	אם (lenp)
		*lenp = fdt32_ld_(&prop->len);

	החזר prop;
}

ثابت بنية fdt_property *fdt_get_property_by_offset(ثابت فراغ *fdt,
						      صحيح offset,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	אם (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		אם (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		החזר عدم;
	}

	החזר fdt_get_property_by_offset_(fdt, offset, lenp);
}

ساكن ثابت بنية fdt_property *fdt_get_property_namelen_(ثابت فراغ *fdt,
						            صحيح offset,
						            ثابت حرف *name,
						            صحيح namelen,
							    صحيح *lenp,
							    صحيح *poffset)
{
	עבור (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ثابت بنية fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		אם (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			اكسر;
		}
		אם (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			אם (poffset)
				*poffset = offset;
			החזר prop;
		}
	}

	אם (lenp)
		*lenp = offset;
	החזר عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح offset,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	אם (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		אם (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		החזר عدم;
	}

	החזר fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	החזר fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ثابت فراغ *fdt_getprop_namelen(ثابت فراغ *fdt, صحيح nodeoffset,
				ثابت حرف *name, صحيح namelen, صحيح *lenp)
{
	صحيح poffset;
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	אם (!prop)
		החזר عدم;

	/* Handle realignment */
	אם (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		החזר prop->data + 4;
	החזר prop->data;
}

ثابت فراغ *fdt_getprop_by_offset(ثابت فراغ *fdt, صحيح offset,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	אם (!prop)
		החזר عدم;
	אם (namep) {
		ثابت حرف *name;
		صحيح namelen;

		אם (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			אם (!name) {
				אם (lenp)
					*lenp = namelen;
				החזר عدم;
			}
			*namep = name;
		} אחרת {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	אם (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		החזר prop->data + 4;
	החזר prop->data;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	החזר fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ثابت فراغ *fdt, صحيح nodeoffset)
{
	ثابت fdt32_t *php;
	صحيح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	אם (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		אם (!php || (len != حجم(*php)))
			החזר 0;
	}

	החזר fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت فراغ *fdt,
				  ثابت حرف *name, صحيح namelen)
{
	صحيح aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	אם (aliasoffset < 0)
		החזר عدم;

	החזר fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	החזר fdt_get_alias_namelen(fdt, name, strlen(name));
}

صحيح fdt_get_path(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح offset, depth, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	אם (buflen < 2)
		החזר -FDT_ERR_NOSPACE;

	עבור (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		כלעוד (pdepth > depth) {
			עשה {
				p--;
			} כלעוד (buf[p-1] != '/');
			pdepth--;
		}

		אם (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			אם (!name)
				החזר namelen;
			אם ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		אם (offset == nodeoffset) {
			אם (pdepth < (depth + 1))
				החזר -FDT_ERR_NOSPACE;

			אם (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			החזר 0;
		}
	}

	אם ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		החזר -FDT_ERR_BADOFFSET;
	אחרת אם (offset == -FDT_ERR_BADOFFSET)
		החזר -FDT_ERR_BADSTRUCTURE;

	החזר offset; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_offset(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح offset, depth;
	صحيح supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	אם (supernodedepth < 0)
		החזר -FDT_ERR_NOTFOUND;

	עבור (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		אם (depth == supernodedepth)
			supernodeoffset = offset;

		אם (offset == nodeoffset) {
			אם (nodedepth)
				*nodedepth = depth;

			אם (supernodedepth > depth)
				החזר -FDT_ERR_NOTFOUND;
			אחרת
				החזר supernodeoffset;
		}
	}

	אם (!can_assume(VALID_INPUT)) {
		אם ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			החזר -FDT_ERR_BADOFFSET;
		אחרת אם (offset == -FDT_ERR_BADOFFSET)
			החזר -FDT_ERR_BADSTRUCTURE;
	}

	החזר offset; /* error from fdt_next_node() */
}

صحيح fdt_node_depth(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	אם (err)
		החזר (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	החזר nodedepth;
}

صحيح fdt_parent_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_node_depth(fdt, nodeoffset);

	אם (nodedepth < 0)
		החזר nodedepth;
	החזר fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, عدم);
}

صحيح fdt_node_offset_by_prop_value(ثابت فراغ *fdt, صحيح startoffset,
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
	עבור (offset = fdt_next_node(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		אם (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			החזר offset;
	}

	החזר offset; /* error from fdt_next_node() */
}

صحيح fdt_node_offset_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح offset;

	אם ((phandle == 0) || (phandle == ~0U))
		החזר -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	עבור (offset = fdt_next_node(fdt, -1, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		אם (fdt_get_phandle(fdt, offset) == phandle)
			החזר offset;
	}

	החזר offset; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_contains(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = strlen(str);
	ثابت حرف *p;

	כלעוד (listlen >= len) {
		אם (memcmp(str, strlist, len+1) == 0)
			החזר 1;
		p = memchr(strlist, '\0', listlen);
		אם (!p)
			החזר 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	החזר 0;
}

صحيح fdt_stringlist_count(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *end;
	صحيح length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	אם (!list)
		החזר length;

	end = list + length;

	כלעוד (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		אם (list + length > end)
			החזר -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	החזר count;
}

صحيح fdt_stringlist_search(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح length, len, idx = 0;
	ثابت حرف *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	אם (!list)
		החזר length;

	len = strlen(string) + 1;
	end = list + length;

	כלעוד (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		אם (list + length > end)
			החזר -FDT_ERR_BADVALUE;

		אם (length == len && memcmp(list, string, length) == 0)
			החזר idx;

		list += length;
		idx++;
	}

	החזר -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *end;
	صحيح length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	אם (!list) {
		אם (lenp)
			*lenp = length;

		החזר عدم;
	}

	end = list + length;

	כלעוד (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		אם (list + length > end) {
			אם (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			החזר عدم;
		}

		אם (idx == 0) {
			אם (lenp)
				*lenp = length - 1;

			החזר list;
		}

		list += length;
		idx--;
	}

	אם (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	החזר عدم;
}

صحيح fdt_node_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	אם (!prop)
		החזר len;

	החזר !fdt_stringlist_contains(prop, len, compatible);
}

صحيح fdt_node_offset_by_compatible(ثابت فراغ *fdt, صحيح startoffset,
				  ثابت حرف *compatible)
{
	صحيح offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	עבור (offset = fdt_next_node(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		אם ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			החזר err;
		אחרת אם (err == 0)
			החזר offset;
	}

	החזר offset; /* error from fdt_next_node() */
}
