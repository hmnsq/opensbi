// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static цео fdt_nodename_eq_(const void *fdt, цео offset,
			    const char *s, цео len)
{
	цео olen;
	const char *p = fdt_get_name(fdt, offset, &olen);

	ако (!p || olen < len)
		/* short match */
		врати 0;

	ако (memcmp(p, s, len) != 0)
		врати 0;

	ако (p[len] == '\0')
		врати 1;
	инако ако (!memchr(s, '@', len) && (p[len] == '@'))
		врати 1;
	инако
		врати 0;
}

const char *fdt_get_string(const void *fdt, цео stroffset, цео *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	цео err;
	const char *s, *n;

	ако (can_assume(VALID_INPUT)) {
		s = (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ако (lenp)
			*lenp = strlen(s);
		врати s;
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
	} инако ако (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned цео sw_stroffset = -stroffset;

		ако ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ако (sw_stroffset < len)
			len = sw_stroffset;
	} инако {
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
	врати s;

fail:
	ако (lenp)
		*lenp = err;
	врати NULL;
}

const char *fdt_string(const void *fdt, цео stroffset)
{
	врати fdt_get_string(fdt, stroffset, NULL);
}

static цео fdt_string_eq_(const void *fdt, цео stroffset,
			  const char *s, цео len)
{
	цео slen;
	const char *p = fdt_get_string(fdt, stroffset, &slen);

	врати p && (slen == len) && (memcmp(p, s, len) == 0);
}

цео fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	цео offset = -1;

	док (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ако (offset < 0) {
			ако (offset == -FDT_ERR_NOTFOUND)
				break;

			врати offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ако (value > max)
			max = value;
	}

	ако (phandle)
		*phandle = max;

	врати 0;
}

цео fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	цео err;

	err = fdt_find_max_phandle(fdt, &max);
	ако (err < 0)
		врати err;

	ако (max == FDT_MAX_PHANDLE)
		врати -FDT_ERR_NOPHANDLES;

	ако (phandle)
		*phandle = max + 1;

	врати 0;
}

static const struct fdt_reserve_entry *fdt_mem_rsv(const void *fdt, цео n)
{
	unsigned цео offset = n * sizeof(struct fdt_reserve_entry);
	unsigned цео absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ако (!can_assume(VALID_INPUT)) {
		ако (absoffset < fdt_off_mem_rsvmap(fdt))
			врати NULL;
		ако (absoffset > fdt_totalsize(fdt) -
		    sizeof(struct fdt_reserve_entry))
			врати NULL;
	}
	врати fdt_mem_rsv_(fdt, n);
}

цео fdt_get_mem_rsv(const void *fdt, цео n, uint64_t *address, uint64_t *size)
{
	const struct fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ако (!can_assume(VALID_INPUT) && !re)
		врати -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	врати 0;
}

цео fdt_num_mem_rsv(const void *fdt)
{
	цео i;
	const struct fdt_reserve_entry *re;

	за (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ако (fdt64_ld_(&re->size) == 0)
			врати i;
	}
	врати -FDT_ERR_TRUNCATED;
}

static цео nextprop_(const void *fdt, цео offset)
{
	uint32_t tag;
	цео nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			ако (nextoffset >= 0)
				врати -FDT_ERR_BADSTRUCTURE;
			инако
				врати nextoffset;

		case FDT_PROP:
			врати offset;
		}
		offset = nextoffset;
	} док (tag == FDT_NOP);

	врати -FDT_ERR_NOTFOUND;
}

