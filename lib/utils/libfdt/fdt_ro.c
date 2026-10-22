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

	གལ་ཏེ (!p || olen < len)
		/* short match */
		ཕྱིར 0;

	གལ་ཏེ (memcmp(p, s, len) != 0)
		ཕྱིར 0;

	གལ་ཏེ (p[len] == '\0')
		ཕྱིར 1;
	དེ་མིན གལ་ཏེ (!memchr(s, '@', len) && (p[len] == '@'))
		ཕྱིར 1;
	དེ་མིན
		ཕྱིར 0;
}

const var_na *fdt_get_string(const void *fdt, ཧྲིལ stroffset, ཧྲིལ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ཧྲིལ err;
	const var_na *s, *n;

	གལ་ཏེ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		གལ་ཏེ (lenp)
			*lenp = strlen(s);
		ཕྱིར s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	གལ་ཏེ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	གལ་ཏེ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	གལ་ཏེ (fdt_magic(fdt) == FDT_MAGIC) {
		གལ་ཏེ (stroffset < 0)
			goto fail;
		གལ་ཏེ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			གལ་ཏེ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			གལ་ཏེ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} དེ་མིན གལ་ཏེ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ཧྲིལ sw_stroffset = -stroffset;

		གལ་ཏེ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		གལ་ཏེ (sw_stroffset < len)
			len = sw_stroffset;
	} དེ་མིན {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	གལ་ཏེ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	གལ་ཏེ (lenp)
		*lenp = n - s;
	ཕྱིར s;

fail:
	གལ་ཏེ (lenp)
		*lenp = err;
	ཕྱིར NULL;
}

const var_na *fdt_string(const void *fdt, ཧྲིལ stroffset)
{
	ཕྱིར fdt_get_string(fdt, stroffset, NULL);
}

static ཧྲིལ fdt_string_eq_(const void *fdt, ཧྲིལ stroffset,
			  const var_na *s, ཧྲིལ len)
{
	ཧྲིལ slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ཕྱིར p && (slen == len) && (memcmp(p, s, len) == 0);
}

ཧྲིལ fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ཧྲིལ offset = -1;

	སྐབས (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		གལ་ཏེ (offset < 0) {
			གལ་ཏེ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ཕྱིར offset;
		}

		value = fdt_get_phandle(fdt, offset);

		གལ་ཏེ (value > max)
			max = value;
	}

	གལ་ཏེ (phandle)
		*phandle = max;

	ཕྱིར 0;
}

ཧྲིལ fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ཧྲིལ err;

	err = fdt_find_max_phandle(fdt, &max);
	གལ་ཏེ (err < 0)
		ཕྱིར err;

	གལ་ཏེ (max == FDT_MAX_PHANDLE)
		ཕྱིར -FDT_ERR_NOPHANDLES;

	གལ་ཏེ (phandle)
		*phandle = max + 1;

	ཕྱིར 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ཧྲིལ n)
{
	unsigned ཧྲིལ offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned ཧྲིལ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	གལ་ཏེ (!can_assume(VALID_INPUT)) {
		གལ་ཏེ (absoffset < fdt_off_mem_rsvmap(fdt))
			ཕྱིར NULL;
		གལ་ཏེ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ཕྱིར NULL;
	}
	ཕྱིར fdt_mem_rsv_(fdt, n);
}

ཧྲིལ fdt_get_mem_rsv(const void *fdt, ཧྲིལ n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	གལ་ཏེ (!can_assume(VALID_INPUT) && !re)
		ཕྱིར -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ཕྱིར 0;
}

ཧྲིལ fdt_num_mem_rsv(const void *fdt)
{
	ཧྲིལ i;
	const sa_nracanaa fdt_reserve_entry *re;

	ཆེད་དུ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		གལ་ཏེ (fdt64_ld_(&re->size) == 0)
			ཕྱིར i;
	}
	ཕྱིར -FDT_ERR_TRUNCATED;
}

static ཧྲིལ nextprop_(const void *fdt, ཧྲིལ offset)
{
	uint32_t tag;
	ཧྲིལ nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			གལ་ཏེ (nextoffset >= 0)
				ཕྱིར -FDT_ERR_BADSTRUCTURE;
			དེ་མིན
				ཕྱིར nextoffset;

		pak_sa FDT_PROP:
			ཕྱིར offset;
		}
		offset = nextoffset;
	} སྐབས (tag == FDT_NOP);

	ཕྱིར -FDT_ERR_NOTFOUND;
}

ཧྲིལ fdt_subnode_offset_namelen(const void *fdt, ཧྲིལ offset,
			       const var_na *name, ཧྲིལ namelen)
{
	ཧྲིལ depth;

	FDT_RO_PROBE(fdt);

	ཆེད་དུ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		གལ་ཏེ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ཕྱིར offset;

	གལ་ཏེ (depth < 0)
		ཕྱིར -FDT_ERR_NOTFOUND;
	ཕྱིར offset; /* error */
}

