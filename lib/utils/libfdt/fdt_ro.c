// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static số_nguyên fdt_nodename_eq_(const void *fdt, số_nguyên offset,
			    const char *s, số_nguyên len)
{
	số_nguyên olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	nếu (!p || olen < len)
		/* short match */
		trả_về 0;

	nếu (memcmp(p, s, len) != 0)
		trả_về 0;

	nếu (p[len] == '\0')
		trả_về 1;
	không_thì nếu (!memchr(s, '@', len) && (p[len] == '@'))
		trả_về 1;
	không_thì
		trả_về 0;
}

const char *fdt_get_string(const void *fdt, số_nguyên stroffset, số_nguyên *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	số_nguyên err;
	const char *s, *n;

	nếu (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		nếu (lenp)
			*lenp = strlen(s);
		trả_về s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	nếu (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	nếu (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	nếu (fdt_magic(fdt) == FDT_MAGIC) {
		nếu (stroffset < 0)
			goto fail;
		nếu (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			nếu ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			nếu ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} không_thì nếu (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned số_nguyên sw_stroffset = -stroffset;

		nếu ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		nếu (sw_stroffset < len)
			len = sw_stroffset;
	} không_thì {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	nếu (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	nếu (lenp)
		*lenp = n - s;
	trả_về s;

fail:
	nếu (lenp)
		*lenp = err;
	trả_về NULL;
}

const char *fdt_string(const void *fdt, số_nguyên stroffset)
{
	trả_về fdt_get_string(fdt, stroffset, NULL);
}

static số_nguyên fdt_string_eq_(const void *fdt, số_nguyên stroffset,
			  const char *s, số_nguyên len)
{
	số_nguyên slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	trả_về p && (slen == len) && (memcmp(p, s, len) == 0);
}

số_nguyên fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	số_nguyên offset = -1;

	trong_khi (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		nếu (offset < 0) {
			nếu (offset == -FDT_ERR_NOTFOUND)
				break;

			trả_về offset;
		}

		value = fdt_get_phandle(fdt, offset);

		nếu (value > max)
			max = value;
	}

	nếu (phandle)
		*phandle = max;

	trả_về 0;
}

số_nguyên fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	số_nguyên err;

	err = fdt_find_max_phandle(fdt, &max);
	nếu (err < 0)
		trả_về err;

	nếu (max == FDT_MAX_PHANDLE)
		trả_về -FDT_ERR_NOPHANDLES;

	nếu (phandle)
		*phandle = max + 1;

	trả_về 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, số_nguyên n)
{
	unsigned số_nguyên offset = n * sizeof(struct fdt_reserve_entry);
	unsigned số_nguyên absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	nếu (!can_assume(VALID_INPUT)) {
		nếu (absoffset < fdt_off_mem_rsvmap(fdt))
			trả_về NULL;
		nếu (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			trả_về NULL;
	}
	trả_về fdt_mem_rsv_(fdt, n);
}

số_nguyên fdt_get_mem_rsv(const void *fdt, số_nguyên n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	nếu (!can_assume(VALID_INPUT) && !re)
		trả_về -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	trả_về 0;
}

số_nguyên fdt_num_mem_rsv(const void *fdt)
{
	số_nguyên i;
	const struct fdt_reserve_entry *re;

	cho (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		nếu (fdt64_ld_(&re->size) == 0)
			trả_về i;
	}
	trả_về -FDT_ERR_TRUNCATED;
}

static số_nguyên nextprop_(const void *fdt, số_nguyên offset)
{
	uint32_t tag;
	số_nguyên nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			nếu (nextoffset >= 0)
				trả_về -FDT_ERR_BADSTRUCTURE;
			không_thì
				trả_về nextoffset;

		case FDT_PROP:
			trả_về offset;
		}
		offset = nextoffset;
	} trong_khi (tag == FDT_NOP);

	trả_về -FDT_ERR_NOTFOUND;
}

số_nguyên fdt_subnode_offset_namelen(const void *fdt, số_nguyên offset,
			       const char *name, số_nguyên namelen)
{
	số_nguyên depth;

	FDT_RO_PROBE(fdt);

	cho (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		nếu ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			trả_về offset;

	nếu (depth < 0)
		trả_về -FDT_ERR_NOTFOUND;
	trả_về offset; /* error */
}

số_nguyên fdt_subnode_offset(const void *fdt, số_nguyên parentoffset,
		       const char *name)
{
	trả_về fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

số_nguyên fdt_path_offset_namelen(const void *fdt, const char *path, số_nguyên namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	số_nguyên offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	nếu (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		nếu (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		nếu (!p)
			trả_về -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	trong_khi (p < end) {
		const char *q;

		trong_khi (*p == '/') {
			p++;
			nếu (p == end)
				trả_về offset;
		}
		q = memchr(p, '/', end - p);
		nếu (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		nếu (offset < 0)
			trả_về offset;

		p = q;
	}

	trả_về offset;
}

số_nguyên fdt_path_offset(const void *fdt, const char *path)
{
	trả_về fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, số_nguyên nodeoffset, số_nguyên *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	số_nguyên err;

	nếu (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	nếu (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		nếu (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	nếu (len)
		*len = strlen(nameptr);

	trả_về nameptr;

 fail:
	nếu (len)
		*len = err;
	trả_về NULL;
}

số_nguyên fdt_first_property_offset(const void *fdt, số_nguyên nodeoffset)
{
	số_nguyên offset;

	nếu ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		trả_về offset;

	trả_về nextprop_(fdt, offset);
}

số_nguyên fdt_next_property_offset(const void *fdt, số_nguyên offset)
{
	nếu ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		trả_về offset;

	trả_về nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              số_nguyên offset,
						              số_nguyên *lenp)
{
	số_nguyên err;
	const struct fdt_property *prop;

	nếu (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		nếu (lenp)
			*lenp = err;
		trả_về NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	nếu (lenp)
		*lenp = fdt32_ld_(&prop->len);

	trả_về prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      số_nguyên offset,
						      số_nguyên *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	nếu (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		nếu (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		trả_về NULL;
	}

	trả_về fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            số_nguyên offset,
						            const char *name,
						            số_nguyên namelen,
							    số_nguyên *lenp,
							    số_nguyên *poffset)
{
	cho (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		nếu (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		nếu (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			nếu (poffset)
				*poffset = offset;
			trả_về prop;
		}
	}

	nếu (lenp)
		*lenp = offset;
	trả_về NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    số_nguyên offset,
						    const char *name,
						    số_nguyên namelen, số_nguyên *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	nếu (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		nếu (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		trả_về NULL;
	}

	trả_về fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    số_nguyên nodeoffset,
					    const char *name, số_nguyên *lenp)
{
	trả_về fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, số_nguyên nodeoffset,
				const char *name, số_nguyên namelen, số_nguyên *lenp)
{
	số_nguyên poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	nếu (!prop)
		trả_về NULL;

	/* Handle realignment */
	nếu (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		trả_về prop->data + 4;
	trả_về prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, số_nguyên offset,
				  const char **namep, số_nguyên *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	nếu (!prop)
		trả_về NULL;
	nếu (namep) {
		const char *name;
		số_nguyên namelen;

		nếu (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			nếu (!name) {
				nếu (lenp)
					*lenp = namelen;
				trả_về NULL;
			}
			*namep = name;
		} không_thì {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	nếu (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		trả_về prop->data + 4;
	trả_về prop->data;
}

const void *fdt_getprop(const void *fdt, số_nguyên nodeoffset,
			const char *name, số_nguyên *lenp)
{
	trả_về fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, số_nguyên nodeoffset)
{
	const fdt32_t *php;
	số_nguyên len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	nếu (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		nếu (!php || (len != sizeof(*php)))
			trả_về 0;
	}

	trả_về fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, số_nguyên namelen)
{
	số_nguyên aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	nếu (aliasoffset < 0)
		trả_về NULL;

	trả_về fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	trả_về fdt_get_alias_namelen(fdt, name, strlen(name));
}

số_nguyên fdt_get_path(const void *fdt, số_nguyên nodeoffset, char *buf, số_nguyên buflen)
{
	số_nguyên pdepth = 0, p = 0;
	số_nguyên offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	nếu (buflen < 2)
		trả_về -FDT_ERR_NOSPACE;

	cho (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		trong_khi (pdepth > depth) {
			do {
				p--;
			} trong_khi (buf[p-1] != '/');
			pdepth--;
		}

		nếu (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			nếu (!name)
				trả_về namelen;
			nếu ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		nếu (offset == nodeoffset) {
			nếu (pdepth < (depth + 1))
				trả_về -FDT_ERR_NOSPACE;

			nếu (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			trả_về 0;
		}
	}

	nếu ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		trả_về -FDT_ERR_BADOFFSET;
	không_thì nếu (offset == -FDT_ERR_BADOFFSET)
		trả_về -FDT_ERR_BADSTRUCTURE;

	trả_về offset; /* error from fdt_next_node() */
}

số_nguyên fdt_supernode_atdepth_offset(const void *fdt, số_nguyên nodeoffset,
				 số_nguyên supernodedepth, số_nguyên *nodedepth)
{
	số_nguyên offset, depth;
	số_nguyên supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	nếu (supernodedepth < 0)
		trả_về -FDT_ERR_NOTFOUND;

	cho (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		nếu (depth == supernodedepth)
			supernodeoffset = offset;

		nếu (offset == nodeoffset) {
			nếu (nodedepth)
				*nodedepth = depth;

			nếu (supernodedepth > depth)
				trả_về -FDT_ERR_NOTFOUND;
			không_thì
				trả_về supernodeoffset;
		}
	}

	nếu (!can_assume(VALID_INPUT)) {
		nếu ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			trả_về -FDT_ERR_BADOFFSET;
		không_thì nếu (offset == -FDT_ERR_BADOFFSET)
			trả_về -FDT_ERR_BADSTRUCTURE;
	}

	trả_về offset; /* error from fdt_next_node() */
}

số_nguyên fdt_node_depth(const void *fdt, số_nguyên nodeoffset)
{
	số_nguyên nodedepth;
	số_nguyên err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	nếu (err)
		trả_về (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	trả_về nodedepth;
}

số_nguyên fdt_parent_offset(const void *fdt, số_nguyên nodeoffset)
{
	số_nguyên nodedepth = fdt_node_depth(fdt, nodeoffset);

	nếu (nodedepth < 0)
		trả_về nodedepth;
	trả_về fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

số_nguyên fdt_node_offset_by_prop_value(const void *fdt, số_nguyên startoffset,
				  const char *propname,
				  const void *propval, số_nguyên proplen)
{
	số_nguyên offset;
	const void *val;
	số_nguyên len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	cho (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		nếu (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			trả_về offset;
	}

	trả_về offset; /* error from fdt_next_node() */
}

số_nguyên fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	số_nguyên offset;

	nếu ((phandle == 0) || (phandle == ~0U))
		trả_về -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	cho (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		nếu (fdt_get_phandle(fdt, offset) == phandle)
			trả_về offset;
	}

	trả_về offset; /* error from fdt_next_node() */
}

số_nguyên fdt_stringlist_contains(const char *strlist, số_nguyên listlen, const char *str)
{
	số_nguyên len = strlen(str);
	const char *p;

	trong_khi (listlen >= len) {
		nếu (memcmp(str, strlist, len+1) == 0)
			trả_về 1;
		p = memchr(strlist, '\0', listlen);
		nếu (!p)
			trả_về 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	trả_về 0;
}

số_nguyên fdt_stringlist_count(const void *fdt, số_nguyên nodeoffset, const char *property)
{
	const char *list, *end;
	số_nguyên length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	nếu (!list)
		trả_về length;

	end = list + length;

	trong_khi (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		nếu (list + length > end)
			trả_về -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	trả_về count;
}

số_nguyên fdt_stringlist_search(const void *fdt, số_nguyên nodeoffset, const char *property,
			  const char *string)
{
	số_nguyên length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	nếu (!list)
		trả_về length;

	len = strlen(string) + 1;
	end = list + length;

	trong_khi (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		nếu (list + length > end)
			trả_về -FDT_ERR_BADVALUE;

		nếu (length == len && memcmp(list, string, length) == 0)
			trả_về idx;

		list += length;
		idx++;
	}

	trả_về -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, số_nguyên nodeoffset,
			       const char *property, số_nguyên idx,
			       số_nguyên *lenp)
{
	const char *list, *end;
	số_nguyên length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	nếu (!list) {
		nếu (lenp)
			*lenp = length;

		trả_về NULL;
	}

	end = list + length;

	trong_khi (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		nếu (list + length > end) {
			nếu (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			trả_về NULL;
		}

		nếu (idx == 0) {
			nếu (lenp)
				*lenp = length - 1;

			trả_về list;
		}

		list += length;
		idx--;
	}

	nếu (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	trả_về NULL;
}

số_nguyên fdt_node_check_compatible(const void *fdt, số_nguyên nodeoffset,
			      const char *compatible)
{
	const void *prop;
	số_nguyên len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	nếu (!prop)
		trả_về len;

	trả_về !fdt_stringlist_contains(prop, len, compatible);
}

số_nguyên fdt_node_offset_by_compatible(const void *fdt, số_nguyên startoffset,
				  const char *compatible)
{
	số_nguyên offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	cho (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		nếu ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			trả_về err;
		không_thì nếu (err == 0)
			trả_về offset;
	}

	trả_về offset; /* error from fdt_next_node() */
}
