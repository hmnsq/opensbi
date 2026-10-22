/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_irqchip.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
sa_nracanaa sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	puur_naa_nka (*raw_handler)(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	void *priv;
};

/** Internal irqchip interrupt handler */
sa_nracanaa sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	sa_nracanaa sbi_dlist node;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 num_hwirq;

	/** Write MSI function of this handler */
	void (*write_msi)(u32 hwirq, const sa_nracanaa sbi_irqchip_msi_msg *msg, void *priv);

	/** Callback function of this handler */
	puur_naa_nka (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

sa_nracanaa sbi_irqchip_hart_data {
	sa_nracanaa sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

puur_naa_nka sbi_irqchip_process(void)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ꦪꦼꦤ꧀ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ꦮꦭꦶ SBI_ENODEV;

	ꦮꦭꦶ hd->chip->process_hwirqs(hd->chip);
}

puur_naa_nka sbi_irqchip_process_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ꦪꦼꦤ꧀ (!data->raw_handler)
		ꦮꦭꦶ SBI_ENOENT;

	ꦮꦭꦶ data->raw_handler(chip, hwirq);
}

static anta_hstha u32 sbi_irqchip_get_irq_state(sa_nracanaa sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ꦪꦼꦤ꧀ (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		ꦮꦭꦶ 0;

	ꦮꦭꦶ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(sa_nracanaa sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ꦮꦭꦶ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

puur_naa_nka sbi_irqchip_unmask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ꦪꦼꦤ꧀ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ꦮꦭꦶ SBI_EALREADY;

	ꦪꦼꦤ꧀ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_mask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	ꦪꦼꦤ꧀ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ꦮꦭꦶ SBI_EALREADY;

	ꦪꦼꦤ꧀ (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ꦮꦭꦶ 0;
}

static sa_nracanaa sbi_irqchip_handler *sbi_irqchip_find_handler(sa_nracanaa sbi_irqchip_device *chip,
							    u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ꦪꦼꦤ꧀ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			ꦮꦭꦶ h;
	}

	ꦮꦭꦶ NULL;
}

puur_naa_nka sbi_irqchip_set_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
	     ꦮꦭꦶ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ꦮꦭꦶ 0;
}

void *sbi_irqchip_get_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
	    ꦮꦭꦶ NULL;

	data = &chip->hwirqs[hwirq];

	ꦮꦭꦶ data->priv;
}

puur_naa_nka sbi_irqchip_raw_handler_default(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	puur_naa_nka rc = SBI_OK;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ꦪꦼꦤ꧀ (!h)
		ꦮꦭꦶ SBI_EINVAL;

	ꦪꦼꦤ꧀ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ꦪꦼꦤ꧀ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ꦮꦭꦶ rc;
}

puur_naa_nka sbi_irqchip_set_raw_handler(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
				puur_naa_nka (*raw_hndl)(sa_nracanaa sbi_irqchip_device *, u32))
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_write_msi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			  const sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq || !msg)
		ꦮꦭꦶ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ꦪꦼꦤ꧀ (!h)
		ꦮꦭꦶ SBI_EFAIL;
	ꦪꦼꦤ꧀ (!h->write_msi)
		ꦮꦭꦶ SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_get_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq)
		ꦮꦭꦶ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ꦪꦼꦤ꧀ (!sbi_irqchip_find_handler(chip, hwirq))
		ꦮꦭꦶ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_set_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	puur_naa_nka rc;

	ꦪꦼꦤ꧀ (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		ꦮꦭꦶ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ꦪꦼꦤ꧀ (!sbi_irqchip_find_handler(chip, hwirq))
		ꦮꦭꦶ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ꦪꦼꦤ꧀ (data->hart_index != hart_index) {
		ꦪꦼꦤ꧀ (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ꦪꦼꦤ꧀ (rc)
				ꦮꦭꦶ rc;
		}
		data->hart_index = hart_index;
	}

	ꦮꦭꦶ 0;
}

static puur_naa_nka __sbi_irqchip_handler_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
					      sa_nracanaa sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	puur_naa_nka rc;

	ꦏꦁꦒꦺꦴ (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;

		ꦪꦼꦤ꧀ (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;
	}

	ꦮꦭꦶ 0;
}

