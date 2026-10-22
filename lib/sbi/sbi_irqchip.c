/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_irqchip.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_list.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
ಸಂರಚನಾ sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	ಪೂರ್ಣಾಙ್ಕ (*raw_handler)(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	ಶೂನ್ಯ *priv;
};

/** Internal irqchip interrupt handler */
ಸಂರಚನಾ sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ಸಂರಚನಾ sbi_dlist ಗಂಟು;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 ಅಂಕೆ_hwirq;

	/** Write MSI function of this handler */
	ಶೂನ್ಯ (*write_msi)(u32 hwirq, ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ, ಶೂನ್ಯ *priv);

	/** Callback function of this handler */
	ಪೂರ್ಣಾಙ್ಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv);

	/** Callback private data */
	ಶೂನ್ಯ *priv;
};

ಸಂರಚನಾ sbi_irqchip_hart_data {
	ಸಂರಚನಾ sbi_irqchip_device *chip;
};

ಸ್ಥಿರ unsigned long irqchip_hart_data_off;
ಸ್ಥಿರ SBI_LIST_HEAD(irqchip_list);

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_process(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ಯದಿ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಪ್ರತ್ಯಾಗಮ hd->chip->process_hwirqs(hd->chip);
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_process_hwirq(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ಯದಿ (!data->raw_handler)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	ಪ್ರತ್ಯಾಗಮ data->raw_handler(chip, hwirq);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ u32 sbi_irqchip_get_irq_state(ಸಂರಚನಾ sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ಯದಿ (!chip || !chip->hwirqs || hwirq >= chip->ಅಂಕೆ_hwirq)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ಸಂರಚನಾ sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ಪ್ರತ್ಯಾಗಮ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_unmask_hwirq(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ಯದಿ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	ಯದಿ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_mask_hwirq(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	ಯದಿ (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_irqchip_handler *sbi_irqchip_find_handler(ಸಂರಚನಾ sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_handler *h;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	sbi_list_for_each_entry(h, &chip->handler_list, ಗಂಟು) {
		ಯದಿ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->ಅಂಕೆ_hwirq))
			ಪ್ರತ್ಯಾಗಮ h;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_set_hwirq_priv(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq, ಶೂನ್ಯ *priv)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
	     ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ *sbi_irqchip_get_hwirq_priv(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;
	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	data = &chip->hwirqs[hwirq];

	ಪ್ರತ್ಯಾಗಮ data->priv;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_raw_handler_default(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_handler *h;
	ಪೂರ್ಣಾಙ್ಕ rc = SBI_OK;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ಯದಿ (!h)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ಯದಿ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_set_raw_handler(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq,
				ಪೂರ್ಣಾಙ್ಕ (*raw_hndl)(ಸಂರಚನಾ sbi_irqchip_device *, u32))
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_write_msi(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq,
			  ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	ಸಂರಚನಾ sbi_irqchip_handler *h;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq || !ಸಂದೇಶ)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ಯದಿ (!h)
		ಪ್ರತ್ಯಾಗಮ SBI_EFAIL;
	ಯದಿ (!h->write_msi)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	h->write_msi(hwirq, ಸಂದೇಶ, h->priv);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_get_affinity(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ಯದಿ (!sbi_irqchip_find_handler(chip, hwirq))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_set_affinity(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	ಸಂರಚನಾ sbi_irqchip_hwirq_data *data;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq || sbi_hart_ಎಣಿಕೆ() <= hart_index)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ಯದಿ (!sbi_irqchip_find_handler(chip, hwirq))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ಯದಿ (data->hart_index != hart_index) {
		ಯದಿ (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ಯದಿ (rc)
				ಪ್ರತ್ಯಾಗಮ rc;
		}
		data->hart_index = hart_index;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __sbi_irqchip_handler_set_affinity(ಸಂರಚನಾ sbi_irqchip_device *chip,
					      ಸಂರಚನಾ sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		ಯದಿ (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			ಅನುವರ್ತ;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __sbi_irqchip_register_handler(ಸಂರಚನಾ sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 ಅಂಕೆ_hwirq, u32 hwirq_flags,
					  ಶೂನ್ಯ (*write_msi)(u32 hwirq,
							    ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
							    ಶೂನ್ಯ *priv),
					  ಪೂರ್ಣಾಙ್ಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv)
{
	ಸಂರಚನಾ sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಪ್ರತಿ (i = first_hwirq; i < (first_hwirq + ಅಂಕೆ_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ಯದಿ (h)
			ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;
	}

	h = sbi_zalloc(ಪರಿಮಾಣ(*h));
	ಯದಿ (!h)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->ಅಂಕೆ_hwirq = ಅಂಕೆ_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ರಿಕ್ತ;
	sbi_list_for_each_entry(th, &chip->handler_list, ಗಂಟು) {
		ಯದಿ (h->first_hwirq < th->first_hwirq) {
			nh = th;
			ಭಙ್ಗ;
		}
	}
	ಯದಿ (nh)
		sbi_list_add(&h->ಗಂಟು, &nh->ಗಂಟು);
	ಅನ್ಯಥಾ
		sbi_list_add_tail(&h->ಗಂಟು, &chip->handler_list);

	ಯದಿ (chip->hwirq_setup) {
		ಪ್ರತಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ಯದಿ (rc) {
				ಯದಿ (chip->hwirq_cleanup) {
					ಪ್ರತಿ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->ಗಂಟು);
				sbi_free(h);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ಯದಿ (rc) {
		ಯದಿ (chip->hwirq_cleanup) {
			ಪ್ರತಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->ಗಂಟು);
		sbi_free(h);
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ (chip->hwirq_unmask) {
		ಪ್ರತಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_register_msi(ಸಂರಚನಾ sbi_irqchip_device *chip, u32 ಅಂಕೆ_hwirq,
			     ಶೂನ್ಯ (*write_msi)(u32 hwirq,
					       ಧ್ರುವ ಸಂರಚನಾ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
					       ಶೂನ್ಯ *priv),
			     ಪೂರ್ಣಾಙ್ಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv,
			     u32 *out_first_hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	ಯದಿ (!chip || !chip->hwirq_set_affinity || !ಅಂಕೆ_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (chip->ಅಂಕೆ_hwirq < ಅಂಕೆ_hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, ಗಂಟು) {
		ಯದಿ (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->ಅಂಕೆ_hwirq)) {
			hwirq = h->first_hwirq + h->ಅಂಕೆ_hwirq;
		} ಅನ್ಯಥಾ ಯದಿ (hwirq < h->first_hwirq) {
			ಯದಿ (h->first_hwirq - hwirq >= ಅಂಕೆ_hwirq) {
				found = true;
				ಭಙ್ಗ;
			} ಅನ್ಯಥಾ {
				hwirq = h->first_hwirq + h->ಅಂಕೆ_hwirq;
			}
		}
	}
	ಯದಿ (!found && (chip->ಅಂಕೆ_hwirq - hwirq) >= ಅಂಕೆ_hwirq)
		found = true;
	ಯದಿ (!found)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ಪ್ರತ್ಯಾಗಮ __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      ಅಂಕೆ_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_register_handler(ಸಂರಚನಾ sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 ಅಂಕೆ_hwirq, u32 hwirq_flags,
				 ಪೂರ್ಣಾಙ್ಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv)
{
	ಯದಿ (!chip || !ಅಂಕೆ_hwirq || !callback)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (chip->ಅಂಕೆ_hwirq <= first_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (first_hwirq + ಅಂಕೆ_hwirq - 1))
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	ಪ್ರತ್ಯಾಗಮ __sbi_irqchip_register_handler(chip, first_hwirq, ಅಂಕೆ_hwirq, hwirq_flags,
					      ರಿಕ್ತ, callback, priv);
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_register_reserved(ಸಂರಚನಾ sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 ಅಂಕೆ_hwirq)
{
	ಯದಿ (!chip || !ಅಂಕೆ_hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (chip->ಅಂಕೆ_hwirq <= first_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (first_hwirq + ಅಂಕೆ_hwirq - 1))
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	ಪ್ರತ್ಯಾಗಮ __sbi_irqchip_register_handler(chip, first_hwirq, ಅಂಕೆ_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_unregister_handler(ಸಂರಚನಾ sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 ಅಂಕೆ_hwirq)
{
	ಸಂರಚನಾ sbi_irqchip_handler *fh, *lh;
	u32 i;

	ಯದಿ (!chip || !ಅಂಕೆ_hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (chip->ಅಂಕೆ_hwirq <= first_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (first_hwirq + ಅಂಕೆ_hwirq - 1))
		ಪ್ರತ್ಯಾಗಮ SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ಯದಿ (!fh || fh->first_hwirq != first_hwirq || fh->ಅಂಕೆ_hwirq != ಅಂಕೆ_hwirq)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + ಅಂಕೆ_hwirq - 1);
	ಯದಿ (!lh || lh != fh)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	ಯದಿ (chip->hwirq_mask) {
		ಪ್ರತಿ (i = 0; i < fh->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ಯದಿ (chip->hwirq_cleanup) {
		ಪ್ರತಿ (i = 0; i < fh->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->ಗಂಟು);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   ಸಂರಚನಾ sbi_irqchip_device *first)
{
	ಸಂರಚನಾ sbi_irqchip_device *chip;
	bool found = (first == ರಿಕ್ತ);

	sbi_list_for_each_entry(chip, &irqchip_list, ಗಂಟು) {
		ಯದಿ (!found) {
			ಯದಿ (first == chip)
				found = true;
			ಅನುವರ್ತ;
		}
		ಯದಿ ((chip->caps & caps) == caps)
			ಪ್ರತ್ಯಾಗಮ chip;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸಂರಚನಾ sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	ಸಂರಚನಾ sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, ಗಂಟು) {
		ಯದಿ (chip->id == id)
			ಪ್ರತ್ಯಾಗಮ chip;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_add_device(ಸಂರಚನಾ sbi_irqchip_device *chip)
{
	ಸಂರಚನಾ sbi_irqchip_hart_data *hd;
	ಸಂರಚನಾ sbi_scratch *scratch;
	u32 i, h;

	ಯದಿ (!chip || !chip->ಅಂಕೆ_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ಯದಿ (sbi_irqchip_find_device(chip->id))
		ಪ್ರತ್ಯಾಗಮ SBI_EALREADY;

	ಯದಿ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ಯದಿ (!scratch)
				ಅನುವರ್ತ;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ಯದಿ (hd->chip && hd->chip != chip)
				ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(ಪರಿಮಾಣ(*chip->hwirqs) * chip->ಅಂಕೆ_hwirq);
	ಯದಿ (!chip->hwirqs)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	ಪ್ರತಿ (i = 0; i < chip->ಅಂಕೆ_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->ಗಂಟು, &irqchip_list);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sbi_irqchip_init(ಸಂರಚನಾ sbi_scratch *scratch, bool cold_boot)
{
	ಧ್ರುವ ಸಂರಚನಾ sbi_platform *plat = sbi_platform_ptr(scratch);
	ಸಂರಚನಾ sbi_irqchip_hart_data *hd;
	ಸಂರಚನಾ sbi_irqchip_device *chip;
	ಪೂರ್ಣಾಙ್ಕ rc;

	ಯದಿ (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(ಪರಿಮಾಣ(ಸಂರಚನಾ sbi_irqchip_hart_data));
		ಯದಿ (!irqchip_hart_data_off)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, ಗಂಟು) {
		ಯದಿ (!chip->warm_init)
			ಅನುವರ್ತ;
		ಯದಿ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			ಅನುವರ್ತ;
		rc = chip->warm_init(chip);
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ಯದಿ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sbi_irqchip_exit(ಸಂರಚನಾ sbi_scratch *scratch)
{
	ಸಂರಚನಾ sbi_irqchip_hart_data *hd;
	ಸಂರಚನಾ sbi_irqchip_device *chip;
	ಸಂರಚನಾ sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ಪೂರ್ಣಾಙ್ಕ rc;

	sbi_for_each_hartindex(i) {
		ಯದಿ (i == current_hartindex())
			ಅನುವರ್ತ;
		ಯದಿ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ಅನುವರ್ತ;
		migrate_hidx = i;
		migrate = true;
		ಭಙ್ಗ;
	}

	ಯದಿ (!migrate)
		ಗಚ್ಛ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, ಗಂಟು) {
		sbi_list_for_each_entry(h, &chip->handler_list, ಗಂಟು) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ಯದಿ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ಯದಿ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
