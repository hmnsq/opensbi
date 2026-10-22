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

	ᤀᤡ᤺ (!p || olen < len)
		/* short match */
		ᤀᤡᤴ 0;

	ᤀᤡ᤺ (memcmp(p, s, len) != 0)
		ᤀᤡᤴ 0;

	ᤀᤡ᤺ (p[len] == '\0')
		ᤀᤡᤴ 1;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!memchr(s, '@', len) && (p[len] == '@'))
		ᤀᤡᤴ 1;
	ᤀᤡ᤺ᤆᤡ
		ᤀᤡᤴ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ᤀᤡ᤺ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ᤀᤡ᤺ (lenp)
			*lenp = strlen(s);
		ᤀᤡᤴ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ᤀᤡ᤺ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ᤀᤡ᤺ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ᤀᤡ᤺ (fdt_magic(fdt) == FDT_MAGIC) {
		ᤀᤡ᤺ (stroffset < 0)
			goto fail;
		ᤀᤡ᤺ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ᤀᤡ᤺ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ᤀᤡ᤺ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ᤀᤡ᤺ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ᤀᤡ᤺ (sw_stroffset < len)
			len = sw_stroffset;
	} ᤀᤡ᤺ᤆᤡ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ᤀᤡ᤺ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ᤀᤡ᤺ (lenp)
		*lenp = n - s;
	ᤀᤡᤴ s;

