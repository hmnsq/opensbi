// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ຈຳນວນເຕັມ fdt_nodename_eq_(const void *fdt, ຈຳນວນເຕັມ offset,
			    const var_na *s, ຈຳນວນເຕັມ len)
{
	ຈຳນວນເຕັມ olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ຖ້າ (!p || olen < len)
		/* short match */
		ສົ່ງຄືນ 0;

	ຖ້າ (memcmp(p, s, len) != 0)
		ສົ່ງຄືນ 0;

	ຖ້າ (p[len] == '\0')
		ສົ່ງຄືນ 1;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (!memchr(s, '@', len) && (p[len] == '@'))
		ສົ່ງຄືນ 1;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ 0;
}

const var_na *fdt_get_string(const void *fdt, ຈຳນວນເຕັມ stroffset, ຈຳນວນເຕັມ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ຈຳນວນເຕັມ err;
	const var_na *s, *n;

	ຖ້າ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ຖ້າ (lenp)
			*lenp = strlen(s);
		ສົ່ງຄືນ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ຖ້າ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ຖ້າ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ຖ້າ (fdt_magic(fdt) == FDT_MAGIC) {
		ຖ້າ (stroffset < 0)
			goto fail;
		ຖ້າ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ຖ້າ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ຖ້າ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ຈຳນວນເຕັມ sw_stroffset = -stroffset;

		ຖ້າ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ຖ້າ (sw_stroffset < len)
			len = sw_stroffset;
	} ບໍ່ດັ່ງນັ້ນ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ຖ້າ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ຖ້າ (lenp)
		*lenp = n - s;
	ສົ່ງຄືນ s;

fail:
	ຖ້າ (lenp)
		*lenp = err;
	ສົ່ງຄືນ NULL;
}

const var_na *fdt_string(const void *fdt, ຈຳນວນເຕັມ stroffset)
{
	ສົ່ງຄືນ fdt_get_string(fdt, stroffset, NULL);
}

static ຈຳນວນເຕັມ fdt_string_eq_(const void *fdt, ຈຳນວນເຕັມ stroffset,
			  const var_na *s, ຈຳນວນເຕັມ len)
{
	ຈຳນວນເຕັມ slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ສົ່ງຄືນ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ຈຳນວນເຕັມ fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ຈຳນວນເຕັມ offset = -1;

	ໃນຂະນະທີ່ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ຖ້າ (offset < 0) {
			ຖ້າ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ສົ່ງຄືນ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ຖ້າ (value > max)
			max = value;
	}

	ຖ້າ (phandle)
		*phandle = max;

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ຈຳນວນເຕັມ err;

	err = fdt_find_max_phandle(fdt, &max);
	ຖ້າ (err < 0)
		ສົ່ງຄືນ err;

	ຖ້າ (max == FDT_MAX_PHANDLE)
		ສົ່ງຄືນ -FDT_ERR_NOPHANDLES;

	ຖ້າ (phandle)
		*phandle = max + 1;

	ສົ່ງຄືນ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ຈຳນວນເຕັມ n)
{
	unsigned ຈຳນວນເຕັມ offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned ຈຳນວນເຕັມ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ຖ້າ (!can_assume(VALID_INPUT)) {
		ຖ້າ (absoffset < fdt_off_mem_rsvmap(fdt))
			ສົ່ງຄືນ NULL;
		ຖ້າ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ສົ່ງຄືນ NULL;
	}
	ສົ່ງຄືນ fdt_mem_rsv_(fdt, n);
}

ຈຳນວນເຕັມ fdt_get_mem_rsv(const void *fdt, ຈຳນວນເຕັມ n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ຖ້າ (!can_assume(VALID_INPUT) && !re)
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_num_mem_rsv(const void *fdt)
{
	ຈຳນວນເຕັມ i;
	const sa_nracanaa fdt_reserve_entry *re;

	ສຳລັບ (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ຖ້າ (fdt64_ld_(&re->size) == 0)
			ສົ່ງຄືນ i;
	}
	ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
}

static ຈຳນວນເຕັມ nextprop_(const void *fdt, ຈຳນວນເຕັມ offset)
{
	uint32_t tag;
	ຈຳນວນເຕັມ nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ຖ້າ (nextoffset >= 0)
				ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ nextoffset;

		pak_sa FDT_PROP:
			ສົ່ງຄືນ offset;
		}
		offset = nextoffset;
	} ໃນຂະນະທີ່ (tag == FDT_NOP);

	ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
}

