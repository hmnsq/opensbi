// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static целое fdt_nodename_eq_(const void *fdt, целое offset,
			    const char *s, целое len)
{
	целое olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	если (!p || olen < len)
		/* short match */
		вернуть 0;

	если (memcmp(p, s, len) != 0)
		вернуть 0;

	если (p[len] == '\0')
		вернуть 1;
	иначе если (!memchr(s, '@', len) && (p[len] == '@'))
		вернуть 1;
	иначе
		вернуть 0;
}

const char *fdt_get_string(const void *fdt, целое stroffset, целое *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	целое err;
	const char *s, *n;

	если (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		если (lenp)
			*lenp = strlen(s);
		вернуть s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	если (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	если (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	если (fdt_magic(fdt) == FDT_MAGIC) {
		если (stroffset < 0)
			goto fail;
		если (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			если ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			если ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} иначе если (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned целое sw_stroffset = -stroffset;

		если ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		если (sw_stroffset < len)
			len = sw_stroffset;
	} иначе {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const char *)fdt + absoffset;
	n = memchr(s, '\0', len);
	если (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	если (lenp)
		*lenp = n - s;
	вернуть s;

fail:
	если (lenp)
		*lenp = err;
	вернуть NULL;
}

const char *fdt_string(const void *fdt, целое stroffset)
{
	вернуть fdt_get_string(fdt, stroffset, NULL);
}

static целое fdt_string_eq_(const void *fdt, целое stroffset,
			  const char *s, целое len)
{
	целое slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	вернуть p && (slen == len) && (memcmp(p, s, len) == 0);
}

целое fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	целое offset = -1;

	пока (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		если (offset < 0) {
			если (offset == -FDT_ERR_NOTFOUND)
				break;

			вернуть offset;
		}

		value = fdt_get_phandle(fdt, offset);

		если (value > max)
			max = value;
	}

	если (phandle)
		*phandle = max;

	вернуть 0;
}

целое fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	целое err;

	err = fdt_find_max_phandle(fdt, &max);
	если (err < 0)
		вернуть err;

	если (max == FDT_MAX_PHANDLE)
		вернуть -FDT_ERR_NOPHANDLES;

	если (phandle)
		*phandle = max + 1;

	вернуть 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, целое n)
{
	unsigned целое offset = n * sizeof(struct fdt_reserve_entry);
	unsigned целое absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	если (!can_assume(VALID_INPUT)) {
		если (absoffset < fdt_off_mem_rsvmap(fdt))
			вернуть NULL;
		если (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			вернуть NULL;
	}
	вернуть fdt_mem_rsv_(fdt, n);
}

целое fdt_get_mem_rsv(const void *fdt, целое n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	если (!can_assume(VALID_INPUT) && !re)
		вернуть -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	вернуть 0;
}

целое fdt_num_mem_rsv(const void *fdt)
{
	целое i;
	const struct fdt_reserve_entry *re;

	для (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		если (fdt64_ld_(&re->size) == 0)
			вернуть i;
	}
	вернуть -FDT_ERR_TRUNCATED;
}

static целое nextprop_(const void *fdt, целое offset)
{
	uint32_t tag;
	целое nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			если (nextoffset >= 0)
				вернуть -FDT_ERR_BADSTRUCTURE;
			иначе
				вернуть nextoffset;

		case FDT_PROP:
			вернуть offset;
		}
		offset = nextoffset;
	} пока (tag == FDT_NOP);

	вернуть -FDT_ERR_NOTFOUND;
}

целое fdt_subnode_offset_namelen(const void *fdt, целое offset,
			       const char *name, целое namelen)
{
	целое depth;

	FDT_RO_PROBE(fdt);

	для (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		если ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			вернуть offset;

	если (depth < 0)
		вернуть -FDT_ERR_NOTFOUND;
	вернуть offset; /* error */
}

целое fdt_subnode_offset(const void *fdt, целое parentoffset,
		       const char *name)
{
	вернуть fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

целое fdt_path_offset_namelen(const void *fdt, const char *path, целое namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	целое offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	если (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		если (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		если (!p)
			вернуть -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	пока (p < end) {
		const char *q;

		пока (*p == '/') {
			p++;
			если (p == end)
				вернуть offset;
		}
		q = memchr(p, '/', end - p);
		если (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		если (offset < 0)
			вернуть offset;

		p = q;
	}

	вернуть offset;
}

целое fdt_path_offset(const void *fdt, const char *path)
{
	вернуть fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, целое nodeoffset, целое *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	целое err;

	если (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	если (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const char *leaf;
		leaf = strrchr(nameptr, '/');
		если (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	если (len)
		*len = strlen(nameptr);

	вернуть nameptr;

 fail:
	если (len)
		*len = err;
	вернуть NULL;
}

целое fdt_first_property_offset(const void *fdt, целое nodeoffset)
{
	целое offset;

	если ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		вернуть offset;

	вернуть nextprop_(fdt, offset);
}

целое fdt_next_property_offset(const void *fdt, целое offset)
{
	если ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		вернуть offset;

	вернуть nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              целое offset,
						              целое *lenp)
{
	целое err;
	const struct fdt_property *prop;

	если (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		если (lenp)
			*lenp = err;
		вернуть NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	если (lenp)
		*lenp = fdt32_ld_(&prop->len);

	вернуть prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      целое offset,
						      целое *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	если (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		если (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		вернуть NULL;
	}

	вернуть fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            целое offset,
						            const char *name,
						            целое namelen,
							    целое *lenp,
							    целое *poffset)
{
	для (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		если (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}
		если (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			если (poffset)
				*poffset = offset;
			вернуть prop;
		}
	}

	если (lenp)
		*lenp = offset;
	вернуть NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    целое offset,
						    const char *name,
						    целое namelen, целое *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	если (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		если (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		вернуть NULL;
	}

	вернуть fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    целое nodeoffset,
					    const char *name, целое *lenp)
{
	вернуть fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, целое nodeoffset,
				const char *name, целое namelen, целое *lenp)
{
	целое poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	если (!prop)
		вернуть NULL;

	/* Handle realignment */
	если (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		вернуть prop->data + 4;
	вернуть prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, целое offset,
				  const char **namep, целое *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	если (!prop)
		вернуть NULL;
	если (namep) {
		const char *name;
		целое namelen;

		если (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			если (!name) {
				если (lenp)
					*lenp = namelen;
				вернуть NULL;
			}
			*namep = name;
		} иначе {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	если (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		вернуть prop->data + 4;
	вернуть prop->data;
}

const void *fdt_getprop(const void *fdt, целое nodeoffset,
			const char *name, целое *lenp)
{
	вернуть fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, целое nodeoffset)
{
	const fdt32_t *php;
	целое len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	если (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		если (!php || (len != sizeof(*php)))
			вернуть 0;
	}

	вернуть fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, целое namelen)
{
	целое aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	если (aliasoffset < 0)
		вернуть NULL;

	вернуть fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	вернуть fdt_get_alias_namelen(fdt, name, strlen(name));
}

целое fdt_get_path(const void *fdt, целое nodeoffset, char *buf, целое buflen)
{
	целое pdepth = 0, p = 0;
	целое offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	если (buflen < 2)
		вернуть -FDT_ERR_NOSPACE;

	для (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		пока (pdepth > depth) {
			do {
				p--;
			} пока (buf[p-1] != '/');
			pdepth--;
		}

		если (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			если (!name)
				вернуть namelen;
			если ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		если (offset == nodeoffset) {
			если (pdepth < (depth + 1))
				вернуть -FDT_ERR_NOSPACE;

			если (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			вернуть 0;
		}
	}

	если ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		вернуть -FDT_ERR_BADOFFSET;
	иначе если (offset == -FDT_ERR_BADOFFSET)
		вернуть -FDT_ERR_BADSTRUCTURE;

	вернуть offset; /* error from fdt_next_node() */
}

целое fdt_supernode_atdepth_offset(const void *fdt, целое nodeoffset,
				 целое supernodedepth, целое *nodedepth)
{
	целое offset, depth;
	целое supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	если (supernodedepth < 0)
		вернуть -FDT_ERR_NOTFOUND;

	для (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		если (depth == supernodedepth)
			supernodeoffset = offset;

		если (offset == nodeoffset) {
			если (nodedepth)
				*nodedepth = depth;

			если (supernodedepth > depth)
				вернуть -FDT_ERR_NOTFOUND;
			иначе
				вернуть supernodeoffset;
		}
	}

	если (!can_assume(VALID_INPUT)) {
		если ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			вернуть -FDT_ERR_BADOFFSET;
		иначе если (offset == -FDT_ERR_BADOFFSET)
			вернуть -FDT_ERR_BADSTRUCTURE;
	}

	вернуть offset; /* error from fdt_next_node() */
}

целое fdt_node_depth(const void *fdt, целое nodeoffset)
{
	целое nodedepth;
	целое err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	если (err)
		вернуть (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	вернуть nodedepth;
}

целое fdt_parent_offset(const void *fdt, целое nodeoffset)
{
	целое nodedepth = fdt_node_depth(fdt, nodeoffset);

	если (nodedepth < 0)
		вернуть nodedepth;
	вернуть fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

целое fdt_node_offset_by_prop_value(const void *fdt, целое startoffset,
				  const char *propname,
				  const void *propval, целое proplen)
{
	целое offset;
	const void *val;
	целое len;

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
		если (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			вернуть offset;
	}

	вернуть offset; /* error from fdt_next_node() */
}

целое fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	целое offset;

	если ((phandle == 0) || (phandle == ~0U))
		вернуть -FDT_ERR_BADPHANDLE;

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
		если (fdt_get_phandle(fdt, offset) == phandle)
			вернуть offset;
	}

	вернуть offset; /* error from fdt_next_node() */
}

целое fdt_stringlist_contains(const char *strlist, целое listlen, const char *str)
{
	целое len = strlen(str);
	const char *p;

	пока (listlen >= len) {
		если (memcmp(str, strlist, len+1) == 0)
			вернуть 1;
		p = memchr(strlist, '\0', listlen);
		если (!p)
			вернуть 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	вернуть 0;
}

целое fdt_stringlist_count(const void *fdt, целое nodeoffset, const char *property)
{
	const char *list, *end;
	целое length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	если (!list)
		вернуть length;

	end = list + length;

	пока (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		если (list + length > end)
			вернуть -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	вернуть count;
}

целое fdt_stringlist_search(const void *fdt, целое nodeoffset, const char *property,
			  const char *string)
{
	целое length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	если (!list)
		вернуть length;

	len = strlen(string) + 1;
	end = list + length;

	пока (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		если (list + length > end)
			вернуть -FDT_ERR_BADVALUE;

		если (length == len && memcmp(list, string, length) == 0)
			вернуть idx;

		list += length;
		idx++;
	}

	вернуть -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, целое nodeoffset,
			       const char *property, целое idx,
			       целое *lenp)
{
	const char *list, *end;
	целое length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	если (!list) {
		если (lenp)
			*lenp = length;

		вернуть NULL;
	}

	end = list + length;

	пока (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		если (list + length > end) {
			если (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			вернуть NULL;
		}

		если (idx == 0) {
			если (lenp)
				*lenp = length - 1;

			вернуть list;
		}

		list += length;
		idx--;
	}

	если (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	вернуть NULL;
}

целое fdt_node_check_compatible(const void *fdt, целое nodeoffset,
			      const char *compatible)
{
	const void *prop;
	целое len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	если (!prop)
		вернуть len;

	вернуть !fdt_stringlist_contains(prop, len, compatible);
}

целое fdt_node_offset_by_compatible(const void *fdt, целое startoffset,
				  const char *compatible)
{
	целое offset, err;

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
		если ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			вернуть err;
		иначе если (err == 0)
			вернуть offset;
	}

	вернуть offset; /* error from fdt_next_node() */
}
