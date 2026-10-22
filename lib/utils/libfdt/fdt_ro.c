// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ціле fdt_nodename_eq_(const void *fdt, ціле offset,
			    const char *s, ціле len)
{
	ціле olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	якщо (!p || olen < len)
		/* short match */
		повернути 0;

	якщо (memcmp(p, s, len) != 0)
		повернути 0;

	якщо (p[len] == '\0')
		повернути 1;
	інакше якщо (!memchr(s, '@', len) && (p[len] == '@'))
		повернути 1;
	інакше
		повернути 0;
}

const char *fdt_get_string(const void *fdt, ціле stroffset, ціле *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ціле err;
	const char *s, *n;

	якщо (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		якщо (lenp)
			*lenp = strlen(s);
		повернути s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	якщо (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	якщо (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	якщо (fdt_magic(fdt) == FDT_MAGIC) {
		якщо (stroffset < 0)
			goto fail;
		якщо (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			якщо ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			якщо ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} інакше якщо (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ціле sw_stroffset = -stroffset;

		якщо ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		якщо (sw_stroffset < len)
			len = sw_stroffset;
	} інакше {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	якщо (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	якщо (lenp)
		*lenp = n - s;
	повернути s;

fail:
	якщо (lenp)
		*lenp = err;
	повернути NULL;
}

const char *fdt_string(const void *fdt, ціле stroffset)
{
	повернути fdt_get_string(fdt, stroffset, NULL);
}

static ціле fdt_string_eq_(const void *fdt, ціле stroffset,
			  const char *s, ціле len)
{
	ціле slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	повернути p && (slen == len) && (memcmp(p, s, len) == 0);
}

ціле fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ціле offset = -1;

	поки (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		якщо (offset < 0) {
			якщо (offset == -FDT_ERR_NOTFOUND)
				break;

			повернути offset;
		}

		value = fdt_get_phandle(fdt, offset);

		якщо (value > max)
			max = value;
	}

	якщо (phandle)
		*phandle = max;

	повернути 0;
}

ціле fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ціле err;

	err = fdt_find_max_phandle(fdt, &max);
	якщо (err < 0)
		повернути err;

	якщо (max == FDT_MAX_PHANDLE)
		повернути -FDT_ERR_NOPHANDLES;

	якщо (phandle)
		*phandle = max + 1;

	повернути 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ціле n)
{
	unsigned ціле offset = n * sizeof(struct fdt_reserve_entry);
	unsigned ціле absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	якщо (!can_assume(VALID_INPUT)) {
		якщо (absoffset < fdt_off_mem_rsvmap(fdt))
			повернути NULL;
		якщо (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			повернути NULL;
	}
	повернути fdt_mem_rsv_(fdt, n);
}

ціле fdt_get_mem_rsv(const void *fdt, ціле n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	якщо (!can_assume(VALID_INPUT) && !re)
		повернути -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	повернути 0;
}

ціле fdt_num_mem_rsv(const void *fdt)
{
	ціле i;
	const struct fdt_reserve_entry *re;

	для (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		якщо (fdt64_ld_(&re->size) == 0)
			повернути i;
	}
	повернути -FDT_ERR_TRUNCATED;
}

static ціле nextprop_(const void *fdt, ціле offset)
{
	uint32_t tag;
	ціле nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			якщо (nextoffset >= 0)
				повернути -FDT_ERR_BADSTRUCTURE;
			інакше
				повернути nextoffset;

		case FDT_PROP:
			повернути offset;
		}
		offset = nextoffset;
	} поки (tag == FDT_NOP);

	повернути -FDT_ERR_NOTFOUND;
}

ціле fdt_subnode_offset_namelen(const void *fdt, ціле offset,
			       const char *name, ціле namelen)
{
	ціле depth;

	FDT_RO_PROBE(fdt);

	для (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		якщо ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			повернути offset;

	якщо (depth < 0)
		повернути -FDT_ERR_NOTFOUND;
	повернути offset; /* error */
}

ціле fdt_subnode_offset(const void *fdt, ціле parentoffset,
		       const char *name)
{
	повернути fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ціле fdt_path_offset_namelen(const void *fdt, const char *path, ціле namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	ціле offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	якщо (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		якщо (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		якщо (!p)
			повернути -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	поки (p < end) {
		const char *q;

		поки (*p == '/') {
			p++;
			якщо (p == end)
				повернути offset;
		}
		q = memchr(p, '/', end - p);
		якщо (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		якщо (offset < 0)
			повернути offset;

		p = q;
	}

	повернути offset;
}

ціле fdt_path_offset(const void *fdt, const char *path)
{
	повернути fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, ціле nodeoffset, ціле *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	ціле err;

	якщо (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	якщо (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		якщо (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	якщо (len)
		*len = strlen(nameptr);

	повернути nameptr;

 fail:
	якщо (len)
		*len = err;
	повернути NULL;
}

ціле fdt_first_property_offset(const void *fdt, ціле nodeoffset)
{
	ціле offset;

	якщо ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		повернути offset;

	повернути nextprop_(fdt, offset);
}

ціле fdt_next_property_offset(const void *fdt, ціле offset)
{
	якщо ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		повернути offset;

	повернути nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ціле offset,
						              ціле *lenp)
{
	ціле err;
	const struct fdt_property *prop;

	якщо (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		якщо (lenp)
			*lenp = err;
		повернути NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	якщо (lenp)
		*lenp = fdt32_ld_(&prop->len);

	повернути prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ціле offset,
						      ціле *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	якщо (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		якщо (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		повернути NULL;
	}

	повернути fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ціле offset,
						            const char *name,
						            ціле namelen,
							    ціле *lenp,
							    ціле *poffset)
{
	для (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		якщо (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		якщо (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			якщо (poffset)
				*poffset = offset;
			повернути prop;
		}
	}

	якщо (lenp)
		*lenp = offset;
	повернути NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    ціле offset,
						    const char *name,
						    ціле namelen, ціле *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	якщо (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		якщо (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		повернути NULL;
	}

	повернути fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    ціле nodeoffset,
					    const char *name, ціле *lenp)
{
	повернути fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ціле nodeoffset,
				const char *name, ціле namelen, ціле *lenp)
{
	ціле poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	якщо (!prop)
		повернути NULL;

	/* Handle realignment */
	якщо (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		повернути prop->data + 4;
	повернути prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ціле offset,
				  const char **namep, ціле *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	якщо (!prop)
		повернути NULL;
	якщо (namep) {
		const char *name;
		ціле namelen;

		якщо (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			якщо (!name) {
				якщо (lenp)
					*lenp = namelen;
				повернути NULL;
			}
			*namep = name;
		} інакше {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	якщо (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		повернути prop->data + 4;
	повернути prop->data;
}

const void *fdt_getprop(const void *fdt, ціле nodeoffset,
			const char *name, ціле *lenp)
{
	повернути fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ціле nodeoffset)
{
	const fdt32_t *php;
	ціле len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	якщо (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		якщо (!php || (len != sizeof(*php)))
			повернути 0;
	}

	повернути fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, ціле namelen)
{
	ціле aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	якщо (aliasoffset < 0)
		повернути NULL;

	повернути fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	повернути fdt_get_alias_namelen(fdt, name, strlen(name));
}

ціле fdt_get_path(const void *fdt, ціле nodeoffset, char *buf, ціле buflen)
{
	ціле pdepth = 0, p = 0;
	ціле offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	якщо (buflen < 2)
		повернути -FDT_ERR_NOSPACE;

	для (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		поки (pdepth > depth) {
			do {
				p--;
			} поки (buf[p-1] != '/');
			pdepth--;
		}

		якщо (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			якщо (!name)
				повернути namelen;
			якщо ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		якщо (offset == nodeoffset) {
			якщо (pdepth < (depth + 1))
				повернути -FDT_ERR_NOSPACE;

			якщо (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			повернути 0;
		}
	}

	якщо ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		повернути -FDT_ERR_BADOFFSET;
	інакше якщо (offset == -FDT_ERR_BADOFFSET)
		повернути -FDT_ERR_BADSTRUCTURE;

	повернути offset; /* error from fdt_next_node() */
}

ціле fdt_supernode_atdepth_offset(const void *fdt, ціле nodeoffset,
				 ціле supernodedepth, ціле *nodedepth)
{
	ціле offset, depth;
	ціле supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	якщо (supernodedepth < 0)
		повернути -FDT_ERR_NOTFOUND;

	для (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		якщо (depth == supernodedepth)
			supernodeoffset = offset;

		якщо (offset == nodeoffset) {
			якщо (nodedepth)
				*nodedepth = depth;

			якщо (supernodedepth > depth)
				повернути -FDT_ERR_NOTFOUND;
			інакше
				повернути supernodeoffset;
		}
	}

	якщо (!can_assume(VALID_INPUT)) {
		якщо ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			повернути -FDT_ERR_BADOFFSET;
		інакше якщо (offset == -FDT_ERR_BADOFFSET)
			повернути -FDT_ERR_BADSTRUCTURE;
	}

	повернути offset; /* error from fdt_next_node() */
}

ціле fdt_node_depth(const void *fdt, ціле nodeoffset)
{
	ціле nodedepth;
	ціле err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	якщо (err)
		повернути (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	повернути nodedepth;
}

ціле fdt_parent_offset(const void *fdt, ціле nodeoffset)
{
	ціле nodedepth = fdt_node_depth(fdt, nodeoffset);

	якщо (nodedepth < 0)
		повернути nodedepth;
	повернути fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

ціле fdt_node_offset_by_prop_value(const void *fdt, ціле startoffset,
				  const char *propname,
				  const void *propval, ціле proplen)
{
	ціле offset;
	const void *val;
	ціле len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	для (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		якщо (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			повернути offset;
	}

	повернути offset; /* error from fdt_next_node() */
}

ціле fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ціле offset;

	якщо ((phandle == 0) || (phandle == ~0U))
		повернути -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	для (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		якщо (fdt_get_phandle(fdt, offset) == phandle)
			повернути offset;
	}

	повернути offset; /* error from fdt_next_node() */
}

ціле fdt_stringlist_contains(const char *strlist, ціле listlen, const char *str)
{
	ціле len = strlen(str);
	const char *p;

	поки (listlen >= len) {
		якщо (memcmp(str, strlist, len+1) == 0)
			повернути 1;
		p = memchr(strlist, '\0', listlen);
		якщо (!p)
			повернути 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	повернути 0;
}

ціле fdt_stringlist_count(const void *fdt, ціле nodeoffset, const char *property)
{
	const char *list, *end;
	ціле length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	якщо (!list)
		повернути length;

	end = list + length;

	поки (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		якщо (list + length > end)
			повернути -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	повернути count;
}

ціле fdt_stringlist_search(const void *fdt, ціле nodeoffset, const char *property,
			  const char *string)
{
	ціле length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	якщо (!list)
		повернути length;

	len = strlen(string) + 1;
	end = list + length;

	поки (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		якщо (list + length > end)
			повернути -FDT_ERR_BADVALUE;

		якщо (length == len && memcmp(list, string, length) == 0)
			повернути idx;

		list += length;
		idx++;
	}

	повернути -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, ціле nodeoffset,
			       const char *property, ціле idx,
			       ціле *lenp)
{
	const char *list, *end;
	ціле length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	якщо (!list) {
		якщо (lenp)
			*lenp = length;

		повернути NULL;
	}

	end = list + length;

	поки (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		якщо (list + length > end) {
			якщо (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			повернути NULL;
		}

		якщо (idx == 0) {
			якщо (lenp)
				*lenp = length - 1;

			повернути list;
		}

		list += length;
		idx--;
	}

	якщо (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	повернути NULL;
}

ціле fdt_node_check_compatible(const void *fdt, ціле nodeoffset,
			      const char *compatible)
{
	const void *prop;
	ціле len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	якщо (!prop)
		повернути len;

	повернути !fdt_stringlist_contains(prop, len, compatible);
}

ціле fdt_node_offset_by_compatible(const void *fdt, ціле startoffset,
				  const char *compatible)
{
	ціле offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	для (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		якщо ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			повернути err;
		інакше якщо (err == 0)
			повернути offset;
	}

	повернути offset; /* error from fdt_next_node() */
}
