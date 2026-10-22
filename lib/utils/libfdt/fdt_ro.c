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

	ᱡᱩᱫᱤ (!p || olen < len)
		/* short match */
		ᱨᱩᱣᱟᱹᱲ 0;

	ᱡᱩᱫᱤ (memcmp(p, s, len) != 0)
		ᱨᱩᱣᱟᱹᱲ 0;

	ᱡᱩᱫᱤ (p[len] == '\0')
		ᱨᱩᱣᱟᱹᱲ 1;
	ᱵᱟᱝ ᱡᱩᱫᱤ (!memchr(s, '@', len) && (p[len] == '@'))
		ᱨᱩᱣᱟᱹᱲ 1;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ᱡᱩᱫᱤ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ᱡᱩᱫᱤ (lenp)
			*lenp = strlen(s);
		ᱨᱩᱣᱟᱹᱲ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ᱡᱩᱫᱤ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ᱡᱩᱫᱤ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ᱡᱩᱫᱤ (fdt_magic(fdt) == FDT_MAGIC) {
		ᱡᱩᱫᱤ (stroffset < 0)
			goto fail;
		ᱡᱩᱫᱤ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ᱡᱩᱫᱤ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ᱡᱩᱫᱤ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ᱵᱟᱝ ᱡᱩᱫᱤ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ᱡᱩᱫᱤ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ᱡᱩᱫᱤ (sw_stroffset < len)
			len = sw_stroffset;
	} ᱵᱟᱝ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ᱡᱩᱫᱤ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ᱡᱩᱫᱤ (lenp)
		*lenp = n - s;
	ᱨᱩᱣᱟᱹᱲ s;