fail:
	ᤀᤡ᤺ (lenp)
		*lenp = err;
	ᤀᤡᤴ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ᤀᤡᤴ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ᤀᤡᤴ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ᤀᤠ᤺ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ᤀᤡ᤺ (offset < 0) {
			ᤀᤡ᤺ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ᤀᤡᤴ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ᤀᤡ᤺ (value > max)
			max = value;
	}

	ᤀᤡ᤺ (phandle)
		*phandle = max;

	ᤀᤡᤴ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ᤀᤡ᤺ (err < 0)
		ᤀᤡᤴ err;

	ᤀᤡ᤺ (max == FDT_MAX_PHANDLE)
		ᤀᤡᤴ -FDT_ERR_NOPHANDLES;

	ᤀᤡ᤺ (phandle)
		*phandle = max + 1;

	ᤀᤡᤴ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ᤀᤡ᤺ (!can_assume(VALID_INPUT)) {
		ᤀᤡ᤺ (absoffset < fdt_off_mem_rsvmap(fdt))
			ᤀᤡᤴ NULL;
		ᤀᤡ᤺ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ᤀᤡᤴ NULL;
	}
	ᤀᤡᤴ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ᤀᤡ᤺ (!can_assume(VALID_INPUT) && !re)
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ᤀᤡᤴ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ᤀᤡ᤺ (fdt64_ld_(&re->size) == 0)
			ᤀᤡᤴ i;
	}
	ᤀᤡᤴ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ᤀᤡ᤺ (nextoffset >= 0)
				ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;
			ᤀᤡ᤺ᤆᤡ
				ᤀᤡᤴ nextoffset;

		pak_sa FDT_PROP:
			ᤀᤡᤴ offset;
		}
		offset = nextoffset;
	} ᤀᤠ᤺ (tag == FDT_NOP);

	ᤀᤡᤴ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ᤀᤡ᤺ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ᤀᤡᤴ offset;

	ᤀᤡ᤺ (depth < 0)
		ᤀᤡᤴ -FDT_ERR_NOTFOUND;
	ᤀᤡᤴ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ᤀᤡᤴ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ᤀᤡ᤺ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ᤀᤡ᤺ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ᤀᤡ᤺ (!p)
			ᤀᤡᤴ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ᤀᤠ᤺ (p < end) {
		const var_na *q;

		ᤀᤠ᤺ (*p == '/') {
			p++;
			ᤀᤡ᤺ (p == end)
				ᤀᤡᤴ offset;
		}
		q = memchr(p, '/', end - p);
		ᤀᤡ᤺ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ᤀᤡ᤺ (offset < 0)
			ᤀᤡᤴ offset;

		p = q;
	}

	ᤀᤡᤴ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ᤀᤡᤴ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ᤀᤡ᤺ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ᤀᤡ᤺ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ᤀᤡ᤺ (len)
		*len = strlen(nameptr);

	ᤀᤡᤴ nameptr;

 fail:
	ᤀᤡ᤺ (len)
		*len = err;
	ᤀᤡᤴ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ᤀᤡ᤺ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᤀᤡᤴ offset;

	ᤀᤡᤴ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ᤀᤡ᤺ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ᤀᤡᤴ offset;

	ᤀᤡᤴ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ᤀᤡ᤺ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ᤀᤡ᤺ (lenp)
			*lenp = err;
		ᤀᤡᤴ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ᤀᤡ᤺ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ᤀᤡᤴ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᤀᤡ᤺ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᤀᤡᤴ NULL;
	}

	ᤀᤡᤴ fdt_get_property_by_offset_(fdt, offset, lenp);
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
		ᤀᤡ᤺ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ᤀᤡ᤺ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ᤀᤡ᤺ (poffset)
				*poffset = offset;
			ᤀᤡᤴ prop;
		}
	}

	ᤀᤡ᤺ (lenp)
		*lenp = offset;
	ᤀᤡᤴ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᤀᤡ᤺ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᤀᤡᤴ NULL;
	}

	ᤀᤡᤴ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ᤀᤡᤴ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ᤀᤡ᤺ (!prop)
		ᤀᤡᤴ NULL;

	/* Handle realignment */
	ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᤀᤡᤴ prop->data + 4;
	ᤀᤡᤴ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ᤀᤡ᤺ (!prop)
		ᤀᤡᤴ NULL;
	ᤀᤡ᤺ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ᤀᤡ᤺ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ᤀᤡ᤺ (!name) {
				ᤀᤡ᤺ (lenp)
					*lenp = namelen;
				ᤀᤡᤴ NULL;
			}
			*namep = name;
		} ᤀᤡ᤺ᤆᤡ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᤀᤡᤴ prop->data + 4;
	ᤀᤡᤴ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ᤀᤡᤴ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ᤀᤡ᤺ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ᤀᤡ᤺ (!php || (len != parimaa_na(*php)))
			ᤀᤡᤴ 0;
	}

	ᤀᤡᤴ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ᤀᤡ᤺ (aliasoffset < 0)
		ᤀᤡᤴ NULL;

	ᤀᤡᤴ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ᤀᤡᤴ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ᤀᤡ᤺ (buflen < 2)
		ᤀᤡᤴ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᤀᤠ᤺ (pdepth > depth) {
			do {
				p--;
			} ᤀᤠ᤺ (buf[p-1] != '/');
			pdepth--;
		}

		ᤀᤡ᤺ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ᤀᤡ᤺ (!name)
				ᤀᤡᤴ namelen;
			ᤀᤡ᤺ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ᤀᤡ᤺ (offset == nodeoffset) {
			ᤀᤡ᤺ (pdepth < (depth + 1))
				ᤀᤡᤴ -FDT_ERR_NOSPACE;

			ᤀᤡ᤺ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ᤀᤡᤴ 0;
		}
	}

	ᤀᤡ᤺ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (offset == -FDT_ERR_BADOFFSET)
		ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;

	ᤀᤡᤴ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ᤀᤡ᤺ (supernodedepth < 0)
		ᤀᤡᤴ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᤀᤡ᤺ (depth == supernodedepth)
			supernodeoffset = offset;

		ᤀᤡ᤺ (offset == nodeoffset) {
			ᤀᤡ᤺ (nodedepth)
				*nodedepth = depth;

			ᤀᤡ᤺ (supernodedepth > depth)
				ᤀᤡᤴ -FDT_ERR_NOTFOUND;
			ᤀᤡ᤺ᤆᤡ
				ᤀᤡᤴ supernodeoffset;
		}
	}

	ᤀᤡ᤺ (!can_assume(VALID_INPUT)) {
		ᤀᤡ᤺ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ᤀᤡᤴ -FDT_ERR_BADOFFSET;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (offset == -FDT_ERR_BADOFFSET)
			ᤀᤡᤴ -FDT_ERR_BADSTRUCTURE;
	}

	ᤀᤡᤴ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ᤀᤡ᤺ (err)
		ᤀᤡᤴ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ᤀᤡᤴ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ᤀᤡ᤺ (nodedepth < 0)
		ᤀᤡᤴ nodedepth;
	ᤀᤡᤴ fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
		ᤀᤡ᤺ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ᤀᤡᤴ offset;
	}

	ᤀᤡᤴ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ᤀᤡ᤺ ((phandle == 0) || (phandle == ~0U))
		ᤀᤡᤴ -FDT_ERR_BADPHANDLE;

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
		ᤀᤡ᤺ (fdt_get_phandle(fdt, offset) == phandle)
			ᤀᤡᤴ offset;
	}

	ᤀᤡᤴ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ᤀᤠ᤺ (listlen >= len) {
		ᤀᤡ᤺ (memcmp(str, strlist, len+1) == 0)
			ᤀᤡᤴ 1;
		p = memchr(strlist, '\0', listlen);
		ᤀᤡ᤺ (!p)
			ᤀᤡᤴ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ᤀᤡᤴ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᤀᤡ᤺ (!list)
		ᤀᤡᤴ length;

	end = list + length;

	ᤀᤠ᤺ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᤀᤡ᤺ (list + length > end)
			ᤀᤡᤴ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ᤀᤡᤴ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᤀᤡ᤺ (!list)
		ᤀᤡᤴ length;

	len = strlen(string) + 1;
	end = list + length;

	ᤀᤠ᤺ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᤀᤡ᤺ (list + length > end)
			ᤀᤡᤴ -FDT_ERR_BADVALUE;

		ᤀᤡ᤺ (length == len && memcmp(list, string, length) == 0)
			ᤀᤡᤴ idx;

		list += length;
		idx++;
	}

	ᤀᤡᤴ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᤀᤡ᤺ (!list) {
		ᤀᤡ᤺ (lenp)
			*lenp = length;

		ᤀᤡᤴ NULL;
	}

	end = list + length;

	ᤀᤠ᤺ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᤀᤡ᤺ (list + length > end) {
			ᤀᤡ᤺ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ᤀᤡᤴ NULL;
		}

		ᤀᤡ᤺ (idx == 0) {
			ᤀᤡ᤺ (lenp)
				*lenp = length - 1;

			ᤀᤡᤴ list;
		}

		list += length;
		idx--;
	}

	ᤀᤡ᤺ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ᤀᤡᤴ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ᤀᤡ᤺ (!prop)
		ᤀᤡᤴ len;

	ᤀᤡᤴ !fdt_stringlist_contains(prop, len, compatible);
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
		ᤀᤡ᤺ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ᤀᤡᤴ err;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (err == 0)
			ᤀᤡᤴ offset;
	}

	ᤀᤡᤴ offset; /* error from fdt_next_node() */
}