ຈຳນວນເຕັມ fdt_subnode_offset_namelen(const void *fdt, ຈຳນວນເຕັມ offset,
			       const var_na *name, ຈຳນວນເຕັມ namelen)
{
	ຈຳນວນເຕັມ depth;

	FDT_RO_PROBE(fdt);

	ສຳລັບ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ຖ້າ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ສົ່ງຄືນ offset;

	ຖ້າ (depth < 0)
		ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
	ສົ່ງຄືນ offset; /* error */
}

ຈຳນວນເຕັມ fdt_subnode_offset(const void *fdt, ຈຳນວນເຕັມ parentoffset,
		       const var_na *name)
{
	ສົ່ງຄືນ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ຈຳນວນເຕັມ fdt_path_offset_namelen(const void *fdt, const var_na *path, ຈຳນວນເຕັມ namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	ຈຳນວນເຕັມ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ຖ້າ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ຖ້າ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ຖ້າ (!p)
			ສົ່ງຄືນ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ໃນຂະນະທີ່ (p < end) {
		const var_na *q;

		ໃນຂະນະທີ່ (*p == '/') {
			p++;
			ຖ້າ (p == end)
				ສົ່ງຄືນ offset;
		}
		q = memchr(p, '/', end - p);
		ຖ້າ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ຖ້າ (offset < 0)
			ສົ່ງຄືນ offset;

		p = q;
	}

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_path_offset(const void *fdt, const var_na *path)
{
	ສົ່ງຄືນ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, ຈຳນວນເຕັມ nodeoffset, ຈຳນວນເຕັມ *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	ຈຳນວນເຕັມ err;

	ຖ້າ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ຖ້າ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ຖ້າ (len)
		*len = strlen(nameptr);

	ສົ່ງຄືນ nameptr;

 fail:
	ຖ້າ (len)
		*len = err;
	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ fdt_first_property_offset(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ offset;

	ຖ້າ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ສົ່ງຄືນ offset;

	ສົ່ງຄືນ nextprop_(fdt, offset);
}

ຈຳນວນເຕັມ fdt_next_property_offset(const void *fdt, ຈຳນວນເຕັມ offset)
{
	ຖ້າ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ສົ່ງຄືນ offset;

	ສົ່ງຄືນ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ຈຳນວນເຕັມ offset,
						              ຈຳນວນເຕັມ *lenp)
{
	ຈຳນວນເຕັມ err;
	const sa_nracanaa fdt_property *prop;

	ຖ້າ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ຖ້າ (lenp)
			*lenp = err;
		ສົ່ງຄືນ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ຖ້າ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ສົ່ງຄືນ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ຈຳນວນເຕັມ offset,
						      ຈຳນວນເຕັມ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ຖ້າ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ສົ່ງຄືນ NULL;
	}

	ສົ່ງຄືນ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ຈຳນວນເຕັມ offset,
						            const var_na *name,
						            ຈຳນວນເຕັມ namelen,
							    ຈຳນວນເຕັມ *lenp,
							    ຈຳນວນເຕັມ *poffset)
{
	ສຳລັບ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ຖ້າ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ຖ້າ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ຖ້າ (poffset)
				*poffset = offset;
			ສົ່ງຄືນ prop;
		}
	}

	ຖ້າ (lenp)
		*lenp = offset;
	ສົ່ງຄືນ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    ຈຳນວນເຕັມ offset,
						    const var_na *name,
						    ຈຳນວນເຕັມ namelen, ຈຳນວນເຕັມ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ຖ້າ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ສົ່ງຄືນ NULL;
	}

	ສົ່ງຄືນ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    ຈຳນວນເຕັມ nodeoffset,
					    const var_na *name, ຈຳນວນເຕັມ *lenp)
{
	ສົ່ງຄືນ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				const var_na *name, ຈຳນວນເຕັມ namelen, ຈຳນວນເຕັມ *lenp)
{
	ຈຳນວນເຕັມ poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ຖ້າ (!prop)
		ສົ່ງຄືນ NULL;

	/* Handle realignment */
	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ສົ່ງຄືນ prop->data + 4;
	ສົ່ງຄືນ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ຈຳນວນເຕັມ offset,
				  const var_na **namep, ຈຳນວນເຕັມ *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ຖ້າ (!prop)
		ສົ່ງຄືນ NULL;
	ຖ້າ (namep) {
		const var_na *name;
		ຈຳນວນເຕັມ namelen;

		ຖ້າ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ຖ້າ (!name) {
				ຖ້າ (lenp)
					*lenp = namelen;
				ສົ່ງຄືນ NULL;
			}
			*namep = name;
		} ບໍ່ດັ່ງນັ້ນ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ສົ່ງຄືນ prop->data + 4;
	ສົ່ງຄືນ prop->data;
}