ཧྲིལ fdt_subnode_offset(const void *fdt, ཧྲིལ parentoffset,
		       const var_na *name)
{
	ཕྱིར fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ཧྲིལ fdt_path_offset_namelen(const void *fdt, const var_na *path, ཧྲིལ namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	ཧྲིལ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	གལ་ཏེ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		གལ་ཏེ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		གལ་ཏེ (!p)
			ཕྱིར -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	སྐབས (p < end) {
		const var_na *q;

		སྐབས (*p == '/') {
			p++;
			གལ་ཏེ (p == end)
				ཕྱིར offset;
		}
		q = memchr(p, '/', end - p);
		གལ་ཏེ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		གལ་ཏེ (offset < 0)
			ཕྱིར offset;

		p = q;
	}

	ཕྱིར offset;
}

ཧྲིལ fdt_path_offset(const void *fdt, const var_na *path)
{
	ཕྱིར fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, ཧྲིལ nodeoffset, ཧྲིལ *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	ཧྲིལ err;

	གལ་ཏེ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		གལ་ཏེ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	གལ་ཏེ (len)
		*len = strlen(nameptr);

	ཕྱིར nameptr;

 fail:
	གལ་ཏེ (len)
		*len = err;
	ཕྱིར NULL;
}

ཧྲིལ fdt_first_property_offset(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ offset;

	གལ་ཏེ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ཕྱིར offset;

	ཕྱིར nextprop_(fdt, offset);
}

ཧྲིལ fdt_next_property_offset(const void *fdt, ཧྲིལ offset)
{
	གལ་ཏེ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ཕྱིར offset;

	ཕྱིར nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ཧྲིལ offset,
						              ཧྲིལ *lenp)
{
	ཧྲིལ err;
	const sa_nracanaa fdt_property *prop;

	གལ་ཏེ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		གལ་ཏེ (lenp)
			*lenp = err;
		ཕྱིར NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	གལ་ཏེ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ཕྱིར prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ཧྲིལ offset,
						      ཧྲིལ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		གལ་ཏེ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ཕྱིར NULL;
	}

	ཕྱིར fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ཧྲིལ offset,
						            const var_na *name,
						            ཧྲིལ namelen,
							    ཧྲིལ *lenp,
							    ཧྲིལ *poffset)
{
	ཆེད་དུ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		གལ་ཏེ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		གལ་ཏེ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			གལ་ཏེ (poffset)
				*poffset = offset;
			ཕྱིར prop;
		}
	}

	གལ་ཏེ (lenp)
		*lenp = offset;
	ཕྱིར NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    ཧྲིལ offset,
						    const var_na *name,
						    ཧྲིལ namelen, ཧྲིལ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		གལ་ཏེ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ཕྱིར NULL;
	}

	ཕྱིར fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    ཧྲིལ nodeoffset,
					    const var_na *name, ཧྲིལ *lenp)
{
	ཕྱིར fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ཧྲིལ nodeoffset,
				const var_na *name, ཧྲིལ namelen, ཧྲིལ *lenp)
{
	ཧྲིལ poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	གལ་ཏེ (!prop)
		ཕྱིར NULL;

	/* Handle realignment */
	གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ཕྱིར prop->data + 4;
	ཕྱིར prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ཧྲིལ offset,
				  const var_na **namep, ཧྲིལ *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	གལ་ཏེ (!prop)
		ཕྱིར NULL;
	གལ་ཏེ (namep) {
		const var_na *name;
		ཧྲིལ namelen;

		གལ་ཏེ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			གལ་ཏེ (!name) {
				གལ་ཏེ (lenp)
					*lenp = namelen;
				ཕྱིར NULL;
			}
			*namep = name;
		} དེ་མིན {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	གལ་ཏེ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ཕྱིར prop->data + 4;
	ཕྱིར prop->data;
}

const void *fdt_getprop(const void *fdt, ཧྲིལ nodeoffset,
			const var_na *name, ཧྲིལ *lenp)
{
	ཕྱིར fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ཧྲིལ nodeoffset)
{
	const fdt32_t *php;
	ཧྲིལ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	གལ་ཏེ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		གལ་ཏེ (!php || (len != parimaa_na(*php)))
			ཕྱིར 0;
	}

	ཕྱིར fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, ཧྲིལ namelen)
{
	ཧྲིལ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	གལ་ཏེ (aliasoffset < 0)
		ཕྱིར NULL;

	ཕྱིར fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ཕྱིར fdt_get_alias_namelen(fdt, name, strlen(name));
}

