// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static puur_naa_nka fdt_nodename_eq_(const void *fdt, puur_naa_nka offset,
			    const var_na *s, puur_naa_nka len)
{
	puur_naa_nka olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ᰀᰧ (!p || olen < len)
		/* short match */
		ᰀᰧᰌ 0;

	ᰀᰧ (memcmp(p, s, len) != 0)
		ᰀᰧᰌ 0;

	ᰀᰧ (p[len] == '\0')
		ᰀᰧᰌ 1;
	ᰀᰧᰀ ᰀᰧ (!memchr(s, '@', len) && (p[len] == '@'))
		ᰀᰧᰌ 1;
	ᰀᰧᰀ
		ᰀᰧᰌ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ᰀᰧ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ᰀᰧ (lenp)
			*lenp = strlen(s);
		ᰀᰧᰌ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ᰀᰧ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ᰀᰧ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ᰀᰧ (fdt_magic(fdt) == FDT_MAGIC) {
		ᰀᰧ (stroffset < 0)
			goto fail;
		ᰀᰧ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ᰀᰧ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ᰀᰧ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ᰀᰧᰀ ᰀᰧ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ᰀᰧ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ᰀᰧ (sw_stroffset < len)
			len = sw_stroffset;
	} ᰀᰧᰀ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ᰀᰧ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ᰀᰧ (lenp)
		*lenp = n - s;
	ᰀᰧᰌ s;

fail:
	ᰀᰧ (lenp)
		*lenp = err;
	ᰀᰧᰌ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ᰀᰧᰌ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ᰀᰧᰌ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ᰀᰬ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ᰀᰧ (offset < 0) {
			ᰀᰧ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ᰀᰧᰌ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ᰀᰧ (value > max)
			max = value;
	}

	ᰀᰧ (phandle)
		*phandle = max;

	ᰀᰧᰌ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ᰀᰧ (err < 0)
		ᰀᰧᰌ err;

	ᰀᰧ (max == FDT_MAX_PHANDLE)
		ᰀᰧᰌ -FDT_ERR_NOPHANDLES;

	ᰀᰧ (phandle)
		*phandle = max + 1;

	ᰀᰧᰌ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ᰀᰧ (!can_assume(VALID_INPUT)) {
		ᰀᰧ (absoffset < fdt_off_mem_rsvmap(fdt))
			ᰀᰧᰌ NULL;
		ᰀᰧ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ᰀᰧᰌ NULL;
	}
	ᰀᰧᰌ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ᰀᰧ (!can_assume(VALID_INPUT) && !re)
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ᰀᰧᰌ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ᰀᰧ (fdt64_ld_(&re->size) == 0)
			ᰀᰧᰌ i;
	}
	ᰀᰧᰌ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ᰀᰧ (nextoffset >= 0)
				ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;
			ᰀᰧᰀ
				ᰀᰧᰌ nextoffset;

		pak_sa FDT_PROP:
			ᰀᰧᰌ offset;
		}
		offset = nextoffset;
	} ᰀᰬ (tag == FDT_NOP);

	ᰀᰧᰌ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ᰀᰧ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ᰀᰧᰌ offset;

	ᰀᰧ (depth < 0)
		ᰀᰧᰌ -FDT_ERR_NOTFOUND;
	ᰀᰧᰌ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ᰀᰧᰌ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ᰀᰧ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ᰀᰧ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ᰀᰧ (!p)
			ᰀᰧᰌ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ᰀᰬ (p < end) {
		const var_na *q;

		ᰀᰬ (*p == '/') {
			p++;
			ᰀᰧ (p == end)
				ᰀᰧᰌ offset;
		}
		q = memchr(p, '/', end - p);
		ᰀᰧ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ᰀᰧ (offset < 0)
			ᰀᰧᰌ offset;

		p = q;
	}

	ᰀᰧᰌ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ᰀᰧᰌ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ᰀᰧ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ᰀᰧ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ᰀᰧ (len)
		*len = strlen(nameptr);

	ᰀᰧᰌ nameptr;

 fail:
	ᰀᰧ (len)
		*len = err;
	ᰀᰧᰌ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ᰀᰧ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᰀᰧᰌ offset;

	ᰀᰧᰌ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ᰀᰧ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ᰀᰧᰌ offset;

	ᰀᰧᰌ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ᰀᰧ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ᰀᰧ (lenp)
			*lenp = err;
		ᰀᰧᰌ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ᰀᰧ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ᰀᰧᰌ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᰀᰧ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᰀᰧᰌ NULL;
	}

	ᰀᰧᰌ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            puur_naa_nka offset,
						            const var_na *name,
						            puur_naa_nka namelen,
							    puur_naa_nka *lenp,
							    puur_naa_nka *poffset)
{
	for (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ᰀᰧ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ᰀᰧ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ᰀᰧ (poffset)
				*poffset = offset;
			ᰀᰧᰌ prop;
		}
	}

	ᰀᰧ (lenp)
		*lenp = offset;
	ᰀᰧᰌ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᰀᰧ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᰀᰧᰌ NULL;
	}

	ᰀᰧᰌ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ᰀᰧᰌ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ᰀᰧ (!prop)
		ᰀᰧᰌ NULL;

	/* Handle realignment */
	ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᰀᰧᰌ prop->data + 4;
	ᰀᰧᰌ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ᰀᰧ (!prop)
		ᰀᰧᰌ NULL;
	ᰀᰧ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ᰀᰧ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ᰀᰧ (!name) {
				ᰀᰧ (lenp)
					*lenp = namelen;
				ᰀᰧᰌ NULL;
			}
			*namep = name;
		} ᰀᰧᰀ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᰀᰧᰌ prop->data + 4;
	ᰀᰧᰌ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ᰀᰧᰌ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ᰀᰧ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ᰀᰧ (!php || (len != parimaa_na(*php)))
			ᰀᰧᰌ 0;
	}

	ᰀᰧᰌ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ᰀᰧ (aliasoffset < 0)
		ᰀᰧᰌ NULL;

	ᰀᰧᰌ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ᰀᰧᰌ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ᰀᰧ (buflen < 2)
		ᰀᰧᰌ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᰀᰬ (pdepth > depth) {
			do {
				p--;
			} ᰀᰬ (buf[p-1] != '/');
			pdepth--;
		}

		ᰀᰧ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ᰀᰧ (!name)
				ᰀᰧᰌ namelen;
			ᰀᰧ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ᰀᰧ (offset == nodeoffset) {
			ᰀᰧ (pdepth < (depth + 1))
				ᰀᰧᰌ -FDT_ERR_NOSPACE;

			ᰀᰧ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ᰀᰧᰌ 0;
		}
	}

	ᰀᰧ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;
	ᰀᰧᰀ ᰀᰧ (offset == -FDT_ERR_BADOFFSET)
		ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;

	ᰀᰧᰌ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ᰀᰧ (supernodedepth < 0)
		ᰀᰧᰌ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᰀᰧ (depth == supernodedepth)
			supernodeoffset = offset;

		ᰀᰧ (offset == nodeoffset) {
			ᰀᰧ (nodedepth)
				*nodedepth = depth;

			ᰀᰧ (supernodedepth > depth)
				ᰀᰧᰌ -FDT_ERR_NOTFOUND;
			ᰀᰧᰀ
				ᰀᰧᰌ supernodeoffset;
		}
	}

	ᰀᰧ (!can_assume(VALID_INPUT)) {
		ᰀᰧ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ᰀᰧᰌ -FDT_ERR_BADOFFSET;
		ᰀᰧᰀ ᰀᰧ (offset == -FDT_ERR_BADOFFSET)
			ᰀᰧᰌ -FDT_ERR_BADSTRUCTURE;
	}

	ᰀᰧᰌ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ᰀᰧ (err)
		ᰀᰧᰌ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ᰀᰧᰌ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ᰀᰧ (nodedepth < 0)
		ᰀᰧᰌ nodedepth;
	ᰀᰧᰌ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

