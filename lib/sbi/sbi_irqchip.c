/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_irqchip.h>
#അന്തര്ഭാവ <sbi/sbi_list.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
ഘടന sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	പൂർണ്ണസംഖ്യ (*raw_handler)(ഘടന sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	ശൂന്യം *priv;
};

/** Internal irqchip interrupt handler */
ഘടന sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ഘടന sbi_dlist കണ്ണി;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 അക്കം_hwirq;

	/** Write MSI function of this handler */
	ശൂന്യം (*write_msi)(u32 hwirq, സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം, ശൂന്യം *priv);

	/** Callback function of this handler */
	പൂർണ്ണസംഖ്യ (*callback)(u32 hwirq, ശൂന്യം *priv);

	/** Callback private data */
	ശൂന്യം *priv;
};

ഘടന sbi_irqchip_hart_data {
	ഘടന sbi_irqchip_device *chip;
};

സ്ഥിതം unsigned long irqchip_hart_data_off;
സ്ഥിതം SBI_LIST_HEAD(irqchip_list);

പൂർണ്ണസംഖ്യ sbi_irqchip_process(ശൂന്യം)
{
	ഘടന sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	എങ്കിൽ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		തിരികെ SBI_ENODEV;

	തിരികെ hd->chip->process_hwirqs(hd->chip);
}

പൂർണ്ണസംഖ്യ sbi_irqchip_process_hwirq(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന sbi_irqchip_hwirq_data *data;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	എങ്കിൽ (!data->raw_handler)
		തിരികെ SBI_ENOENT;

	തിരികെ data->raw_handler(chip, hwirq);
}

