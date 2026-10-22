// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static 정수 fdt_nodename_eq_(const void *fdt, 정수 offset,
			    const char *s, 정수 len)
{
	정수 olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	만약 (!p || olen < len)
		/* short match */
		반환 0;

	만약 (memcmp(p, s, len) != 0)
		반환 0;

	만약 (p[len] == '\0')
		반환 1;
	아니면 만약 (!memchr(s, '@', len) && (p[len] == '@'))
		반환 1;
	아니면
		반환 0;
}

const char *fdt_get_string(const void *fdt, 정수 stroffset, 정수 *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	정수 err;
	const char *s, *n;

	만약 (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		만약 (lenp)
			*lenp = strlen(s);
		반환 s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	만약 (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	만약 (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	만약 (fdt_magic(fdt) == FDT_MAGIC) {
		만약 (stroffset < 0)
			goto fail;
		만약 (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			만약 ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			만약 ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} 아니면 만약 (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned 정수 sw_stroffset = -stroffset;

		만약 ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		만약 (sw_stroffset < len)
			len = sw_stroffset;
	} 아니면 {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	만약 (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	만약 (lenp)
		*lenp = n - s;
	반환 s;

fail:
	만약 (lenp)
		*lenp = err;
	반환 NULL;
}

const char *fdt_string(const void *fdt, 정수 stroffset)
{
	반환 fdt_get_string(fdt, stroffset, NULL);
}

static 정수 fdt_string_eq_(const void *fdt, 정수 stroffset,
			  const char *s, 정수 len)
{
	정수 slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	반환 p && (slen == len) && (memcmp(p, s, len) == 0);
}

정수 fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	정수 offset = -1;

	동안 (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		만약 (offset < 0) {
			만약 (offset == -FDT_ERR_NOTFOUND)
				break;

			반환 offset;
		}

		value = fdt_get_phandle(fdt, offset);

		만약 (value > max)
			max = value;
	}

	만약 (phandle)
		*phandle = max;

	반환 0;
}

정수 fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	정수 err;

	err = fdt_find_max_phandle(fdt, &max);
	만약 (err < 0)
		반환 err;

	만약 (max == FDT_MAX_PHANDLE)
		반환 -FDT_ERR_NOPHANDLES;

	만약 (phandle)
		*phandle = max + 1;

	반환 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, 정수 n)
{
	unsigned 정수 offset = n * sizeof(struct fdt_reserve_entry);
	unsigned 정수 absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	만약 (!can_assume(VALID_INPUT)) {
		만약 (absoffset < fdt_off_mem_rsvmap(fdt))
			반환 NULL;
		만약 (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			반환 NULL;
	}
	반환 fdt_mem_rsv_(fdt, n);
}

정수 fdt_get_mem_rsv(const void *fdt, 정수 n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	만약 (!can_assume(VALID_INPUT) && !re)
		반환 -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	반환 0;
}

정수 fdt_num_mem_rsv(const void *fdt)
{
	정수 i;
	const struct fdt_reserve_entry *re;

	위해 (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		만약 (fdt64_ld_(&re->size) == 0)
			반환 i;
	}
	반환 -FDT_ERR_TRUNCATED;
}

static 정수 nextprop_(const void *fdt, 정수 offset)
{
	uint32_t tag;
	정수 nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			만약 (nextoffset >= 0)
				반환 -FDT_ERR_BADSTRUCTURE;
			아니면
				반환 nextoffset;

		case FDT_PROP:
			반환 offset;
		}
		offset = nextoffset;
	} 동안 (tag == FDT_NOP);

	반환 -FDT_ERR_NOTFOUND;
}

정수 fdt_subnode_offset_namelen(const void *fdt, 정수 offset,
			       const char *name, 정수 namelen)
{
	정수 depth;

	FDT_RO_PROBE(fdt);

	위해 (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		만약 ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			반환 offset;

	만약 (depth < 0)
		반환 -FDT_ERR_NOTFOUND;
	반환 offset; /* error */
}

정수 fdt_subnode_offset(const void *fdt, 정수 parentoffset,
		       const char *name)
{
	반환 fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

정수 fdt_path_offset_namelen(const void *fdt, const char *path, 정수 namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	정수 offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	만약 (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		만약 (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		만약 (!p)
			반환 -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	동안 (p < end) {
		const char *q;

		동안 (*p == '/') {
			p++;
			만약 (p == end)
				반환 offset;
		}
		q = memchr(p, '/', end - p);
		만약 (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		만약 (offset < 0)
			반환 offset;

		p = q;
	}

	반환 offset;
}

정수 fdt_path_offset(const void *fdt, const char *path)
{
	반환 fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, 정수 nodeoffset, 정수 *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	정수 err;

	만약 (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	만약 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		만약 (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	만약 (len)
		*len = strlen(nameptr);

	반환 nameptr;

 fail:
	만약 (len)
		*len = err;
	반환 NULL;
}

정수 fdt_first_property_offset(const void *fdt, 정수 nodeoffset)
{
	정수 offset;

	만약 ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		반환 offset;

	반환 nextprop_(fdt, offset);
}

정수 fdt_next_property_offset(const void *fdt, 정수 offset)
{
	만약 ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		반환 offset;

	반환 nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              정수 offset,
						              정수 *lenp)
{
	정수 err;
	const struct fdt_property *prop;

	만약 (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		만약 (lenp)
			*lenp = err;
		반환 NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	만약 (lenp)
		*lenp = fdt32_ld_(&prop->len);

	반환 prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      정수 offset,
						      정수 *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	만약 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		만약 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		반환 NULL;
	}

	반환 fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            정수 offset,
						            const char *name,
						            정수 namelen,
							    정수 *lenp,
							    정수 *poffset)
{
	위해 (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		만약 (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		만약 (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			만약 (poffset)
				*poffset = offset;
			반환 prop;
		}
	}

	만약 (lenp)
		*lenp = offset;
	반환 NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    정수 offset,
						    const char *name,
						    정수 namelen, 정수 *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	만약 (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		만약 (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		반환 NULL;
	}

	반환 fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    정수 nodeoffset,
					    const char *name, 정수 *lenp)
{
	반환 fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, 정수 nodeoffset,
				const char *name, 정수 namelen, 정수 *lenp)
{
	정수 poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	만약 (!prop)
		반환 NULL;

	/* Handle realignment */
	만약 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		반환 prop->data + 4;
	반환 prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, 정수 offset,
				  const char **namep, 정수 *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	만약 (!prop)
		반환 NULL;
	만약 (namep) {
		const char *name;
		정수 namelen;

		만약 (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			만약 (!name) {
				만약 (lenp)
					*lenp = namelen;
				반환 NULL;
			}
			*namep = name;
		} 아니면 {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	만약 (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		반환 prop->data + 4;
	반환 prop->data;
}

const void *fdt_getprop(const void *fdt, 정수 nodeoffset,
			const char *name, 정수 *lenp)
{
	반환 fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, 정수 nodeoffset)
{
	const fdt32_t *php;
	정수 len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	만약 (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		만약 (!php || (len != sizeof(*php)))
			반환 0;
	}

	반환 fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, 정수 namelen)
{
	정수 aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	만약 (aliasoffset < 0)
		반환 NULL;

	반환 fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	반환 fdt_get_alias_namelen(fdt, name, strlen(name));
}

정수 fdt_get_path(const void *fdt, 정수 nodeoffset, char *buf, 정수 buflen)
{
	정수 pdepth = 0, p = 0;
	정수 offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	만약 (buflen < 2)
		반환 -FDT_ERR_NOSPACE;

	위해 (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		동안 (pdepth > depth) {
			do {
				p--;
			} 동안 (buf[p-1] != '/');
			pdepth--;
		}

		만약 (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			만약 (!name)
				반환 namelen;
			만약 ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		만약 (offset == nodeoffset) {
			만약 (pdepth < (depth + 1))
				반환 -FDT_ERR_NOSPACE;

			만약 (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			반환 0;
		}
	}

	만약 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		반환 -FDT_ERR_BADOFFSET;
	아니면 만약 (offset == -FDT_ERR_BADOFFSET)
		반환 -FDT_ERR_BADSTRUCTURE;

	반환 offset; /* error from fdt_next_node() */
}

정수 fdt_supernode_atdepth_offset(const void *fdt, 정수 nodeoffset,
				 정수 supernodedepth, 정수 *nodedepth)
{
	정수 offset, depth;
	정수 supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	만약 (supernodedepth < 0)
		반환 -FDT_ERR_NOTFOUND;

	위해 (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		만약 (depth == supernodedepth)
			supernodeoffset = offset;

		만약 (offset == nodeoffset) {
			만약 (nodedepth)
				*nodedepth = depth;

			만약 (supernodedepth > depth)
				반환 -FDT_ERR_NOTFOUND;
			아니면
				반환 supernodeoffset;
		}
	}

	만약 (!can_assume(VALID_INPUT)) {
		만약 ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			반환 -FDT_ERR_BADOFFSET;
		아니면 만약 (offset == -FDT_ERR_BADOFFSET)
			반환 -FDT_ERR_BADSTRUCTURE;
	}

	반환 offset; /* error from fdt_next_node() */
}

정수 fdt_node_depth(const void *fdt, 정수 nodeoffset)
{
	정수 nodedepth;
	정수 err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	만약 (err)
		반환 (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	반환 nodedepth;
}

정수 fdt_parent_offset(const void *fdt, 정수 nodeoffset)
{
	정수 nodedepth = fdt_node_depth(fdt, nodeoffset);

	만약 (nodedepth < 0)
		반환 nodedepth;
	반환 fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

정수 fdt_node_offset_by_prop_value(const void *fdt, 정수 startoffset,
				  const char *propname,
				  const void *propval, 정수 proplen)
{
	정수 offset;
	const void *val;
	정수 len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	위해 (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		만약 (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			반환 offset;
	}

	반환 offset; /* error from fdt_next_node() */
}

정수 fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	정수 offset;

	만약 ((phandle == 0) || (phandle == ~0U))
		반환 -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	위해 (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		만약 (fdt_get_phandle(fdt, offset) == phandle)
			반환 offset;
	}

	반환 offset; /* error from fdt_next_node() */
}

정수 fdt_stringlist_contains(const char *strlist, 정수 listlen, const char *str)
{
	정수 len = strlen(str);
	const char *p;

	동안 (listlen >= len) {
		만약 (memcmp(str, strlist, len+1) == 0)
			반환 1;
		p = memchr(strlist, '\0', listlen);
		만약 (!p)
			반환 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	반환 0;
}

정수 fdt_stringlist_count(const void *fdt, 정수 nodeoffset, const char *property)
{
	const char *list, *end;
	정수 length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	만약 (!list)
		반환 length;

	end = list + length;

	동안 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		만약 (list + length > end)
			반환 -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	반환 count;
}

정수 fdt_stringlist_search(const void *fdt, 정수 nodeoffset, const char *property,
			  const char *string)
{
	정수 length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	만약 (!list)
		반환 length;

	len = strlen(string) + 1;
	end = list + length;

	동안 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		만약 (list + length > end)
			반환 -FDT_ERR_BADVALUE;

		만약 (length == len && memcmp(list, string, length) == 0)
			반환 idx;

		list += length;
		idx++;
	}

	반환 -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, 정수 nodeoffset,
			       const char *property, 정수 idx,
			       정수 *lenp)
{
	const char *list, *end;
	정수 length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	만약 (!list) {
		만약 (lenp)
			*lenp = length;

		반환 NULL;
	}

	end = list + length;

	동안 (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		만약 (list + length > end) {
			만약 (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			반환 NULL;
		}

		만약 (idx == 0) {
			만약 (lenp)
				*lenp = length - 1;

			반환 list;
		}

		list += length;
		idx--;
	}

	만약 (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	반환 NULL;
}

정수 fdt_node_check_compatible(const void *fdt, 정수 nodeoffset,
			      const char *compatible)
{
	const void *prop;
	정수 len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	만약 (!prop)
		반환 len;

	반환 !fdt_stringlist_contains(prop, len, compatible);
}

정수 fdt_node_offset_by_compatible(const void *fdt, 정수 startoffset,
				  const char *compatible)
{
	정수 offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	위해 (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		만약 ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			반환 err;
		아니면 만약 (err == 0)
			반환 offset;
	}

	반환 offset; /* error from fdt_next_node() */
}
