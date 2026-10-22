// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#অন্তর্ভাৱ "libfdt_env.h"

#অন্তর্ভাৱ <fdt.h>
#অন্তর্ভাৱ <libfdt.h>

#অন্তর্ভাৱ "libfdt_internal.h"

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
স্থির uint32_t overlay_get_target_phandle(ধ্রুৱ শূন্য *fdto, পূর্ণাঙ্ক fragment)
{
	ধ্রুৱ fdt32_t *val;
	পূর্ণাঙ্ক len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	যদি (!val)
		ঘূৰাই 0;

	যদি ((len != পরিমাণ(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ঘূৰাই (uint32_t)-1;

	ঘূৰাই fdt32_to_cpu(*val);
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
স্থির পূর্ণাঙ্ক overlay_get_target(ধ্রুৱ শূন্য *fdt, ধ্রুৱ শূন্য *fdto,
			      পূর্ণাঙ্ক fragment, ৱর্ণ ধ্রুৱ **pathp)
{
	uint32_t phandle;
	ধ্রুৱ ৱর্ণ *path = রিক্ত;
	পূর্ণাঙ্ক path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	যদি (phandle == (uint32_t)-1)
		ঘূৰাই -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	যদি (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		যদি (path)
			ret = fdt_path_offset(fdt, path);
		অন্যথা
			ret = path_len;
	} অন্যথা
		ret = fdt_গ্ৰন্থি_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	যদি (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	যদি (ret < 0)
		ঘূৰাই ret;

	/* return pointer to path (if available) */
	যদি (pathp)
		*pathp = path ? path : রিক্ত;

	ঘূৰাই ret;
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
স্থির পূর্ণাঙ্ক overlay_phandle_add_offset(শূন্য *fdt, পূর্ণাঙ্ক গ্ৰন্থি,
				      ধ্রুৱ ৱর্ণ *name, uint32_t delta)
{
	ধ্রুৱ fdt32_t *val;
	uint32_t adj_val;
	পূর্ণাঙ্ক len;

	val = fdt_getprop(fdt, গ্ৰন্থি, name, &len);
	যদি (!val)
		ঘূৰাই len;

	যদি (len != পরিমাণ(*val))
		ঘূৰাই -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	যদি ((adj_val + delta) < adj_val)
		ঘূৰাই -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	যদি (adj_val == (uint32_t)-1)
		ঘূৰাই -FDT_ERR_NOPHANDLES;

	ঘূৰাই fdt_setprop_inplace_u32(fdt, গ্ৰন্থি, name, adj_val);
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
স্থির পূর্ণাঙ্ক overlay_adjust_গ্ৰন্থি_phandles(শূন্য *fdto, পূর্ণাঙ্ক গ্ৰন্থি,
					uint32_t delta)
{
	পূর্ণাঙ্ক child;
	পূর্ণাঙ্ক ret;

	ret = overlay_phandle_add_offset(fdto, গ্ৰন্থি, "phandle", delta);
	যদি (ret && ret != -FDT_ERR_NOTFOUND)
		ঘূৰাই ret;

	ret = overlay_phandle_add_offset(fdto, গ্ৰন্থি, "linux,phandle", delta);
	যদি (ret && ret != -FDT_ERR_NOTFOUND)
		ঘূৰাই ret;

	fdt_for_each_subnode(child, fdto, গ্ৰন্থি) {
		ret = overlay_adjust_গ্ৰন্থি_phandles(fdto, child, delta);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
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
স্থির পূর্ণাঙ্ক overlay_adjust_local_phandles(শূন্য *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ঘূৰাই overlay_adjust_গ্ৰন্থি_phandles(fdto, 0, delta);
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
স্থির পূর্ণাঙ্ক overlay_update_local_গ্ৰন্থি_references(শূন্য *fdto,
						পূর্ণাঙ্ক বৃক্ষ_গ্ৰন্থি,
						পূর্ণাঙ্ক fixup_গ্ৰন্থি,
						uint32_t delta)
{
	পূর্ণাঙ্ক fixup_prop;
	পূর্ণাঙ্ক fixup_child;
	পূর্ণাঙ্ক ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_গ্ৰন্থি) {
		ধ্রুৱ fdt32_t *fixup_val;
		ধ্রুৱ ৱর্ণ *বৃক্ষ_val;
		ধ্রুৱ ৱর্ণ *name;
		পূর্ণাঙ্ক fixup_len;
		পূর্ণাঙ্ক বৃক্ষ_len;
		পূর্ণাঙ্ক i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		যদি (!fixup_val)
			ঘূৰাই fixup_len;

		যদি (fixup_len % পরিমাণ(uint32_t))
			ঘূৰাই -FDT_ERR_BADOVERLAY;
		fixup_len /= পরিমাণ(uint32_t);

		বৃক্ষ_val = fdt_getprop(fdto, বৃক্ষ_গ্ৰন্থি, name, &বৃক্ষ_len);
		যদি (!বৃক্ষ_val) {
			যদি (বৃক্ষ_len == -FDT_ERR_NOTFOUND)
				ঘূৰাই -FDT_ERR_BADOVERLAY;

			ঘূৰাই বৃক্ষ_len;
		}

		বাবে (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, বৃক্ষ_val + poffset, পরিমাণ(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  বৃক্ষ_গ্ৰন্থি,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  পরিমাণ(adj_val));
			যদি (ret == -FDT_ERR_NOSPACE)
				ঘূৰাই -FDT_ERR_BADOVERLAY;

			যদি (ret)
				ঘূৰাই ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_গ্ৰন্থি) {
		ধ্রুৱ ৱর্ণ *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    রিক্ত);
		পূর্ণাঙ্ক বৃক্ষ_child;

		বৃক্ষ_child = fdt_subnode_offset(fdto, বৃক্ষ_গ্ৰন্থি,
						fixup_child_name);
		যদি (বৃক্ষ_child == -FDT_ERR_NOTFOUND)
			ঘূৰাই -FDT_ERR_BADOVERLAY;
		যদি (বৃক্ষ_child < 0)
			ঘূৰাই বৃক্ষ_child;

		ret = overlay_update_local_গ্ৰন্থি_references(fdto,
							   বৃক্ষ_child,
							   fixup_child,
							   delta);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
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
স্থির পূর্ণাঙ্ক overlay_update_local_references(শূন্য *fdto, uint32_t delta)
{
	পূর্ণাঙ্ক fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	যদি (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		যদি (fixups == -FDT_ERR_NOTFOUND)
			ঘূৰাই 0;

		ঘূৰাই fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ঘূৰাই overlay_update_local_গ্ৰন্থি_references(fdto, 0, fixups,
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
স্থির পূর্ণাঙ্ক overlay_fixup_one_phandle(শূন্য *fdt, শূন্য *fdto,
				     পূর্ণাঙ্ক symbols_off,
				     ধ্রুৱ ৱর্ণ *path, uint32_t path_len,
				     ধ্রুৱ ৱর্ণ *name, uint32_t name_len,
				     পূর্ণাঙ্ক poffset, ধ্রুৱ ৱর্ণ *label)
{
	ধ্রুৱ ৱর্ণ *প্ৰতীক_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	পূর্ণাঙ্ক প্ৰতীক_off, fixup_off;
	পূর্ণাঙ্ক prop_len;

	যদি (symbols_off < 0)
		ঘূৰাই symbols_off;

	প্ৰতীক_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	যদি (!প্ৰতীক_path)
		ঘূৰাই prop_len;

	প্ৰতীক_off = fdt_path_offset(fdt, প্ৰতীক_path);
	যদি (প্ৰতীক_off < 0)
		ঘূৰাই প্ৰতীক_off;

	phandle = fdt_get_phandle(fdt, প্ৰতীক_off);
	যদি (!phandle)
		ঘূৰাই -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	যদি (fixup_off == -FDT_ERR_NOTFOUND)
		ঘূৰাই -FDT_ERR_BADOVERLAY;
	যদি (fixup_off < 0)
		ঘূৰাই fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ঘূৰাই fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   পরিমাণ(phandle_prop));
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
স্থির পূর্ণাঙ্ক overlay_fixup_phandle(শূন্য *fdt, শূন্য *fdto, পূর্ণাঙ্ক symbols_off,
				 পূর্ণাঙ্ক property)
{
	ধ্রুৱ ৱর্ণ *মান;
	ধ্রুৱ ৱর্ণ *label;
	পূর্ণাঙ্ক len;

	মান = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	যদি (!মান) {
		যদি (len == -FDT_ERR_NOTFOUND)
			ঘূৰাই -FDT_ERR_INTERNAL;

		ঘূৰাই len;
	}

	কৰা {
		ধ্রুৱ ৱর্ণ *path, *name, *fixup_end;
		ধ্রুৱ ৱর্ণ *fixup_str = মান;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		ৱর্ণ *sep, *endptr;
		পূর্ণাঙ্ক poffset, ret;

		fixup_end = memchr(মান, '\0', len);
		যদি (!fixup_end)
			ঘূৰাই -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		মান += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		যদি (!sep || *sep != ':')
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		যদি (path_len == (fixup_len - 1))
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		যদি (!sep || *sep != ':')
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		যদি (!name_len)
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		যদি ((*endptr != '\0') || (endptr <= (sep + 1)))
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		যদি (ret)
			ঘূৰাই ret;
	} যেতিয়ালৈ (len > 0);

	ঘূৰাই 0;
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
স্থির পূর্ণাঙ্ক overlay_fixup_phandles(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণাঙ্ক fixups_off, symbols_off;
	পূর্ণাঙ্ক property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	যদি (fixups_off == -FDT_ERR_NOTFOUND)
		ঘূৰাই 0; /* nothing to do */
	যদি (fixups_off < 0)
		ঘূৰাই fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	যদি ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ঘূৰাই symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		পূর্ণাঙ্ক ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
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
স্থির পূর্ণাঙ্ক overlay_apply_গ্ৰন্থি(শূন্য *fdt, পূর্ণাঙ্ক target,
			      শূন্য *fdto, পূর্ণাঙ্ক গ্ৰন্থি)
{
	পূর্ণাঙ্ক property;
	পূর্ণাঙ্ক subnode;

	fdt_for_each_property_offset(property, fdto, গ্ৰন্থি) {
		ধ্রুৱ ৱর্ণ *name;
		ধ্রুৱ শূন্য *prop;
		পূর্ণাঙ্ক prop_len;
		পূর্ণাঙ্ক ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		যদি (prop_len == -FDT_ERR_NOTFOUND)
			ঘূৰাই -FDT_ERR_INTERNAL;
		যদি (prop_len < 0)
			ঘূৰাই prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		যদি (ret)
			ঘূৰাই ret;
	}

	fdt_for_each_subnode(subnode, fdto, গ্ৰন্থি) {
		ধ্রুৱ ৱর্ণ *name = fdt_get_name(fdto, subnode, রিক্ত);
		পূর্ণাঙ্ক nnode;
		পূর্ণাঙ্ক ret;

		nnode = fdt_add_subnode(fdt, target, name);
		যদি (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			যদি (nnode == -FDT_ERR_NOTFOUND)
				ঘূৰাই -FDT_ERR_INTERNAL;
		}

		যদি (nnode < 0)
			ঘূৰাই nnode;

		ret = overlay_apply_গ্ৰন্থি(fdt, nnode, fdto, subnode);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
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
স্থির পূর্ণাঙ্ক overlay_merge(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণাঙ্ক fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		পূর্ণাঙ্ক overlay;
		পূর্ণাঙ্ক target;
		পূর্ণাঙ্ক ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		যদি (overlay == -FDT_ERR_NOTFOUND)
			অনুৱর্ত;

		যদি (overlay < 0)
			ঘূৰাই overlay;

		target = overlay_get_target(fdt, fdto, fragment, রিক্ত);
		যদি (target < 0)
			ঘূৰাই target;

		ret = overlay_apply_গ্ৰন্থি(fdt, target, fdto, overlay);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক get_path_len(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক len = 0, namelen;
	ধ্রুৱ ৱর্ণ *name;

	FDT_RO_PROBE(fdt);

	বাবে (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		যদি (!name)
			ঘূৰাই namelen;

		/* root? we're done */
		যদি (namelen == 0)
			ভঙ্গ;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		যদি (nodeoffset < 0)
			ঘূৰাই nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	যদি (len == 0)
		len++;
	ঘূৰাই len;
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
স্থির পূর্ণাঙ্ক overlay_প্ৰতীক_update(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণাঙ্ক মূল_sym, ov_sym, prop, path_len, fragment, target;
	পূর্ণাঙ্ক len, frag_name_len, ret, rel_path_len;
	ধ্রুৱ ৱর্ণ *s, *e;
	ধ্রুৱ ৱর্ণ *path;
	ধ্রুৱ ৱর্ণ *name;
	ধ্রুৱ ৱর্ণ *frag_name;
	ধ্রুৱ ৱর্ণ *rel_path;
	ধ্রুৱ ৱর্ণ *target_path;
	ৱর্ণ *buf;
	শূন্য *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	যদি (ov_sym < 0)
		ঘূৰাই 0;

	মূল_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	যদি (মূল_sym == -FDT_ERR_NOTFOUND)
		মূল_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	যদি (মূল_sym < 0)
		ঘূৰাই মূল_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		যদি (!path)
			ঘূৰাই path_len;

		/* verify it's a string property (terminated by a single \0) */
		যদি (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ঘূৰাই -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		যদি (*path != '/')
			ঘূৰাই -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		যদি (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			অনুৱর্ত;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = পরিমাণ("/__overlay__/") - 1;
		যদি ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} অন্যথা যদি ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} অন্যথা {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			অনুৱর্ত;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		যদি (ret < 0)
			ঘূৰাই -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		যদি (ret < 0)
			ঘূৰাই -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		যদি (ret < 0)
			ঘূৰাই ret;
		target = ret;

		/* if we have a target path use */
		যদি (!target_path) {
			ret = get_path_len(fdt, target);
			যদি (ret < 0)
				ঘূৰাই ret;
			len = ret;
		} অন্যথা {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, মূল_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		যদি (ret < 0)
			ঘূৰাই ret;

		যদি (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			যদি (ret < 0)
				ঘূৰাই ret;
			target = ret;
		}

		buf = p;
		যদি (len > 1) { /* target is not root */
			যদি (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				যদি (ret < 0)
					ঘূৰাই ret;
			} অন্যথা
				memcpy(buf, target_path, len + 1);

		} অন্যথা
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ঘূৰাই 0;
}

পূর্ণাঙ্ক fdt_overlay_apply(শূন্য *fdt, শূন্য *fdto)
{
	uint32_t delta;
	পূর্ণাঙ্ক ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	যদি (ret)
		গচ্ছ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	যদি (ret)
		গচ্ছ err;

	ret = overlay_update_local_references(fdto, delta);
	যদি (ret)
		গচ্ছ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	যদি (ret)
		গচ্ছ err;

	ret = overlay_merge(fdt, fdto);
	যদি (ret)
		গচ্ছ err;

	ret = overlay_প্ৰতীক_update(fdt, fdto);
	যদি (ret)
		গচ্ছ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ঘূৰাই 0;

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

	ঘূৰাই ret;
}