fail:
	ᱡᱩᱫᱤ (lenp)
		*lenp = err;
	ᱨᱩᱣᱟᱹᱲ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ᱨᱩᱣᱟᱹᱲ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ᱨᱩᱣᱟᱹᱲ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ᱡᱚᱠᱷᱚᱱ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ᱡᱩᱫᱤ (offset < 0) {
			ᱡᱩᱫᱤ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ᱨᱩᱣᱟᱹᱲ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ᱡᱩᱫᱤ (value > max)
			max = value;
	}

	ᱡᱩᱫᱤ (phandle)
		*phandle = max;

	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ᱡᱩᱫᱤ (err < 0)
		ᱨᱩᱣᱟᱹᱲ err;

	ᱡᱩᱫᱤ (max == FDT_MAX_PHANDLE)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOPHANDLES;

	ᱡᱩᱫᱤ (phandle)
		*phandle = max + 1;

	ᱨᱩᱣᱟᱹᱲ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT)) {
		ᱡᱩᱫᱤ (absoffset < fdt_off_mem_rsvmap(fdt))
			ᱨᱩᱣᱟᱹᱲ NULL;
		ᱡᱩᱫᱤ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ᱨᱩᱣᱟᱹᱲ NULL;
	}
	ᱨᱩᱣᱟᱹᱲ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT) && !re)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ᱡᱩᱫᱤ (fdt64_ld_(&re->size) == 0)
			ᱨᱩᱣᱟᱹᱲ i;
	}
	ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ᱡᱩᱫᱤ (nextoffset >= 0)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;
			ᱵᱟᱝ
				ᱨᱩᱣᱟᱹᱲ nextoffset;

		pak_sa FDT_PROP:
			ᱨᱩᱣᱟᱹᱲ offset;
		}
		offset = nextoffset;
	} ᱡᱚᱠᱷᱚᱱ (tag == FDT_NOP);

	ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ᱡᱩᱫᱤ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ᱨᱩᱣᱟᱹᱲ offset;

	ᱡᱩᱫᱤ (depth < 0)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
	ᱨᱩᱣᱟᱹᱲ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ᱨᱩᱣᱟᱹᱲ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ᱡᱩᱫᱤ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ᱡᱩᱫᱤ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ᱡᱩᱫᱤ (!p)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ᱡᱚᱠᱷᱚᱱ (p < end) {
		const var_na *q;

		ᱡᱚᱠᱷᱚᱱ (*p == '/') {
			p++;
			ᱡᱩᱫᱤ (p == end)
				ᱨᱩᱣᱟᱹᱲ offset;
		}
		q = memchr(p, '/', end - p);
		ᱡᱩᱫᱤ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ᱡᱩᱫᱤ (offset < 0)
			ᱨᱩᱣᱟᱹᱲ offset;

		p = q;
	}

	ᱨᱩᱣᱟᱹᱲ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ᱨᱩᱣᱟᱹᱲ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ᱡᱩᱫᱤ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ᱡᱩᱫᱤ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ᱡᱩᱫᱤ (len)
		*len = strlen(nameptr);

	ᱨᱩᱣᱟᱹᱲ nameptr;

 fail:
	ᱡᱩᱫᱤ (len)
		*len = err;
	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ᱡᱩᱫᱤ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᱨᱩᱣᱟᱹᱲ offset;

	ᱨᱩᱣᱟᱹᱲ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ᱡᱩᱫᱤ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ᱨᱩᱣᱟᱹᱲ offset;

	ᱨᱩᱣᱟᱹᱲ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ᱡᱩᱫᱤ (lenp)
			*lenp = err;
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ᱡᱩᱫᱤ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ᱨᱩᱣᱟᱹᱲ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᱡᱩᱫᱤ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱨᱩᱣᱟᱹᱲ fdt_get_property_by_offset_(fdt, offset, lenp);
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
		ᱡᱩᱫᱤ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ᱡᱩᱫᱤ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ᱡᱩᱫᱤ (poffset)
				*poffset = offset;
			ᱨᱩᱣᱟᱹᱲ prop;
		}
	}

	ᱡᱩᱫᱤ (lenp)
		*lenp = offset;
	ᱨᱩᱣᱟᱹᱲ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᱡᱩᱫᱤ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	ᱨᱩᱣᱟᱹᱲ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ᱨᱩᱣᱟᱹᱲ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ᱡᱩᱫᱤ (!prop)
		ᱨᱩᱣᱟᱹᱲ NULL;

	/* Handle realignment */
	ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᱨᱩᱣᱟᱹᱲ prop->data + 4;
	ᱨᱩᱣᱟᱹᱲ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ᱡᱩᱫᱤ (!prop)
		ᱨᱩᱣᱟᱹᱲ NULL;
	ᱡᱩᱫᱤ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ᱡᱩᱫᱤ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ᱡᱩᱫᱤ (!name) {
				ᱡᱩᱫᱤ (lenp)
					*lenp = namelen;
				ᱨᱩᱣᱟᱹᱲ NULL;
			}
			*namep = name;
		} ᱵᱟᱝ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᱨᱩᱣᱟᱹᱲ prop->data + 4;
	ᱨᱩᱣᱟᱹᱲ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ᱨᱩᱣᱟᱹᱲ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ᱡᱩᱫᱤ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ᱡᱩᱫᱤ (!php || (len != parimaa_na(*php)))
			ᱨᱩᱣᱟᱹᱲ 0;
	}

	ᱨᱩᱣᱟᱹᱲ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ᱡᱩᱫᱤ (aliasoffset < 0)
		ᱨᱩᱣᱟᱹᱲ NULL;

	ᱨᱩᱣᱟᱹᱲ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ᱨᱩᱣᱟᱹᱲ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ᱡᱩᱫᱤ (buflen < 2)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᱡᱚᱠᱷᱚᱱ (pdepth > depth) {
			do {
				p--;
			} ᱡᱚᱠᱷᱚᱱ (buf[p-1] != '/');
			pdepth--;
		}

		ᱡᱩᱫᱤ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ᱡᱩᱫᱤ (!name)
				ᱨᱩᱣᱟᱹᱲ namelen;
			ᱡᱩᱫᱤ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ᱡᱩᱫᱤ (offset == nodeoffset) {
			ᱡᱩᱫᱤ (pdepth < (depth + 1))
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

			ᱡᱩᱫᱤ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ᱨᱩᱣᱟᱹᱲ 0;
		}
	}

	ᱡᱩᱫᱤ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;
	ᱵᱟᱝ ᱡᱩᱫᱤ (offset == -FDT_ERR_BADOFFSET)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;

	ᱨᱩᱣᱟᱹᱲ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ᱡᱩᱫᱤ (supernodedepth < 0)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᱡᱩᱫᱤ (depth == supernodedepth)
			supernodeoffset = offset;

		ᱡᱩᱫᱤ (offset == nodeoffset) {
			ᱡᱩᱫᱤ (nodedepth)
				*nodedepth = depth;

			ᱡᱩᱫᱤ (supernodedepth > depth)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
			ᱵᱟᱝ
				ᱨᱩᱣᱟᱹᱲ supernodeoffset;
		}
	}

	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT)) {
		ᱡᱩᱫᱤ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;
		ᱵᱟᱝ ᱡᱩᱫᱤ (offset == -FDT_ERR_BADOFFSET)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTRUCTURE;
	}

	ᱨᱩᱣᱟᱹᱲ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ᱡᱩᱫᱤ (err)
		ᱨᱩᱣᱟᱹᱲ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ᱨᱩᱣᱟᱹᱲ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ᱡᱩᱫᱤ (nodedepth < 0)
		ᱨᱩᱣᱟᱹᱲ nodedepth;
	ᱨᱩᱣᱟᱹᱲ fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
		ᱡᱩᱫᱤ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ᱨᱩᱣᱟᱹᱲ offset;
	}

	ᱨᱩᱣᱟᱹᱲ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ᱡᱩᱫᱤ ((phandle == 0) || (phandle == ~0U))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADPHANDLE;

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
		ᱡᱩᱫᱤ (fdt_get_phandle(fdt, offset) == phandle)
			ᱨᱩᱣᱟᱹᱲ offset;
	}

	ᱨᱩᱣᱟᱹᱲ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ᱡᱚᱠᱷᱚᱱ (listlen >= len) {
		ᱡᱩᱫᱤ (memcmp(str, strlist, len+1) == 0)
			ᱨᱩᱣᱟᱹᱲ 1;
		p = memchr(strlist, '\0', listlen);
		ᱡᱩᱫᱤ (!p)
			ᱨᱩᱣᱟᱹᱲ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᱡᱩᱫᱤ (!list)
		ᱨᱩᱣᱟᱹᱲ length;

	end = list + length;

	ᱡᱚᱠᱷᱚᱱ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᱡᱩᱫᱤ (list + length > end)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ᱨᱩᱣᱟᱹᱲ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᱡᱩᱫᱤ (!list)
		ᱨᱩᱣᱟᱹᱲ length;

	len = strlen(string) + 1;
	end = list + length;

	ᱡᱚᱠᱷᱚᱱ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᱡᱩᱫᱤ (list + length > end)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVALUE;

		ᱡᱩᱫᱤ (length == len && memcmp(list, string, length) == 0)
			ᱨᱩᱣᱟᱹᱲ idx;

		list += length;
		idx++;
	}

	ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᱡᱩᱫᱤ (!list) {
		ᱡᱩᱫᱤ (lenp)
			*lenp = length;

		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	end = list + length;

	ᱡᱚᱠᱷᱚᱱ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᱡᱩᱫᱤ (list + length > end) {
			ᱡᱩᱫᱤ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ᱨᱩᱣᱟᱹᱲ NULL;
		}

		ᱡᱩᱫᱤ (idx == 0) {
			ᱡᱩᱫᱤ (lenp)
				*lenp = length - 1;

			ᱨᱩᱣᱟᱹᱲ list;
		}

		list += length;
		idx--;
	}

	ᱡᱩᱫᱤ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ᱡᱩᱫᱤ (!prop)
		ᱨᱩᱣᱟᱹᱲ len;

	ᱨᱩᱣᱟᱹᱲ !fdt_stringlist_contains(prop, len, compatible);
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
		ᱡᱩᱫᱤ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ᱨᱩᱣᱟᱹᱲ err;
		ᱵᱟᱝ ᱡᱩᱫᱤ (err == 0)
			ᱨᱩᱣᱟᱹᱲ offset;
	}

	ᱨᱩᱣᱟᱹᱲ offset; /* error from fdt_next_node() */
}
