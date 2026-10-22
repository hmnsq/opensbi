/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_irqchip.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
संरचना sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	पूर्णाङ्क (*raw_handler)(संरचना sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	शून्य *priv;
};

/** Internal irqchip interrupt handler */
संरचना sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	संरचना sbi_dlist ग्रन्थिः;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 अङ्कः_hwirq;

	/** Write MSI function of this handler */
	शून्य (*write_msi)(u32 hwirq, ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः, शून्य *priv);

	/** Callback function of this handler */
	पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv);

	/** Callback private data */
	शून्य *priv;
};

संरचना sbi_irqchip_hart_data {
	संरचना sbi_irqchip_device *chip;
};

स्थिर unsigned long irqchip_hart_data_off;
स्थिर SBI_LIST_HEAD(irqchip_list);

पूर्णाङ्क sbi_irqchip_process(शून्य)
{
	संरचना sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	सचे (!hd || !hd->chip || !hd->chip->process_hwirqs)
		पटिनिवत्त SBI_ENODEV;

	पटिनिवत्त hd->chip->process_hwirqs(hd->chip);
}

पूर्णाङ्क sbi_irqchip_process_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	सचे (!data->raw_handler)
		पटिनिवत्त SBI_ENOENT;

	पटिनिवत्त data->raw_handler(chip, hwirq);
}

