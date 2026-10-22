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

	ܐܢ (!p || olen < len)
		/* short match */
		ܦܢܐ 0;

	ܐܢ (memcmp(p, s, len) != 0)
		ܦܢܐ 0;

	ܐܢ (p[len] == '\0')
		ܦܢܐ 1;
	ܐܚܪܢܐ ܐܢ (!memchr(s, '@', len) && (p[len] == '@'))
		ܦܢܐ 1;
	ܐܚܪܢܐ
		ܦܢܐ 0;
}

ثابت حرف *fdt_get_string(ثابت فراغ *fdt, صحيح stroffset, صحيح *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	صحيح err;
	ثابت حرف *s, *n;

	ܐܢ (can_assume(VALID_INPUT)) {
		s = (ثابت حرف *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ܐܢ (lenp)
			*lenp = strlen(s);
		ܦܢܐ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ܐܢ (totalsize < 0)
		اذهب fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ܐܢ (absoffset >= (unsigned)totalsize)
		اذهب fail;
	len = totalsize - absoffset;

	ܐܢ (fdt_magic(fdt) == FDT_MAGIC) {
		ܐܢ (stroffset < 0)
			اذهب fail;
		ܐܢ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ܐܢ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				اذهب fail;
			ܐܢ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ܐܚܪܢܐ ܐܢ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned صحيح sw_stroffset = -stroffset;

		ܐܢ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			اذهب fail;
		ܐܢ (sw_stroffset < len)
			len = sw_stroffset;
	} ܐܚܪܢܐ {
		err = -FDT_ERR_INTERNAL;
		اذهب fail;
	}

	s = (ثابت حرف *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ܐܢ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		اذهب fail;
	}

	ܐܢ (lenp)
		*lenp = n - s;
	ܦܢܐ s;

fail:
	ܐܢ (lenp)
		*lenp = err;
	ܦܢܐ عدم;
}

ثابت حرف *fdt_string(ثابت فراغ *fdt, صحيح stroffset)
{
	ܦܢܐ fdt_get_string(fdt, stroffset, عدم);
}

ساكن صحيح fdt_string_eq_(ثابت فراغ *fdt, صحيح stroffset,
			  ثابت حرف *s, صحيح len)
{
	صحيح slen;
	ثابت حرف *p = fdt_get_string(fdt, stroffset, &slen);

	ܦܢܐ p && (slen == len) && (memcmp(p, s, len) == 0);
}

صحيح fdt_find_max_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	صحيح offset = -1;

	ܟܡܐ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, عدم);
		ܐܢ (offset < 0) {
			ܐܢ (offset == -FDT_ERR_NOTFOUND)
				اكسر;

			ܦܢܐ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ܐܢ (value > max)
			max = value;
	}

	ܐܢ (phandle)
		*phandle = max;

	ܦܢܐ 0;
}

صحيح fdt_generate_phandle(ثابت فراغ *fdt, uint32_t *phandle)
{
	uint32_t max;
	صحيح err;

	err = fdt_find_max_phandle(fdt, &max);
	ܐܢ (err < 0)
		ܦܢܐ err;

	ܐܢ (max == FDT_MAX_PHANDLE)
		ܦܢܐ -FDT_ERR_NOPHANDLES;

	ܐܢ (phandle)
		*phandle = max + 1;

	ܦܢܐ 0;
}

ساكن ثابت بنية fdt_reserve_entry *fdt_mem_rsv(ثابت فراغ *fdt, صحيح n)
{
	unsigned صحيح offset = n * حجم(بنية fdt_reserve_entry);
	unsigned صحيح absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ܐܢ (!can_assume(VALID_INPUT)) {
		ܐܢ (absoffset < fdt_off_mem_rsvmap(fdt))
			ܦܢܐ عدم;
		ܐܢ (absoffset > fdt_totalsize(fdt) -
		    حجم(بنية fdt_reserve_entry))
			ܦܢܐ عدم;
	}
	ܦܢܐ fdt_mem_rsv_(fdt, n);
}

صحيح fdt_get_mem_rsv(ثابت فراغ *fdt, صحيح n, uint64_t *address, uint64_t *size)
{
	ثابت بنية fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ܐܢ (!can_assume(VALID_INPUT) && !re)
		ܦܢܐ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ܦܢܐ 0;
}

