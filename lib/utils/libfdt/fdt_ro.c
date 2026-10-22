// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static цяло fdt_nodename_eq_(const void *fdt, цяло offset,
			    const char *s, цяло len)
{
	цяло olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	ако (!p || olen < len)
		/* short match */
		върни 0;

	ако (memcmp(p, s, len) != 0)
		върни 0;

	ако (p[len] == '\0')
		върни 1;
	иначе ако (!memchr(s, '@', len) && (p[len] == '@'))
		върни 1;
	иначе
		върни 0;
}

const char *fdt_get_string(const void *fdt, цяло stroffset, цяло *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	цяло err;
	const char *s, *n;

	ако (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ако (lenp)
			*lenp = strlen(s);
		върни s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ако (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ако (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ако (fdt_magic(fdt) == FDT_MAGIC) {
		ако (stroffset < 0)
			goto fail;
		ако (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ако ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ако ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} иначе ако (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned цяло sw_stroffset = -stroffset;

		ако ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ако (sw_stroffset < len)
			len = sw_stroffset;
	} иначе {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ако (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ако (lenp)
		*lenp = n - s;
	върни s;

fail:
	ако (lenp)
		*lenp = err;
	върни NULL;
}

const char *fdt_string(const void *fdt, цяло stroffset)
{
	върни fdt_get_string(fdt, stroffset, NULL);
}

static цяло fdt_string_eq_(const void *fdt, цяло stroffset,
			  const char *s, цяло len)
{
	цяло slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	върни p && (slen == len) && (memcmp(p, s, len) == 0);
}

цяло fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	цяло offset = -1;

	докато (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ако (offset < 0) {
			ако (offset == -FDT_ERR_NOTFOUND)
				break;

			върни offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ако (value > max)
			max = value;
	}

	ако (phandle)
		*phandle = max;

	върни 0;
}

цяло fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	цяло err;

	err = fdt_find_max_phandle(fdt, &max);
	ако (err < 0)
		върни err;

	ако (max == FDT_MAX_PHANDLE)
		върни -FDT_ERR_NOPHANDLES;

	ако (phandle)
		*phandle = max + 1;

	върни 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, цяло n)
{
	unsigned цяло offset = n * sizeof(struct fdt_reserve_entry);
	unsigned цяло absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ако (!can_assume(VALID_INPUT)) {
		ако (absoffset < fdt_off_mem_rsvmap(fdt))
			върни NULL;
		ако (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			върни NULL;
	}
	върни fdt_mem_rsv_(fdt, n);
}

цяло fdt_get_mem_rsv(const void *fdt, цяло n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ако (!can_assume(VALID_INPUT) && !re)
		върни -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	върни 0;
}

цяло fdt_num_mem_rsv(const void *fdt)
{
	цяло i;
	const struct fdt_reserve_entry *re;

	за (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ако (fdt64_ld_(&re->size) == 0)
			върни i;
	}
	върни -FDT_ERR_TRUNCATED;
}

static цяло nextprop_(const void *fdt, цяло offset)
{
	uint32_t tag;
	цяло nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			ако (nextoffset >= 0)
				върни -FDT_ERR_BADSTRUCTURE;
			иначе
				върни nextoffset;

		case FDT_PROP:
			върни offset;
		}
		offset = nextoffset;
	} докато (tag == FDT_NOP);

	върни -FDT_ERR_NOTFOUND;
}

цяло fdt_subnode_offset_namelen(const void *fdt, цяло offset,
			       const char *name, цяло namelen)
{
	цяло depth;

	FDT_RO_PROBE(fdt);

	за (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ако ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			върни offset;

	ако (depth < 0)
		върни -FDT_ERR_NOTFOUND;
	върни offset; /* error */
}

цяло fdt_subnode_offset(const void *fdt, цяло parentoffset,
		       const char *name)
{
	върни fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

цяло fdt_path_offset_namelen(const void *fdt, const char *path, цяло namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	цяло offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ако (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		ако (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ако (!p)
			върни -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	докато (p < end) {
		const char *q;

		докато (*p == '/') {
			p++;
			ако (p == end)
				върни offset;
		}
		q = memchr(p, '/', end - p);
		ако (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ако (offset < 0)
			върни offset;

		p = q;
	}

	върни offset;
}

цяло fdt_path_offset(const void *fdt, const char *path)
{
	върни fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, цяло nodeoffset, цяло *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	цяло err;

	ако (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		ако (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ако (len)
		*len = strlen(nameptr);

	върни nameptr;

 fail:
	ако (len)
		*len = err;
	върни NULL;
}

цяло fdt_first_property_offset(const void *fdt, цяло nodeoffset)
{
	цяло offset;

	ако ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		върни offset;

	върни nextprop_(fdt, offset);
}

цяло fdt_next_property_offset(const void *fdt, цяло offset)
{
	ако ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		върни offset;

	върни nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              цяло offset,
						              цяло *lenp)
{
	цяло err;
	const struct fdt_property *prop;

	ако (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ако (lenp)
			*lenp = err;
		върни NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ако (lenp)
		*lenp = fdt32_ld_(&prop->len);

	върни prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      цяло offset,
						      цяло *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ако (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		върни NULL;
	}

	върни fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            цяло offset,
						            const char *name,
						            цяло namelen,
							    цяло *lenp,
							    цяло *poffset)
{
	за (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ако (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		ако (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ако (poffset)
				*poffset = offset;
			върни prop;
		}
	}

	ако (lenp)
		*lenp = offset;
	върни NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    цяло offset,
						    const char *name,
						    цяло namelen, цяло *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ако (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		върни NULL;
	}

	върни fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    цяло nodeoffset,
					    const char *name, цяло *lenp)
{
	върни fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, цяло nodeoffset,
				const char *name, цяло namelen, цяло *lenp)
{
	цяло poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ако (!prop)
		върни NULL;

	/* Handle realignment */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		върни prop->data + 4;
	върни prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, цяло offset,
				  const char **namep, цяло *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ако (!prop)
		върни NULL;
	ако (namep) {
		const char *name;
		цяло namelen;

		ако (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ако (!name) {
				ако (lenp)
					*lenp = namelen;
				върни NULL;
			}
			*namep = name;
		} иначе {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		върни prop->data + 4;
	върни prop->data;
}

const void *fdt_getprop(const void *fdt, цяло nodeoffset,
			const char *name, цяло *lenp)
{
	върни fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, цяло nodeoffset)
{
	const fdt32_t *php;
	цяло len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ако (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ако (!php || (len != sizeof(*php)))
			върни 0;
	}

	върни fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, цяло namelen)
{
	цяло aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ако (aliasoffset < 0)
		върни NULL;

	върни fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	върни fdt_get_alias_namelen(fdt, name, strlen(name));
}

цяло fdt_get_path(const void *fdt, цяло nodeoffset, char *buf, цяло buflen)
{
	цяло pdepth = 0, p = 0;
	цяло offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	ако (buflen < 2)
		върни -FDT_ERR_NOSPACE;

	за (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		докато (pdepth > depth) {
			do {
				p--;
			} докато (buf[p-1] != '/');
			pdepth--;
		}

		ако (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ако (!name)
				върни namelen;
			ако ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ако (offset == nodeoffset) {
			ако (pdepth < (depth + 1))
				върни -FDT_ERR_NOSPACE;

			ако (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			върни 0;
		}
	}

	ако ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		върни -FDT_ERR_BADOFFSET;
	иначе ако (offset == -FDT_ERR_BADOFFSET)
		върни -FDT_ERR_BADSTRUCTURE;

	върни offset; /* error from fdt_next_node() */
}

цяло fdt_supernode_atdepth_offset(const void *fdt, цяло nodeoffset,
				 цяло supernodedepth, цяло *nodedepth)
{
	цяло offset, depth;
	цяло supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ако (supernodedepth < 0)
		върни -FDT_ERR_NOTFOUND;

	за (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ако (depth == supernodedepth)
			supernodeoffset = offset;

		ако (offset == nodeoffset) {
			ако (nodedepth)
				*nodedepth = depth;

			ако (supernodedepth > depth)
				върни -FDT_ERR_NOTFOUND;
			иначе
				върни supernodeoffset;
		}
	}

	ако (!can_assume(VALID_INPUT)) {
		ако ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			върни -FDT_ERR_BADOFFSET;
		иначе ако (offset == -FDT_ERR_BADOFFSET)
			върни -FDT_ERR_BADSTRUCTURE;
	}

	върни offset; /* error from fdt_next_node() */
}

цяло fdt_node_depth(const void *fdt, цяло nodeoffset)
{
	цяло nodedepth;
	цяло err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ако (err)
		върни (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	върни nodedepth;
}

цяло fdt_parent_offset(const void *fdt, цяло nodeoffset)
{
	цяло nodedepth = fdt_node_depth(fdt, nodeoffset);

	ако (nodedepth < 0)
		върни nodedepth;
	върни fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

цяло fdt_node_offset_by_prop_value(const void *fdt, цяло startoffset,
				  const char *propname,
				  const void *propval, цяло proplen)
{
	цяло offset;
	const void *val;
	цяло len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	за (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ако (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			върни offset;
	}

	върни offset; /* error from fdt_next_node() */
}

цяло fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	цяло offset;

	ако ((phandle == 0) || (phandle == ~0U))
		върни -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	за (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ако (fdt_get_phandle(fdt, offset) == phandle)
			върни offset;
	}

	върни offset; /* error from fdt_next_node() */
}

цяло fdt_stringlist_contains(const char *strlist, цяло listlen, const char *str)
{
	цяло len = strlen(str);
	const char *p;

	докато (listlen >= len) {
		ако (memcmp(str, strlist, len+1) == 0)
			върни 1;
		p = memchr(strlist, '\0', listlen);
		ако (!p)
			върни 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	върни 0;
}

цяло fdt_stringlist_count(const void *fdt, цяло nodeoffset, const char *property)
{
	const char *list, *end;
	цяло length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list)
		върни length;

	end = list + length;

	докато (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end)
			върни -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	върни count;
}

цяло fdt_stringlist_search(const void *fdt, цяло nodeoffset, const char *property,
			  const char *string)
{
	цяло length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list)
		върни length;

	len = strlen(string) + 1;
	end = list + length;

	докато (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end)
			върни -FDT_ERR_BADVALUE;

		ако (length == len && memcmp(list, string, length) == 0)
			върни idx;

		list += length;
		idx++;
	}

	върни -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, цяло nodeoffset,
			       const char *property, цяло idx,
			       цяло *lenp)
{
	const char *list, *end;
	цяло length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list) {
		ако (lenp)
			*lenp = length;

		върни NULL;
	}

	end = list + length;

	докато (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end) {
			ако (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			върни NULL;
		}

		ако (idx == 0) {
			ако (lenp)
				*lenp = length - 1;

			върни list;
		}

		list += length;
		idx--;
	}

	ако (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	върни NULL;
}

цяло fdt_node_check_compatible(const void *fdt, цяло nodeoffset,
			      const char *compatible)
{
	const void *prop;
	цяло len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ако (!prop)
		върни len;

	върни !fdt_stringlist_contains(prop, len, compatible);
}

цяло fdt_node_offset_by_compatible(const void *fdt, цяло startoffset,
				  const char *compatible)
{
	цяло offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	за (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ако ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			върни err;
		иначе ако (err == 0)
			върни offset;
	}

	върни offset; /* error from fdt_next_node() */
}
