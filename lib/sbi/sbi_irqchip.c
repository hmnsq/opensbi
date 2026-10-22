/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_irqchip.h>
#அந்தர்பாவ <sbi/sbi_list.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
கட்டமைப்பு sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	முழு (*raw_handler)(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	வெறுமை *priv;
};

/** Internal irqchip interrupt handler */
கட்டமைப்பு sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	கட்டமைப்பு sbi_dlist முனை;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 இலக்கம்_hwirq;

	/** Write MSI function of this handler */
	வெறுமை (*write_msi)(u32 hwirq, மாறிலி கட்டமைப்பு sbi_irqchip_msi_செய்தி *செய்தி, வெறுமை *priv);

	/** Callback function of this handler */
	முழு (*callback)(u32 hwirq, வெறுமை *priv);

	/** Callback private data */
	வெறுமை *priv;
};

கட்டமைப்பு sbi_irqchip_hart_data {
	கட்டமைப்பு sbi_irqchip_device *chip;
};

நிலையான unsigned long irqchip_hart_data_off;
நிலையான SBI_LIST_HEAD(irqchip_list);

முழு sbi_irqchip_process(வெறுமை)
{
	கட்டமைப்பு sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	எனில் (!hd || !hd->chip || !hd->chip->process_hwirqs)
		திரும்பு SBI_ENODEV;

	திரும்பு hd->chip->process_hwirqs(hd->chip);
}

முழு sbi_irqchip_process_hwirq(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	எனில் (!data->raw_handler)
		திரும்பு SBI_ENOENT;

	திரும்பு data->raw_handler(chip, hwirq);
}

