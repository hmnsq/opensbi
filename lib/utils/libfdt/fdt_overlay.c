// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

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
स्थिर uint32_t overlay_get_target_phandle(ध्रुव शून्य *fdto, पूर्णाङ्क fragment)
{
	ध्रुव fdt32_t *val;
	पूर्णाङ्क len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	सचे (!val)
		पटिनिवत्त 0;

	सचे ((len != परिमाण(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		पटिनिवत्त (uint32_t)-1;

	पटिनिवत्त fdt32_to_cpu(*val);
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
स्थिर पूर्णाङ्क overlay_get_target(ध्रुव शून्य *fdt, ध्रुव शून्य *fdto,
			      पूर्णाङ्क fragment, वर्ण ध्रुव **pathp)
{
	uint32_t phandle;
	ध्रुव वर्ण *path = रिक्त;
	पूर्णाङ्क path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	सचे (phandle == (uint32_t)-1)
		पटिनिवत्त -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	सचे (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		सचे (path)
			ret = fdt_path_offset(fdt, path);
		अञ्ञथा
			ret = path_len;
	} अञ्ञथा
		ret = fdt_ग्रन्थिः_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	सचे (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	सचे (ret < 0)
		पटिनिवत्त ret;

	/* return pointer to path (if available) */
	सचे (pathp)
		*pathp = path ? path : रिक्त;

	पटिनिवत्त ret;
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
स्थिर पूर्णाङ्क overlay_phandle_add_offset(शून्य *fdt, पूर्णाङ्क ग्रन्थिः,
				      ध्रुव वर्ण *name, uint32_t delta)
{
	ध्रुव fdt32_t *val;
	uint32_t adj_val;
	पूर्णाङ्क len;

	val = fdt_getprop(fdt, ग्रन्थिः, name, &len);
	सचे (!val)
		पटिनिवत्त len;

	सचे (len != परिमाण(*val))
		पटिनिवत्त -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	सचे ((adj_val + delta) < adj_val)
		पटिनिवत्त -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	सचे (adj_val == (uint32_t)-1)
		पटिनिवत्त -FDT_ERR_NOPHANDLES;

	पटिनिवत्त fdt_setprop_inplace_u32(fdt, ग्रन्थिः, name, adj_val);
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
स्थिर पूर्णाङ्क overlay_adjust_ग्रन्थिः_phandles(शून्य *fdto, पूर्णाङ्क ग्रन्थिः,
					uint32_t delta)
{
	पूर्णाङ्क child;
	पूर्णाङ्क ret;

	ret = overlay_phandle_add_offset(fdto, ग्रन्थिः, "phandle", delta);
	सचे (ret && ret != -FDT_ERR_NOTFOUND)
		पटिनिवत्त ret;

	ret = overlay_phandle_add_offset(fdto, ग्रन्थिः, "linux,phandle", delta);
	सचे (ret && ret != -FDT_ERR_NOTFOUND)
		पटिनिवत्त ret;

	fdt_for_each_subnode(child, fdto, ग्रन्थिः) {
		ret = overlay_adjust_ग्रन्थिः_phandles(fdto, child, delta);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
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
स्थिर पूर्णाङ्क overlay_adjust_local_phandles(शून्य *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	पटिनिवत्त overlay_adjust_ग्रन्थिः_phandles(fdto, 0, delta);
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
स्थिर पूर्णाङ्क overlay_update_local_ग्रन्थिः_references(शून्य *fdto,
						पूर्णाङ्क वृक्ष_ग्रन्थिः,
						पूर्णाङ्क fixup_ग्रन्थिः,
						uint32_t delta)
{
	पूर्णाङ्क fixup_prop;
	पूर्णाङ्क fixup_child;
	पूर्णाङ्क ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_ग्रन्थिः) {
		ध्रुव fdt32_t *fixup_val;
		ध्रुव वर्ण *वृक्षः_val;
		ध्रुव वर्ण *name;
		पूर्णाङ्क fixup_len;
		पूर्णाङ्क वृक्षः_len;
		पूर्णाङ्क i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		सचे (!fixup_val)
			पटिनिवत्त fixup_len;

		सचे (fixup_len % परिमाण(uint32_t))
			पटिनिवत्त -FDT_ERR_BADOVERLAY;
		fixup_len /= परिमाण(uint32_t);

		वृक्षः_val = fdt_getprop(fdto, वृक्ष_ग्रन्थिः, name, &वृक्षः_len);
		सचे (!वृक्षः_val) {
			सचे (वृक्षः_len == -FDT_ERR_NOTFOUND)
				पटिनिवत्त -FDT_ERR_BADOVERLAY;

			पटिनिवत्त वृक्षः_len;
		}

		अत्थाय (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, वृक्षः_val + poffset, परिमाण(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  वृक्ष_ग्रन्थिः,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  परिमाण(adj_val));
			सचे (ret == -FDT_ERR_NOSPACE)
				पटिनिवत्त -FDT_ERR_BADOVERLAY;

			सचे (ret)
				पटिनिवत्त ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_ग्रन्थिः) {
		ध्रुव वर्ण *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    रिक्त);
		पूर्णाङ्क वृक्षः_child;

		वृक्षः_child = fdt_subnode_offset(fdto, वृक्ष_ग्रन्थिः,
						fixup_child_name);
		सचे (वृक्षः_child == -FDT_ERR_NOTFOUND)
			पटिनिवत्त -FDT_ERR_BADOVERLAY;
		सचे (वृक्षः_child < 0)
			पटिनिवत्त वृक्षः_child;

		ret = overlay_update_local_ग्रन्थिः_references(fdto,
							   वृक्षः_child,
							   fixup_child,
							   delta);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
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
स्थिर पूर्णाङ्क overlay_update_local_references(शून्य *fdto, uint32_t delta)
{
	पूर्णाङ्क fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	सचे (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		सचे (fixups == -FDT_ERR_NOTFOUND)
			पटिनिवत्त 0;

		पटिनिवत्त fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	पटिनिवत्त overlay_update_local_ग्रन्थिः_references(fdto, 0, fixups,
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
स्थिर पूर्णाङ्क overlay_fixup_one_phandle(शून्य *fdt, शून्य *fdto,
				     पूर्णाङ्क symbols_off,
				     ध्रुव वर्ण *path, uint32_t path_len,
				     ध्रुव वर्ण *name, uint32_t name_len,
				     पूर्णाङ्क poffset, ध्रुव वर्ण *label)
{
	ध्रुव वर्ण *चिह्नम्_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	पूर्णाङ्क चिह्नम्_off, fixup_off;
	पूर्णाङ्क prop_len;

	सचे (symbols_off < 0)
		पटिनिवत्त symbols_off;

	चिह्नम्_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	सचे (!चिह्नम्_path)
		पटिनिवत्त prop_len;

	चिह्नम्_off = fdt_path_offset(fdt, चिह्नम्_path);
	सचे (चिह्नम्_off < 0)
		पटिनिवत्त चिह्नम्_off;

	phandle = fdt_get_phandle(fdt, चिह्नम्_off);
	सचे (!phandle)
		पटिनिवत्त -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	सचे (fixup_off == -FDT_ERR_NOTFOUND)
		पटिनिवत्त -FDT_ERR_BADOVERLAY;
	सचे (fixup_off < 0)
		पटिनिवत्त fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	पटिनिवत्त fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   परिमाण(phandle_prop));
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
स्थिर पूर्णाङ्क overlay_fixup_phandle(शून्य *fdt, शून्य *fdto, पूर्णाङ्क symbols_off,
				 पूर्णाङ्क property)
{
	ध्रुव वर्ण *मानम्;
	ध्रुव वर्ण *label;
	पूर्णाङ्क len;

	मानम् = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	सचे (!मानम्) {
		सचे (len == -FDT_ERR_NOTFOUND)
			पटिनिवत्त -FDT_ERR_INTERNAL;

		पटिनिवत्त len;
	}

	करोहि {
		ध्रुव वर्ण *path, *name, *fixup_end;
		ध्रुव वर्ण *fixup_str = मानम्;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		वर्ण *sep, *endptr;
		पूर्णाङ्क poffset, ret;

		fixup_end = memchr(मानम्, '\0', len);
		सचे (!fixup_end)
			पटिनिवत्त -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		मानम् += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		सचे (!sep || *sep != ':')
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		सचे (path_len == (fixup_len - 1))
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		सचे (!sep || *sep != ':')
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		सचे (!name_len)
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		सचे ((*endptr != '\0') || (endptr <= (sep + 1)))
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		सचे (ret)
			पटिनिवत्त ret;
	} याव (len > 0);

	पटिनिवत्त 0;
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
स्थिर पूर्णाङ्क overlay_fixup_phandles(शून्य *fdt, शून्य *fdto)
{
	पूर्णाङ्क fixups_off, symbols_off;
	पूर्णाङ्क property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	सचे (fixups_off == -FDT_ERR_NOTFOUND)
		पटिनिवत्त 0; /* nothing to do */
	सचे (fixups_off < 0)
		पटिनिवत्त fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	सचे ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		पटिनिवत्त symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		पूर्णाङ्क ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
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
स्थिर पूर्णाङ्क overlay_apply_ग्रन्थिः(शून्य *fdt, पूर्णाङ्क target,
			      शून्य *fdto, पूर्णाङ्क ग्रन्थिः)
{
	पूर्णाङ्क property;
	पूर्णाङ्क subnode;

	fdt_for_each_property_offset(property, fdto, ग्रन्थिः) {
		ध्रुव वर्ण *name;
		ध्रुव शून्य *prop;
		पूर्णाङ्क prop_len;
		पूर्णाङ्क ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		सचे (prop_len == -FDT_ERR_NOTFOUND)
			पटिनिवत्त -FDT_ERR_INTERNAL;
		सचे (prop_len < 0)
			पटिनिवत्त prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		सचे (ret)
			पटिनिवत्त ret;
	}

	fdt_for_each_subnode(subnode, fdto, ग्रन्थिः) {
		ध्रुव वर्ण *name = fdt_get_name(fdto, subnode, रिक्त);
		पूर्णाङ्क nnode;
		पूर्णाङ्क ret;

		nnode = fdt_add_subnode(fdt, target, name);
		सचे (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			सचे (nnode == -FDT_ERR_NOTFOUND)
				पटिनिवत्त -FDT_ERR_INTERNAL;
		}

		सचे (nnode < 0)
			पटिनिवत्त nnode;

		ret = overlay_apply_ग्रन्थिः(fdt, nnode, fdto, subnode);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
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
स्थिर पूर्णाङ्क overlay_merge(शून्य *fdt, शून्य *fdto)
{
	पूर्णाङ्क fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		पूर्णाङ्क overlay;
		पूर्णाङ्क target;
		पूर्णाङ्क ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		सचे (overlay == -FDT_ERR_NOTFOUND)
			अनुवर्त;

		सचे (overlay < 0)
			पटिनिवत्त overlay;

		target = overlay_get_target(fdt, fdto, fragment, रिक्त);
		सचे (target < 0)
			पटिनिवत्त target;

		ret = overlay_apply_ग्रन्थिः(fdt, target, fdto, overlay);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क get_path_len(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क len = 0, namelen;
	ध्रुव वर्ण *name;

	FDT_RO_PROBE(fdt);

	अत्थाय (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		सचे (!name)
			पटिनिवत्त namelen;

		/* root? we're done */
		सचे (namelen == 0)
			भङ्ग;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		सचे (nodeoffset < 0)
			पटिनिवत्त nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	सचे (len == 0)
		len++;
	पटिनिवत्त len;
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
स्थिर पूर्णाङ्क overlay_चिह्नम्_update(शून्य *fdt, शून्य *fdto)
{
	पूर्णाङ्क मूलम्_sym, ov_sym, prop, path_len, fragment, target;
	पूर्णाङ्क len, frag_name_len, ret, rel_path_len;
	ध्रुव वर्ण *s, *e;
	ध्रुव वर्ण *path;
	ध्रुव वर्ण *name;
	ध्रुव वर्ण *frag_name;
	ध्रुव वर्ण *rel_path;
	ध्रुव वर्ण *target_path;
	वर्ण *buf;
	शून्य *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	सचे (ov_sym < 0)
		पटिनिवत्त 0;

	मूलम्_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	सचे (मूलम्_sym == -FDT_ERR_NOTFOUND)
		मूलम्_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	सचे (मूलम्_sym < 0)
		पटिनिवत्त मूलम्_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		सचे (!path)
			पटिनिवत्त path_len;

		/* verify it's a string property (terminated by a single \0) */
		सचे (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			पटिनिवत्त -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		सचे (*path != '/')
			पटिनिवत्त -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		सचे (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			अनुवर्त;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = परिमाण("/__overlay__/") - 1;
		सचे ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} अञ्ञथा सचे ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} अञ्ञथा {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			अनुवर्त;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		सचे (ret < 0)
			पटिनिवत्त -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		सचे (ret < 0)
			पटिनिवत्त -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		सचे (ret < 0)
			पटिनिवत्त ret;
		target = ret;

		/* if we have a target path use */
		सचे (!target_path) {
			ret = get_path_len(fdt, target);
			सचे (ret < 0)
				पटिनिवत्त ret;
			len = ret;
		} अञ्ञथा {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, मूलम्_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		सचे (ret < 0)
			पटिनिवत्त ret;

		सचे (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			सचे (ret < 0)
				पटिनिवत्त ret;
			target = ret;
		}

		buf = p;
		सचे (len > 1) { /* target is not root */
			सचे (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				सचे (ret < 0)
					पटिनिवत्त ret;
			} अञ्ञथा
				memcpy(buf, target_path, len + 1);

		} अञ्ञथा
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क fdt_overlay_apply(शून्य *fdt, शून्य *fdto)
{
	uint32_t delta;
	पूर्णाङ्क ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	सचे (ret)
		गच्छ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	सचे (ret)
		गच्छ err;

	ret = overlay_update_local_references(fdto, delta);
	सचे (ret)
		गच्छ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	सचे (ret)
		गच्छ err;

	ret = overlay_merge(fdt, fdto);
	सचे (ret)
		गच्छ err;

	ret = overlay_चिह्नम्_update(fdt, fdto);
	सचे (ret)
		गच्छ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	पटिनिवत्त 0;

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

	पटिनिवत्त ret;
}