puur_naa_nka fdt_node_offset_by_prop_value(const void *fdt, puur_naa_nka startoffset,
				  const var_na *propname,
				  const void *propval, puur_naa_nka proplen)
{
	puur_naa_nka offset;
	const void *val;
	puur_naa_nka len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ᰀᰧ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ᰀᰧᰌ offset;
	}

	ᰀᰧᰌ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ᰀᰧ ((phandle == 0) || (phandle == ~0U))
		ᰀᰧᰌ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	for (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ᰀᰧ (fdt_get_phandle(fdt, offset) == phandle)
			ᰀᰧᰌ offset;
	}

	ᰀᰧᰌ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ᰀᰬ (listlen >= len) {
		ᰀᰧ (memcmp(str, strlist, len+1) == 0)
			ᰀᰧᰌ 1;
		p = memchr(strlist, '\0', listlen);
		ᰀᰧ (!p)
			ᰀᰧᰌ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ᰀᰧᰌ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᰀᰧ (!list)
		ᰀᰧᰌ length;

	end = list + length;

	ᰀᰬ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᰀᰧ (list + length > end)
			ᰀᰧᰌ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ᰀᰧᰌ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᰀᰧ (!list)
		ᰀᰧᰌ length;

	len = strlen(string) + 1;
	end = list + length;

	ᰀᰬ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᰀᰧ (list + length > end)
			ᰀᰧᰌ -FDT_ERR_BADVALUE;

		ᰀᰧ (length == len && memcmp(list, string, length) == 0)
			ᰀᰧᰌ idx;

		list += length;
		idx++;
	}

	ᰀᰧᰌ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᰀᰧ (!list) {
		ᰀᰧ (lenp)
			*lenp = length;

		ᰀᰧᰌ NULL;
	}

	end = list + length;

	ᰀᰬ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᰀᰧ (list + length > end) {
			ᰀᰧ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ᰀᰧᰌ NULL;
		}

		ᰀᰧ (idx == 0) {
			ᰀᰧ (lenp)
				*lenp = length - 1;

			ᰀᰧᰌ list;
		}

		list += length;
		idx--;
	}

	ᰀᰧ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ᰀᰧᰌ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ᰀᰧ (!prop)
		ᰀᰧᰌ len;

	ᰀᰧᰌ !fdt_stringlist_contains(prop, len, compatible);
}

puur_naa_nka fdt_node_offset_by_compatible(const void *fdt, puur_naa_nka startoffset,
				  const var_na *compatible)
{
	puur_naa_nka offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ᰀᰧ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ᰀᰧᰌ err;
		ᰀᰧᰀ ᰀᰧ (err == 0)
			ᰀᰧᰌ offset;
	}

	ᰀᰧᰌ offset; /* error from fdt_next_node() */
}
