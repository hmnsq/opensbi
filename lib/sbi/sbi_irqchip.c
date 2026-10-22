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
	ပေႃး (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ပွၵ်ႈ SBI_ENODEV;

	ပွၵ်ႈ hd->chip->process_hwirqs(hd->chip);
}

puur_naa_nka sbi_irqchip_process_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ပေႃး (!data->raw_handler)
		ပွၵ်ႈ SBI_ENOENT;

	ပွၵ်ႈ data->raw_handler(chip, hwirq);
}

static anta_hstha u32 sbi_irqchip_get_irq_state(sa_nracanaa sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ပေႃး (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		ပွၵ်ႈ 0;

	ပွၵ်ႈ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(sa_nracanaa sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ပွၵ်ႈ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

puur_naa_nka sbi_irqchip_unmask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ပေႃး (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ပွၵ်ႈ SBI_EALREADY;

	ပေႃး (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_irqchip_mask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ပွၵ်ႈ SBI_EALREADY;

	ပေႃး (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ပွၵ်ႈ 0;
}

static sa_nracanaa sbi_irqchip_handler *sbi_irqchip_find_handler(sa_nracanaa sbi_irqchip_device *chip,
							    u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ပေႃး (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			ပွၵ်ႈ h;
	}

	ပွၵ်ႈ NULL;
}

puur_naa_nka sbi_irqchip_set_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
	     ပွၵ်ႈ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ပွၵ်ႈ 0;
}

void *sbi_irqchip_get_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	ပေႃး (!chip || chip->num_hwirq <= hwirq)
	    ပွၵ်ႈ NULL;

	data = &chip->hwirqs[hwirq];

	ပွၵ်ႈ data->priv;
}

puur_naa_nka sbi_irqchip_raw_handler_default(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	puur_naa_nka rc = SBI_OK;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ပေႃး (!h)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (h->callback)
		rc = h->callback(hwirq, h->priv);

	ပေႃး (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ပွၵ်ႈ rc;
}

puur_naa_nka sbi_irqchip_set_raw_handler(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
				puur_naa_nka (*raw_hndl)(sa_nracanaa sbi_irqchip_device *, u32))
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_irqchip_write_msi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			  const sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ပေႃး (!chip || chip->num_hwirq <= hwirq || !msg)
		ပွၵ်ႈ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ပေႃး (!h)
		ပွၵ်ႈ SBI_EFAIL;
	ပေႃး (!h->write_msi)
		ပွၵ်ႈ SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_irqchip_get_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ပေႃး (!chip || chip->num_hwirq <= hwirq)
		ပွၵ်ႈ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ပေႃး (!sbi_irqchip_find_handler(chip, hwirq))
		ပွၵ်ႈ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_irqchip_set_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	puur_naa_nka rc;

	ပေႃး (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		ပွၵ်ႈ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ပေႃး (!sbi_irqchip_find_handler(chip, hwirq))
		ပွၵ်ႈ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ပေႃး (data->hart_index != hart_index) {
		ပေႃး (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ပေႃး (rc)
				ပွၵ်ႈ rc;
		}
		data->hart_index = hart_index;
	}

	ပွၵ်ႈ 0;
}

static puur_naa_nka __sbi_irqchip_handler_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
					      sa_nracanaa sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	puur_naa_nka rc;

	for (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ပေႃး (rc)
			ပွၵ်ႈ rc;

		ပေႃး (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ပေႃး (rc)
			ပွၵ်ႈ rc;
	}

	ပွၵ်ႈ 0;
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

	for (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ပေႃး (h)
			ပွၵ်ႈ SBI_EALREADY;
	}

	h = sbi_zalloc(parimaa_na(*h));
	ပေႃး (!h)
		ပွၵ်ႈ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		ပေႃး (h->first_hwirq < th->first_hwirq) {
			nh = th;
			bha_nga;
		}
	}
	ပေႃး (nh)
		sbi_list_add(&h->node, &nh->node);
	ပေႃးဢမ်ႇ
		sbi_list_add_tail(&h->node, &chip->handler_list);

	ပေႃး (chip->hwirq_setup) {
		for (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ပေႃး (rc) {
				ပေႃး (chip->hwirq_cleanup) {
					for (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				ပွၵ်ႈ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ပေႃး (rc) {
		ပေႃး (chip->hwirq_cleanup) {
			for (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		ပွၵ်ႈ rc;
	}

	ပေႃး (chip->hwirq_unmask) {
		for (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ပွၵ်ႈ 0;
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

	ပေႃး (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (chip->num_hwirq < num_hwirq)
		ပွၵ်ႈ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ပေႃး (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} ပေႃးဢမ်ႇ ပေႃး (hwirq < h->first_hwirq) {
			ပေႃး (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				bha_nga;
			} ပေႃးဢမ်ႇ {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	ပေႃး (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	ပေႃး (!found)
		ပွၵ်ႈ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ပွၵ်ႈ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

puur_naa_nka sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 puur_naa_nka (*callback)(u32 hwirq, void *priv), void *priv)
{
	ပေႃး (!chip || !num_hwirq || !callback)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ပွၵ်ႈ SBI_EBAD_RANGE;

	ပွၵ်ႈ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

puur_naa_nka sbi_irqchip_register_reserved(sa_nracanaa sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	ပေႃး (!chip || !num_hwirq)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ပွၵ်ႈ SBI_EBAD_RANGE;

	ပွၵ်ႈ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

puur_naa_nka sbi_irqchip_unregister_handler(sa_nracanaa sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *fh, *lh;
	u32 i;

	ပေႃး (!chip || !num_hwirq)
		ပွၵ်ႈ SBI_EINVAL;
	ပေႃး (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ပွၵ်ႈ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ပေႃး (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		ပွၵ်ႈ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	ပေႃး (!lh || lh != fh)
		ပွၵ်ႈ SBI_ENODEV;

	ပေႃး (chip->hwirq_mask) {
		for (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ပေႃး (chip->hwirq_cleanup) {
		for (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	ပွၵ်ႈ 0;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   sa_nracanaa sbi_irqchip_device *first)
{
	sa_nracanaa sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ပေႃး (!found) {
			ပေႃး (first == chip)
				found = true;
			continue;
		}
		ပေႃး ((chip->caps & caps) == caps)
			ပွၵ်ႈ chip;
	}

	ပွၵ်ႈ NULL;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	sa_nracanaa sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ပေႃး (chip->id == id)
			ပွၵ်ႈ chip;
	}

	ပွၵ်ႈ NULL;
}

puur_naa_nka sbi_irqchip_add_device(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_scratch *scratch;
	u32 i, h;

	ပေႃး (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (sbi_irqchip_find_device(chip->id))
		ပွၵ်ႈ SBI_EALREADY;

	ပေႃး (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ပေႃး (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ပေႃး (hd->chip && hd->chip != chip)
				ပွၵ်ႈ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(parimaa_na(*chip->hwirqs) * chip->num_hwirq);
	ပေႃး (!chip->hwirqs)
		ပွၵ်ႈ SBI_ENOMEM;
	for (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	ပွၵ်ႈ 0;
}

puur_naa_nka sbi_irqchip_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	puur_naa_nka rc;

	ပေႃး (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(parimaa_na(sa_nracanaa sbi_irqchip_hart_data));
		ပေႃး (!irqchip_hart_data_off)
			ပွၵ်ႈ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ပေႃး (rc)
			ပွၵ်ႈ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ပေႃး (!chip->warm_init)
			continue;
		ပေႃး (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		ပေႃး (rc)
			ပွၵ်ႈ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ပေႃး (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ပွၵ်ႈ 0;
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
		ပေႃး (i == current_hartindex())
			continue;
		ပေႃး (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		bha_nga;
	}

	ပေႃး (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ပေႃး (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ပေႃး (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
