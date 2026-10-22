/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_irqchip.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
সংরচনা sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	পূর্ণাঙ্ক (*raw_handler)(সংরচনা sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	শূন্য *priv;
};

/** Internal irqchip interrupt handler */
সংরচনা sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	সংরচনা sbi_dlist গ্রন্থি;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 অঙ্ক_hwirq;

	/** Write MSI function of this handler */
	শূন্য (*write_msi)(u32 hwirq, ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা, শূন্য *priv);

	/** Callback function of this handler */
	পূর্ণাঙ্ক (*callback)(u32 hwirq, শূন্য *priv);

	/** Callback private data */
	শূন্য *priv;
};

সংরচনা sbi_irqchip_hart_data {
	সংরচনা sbi_irqchip_device *chip;
};

স্থির unsigned long irqchip_hart_data_off;
স্থির SBI_LIST_HEAD(irqchip_list);

পূর্ণাঙ্ক sbi_irqchip_process(শূন্য)
{
	সংরচনা sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	যদি (!hd || !hd->chip || !hd->chip->process_hwirqs)
		প্রত্যাগম SBI_ENODEV;

	প্রত্যাগম hd->chip->process_hwirqs(hd->chip);
}

পূর্ণাঙ্ক sbi_irqchip_process_hwirq(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা sbi_irqchip_hwirq_data *data;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	যদি (!data->raw_handler)
		প্রত্যাগম SBI_ENOENT;

	প্রত্যাগম data->raw_handler(chip, hwirq);
}

