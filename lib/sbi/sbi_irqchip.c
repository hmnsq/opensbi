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
	संरचना sbi_dlist गाँठो;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 अङ्क_hwirq;

	/** Write MSI function of this handler */
	शून्य (*write_msi)(u32 hwirq, अचर संरचना sbi_irqchip_msi_सन्देश *सन्देश, शून्य *priv);

	/** Callback function of this handler */
	पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv);

	/** Callback private data */
	शून्य *priv;
};

संरचना sbi_irqchip_hart_data {
	संरचना sbi_irqchip_device *chip;
};

स्थिर अचिन्हित दीर्घ irqchip_hart_data_off;
स्थिर SBI_LIST_HEAD(irqchip_list);

पूर्णाङ्क sbi_irqchip_process(शून्य)
{
	संरचना sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	यदि (!hd || !hd->chip || !hd->chip->process_hwirqs)
		फर्काऊ SBI_ENODEV;

	फर्काऊ hd->chip->process_hwirqs(hd->chip);
}

पूर्णाङ्क sbi_irqchip_process_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	यदि (!data->raw_handler)
		फर्काऊ SBI_ENOENT;

	फर्काऊ data->raw_handler(chip, hwirq);
}

स्थिर अन्तःस्थ u32 sbi_irqchip_get_irq_state(संरचना sbi_irqchip_device *chip,
					     u32 hwirq)
{
	यदि (!chip || !chip->hwirqs || hwirq >= chip->अङ्क_hwirq)
		फर्काऊ 0;

	फर्काऊ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(संरचना sbi_irqchip_device *chip,
				   u32 hwirq)
{
	फर्काऊ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

पूर्णाङ्क sbi_irqchip_unmask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	यदि (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		फर्काऊ SBI_EALREADY;

	यदि (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_mask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	यदि (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		फर्काऊ SBI_EALREADY;

	यदि (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	फर्काऊ 0;
}

स्थिर संरचना sbi_irqchip_handler *sbi_irqchip_find_handler(संरचना sbi_irqchip_device *chip,
							    u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ रिक्त;

	sbi_list_for_each_entry(h, &chip->handler_list, गाँठो) {
		यदि (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्क_hwirq))
			फर्काऊ h;
	}

	फर्काऊ रिक्त;
}

पूर्णाङ्क sbi_irqchip_set_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq, शून्य *priv)
{
	संरचना sbi_irqchip_hwirq_data *data;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
	     फर्काऊ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	फर्काऊ 0;
}

शून्य *sbi_irqchip_get_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;
	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
	    फर्काऊ रिक्त;

	data = &chip->hwirqs[hwirq];

	फर्काऊ data->priv;
}

पूर्णाङ्क sbi_irqchip_raw_handler_default(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;
	पूर्णाङ्क rc = SBI_OK;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	यदि (!h)
		फर्काऊ SBI_EINVAL;

	यदि (h->callback)
		rc = h->callback(hwirq, h->priv);

	यदि (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	फर्काऊ rc;
}

पूर्णाङ्क sbi_irqchip_set_raw_handler(संरचना sbi_irqchip_device *chip, u32 hwirq,
				पूर्णाङ्क (*raw_hndl)(संरचना sbi_irqchip_device *, u32))
{
	संरचना sbi_irqchip_hwirq_data *data;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_write_msi(संरचना sbi_irqchip_device *chip, u32 hwirq,
			  अचर संरचना sbi_irqchip_msi_सन्देश *सन्देश)
{
	संरचना sbi_irqchip_handler *h;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq || !सन्देश)
		फर्काऊ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	यदि (!h)
		फर्काऊ SBI_EFAIL;
	यदि (!h->write_msi)
		फर्काऊ SBI_ENOTSUPP;

	h->write_msi(hwirq, सन्देश, h->priv);
	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_get_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	यदि (!chip || chip->अङ्क_hwirq <= hwirq)
		फर्काऊ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	यदि (!sbi_irqchip_find_handler(chip, hwirq))
		फर्काऊ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_set_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	संरचना sbi_irqchip_hwirq_data *data;
	पूर्णाङ्क rc;

	यदि (!chip || chip->अङ्क_hwirq <= hwirq || sbi_hart_गन्ती() <= hart_index)
		फर्काऊ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	यदि (!sbi_irqchip_find_handler(chip, hwirq))
		फर्काऊ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	यदि (data->hart_index != hart_index) {
		यदि (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			यदि (rc)
				फर्काऊ rc;
		}
		data->hart_index = hart_index;
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क __sbi_irqchip_handler_set_affinity(संरचना sbi_irqchip_device *chip,
					      संरचना sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	पूर्णाङ्क rc;

	लागि (i = 0; i < h->अङ्क_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		यदि (rc)
			फर्काऊ rc;

		यदि (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			जारी;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		यदि (rc)
			फर्काऊ rc;
	}

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क __sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 अङ्क_hwirq, u32 hwirq_flags,
					  शून्य (*write_msi)(u32 hwirq,
							    अचर संरचना sbi_irqchip_msi_सन्देश *सन्देश,
							    शून्य *priv),
					  पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	संरचना sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	पूर्णाङ्क rc;

	लागि (i = first_hwirq; i < (first_hwirq + अङ्क_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		यदि (h)
			फर्काऊ SBI_EALREADY;
	}

	h = sbi_zalloc(परिमाण(*h));
	यदि (!h)
		फर्काऊ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->अङ्क_hwirq = अङ्क_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = रिक्त;
	sbi_list_for_each_entry(th, &chip->handler_list, गाँठो) {
		यदि (h->first_hwirq < th->first_hwirq) {
			nh = th;
			रोक;
		}
	}
	यदि (nh)
		sbi_list_add(&h->गाँठो, &nh->गाँठो);
	अन्यथा
		sbi_list_add_tail(&h->गाँठो, &chip->handler_list);

	यदि (chip->hwirq_setup) {
		लागि (i = 0; i < h->अङ्क_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			यदि (rc) {
				यदि (chip->hwirq_cleanup) {
					लागि (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->गाँठो);
				sbi_free(h);
				फर्काऊ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	यदि (rc) {
		यदि (chip->hwirq_cleanup) {
			लागि (i = 0; i < h->अङ्क_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->गाँठो);
		sbi_free(h);
		फर्काऊ rc;
	}

	यदि (chip->hwirq_unmask) {
		लागि (i = 0; i < h->अङ्क_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_register_msi(संरचना sbi_irqchip_device *chip, u32 अङ्क_hwirq,
			     शून्य (*write_msi)(u32 hwirq,
					       अचर संरचना sbi_irqchip_msi_सन्देश *सन्देश,
					       शून्य *priv),
			     पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv,
			     u32 *out_first_hwirq)
{
	संरचना sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	यदि (!chip || !chip->hwirq_set_affinity || !अङ्क_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		फर्काऊ SBI_EINVAL;
	यदि (chip->अङ्क_hwirq < अङ्क_hwirq)
		फर्काऊ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, गाँठो) {
		यदि (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्क_hwirq)) {
			hwirq = h->first_hwirq + h->अङ्क_hwirq;
		} अन्यथा यदि (hwirq < h->first_hwirq) {
			यदि (h->first_hwirq - hwirq >= अङ्क_hwirq) {
				found = true;
				रोक;
			} अन्यथा {
				hwirq = h->first_hwirq + h->अङ्क_hwirq;
			}
		}
	}
	यदि (!found && (chip->अङ्क_hwirq - hwirq) >= अङ्क_hwirq)
		found = true;
	यदि (!found)
		फर्काऊ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	फर्काऊ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      अङ्क_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

पूर्णाङ्क sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 अङ्क_hwirq, u32 hwirq_flags,
				 पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	यदि (!chip || !अङ्क_hwirq || !callback)
		फर्काऊ SBI_EINVAL;
	यदि (chip->अङ्क_hwirq <= first_hwirq ||
	    chip->अङ्क_hwirq <= (first_hwirq + अङ्क_hwirq - 1))
		फर्काऊ SBI_EBAD_RANGE;

	फर्काऊ __sbi_irqchip_register_handler(chip, first_hwirq, अङ्क_hwirq, hwirq_flags,
					      रिक्त, callback, priv);
}

पूर्णाङ्क sbi_irqchip_register_reserved(संरचना sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 अङ्क_hwirq)
{
	यदि (!chip || !अङ्क_hwirq)
		फर्काऊ SBI_EINVAL;
	यदि (chip->अङ्क_hwirq <= first_hwirq ||
	    chip->अङ्क_hwirq <= (first_hwirq + अङ्क_hwirq - 1))
		फर्काऊ SBI_EBAD_RANGE;

	फर्काऊ __sbi_irqchip_register_handler(chip, first_hwirq, अङ्क_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, रिक्त, रिक्त, रिक्त);
}

पूर्णाङ्क sbi_irqchip_unregister_handler(संरचना sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 अङ्क_hwirq)
{
	संरचना sbi_irqchip_handler *fh, *lh;
	u32 i;

	यदि (!chip || !अङ्क_hwirq)
		फर्काऊ SBI_EINVAL;
	यदि (chip->अङ्क_hwirq <= first_hwirq ||
	    chip->अङ्क_hwirq <= (first_hwirq + अङ्क_hwirq - 1))
		फर्काऊ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	यदि (!fh || fh->first_hwirq != first_hwirq || fh->अङ्क_hwirq != अङ्क_hwirq)
		फर्काऊ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + अङ्क_hwirq - 1);
	यदि (!lh || lh != fh)
		फर्काऊ SBI_ENODEV;

	यदि (chip->hwirq_mask) {
		लागि (i = 0; i < fh->अङ्क_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	यदि (chip->hwirq_cleanup) {
		लागि (i = 0; i < fh->अङ्क_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->गाँठो);
	फर्काऊ 0;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device_by_caps(अचिन्हित दीर्घ caps,
							   संरचना sbi_irqchip_device *first)
{
	संरचना sbi_irqchip_device *chip;
	bool found = (first == रिक्त);

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठो) {
		यदि (!found) {
			यदि (first == chip)
				found = true;
			जारी;
		}
		यदि ((chip->caps & caps) == caps)
			फर्काऊ chip;
	}

	फर्काऊ रिक्त;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	संरचना sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठो) {
		यदि (chip->id == id)
			फर्काऊ chip;
	}

	फर्काऊ रिक्त;
}

पूर्णाङ्क sbi_irqchip_add_device(संरचना sbi_irqchip_device *chip)
{
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_scratch *scratch;
	u32 i, h;

	यदि (!chip || !chip->अङ्क_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		फर्काऊ SBI_EINVAL;

	यदि (sbi_irqchip_find_device(chip->id))
		फर्काऊ SBI_EALREADY;

	यदि (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			यदि (!scratch)
				जारी;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			यदि (hd->chip && hd->chip != chip)
				फर्काऊ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(परिमाण(*chip->hwirqs) * chip->अङ्क_hwirq);
	यदि (!chip->hwirqs)
		फर्काऊ SBI_ENOMEM;
	लागि (i = 0; i < chip->अङ्क_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->गाँठो, &irqchip_list);
	फर्काऊ 0;
}

पूर्णाङ्क sbi_irqchip_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	अचर संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_irqchip_device *chip;
	पूर्णाङ्क rc;

	यदि (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(परिमाण(संरचना sbi_irqchip_hart_data));
		यदि (!irqchip_hart_data_off)
			फर्काऊ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		यदि (rc)
			फर्काऊ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठो) {
		यदि (!chip->warm_init)
			जारी;
		यदि (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			जारी;
		rc = chip->warm_init(chip);
		यदि (rc)
			फर्काऊ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	यदि (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	फर्काऊ 0;
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
		यदि (i == current_hartindex())
			जारी;
		यदि (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			जारी;
		migrate_hidx = i;
		migrate = true;
		रोक;
	}

	यदि (!migrate)
		गच्छ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, गाँठो) {
		sbi_list_for_each_entry(h, &chip->handler_list, गाँठो) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			यदि (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	यदि (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
