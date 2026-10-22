// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

/**
 * overlay_get_target_phandle - retrieves the target phandle of a fragment
 * @fdto: pointer to the device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 *
 * overlay_get_target_phandle() retrieves the target phandle of an
 * overlay fragment when that fragment uses a phandle (target
 * property) instead of a path (target-path property).
 *
 * returns:
 *      the phandle pointed by the target property
 *      0, if the phandle was not found
 *	-1, if the phandle was malformed
 */
நிலையான uint32_t overlay_get_target_phandle(மாறிலி வெறுமை *fdto, முழு fragment)
{
	மாறிலி fdt32_t *val;
	முழு len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	எனில் (!val)
		திரும்பு 0;

	எனில் ((len != அளவு(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		திரும்பு (uint32_t)-1;

	திரும்பு fdt32_to_cpu(*val);
}

/**
 * overlay_get_target - retrieves the offset of a fragment's target
 * @fdt: Base device tree blob
 * @fdto: Device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 * @pathp: pointer which receives the path of the target (or NULL)
 *
 * overlay_get_target() retrieves the target offset in the base
 * device tree of a fragment, no matter how the actual targeting is
 * done (through a phandle or a path)
 *
 * returns:
 *      the targeted node offset in the base device tree
 *      Negative error code on error
 */
நிலையான முழு overlay_get_target(மாறிலி வெறுமை *fdt, மாறிலி வெறுமை *fdto,
			      முழு fragment, எழுத்து மாறிலி **pathp)
{
	uint32_t phandle;
	மாறிலி எழுத்து *path = வெற்று;
	முழு path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	எனில் (phandle == (uint32_t)-1)
		திரும்பு -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	எனில் (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		எனில் (path)
			ret = fdt_path_offset(fdt, path);
		இல்லையெனில்
			ret = path_len;
	} இல்லையெனில்
		ret = fdt_முனை_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	எனில் (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	எனில் (ret < 0)
		திரும்பு ret;

	/* return pointer to path (if available) */
	எனில் (pathp)
		*pathp = path ? path : வெற்று;

	திரும்பு ret;
}

/**
 * overlay_phandle_add_offset - Increases a phandle by an offset
 * @fdt: Base device tree blob
 * @node: Device tree overlay blob
 * @name: Name of the property to modify (phandle or linux,phandle)
 * @delta: offset to apply
 *
 * overlay_phandle_add_offset() increments a node phandle by a given
 * offset.
 *
 * returns:
 *      0 on success.
 *      Negative error code on error
 */
நிலையான முழு overlay_phandle_add_offset(வெறுமை *fdt, முழு முனை,
				      மாறிலி எழுத்து *name, uint32_t delta)
{
	மாறிலி fdt32_t *val;
	uint32_t adj_val;
	முழு len;

	val = fdt_getprop(fdt, முனை, name, &len);
	எனில் (!val)
		திரும்பு len;

	எனில் (len != அளவு(*val))
		திரும்பு -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	எனில் ((adj_val + delta) < adj_val)
		திரும்பு -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	எனில் (adj_val == (uint32_t)-1)
		திரும்பு -FDT_ERR_NOPHANDLES;

	திரும்பு fdt_setprop_inplace_u32(fdt, முனை, name, adj_val);
}

/**
 * overlay_adjust_node_phandles - Offsets the phandles of a node
 * @fdto: Device tree overlay blob
 * @node: Offset of the node we want to adjust
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_node_phandles() adds a constant to all the phandles
 * of a given node. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_adjust_முனை_phandles(வெறுமை *fdto, முழு முனை,
					uint32_t delta)
{
	முழு child;
	முழு ret;

	ret = overlay_phandle_add_offset(fdto, முனை, "phandle", delta);
	எனில் (ret && ret != -FDT_ERR_NOTFOUND)
		திரும்பு ret;

	ret = overlay_phandle_add_offset(fdto, முனை, "linux,phandle", delta);
	எனில் (ret && ret != -FDT_ERR_NOTFOUND)
		திரும்பு ret;

	fdt_for_each_subnode(child, fdto, முனை) {
		ret = overlay_adjust_முனை_phandles(fdto, child, delta);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

/**
 * overlay_adjust_local_phandles - Adjust the phandles of a whole overlay
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_local_phandles() adds a constant to all the
 * phandles of an overlay. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_adjust_local_phandles(வெறுமை *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	திரும்பு overlay_adjust_முனை_phandles(fdto, 0, delta);
}

/**
 * overlay_update_local_node_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @tree_node: Node offset of the node to operate on
 * @fixup_node: Node offset of the matching local fixups node
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_nodes_references() update the phandles
 * pointing to a node within the device tree overlay by adding a
 * constant delta.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_update_local_முனை_references(வெறுமை *fdto,
						முழு மர_முனை,
						முழு fixup_முனை,
						uint32_t delta)
{
	முழு fixup_prop;
	முழு fixup_child;
	முழு ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_முனை) {
		மாறிலி fdt32_t *fixup_val;
		மாறிலி எழுத்து *மரம்_val;
		மாறிலி எழுத்து *name;
		முழு fixup_len;
		முழு மரம்_len;
		முழு i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		எனில் (!fixup_val)
			திரும்பு fixup_len;

		எனில் (fixup_len % அளவு(uint32_t))
			திரும்பு -FDT_ERR_BADOVERLAY;
		fixup_len /= அளவு(uint32_t);

		மரம்_val = fdt_getprop(fdto, மர_முனை, name, &மரம்_len);
		எனில் (!மரம்_val) {
			எனில் (மரம்_len == -FDT_ERR_NOTFOUND)
				திரும்பு -FDT_ERR_BADOVERLAY;

			திரும்பு மரம்_len;
		}

		க்கு (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, மரம்_val + poffset, அளவு(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  மர_முனை,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  அளவு(adj_val));
			எனில் (ret == -FDT_ERR_NOSPACE)
				திரும்பு -FDT_ERR_BADOVERLAY;

			எனில் (ret)
				திரும்பு ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_முனை) {
		மாறிலி எழுத்து *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    வெற்று);
		முழு மரம்_child;

		மரம்_child = fdt_subnode_offset(fdto, மர_முனை,
						fixup_child_name);
		எனில் (மரம்_child == -FDT_ERR_NOTFOUND)
			திரும்பு -FDT_ERR_BADOVERLAY;
		எனில் (மரம்_child < 0)
			திரும்பு மரம்_child;

		ret = overlay_update_local_முனை_references(fdto,
							   மரம்_child,
							   fixup_child,
							   delta);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

/**
 * overlay_update_local_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_references() update all the phandles pointing
 * to a node within the device tree overlay by adding a constant
 * delta to not conflict with the base overlay.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_update_local_references(வெறுமை *fdto, uint32_t delta)
{
	முழு fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	எனில் (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		எனில் (fixups == -FDT_ERR_NOTFOUND)
			திரும்பு 0;

		திரும்பு fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	திரும்பு overlay_update_local_முனை_references(fdto, 0, fixups,
						    delta);
}

/**
 * overlay_fixup_one_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @path: Path to a node holding a phandle in the overlay
 * @path_len: number of path characters to consider
 * @name: Name of the property holding the phandle reference in the overlay
 * @name_len: number of name characters to consider
 * @poffset: Offset within the overlay property where the phandle is stored
 * @label: Label of the node referenced by the phandle
 *
 * overlay_fixup_one_phandle() resolves an overlay phandle pointing to
 * a node in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_fixup_one_phandle(வெறுமை *fdt, வெறுமை *fdto,
				     முழு symbols_off,
				     மாறிலி எழுத்து *path, uint32_t path_len,
				     மாறிலி எழுத்து *name, uint32_t name_len,
				     முழு poffset, மாறிலி எழுத்து *label)
{
	மாறிலி எழுத்து *குறியீடு_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	முழு குறியீடு_off, fixup_off;
	முழு prop_len;

	எனில் (symbols_off < 0)
		திரும்பு symbols_off;

	குறியீடு_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	எனில் (!குறியீடு_path)
		திரும்பு prop_len;

	குறியீடு_off = fdt_path_offset(fdt, குறியீடு_path);
	எனில் (குறியீடு_off < 0)
		திரும்பு குறியீடு_off;

	phandle = fdt_get_phandle(fdt, குறியீடு_off);
	எனில் (!phandle)
		திரும்பு -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	எனில் (fixup_off == -FDT_ERR_NOTFOUND)
		திரும்பு -FDT_ERR_BADOVERLAY;
	எனில் (fixup_off < 0)
		திரும்பு fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	திரும்பு fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   அளவு(phandle_prop));
}

/**
 * overlay_fixup_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @property: Property offset in the overlay holding the list of fixups
 *
 * overlay_fixup_phandle() resolves all the overlay phandles pointed
 * to in a __fixups__ property, and updates them to match the phandles
 * in use in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_fixup_phandle(வெறுமை *fdt, வெறுமை *fdto, முழு symbols_off,
				 முழு property)
{
	மாறிலி எழுத்து *மதிப்பு;
	மாறிலி எழுத்து *label;
	முழு len;

	மதிப்பு = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	எனில் (!மதிப்பு) {
		எனில் (len == -FDT_ERR_NOTFOUND)
			திரும்பு -FDT_ERR_INTERNAL;

		திரும்பு len;
	}

	செய் {
		மாறிலி எழுத்து *path, *name, *fixup_end;
		மாறிலி எழுத்து *fixup_str = மதிப்பு;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		எழுத்து *sep, *endptr;
		முழு poffset, ret;

		fixup_end = memchr(மதிப்பு, '\0', len);
		எனில் (!fixup_end)
			திரும்பு -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		மதிப்பு += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		எனில் (!sep || *sep != ':')
			திரும்பு -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		எனில் (path_len == (fixup_len - 1))
			திரும்பு -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		எனில் (!sep || *sep != ':')
			திரும்பு -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		எனில் (!name_len)
			திரும்பு -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		எனில் ((*endptr != '\0') || (endptr <= (sep + 1)))
			திரும்பு -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		எனில் (ret)
			திரும்பு ret;
	} போது (len > 0);

	திரும்பு 0;
}

/**
 * overlay_fixup_phandles - Resolve the overlay phandles to the base
 *                          device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_fixup_phandles() resolves all the overlay phandles pointing
 * to nodes in the base device tree.
 *
 * This is one of the steps of the device tree overlay application
 * process, when you want all the phandles in the overlay to point to
 * the actual base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_fixup_phandles(வெறுமை *fdt, வெறுமை *fdto)
{
	முழு fixups_off, symbols_off;
	முழு property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	எனில் (fixups_off == -FDT_ERR_NOTFOUND)
		திரும்பு 0; /* nothing to do */
	எனில் (fixups_off < 0)
		திரும்பு fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	எனில் ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		திரும்பு symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		முழு ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

/**
 * overlay_apply_node - Merges a node into the base device tree
 * @fdt: Base Device Tree blob
 * @target: Node offset in the base device tree to apply the fragment to
 * @fdto: Device tree overlay blob
 * @node: Node offset in the overlay holding the changes to merge
 *
 * overlay_apply_node() merges a node into a target base device tree
 * node pointed.
 *
 * This is part of the final step in the device tree overlay
 * application process, when all the phandles have been adjusted and
 * resolved and you just have to merge overlay into the base device
 * tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_apply_முனை(வெறுமை *fdt, முழு target,
			      வெறுமை *fdto, முழு முனை)
{
	முழு property;
	முழு subnode;

	fdt_for_each_property_offset(property, fdto, முனை) {
		மாறிலி எழுத்து *name;
		மாறிலி வெறுமை *prop;
		முழு prop_len;
		முழு ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		எனில் (prop_len == -FDT_ERR_NOTFOUND)
			திரும்பு -FDT_ERR_INTERNAL;
		எனில் (prop_len < 0)
			திரும்பு prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		எனில் (ret)
			திரும்பு ret;
	}

	fdt_for_each_subnode(subnode, fdto, முனை) {
		மாறிலி எழுத்து *name = fdt_get_name(fdto, subnode, வெற்று);
		முழு nnode;
		முழு ret;

		nnode = fdt_add_subnode(fdt, target, name);
		எனில் (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			எனில் (nnode == -FDT_ERR_NOTFOUND)
				திரும்பு -FDT_ERR_INTERNAL;
		}

		எனில் (nnode < 0)
			திரும்பு nnode;

		ret = overlay_apply_முனை(fdt, nnode, fdto, subnode);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

/**
 * overlay_merge - Merge an overlay into its base device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_merge() merges an overlay into its base device tree.
 *
 * This is the next to last step in the device tree overlay application
 * process, when all the phandles have been adjusted and resolved and
 * you just have to merge overlay into the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_merge(வெறுமை *fdt, வெறுமை *fdto)
{
	முழு fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		முழு overlay;
		முழு target;
		முழு ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		எனில் (overlay == -FDT_ERR_NOTFOUND)
			தொடர்;

		எனில் (overlay < 0)
			திரும்பு overlay;

		target = overlay_get_target(fdt, fdto, fragment, வெற்று);
		எனில் (target < 0)
			திரும்பு target;

		ret = overlay_apply_முனை(fdt, target, fdto, overlay);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

நிலையான முழு get_path_len(மாறிலி வெறுமை *fdt, முழு nodeoffset)
{
	முழு len = 0, namelen;
	மாறிலி எழுத்து *name;

	FDT_RO_PROBE(fdt);

	க்கு (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		எனில் (!name)
			திரும்பு namelen;

		/* root? we're done */
		எனில் (namelen == 0)
			நிறுத்து;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		எனில் (nodeoffset < 0)
			திரும்பு nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	எனில் (len == 0)
		len++;
	திரும்பு len;
}

/**
 * overlay_symbol_update - Update the symbols of base tree after a merge
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_symbol_update() updates the symbols of the base tree with the
 * symbols of the applied overlay
 *
 * This is the last step in the device tree overlay application
 * process, allowing the reference of overlay symbols by subsequent
 * overlay operations.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
நிலையான முழு overlay_குறியீடு_update(வெறுமை *fdt, வெறுமை *fdto)
{
	முழு வேர்_sym, ov_sym, prop, path_len, fragment, target;
	முழு len, frag_name_len, ret, rel_path_len;
	மாறிலி எழுத்து *s, *e;
	மாறிலி எழுத்து *path;
	மாறிலி எழுத்து *name;
	மாறிலி எழுத்து *frag_name;
	மாறிலி எழுத்து *rel_path;
	மாறிலி எழுத்து *target_path;
	எழுத்து *buf;
	வெறுமை *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	எனில் (ov_sym < 0)
		திரும்பு 0;

	வேர்_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	எனில் (வேர்_sym == -FDT_ERR_NOTFOUND)
		வேர்_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	எனில் (வேர்_sym < 0)
		திரும்பு வேர்_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		எனில் (!path)
			திரும்பு path_len;

		/* verify it's a string property (terminated by a single \0) */
		எனில் (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			திரும்பு -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		எனில் (*path != '/')
			திரும்பு -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		எனில் (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			தொடர்;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = அளவு("/__overlay__/") - 1;
		எனில் ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} இல்லையெனில் எனில் ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} இல்லையெனில் {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			தொடர்;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		எனில் (ret < 0)
			திரும்பு -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		எனில் (ret < 0)
			திரும்பு -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		எனில் (ret < 0)
			திரும்பு ret;
		target = ret;

		/* if we have a target path use */
		எனில் (!target_path) {
			ret = get_path_len(fdt, target);
			எனில் (ret < 0)
				திரும்பு ret;
			len = ret;
		} இல்லையெனில் {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, வேர்_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		எனில் (ret < 0)
			திரும்பு ret;

		எனில் (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			எனில் (ret < 0)
				திரும்பு ret;
			target = ret;
		}

		buf = p;
		எனில் (len > 1) { /* target is not root */
			எனில் (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				எனில் (ret < 0)
					திரும்பு ret;
			} இல்லையெனில்
				memcpy(buf, target_path, len + 1);

		} இல்லையெனில்
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	திரும்பு 0;
}

முழு fdt_overlay_apply(வெறுமை *fdt, வெறுமை *fdto)
{
	uint32_t delta;
	முழு ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	எனில் (ret)
		செல் err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	எனில் (ret)
		செல் err;

	ret = overlay_update_local_references(fdto, delta);
	எனில் (ret)
		செல் err;

	ret = overlay_fixup_phandles(fdt, fdto);
	எனில் (ret)
		செல் err;

	ret = overlay_merge(fdt, fdto);
	எனில் (ret)
		செல் err;

	ret = overlay_குறியீடு_update(fdt, fdto);
	எனில் (ret)
		செல் err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	திரும்பு 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_set_magic(fdt, ~0);

	திரும்பு ret;
}