স্থির অন্তঃস্থ u32 sbi_irqchip_get_irq_state(সংরচনা sbi_irqchip_device *chip,
					     u32 hwirq)
{
	যদি (!chip || !chip->hwirqs || hwirq >= chip->অঙ্ক_hwirq)
		প্রত্যাগম 0;

	প্রত্যাগম chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(সংরচনা sbi_irqchip_device *chip,
				   u32 hwirq)
{
	প্রত্যাগম !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

পূর্ণাঙ্ক sbi_irqchip_unmask_hwirq(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা sbi_irqchip_hwirq_data *data;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	যদি (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		প্রত্যাগম SBI_EALREADY;

	যদি (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_mask_hwirq(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা sbi_irqchip_hwirq_data *data;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	যদি (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		প্রত্যাগম SBI_EALREADY;

	যদি (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	প্রত্যাগম 0;
}

স্থির সংরচনা sbi_irqchip_handler *sbi_irqchip_find_handler(সংরচনা sbi_irqchip_device *chip,
							    u32 hwirq)
{
	সংরচনা sbi_irqchip_handler *h;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম রিক্ত;

	sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->অঙ্ক_hwirq))
			প্রত্যাগম h;
	}

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক sbi_irqchip_set_hwirq_priv(সংরচনা sbi_irqchip_device *chip, u32 hwirq, শূন্য *priv)
{
	সংরচনা sbi_irqchip_hwirq_data *data;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
	     প্রত্যাগম SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	প্রত্যাগম 0;
}

শূন্য *sbi_irqchip_get_hwirq_priv(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা sbi_irqchip_hwirq_data *data;
	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
	    প্রত্যাগম রিক্ত;

	data = &chip->hwirqs[hwirq];

	প্রত্যাগম data->priv;
}

পূর্ণাঙ্ক sbi_irqchip_raw_handler_default(সংরচনা sbi_irqchip_device *chip, u32 hwirq)
{
	সংরচনা sbi_irqchip_handler *h;
	পূর্ণাঙ্ক rc = SBI_OK;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	যদি (!h)
		প্রত্যাগম SBI_EINVAL;

	যদি (h->callback)
		rc = h->callback(hwirq, h->priv);

	যদি (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	প্রত্যাগম rc;
}

পূর্ণাঙ্ক sbi_irqchip_set_raw_handler(সংরচনা sbi_irqchip_device *chip, u32 hwirq,
				পূর্ণাঙ্ক (*raw_hndl)(সংরচনা sbi_irqchip_device *, u32))
{
	সংরচনা sbi_irqchip_hwirq_data *data;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_write_msi(সংরচনা sbi_irqchip_device *chip, u32 hwirq,
			  ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা)
{
	সংরচনা sbi_irqchip_handler *h;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq || !বার্তা)
		প্রত্যাগম SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	যদি (!h)
		প্রত্যাগম SBI_EFAIL;
	যদি (!h->write_msi)
		প্রত্যাগম SBI_ENOTSUPP;

	h->write_msi(hwirq, বার্তা, h->priv);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_get_affinity(সংরচনা sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		প্রত্যাগম SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	যদি (!sbi_irqchip_find_handler(chip, hwirq))
		প্রত্যাগম SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_set_affinity(সংরচনা sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	সংরচনা sbi_irqchip_hwirq_data *data;
	পূর্ণাঙ্ক rc;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq || sbi_hart_গুনতি() <= hart_index)
		প্রত্যাগম SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	যদি (!sbi_irqchip_find_handler(chip, hwirq))
		প্রত্যাগম SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	যদি (data->hart_index != hart_index) {
		যদি (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			যদি (rc)
				প্রত্যাগম rc;
		}
		data->hart_index = hart_index;
	}

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক __sbi_irqchip_handler_set_affinity(সংরচনা sbi_irqchip_device *chip,
					      সংরচনা sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	পূর্ণাঙ্ক rc;

	প্রতি (i = 0; i < h->অঙ্ক_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		যদি (rc)
			প্রত্যাগম rc;

		যদি (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			অনুবর্ত;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		যদি (rc)
			প্রত্যাগম rc;
	}

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক __sbi_irqchip_register_handler(সংরচনা sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 অঙ্ক_hwirq, u32 hwirq_flags,
					  শূন্য (*write_msi)(u32 hwirq,
							    ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা,
							    শূন্য *priv),
					  পূর্ণাঙ্ক (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv)
{
	সংরচনা sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	পূর্ণাঙ্ক rc;

	প্রতি (i = first_hwirq; i < (first_hwirq + অঙ্ক_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		যদি (h)
			প্রত্যাগম SBI_EALREADY;
	}

	h = sbi_zalloc(পরিমাণ(*h));
	যদি (!h)
		প্রত্যাগম SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->অঙ্ক_hwirq = অঙ্ক_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = রিক্ত;
	sbi_list_for_each_entry(th, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq < th->first_hwirq) {
			nh = th;
			ভঙ্গ;
		}
	}
	যদি (nh)
		sbi_list_add(&h->গ্রন্থি, &nh->গ্রন্থি);
	অন্যথা
		sbi_list_add_tail(&h->গ্রন্থি, &chip->handler_list);

	যদি (chip->hwirq_setup) {
		প্রতি (i = 0; i < h->অঙ্ক_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			যদি (rc) {
				যদি (chip->hwirq_cleanup) {
					প্রতি (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->গ্রন্থি);
				sbi_free(h);
				প্রত্যাগম rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	যদি (rc) {
		যদি (chip->hwirq_cleanup) {
			প্রতি (i = 0; i < h->অঙ্ক_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->গ্রন্থি);
		sbi_free(h);
		প্রত্যাগম rc;
	}

	যদি (chip->hwirq_unmask) {
		প্রতি (i = 0; i < h->অঙ্ক_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_register_msi(সংরচনা sbi_irqchip_device *chip, u32 অঙ্ক_hwirq,
			     শূন্য (*write_msi)(u32 hwirq,
					       ধ্রুব সংরচনা sbi_irqchip_msi_বার্তা *বার্তা,
					       শূন্য *priv),
			     পূর্ণাঙ্ক (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv,
			     u32 *out_first_hwirq)
{
	সংরচনা sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	যদি (!chip || !chip->hwirq_set_affinity || !অঙ্ক_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		প্রত্যাগম SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq < অঙ্ক_hwirq)
		প্রত্যাগম SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->অঙ্ক_hwirq)) {
			hwirq = h->first_hwirq + h->অঙ্ক_hwirq;
		} অন্যথা যদি (hwirq < h->first_hwirq) {
			যদি (h->first_hwirq - hwirq >= অঙ্ক_hwirq) {
				found = true;
				ভঙ্গ;
			} অন্যথা {
				hwirq = h->first_hwirq + h->অঙ্ক_hwirq;
			}
		}
	}
	যদি (!found && (chip->অঙ্ক_hwirq - hwirq) >= অঙ্ক_hwirq)
		found = true;
	যদি (!found)
		প্রত্যাগম SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	প্রত্যাগম __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      অঙ্ক_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

পূর্ণাঙ্ক sbi_irqchip_register_handler(সংরচনা sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 অঙ্ক_hwirq, u32 hwirq_flags,
				 পূর্ণাঙ্ক (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv)
{
	যদি (!chip || !অঙ্ক_hwirq || !callback)
		প্রত্যাগম SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		প্রত্যাগম SBI_EBAD_RANGE;

	প্রত্যাগম __sbi_irqchip_register_handler(chip, first_hwirq, অঙ্ক_hwirq, hwirq_flags,
					      রিক্ত, callback, priv);
}

পূর্ণাঙ্ক sbi_irqchip_register_reserved(সংরচনা sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 অঙ্ক_hwirq)
{
	যদি (!chip || !অঙ্ক_hwirq)
		প্রত্যাগম SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		প্রত্যাগম SBI_EBAD_RANGE;

	প্রত্যাগম __sbi_irqchip_register_handler(chip, first_hwirq, অঙ্ক_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, রিক্ত, রিক্ত, রিক্ত);
}

পূর্ণাঙ্ক sbi_irqchip_unregister_handler(সংরচনা sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 অঙ্ক_hwirq)
{
	সংরচনা sbi_irqchip_handler *fh, *lh;
	u32 i;

	যদি (!chip || !অঙ্ক_hwirq)
		প্রত্যাগম SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		প্রত্যাগম SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	যদি (!fh || fh->first_hwirq != first_hwirq || fh->অঙ্ক_hwirq != অঙ্ক_hwirq)
		প্রত্যাগম SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + অঙ্ক_hwirq - 1);
	যদি (!lh || lh != fh)
		প্রত্যাগম SBI_ENODEV;

	যদি (chip->hwirq_mask) {
		প্রতি (i = 0; i < fh->অঙ্ক_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	যদি (chip->hwirq_cleanup) {
		প্রতি (i = 0; i < fh->অঙ্ক_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->গ্রন্থি);
	প্রত্যাগম 0;
}

সংরচনা sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   সংরচনা sbi_irqchip_device *first)
{
	সংরচনা sbi_irqchip_device *chip;
	bool found = (first == রিক্ত);

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (!found) {
			যদি (first == chip)
				found = true;
			অনুবর্ত;
		}
		যদি ((chip->caps & caps) == caps)
			প্রত্যাগম chip;
	}

	প্রত্যাগম রিক্ত;
}

সংরচনা sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	সংরচনা sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (chip->id == id)
			প্রত্যাগম chip;
	}

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক sbi_irqchip_add_device(সংরচনা sbi_irqchip_device *chip)
{
	সংরচনা sbi_irqchip_hart_data *hd;
	সংরচনা sbi_scratch *scratch;
	u32 i, h;

	যদি (!chip || !chip->অঙ্ক_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		প্রত্যাগম SBI_EINVAL;

	যদি (sbi_irqchip_find_device(chip->id))
		প্রত্যাগম SBI_EALREADY;

	যদি (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			যদি (!scratch)
				অনুবর্ত;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			যদি (hd->chip && hd->chip != chip)
				প্রত্যাগম SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(পরিমাণ(*chip->hwirqs) * chip->অঙ্ক_hwirq);
	যদি (!chip->hwirqs)
		প্রত্যাগম SBI_ENOMEM;
	প্রতি (i = 0; i < chip->অঙ্ক_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->গ্রন্থি, &irqchip_list);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক sbi_irqchip_init(সংরচনা sbi_scratch *scratch, bool cold_boot)
{
	ধ্রুব সংরচনা sbi_platform *plat = sbi_platform_ptr(scratch);
	সংরচনা sbi_irqchip_hart_data *hd;
	সংরচনা sbi_irqchip_device *chip;
	পূর্ণাঙ্ক rc;

	যদি (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(পরিমাণ(সংরচনা sbi_irqchip_hart_data));
		যদি (!irqchip_hart_data_off)
			প্রত্যাগম SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		যদি (rc)
			প্রত্যাগম rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (!chip->warm_init)
			অনুবর্ত;
		যদি (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			অনুবর্ত;
		rc = chip->warm_init(chip);
		যদি (rc)
			প্রত্যাগম rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	যদি (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	প্রত্যাগম 0;
}

শূন্য sbi_irqchip_exit(সংরচনা sbi_scratch *scratch)
{
	সংরচনা sbi_irqchip_hart_data *hd;
	সংরচনা sbi_irqchip_device *chip;
	সংরচনা sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	পূর্ণাঙ্ক rc;

	sbi_for_each_hartindex(i) {
		যদি (i == current_hartindex())
			অনুবর্ত;
		যদি (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			অনুবর্ত;
		migrate_hidx = i;
		migrate = true;
		ভঙ্গ;
	}

	যদি (!migrate)
		গচ্ছ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			যদি (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	যদি (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