صحيح fdt_num_mem_rsv(ثابت فراغ *fdt)
{
	صحيح i;
	ثابت بنية fdt_reserve_entry *re;

	لكل (i = 0; (re = fdt_mem_rsv(fdt, i)) != عدم; i++) {
		ܐܢ (fdt64_ld_(&re->size) == 0)
			ܦܢܐ i;
	}
	ܦܢܐ -FDT_ERR_TRUNCATED;
}

ساكن صحيح nextprop_(ثابت فراغ *fdt, صحيح offset)
{
	uint32_t tag;
	صحيح nextoffset;

	ܥܒܕ {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		بدل (tag) {
		حالة FDT_END:
			ܐܢ (nextoffset >= 0)
				ܦܢܐ -FDT_ERR_BADSTRUCTURE;
			ܐܚܪܢܐ
				ܦܢܐ nextoffset;

		حالة FDT_PROP:
			ܦܢܐ offset;
		}
		offset = nextoffset;
	} ܟܡܐ (tag == FDT_NOP);

	ܦܢܐ -FDT_ERR_NOTFOUND;
}

صحيح fdt_subnode_offset_namelen(ثابت فراغ *fdt, صحيح offset,
			       ثابت حرف *name, صحيح namelen)
{
	صحيح depth;

	FDT_RO_PROBE(fdt);

	لكل (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ܐܢ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ܦܢܐ offset;

	ܐܢ (depth < 0)
		ܦܢܐ -FDT_ERR_NOTFOUND;
	ܦܢܐ offset; /* error */
}

صحيح fdt_subnode_offset(ثابت فراغ *fdt, صحيح parentoffset,
		       ثابت حرف *name)
{
	ܦܢܐ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

صحيح fdt_path_offset_namelen(ثابت فراغ *fdt, ثابت حرف *path, صحيح namelen)
{
	ثابت حرف *end = path + namelen;
	ثابت حرف *p = path;
	صحيح offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ܐܢ (*path != '/') {
		ثابت حرف *q = memchr(path, '/', end - p);

		ܐܢ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ܐܢ (!p)
			ܦܢܐ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ܟܡܐ (p < end) {
		ثابت حرف *q;

		ܟܡܐ (*p == '/') {
			p++;
			ܐܢ (p == end)
				ܦܢܐ offset;
		}
		q = memchr(p, '/', end - p);
		ܐܢ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ܐܢ (offset < 0)
			ܦܢܐ offset;

		p = q;
	}

	ܦܢܐ offset;
}

صحيح fdt_path_offset(ثابت فراغ *fdt, ثابت حرف *path)
{
	ܦܢܐ fdt_path_offset_namelen(fdt, path, strlen(path));
}

ثابت حرف *fdt_get_name(ثابت فراغ *fdt, صحيح nodeoffset, صحيح *len)
{
	ثابت بنية fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ثابت حرف *nameptr;
	صحيح err;

	ܐܢ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			اذهب fail;

	nameptr = nh->name;

	ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ثابت حرف *leaf;
		leaf = strrchr(nameptr, '/');
		ܐܢ (leaf == عدم) {
			err = -FDT_ERR_BADSTRUCTURE;
			اذهب fail;
		}
		nameptr = leaf+1;
	}

	ܐܢ (len)
		*len = strlen(nameptr);

	ܦܢܐ nameptr;

 fail:
	ܐܢ (len)
		*len = err;
	ܦܢܐ عدم;
}

صحيح fdt_first_property_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح offset;

	ܐܢ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ܦܢܐ offset;

	ܦܢܐ nextprop_(fdt, offset);
}

صحيح fdt_next_property_offset(ثابت فراغ *fdt, صحيح offset)
{
	ܐܢ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ܦܢܐ offset;

	ܦܢܐ nextprop_(fdt, offset);
}

ساكن ثابت بنية fdt_property *fdt_get_property_by_offset_(ثابت فراغ *fdt,
						              صحيح offset,
						              صحيح *lenp)
{
	صحيح err;
	ثابت بنية fdt_property *prop;

	ܐܢ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ܐܢ (lenp)
			*lenp = err;
		ܦܢܐ عدم;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ܐܢ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ܦܢܐ prop;
}

ثابت بنية fdt_property *fdt_get_property_by_offset(ثابت فراغ *fdt,
						      صحيح offset,
						      صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ܐܢ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ܦܢܐ عدم;
	}

	ܦܢܐ fdt_get_property_by_offset_(fdt, offset, lenp);
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
		ܐܢ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			اكسر;
		}
		ܐܢ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ܐܢ (poffset)
				*poffset = offset;
			ܦܢܐ prop;
		}
	}

	ܐܢ (lenp)
		*lenp = offset;
	ܦܢܐ عدم;
}


