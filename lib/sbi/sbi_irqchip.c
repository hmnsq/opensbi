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
	ຈຳນວນເຕັມ (*raw_handler)(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq);

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
	ຈຳນວນເຕັມ (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

sa_nracanaa sbi_irqchip_hart_data {
	sa_nracanaa sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

ຈຳນວນເຕັມ sbi_irqchip_process(void)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ຖ້າ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ສົ່ງຄືນ SBI_ENODEV;

	ສົ່ງຄືນ hd->chip->process_hwirqs(hd->chip);
}

ຈຳນວນເຕັມ sbi_irqchip_process_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ຖ້າ (!data->raw_handler)
		ສົ່ງຄືນ SBI_ENOENT;

	ສົ່ງຄືນ data->raw_handler(chip, hwirq);
}

static anta_hstha u32 sbi_irqchip_get_irq_state(sa_nracanaa sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ຖ້າ (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		ສົ່ງຄືນ 0;

	ສົ່ງຄືນ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(sa_nracanaa sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ສົ່ງຄືນ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ຈຳນວນເຕັມ sbi_irqchip_unmask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ຖ້າ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ສົ່ງຄືນ SBI_EALREADY;

	ຖ້າ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_mask_hwirq(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ສົ່ງຄືນ SBI_EALREADY;

	ຖ້າ (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ສົ່ງຄືນ 0;
}

static sa_nracanaa sbi_irqchip_handler *sbi_irqchip_find_handler(sa_nracanaa sbi_irqchip_device *chip,
							    u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ຖ້າ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			ສົ່ງຄືນ h;
	}

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ sbi_irqchip_set_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
	     ສົ່ງຄືນ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ສົ່ງຄືນ 0;
}

void *sbi_irqchip_get_hwirq_priv(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
	    ສົ່ງຄືນ NULL;

	data = &chip->hwirqs[hwirq];

	ສົ່ງຄືນ data->priv;
}

ຈຳນວນເຕັມ sbi_irqchip_raw_handler_default(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	ຈຳນວນເຕັມ rc = SBI_OK;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ຖ້າ (!h)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ຖ້າ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ສົ່ງຄືນ rc;
}

ຈຳນວນເຕັມ sbi_irqchip_set_raw_handler(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
				ຈຳນວນເຕັມ (*raw_hndl)(sa_nracanaa sbi_irqchip_device *, u32))
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_write_msi(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			  const sa_nracanaa sbi_irqchip_msi_msg *msg)
{
	sa_nracanaa sbi_irqchip_handler *h;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq || !msg)
		ສົ່ງຄືນ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ຖ້າ (!h)
		ສົ່ງຄືນ SBI_EFAIL;
	ຖ້າ (!h->write_msi)
		ສົ່ງຄືນ SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_get_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ຖ້າ (!chip || chip->num_hwirq <= hwirq)
		ສົ່ງຄືນ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ຖ້າ (!sbi_irqchip_find_handler(chip, hwirq))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_set_affinity(sa_nracanaa sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	sa_nracanaa sbi_irqchip_hwirq_data *data;
	ຈຳນວນເຕັມ rc;

	ຖ້າ (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		ສົ່ງຄືນ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ຖ້າ (!sbi_irqchip_find_handler(chip, hwirq))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ຖ້າ (data->hart_index != hart_index) {
		ຖ້າ (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ຖ້າ (rc)
				ສົ່ງຄືນ rc;
		}
		data->hart_index = hart_index;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ __sbi_irqchip_handler_set_affinity(sa_nracanaa sbi_irqchip_device *chip,
					      sa_nracanaa sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	ຈຳນວນເຕັມ rc;

	ສຳລັບ (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;

		ຖ້າ (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ __sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const sa_nracanaa sbi_irqchip_msi_msg *msg,
							    void *priv),
					  ຈຳນວນເຕັມ (*callback)(u32 hwirq, void *priv), void *priv)
{
	sa_nracanaa sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ຈຳນວນເຕັມ rc;

	ສຳລັບ (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ຖ້າ (h)
			ສົ່ງຄືນ SBI_EALREADY;
	}

	h = sbi_zalloc(parimaa_na(*h));
	ຖ້າ (!h)
		ສົ່ງຄືນ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		ຖ້າ (h->first_hwirq < th->first_hwirq) {
			nh = th;
			bha_nga;
		}
	}
	ຖ້າ (nh)
		sbi_list_add(&h->node, &nh->node);
	ບໍ່ດັ່ງນັ້ນ
		sbi_list_add_tail(&h->node, &chip->handler_list);

	ຖ້າ (chip->hwirq_setup) {
		ສຳລັບ (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ຖ້າ (rc) {
				ຖ້າ (chip->hwirq_cleanup) {
					ສຳລັບ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				ສົ່ງຄືນ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ຖ້າ (rc) {
		ຖ້າ (chip->hwirq_cleanup) {
			ສຳລັບ (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		ສົ່ງຄືນ rc;
	}

	ຖ້າ (chip->hwirq_unmask) {
		ສຳລັບ (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_register_msi(sa_nracanaa sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const sa_nracanaa sbi_irqchip_msi_msg *msg,
					       void *priv),
			     ຈຳນວນເຕັມ (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	ຖ້າ (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (chip->num_hwirq < num_hwirq)
		ສົ່ງຄືນ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ຖ້າ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (hwirq < h->first_hwirq) {
			ຖ້າ (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				bha_nga;
			} ບໍ່ດັ່ງນັ້ນ {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	ຖ້າ (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	ຖ້າ (!found)
		ສົ່ງຄືນ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ສົ່ງຄືນ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ຈຳນວນເຕັມ sbi_irqchip_register_handler(sa_nracanaa sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 ຈຳນວນເຕັມ (*callback)(u32 hwirq, void *priv), void *priv)
{
	ຖ້າ (!chip || !num_hwirq || !callback)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ສົ່ງຄືນ SBI_EBAD_RANGE;

	ສົ່ງຄືນ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

ຈຳນວນເຕັມ sbi_irqchip_register_reserved(sa_nracanaa sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	ຖ້າ (!chip || !num_hwirq)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ສົ່ງຄືນ SBI_EBAD_RANGE;

	ສົ່ງຄືນ __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

ຈຳນວນເຕັມ sbi_irqchip_unregister_handler(sa_nracanaa sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	sa_nracanaa sbi_irqchip_handler *fh, *lh;
	u32 i;

	ຖ້າ (!chip || !num_hwirq)
		ສົ່ງຄືນ SBI_EINVAL;
	ຖ້າ (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		ສົ່ງຄືນ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ຖ້າ (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		ສົ່ງຄືນ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	ຖ້າ (!lh || lh != fh)
		ສົ່ງຄືນ SBI_ENODEV;

	ຖ້າ (chip->hwirq_mask) {
		ສຳລັບ (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ຖ້າ (chip->hwirq_cleanup) {
		ສຳລັບ (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	ສົ່ງຄືນ 0;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   sa_nracanaa sbi_irqchip_device *first)
{
	sa_nracanaa sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ຖ້າ (!found) {
			ຖ້າ (first == chip)
				found = true;
			continue;
		}
		ຖ້າ ((chip->caps & caps) == caps)
			ສົ່ງຄືນ chip;
	}

	ສົ່ງຄືນ NULL;
}

sa_nracanaa sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	sa_nracanaa sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ຖ້າ (chip->id == id)
			ສົ່ງຄືນ chip;
	}

	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ sbi_irqchip_add_device(sa_nracanaa sbi_irqchip_device *chip)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_scratch *scratch;
	u32 i, h;

	ຖ້າ (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (sbi_irqchip_find_device(chip->id))
		ສົ່ງຄືນ SBI_EALREADY;

	ຖ້າ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ຖ້າ (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ຖ້າ (hd->chip && hd->chip != chip)
				ສົ່ງຄືນ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(parimaa_na(*chip->hwirqs) * chip->num_hwirq);
	ຖ້າ (!chip->hwirqs)
		ສົ່ງຄືນ SBI_ENOMEM;
	ສຳລັບ (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sbi_irqchip_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	const sa_nracanaa sbi_platform *plat = sbi_platform_ptr(scratch);
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	ຈຳນວນເຕັມ rc;

	ຖ້າ (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(parimaa_na(sa_nracanaa sbi_irqchip_hart_data));
		ຖ້າ (!irqchip_hart_data_off)
			ສົ່ງຄືນ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ຖ້າ (!chip->warm_init)
			continue;
		ຖ້າ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		ຖ້າ (rc)
			ສົ່ງຄືນ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ຖ້າ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ສົ່ງຄືນ 0;
}

void sbi_irqchip_exit(sa_nracanaa sbi_scratch *scratch)
{
	sa_nracanaa sbi_irqchip_hart_data *hd;
	sa_nracanaa sbi_irqchip_device *chip;
	sa_nracanaa sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ຈຳນວນເຕັມ rc;

	sbi_for_each_hartindex(i) {
		ຖ້າ (i == current_hartindex())
			continue;
		ຖ້າ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		bha_nga;
	}

	ຖ້າ (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ຖ້າ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ຖ້າ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