ཧྲིལ fdt_get_path(const void *fdt, ཧྲིལ nodeoffset, var_na *buf, ཧྲིལ buflen)
{
	ཧྲིལ pdepth = 0, p = 0;
	ཧྲིལ offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	གལ་ཏེ (buflen < 2)
		ཕྱིར -FDT_ERR_NOSPACE;

	ཆེད་དུ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		སྐབས (pdepth > depth) {
			do {
				p--;
			} སྐབས (buf[p-1] != '/');
			pdepth--;
		}

		གལ་ཏེ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			གལ་ཏེ (!name)
				ཕྱིར namelen;
			གལ་ཏེ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		གལ་ཏེ (offset == nodeoffset) {
			གལ་ཏེ (pdepth < (depth + 1))
				ཕྱིར -FDT_ERR_NOSPACE;

			གལ་ཏེ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ཕྱིར 0;
		}
	}

	གལ་ཏེ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ཕྱིར -FDT_ERR_BADOFFSET;
	དེ་མིན གལ་ཏེ (offset == -FDT_ERR_BADOFFSET)
		ཕྱིར -FDT_ERR_BADSTRUCTURE;

	ཕྱིར offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_supernode_atdepth_offset(const void *fdt, ཧྲིལ nodeoffset,
				 ཧྲིལ supernodedepth, ཧྲིལ *nodedepth)
{
	ཧྲིལ offset, depth;
	ཧྲིལ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	གལ་ཏེ (supernodedepth < 0)
		ཕྱིར -FDT_ERR_NOTFOUND;

	ཆེད་དུ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		གལ་ཏེ (depth == supernodedepth)
			supernodeoffset = offset;

		གལ་ཏེ (offset == nodeoffset) {
			གལ་ཏེ (nodedepth)
				*nodedepth = depth;

			གལ་ཏེ (supernodedepth > depth)
				ཕྱིར -FDT_ERR_NOTFOUND;
			དེ་མིན
				ཕྱིར supernodeoffset;
		}
	}

	གལ་ཏེ (!can_assume(VALID_INPUT)) {
		གལ་ཏེ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ཕྱིར -FDT_ERR_BADOFFSET;
		དེ་མིན གལ་ཏེ (offset == -FDT_ERR_BADOFFSET)
			ཕྱིར -FDT_ERR_BADSTRUCTURE;
	}

	ཕྱིར offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_node_depth(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ nodedepth;
	ཧྲིལ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	གལ་ཏེ (err)
		ཕྱིར (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ཕྱིར nodedepth;
}

ཧྲིལ fdt_parent_offset(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ nodedepth = fdt_node_depth(fdt, nodeoffset);

	གལ་ཏེ (nodedepth < 0)
		ཕྱིར nodedepth;
	ཕྱིར fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
	ཆེད་དུ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		གལ་ཏེ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ཕྱིར offset;
	}

	ཕྱིར offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ཧྲིལ offset;

	གལ་ཏེ ((phandle == 0) || (phandle == ~0U))
		ཕྱིར -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ཆེད་དུ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		གལ་ཏེ (fdt_get_phandle(fdt, offset) == phandle)
			ཕྱིར offset;
	}

	ཕྱིར offset; /* error from fdt_next_node() */
}

ཧྲིལ fdt_stringlist_contains(const var_na *strlist, ཧྲིལ listlen, const var_na *str)
{
	ཧྲིལ len = strlen(str);
	const var_na *p;

	སྐབས (listlen >= len) {
		གལ་ཏེ (memcmp(str, strlist, len+1) == 0)
			ཕྱིར 1;
		p = memchr(strlist, '\0', listlen);
		གལ་ཏེ (!p)
			ཕྱིར 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ཕྱིར 0;
}

ཧྲིལ fdt_stringlist_count(const void *fdt, ཧྲིལ nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	ཧྲིལ length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་ཏེ (!list)
		ཕྱིར length;

	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་ཏེ (list + length > end)
			ཕྱིར -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ཕྱིར count;
}

ཧྲིལ fdt_stringlist_search(const void *fdt, ཧྲིལ nodeoffset, const var_na *property,
			  const var_na *string)
{
	ཧྲིལ length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་ཏེ (!list)
		ཕྱིར length;

	len = strlen(string) + 1;
	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་ཏེ (list + length > end)
			ཕྱིར -FDT_ERR_BADVALUE;

		གལ་ཏེ (length == len && memcmp(list, string, length) == 0)
			ཕྱིར idx;

		list += length;
		idx++;
	}

	ཕྱིར -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, ཧྲིལ nodeoffset,
			       const var_na *property, ཧྲིལ idx,
			       ཧྲིལ *lenp)
{
	const var_na *list, *end;
	ཧྲིལ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	གལ་ཏེ (!list) {
		གལ་ཏེ (lenp)
			*lenp = length;

		ཕྱིར NULL;
	}

	end = list + length;

	སྐབས (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		གལ་ཏེ (list + length > end) {
			གལ་ཏེ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ཕྱིར NULL;
		}

		གལ་ཏེ (idx == 0) {
			གལ་ཏེ (lenp)
				*lenp = length - 1;

			ཕྱིར list;
		}

		list += length;
		idx--;
	}

	གལ་ཏེ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ཕྱིར NULL;
}

ཧྲིལ fdt_node_check_compatible(const void *fdt, ཧྲིལ nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	ཧྲིལ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	གལ་ཏེ (!prop)
		ཕྱིར len;

	ཕྱིར !fdt_stringlist_contains(prop, len, compatible);
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
	ཆེད་དུ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		གལ་ཏེ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ཕྱིར err;
		དེ་མིན གལ་ཏེ (err == 0)
			ཕྱིར offset;
	}

	ཕྱིར offset; /* error from fdt_next_node() */
}