ثابت بنية fdt_property *fdt_get_property_namelen(ثابت فراغ *fdt,
						    صحيح offset,
						    ثابت حرف *name,
						    صحيح namelen, صحيح *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ܐܢ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ܦܢܐ عدم;
	}

	ܦܢܐ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 عدم);
}


ثابت بنية fdt_property *fdt_get_property(ثابت فراغ *fdt,
					    صحيح nodeoffset,
					    ثابت حرف *name, صحيح *lenp)
{
	ܦܢܐ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ثابت فراغ *fdt_getprop_namelen(ثابت فراغ *fdt, صحيح nodeoffset,
				ثابت حرف *name, صحيح namelen, صحيح *lenp)
{
	صحيح poffset;
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ܐܢ (!prop)
		ܦܢܐ عدم;

	/* Handle realignment */
	ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ܦܢܐ prop->data + 4;
	ܦܢܐ prop->data;
}

ثابت فراغ *fdt_getprop_by_offset(ثابت فراغ *fdt, صحيح offset,
				  ثابت حرف **namep, صحيح *lenp)
{
	ثابت بنية fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ܐܢ (!prop)
		ܦܢܐ عدم;
	ܐܢ (namep) {
		ثابت حرف *name;
		صحيح namelen;

		ܐܢ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ܐܢ (!name) {
				ܐܢ (lenp)
					*lenp = namelen;
				ܦܢܐ عدم;
			}
			*namep = name;
		} ܐܚܪܢܐ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ܐܢ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + حجم(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ܦܢܐ prop->data + 4;
	ܦܢܐ prop->data;
}

ثابت فراغ *fdt_getprop(ثابت فراغ *fdt, صحيح nodeoffset,
			ثابت حرف *name, صحيح *lenp)
{
	ܦܢܐ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ثابت فراغ *fdt, صحيح nodeoffset)
{
	ثابت fdt32_t *php;
	صحيح len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ܐܢ (!php || (len != حجم(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ܐܢ (!php || (len != حجم(*php)))
			ܦܢܐ 0;
	}

	ܦܢܐ fdt32_ld_(php);
}

ثابت حرف *fdt_get_alias_namelen(ثابت فراغ *fdt,
				  ثابت حرف *name, صحيح namelen)
{
	صحيح aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ܐܢ (aliasoffset < 0)
		ܦܢܐ عدم;

	ܦܢܐ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, عدم);
}

ثابت حرف *fdt_get_alias(ثابت فراغ *fdt, ثابت حرف *name)
{
	ܦܢܐ fdt_get_alias_namelen(fdt, name, strlen(name));
}

صحيح fdt_get_path(ثابت فراغ *fdt, صحيح nodeoffset, حرف *buf, صحيح buflen)
{
	صحيح pdepth = 0, p = 0;
	صحيح offset, depth, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	ܐܢ (buflen < 2)
		ܦܢܐ -FDT_ERR_NOSPACE;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ܟܡܐ (pdepth > depth) {
			ܥܒܕ {
				p--;
			} ܟܡܐ (buf[p-1] != '/');
			pdepth--;
		}

		ܐܢ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ܐܢ (!name)
				ܦܢܐ namelen;
			ܐܢ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ܐܢ (offset == nodeoffset) {
			ܐܢ (pdepth < (depth + 1))
				ܦܢܐ -FDT_ERR_NOSPACE;

			ܐܢ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ܦܢܐ 0;
		}
	}

	ܐܢ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ܦܢܐ -FDT_ERR_BADOFFSET;
	ܐܚܪܢܐ ܐܢ (offset == -FDT_ERR_BADOFFSET)
		ܦܢܐ -FDT_ERR_BADSTRUCTURE;

	ܦܢܐ offset; /* error from fdt_next_node() */
}

صحيح fdt_supernode_atdepth_offset(ثابت فراغ *fdt, صحيح nodeoffset,
				 صحيح supernodedepth, صحيح *nodedepth)
{
	صحيح offset, depth;
	صحيح supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ܐܢ (supernodedepth < 0)
		ܦܢܐ -FDT_ERR_NOTFOUND;

	لكل (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ܐܢ (depth == supernodedepth)
			supernodeoffset = offset;

		ܐܢ (offset == nodeoffset) {
			ܐܢ (nodedepth)
				*nodedepth = depth;

			ܐܢ (supernodedepth > depth)
				ܦܢܐ -FDT_ERR_NOTFOUND;
			ܐܚܪܢܐ
				ܦܢܐ supernodeoffset;
		}
	}

	ܐܢ (!can_assume(VALID_INPUT)) {
		ܐܢ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ܦܢܐ -FDT_ERR_BADOFFSET;
		ܐܚܪܢܐ ܐܢ (offset == -FDT_ERR_BADOFFSET)
			ܦܢܐ -FDT_ERR_BADSTRUCTURE;
	}

	ܦܢܐ offset; /* error from fdt_next_node() */
}

صحيح fdt_node_depth(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth;
	صحيح err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ܐܢ (err)
		ܦܢܐ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ܦܢܐ nodedepth;
}

صحيح fdt_parent_offset(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح nodedepth = fdt_node_depth(fdt, nodeoffset);

	ܐܢ (nodedepth < 0)
		ܦܢܐ nodedepth;
	ܦܢܐ fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
		ܐܢ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ܦܢܐ offset;
	}

	ܦܢܐ offset; /* error from fdt_next_node() */
}

صحيح fdt_node_offset_by_phandle(ثابت فراغ *fdt, uint32_t phandle)
{
	صحيح offset;

	ܐܢ ((phandle == 0) || (phandle == ~0U))
		ܦܢܐ -FDT_ERR_BADPHANDLE;

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
		ܐܢ (fdt_get_phandle(fdt, offset) == phandle)
			ܦܢܐ offset;
	}

	ܦܢܐ offset; /* error from fdt_next_node() */
}

