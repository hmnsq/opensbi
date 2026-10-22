// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static 整数 fdt_nodename_eq_(const void *fdt, 整数 offset,
			    const char *s, 整数 len)
{
	整数 olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	如果 (!p || olen < len)
		/* short match */
		返回 0;

	如果 (memcmp(p, s, len) != 0)
		返回 0;

	如果 (p[len] == '\0')
		返回 1;
	否则 如果 (!memchr(s, '@', len) && (p[len] == '@'))
		返回 1;
	否则
		返回 0;
}

const char *fdt_get_string(const void *fdt, 整数 stroffset, 整数 *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	整数 err;
	const char *s, *n;

	如果 (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		如果 (lenp)
			*lenp = strlen(s);
		返回 s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	如果 (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	如果 (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	如果 (fdt_magic(fdt) == FDT_MAGIC) {
		如果 (stroffset < 0)
			goto fail;
		如果 (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			如果 ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			如果 ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} 否则 如果 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned 整数 sw_stroffset = -stroffset;

		如果 ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		如果 (sw_stroffset < len)
			len = sw_stroffset;
	} 否则 {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	如果 (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	如果 (lenp)
		*lenp = n - s;
	返回 s;

fail:
	如果 (lenp)
		*lenp = err;
	返回 NULL;
}

const char *fdt_string(const void *fdt, 整数 stroffset)
{
	返回 fdt_get_string(fdt, stroffset, NULL);
}

static 整数 fdt_string_eq_(const void *fdt, 整数 stroffset,
			  const char *s, 整数 len)
{
	整数 slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	返回 p && (slen == len) && (memcmp(p, s, len) == 0);
}

整数 fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	整数 offset = -1;

	当 (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		如果 (offset < 0) {
			如果 (offset == -FDT_ERR_NOTFOUND)
				break;

			返回 offset;
		}

		value = fdt_get_phandle(fdt, offset);

		如果 (value > max)
			max = value;
	}

	如果 (phandle)
		*phandle = max;

	返回 0;
}

整数 fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	整数 err;

	err = fdt_find_max_phandle(fdt, &max);
	如果 (err < 0)
		返回 err;

	如果 (max == FDT_MAX_PHANDLE)
		返回 -FDT_ERR_NOPHANDLES;

	如果 (phandle)
		*phandle = max + 1;

	返回 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, 整数 n)
{
	unsigned 整数 offset = n * sizeof(struct fdt_reserve_entry);
	unsigned 整数 absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	如果 (!can_assume(VALID_INPUT)) {
		如果 (absoffset < fdt_off_mem_rsvmap(fdt))
			返回 NULL;
		如果 (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			返回 NULL;
	}
	返回 fdt_mem_rsv_(fdt, n);
}

整数 fdt_get_mem_rsv(const void *fdt, 整数 n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	如果 (!can_assume(VALID_INPUT) && !re)
		返回 -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	返回 0;
}

整数 fdt_num_mem_rsv(const void *fdt)
{
	整数 i;
	const struct fdt_reserve_entry *re;

	为 (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		如果 (fdt64_ld_(&re->size) == 0)
			返回 i;
	}
	返回 -FDT_ERR_TRUNCATED;
}

static 整数 nextprop_(const void *fdt, 整数 offset)
{
	uint32_t tag;
	整数 nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			如果 (nextoffset >= 0)
				返回 -FDT_ERR_BADSTRUCTURE;
			否则
				返回 nextoffset;

		case FDT_PROP:
			返回 offset;
		}
		offset = nextoffset;
	} 当 (tag == FDT_NOP);

	返回 -FDT_ERR_NOTFOUND;
}

整数 fdt_subnode_offset_namelen(const void *fdt, 整数 offset,
			       const char *name, 整数 namelen)
{
	整数 depth;

	FDT_RO_PROBE(fdt);

	为 (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		如果 ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			返回 offset;

	如果 (depth < 0)
		返回 -FDT_ERR_NOTFOUND;
	返回 offset; /* error */
}

整数 fdt_subnode_offset(const void *fdt, 整数 parentoffset,
		       const char *name)
{
	返回 fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

整数 fdt_path_offset_namelen(const void *fdt, const char *path, 整数 namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	整数 offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	如果 (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		如果 (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		如果 (!p)
			返回 -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	当 (p < end) {
		const char *q;

		当 (*p == '/') {
			p++;
			如果 (p == end)
				返回 offset;
		}
		q = memchr(p, '/', end - p);
		如果 (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		如果 (offset < 0)
			返回 offset;

		p = q;
	}

	返回 offset;
}

整数 fdt_path_offset(const void *fdt, const char *path)
{
	返回 fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, 整数 nodeoffset, 整数 *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	整数 err;

	如果 (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	如果 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		如果 (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	如果 (len)
		*len = strlen(nameptr);

	返回 nameptr;

 fail:
	如果 (len)
		*len = err;
	返回 NULL;
}

整数 fdt_first_property_offset(const void *fdt, 整数 nodeoffset)
{
	整数 offset;

	如果 ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		返回 offset;

	返回 nextprop_(fdt, offset);
}

整数 fdt_next_property_offset(const void *fdt, 整数 offset)
{
	如果 ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		返回 offset;

	返回 nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              整数 offset,
						              整数 *lenp)
{
	整数 err;
	const struct fdt_property *prop;

	如果 (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		如果 (lenp)
			*lenp = err;
		返回 NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	如果 (lenp)
		*lenp = fdt32_ld_(&prop->len);

	返回 prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      整数 offset,
						      整数 *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	如果 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		如果 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		返回 NULL;
	}

	返回 fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            整数 offset,
						            const char *name,
						            整数 namelen,
							    整数 *lenp,
							    整数 *poffset)
{
	为 (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		如果 (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		如果 (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			如果 (poffset)
				*poffset = offset;
			返回 prop;
		}
	}

	如果 (lenp)
		*lenp = offset;
	返回 NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    整数 offset,
						    const char *name,
						    整数 namelen, 整数 *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	如果 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		如果 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		返回 NULL;
	}

	返回 fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    整数 nodeoffset,
					    const char *name, 整数 *lenp)
{
	返回 fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, 整数 nodeoffset,
				const char *name, 整数 namelen, 整数 *lenp)
{
	整数 poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	如果 (!prop)
		返回 NULL;

	/* Handle realignment */
	如果 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		返回 prop->data + 4;
	返回 prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, 整数 offset,
				  const char **namep, 整数 *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	如果 (!prop)
		返回 NULL;
	如果 (namep) {
		const char *name;
		整数 namelen;

		如果 (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			如果 (!name) {
				如果 (lenp)
					*lenp = namelen;
				返回 NULL;
			}
			*namep = name;
		} 否则 {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	如果 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		返回 prop->data + 4;
	返回 prop->data;
}

const void *fdt_getprop(const void *fdt, 整数 nodeoffset,
			const char *name, 整数 *lenp)
{
	返回 fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, 整数 nodeoffset)
{
	const fdt32_t *php;
	整数 len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	如果 (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		如果 (!php || (len != sizeof(*php)))
			返回 0;
	}

	返回 fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, 整数 namelen)
{
	整数 aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	如果 (aliasoffset < 0)
		返回 NULL;

	返回 fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	返回 fdt_get_alias_namelen(fdt, name, strlen(name));
}

整数 fdt_get_path(const void *fdt, 整数 nodeoffset, char *buf, 整数 buflen)
{
	整数 pdepth = 0, p = 0;
	整数 offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	如果 (buflen < 2)
		返回 -FDT_ERR_NOSPACE;

	为 (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		当 (pdepth > depth) {
			do {
				p--;
			} 当 (buf[p-1] != '/');
			pdepth--;
		}

		如果 (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			如果 (!name)
				返回 namelen;
			如果 ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		如果 (offset == nodeoffset) {
			如果 (pdepth < (depth + 1))
				返回 -FDT_ERR_NOSPACE;

			如果 (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			返回 0;
		}
	}

	如果 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		返回 -FDT_ERR_BADOFFSET;
	否则 如果 (offset == -FDT_ERR_BADOFFSET)
		返回 -FDT_ERR_BADSTRUCTURE;

	返回 offset; /* error from fdt_next_node() */
}

整数 fdt_supernode_atdepth_offset(const void *fdt, 整数 nodeoffset,
				 整数 supernodedepth, 整数 *nodedepth)
{
	整数 offset, depth;
	整数 supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	如果 (supernodedepth < 0)
		返回 -FDT_ERR_NOTFOUND;

	为 (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		如果 (depth == supernodedepth)
			supernodeoffset = offset;

		如果 (offset == nodeoffset) {
			如果 (nodedepth)
				*nodedepth = depth;

			如果 (supernodedepth > depth)
				返回 -FDT_ERR_NOTFOUND;
			否则
				返回 supernodeoffset;
		}
	}

	如果 (!can_assume(VALID_INPUT)) {
		如果 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			返回 -FDT_ERR_BADOFFSET;
		否则 如果 (offset == -FDT_ERR_BADOFFSET)
			返回 -FDT_ERR_BADSTRUCTURE;
	}

	返回 offset; /* error from fdt_next_node() */
}

整数 fdt_node_depth(const void *fdt, 整数 nodeoffset)
{
	整数 nodedepth;
	整数 err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	如果 (err)
		返回 (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	返回 nodedepth;
}

整数 fdt_parent_offset(const void *fdt, 整数 nodeoffset)
{
	整数 nodedepth = fdt_node_depth(fdt, nodeoffset);

	如果 (nodedepth < 0)
		返回 nodedepth;
	返回 fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

整数 fdt_node_offset_by_prop_value(const void *fdt, 整数 startoffset,
				  const char *propname,
				  const void *propval, 整数 proplen)
{
	整数 offset;
	const void *val;
	整数 len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	为 (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		如果 (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			返回 offset;
	}

	返回 offset; /* error from fdt_next_node() */
}

整数 fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	整数 offset;

	如果 ((phandle == 0) || (phandle == ~0U))
		返回 -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	为 (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		如果 (fdt_get_phandle(fdt, offset) == phandle)
			返回 offset;
	}

	返回 offset; /* error from fdt_next_node() */
}

整数 fdt_stringlist_contains(const char *strlist, 整数 listlen, const char *str)
{
	整数 len = strlen(str);
	const char *p;

	当 (listlen >= len) {
		如果 (memcmp(str, strlist, len+1) == 0)
			返回 1;
		p = memchr(strlist, '\0', listlen);
		如果 (!p)
			返回 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	返回 0;
}

整数 fdt_stringlist_count(const void *fdt, 整数 nodeoffset, const char *property)
{
	const char *list, *end;
	整数 length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	如果 (!list)
		返回 length;

	end = list + length;

	当 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		如果 (list + length > end)
			返回 -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	返回 count;
}

整数 fdt_stringlist_search(const void *fdt, 整数 nodeoffset, const char *property,
			  const char *string)
{
	整数 length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	如果 (!list)
		返回 length;

	len = strlen(string) + 1;
	end = list + length;

	当 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		如果 (list + length > end)
			返回 -FDT_ERR_BADVALUE;

		如果 (length == len && memcmp(list, string, length) == 0)
			返回 idx;

		list += length;
		idx++;
	}

	返回 -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, 整数 nodeoffset,
			       const char *property, 整数 idx,
			       整数 *lenp)
{
	const char *list, *end;
	整数 length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	如果 (!list) {
		如果 (lenp)
			*lenp = length;

		返回 NULL;
	}

	end = list + length;

	当 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		如果 (list + length > end) {
			如果 (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			返回 NULL;
		}

		如果 (idx == 0) {
			如果 (lenp)
				*lenp = length - 1;

			返回 list;
		}

		list += length;
		idx--;
	}

	如果 (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	返回 NULL;
}

整数 fdt_node_check_compatible(const void *fdt, 整数 nodeoffset,
			      const char *compatible)
{
	const void *prop;
	整数 len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	如果 (!prop)
		返回 len;

	返回 !fdt_stringlist_contains(prop, len, compatible);
}

整数 fdt_node_offset_by_compatible(const void *fdt, 整数 startoffset,
				  const char *compatible)
{
	整数 offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	为 (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		如果 ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			返回 err;
		否则 如果 (err == 0)
			返回 offset;
	}

	返回 offset; /* error from fdt_next_node() */
}