const void *fdt_getprop(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			const var_na *name, ຈຳນວນເຕັມ *lenp)
{
	ສົ່ງຄືນ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	const fdt32_t *php;
	ຈຳນວນເຕັມ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ຖ້າ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ຖ້າ (!php || (len != parimaa_na(*php)))
			ສົ່ງຄືນ 0;
	}

	ສົ່ງຄືນ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, ຈຳນວນເຕັມ namelen)
{
	ຈຳນວນເຕັມ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ຖ້າ (aliasoffset < 0)
		ສົ່ງຄືນ NULL;

	ສົ່ງຄືນ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ສົ່ງຄືນ fdt_get_alias_namelen(fdt, name, strlen(name));
}

ຈຳນວນເຕັມ fdt_get_path(const void *fdt, ຈຳນວນເຕັມ nodeoffset, var_na *buf, ຈຳນວນເຕັມ buflen)
{
	ຈຳນວນເຕັມ pdepth = 0, p = 0;
	ຈຳນວນເຕັມ offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ຖ້າ (buflen < 2)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	ສຳລັບ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ໃນຂະນະທີ່ (pdepth > depth) {
			do {
				p--;
			} ໃນຂະນະທີ່ (buf[p-1] != '/');
			pdepth--;
		}

		ຖ້າ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ຖ້າ (!name)
				ສົ່ງຄືນ namelen;
			ຖ້າ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ຖ້າ (offset == nodeoffset) {
			ຖ້າ (pdepth < (depth + 1))
				ສົ່ງຄືນ -FDT_ERR_NOSPACE;

			ຖ້າ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ສົ່ງຄືນ 0;
		}
	}

	ຖ້າ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (offset == -FDT_ERR_BADOFFSET)
		ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;

	ສົ່ງຄືນ offset; /* error from fdt_next_node() */
}

ຈຳນວນເຕັມ fdt_supernode_atdepth_offset(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
				 ຈຳນວນເຕັມ supernodedepth, ຈຳນວນເຕັມ *nodedepth)
{
	ຈຳນວນເຕັມ offset, depth;
	ຈຳນວນເຕັມ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ຖ້າ (supernodedepth < 0)
		ສົ່ງຄືນ -FDT_ERR_NOTFOUND;

	ສຳລັບ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ຖ້າ (depth == supernodedepth)
			supernodeoffset = offset;

		ຖ້າ (offset == nodeoffset) {
			ຖ້າ (nodedepth)
				*nodedepth = depth;

			ຖ້າ (supernodedepth > depth)
				ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ supernodeoffset;
		}
	}

	ຖ້າ (!can_assume(VALID_INPUT)) {
		ຖ້າ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ສົ່ງຄືນ -FDT_ERR_BADOFFSET;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (offset == -FDT_ERR_BADOFFSET)
			ສົ່ງຄືນ -FDT_ERR_BADSTRUCTURE;
	}

	ສົ່ງຄືນ offset; /* error from fdt_next_node() */
}

ຈຳນວນເຕັມ fdt_node_depth(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ nodedepth;
	ຈຳນວນເຕັມ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ຖ້າ (err)
		ສົ່ງຄືນ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ສົ່ງຄືນ nodedepth;
}