صحيح fdt_stringlist_contains(ثابت حرف *strlist, صحيح listlen, ثابت حرف *str)
{
	صحيح len = strlen(str);
	ثابت حرف *p;

	ܟܡܐ (listlen >= len) {
		ܐܢ (memcmp(str, strlist, len+1) == 0)
			ܦܢܐ 1;
		p = memchr(strlist, '\0', listlen);
		ܐܢ (!p)
			ܦܢܐ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ܦܢܐ 0;
}

صحيح fdt_stringlist_count(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property)
{
	ثابت حرف *list, *end;
	صحيح length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ܐܢ (!list)
		ܦܢܐ length;

	end = list + length;

	ܟܡܐ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ܐܢ (list + length > end)
			ܦܢܐ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ܦܢܐ count;
}

صحيح fdt_stringlist_search(ثابت فراغ *fdt, صحيح nodeoffset, ثابت حرف *property,
			  ثابت حرف *string)
{
	صحيح length, len, idx = 0;
	ثابت حرف *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ܐܢ (!list)
		ܦܢܐ length;

	len = strlen(string) + 1;
	end = list + length;

	ܟܡܐ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ܐܢ (list + length > end)
			ܦܢܐ -FDT_ERR_BADVALUE;

		ܐܢ (length == len && memcmp(list, string, length) == 0)
			ܦܢܐ idx;

		list += length;
		idx++;
	}

	ܦܢܐ -FDT_ERR_NOTFOUND;
}

ثابت حرف *fdt_stringlist_get(ثابت فراغ *fdt, صحيح nodeoffset,
			       ثابت حرف *property, صحيح idx,
			       صحيح *lenp)
{
	ثابت حرف *list, *end;
	صحيح length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ܐܢ (!list) {
		ܐܢ (lenp)
			*lenp = length;

		ܦܢܐ عدم;
	}

	end = list + length;

	ܟܡܐ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ܐܢ (list + length > end) {
			ܐܢ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ܦܢܐ عدم;
		}

		ܐܢ (idx == 0) {
			ܐܢ (lenp)
				*lenp = length - 1;

			ܦܢܐ list;
		}

		list += length;
		idx--;
	}

	ܐܢ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ܦܢܐ عدم;
}

صحيح fdt_node_check_compatible(ثابت فراغ *fdt, صحيح nodeoffset,
			      ثابت حرف *compatible)
{
	ثابت فراغ *prop;
	صحيح len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ܐܢ (!prop)
		ܦܢܐ len;

	ܦܢܐ !fdt_stringlist_contains(prop, len, compatible);
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
		ܐܢ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ܦܢܐ err;
		ܐܚܪܢܐ ܐܢ (err == 0)
			ܦܢܐ offset;
	}

	ܦܢܐ offset; /* error from fdt_next_node() */
}