സ്ഥിതം വരിയിൽ u32 sbi_irqchip_get_irq_state(ഘടന sbi_irqchip_device *chip,
					     u32 hwirq)
{
	എങ്കിൽ (!chip || !chip->hwirqs || hwirq >= chip->അക്കം_hwirq)
		തിരികെ 0;

	തിരികെ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ഘടന sbi_irqchip_device *chip,
				   u32 hwirq)
{
	തിരികെ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

പൂർണ്ണസംഖ്യ sbi_irqchip_unmask_hwirq(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന sbi_irqchip_hwirq_data *data;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	എങ്കിൽ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		തിരികെ SBI_EALREADY;

	എങ്കിൽ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_mask_hwirq(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന sbi_irqchip_hwirq_data *data;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		തിരികെ SBI_EALREADY;

	എങ്കിൽ (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	തിരികെ 0;
}

സ്ഥിതം ഘടന sbi_irqchip_handler *sbi_irqchip_find_handler(ഘടന sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ഘടന sbi_irqchip_handler *h;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ ശൂന്യ;

	sbi_list_for_each_entry(h, &chip->handler_list, കണ്ണി) {
		എങ്കിൽ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->അക്കം_hwirq))
			തിരികെ h;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_set_hwirq_priv(ഘടന sbi_irqchip_device *chip, u32 hwirq, ശൂന്യം *priv)
{
	ഘടന sbi_irqchip_hwirq_data *data;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
	     തിരികെ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	തിരികെ 0;
}

ശൂന്യം *sbi_irqchip_get_hwirq_priv(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന sbi_irqchip_hwirq_data *data;
	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
	    തിരികെ ശൂന്യ;

	data = &chip->hwirqs[hwirq];

	തിരികെ data->priv;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_raw_handler_default(ഘടന sbi_irqchip_device *chip, u32 hwirq)
{
	ഘടന sbi_irqchip_handler *h;
	പൂർണ്ണസംഖ്യ rc = SBI_OK;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	എങ്കിൽ (!h)
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (h->callback)
		rc = h->callback(hwirq, h->priv);

	എങ്കിൽ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	തിരികെ rc;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_set_raw_handler(ഘടന sbi_irqchip_device *chip, u32 hwirq,
				പൂർണ്ണസംഖ്യ (*raw_hndl)(ഘടന sbi_irqchip_device *, u32))
{
	ഘടന sbi_irqchip_hwirq_data *data;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_write_msi(ഘടന sbi_irqchip_device *chip, u32 hwirq,
			  സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം)
{
	ഘടന sbi_irqchip_handler *h;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq || !സന്ദേശം)
		തിരികെ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	എങ്കിൽ (!h)
		തിരികെ SBI_EFAIL;
	എങ്കിൽ (!h->write_msi)
		തിരികെ SBI_ENOTSUPP;

	h->write_msi(hwirq, സന്ദേശം, h->priv);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_get_affinity(ഘടന sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq)
		തിരികെ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	എങ്കിൽ (!sbi_irqchip_find_handler(chip, hwirq))
		തിരികെ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_set_affinity(ഘടന sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	ഘടന sbi_irqchip_hwirq_data *data;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (!chip || chip->അക്കം_hwirq <= hwirq || sbi_hart_എണ്ണം() <= hart_index)
		തിരികെ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	എങ്കിൽ (!sbi_irqchip_find_handler(chip, hwirq))
		തിരികെ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	എങ്കിൽ (data->hart_index != hart_index) {
		എങ്കിൽ (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			എങ്കിൽ (rc)
				തിരികെ rc;
		}
		data->hart_index = hart_index;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __sbi_irqchip_handler_set_affinity(ഘടന sbi_irqchip_device *chip,
					      ഘടന sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	പൂർണ്ണസംഖ്യ rc;

	വേണ്ടി (i = 0; i < h->അക്കം_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		എങ്കിൽ (rc)
			തിരികെ rc;

		എങ്കിൽ (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			തുടരുക;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __sbi_irqchip_register_handler(ഘടന sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 അക്കം_hwirq, u32 hwirq_flags,
					  ശൂന്യം (*write_msi)(u32 hwirq,
							    സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം,
							    ശൂന്യം *priv),
					  പൂർണ്ണസംഖ്യ (*callback)(u32 hwirq, ശൂന്യം *priv), ശൂന്യം *priv)
{
	ഘടന sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	പൂർണ്ണസംഖ്യ rc;

	വേണ്ടി (i = first_hwirq; i < (first_hwirq + അക്കം_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		എങ്കിൽ (h)
			തിരികെ SBI_EALREADY;
	}

	h = sbi_zalloc(വലുപ്പം(*h));
	എങ്കിൽ (!h)
		തിരികെ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->അക്കം_hwirq = അക്കം_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ശൂന്യ;
	sbi_list_for_each_entry(th, &chip->handler_list, കണ്ണി) {
		എങ്കിൽ (h->first_hwirq < th->first_hwirq) {
			nh = th;
			നിർത്തുക;
		}
	}
	എങ്കിൽ (nh)
		sbi_list_add(&h->കണ്ണി, &nh->കണ്ണി);
	അല്ലെങ്കിൽ
		sbi_list_add_tail(&h->കണ്ണി, &chip->handler_list);

	എങ്കിൽ (chip->hwirq_setup) {
		വേണ്ടി (i = 0; i < h->അക്കം_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			എങ്കിൽ (rc) {
				എങ്കിൽ (chip->hwirq_cleanup) {
					വേണ്ടി (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->കണ്ണി);
				sbi_free(h);
				തിരികെ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	എങ്കിൽ (rc) {
		എങ്കിൽ (chip->hwirq_cleanup) {
			വേണ്ടി (i = 0; i < h->അക്കം_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->കണ്ണി);
		sbi_free(h);
		തിരികെ rc;
	}

	എങ്കിൽ (chip->hwirq_unmask) {
		വേണ്ടി (i = 0; i < h->അക്കം_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_register_msi(ഘടന sbi_irqchip_device *chip, u32 അക്കം_hwirq,
			     ശൂന്യം (*write_msi)(u32 hwirq,
					       സ്ഥിരം ഘടന sbi_irqchip_msi_സന്ദേശം *സന്ദേശം,
					       ശൂന്യം *priv),
			     പൂർണ്ണസംഖ്യ (*callback)(u32 hwirq, ശൂന്യം *priv), ശൂന്യം *priv,
			     u32 *out_first_hwirq)
{
	ഘടന sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	എങ്കിൽ (!chip || !chip->hwirq_set_affinity || !അക്കം_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (chip->അക്കം_hwirq < അക്കം_hwirq)
		തിരികെ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, കണ്ണി) {
		എങ്കിൽ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->അക്കം_hwirq)) {
			hwirq = h->first_hwirq + h->അക്കം_hwirq;
		} അല്ലെങ്കിൽ എങ്കിൽ (hwirq < h->first_hwirq) {
			എങ്കിൽ (h->first_hwirq - hwirq >= അക്കം_hwirq) {
				found = true;
				നിർത്തുക;
			} അല്ലെങ്കിൽ {
				hwirq = h->first_hwirq + h->അക്കം_hwirq;
			}
		}
	}
	എങ്കിൽ (!found && (chip->അക്കം_hwirq - hwirq) >= അക്കം_hwirq)
		found = true;
	എങ്കിൽ (!found)
		തിരികെ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	തിരികെ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      അക്കം_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

പൂർണ്ണസംഖ്യ sbi_irqchip_register_handler(ഘടന sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 അക്കം_hwirq, u32 hwirq_flags,
				 പൂർണ്ണസംഖ്യ (*callback)(u32 hwirq, ശൂന്യം *priv), ശൂന്യം *priv)
{
	എങ്കിൽ (!chip || !അക്കം_hwirq || !callback)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (chip->അക്കം_hwirq <= first_hwirq ||
	    chip->അക്കം_hwirq <= (first_hwirq + അക്കം_hwirq - 1))
		തിരികെ SBI_EBAD_RANGE;

	തിരികെ __sbi_irqchip_register_handler(chip, first_hwirq, അക്കം_hwirq, hwirq_flags,
					      ശൂന്യ, callback, priv);
}

പൂർണ്ണസംഖ്യ sbi_irqchip_register_reserved(ഘടന sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 അക്കം_hwirq)
{
	എങ്കിൽ (!chip || !അക്കം_hwirq)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (chip->അക്കം_hwirq <= first_hwirq ||
	    chip->അക്കം_hwirq <= (first_hwirq + അക്കം_hwirq - 1))
		തിരികെ SBI_EBAD_RANGE;

	തിരികെ __sbi_irqchip_register_handler(chip, first_hwirq, അക്കം_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ശൂന്യ, ശൂന്യ, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ sbi_irqchip_unregister_handler(ഘടന sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 അക്കം_hwirq)
{
	ഘടന sbi_irqchip_handler *fh, *lh;
	u32 i;

	എങ്കിൽ (!chip || !അക്കം_hwirq)
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (chip->അക്കം_hwirq <= first_hwirq ||
	    chip->അക്കം_hwirq <= (first_hwirq + അക്കം_hwirq - 1))
		തിരികെ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	എങ്കിൽ (!fh || fh->first_hwirq != first_hwirq || fh->അക്കം_hwirq != അക്കം_hwirq)
		തിരികെ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + അക്കം_hwirq - 1);
	എങ്കിൽ (!lh || lh != fh)
		തിരികെ SBI_ENODEV;

	എങ്കിൽ (chip->hwirq_mask) {
		വേണ്ടി (i = 0; i < fh->അക്കം_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	എങ്കിൽ (chip->hwirq_cleanup) {
		വേണ്ടി (i = 0; i < fh->അക്കം_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->കണ്ണി);
	തിരികെ 0;
}

ഘടന sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   ഘടന sbi_irqchip_device *first)
{
	ഘടന sbi_irqchip_device *chip;
	bool found = (first == ശൂന്യ);

	sbi_list_for_each_entry(chip, &irqchip_list, കണ്ണി) {
		എങ്കിൽ (!found) {
			എങ്കിൽ (first == chip)
				found = true;
			തുടരുക;
		}
		എങ്കിൽ ((chip->caps & caps) == caps)
			തിരികെ chip;
	}

	തിരികെ ശൂന്യ;
}

ഘടന sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	ഘടന sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, കണ്ണി) {
		എങ്കിൽ (chip->id == id)
			തിരികെ chip;
	}

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_add_device(ഘടന sbi_irqchip_device *chip)
{
	ഘടന sbi_irqchip_hart_data *hd;
	ഘടന sbi_scratch *scratch;
	u32 i, h;

	എങ്കിൽ (!chip || !chip->അക്കം_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		തിരികെ SBI_EINVAL;

	എങ്കിൽ (sbi_irqchip_find_device(chip->id))
		തിരികെ SBI_EALREADY;

	എങ്കിൽ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			എങ്കിൽ (!scratch)
				തുടരുക;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			എങ്കിൽ (hd->chip && hd->chip != chip)
				തിരികെ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(വലുപ്പം(*chip->hwirqs) * chip->അക്കം_hwirq);
	എങ്കിൽ (!chip->hwirqs)
		തിരികെ SBI_ENOMEM;
	വേണ്ടി (i = 0; i < chip->അക്കം_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->കണ്ണി, &irqchip_list);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sbi_irqchip_init(ഘടന sbi_scratch *scratch, bool cold_boot)
{
	സ്ഥിരം ഘടന sbi_platform *plat = sbi_platform_ptr(scratch);
	ഘടന sbi_irqchip_hart_data *hd;
	ഘടന sbi_irqchip_device *chip;
	പൂർണ്ണസംഖ്യ rc;

	എങ്കിൽ (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(വലുപ്പം(ഘടന sbi_irqchip_hart_data));
		എങ്കിൽ (!irqchip_hart_data_off)
			തിരികെ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, കണ്ണി) {
		എങ്കിൽ (!chip->warm_init)
			തുടരുക;
		എങ്കിൽ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			തുടരുക;
		rc = chip->warm_init(chip);
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	എങ്കിൽ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	തിരികെ 0;
}

ശൂന്യം sbi_irqchip_exit(ഘടന sbi_scratch *scratch)
{
	ഘടന sbi_irqchip_hart_data *hd;
	ഘടന sbi_irqchip_device *chip;
	ഘടന sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	പൂർണ്ണസംഖ്യ rc;

	sbi_for_each_hartindex(i) {
		എങ്കിൽ (i == current_hartindex())
			തുടരുക;
		എങ്കിൽ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			തുടരുക;
		migrate_hidx = i;
		migrate = true;
		നിർത്തുക;
	}

	എങ്കിൽ (!migrate)
		പോകുക skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, കണ്ണി) {
		sbi_list_for_each_entry(h, &chip->handler_list, കണ്ണി) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			എങ്കിൽ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	എങ്കിൽ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
