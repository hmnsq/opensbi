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

	𐤀𐤌 (!p || olen < len)
		/* short match */
		أرجع 0;

	𐤀𐤌 (memcmp(p, s, len) != 0)
		أرجع 0;

	𐤀𐤌 (p[len] == '\0')
		أرجع 1;
	وإلا 𐤀𐤌 (!memchr(s, '@', len) && (p[len] == '@'))
		أرجع 1;
	وإلا
		أرجع 0;
}

ثابت حرف *fdt_get_string(ثابت فراغ *fdt, صحيح stroffset, صحيح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحيح err;
	ثابت حرف *s, *n;

	𐤀𐤌 (can_assume(VALID_INPUT)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		𐤀𐤌 (lenp)
			*lenp = strlen(s);
		أرجع s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	𐤀𐤌 (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	𐤀𐤌 (absoffset >= (unsigned)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	𐤀𐤌 (fdt_magic(fdt) == FDT_MAGIC) {
		𐤀𐤌 (stroffset < 0)
			اذهب fail;
		𐤀𐤌 (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			𐤀𐤌 ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				اذهب fail;
			𐤀𐤌 ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} وإلا 𐤀𐤌 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned صحيح sw_stroffset = -stroffset;

		𐤀𐤌 ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			اذهب fail;
		𐤀𐤌 (sw_stroffset < len)
			len = sw_stroffset;
	} وإلا {
		err = -FDT_ERR_INTERNAL;
		اذهب fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	𐤀𐤌 (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		اذهب fail;
	}

	𐤀𐤌 (lenp)
		*lenp = n - s;
	أرجع s;

fail:
	𐤀𐤌 (lenp)
		*lenp = err;
	أرجع عدم;
}

ثابت حرف *fdt_string(ثابت فراغ *fdt, صحيح stroffset)
{
	أرجع fdt_get_string(fdt, stroffset, عدم);
}

ساكن صحيح fdt_string_eq_(ثابت فراغ *fdt, صحيح stroffset,
			  ثابت حرف *s, صحيح len)
{
	صحيح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	أرجع p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحيح fdt_find_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح offset = -1;

	طالما (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, عدم);
		𐤀𐤌 (offset < 0) {
			𐤀𐤌 (offset == -FDT_ERR_NOTFOUND)
				اكسر;

			أرجع offset;
		}

		value = fdt_get_phandle(fdt, offset);

		𐤀𐤌 (value > max)
			max = value;
	}

	𐤀𐤌 (phandle)
		*phandle = max;

	أرجع 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_find_max_phandle(fdt, &max);
	𐤀𐤌 (err < 0)
		أرجع err;

	𐤀𐤌 (max == FDT_MAX_PHANDLE)
		أرجع -FDT_ERR_NOPHANDLES;

	𐤀𐤌 (phandle)
		*phandle = max + 1;

	أرجع 0;
}

ساكن ثابت بنية fdt_reserve_entry *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	unsigned صحيح offset = n * حجم(بنية fdt_reserve_entry);
	unsigned صحيح absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	𐤀𐤌 (!can_assume(VALID_INPUT)) {
		𐤀𐤌 (absoffset < fdt_off_mem_rsvmap(fdt))
			أرجع عدم;
		𐤀𐤌 (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_entry))
			أرجع عدم;
	}
	أرجع fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *address, uint64_t *size)
{
	ثابت بنية fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	𐤀𐤌 (!can_assume(VALID_INPUT) && !re)
		أرجع -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	أرجع 0;
}

صحيح fdt_num_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_entry *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		𐤀𐤌 (fdt64_ld_(&re->size) == 0)
			أرجع i;
	}
	أرجع -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح offset)
{
	uint32_t tag;
	صحيح nextoffset;

	𐤐𐤏𐤋 {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_END:
			𐤀𐤌 (nextoffset >= 0)
				أرجع -FDT_ERR_BADSTRUCTURE;
			وإلا
				أرجع nextoffset;

		حالة FDT_PROP:
			أرجع offset;
		}
		offset = nextoffset;
	} طالما (tag == FDT_NOP);

	أرجع -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_offset_namelen(ثابت فراغ *fdt, صحيح offset,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح depth;

	FDT_RO_PROBE(fdt);

	لكل (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		𐤀𐤌 ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			أرجع offset;

	𐤀𐤌 (depth < 0)
		أرجع -FDT_ERR_NOTFOUND;
	أرجع offset; /* error */
}

صحيح fdt_subnode_offset(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	أرجع fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

صحيح fdt_path_offset_namelen(ثابت فراغ *fdt, ثابت حرف *path, صحيح namelen)
{
	ثابت حرف *end = path + namelen;
	ثابت حرف *p = path;
	صحيح offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	𐤀𐤌 (*path != '/') {
		ثابت حرف *q = memchr(path, '/', end - p);

		𐤀𐤌 (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		𐤀𐤌 (!p)
			أرجع -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	طالما (p < end) {
		ثابت حرف *q;

		طالما (*p == '/') {
			p++;
			𐤀𐤌 (p == end)
				أرجع offset;
		}
		q = memchr(p, '/', end - p);
		𐤀𐤌 (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		𐤀𐤌 (offset < 0)
			أرجع offset;

		p = q;
	}

	أرجع offset;
}

صحيح fdt_path_offset(ثابت فراغ *fdt, ثابت حرف *path)
{
	أرجع fdt_path_offset_namelen(fdt, path, strlen(path));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	𐤀𐤌 (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *leaf;
		leaf = strrchr(nameptr, '/');
		𐤀𐤌 (leaf == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = leaf+1;
	}

	𐤀𐤌 (len)
		*len = strlen(nameptr);

	أرجع nameptr;

 fail:
	𐤀𐤌 (len)
		*len = err;
	أرجع عدم;
}

صحيح fdt_first_property_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح offset;

	𐤀𐤌 ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		أرجع offset;

	أرجع nextprop_(fdt, offset);
}

صحيح fdt_next_property_offset(ثابت فراغ *fdt, صحيح offset)
{
	𐤀𐤌 ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		أرجع offset;

	أرجع nextprop_(fdt, offset);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_offset_(ثابت فراغ *fdt,
						              صحيح offset,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	𐤀𐤌 (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		𐤀𐤌 (lenp)
			*lenp = err;
		أرجع عدم;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	𐤀𐤌 (lenp)
		*lenp = fdt32_ld_(&prop->len);

	أرجع prop;
}

ثابت بنية fdt_property *fdt_get_property_by_offset(ثابت فراغ *fdt,
						      صحيح offset,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		𐤀𐤌 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_by_offset_(fdt, offset, lenp);
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
		𐤀𐤌 (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			اكسر;
		}
		𐤀𐤌 (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			𐤀𐤌 (poffset)
				*poffset = offset;
			أرجع prop;
		}
	}

	𐤀𐤌 (lenp)
		*lenp = offset;
	أرجع عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح offset,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		𐤀𐤌 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		أرجع عدم;
	}

	أرجع fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ثابت فراغ *fdt_getprop_namelen(ثابت فراغ *fdt, صحيح nodeoffset,
				ثابت حرف *name, صحيح namelen, صحيح *lenp)
{
	صحيح poffset;
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	𐤀𐤌 (!prop)
		أرجع عدم;

	/* Handle realignment */
	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->data + 4;
	أرجع prop->data;
}

ثابت فراغ *fdt_getprop_by_offset(ثابت فراغ *fdt, صحيح offset,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	𐤀𐤌 (!prop)
		أرجع عدم;
	𐤀𐤌 (namep) {
		ثابت حرف *name;
		صحيح namelen;

		𐤀𐤌 (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			𐤀𐤌 (!name) {
				𐤀𐤌 (lenp)
					*lenp = namelen;
				أرجع عدم;
			}
			*namep = name;
		} وإلا {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		أرجع prop->data + 4;
	أرجع prop->data;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	أرجع fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ثابت فراغ *fdt, صحيح nodeoffset)
{
	ثابت fdt32_t *php;
	صحيح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	𐤀𐤌 (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		𐤀𐤌 (!php || (len != حجم(*php)))
			أرجع 0;
	}

	أرجع fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت فراغ *fdt,
				  ثابت حرف *name, صحيح namelen)
{
	صحيح aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	𐤀𐤌 (aliasoffset < 0)
		أرجع عدم;

	أرجع fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	أرجع fdt_get_alias_namelen(fdt, name, strlen(name));
}

صحيح fdt_get_path(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح offset, depth, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	𐤀𐤌 (buflen < 2)
		أرجع -FDT_ERR_NOSPACE;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		طالما (pdepth > depth) {
			𐤐𐤏𐤋 {
				p--;
			} طالما (buf[p-1] != '/');
			pdepth--;
		}

		𐤀𐤌 (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			𐤀𐤌 (!name)
				أرجع namelen;
			𐤀𐤌 ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		𐤀𐤌 (offset == nodeoffset) {
			𐤀𐤌 (pdepth < (depth + 1))
				أرجع -FDT_ERR_NOSPACE;

			𐤀𐤌 (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			أرجع 0;
		}
	}

	𐤀𐤌 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		أرجع -FDT_ERR_BADOFFSET;
	وإلا 𐤀𐤌 (offset == -FDT_ERR_BADOFFSET)
		أرجع -FDT_ERR_BADSTRUCTURE;

	أرجع offset; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_offset(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح offset, depth;
	صحيح supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	𐤀𐤌 (supernodedepth < 0)
		أرجع -FDT_ERR_NOTFOUND;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		𐤀𐤌 (depth == supernodedepth)
			supernodeoffset = offset;

		𐤀𐤌 (offset == nodeoffset) {
			𐤀𐤌 (nodedepth)
				*nodedepth = depth;

			𐤀𐤌 (supernodedepth > depth)
				أرجع -FDT_ERR_NOTFOUND;
			وإلا
				أرجع supernodeoffset;
		}
	}

	𐤀𐤌 (!can_assume(VALID_INPUT)) {
		𐤀𐤌 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			أرجع -FDT_ERR_BADOFFSET;
		وإلا 𐤀𐤌 (offset == -FDT_ERR_BADOFFSET)
			أرجع -FDT_ERR_BADSTRUCTURE;
	}

	أرجع offset; /* error from fdt_next_node() */
}

صحيح fdt_node_depth(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	𐤀𐤌 (err)
		أرجع (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	أرجع nodedepth;
}

صحيح fdt_parent_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_node_depth(fdt, nodeoffset);

	𐤀𐤌 (nodedepth < 0)
		أرجع nodedepth;
	أرجع fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
	لكل (offset = fdt_next_node(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		𐤀𐤌 (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			أرجع offset;
	}

	أرجع offset; /* error from fdt_next_node() */
}

صحيح fdt_node_offset_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح offset;

	𐤀𐤌 ((phandle == 0) || (phandle == ~0U))
		أرجع -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	لكل (offset = fdt_next_node(fdt, -1, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		𐤀𐤌 (fdt_get_phandle(fdt, offset) == phandle)
			أرجع offset;
	}

	أرجع offset; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_contains(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = strlen(str);
	ثابت حرف *p;

	طالما (listlen >= len) {
		𐤀𐤌 (memcmp(str, strlist, len+1) == 0)
			أرجع 1;
		p = memchr(strlist, '\0', listlen);
		𐤀𐤌 (!p)
			أرجع 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	أرجع 0;
}

صحيح fdt_stringlist_count(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *end;
	صحيح length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	𐤀𐤌 (!list)
		أرجع length;

	end = list + length;

	طالما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		𐤀𐤌 (list + length > end)
			أرجع -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	أرجع count;
}

صحيح fdt_stringlist_search(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح length, len, idx = 0;
	ثابت حرف *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	𐤀𐤌 (!list)
		أرجع length;

	len = strlen(string) + 1;
	end = list + length;

	طالما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		𐤀𐤌 (list + length > end)
			أرجع -FDT_ERR_BADVALUE;

		𐤀𐤌 (length == len && memcmp(list, string, length) == 0)
			أرجع idx;

		list += length;
		idx++;
	}

	أرجع -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *end;
	صحيح length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	𐤀𐤌 (!list) {
		𐤀𐤌 (lenp)
			*lenp = length;

		أرجع عدم;
	}

	end = list + length;

	طالما (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		𐤀𐤌 (list + length > end) {
			𐤀𐤌 (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			أرجع عدم;
		}

		𐤀𐤌 (idx == 0) {
			𐤀𐤌 (lenp)
				*lenp = length - 1;

			أرجع list;
		}

		list += length;
		idx--;
	}

	𐤀𐤌 (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	أرجع عدم;
}

صحيح fdt_node_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	𐤀𐤌 (!prop)
		أرجع len;

	أرجع !fdt_stringlist_contains(prop, len, compatible);
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
	لكل (offset = fdt_next_node(fdt, startoffset, عدم);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, عدم)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		𐤀𐤌 ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			أرجع err;
		وإلا 𐤀𐤌 (err == 0)
			أرجع offset;
	}

	أرجع offset; /* error from fdt_next_node() */
}
