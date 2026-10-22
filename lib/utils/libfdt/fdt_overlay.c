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
	यदि (!val)
		प्रत्यागम 0;

	यदि ((len != परिमाण(*val)) || (fdt32_गन्तव्यम्_cpu(*val) == (uint32_t)-1))
		प्रत्यागम (uint32_t)-1;

	प्रत्यागम fdt32_गन्तव्यम्_cpu(*val);
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
	ध्रुव वर्ण *मार्गः = रिक्त;
	पूर्णाङ्क मार्गः_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	यदि (phandle == (uint32_t)-1)
		प्रत्यागम -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	यदि (!phandle) {
		/* And then a path based lookup */
		मार्गः = fdt_getprop(fdto, fragment, "target-path", &मार्गः_len);
		यदि (मार्गः)
			ret = fdt_मार्गः_विस्थापनम्(fdt, मार्गः);
		अन्यथा
			ret = मार्गः_len;
	} अन्यथा
		ret = fdt_ग्रन्थिः_विस्थापनम्_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	यदि (ret < 0 && मार्गः_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	यदि (ret < 0)
		प्रत्यागम ret;

	/* return pointer to path (if available) */
	यदि (pathp)
		*pathp = मार्गः ? मार्गः : रिक्त;

	प्रत्यागम ret;
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
स्थिर पूर्णाङ्क overlay_phandle_सङ्कलय_विस्थापनम्(शून्य *fdt, पूर्णाङ्क ग्रन्थिः,
				      ध्रुव वर्ण *name, uint32_t delta)
{
	ध्रुव fdt32_t *val;
	uint32_t adj_val;
	पूर्णाङ्क len;

	val = fdt_getprop(fdt, ग्रन्थिः, name, &len);
	यदि (!val)
		प्रत्यागम len;

	यदि (len != परिमाण(*val))
		प्रत्यागम -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_गन्तव्यम्_cpu(*val);
	यदि ((adj_val + delta) < adj_val)
		प्रत्यागम -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	यदि (adj_val == (uint32_t)-1)
		प्रत्यागम -FDT_ERR_NOPHANDLES;

	प्रत्यागम fdt_setprop_inplace_u32(fdt, ग्रन्थिः, name, adj_val);
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

	ret = overlay_phandle_सङ्कलय_विस्थापनम्(fdto, ग्रन्थिः, "phandle", delta);
	यदि (ret && ret != -FDT_ERR_NOTFOUND)
		प्रत्यागम ret;

	ret = overlay_phandle_सङ्कलय_विस्थापनम्(fdto, ग्रन्थिः, "linux,phandle", delta);
	यदि (ret && ret != -FDT_ERR_NOTFOUND)
		प्रत्यागम ret;

	fdt_for_each_subnode(child, fdto, ग्रन्थिः) {
		ret = overlay_adjust_ग्रन्थिः_phandles(fdto, child, delta);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
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
	प्रत्यागम overlay_adjust_ग्रन्थिः_phandles(fdto, 0, delta);
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

	fdt_for_each_property_विस्थापनम्(fixup_prop, fdto, fixup_ग्रन्थिः) {
		ध्रुव fdt32_t *fixup_val;
		ध्रुव वर्ण *वृक्षः_val;
		ध्रुव वर्ण *name;
		पूर्णाङ्क fixup_len;
		पूर्णाङ्क वृक्षः_len;
		पूर्णाङ्क i;

		fixup_val = fdt_getprop_by_विस्थापनम्(fdto, fixup_prop,
						  &name, &fixup_len);
		यदि (!fixup_val)
			प्रत्यागम fixup_len;

		यदि (fixup_len % परिमाण(uint32_t))
			प्रत्यागम -FDT_ERR_BADOVERLAY;
		fixup_len /= परिमाण(uint32_t);

		वृक्षः_val = fdt_getprop(fdto, वृक्ष_ग्रन्थिः, name, &वृक्षः_len);
		यदि (!वृक्षः_val) {
			यदि (वृक्षः_len == -FDT_ERR_NOTFOUND)
				प्रत्यागम -FDT_ERR_BADOVERLAY;

			प्रत्यागम वृक्षः_len;
		}

		प्रति (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_गन्तव्यम्_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			स्मृतिं_प्रतिलिख(&adj_val, वृक्षः_val + poffset, परिमाण(adj_val));

			adj_val = cpu_गन्तव्यम्_fdt32(fdt32_गन्तव्यम्_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  वृक्ष_ग्रन्थिः,
								  name,
								  वाक्यदैर्घ्यम्(name),
								  poffset,
								  &adj_val,
								  परिमाण(adj_val));
			यदि (ret == -FDT_ERR_NOSPACE)
				प्रत्यागम -FDT_ERR_BADOVERLAY;

			यदि (ret)
				प्रत्यागम ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_ग्रन्थिः) {
		ध्रुव वर्ण *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    रिक्त);
		पूर्णाङ्क वृक्षः_child;

		वृक्षः_child = fdt_subnode_विस्थापनम्(fdto, वृक्ष_ग्रन्थिः,
						fixup_child_name);
		यदि (वृक्षः_child == -FDT_ERR_NOTFOUND)
			प्रत्यागम -FDT_ERR_BADOVERLAY;
		यदि (वृक्षः_child < 0)
			प्रत्यागम वृक्षः_child;

		ret = overlay_update_local_ग्रन्थिः_references(fdto,
							   वृक्षः_child,
							   fixup_child,
							   delta);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
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

	fixups = fdt_मार्गः_विस्थापनम्(fdto, "/__local_fixups__");
	यदि (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		यदि (fixups == -FDT_ERR_NOTFOUND)
			प्रत्यागम 0;

		प्रत्यागम fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	प्रत्यागम overlay_update_local_ग्रन्थिः_references(fdto, 0, fixups,
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
				     ध्रुव वर्ण *मार्गः, uint32_t मार्गः_len,
				     ध्रुव वर्ण *name, uint32_t name_len,
				     पूर्णाङ्क poffset, ध्रुव वर्ण *label)
{
	ध्रुव वर्ण *चिह्नम्_मार्गः;
	uint32_t phandle;
	fdt32_t phandle_prop;
	पूर्णाङ्क चिह्नम्_off, fixup_off;
	पूर्णाङ्क prop_len;

	यदि (symbols_off < 0)
		प्रत्यागम symbols_off;

	चिह्नम्_मार्गः = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	यदि (!चिह्नम्_मार्गः)
		प्रत्यागम prop_len;

	चिह्नम्_off = fdt_मार्गः_विस्थापनम्(fdt, चिह्नम्_मार्गः);
	यदि (चिह्नम्_off < 0)
		प्रत्यागम चिह्नम्_off;

	phandle = fdt_get_phandle(fdt, चिह्नम्_off);
	यदि (!phandle)
		प्रत्यागम -FDT_ERR_NOTFOUND;

	fixup_off = fdt_मार्गः_विस्थापनम्_namelen(fdto, मार्गः, मार्गः_len);
	यदि (fixup_off == -FDT_ERR_NOTFOUND)
		प्रत्यागम -FDT_ERR_BADOVERLAY;
	यदि (fixup_off < 0)
		प्रत्यागम fixup_off;

	phandle_prop = cpu_गन्तव्यम्_fdt32(phandle);
	प्रत्यागम fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
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

	मानम् = fdt_getprop_by_विस्थापनम्(fdto, property,
				      &label, &len);
	यदि (!मानम्) {
		यदि (len == -FDT_ERR_NOTFOUND)
			प्रत्यागम -FDT_ERR_आन्तरिकम्;

		प्रत्यागम len;
	}

	कुरु {
		ध्रुव वर्ण *मार्गः, *name, *fixup_अन्तः;
		ध्रुव वर्ण *fixup_str = मानम्;
		uint32_t मार्गः_len, name_len;
		uint32_t fixup_len;
		वर्ण *sep, *endptr;
		पूर्णाङ्क poffset, ret;

		fixup_अन्तः = memchr(मानम्, '\0', len);
		यदि (!fixup_अन्तः)
			प्रत्यागम -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_अन्तः - fixup_str;

		len -= fixup_len + 1;
		मानम् += fixup_len + 1;

		मार्गः = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		यदि (!sep || *sep != ':')
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		मार्गः_len = sep - मार्गः;
		यदि (मार्गः_len == (fixup_len - 1))
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		fixup_len -= मार्गः_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		यदि (!sep || *sep != ':')
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		यदि (!name_len)
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		यदि ((*endptr != '\0') || (endptr <= (sep + 1)))
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						मार्गः, मार्गः_len, name, name_len,
						poffset, label);
		यदि (ret)
			प्रत्यागम ret;
	} यावत् (len > 0);

	प्रत्यागम 0;
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
	fixups_off = fdt_मार्गः_विस्थापनम्(fdto, "/__fixups__");
	यदि (fixups_off == -FDT_ERR_NOTFOUND)
		प्रत्यागम 0; /* nothing to do */
	यदि (fixups_off < 0)
		प्रत्यागम fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_मार्गः_विस्थापनम्(fdt, "/__symbols__");
	यदि ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		प्रत्यागम symbols_off;

	fdt_for_each_property_विस्थापनम्(property, fdto, fixups_off) {
		पूर्णाङ्क ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
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

	fdt_for_each_property_विस्थापनम्(property, fdto, ग्रन्थिः) {
		ध्रुव वर्ण *name;
		ध्रुव शून्य *prop;
		पूर्णाङ्क prop_len;
		पूर्णाङ्क ret;

		prop = fdt_getprop_by_विस्थापनम्(fdto, property, &name,
					     &prop_len);
		यदि (prop_len == -FDT_ERR_NOTFOUND)
			प्रत्यागम -FDT_ERR_आन्तरिकम्;
		यदि (prop_len < 0)
			प्रत्यागम prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		यदि (ret)
			प्रत्यागम ret;
	}

	fdt_for_each_subnode(subnode, fdto, ग्रन्थिः) {
		ध्रुव वर्ण *name = fdt_get_name(fdto, subnode, रिक्त);
		पूर्णाङ्क nnode;
		पूर्णाङ्क ret;

		nnode = fdt_सङ्कलय_subnode(fdt, target, name);
		यदि (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_विस्थापनम्(fdt, target, name);
			यदि (nnode == -FDT_ERR_NOTFOUND)
				प्रत्यागम -FDT_ERR_आन्तरिकम्;
		}

		यदि (nnode < 0)
			प्रत्यागम nnode;

		ret = overlay_apply_ग्रन्थिः(fdt, nnode, fdto, subnode);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
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
		overlay = fdt_subnode_विस्थापनम्(fdto, fragment, "__overlay__");
		यदि (overlay == -FDT_ERR_NOTFOUND)
			अनुवर्त;

		यदि (overlay < 0)
			प्रत्यागम overlay;

		target = overlay_get_target(fdt, fdto, fragment, रिक्त);
		यदि (target < 0)
			प्रत्यागम target;

		ret = overlay_apply_ग्रन्थिः(fdt, target, fdto, overlay);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क get_मार्गः_len(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoffset)
{
	पूर्णाङ्क len = 0, namelen;
	ध्रुव वर्ण *name;

	FDT_RO_PROBE(fdt);

	प्रति (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		यदि (!name)
			प्रत्यागम namelen;

		/* root? we're done */
		यदि (namelen == 0)
			भङ्ग;

		nodeoffset = fdt_parent_विस्थापनम्(fdt, nodeoffset);
		यदि (nodeoffset < 0)
			प्रत्यागम nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	यदि (len == 0)
		len++;
	प्रत्यागम len;
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
	पूर्णाङ्क मूलम्_sym, ov_sym, prop, मार्गः_len, fragment, target;
	पूर्णाङ्क len, frag_name_len, ret, rel_मार्गः_len;
	ध्रुव वर्ण *s, *e;
	ध्रुव वर्ण *मार्गः;
	ध्रुव वर्ण *name;
	ध्रुव वर्ण *frag_name;
	ध्रुव वर्ण *rel_मार्गः;
	ध्रुव वर्ण *target_मार्गः;
	वर्ण *buf;
	शून्य *p;

	ov_sym = fdt_subnode_विस्थापनम्(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	यदि (ov_sym < 0)
		प्रत्यागम 0;

	मूलम्_sym = fdt_subnode_विस्थापनम्(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	यदि (मूलम्_sym == -FDT_ERR_NOTFOUND)
		मूलम्_sym = fdt_सङ्कलय_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	यदि (मूलम्_sym < 0)
		प्रत्यागम मूलम्_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_विस्थापनम्(prop, fdto, ov_sym) {
		मार्गः = fdt_getprop_by_विस्थापनम्(fdto, prop, &name, &मार्गः_len);
		यदि (!मार्गः)
			प्रत्यागम मार्गः_len;

		/* verify it's a string property (terminated by a single \0) */
		यदि (मार्गः_len < 1 || memchr(मार्गः, '\0', मार्गः_len) != &मार्गः[मार्गः_len - 1])
			प्रत्यागम -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = मार्गः + मार्गः_len;

		यदि (*मार्गः != '/')
			प्रत्यागम -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(मार्गः + 1, '/');
		यदि (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			अनुवर्त;
		}

		frag_name = मार्गः + 1;
		frag_name_len = s - मार्गः - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = परिमाण("/__overlay__/") - 1;
		यदि ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_मार्गः = s + len;
			rel_मार्गः_len = e - rel_मार्गः - 1;
		} अन्यथा यदि ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_मार्गः = "";
			rel_मार्गः_len = 0;
		} अन्यथा {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			अनुवर्त;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_विस्थापनम्_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		यदि (ret < 0)
			प्रत्यागम -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_विस्थापनम्(fdto, fragment, "__overlay__");
		यदि (ret < 0)
			प्रत्यागम -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_मार्गः);
		यदि (ret < 0)
			प्रत्यागम ret;
		target = ret;

		/* if we have a target path use */
		यदि (!target_मार्गः) {
			ret = get_मार्गः_len(fdt, target);
			यदि (ret < 0)
				प्रत्यागम ret;
			len = ret;
		} अन्यथा {
			len = वाक्यदैर्घ्यम्(target_मार्गः);
		}

		ret = fdt_setprop_placeholder(fdt, मूलम्_sym, name,
				len + (len > 1) + rel_मार्गः_len + 1, &p);
		यदि (ret < 0)
			प्रत्यागम ret;

		यदि (!target_मार्गः) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_मार्गः);
			यदि (ret < 0)
				प्रत्यागम ret;
			target = ret;
		}

		buf = p;
		यदि (len > 1) { /* target is not root */
			यदि (!target_मार्गः) {
				ret = fdt_get_मार्गः(fdt, target, buf, len + 1);
				यदि (ret < 0)
					प्रत्यागम ret;
			} अन्यथा
				स्मृतिं_प्रतिलिख(buf, target_मार्गः, len + 1);

		} अन्यथा
			len--;

		buf[len] = '/';
		स्मृतिं_प्रतिलिख(buf + len + 1, rel_मार्गः, rel_मार्गः_len);
		buf[len + 1 + rel_मार्गः_len] = '\0';
	}

	प्रत्यागम 0;
}

पूर्णाङ्क fdt_overlay_apply(शून्य *fdt, शून्य *fdto)
{
	uint32_t delta;
	पूर्णाङ्क ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	यदि (ret)
		गच्छ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	यदि (ret)
		गच्छ err;

	ret = overlay_update_local_references(fdto, delta);
	यदि (ret)
		गच्छ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	यदि (ret)
		गच्छ err;

	ret = overlay_merge(fdt, fdto);
	यदि (ret)
		गच्छ err;

	ret = overlay_चिह्नम्_update(fdt, fdto);
	यदि (ret)
		गच्छ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_समुच्चयः_magic(fdto, ~0);

	प्रत्यागम 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_समुच्चयः_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_समुच्चयः_magic(fdt, ~0);

	प्रत्यागम ret;
}