цео fdt_subnode_offset_namelen(const void *fdt, цео offset,
			       const char *name, цео namelen)
{
	цео depth;

	FDT_RO_PROBE(fdt);

	за (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ако ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			врати offset;

	ако (depth < 0)
		врати -FDT_ERR_NOTFOUND;
	врати offset; /* error */
}

цео fdt_subnode_offset(const void *fdt, цео parentoffset,
		       const char *name)
{
	врати fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

цео fdt_path_offset_namelen(const void *fdt, const char *path, цео namelen)
{
	const char *end = path + namelen;
	const char *p = path;
	цео offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ако (*path != '/') {
		const char *q = memchr(path, '/', end - p);

		ако (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ако (!p)
			врати -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	док (p < end) {
		const char *q;

		док (*p == '/') {
			p++;
			ако (p == end)
				врати offset;
		}
		q = memchr(p, '/', end - p);
		ако (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ако (offset < 0)
			врати offset;

		p = q;
	}

	врати offset;
}

цео fdt_path_offset(const void *fdt, const char *path)
{
	врати fdt_path_offset_namelen(fdt, path, strlen(path));
}

const char *fdt_get_name(const void *fdt, цео nodeoffset, цео *len)
{
	const struct fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const char *nameptr;
	цео err;

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

	врати nameptr;

 fail:
	ако (len)
		*len = err;
	врати NULL;
}

цео fdt_first_property_offset(const void *fdt, цео nodeoffset)
{
	цео offset;

	ако ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		врати offset;

	врати nextprop_(fdt, offset);
}

цео fdt_next_property_offset(const void *fdt, цео offset)
{
	ако ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		врати offset;

	врати nextprop_(fdt, offset);
}

static const struct fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              цео offset,
						              цео *lenp)
{
	цео err;
	const struct fdt_property *prop;

	ако (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ако (lenp)
			*lenp = err;
		врати NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ако (lenp)
		*lenp = fdt32_ld_(&prop->len);

	врати prop;
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      цео offset,
						      цео *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ако (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		врати NULL;
	}

	врати fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const struct fdt_property *fdt_get_property_namelen_(const void *fdt,
						            цео offset,
						            const char *name,
						            цео namelen,
							    цео *lenp,
							    цео *poffset)
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
			врати prop;
		}
	}

	ако (lenp)
		*lenp = offset;
	врати NULL;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    цео offset,
						    const char *name,
						    цео namelen, цео *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ако (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		врати NULL;
	}

	врати fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const struct fdt_property *fdt_get_property(const void *fdt,
					    цео nodeoffset,
					    const char *name, цео *lenp)
{
	врати fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, цео nodeoffset,
				const char *name, цео namelen, цео *lenp)
{
	цео poffset;
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ако (!prop)
		врати NULL;

	/* Handle realignment */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		врати prop->data + 4;
	врати prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, цео offset,
				  const char **namep, цео *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ако (!prop)
		врати NULL;
	ако (namep) {
		const char *name;
		цео namelen;

		ако (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ако (!name) {
				ако (lenp)
					*lenp = namelen;
				врати NULL;
			}
			*namep = name;
		} инако {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ако (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + sizeof(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		врати prop->data + 4;
	врати prop->data;
}

const void *fdt_getprop(const void *fdt, цео nodeoffset,
			const char *name, цео *lenp)
{
	врати fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, цео nodeoffset)
{
	const fdt32_t *php;
	цео len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ако (!php || (len != sizeof(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ако (!php || (len != sizeof(*php)))
			врати 0;
	}

	врати fdt32_ld_(php);
}

const char *fdt_get_alias_namelen(const void *fdt,
				  const char *name, цео namelen)
{
	цео aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ако (aliasoffset < 0)
		врати NULL;

	врати fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const char *fdt_get_alias(const void *fdt, const char *name)
{
	врати fdt_get_alias_namelen(fdt, name, strlen(name));
}

цео fdt_get_path(const void *fdt, цео nodeoffset, char *buf, цео buflen)
{
	цео pdepth = 0, p = 0;
	цео offset, depth, namelen;
	const char *name;

	FDT_RO_PROBE(fdt);

	ако (buflen < 2)
		врати -FDT_ERR_NOSPACE;

	за (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		док (pdepth > depth) {
			do {
				p--;
			} док (buf[p-1] != '/');
			pdepth--;
		}

		ако (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ако (!name)
				врати namelen;
			ако ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ако (offset == nodeoffset) {
			ако (pdepth < (depth + 1))
				врати -FDT_ERR_NOSPACE;

			ако (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			врати 0;
		}
	}

	ако ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		врати -FDT_ERR_BADOFFSET;
	инако ако (offset == -FDT_ERR_BADOFFSET)
		врати -FDT_ERR_BADSTRUCTURE;

	врати offset; /* error from fdt_next_node() */
}

цео fdt_supernode_atdepth_offset(const void *fdt, цео nodeoffset,
				 цео supernodedepth, цео *nodedepth)
{
	цео offset, depth;
	цео supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ако (supernodedepth < 0)
		врати -FDT_ERR_NOTFOUND;

	за (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ако (depth == supernodedepth)
			supernodeoffset = offset;

		ако (offset == nodeoffset) {
			ако (nodedepth)
				*nodedepth = depth;

			ако (supernodedepth > depth)
				врати -FDT_ERR_NOTFOUND;
			инако
				врати supernodeoffset;
		}
	}

	ако (!can_assume(VALID_INPUT)) {
		ако ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			врати -FDT_ERR_BADOFFSET;
		инако ако (offset == -FDT_ERR_BADOFFSET)
			врати -FDT_ERR_BADSTRUCTURE;
	}

	врати offset; /* error from fdt_next_node() */
}

цео fdt_node_depth(const void *fdt, цео nodeoffset)
{
	цео nodedepth;
	цео err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ако (err)
		врати (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	врати nodedepth;
}

цео fdt_parent_offset(const void *fdt, цео nodeoffset)
{
	цео nodedepth = fdt_node_depth(fdt, nodeoffset);

	ако (nodedepth < 0)
		врати nodedepth;
	врати fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

цео fdt_node_offset_by_prop_value(const void *fdt, цео startoffset,
				  const char *propname,
				  const void *propval, цео proplen)
{
	цео offset;
	const void *val;
	цео len;

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
			врати offset;
	}

	врати offset; /* error from fdt_next_node() */
}

цео fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	цео offset;

	ако ((phandle == 0) || (phandle == ~0U))
		врати -FDT_ERR_BADPHANDLE;

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
			врати offset;
	}

	врати offset; /* error from fdt_next_node() */
}

цео fdt_stringlist_contains(const char *strlist, цео listlen, const char *str)
{
	цео len = strlen(str);
	const char *p;

	док (listlen >= len) {
		ако (memcmp(str, strlist, len+1) == 0)
			врати 1;
		p = memchr(strlist, '\0', listlen);
		ако (!p)
			врати 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	врати 0;
}

цео fdt_stringlist_count(const void *fdt, цео nodeoffset, const char *property)
{
	const char *list, *end;
	цео length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list)
		врати length;

	end = list + length;

	док (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end)
			врати -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	врати count;
}

цео fdt_stringlist_search(const void *fdt, цео nodeoffset, const char *property,
			  const char *string)
{
	цео length, len, idx = 0;
	const char *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list)
		врати length;

	len = strlen(string) + 1;
	end = list + length;

	док (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end)
			врати -FDT_ERR_BADVALUE;

		ако (length == len && memcmp(list, string, length) == 0)
			врати idx;

		list += length;
		idx++;
	}

	врати -FDT_ERR_NOTFOUND;
}

const char *fdt_stringlist_get(const void *fdt, цео nodeoffset,
			       const char *property, цео idx,
			       цео *lenp)
{
	const char *list, *end;
	цео length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ако (!list) {
		ако (lenp)
			*lenp = length;

		врати NULL;
	}

	end = list + length;

	док (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ако (list + length > end) {
			ако (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			врати NULL;
		}

		ако (idx == 0) {
			ако (lenp)
				*lenp = length - 1;

			врати list;
		}

		list += length;
		idx--;
	}

	ако (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	врати NULL;
}

цео fdt_node_check_compatible(const void *fdt, цео nodeoffset,
			      const char *compatible)
{
	const void *prop;
	цео len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ако (!prop)
		врати len;

	врати !fdt_stringlist_contains(prop, len, compatible);
}

цео fdt_node_offset_by_compatible(const void *fdt, цео startoffset,
				  const char *compatible)
{
	цео offset, err;

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
			врати err;
		инако ако (err == 0)
			врати offset;
	}

	врати offset; /* error from fdt_next_node() */
}
