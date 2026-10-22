// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

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
static uint32_t overlay_get_target_phandle(const void *fdto, puur_naa_nka fragment)
{
	const fdt32_t *val;
	puur_naa_nka len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ᯀᯮᯝ (!val)
		ᯀᯮᯞ 0;

	ᯀᯮᯝ ((len != parimaa_na(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ᯀᯮᯞ (uint32_t)-1;

	ᯀᯮᯞ fdt32_to_cpu(*val);
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
static puur_naa_nka overlay_get_target(const void *fdt, const void *fdto,
			      puur_naa_nka fragment, var_na const **pathp)
{
	uint32_t phandle;
	const var_na *path = NULL;
	puur_naa_nka path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	ᯀᯮᯝ (phandle == (uint32_t)-1)
		ᯀᯮᯞ -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ᯀᯮᯝ (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		ᯀᯮᯝ (path)
			ret = fdt_path_offset(fdt, path);
		ᯀᯮᯝᯂᯪ
			ret = path_len;
	} ᯀᯮᯝᯂᯪ
		ret = fdt_node_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ᯀᯮᯝ (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ᯀᯮᯝ (ret < 0)
		ᯀᯮᯞ ret;

	/* return pointer to path (if available) */
	ᯀᯮᯝ (pathp)
		*pathp = path ? path : NULL;

	ᯀᯮᯞ ret;
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
static puur_naa_nka overlay_phandle_add_offset(void *fdt, puur_naa_nka node,
				      const var_na *name, uint32_t delta)
{
	const fdt32_t *val;
	uint32_t adj_val;
	puur_naa_nka len;

	val = fdt_getprop(fdt, node, name, &len);
	ᯀᯮᯝ (!val)
		ᯀᯮᯞ len;

	ᯀᯮᯝ (len != parimaa_na(*val))
		ᯀᯮᯞ -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	ᯀᯮᯝ ((adj_val + delta) < adj_val)
		ᯀᯮᯞ -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ᯀᯮᯝ (adj_val == (uint32_t)-1)
		ᯀᯮᯞ -FDT_ERR_NOPHANDLES;

	ᯀᯮᯞ fdt_setprop_inplace_u32(fdt, node, name, adj_val);
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
static puur_naa_nka overlay_adjust_node_phandles(void *fdto, puur_naa_nka node,
					uint32_t delta)
{
	puur_naa_nka child;
	puur_naa_nka ret;

	ret = overlay_phandle_add_offset(fdto, node, "phandle", delta);
	ᯀᯮᯝ (ret && ret != -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ ret;

	ret = overlay_phandle_add_offset(fdto, node, "linux,phandle", delta);
	ᯀᯮᯝ (ret && ret != -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ ret;

	fdt_for_each_subnode(child, fdto, node) {
		ret = overlay_adjust_node_phandles(fdto, child, delta);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	ᯀᯮᯞ 0;
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
static puur_naa_nka overlay_adjust_local_phandles(void *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ᯀᯮᯞ overlay_adjust_node_phandles(fdto, 0, delta);
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
static puur_naa_nka overlay_update_local_node_references(void *fdto,
						puur_naa_nka tree_node,
						puur_naa_nka fixup_node,
						uint32_t delta)
{
	puur_naa_nka fixup_prop;
	puur_naa_nka fixup_child;
	puur_naa_nka ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_node) {
		const fdt32_t *fixup_val;
		const var_na *tree_val;
		const var_na *name;
		puur_naa_nka fixup_len;
		puur_naa_nka tree_len;
		puur_naa_nka i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		ᯀᯮᯝ (!fixup_val)
			ᯀᯮᯞ fixup_len;

		ᯀᯮᯝ (fixup_len % parimaa_na(uint32_t))
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;
		fixup_len /= parimaa_na(uint32_t);

		tree_val = fdt_getprop(fdto, tree_node, name, &tree_len);
		ᯀᯮᯝ (!tree_val) {
			ᯀᯮᯝ (tree_len == -FDT_ERR_NOTFOUND)
				ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

			ᯀᯮᯞ tree_len;
		}

		for (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, tree_val + poffset, parimaa_na(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  tree_node,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  parimaa_na(adj_val));
			ᯀᯮᯝ (ret == -FDT_ERR_NOSPACE)
				ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

			ᯀᯮᯝ (ret)
				ᯀᯮᯞ ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_node) {
		const var_na *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    NULL);
		puur_naa_nka tree_child;

		tree_child = fdt_subnode_offset(fdto, tree_node,
						fixup_child_name);
		ᯀᯮᯝ (tree_child == -FDT_ERR_NOTFOUND)
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;
		ᯀᯮᯝ (tree_child < 0)
			ᯀᯮᯞ tree_child;

		ret = overlay_update_local_node_references(fdto,
							   tree_child,
							   fixup_child,
							   delta);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	ᯀᯮᯞ 0;
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
static puur_naa_nka overlay_update_local_references(void *fdto, uint32_t delta)
{
	puur_naa_nka fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	ᯀᯮᯝ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ᯀᯮᯝ (fixups == -FDT_ERR_NOTFOUND)
			ᯀᯮᯞ 0;

		ᯀᯮᯞ fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ᯀᯮᯞ overlay_update_local_node_references(fdto, 0, fixups,
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
static puur_naa_nka overlay_fixup_one_phandle(void *fdt, void *fdto,
				     puur_naa_nka symbols_off,
				     const var_na *path, uint32_t path_len,
				     const var_na *name, uint32_t name_len,
				     puur_naa_nka poffset, const var_na *label)
{
	const var_na *symbol_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	puur_naa_nka symbol_off, fixup_off;
	puur_naa_nka prop_len;

	ᯀᯮᯝ (symbols_off < 0)
		ᯀᯮᯞ symbols_off;

	symbol_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ᯀᯮᯝ (!symbol_path)
		ᯀᯮᯞ prop_len;

	symbol_off = fdt_path_offset(fdt, symbol_path);
	ᯀᯮᯝ (symbol_off < 0)
		ᯀᯮᯞ symbol_off;

	phandle = fdt_get_phandle(fdt, symbol_off);
	ᯀᯮᯝ (!phandle)
		ᯀᯮᯞ -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	ᯀᯮᯝ (fixup_off == -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ -FDT_ERR_BADOVERLAY;
	ᯀᯮᯝ (fixup_off < 0)
		ᯀᯮᯞ fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ᯀᯮᯞ fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   parimaa_na(phandle_prop));
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
static puur_naa_nka overlay_fixup_phandle(void *fdt, void *fdto, puur_naa_nka symbols_off,
				 puur_naa_nka property)
{
	const var_na *value;
	const var_na *label;
	puur_naa_nka len;

	value = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	ᯀᯮᯝ (!value) {
		ᯀᯮᯝ (len == -FDT_ERR_NOTFOUND)
			ᯀᯮᯞ -FDT_ERR_INTERNAL;

		ᯀᯮᯞ len;
	}

	do {
		const var_na *path, *name, *fixup_end;
		const var_na *fixup_str = value;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		var_na *sep, *endptr;
		puur_naa_nka poffset, ret;

		fixup_end = memchr(value, '\0', len);
		ᯀᯮᯝ (!fixup_end)
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		value += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ᯀᯮᯝ (!sep || *sep != ':')
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		ᯀᯮᯝ (path_len == (fixup_len - 1))
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ᯀᯮᯝ (!sep || *sep != ':')
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ᯀᯮᯝ (!name_len)
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ᯀᯮᯝ ((*endptr != '\0') || (endptr <= (sep + 1)))
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	} ᯀᯬᯝ (len > 0);

	ᯀᯮᯞ 0;
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
static puur_naa_nka overlay_fixup_phandles(void *fdt, void *fdto)
{
	puur_naa_nka fixups_off, symbols_off;
	puur_naa_nka property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	ᯀᯮᯝ (fixups_off == -FDT_ERR_NOTFOUND)
		ᯀᯮᯞ 0; /* nothing to do */
	ᯀᯮᯝ (fixups_off < 0)
		ᯀᯮᯞ fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	ᯀᯮᯝ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ᯀᯮᯞ symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		puur_naa_nka ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	ᯀᯮᯞ 0;
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
static puur_naa_nka overlay_apply_node(void *fdt, puur_naa_nka target,
			      void *fdto, puur_naa_nka node)
{
	puur_naa_nka property;
	puur_naa_nka subnode;

	fdt_for_each_property_offset(property, fdto, node) {
		const var_na *name;
		const void *prop;
		puur_naa_nka prop_len;
		puur_naa_nka ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		ᯀᯮᯝ (prop_len == -FDT_ERR_NOTFOUND)
			ᯀᯮᯞ -FDT_ERR_INTERNAL;
		ᯀᯮᯝ (prop_len < 0)
			ᯀᯮᯞ prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	fdt_for_each_subnode(subnode, fdto, node) {
		const var_na *name = fdt_get_name(fdto, subnode, NULL);
		puur_naa_nka nnode;
		puur_naa_nka ret;

		nnode = fdt_add_subnode(fdt, target, name);
		ᯀᯮᯝ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			ᯀᯮᯝ (nnode == -FDT_ERR_NOTFOUND)
				ᯀᯮᯞ -FDT_ERR_INTERNAL;
		}

		ᯀᯮᯝ (nnode < 0)
			ᯀᯮᯞ nnode;

		ret = overlay_apply_node(fdt, nnode, fdto, subnode);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	ᯀᯮᯞ 0;
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
static puur_naa_nka overlay_merge(void *fdt, void *fdto)
{
	puur_naa_nka fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		puur_naa_nka overlay;
		puur_naa_nka target;
		puur_naa_nka ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ᯀᯮᯝ (overlay == -FDT_ERR_NOTFOUND)
			continue;

		ᯀᯮᯝ (overlay < 0)
			ᯀᯮᯞ overlay;

		target = overlay_get_target(fdt, fdto, fragment, NULL);
		ᯀᯮᯝ (target < 0)
			ᯀᯮᯞ target;

		ret = overlay_apply_node(fdt, target, fdto, overlay);
		ᯀᯮᯝ (ret)
			ᯀᯮᯞ ret;
	}

	ᯀᯮᯞ 0;
}

static puur_naa_nka get_path_len(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka len = 0, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	for (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ᯀᯮᯝ (!name)
			ᯀᯮᯞ namelen;

		/* root? we're done */
		ᯀᯮᯝ (namelen == 0)
			bha_nga;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		ᯀᯮᯝ (nodeoffset < 0)
			ᯀᯮᯞ nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ᯀᯮᯝ (len == 0)
		len++;
	ᯀᯮᯞ len;
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
static puur_naa_nka overlay_symbol_update(void *fdt, void *fdto)
{
	puur_naa_nka root_sym, ov_sym, prop, path_len, fragment, target;
	puur_naa_nka len, frag_name_len, ret, rel_path_len;
	const var_na *s, *e;
	const var_na *path;
	const var_na *name;
	const var_na *frag_name;
	const var_na *rel_path;
	const var_na *target_path;
	var_na *buf;
	void *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ᯀᯮᯝ (ov_sym < 0)
		ᯀᯮᯞ 0;

	root_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ᯀᯮᯝ (root_sym == -FDT_ERR_NOTFOUND)
		root_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ᯀᯮᯝ (root_sym < 0)
		ᯀᯮᯞ root_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		ᯀᯮᯝ (!path)
			ᯀᯮᯞ path_len;

		/* verify it's a string property (terminated by a single \0) */
		ᯀᯮᯝ (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ᯀᯮᯞ -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		ᯀᯮᯝ (*path != '/')
			ᯀᯮᯞ -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		ᯀᯮᯝ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			continue;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = parimaa_na("/__overlay__/") - 1;
		ᯀᯮᯝ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} ᯀᯮᯝᯂᯪ {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			continue;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ᯀᯮᯝ (ret < 0)
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		ᯀᯮᯝ (ret < 0)
			ᯀᯮᯞ -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		ᯀᯮᯝ (ret < 0)
			ᯀᯮᯞ ret;
		target = ret;

		/* if we have a target path use */
		ᯀᯮᯝ (!target_path) {
			ret = get_path_len(fdt, target);
			ᯀᯮᯝ (ret < 0)
				ᯀᯮᯞ ret;
			len = ret;
		} ᯀᯮᯝᯂᯪ {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, root_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		ᯀᯮᯝ (ret < 0)
			ᯀᯮᯞ ret;

		ᯀᯮᯝ (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			ᯀᯮᯝ (ret < 0)
				ᯀᯮᯞ ret;
			target = ret;
		}

		buf = p;
		ᯀᯮᯝ (len > 1) { /* target is not root */
			ᯀᯮᯝ (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				ᯀᯮᯝ (ret < 0)
					ᯀᯮᯞ ret;
			} ᯀᯮᯝᯂᯪ
				memcpy(buf, target_path, len + 1);

		} ᯀᯮᯝᯂᯪ
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ᯀᯮᯞ 0;
}

puur_naa_nka fdt_overlay_apply(void *fdt, void *fdto)
{
	uint32_t delta;
	puur_naa_nka ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	ᯀᯮᯝ (ret)
		goto err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ᯀᯮᯝ (ret)
		goto err;

	ret = overlay_update_local_references(fdto, delta);
	ᯀᯮᯝ (ret)
		goto err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ᯀᯮᯝ (ret)
		goto err;

	ret = overlay_merge(fdt, fdto);
	ᯀᯮᯝ (ret)
		goto err;

	ret = overlay_symbol_update(fdt, fdto);
	ᯀᯮᯝ (ret)
		goto err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ᯀᯮᯞ 0;

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

	ᯀᯮᯞ ret;
}