நிலையான உள்வரி u32 sbi_irqchip_get_irq_state(கட்டமைப்பு sbi_irqchip_device *chip,
					     u32 hwirq)
{
	எனில் (!chip || !chip->hwirqs || hwirq >= chip->இலக்கம்_hwirq)
		திரும்பு 0;

	திரும்பு chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(கட்டமைப்பு sbi_irqchip_device *chip,
				   u32 hwirq)
{
	திரும்பு !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

முழு sbi_irqchip_unmask_hwirq(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	எனில் (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		திரும்பு SBI_EALREADY;

	எனில் (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	திரும்பு 0;
}

முழு sbi_irqchip_mask_hwirq(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	எனில் (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		திரும்பு SBI_EALREADY;

	எனில் (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	திரும்பு 0;
}

நிலையான கட்டமைப்பு sbi_irqchip_handler *sbi_irqchip_find_handler(கட்டமைப்பு sbi_irqchip_device *chip,
							    u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_handler *h;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு வெற்று;

	sbi_list_for_each_entry(h, &chip->handler_list, முனை) {
		எனில் (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->இலக்கம்_hwirq))
			திரும்பு h;
	}

	திரும்பு வெற்று;
}

முழு sbi_irqchip_set_hwirq_priv(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq, வெறுமை *priv)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
	     திரும்பு SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	திரும்பு 0;
}

வெறுமை *sbi_irqchip_get_hwirq_priv(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;
	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
	    திரும்பு வெற்று;

	data = &chip->hwirqs[hwirq];

	திரும்பு data->priv;
}

முழு sbi_irqchip_raw_handler_default(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq)
{
	கட்டமைப்பு sbi_irqchip_handler *h;
	முழு rc = SBI_OK;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	எனில் (!h)
		திரும்பு SBI_EINVAL;

	எனில் (h->callback)
		rc = h->callback(hwirq, h->priv);

	எனில் (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	திரும்பு rc;
}

முழு sbi_irqchip_set_raw_handler(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq,
				முழு (*raw_hndl)(கட்டமைப்பு sbi_irqchip_device *, u32))
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	திரும்பு 0;
}

முழு sbi_irqchip_write_msi(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq,
			  மாறிலி கட்டமைப்பு sbi_irqchip_msi_செய்தி *செய்தி)
{
	கட்டமைப்பு sbi_irqchip_handler *h;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq || !செய்தி)
		திரும்பு SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	எனில் (!h)
		திரும்பு SBI_EFAIL;
	எனில் (!h->write_msi)
		திரும்பு SBI_ENOTSUPP;

	h->write_msi(hwirq, செய்தி, h->priv);
	திரும்பு 0;
}

முழு sbi_irqchip_get_affinity(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq)
		திரும்பு SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	எனில் (!sbi_irqchip_find_handler(chip, hwirq))
		திரும்பு SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	திரும்பு 0;
}

முழு sbi_irqchip_set_affinity(கட்டமைப்பு sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	கட்டமைப்பு sbi_irqchip_hwirq_data *data;
	முழு rc;

	எனில் (!chip || chip->இலக்கம்_hwirq <= hwirq || sbi_hart_மொத்தம்() <= hart_index)
		திரும்பு SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	எனில் (!sbi_irqchip_find_handler(chip, hwirq))
		திரும்பு SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	எனில் (data->hart_index != hart_index) {
		எனில் (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			எனில் (rc)
				திரும்பு rc;
		}
		data->hart_index = hart_index;
	}

	திரும்பு 0;
}

நிலையான முழு __sbi_irqchip_handler_set_affinity(கட்டமைப்பு sbi_irqchip_device *chip,
					      கட்டமைப்பு sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	முழு rc;

	க்கு (i = 0; i < h->இலக்கம்_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		எனில் (rc)
			திரும்பு rc;

		எனில் (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			தொடர்;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		எனில் (rc)
			திரும்பு rc;
	}

	திரும்பு 0;
}

நிலையான முழு __sbi_irqchip_register_handler(கட்டமைப்பு sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 இலக்கம்_hwirq, u32 hwirq_flags,
					  வெறுமை (*write_msi)(u32 hwirq,
							    மாறிலி கட்டமைப்பு sbi_irqchip_msi_செய்தி *செய்தி,
							    வெறுமை *priv),
					  முழு (*callback)(u32 hwirq, வெறுமை *priv), வெறுமை *priv)
{
	கட்டமைப்பு sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	முழு rc;

	க்கு (i = first_hwirq; i < (first_hwirq + இலக்கம்_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		எனில் (h)
			திரும்பு SBI_EALREADY;
	}

	h = sbi_zalloc(அளவு(*h));
	எனில் (!h)
		திரும்பு SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->இலக்கம்_hwirq = இலக்கம்_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = வெற்று;
	sbi_list_for_each_entry(th, &chip->handler_list, முனை) {
		எனில் (h->first_hwirq < th->first_hwirq) {
			nh = th;
			நிறுத்து;
		}
	}
	எனில் (nh)
		sbi_list_add(&h->முனை, &nh->முனை);
	இல்லையெனில்
		sbi_list_add_tail(&h->முனை, &chip->handler_list);

	எனில் (chip->hwirq_setup) {
		க்கு (i = 0; i < h->இலக்கம்_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			எனில் (rc) {
				எனில் (chip->hwirq_cleanup) {
					க்கு (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->முனை);
				sbi_free(h);
				திரும்பு rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	எனில் (rc) {
		எனில் (chip->hwirq_cleanup) {
			க்கு (i = 0; i < h->இலக்கம்_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->முனை);
		sbi_free(h);
		திரும்பு rc;
	}

	எனில் (chip->hwirq_unmask) {
		க்கு (i = 0; i < h->இலக்கம்_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	திரும்பு 0;
}

முழு sbi_irqchip_register_msi(கட்டமைப்பு sbi_irqchip_device *chip, u32 இலக்கம்_hwirq,
			     வெறுமை (*write_msi)(u32 hwirq,
					       மாறிலி கட்டமைப்பு sbi_irqchip_msi_செய்தி *செய்தி,
					       வெறுமை *priv),
			     முழு (*callback)(u32 hwirq, வெறுமை *priv), வெறுமை *priv,
			     u32 *out_first_hwirq)
{
	கட்டமைப்பு sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	எனில் (!chip || !chip->hwirq_set_affinity || !இலக்கம்_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		திரும்பு SBI_EINVAL;
	எனில் (chip->இலக்கம்_hwirq < இலக்கம்_hwirq)
		திரும்பு SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, முனை) {
		எனில் (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->இலக்கம்_hwirq)) {
			hwirq = h->first_hwirq + h->இலக்கம்_hwirq;
		} இல்லையெனில் எனில் (hwirq < h->first_hwirq) {
			எனில் (h->first_hwirq - hwirq >= இலக்கம்_hwirq) {
				found = true;
				நிறுத்து;
			} இல்லையெனில் {
				hwirq = h->first_hwirq + h->இலக்கம்_hwirq;
			}
		}
	}
	எனில் (!found && (chip->இலக்கம்_hwirq - hwirq) >= இலக்கம்_hwirq)
		found = true;
	எனில் (!found)
		திரும்பு SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	திரும்பு __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      இலக்கம்_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

முழு sbi_irqchip_register_handler(கட்டமைப்பு sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 இலக்கம்_hwirq, u32 hwirq_flags,
				 முழு (*callback)(u32 hwirq, வெறுமை *priv), வெறுமை *priv)
{
	எனில் (!chip || !இலக்கம்_hwirq || !callback)
		திரும்பு SBI_EINVAL;
	எனில் (chip->இலக்கம்_hwirq <= first_hwirq ||
	    chip->இலக்கம்_hwirq <= (first_hwirq + இலக்கம்_hwirq - 1))
		திரும்பு SBI_EBAD_RANGE;

	திரும்பு __sbi_irqchip_register_handler(chip, first_hwirq, இலக்கம்_hwirq, hwirq_flags,
					      வெற்று, callback, priv);
}

முழு sbi_irqchip_register_reserved(கட்டமைப்பு sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 இலக்கம்_hwirq)
{
	எனில் (!chip || !இலக்கம்_hwirq)
		திரும்பு SBI_EINVAL;
	எனில் (chip->இலக்கம்_hwirq <= first_hwirq ||
	    chip->இலக்கம்_hwirq <= (first_hwirq + இலக்கம்_hwirq - 1))
		திரும்பு SBI_EBAD_RANGE;

	திரும்பு __sbi_irqchip_register_handler(chip, first_hwirq, இலக்கம்_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, வெற்று, வெற்று, வெற்று);
}

முழு sbi_irqchip_unregister_handler(கட்டமைப்பு sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 இலக்கம்_hwirq)
{
	கட்டமைப்பு sbi_irqchip_handler *fh, *lh;
	u32 i;

	எனில் (!chip || !இலக்கம்_hwirq)
		திரும்பு SBI_EINVAL;
	எனில் (chip->இலக்கம்_hwirq <= first_hwirq ||
	    chip->இலக்கம்_hwirq <= (first_hwirq + இலக்கம்_hwirq - 1))
		திரும்பு SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	எனில் (!fh || fh->first_hwirq != first_hwirq || fh->இலக்கம்_hwirq != இலக்கம்_hwirq)
		திரும்பு SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + இலக்கம்_hwirq - 1);
	எனில் (!lh || lh != fh)
		திரும்பு SBI_ENODEV;

	எனில் (chip->hwirq_mask) {
		க்கு (i = 0; i < fh->இலக்கம்_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	எனில் (chip->hwirq_cleanup) {
		க்கு (i = 0; i < fh->இலக்கம்_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->முனை);
	திரும்பு 0;
}

கட்டமைப்பு sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   கட்டமைப்பு sbi_irqchip_device *first)
{
	கட்டமைப்பு sbi_irqchip_device *chip;
	bool found = (first == வெற்று);

	sbi_list_for_each_entry(chip, &irqchip_list, முனை) {
		எனில் (!found) {
			எனில் (first == chip)
				found = true;
			தொடர்;
		}
		எனில் ((chip->caps & caps) == caps)
			திரும்பு chip;
	}

	திரும்பு வெற்று;
}

கட்டமைப்பு sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	கட்டமைப்பு sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, முனை) {
		எனில் (chip->id == id)
			திரும்பு chip;
	}

	திரும்பு வெற்று;
}

முழு sbi_irqchip_add_device(கட்டமைப்பு sbi_irqchip_device *chip)
{
	கட்டமைப்பு sbi_irqchip_hart_data *hd;
	கட்டமைப்பு sbi_scratch *scratch;
	u32 i, h;

	எனில் (!chip || !chip->இலக்கம்_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		திரும்பு SBI_EINVAL;

	எனில் (sbi_irqchip_find_device(chip->id))
		திரும்பு SBI_EALREADY;

	எனில் (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			எனில் (!scratch)
				தொடர்;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			எனில் (hd->chip && hd->chip != chip)
				திரும்பு SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(அளவு(*chip->hwirqs) * chip->இலக்கம்_hwirq);
	எனில் (!chip->hwirqs)
		திரும்பு SBI_ENOMEM;
	க்கு (i = 0; i < chip->இலக்கம்_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->முனை, &irqchip_list);
	திரும்பு 0;
}

முழு sbi_irqchip_init(கட்டமைப்பு sbi_scratch *scratch, bool cold_boot)
{
	மாறிலி கட்டமைப்பு sbi_platform *plat = sbi_platform_ptr(scratch);
	கட்டமைப்பு sbi_irqchip_hart_data *hd;
	கட்டமைப்பு sbi_irqchip_device *chip;
	முழு rc;

	எனில் (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(அளவு(கட்டமைப்பு sbi_irqchip_hart_data));
		எனில் (!irqchip_hart_data_off)
			திரும்பு SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		எனில் (rc)
			திரும்பு rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, முனை) {
		எனில் (!chip->warm_init)
			தொடர்;
		எனில் (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			தொடர்;
		rc = chip->warm_init(chip);
		எனில் (rc)
			திரும்பு rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	எனில் (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	திரும்பு 0;
}

வெறுமை sbi_irqchip_exit(கட்டமைப்பு sbi_scratch *scratch)
{
	கட்டமைப்பு sbi_irqchip_hart_data *hd;
	கட்டமைப்பு sbi_irqchip_device *chip;
	கட்டமைப்பு sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	முழு rc;

	sbi_for_each_hartindex(i) {
		எனில் (i == current_hartindex())
			தொடர்;
		எனில் (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			தொடர்;
		migrate_hidx = i;
		migrate = true;
		நிறுத்து;
	}

	எனில் (!migrate)
		செல் skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, முனை) {
		sbi_list_for_each_entry(h, &chip->handler_list, முனை) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			எனில் (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	எனில் (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
