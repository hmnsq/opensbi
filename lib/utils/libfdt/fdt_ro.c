// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ཧྲིལ fdt_nodename_eq_(const void *fdt, ཧྲིལ offset,
			    const var_na *s, ཧྲིལ len)
{
	ཧྲིལ olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	གལ་སྲིད (!p || olen < len)
		/* short match */
		ལོག 0;

	གལ་སྲིད (memcmp(p, s, len) != 0)
		ལོག 0;

	གལ་སྲིད (p[len] == '\0')
		ལོག 1;
	དེ་མིན གལ་སྲིད (!memchr(s, '@', len) && (p[len] == '@'))
		ལོག 1;
	དེ་མིན
		ལོག 0;
}

const var_na *fdt_get_string(const void *fdt, ཧྲིལ stroffset, ཧྲིལ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ཧྲིལ err;
	const var_na *s, *n;

	གལ་སྲིད (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		གལ་སྲིད (lenp)
			*lenp = strlen(s);
		ལོག s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	གལ་སྲིད (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	གལ་སྲིད (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	གལ་སྲིད (fdt_magic(fdt) == FDT_MAGIC) {
		གལ་སྲིད (stroffset < 0)
			goto fail;
		གལ་སྲིད (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			གལ་སྲིད ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			གལ་སྲིད ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} དེ་མིན གལ་སྲིད (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ཧྲིལ sw_stroffset = -stroffset;

		གལ་སྲིད ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		གལ་སྲིད (sw_stroffset < len)
			len = sw_stroffset;
	} དེ་མིན {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	གལ་སྲིད (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	གལ་སྲིད (lenp)
		*lenp = n - s;
	ལོག s;

fail:
	གལ་སྲིད (lenp)
		*lenp = err;
	ལོག NULL;
}

const var_na *fdt_string(const void *fdt, ཧྲིལ stroffset)
{
	ལོག fdt_get_string(fdt, stroffset, NULL);
}

static ཧྲིལ fdt_string_eq_(const void *fdt, ཧྲིལ stroffset,
			  const var_na *s, ཧྲིལ len)
{
	ཧྲིལ slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ལོག p && (slen == len) && (memcmp(p, s, len) == 0);
}

ཧྲིལ fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ཧྲིལ offset = -1;

	སྐབས (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		གལ་སྲིད (offset < 0) {
			གལ་སྲིད (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ལོག offset;
		}

		value = fdt_get_phandle(fdt, offset);

		གལ་སྲིད (value > max)
			max = value;
	}

	གལ་སྲིད (phandle)
		*phandle = max;

	ལོག 0;
}

ཧྲིལ fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ཧྲིལ err;

	err = fdt_find_max_phandle(fdt, &max);
	གལ་སྲིད (err < 0)
		ལོག err;

	གལ་སྲིད (max == FDT_MAX_PHANDLE)
		ལོག -FDT_ERR_NOPHANDLES;

	གལ་སྲིད (phandle)
		*phandle = max + 1;

	ལོག 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ཧྲིལ n)
{
	unsigned ཧྲིལ offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned ཧྲིལ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	གལ་སྲིད (!can_assume(VALID_INPUT)) {
		གལ་སྲིད (absoffset < fdt_off_mem_rsvmap(fdt))
			ལོག NULL;
		གལ་སྲིད (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ལོག NULL;
	}
	ལོག fdt_mem_rsv_(fdt, n);
}

ཧྲིལ fdt_get_mem_rsv(const void *fdt, ཧྲིལ n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	གལ་སྲིད (!can_assume(VALID_INPUT) && !re)
		ལོག -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ལོག 0;
}

ཧྲིལ fdt_num_mem_rsv(const void *fdt)
{
	ཧྲིལ i;
	const sa_nracanaa fdt_reserve_entry *re;

	དོན་ལུ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		གལ་སྲིད (fdt64_ld_(&re->size) == 0)
			ལོག i;
	}
	ལོག -FDT_ERR_TRUNCATED;
}

static ཧྲིལ nextprop_(const void *fdt, ཧྲིལ offset)
{
	uint32_t tag;
	ཧྲིལ nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			གལ་སྲིད (nextoffset >= 0)
				ལོག -FDT_ERR_BADSTRUCTURE;
			དེ་མིན
				ལོག nextoffset;

		pak_sa FDT_PROP:
			ལོག offset;
		}
		offset = nextoffset;
	} སྐབས (tag == FDT_NOP);

	ལོག -FDT_ERR_NOTFOUND;
}

ཧྲིལ fdt_subnode_offset_namelen(const void *fdt, ཧྲིལ offset,
			       const var_na *name, ཧྲིལ namelen)
{
	ཧྲིལ depth;

	FDT_RO_PROBE(fdt);

	དོན་ལུ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		གལ་སྲིད ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ལོག offset;

	གལ་སྲིད (depth < 0)
		ལོག -FDT_ERR_NOTFOUND;
	ལོག offset; /* error */
}

ཧྲིལ fdt_subnode_offset(const void *fdt, ཧྲིལ parentoffset,
		       const var_na *name)
{
	ལོག fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ཧྲིལ fdt_path_offset_namelen(const void *fdt, const var_na *path, ཧྲིལ namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	ཧྲིལ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	གལ་སྲིད (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		གལ་སྲིད (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		གལ་སྲིད (!p)
			ལོག -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	སྐབས (p < end) {
		const var_na *q;

		སྐབས (*p == '/') {
			p++;
			གལ་སྲིད (p == end)
				ལོག offset;
		}
		q = memchr(p, '/', end - p);
		གལ་སྲིད (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		གལ་སྲིད (offset < 0)
			ལོག offset;

		p = q;
	}

	ལོག offset;
}

ཧྲིལ fdt_path_offset(const void *fdt, const var_na *path)
{
	ལོག fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, ཧྲིལ nodeoffset, ཧྲིལ *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	ཧྲིལ err;

	གལ་སྲིད (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		གལ་སྲིད (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	གལ་སྲིད (len)
		*len = strlen(nameptr);

	ལོག nameptr;

 fail:
	གལ་སྲིད (len)
		*len = err;
	ལོག NULL;
}

ཧྲིལ fdt_first_property_offset(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ offset;

	གལ་སྲིད ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ལོག offset;

	ལོག nextprop_(fdt, offset);
}

ཧྲིལ fdt_next_property_offset(const void *fdt, ཧྲིལ offset)
{
	གལ་སྲིད ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ལོག offset;

	ལོག nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ཧྲིལ offset,
						              ཧྲིལ *lenp)
{
	ཧྲིལ err;
	const sa_nracanaa fdt_property *prop;

	གལ་སྲིད (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		གལ་སྲིད (lenp)
			*lenp = err;
		ལོག NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	གལ་སྲིད (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ལོག prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ཧྲིལ offset,
						      ཧྲིལ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		གལ་སྲིད (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ལོག NULL;
	}

	ལོག fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ཧྲིལ offset,
						            const var_na *name,
						            ཧྲིལ namelen,
							    ཧྲིལ *lenp,
							    ཧྲིལ *poffset)
{
	དོན་ལུ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		གལ་སྲིད (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		གལ་སྲིད (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			གལ་སྲིད (poffset)
				*poffset = offset;
			ལོག prop;
		}
	}

	གལ་སྲིད (lenp)
		*lenp = offset;
	ལོག NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    ཧྲིལ offset,
						    const var_na *name,
						    ཧྲིལ namelen, ཧྲིལ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		གལ་སྲིད (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ལོག NULL;
	}

	ལོག fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    ཧྲིལ nodeoffset,
					    const var_na *name, ཧྲིལ *lenp)
{
	ལོག fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ཧྲིལ nodeoffset,
				const var_na *name, ཧྲིལ namelen, ཧྲིལ *lenp)
{
	ཧྲིལ poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	གལ་སྲིད (!prop)
		ལོག NULL;

	/* Handle realignment */
	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ལོག prop->data + 4;
	ལོག prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ཧྲིལ offset,
				  const var_na **namep, ཧྲིལ *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	གལ་སྲིད (!prop)
		ལོག NULL;
	གལ་སྲིད (namep) {
		const var_na *name;
		ཧྲིལ namelen;

		གལ་སྲིད (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			གལ་སྲིད (!name) {
				གལ་སྲིད (lenp)
					*lenp = namelen;
				ལོག NULL;
			}
			*namep = name;
		} དེ་མིན {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	གལ་སྲིད (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ལོག prop->data + 4;
	ལོག prop->data;
}

const void *fdt_getprop(const void *fdt, ཧྲིལ nodeoffset,
			const var_na *name, ཧྲིལ *lenp)
{
	ལོག fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ཧྲིལ nodeoffset)
{
	const fdt32_t *php;
	ཧྲིལ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	གལ་སྲིད (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		གལ་སྲིད (!php || (len != parimaa_na(*php)))
			ལོག 0;
	}

	ལོག fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, ཧྲིལ namelen)
{
	ཧྲིལ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	གལ་སྲིད (aliasoffset < 0)
		ལོག NULL;

	ལོག fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ལོག fdt_get_alias_namelen(fdt, name, strlen(name));
}

ཧྲིལ fdt_get_path(const void *fdt, ཧྲིལ nodeoffset, var_na *buf, ཧྲིལ buflen)
{
	ཧྲིལ pdepth = 0, p = 0;
	ཧྲིལ offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	གལ་སྲིད (buflen < 2)
		ལོག -FDT_ERR_NOSPACE;

	དོན་ལུ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		སྐབས (pdepth > depth) {
			do {
				p--;
			} སྐབས (buf[p-1] != '/');
			pdepth--;
		}

		གལ་སྲིད (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			གལ་སྲིད (!name)
				ལོག namelen;
			གལ་སྲིད ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		གལ་སྲིད (offset == nodeoffset) {
			གལ་སྲིད (pdepth < (depth + 1))
				ལོག -FDT_ERR_NOSPACE;

			གལ་སྲིད (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ལོག 0;
		}
	}

	གལ་སྲིད ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ལོག -FDT_ERR_BADOFFSET;
	དེ་མིན གལ་སྲིད (offset == -FDT_ERR_BADOFFSET)
		ལོག -FDT_ERR_BADSTRUCTURE;

	ལོག offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_supernode_atdepth_offset(const void *fdt, ཧྲིལ nodeoffset,
				 ཧྲིལ supernodedepth, ཧྲིལ *nodedepth)
{
	ཧྲིལ offset, depth;
	ཧྲིལ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	གལ་སྲིད (supernodedepth < 0)
		ལོག -FDT_ERR_NOTFOUND;

	དོན་ལུ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		གལ་སྲིད (depth == supernodedepth)
			supernodeoffset = offset;

		གལ་སྲིད (offset == nodeoffset) {
			གལ་སྲིད (nodedepth)
				*nodedepth = depth;

			གལ་སྲིད (supernodedepth > depth)
				ལོག -FDT_ERR_NOTFOUND;
			དེ་མིན
				ལོག supernodeoffset;
		}
	}

	གལ་སྲིད (!can_assume(VALID_INPUT)) {
		གལ་སྲིད ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ལོག -FDT_ERR_BADOFFSET;
		དེ་མིན གལ་སྲིད (offset == -FDT_ERR_BADOFFSET)
			ལོག -FDT_ERR_BADSTRUCTURE;
	}

	ལོག offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_node_depth(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ nodedepth;
	ཧྲིལ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	གལ་སྲིད (err)
		ལོག (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ལོག nodedepth;
}

ཧྲིལ fdt_parent_offset(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ nodedepth = fdt_node_depth(fdt, nodeoffset);

	གལ་སྲིད (nodedepth < 0)
		ལོག nodedepth;
	ལོག fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

ཧྲིལ fdt_node_offset_by_prop_value(const void *fdt, ཧྲིལ startoffset,
				  const var_na *propname,
				  const void *propval, ཧྲིལ proplen)
{
	ཧྲིལ offset;
	const void *val;
	ཧྲིལ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	དོན་ལུ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		གལ་སྲིད (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ལོག offset;
	}

	ལོག offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ཧྲིལ offset;

	གལ་སྲིད ((phandle == 0) || (phandle == ~0U))
		ལོག -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	དོན་ལུ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		གལ་སྲིད (fdt_get_phandle(fdt, offset) == phandle)
			ལོག offset;
	}

	ལོག offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_stringlist_contains(const var_na *strlist, ཧྲིལ listlen, const var_na *str)
{
	ཧྲིལ len = strlen(str);
	const var_na *p;

	སྐབས (listlen >= len) {
		གལ་སྲིད (memcmp(str, strlist, len+1) == 0)
			ལོག 1;
		p = memchr(strlist, '\0', listlen);
		གལ་སྲིད (!p)
			ལོག 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ལོག 0;
}

ཧྲིལ fdt_stringlist_count(const void *fdt, ཧྲིལ nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	ཧྲིལ length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་སྲིད (!list)
		ལོག length;

	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་སྲིད (list + length > end)
			ལོག -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ལོག count;
}

ཧྲིལ fdt_stringlist_search(const void *fdt, ཧྲིལ nodeoffset, const var_na *property,
			  const var_na *string)
{
	ཧྲིལ length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་སྲིད (!list)
		ལོག length;

	len = strlen(string) + 1;
	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་སྲིད (list + length > end)
			ལོག -FDT_ERR_BADVALUE;

		གལ་སྲིད (length == len && memcmp(list, string, length) == 0)
			ལོག idx;

		list += length;
		idx++;
	}

	ལོག -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, ཧྲིལ nodeoffset,
			       const var_na *property, ཧྲིལ idx,
			       ཧྲིལ *lenp)
{
	const var_na *list, *end;
	ཧྲིལ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་སྲིད (!list) {
		གལ་སྲིད (lenp)
			*lenp = length;

		ལོག NULL;
	}

	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་སྲིད (list + length > end) {
			གལ་སྲིད (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ལོག NULL;
		}

		གལ་སྲིད (idx == 0) {
			གལ་སྲིད (lenp)
				*lenp = length - 1;

			ལོག list;
		}

		list += length;
		idx--;
	}

	གལ་སྲིད (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ལོག NULL;
}

ཧྲིལ fdt_node_check_compatible(const void *fdt, ཧྲིལ nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	ཧྲིལ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	གལ་སྲིད (!prop)
		ལོག len;

	ལོག !fdt_stringlist_contains(prop, len, compatible);
}

ཧྲིལ fdt_node_offset_by_compatible(const void *fdt, ཧྲིལ startoffset,
				  const var_na *compatible)
{
	ཧྲིལ offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	དོན་ལུ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		གལ་སྲིད ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ལོག err;
		དེ་མིན གལ་སྲིད (err == 0)
			ལོག offset;
	}

	ལོག offset; /* error from fdt_next_node() */
}