ຈຳນວນເຕັມ fdt_parent_offset(const void *fdt, ຈຳນວນເຕັມ nodeoffset)
{
	ຈຳນວນເຕັມ nodedepth = fdt_node_depth(fdt, nodeoffset);

	ຖ້າ (nodedepth < 0)
		ສົ່ງຄືນ nodedepth;
	ສົ່ງຄືນ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

ຈຳນວນເຕັມ fdt_node_offset_by_prop_value(const void *fdt, ຈຳນວນເຕັມ startoffset,
				  const var_na *propname,
				  const void *propval, ຈຳນວນເຕັມ proplen)
{
	ຈຳນວນເຕັມ offset;
	const void *val;
	ຈຳນວນເຕັມ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	ສຳລັບ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ຖ້າ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ສົ່ງຄືນ offset;
	}

	ສົ່ງຄືນ offset; /* error from fdt_next_node() */
}

ຈຳນວນເຕັມ fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ຈຳນວນເຕັມ offset;

	ຖ້າ ((phandle == 0) || (phandle == ~0U))
		ສົ່ງຄືນ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ສຳລັບ (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ຖ້າ (fdt_get_phandle(fdt, offset) == phandle)
			ສົ່ງຄືນ offset;
	}

	ສົ່ງຄືນ offset; /* error from fdt_next_node() */
}

ຈຳນວນເຕັມ fdt_stringlist_contains(const var_na *strlist, ຈຳນວນເຕັມ listlen, const var_na *str)
{
	ຈຳນວນເຕັມ len = strlen(str);
	const var_na *p;

	ໃນຂະນະທີ່ (listlen >= len) {
		ຖ້າ (memcmp(str, strlist, len+1) == 0)
			ສົ່ງຄືນ 1;
		p = memchr(strlist, '\0', listlen);
		ຖ້າ (!p)
			ສົ່ງຄືນ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fdt_stringlist_count(const void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	ຈຳນວນເຕັມ length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ຖ້າ (!list)
		ສົ່ງຄືນ length;

	end = list + length;

	ໃນຂະນະທີ່ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ຖ້າ (list + length > end)
			ສົ່ງຄືນ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ສົ່ງຄືນ count;
}

ຈຳນວນເຕັມ fdt_stringlist_search(const void *fdt, ຈຳນວນເຕັມ nodeoffset, const var_na *property,
			  const var_na *string)
{
	ຈຳນວນເຕັມ length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ຖ້າ (!list)
		ສົ່ງຄືນ length;

	len = strlen(string) + 1;
	end = list + length;

	ໃນຂະນະທີ່ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ຖ້າ (list + length > end)
			ສົ່ງຄືນ -FDT_ERR_BADVALUE;

		ຖ້າ (length == len && memcmp(list, string, length) == 0)
			ສົ່ງຄືນ idx;

		list += length;
		idx++;
	}

	ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			       const var_na *property, ຈຳນວນເຕັມ idx,
			       ຈຳນວນເຕັມ *lenp)
{
	const var_na *list, *end;
	ຈຳນວນເຕັມ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ຖ້າ (!list) {
		ຖ້າ (lenp)
			*lenp = length;

		ສົ່ງຄືນ NULL;
	}

	end = list + length;

	ໃນຂະນະທີ່ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ຖ້າ (list + length > end) {
			ຖ້າ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ສົ່ງຄືນ NULL;
		}

		ຖ້າ (idx == 0) {
			ຖ້າ (lenp)
				*lenp = length - 1;

			ສົ່ງຄືນ list;
		}

		list += length;
		idx--;
	}

	ຖ້າ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ fdt_node_check_compatible(const void *fdt, ຈຳນວນເຕັມ nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	ຈຳນວນເຕັມ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ຖ້າ (!prop)
		ສົ່ງຄືນ len;

	ສົ່ງຄືນ !fdt_stringlist_contains(prop, len, compatible);
}

ຈຳນວນເຕັມ fdt_node_offset_by_compatible(const void *fdt, ຈຳນວນເຕັມ startoffset,
				  const var_na *compatible)
{
	ຈຳນວນເຕັມ offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	ສຳລັບ (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ຖ້າ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ສົ່ງຄືນ err;
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (err == 0)
			ສົ່ງຄືນ offset;
	}

	ສົ່ງຄືນ offset; /* error from fdt_next_node() */
}
