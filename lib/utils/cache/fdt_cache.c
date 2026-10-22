/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>

/* List of FDT cache drivers generated at compile time */
બાહ્ય અચલ સંરચના fdt_driver *અચલ fdt_cache_drivers[];

પૂર્ણાંક fdt_cache_add(અચલ શૂન્ય *fdt, પૂર્ણાંક noff, સંરચના cache_device *dev)
{
	પૂર્ણાંક rc;

	dev->id = noff;
	sbi_strncpy(dev->name, fdt_get_name(fdt, noff, ખાલી), કદ(dev->name) - 1);
	sbi_dprintf("%s: %s\n", __func__, dev->name);

	rc = fdt_next_cache_get(fdt, noff, &dev->next);
	જો (rc == SBI_ENOENT)
		dev->next = ખાલી;
	નહીંતર જો (rc)
		પાછા rc;

	પાછા cache_add(dev);
}

સ્થિર પૂર્ણાંક fdt_cache_add_generic(અચલ શૂન્ય *fdt, પૂર્ણાંક noff)
{
	સંરચના cache_device *dev;
	પૂર્ણાંક rc;

	dev = sbi_zalloc(કદ(*dev));
	જો (!dev)
		પાછા SBI_ENOMEM;

	rc = fdt_cache_add(fdt, noff, dev);
	જો (rc) {
		sbi_free(dev);
		પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_cache_find(અચલ શૂન્ય *fdt, પૂર્ણાંક noff, સંરચના cache_device **out_dev)
{
	સંરચના cache_device *dev = cache_find(noff);
	પૂર્ણાંક rc;

	જો (!dev) {
		rc = fdt_driver_init_by_offset(fdt, noff, fdt_cache_drivers);
		જો (rc == SBI_ENODEV)
			rc = fdt_cache_add_generic(fdt, noff);
		જો (rc)
			પાછા rc;

		dev = cache_find(noff);
		જો (!dev)
			પાછા SBI_EFAIL;
	}

	જો (out_dev)
		*out_dev = dev;

	પાછા SBI_OK;
}

પૂર્ણાંક fdt_next_cache_get(અચલ શૂન્ય *fdt, પૂર્ણાંક noff, સંરચના cache_device **out_dev)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક len;

	val = fdt_getprop(fdt, noff, "next-level-cache", &len);
	જો (!val || len < કદ(*val))
		પાછા SBI_ENOENT;

	noff = fdt_ગાંઠ_offset_by_phandle(fdt, fdt32_to_cpu(val[0]));
	જો (noff < 0)
		પાછા noff;

	પાછા fdt_cache_find(fdt, noff, out_dev);
}