static puur_naa_nka __sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const sa_nracanaa sbi_irqchip_msi_msg *msg,
							    void *priv),
					  puur_naa_nka (*callback)(u32 hwirq, void *priv), void *priv)
{
	sa_nracanaa sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	puur_naa_nka rc;

	ꦏꦁꦒꦺꦴ (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ꦪꦼꦤ꧀ (h)
			ꦮꦭꦶ SBI_EALREADY;
	}

	h = sbi_zalloc(parimaa_na(*h));
	ꦪꦼꦤ꧀ (!h)
		ꦮꦭꦶ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		ꦪꦼꦤ꧀ (h->first_hwirq < th->first_hwirq) {
			nh = th;
			bha_nga;
		}
	}
	ꦪꦼꦤ꧀ (nh)
		sbi_list_add(&h->node, &nh->node);
	ꦭꦶꦪ
		sbi_list_add_tail(&h->node, &chip->handler_list);

	ꦪꦼꦤ꧀ (chip->hwirq_setup) {
		ꦏꦁꦒꦺꦴ (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ꦪꦼꦤ꧀ (rc) {
				ꦪꦼꦤ꧀ (chip->hwirq_cleanup) {
					ꦏꦁꦒꦺꦴ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				ꦮꦭꦶ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ꦪꦼꦤ꧀ (rc) {
		ꦪꦼꦤ꧀ (chip->hwirq_cleanup) {
			ꦏꦁꦒꦺꦴ (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		ꦮꦭꦶ rc;
	}

	ꦪꦼꦤ꧀ (chip->hwirq_unmask) {
		ꦏꦁꦒꦺꦴ (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_register_msi(sa_nracanaa sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const sa_nracanaa sbi_irqchip_msi_msg *msg,
					       void *priv),
			     puur_naa_nka (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	ꦪꦼꦤ꧀ (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (chip->num_hwirq < num_hwirq)
		ꦮꦭꦶ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ꦪꦼꦤ꧀ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (hwirq < h->first_hwirq) {
			ꦪꦼꦤ꧀ (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				bha_nga;
			} ꦭꦶꦪ {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	ꦪꦼꦤ꧀ (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	ꦪꦼꦤ꧀ (!found)
		ꦮꦭꦶ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ꦮꦭꦶ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

puur_naa_nka sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 puur_naa_nka (*callback)(u32 hwirq, void *priv), void *priv)
{
	ꦪꦼꦤ꧀ (!chip || !num_hwirq || !callback)
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ꦮꦭꦶ SBI_EBAD_RANGE;

	ꦮꦭꦶ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

puur_naa_nka sbi_irqchip_register_reserved(sa_nracanaa sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	ꦪꦼꦤ꧀ (!chip || !num_hwirq)
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ꦮꦭꦶ SBI_EBAD_RANGE;

	ꦮꦭꦶ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

puur_naa_nka sbi_irqchip_unregister_handler(sa_nracanaa sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *fh, *lh;
	u32 i;

	ꦪꦼꦤ꧀ (!chip || !num_hwirq)
		ꦮꦭꦶ SBI_EINVAL;
	ꦪꦼꦤ꧀ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ꦮꦭꦶ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ꦪꦼꦤ꧀ (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		ꦮꦭꦶ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	ꦪꦼꦤ꧀ (!lh || lh != fh)
		ꦮꦭꦶ SBI_ENODEV;

	ꦪꦼꦤ꧀ (chip->hwirq_mask) {
		ꦏꦁꦒꦺꦴ (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ꦪꦼꦤ꧀ (chip->hwirq_cleanup) {
		ꦏꦁꦒꦺꦴ (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	ꦮꦭꦶ 0;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   sa_nracanaa sbi_irqchip_device *first)
{
	sa_nracanaa sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ꦪꦼꦤ꧀ (!found) {
			ꦪꦼꦤ꧀ (first == chip)
				found = true;
			continue;
		}
		ꦪꦼꦤ꧀ ((chip->caps & caps) == caps)
			ꦮꦭꦶ chip;
	}

	ꦮꦭꦶ NULL;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	sa_nracanaa sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ꦪꦼꦤ꧀ (chip->id == id)
			ꦮꦭꦶ chip;
	}

	ꦮꦭꦶ NULL;
}

puur_naa_nka sbi_irqchip_add_device(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_scratch *scratch;
	u32 i, h;

	ꦪꦼꦤ꧀ (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ꦮꦭꦶ SBI_EINVAL;

	ꦪꦼꦤ꧀ (sbi_irqchip_find_device(chip->id))
		ꦮꦭꦶ SBI_EALREADY;

	ꦪꦼꦤ꧀ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ꦪꦼꦤ꧀ (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ꦪꦼꦤ꧀ (hd->chip && hd->chip != chip)
				ꦮꦭꦶ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(parimaa_na(*chip->hwirqs) * chip->num_hwirq);
	ꦪꦼꦤ꧀ (!chip->hwirqs)
		ꦮꦭꦶ SBI_ENOMEM;
	ꦏꦁꦒꦺꦴ (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	ꦮꦭꦶ 0;
}

puur_naa_nka sbi_irqchip_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	puur_naa_nka rc;

	ꦪꦼꦤ꧀ (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(parimaa_na(sa_nracanaa sbi_irqchip_hart_data));
		ꦪꦼꦤ꧀ (!irqchip_hart_data_off)
			ꦮꦭꦶ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ꦪꦼꦤ꧀ (!chip->warm_init)
			continue;
		ꦪꦼꦤ꧀ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		ꦪꦼꦤ꧀ (rc)
			ꦮꦭꦶ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ꦪꦼꦤ꧀ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ꦮꦭꦶ 0;
}

void sbi_irqchip_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	sa_nracanaa sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	puur_naa_nka rc;

	sbi_for_each_hartindex(i) {
		ꦪꦼꦤ꧀ (i == current_hartindex())
			continue;
		ꦪꦼꦤ꧀ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		bha_nga;
	}

	ꦪꦼꦤ꧀ (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ꦪꦼꦤ꧀ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ꦪꦼꦤ꧀ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