स्थिर अन्तःस्थ u32 sbi_irqchip_get_irq_state(संरचना sbi_irqchip_device *chip,
					     u32 hwirq)
{
	सचे (!chip || !chip->hwirqs || hwirq >= chip->अङ्कः_hwirq)
		पटिनिवत्त 0;

	पटिनिवत्त chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(संरचना sbi_irqchip_device *chip,
				   u32 hwirq)
{
	पटिनिवत्त !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

पूर्णाङ्क sbi_irqchip_unmask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	सचे (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		पटिनिवत्त SBI_EALREADY;

	सचे (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_mask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	सचे (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		पटिनिवत्त SBI_EALREADY;

	सचे (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	पटिनिवत्त 0;
}

स्थिर संरचना sbi_irqchip_handler *sbi_irqchip_find_handler(संरचना sbi_irqchip_device *chip,
							    u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त रिक्त;

	sbi_list_for_each_entry(h, &chip->handler_list, ग्रन्थिः) {
		सचे (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्कः_hwirq))
			पटिनिवत्त h;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क sbi_irqchip_set_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq, शून्य *priv)
{
	संरचना sbi_irqchip_hwirq_data *data;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
	     पटिनिवत्त SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	पटिनिवत्त 0;
}

शून्य *sbi_irqchip_get_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;
	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
	    पटिनिवत्त रिक्त;

	data = &chip->hwirqs[hwirq];

	पटिनिवत्त data->priv;
}

पूर्णाङ्क sbi_irqchip_raw_handler_default(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;
	पूर्णाङ्क rc = SBI_OK;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	सचे (!h)
		पटिनिवत्त SBI_EINVAL;

	सचे (h->callback)
		rc = h->callback(hwirq, h->priv);

	सचे (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	पटिनिवत्त rc;
}

पूर्णाङ्क sbi_irqchip_set_raw_handler(संरचना sbi_irqchip_device *chip, u32 hwirq,
				पूर्णाङ्क (*raw_hndl)(संरचना sbi_irqchip_device *, u32))
{
	संरचना sbi_irqchip_hwirq_data *data;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_write_msi(संरचना sbi_irqchip_device *chip, u32 hwirq,
			  ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः)
{
	संरचना sbi_irqchip_handler *h;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq || !सन्देशः)
		पटिनिवत्त SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	सचे (!h)
		पटिनिवत्त SBI_EFAIL;
	सचे (!h->write_msi)
		पटिनिवत्त SBI_ENOTSUPP;

	h->write_msi(hwirq, सन्देशः, h->priv);
	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_get_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	सचे (!chip || chip->अङ्कः_hwirq <= hwirq)
		पटिनिवत्त SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	सचे (!sbi_irqchip_find_handler(chip, hwirq))
		पटिनिवत्त SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_set_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	संरचना sbi_irqchip_hwirq_data *data;
	पूर्णाङ्क rc;

	सचे (!chip || chip->अङ्कः_hwirq <= hwirq || sbi_hart_गणनम्() <= hart_index)
		पटिनिवत्त SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	सचे (!sbi_irqchip_find_handler(chip, hwirq))
		पटिनिवत्त SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	सचे (data->hart_index != hart_index) {
		सचे (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			सचे (rc)
				पटिनिवत्त rc;
		}
		data->hart_index = hart_index;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क __sbi_irqchip_handler_set_affinity(संरचना sbi_irqchip_device *chip,
					      संरचना sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	पूर्णाङ्क rc;

	अत्थाय (i = 0; i < h->अङ्कः_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		सचे (rc)
			पटिनिवत्त rc;

		सचे (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			अनुवर्त;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		सचे (rc)
			पटिनिवत्त rc;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क __sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 अङ्कः_hwirq, u32 hwirq_flags,
					  शून्य (*write_msi)(u32 hwirq,
							    ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः,
							    शून्य *priv),
					  पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	संरचना sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	पूर्णाङ्क rc;

	अत्थाय (i = first_hwirq; i < (first_hwirq + अङ्कः_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		सचे (h)
			पटिनिवत्त SBI_EALREADY;
	}

	h = sbi_zalloc(परिमाण(*h));
	सचे (!h)
		पटिनिवत्त SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->अङ्कः_hwirq = अङ्कः_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = रिक्त;
	sbi_list_for_each_entry(th, &chip->handler_list, ग्रन्थिः) {
		सचे (h->first_hwirq < th->first_hwirq) {
			nh = th;
			भङ्ग;
		}
	}
	सचे (nh)
		sbi_list_add(&h->ग्रन्थिः, &nh->ग्रन्थिः);
	अञ्ञथा
		sbi_list_add_tail(&h->ग्रन्थिः, &chip->handler_list);

	सचे (chip->hwirq_setup) {
		अत्थाय (i = 0; i < h->अङ्कः_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			सचे (rc) {
				सचे (chip->hwirq_cleanup) {
					अत्थाय (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->ग्रन्थिः);
				sbi_free(h);
				पटिनिवत्त rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	सचे (rc) {
		सचे (chip->hwirq_cleanup) {
			अत्थाय (i = 0; i < h->अङ्कः_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->ग्रन्थिः);
		sbi_free(h);
		पटिनिवत्त rc;
	}

	सचे (chip->hwirq_unmask) {
		अत्थाय (i = 0; i < h->अङ्कः_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_register_msi(संरचना sbi_irqchip_device *chip, u32 अङ्कः_hwirq,
			     शून्य (*write_msi)(u32 hwirq,
					       ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः,
					       शून्य *priv),
			     पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv,
			     u32 *out_first_hwirq)
{
	संरचना sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	सचे (!chip || !chip->hwirq_set_affinity || !अङ्कः_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		पटिनिवत्त SBI_EINVAL;
	सचे (chip->अङ्कः_hwirq < अङ्कः_hwirq)
		पटिनिवत्त SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, ग्रन्थिः) {
		सचे (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्कः_hwirq)) {
			hwirq = h->first_hwirq + h->अङ्कः_hwirq;
		} अञ्ञथा सचे (hwirq < h->first_hwirq) {
			सचे (h->first_hwirq - hwirq >= अङ्कः_hwirq) {
				found = true;
				भङ्ग;
			} अञ्ञथा {
				hwirq = h->first_hwirq + h->अङ्कः_hwirq;
			}
		}
	}
	सचे (!found && (chip->अङ्कः_hwirq - hwirq) >= अङ्कः_hwirq)
		found = true;
	सचे (!found)
		पटिनिवत्त SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	पटिनिवत्त __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      अङ्कः_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

पूर्णाङ्क sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 अङ्कः_hwirq, u32 hwirq_flags,
				 पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	सचे (!chip || !अङ्कः_hwirq || !callback)
		पटिनिवत्त SBI_EINVAL;
	सचे (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		पटिनिवत्त SBI_EBAD_RANGE;

	पटिनिवत्त __sbi_irqchip_register_handler(chip, first_hwirq, अङ्कः_hwirq, hwirq_flags,
					      रिक्त, callback, priv);
}

पूर्णाङ्क sbi_irqchip_register_reserved(संरचना sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 अङ्कः_hwirq)
{
	सचे (!chip || !अङ्कः_hwirq)
		पटिनिवत्त SBI_EINVAL;
	सचे (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		पटिनिवत्त SBI_EBAD_RANGE;

	पटिनिवत्त __sbi_irqchip_register_handler(chip, first_hwirq, अङ्कः_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, रिक्त, रिक्त, रिक्त);
}

पूर्णाङ्क sbi_irqchip_unregister_handler(संरचना sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 अङ्कः_hwirq)
{
	संरचना sbi_irqchip_handler *fh, *lh;
	u32 i;

	सचे (!chip || !अङ्कः_hwirq)
		पटिनिवत्त SBI_EINVAL;
	सचे (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		पटिनिवत्त SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	सचे (!fh || fh->first_hwirq != first_hwirq || fh->अङ्कः_hwirq != अङ्कः_hwirq)
		पटिनिवत्त SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + अङ्कः_hwirq - 1);
	सचे (!lh || lh != fh)
		पटिनिवत्त SBI_ENODEV;

	सचे (chip->hwirq_mask) {
		अत्थाय (i = 0; i < fh->अङ्कः_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	सचे (chip->hwirq_cleanup) {
		अत्थाय (i = 0; i < fh->अङ्कः_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->ग्रन्थिः);
	पटिनिवत्त 0;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   संरचना sbi_irqchip_device *first)
{
	संरचना sbi_irqchip_device *chip;
	bool found = (first == रिक्त);

	sbi_list_for_each_entry(chip, &irqchip_list, ग्रन्थिः) {
		सचे (!found) {
			सचे (first == chip)
				found = true;
			अनुवर्त;
		}
		सचे ((chip->caps & caps) == caps)
			पटिनिवत्त chip;
	}

	पटिनिवत्त रिक्त;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	संरचना sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, ग्रन्थिः) {
		सचे (chip->id == id)
			पटिनिवत्त chip;
	}

	पटिनिवत्त रिक्त;
}

पूर्णाङ्क sbi_irqchip_add_device(संरचना sbi_irqchip_device *chip)
{
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_scratch *scratch;
	u32 i, h;

	सचे (!chip || !chip->अङ्कः_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		पटिनिवत्त SBI_EINVAL;

	सचे (sbi_irqchip_find_device(chip->id))
		पटिनिवत्त SBI_EALREADY;

	सचे (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			सचे (!scratch)
				अनुवर्त;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			सचे (hd->chip && hd->chip != chip)
				पटिनिवत्त SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(परिमाण(*chip->hwirqs) * chip->अङ्कः_hwirq);
	सचे (!chip->hwirqs)
		पटिनिवत्त SBI_ENOMEM;
	अत्थाय (i = 0; i < chip->अङ्कः_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->ग्रन्थिः, &irqchip_list);
	पटिनिवत्त 0;
}

पूर्णाङ्क sbi_irqchip_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_irqchip_device *chip;
	पूर्णाङ्क rc;

	सचे (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(परिमाण(संरचना sbi_irqchip_hart_data));
		सचे (!irqchip_hart_data_off)
			पटिनिवत्त SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		सचे (rc)
			पटिनिवत्त rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, ग्रन्थिः) {
		सचे (!chip->warm_init)
			अनुवर्त;
		सचे (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			अनुवर्त;
		rc = chip->warm_init(chip);
		सचे (rc)
			पटिनिवत्त rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	सचे (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	पटिनिवत्त 0;
}

शून्य sbi_irqchip_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_irqchip_device *chip;
	संरचना sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	पूर्णाङ्क rc;

	sbi_for_each_hartindex(i) {
		सचे (i == current_hartindex())
			अनुवर्त;
		सचे (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			अनुवर्त;
		migrate_hidx = i;
		migrate = true;
		भङ्ग;
	}

	सचे (!migrate)
		गच्छ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, ग्रन्थिः) {
		sbi_list_for_each_entry(h, &chip->handler_list, ग्रन्थिः) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			सचे (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	सचे (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
