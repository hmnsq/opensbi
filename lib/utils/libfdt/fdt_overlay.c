// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

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
স্থির uint32_t overlay_get_target_phandle(ধ্রুবক শূন্য *fdto, পূর্ণসংখ্যা fragment)
{
	ধ্রুবক fdt32_t *val;
	পূর্ণসংখ্যা len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	যদি (!val)
		ফেরত 0;

	যদি ((len != আকার(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ফেরত (uint32_t)-1;

	ফেরত fdt32_to_cpu(*val);
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
স্থির পূর্ণসংখ্যা overlay_get_target(ধ্রুবক শূন্য *fdt, ধ্রুবক শূন্য *fdto,
			      পূর্ণসংখ্যা fragment, অক্ষর ধ্রুবক **pathp)
{
	uint32_t phandle;
	ধ্রুবক অক্ষর *path = ফাঁকা;
	পূর্ণসংখ্যা path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	যদি (phandle == (uint32_t)-1)
		ফেরত -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	যদি (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		যদি (path)
			ret = fdt_path_offset(fdt, path);
		অন্যথা
			ret = path_len;
	} অন্যথা
		ret = fdt_গ্রন্থি_offset_by_phandle(fdt, phandle);

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
		ফেরত ret;

	/* return pointer to path (if available) */
	যদি (pathp)
		*pathp = path ? path : ফাঁকা;

	ফেরত ret;
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
স্থির পূর্ণসংখ্যা overlay_phandle_add_offset(শূন্য *fdt, পূর্ণসংখ্যা গ্রন্থি,
				      ধ্রুবক অক্ষর *name, uint32_t delta)
{
	ধ্রুবক fdt32_t *val;
	uint32_t adj_val;
	পূর্ণসংখ্যা len;

	val = fdt_getprop(fdt, গ্রন্থি, name, &len);
	যদি (!val)
		ফেরত len;

	যদি (len != আকার(*val))
		ফেরত -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	যদি ((adj_val + delta) < adj_val)
		ফেরত -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	যদি (adj_val == (uint32_t)-1)
		ফেরত -FDT_ERR_NOPHANDLES;

	ফেরত fdt_setprop_inplace_u32(fdt, গ্রন্থি, name, adj_val);
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
স্থির পূর্ণসংখ্যা overlay_adjust_গ্রন্থি_phandles(শূন্য *fdto, পূর্ণসংখ্যা গ্রন্থি,
					uint32_t delta)
{
	পূর্ণসংখ্যা child;
	পূর্ণসংখ্যা ret;

	ret = overlay_phandle_add_offset(fdto, গ্রন্থি, "phandle", delta);
	যদি (ret && ret != -FDT_ERR_NOTFOUND)
		ফেরত ret;

	ret = overlay_phandle_add_offset(fdto, গ্রন্থি, "linux,phandle", delta);
	যদি (ret && ret != -FDT_ERR_NOTFOUND)
		ফেরত ret;

	fdt_for_each_subnode(child, fdto, গ্রন্থি) {
		ret = overlay_adjust_গ্রন্থি_phandles(fdto, child, delta);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
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
স্থির পূর্ণসংখ্যা overlay_adjust_local_phandles(শূন্য *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ফেরত overlay_adjust_গ্রন্থি_phandles(fdto, 0, delta);
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
স্থির পূর্ণসংখ্যা overlay_update_local_গ্রন্থি_references(শূন্য *fdto,
						পূর্ণসংখ্যা বৃক্ষ_গ্রন্থি,
						পূর্ণসংখ্যা fixup_গ্রন্থি,
						uint32_t delta)
{
	পূর্ণসংখ্যা fixup_prop;
	পূর্ণসংখ্যা fixup_child;
	পূর্ণসংখ্যা ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_গ্রন্থি) {
		ধ্রুবক fdt32_t *fixup_val;
		ধ্রুবক অক্ষর *বৃক্ষ_val;
		ধ্রুবক অক্ষর *name;
		পূর্ণসংখ্যা fixup_len;
		পূর্ণসংখ্যা বৃক্ষ_len;
		পূর্ণসংখ্যা i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		যদি (!fixup_val)
			ফেরত fixup_len;

		যদি (fixup_len % আকার(uint32_t))
			ফেরত -FDT_ERR_BADOVERLAY;
		fixup_len /= আকার(uint32_t);

		বৃক্ষ_val = fdt_getprop(fdto, বৃক্ষ_গ্রন্থি, name, &বৃক্ষ_len);
		যদি (!বৃক্ষ_val) {
			যদি (বৃক্ষ_len == -FDT_ERR_NOTFOUND)
				ফেরত -FDT_ERR_BADOVERLAY;

			ফেরত বৃক্ষ_len;
		}

		জন্য (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, বৃক্ষ_val + poffset, আকার(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  বৃক্ষ_গ্রন্থি,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  আকার(adj_val));
			যদি (ret == -FDT_ERR_NOSPACE)
				ফেরত -FDT_ERR_BADOVERLAY;

			যদি (ret)
				ফেরত ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_গ্রন্থি) {
		ধ্রুবক অক্ষর *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    ফাঁকা);
		পূর্ণসংখ্যা বৃক্ষ_child;

		বৃক্ষ_child = fdt_subnode_offset(fdto, বৃক্ষ_গ্রন্থি,
						fixup_child_name);
		যদি (বৃক্ষ_child == -FDT_ERR_NOTFOUND)
			ফেরত -FDT_ERR_BADOVERLAY;
		যদি (বৃক্ষ_child < 0)
			ফেরত বৃক্ষ_child;

		ret = overlay_update_local_গ্রন্থি_references(fdto,
							   বৃক্ষ_child,
							   fixup_child,
							   delta);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
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
স্থির পূর্ণসংখ্যা overlay_update_local_references(শূন্য *fdto, uint32_t delta)
{
	পূর্ণসংখ্যা fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	যদি (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		যদি (fixups == -FDT_ERR_NOTFOUND)
			ফেরত 0;

		ফেরত fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ফেরত overlay_update_local_গ্রন্থি_references(fdto, 0, fixups,
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
স্থির পূর্ণসংখ্যা overlay_fixup_one_phandle(শূন্য *fdt, শূন্য *fdto,
				     পূর্ণসংখ্যা symbols_off,
				     ধ্রুবক অক্ষর *path, uint32_t path_len,
				     ধ্রুবক অক্ষর *name, uint32_t name_len,
				     পূর্ণসংখ্যা poffset, ধ্রুবক অক্ষর *label)
{
	ধ্রুবক অক্ষর *প্রতীক_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	পূর্ণসংখ্যা প্রতীক_off, fixup_off;
	পূর্ণসংখ্যা prop_len;

	যদি (symbols_off < 0)
		ফেরত symbols_off;

	প্রতীক_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	যদি (!প্রতীক_path)
		ফেরত prop_len;

	প্রতীক_off = fdt_path_offset(fdt, প্রতীক_path);
	যদি (প্রতীক_off < 0)
		ফেরত প্রতীক_off;

	phandle = fdt_get_phandle(fdt, প্রতীক_off);
	যদি (!phandle)
		ফেরত -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	যদি (fixup_off == -FDT_ERR_NOTFOUND)
		ফেরত -FDT_ERR_BADOVERLAY;
	যদি (fixup_off < 0)
		ফেরত fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ফেরত fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   আকার(phandle_prop));
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
স্থির পূর্ণসংখ্যা overlay_fixup_phandle(শূন্য *fdt, শূন্য *fdto, পূর্ণসংখ্যা symbols_off,
				 পূর্ণসংখ্যা property)
{
	ধ্রুবক অক্ষর *মান;
	ধ্রুবক অক্ষর *label;
	পূর্ণসংখ্যা len;

	মান = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	যদি (!মান) {
		যদি (len == -FDT_ERR_NOTFOUND)
			ফেরত -FDT_ERR_INTERNAL;

		ফেরত len;
	}

	করো {
		ধ্রুবক অক্ষর *path, *name, *fixup_end;
		ধ্রুবক অক্ষর *fixup_str = মান;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		অক্ষর *sep, *endptr;
		পূর্ণসংখ্যা poffset, ret;

		fixup_end = memchr(মান, '\0', len);
		যদি (!fixup_end)
			ফেরত -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		মান += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		যদি (!sep || *sep != ':')
			ফেরত -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		যদি (path_len == (fixup_len - 1))
			ফেরত -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		যদি (!sep || *sep != ':')
			ফেরত -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		যদি (!name_len)
			ফেরত -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		যদি ((*endptr != '\0') || (endptr <= (sep + 1)))
			ফেরত -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		যদি (ret)
			ফেরত ret;
	} যখন (len > 0);

	ফেরত 0;
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
স্থির পূর্ণসংখ্যা overlay_fixup_phandles(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণসংখ্যা fixups_off, symbols_off;
	পূর্ণসংখ্যা property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	যদি (fixups_off == -FDT_ERR_NOTFOUND)
		ফেরত 0; /* nothing to do */
	যদি (fixups_off < 0)
		ফেরত fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	যদি ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ফেরত symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		পূর্ণসংখ্যা ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
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
স্থির পূর্ণসংখ্যা overlay_apply_গ্রন্থি(শূন্য *fdt, পূর্ণসংখ্যা target,
			      শূন্য *fdto, পূর্ণসংখ্যা গ্রন্থি)
{
	পূর্ণসংখ্যা property;
	পূর্ণসংখ্যা subnode;

	fdt_for_each_property_offset(property, fdto, গ্রন্থি) {
		ধ্রুবক অক্ষর *name;
		ধ্রুবক শূন্য *prop;
		পূর্ণসংখ্যা prop_len;
		পূর্ণসংখ্যা ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		যদি (prop_len == -FDT_ERR_NOTFOUND)
			ফেরত -FDT_ERR_INTERNAL;
		যদি (prop_len < 0)
			ফেরত prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		যদি (ret)
			ফেরত ret;
	}

	fdt_for_each_subnode(subnode, fdto, গ্রন্থি) {
		ধ্রুবক অক্ষর *name = fdt_get_name(fdto, subnode, ফাঁকা);
		পূর্ণসংখ্যা nnode;
		পূর্ণসংখ্যা ret;

		nnode = fdt_add_subnode(fdt, target, name);
		যদি (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			যদি (nnode == -FDT_ERR_NOTFOUND)
				ফেরত -FDT_ERR_INTERNAL;
		}

		যদি (nnode < 0)
			ফেরত nnode;

		ret = overlay_apply_গ্রন্থি(fdt, nnode, fdto, subnode);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
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
স্থির পূর্ণসংখ্যা overlay_merge(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণসংখ্যা fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		পূর্ণসংখ্যা overlay;
		পূর্ণসংখ্যা target;
		পূর্ণসংখ্যা ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		যদি (overlay == -FDT_ERR_NOTFOUND)
			চালু;

		যদি (overlay < 0)
			ফেরত overlay;

		target = overlay_get_target(fdt, fdto, fragment, ফাঁকা);
		যদি (target < 0)
			ফেরত target;

		ret = overlay_apply_গ্রন্থি(fdt, target, fdto, overlay);
		যদি (ret)
			ফেরত ret;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা get_path_len(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা len = 0, namelen;
	ধ্রুবক অক্ষর *name;

	FDT_RO_PROBE(fdt);

	জন্য (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		যদি (!name)
			ফেরত namelen;

		/* root? we're done */
		যদি (namelen == 0)
			বিরতি;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		যদি (nodeoffset < 0)
			ফেরত nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	যদি (len == 0)
		len++;
	ফেরত len;
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
স্থির পূর্ণসংখ্যা overlay_প্রতীক_update(শূন্য *fdt, শূন্য *fdto)
{
	পূর্ণসংখ্যা মূল_sym, ov_sym, prop, path_len, fragment, target;
	পূর্ণসংখ্যা len, frag_name_len, ret, rel_path_len;
	ধ্রুবক অক্ষর *s, *e;
	ধ্রুবক অক্ষর *path;
	ধ্রুবক অক্ষর *name;
	ধ্রুবক অক্ষর *frag_name;
	ধ্রুবক অক্ষর *rel_path;
	ধ্রুবক অক্ষর *target_path;
	অক্ষর *buf;
	শূন্য *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	যদি (ov_sym < 0)
		ফেরত 0;

	মূল_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	যদি (মূল_sym == -FDT_ERR_NOTFOUND)
		মূল_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	যদি (মূল_sym < 0)
		ফেরত মূল_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		যদি (!path)
			ফেরত path_len;

		/* verify it's a string property (terminated by a single \0) */
		যদি (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ফেরত -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		যদি (*path != '/')
			ফেরত -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		যদি (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			চালু;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = আকার("/__overlay__/") - 1;
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
			চালু;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		যদি (ret < 0)
			ফেরত -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		যদি (ret < 0)
			ফেরত -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		যদি (ret < 0)
			ফেরত ret;
		target = ret;

		/* if we have a target path use */
		যদি (!target_path) {
			ret = get_path_len(fdt, target);
			যদি (ret < 0)
				ফেরত ret;
			len = ret;
		} অন্যথা {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, মূল_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		যদি (ret < 0)
			ফেরত ret;

		যদি (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			যদি (ret < 0)
				ফেরত ret;
			target = ret;
		}

		buf = p;
		যদি (len > 1) { /* target is not root */
			যদি (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				যদি (ret < 0)
					ফেরত ret;
			} অন্যথা
				memcpy(buf, target_path, len + 1);

		} অন্যথা
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_overlay_apply(শূন্য *fdt, শূন্য *fdto)
{
	uint32_t delta;
	পূর্ণসংখ্যা ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	যদি (ret)
		যাও err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	যদি (ret)
		যাও err;

	ret = overlay_update_local_references(fdto, delta);
	যদি (ret)
		যাও err;

	ret = overlay_fixup_phandles(fdt, fdto);
	যদি (ret)
		যাও err;

	ret = overlay_merge(fdt, fdto);
	যদি (ret)
		যাও err;

	ret = overlay_প্রতীক_update(fdt, fdto);
	যদি (ret)
		যাও err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ফেরত 0;

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

	ফেরত ret;
}
