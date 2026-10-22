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
ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ {
	/** raw hardware interrupt handler */
	ಪೂರ್ಣಾಂಕ (*raw_handler)(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq);

#ವ್ಯಾಖ್ಯಾನಿಸು IRQ_ENABLED	ಬಿಟ್(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_ಸೂಚ್ಯಂಕ;

	/** chip's private data */
	ಶೂನ್ಯ *priv;
};

/** Internal irqchip interrupt handler */
ರಚನೆ sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ರಚನೆ sbi_dlist ಗಂಟು;

	/** First hardware IRQ handled by this handler */
	u32 ಮೊದಲ_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 ಅಂಕೆ_hwirq;

	/** Write MSI function of this handler */
	ಶೂನ್ಯ (*write_msi)(u32 hwirq, ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ, ಶೂನ್ಯ *priv);

	/** Callback function of this handler */
	ಪೂರ್ಣಾಂಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv);

	/** Callback private data */
	ಶೂನ್ಯ *priv;
};

ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ {
	ರಚನೆ sbi_irqchip_device *chip;
};

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ irqchip_hart_ದತ್ತಾಂಶ_off;
ಸ್ಥಿರ SBI_LIST_ತಲೆ(irqchip_list);

ಪೂರ್ಣಾಂಕ sbi_irqchip_process(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ *hd;

	hd = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(irqchip_hart_ದತ್ತಾಂಶ_off);
	ಆದರೆ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಹಿಂತಿರುಗಿ hd->chip->process_hwirqs(hd->chip);
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_process_hwirq(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ಆದರೆ (!ದತ್ತಾಂಶ->raw_handler)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->raw_handler(chip, hwirq);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ u32 sbi_irqchip_get_irq_state(ರಚನೆ sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ಆದರೆ (!chip || !chip->hwirqs || hwirq >= chip->ಅಂಕೆ_hwirq)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ರಚನೆ sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ಹಿಂತಿರುಗಿ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_unmask_hwirq(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ಆದರೆ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	ಆದರೆ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	ದತ್ತಾಂಶ->irq_state |= IRQ_ENABLED;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ಮುಖವಾಡ_hwirq(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	ಆದರೆ (chip->hwirq_ಮುಖವಾಡ)
		chip->hwirq_ಮುಖವಾಡ(chip, hwirq);

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ದತ್ತಾಂಶ->irq_state &= ~IRQ_ENABLED;
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ sbi_irqchip_handler *sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(ರಚನೆ sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ರಚನೆ sbi_irqchip_handler *h;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	sbi_list_for_each_ನಮೂದು(h, &chip->handler_list, ಗಂಟು) {
		ಆದರೆ (h->ಮೊದಲ_hwirq <= hwirq && hwirq < (h->ಮೊದಲ_hwirq + h->ಅಂಕೆ_hwirq))
			ಹಿಂತಿರುಗಿ h;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ಗಣ_hwirq_priv(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq, ಶೂನ್ಯ *priv)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
	     ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ದತ್ತಾಂಶ->priv = priv;

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ *sbi_irqchip_get_hwirq_priv(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;
	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];

	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->priv;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_raw_handler_default(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq)
{
	ರಚನೆ sbi_irqchip_handler *h;
	ಪೂರ್ಣಾಂಕ rc = SBI_OK;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	h = sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, hwirq);
	ಆದರೆ (!h)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ಆದರೆ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ಹಿಂತಿರುಗಿ rc;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ಗಣ_raw_handler(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq,
				ಪೂರ್ಣಾಂಕ (*raw_hndl)(ರಚನೆ sbi_irqchip_device *, u32))
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ದತ್ತಾಂಶ->raw_handler = raw_hndl;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_write_msi(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq,
			  ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ)
{
	ರಚನೆ sbi_irqchip_handler *h;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq || !ಸಂದೇಶ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	h = sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, hwirq);
	ಆದರೆ (!h)
		ಹಿಂತಿರುಗಿ SBI_EFAIL;
	ಆದರೆ (!h->write_msi)
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	h->write_msi(hwirq, ಸಂದೇಶ, h->priv);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_get_affinity(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq,
			     u32 *ಹೊರಹೋಗು_hart_ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ಆದರೆ (!sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, hwirq))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	*ಹೊರಹೋಗು_hart_ಸೂಚ್ಯಂಕ = chip->hwirqs[hwirq].hart_ಸೂಚ್ಯಂಕ;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ಗಣ_affinity(ರಚನೆ sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_ಸೂಚ್ಯಂಕ)
{
	ರಚನೆ sbi_irqchip_hwirq_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (!chip || chip->ಅಂಕೆ_hwirq <= hwirq || sbi_hart_ಎಣಿಕೆ() <= hart_ಸೂಚ್ಯಂಕ)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ಆದರೆ (!sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, hwirq))
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;

	ದತ್ತಾಂಶ = &chip->hwirqs[hwirq];
	ಆದರೆ (ದತ್ತಾಂಶ->hart_ಸೂಚ್ಯಂಕ != hart_ಸೂಚ್ಯಂಕ) {
		ಆದರೆ (chip->hwirq_ಗಣ_affinity) {
			rc = chip->hwirq_ಗಣ_affinity(chip, hwirq, hart_ಸೂಚ್ಯಂಕ);
			ಆದರೆ (rc)
				ಹಿಂತಿರುಗಿ rc;
		}
		ದತ್ತಾಂಶ->hart_ಸೂಚ್ಯಂಕ = hart_ಸೂಚ್ಯಂಕ;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __sbi_irqchip_handler_set_affinity(ರಚನೆ sbi_irqchip_device *chip,
					      ರಚನೆ sbi_irqchip_handler *h,
					      u32 compare_hart_ಸೂಚ್ಯಂಕ,
					      u32 hart_ಸೂಚ್ಯಂಕ)
{
	u32 i, current_hart_ಸೂಚ್ಯಂಕ;
	ಪೂರ್ಣಾಂಕ rc;

	ಗಾಗಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->ಮೊದಲ_hwirq + i,
					      &current_hart_ಸೂಚ್ಯಂಕ);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		ಆದರೆ (compare_hart_ಸೂಚ್ಯಂಕ != -1U &&
		    current_hart_ಸೂಚ್ಯಂಕ != compare_hart_ಸೂಚ್ಯಂಕ)
			ಮುಂದುವರಿ;

		rc = sbi_irqchip_ಗಣ_affinity(chip, h->ಮೊದಲ_hwirq + i, hart_ಸೂಚ್ಯಂಕ);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __sbi_irqchip_register_handler(ರಚನೆ sbi_irqchip_device *chip,
					  u32 ಮೊದಲ_hwirq, u32 ಅಂಕೆ_hwirq, u32 hwirq_flags,
					  ಶೂನ್ಯ (*write_msi)(u32 hwirq,
							    ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
							    ಶೂನ್ಯ *priv),
					  ಪೂರ್ಣಾಂಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv)
{
	ರಚನೆ sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ಪೂರ್ಣಾಂಕ rc;

	ಗಾಗಿ (i = ಮೊದಲ_hwirq; i < (ಮೊದಲ_hwirq + ಅಂಕೆ_hwirq); i++) {
		h = sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, i);
		ಆದರೆ (h)
			ಹಿಂತಿರುಗಿ SBI_EALREADY;
	}

	h = sbi_zalloc(ಗಾತ್ರ(*h));
	ಆದರೆ (!h)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	h->ಮೊದಲ_hwirq = ಮೊದಲ_hwirq;
	h->ಅಂಕೆ_hwirq = ಅಂಕೆ_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ಖಾಲಿ;
	sbi_list_for_each_ನಮೂದು(th, &chip->handler_list, ಗಂಟು) {
		ಆದರೆ (h->ಮೊದಲ_hwirq < th->ಮೊದಲ_hwirq) {
			nh = th;
			ನಿಲ್ಲಿಸು;
		}
	}
	ಆದರೆ (nh)
		sbi_list_ಕೂಡಿಸು(&h->ಗಂಟು, &nh->ಗಂಟು);
	ಇಲ್ಲದಿದ್ದರೆ
		sbi_list_ಕೂಡಿಸು_ಬಾಲ(&h->ಗಂಟು, &chip->handler_list);

	ಆದರೆ (chip->hwirq_ಸಿದ್ಧತೆ) {
		ಗಾಗಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++) {
			rc = chip->hwirq_ಸಿದ್ಧತೆ(chip, h->ಮೊದಲ_hwirq + i, hwirq_flags);
			ಆದರೆ (rc) {
				ಆದರೆ (chip->hwirq_cleanup) {
					ಗಾಗಿ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->ಮೊದಲ_hwirq + j);
				}
				sbi_list_del(&h->ಗಂಟು);
				sbi_free(h);
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ಆದರೆ (rc) {
		ಆದರೆ (chip->hwirq_cleanup) {
			ಗಾಗಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++)
				chip->hwirq_cleanup(chip, h->ಮೊದಲ_hwirq + i);
		}
		sbi_list_del(&h->ಗಂಟು);
		sbi_free(h);
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ (chip->hwirq_unmask) {
		ಗಾಗಿ (i = 0; i < h->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_unmask(chip, h->ಮೊದಲ_hwirq + i);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ರಿಜಿಸ್ಟರ್_msi(ರಚನೆ sbi_irqchip_device *chip, u32 ಅಂಕೆ_hwirq,
			     ಶೂನ್ಯ (*write_msi)(u32 hwirq,
					       ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_irqchip_msi_ಸಂದೇಶ *ಸಂದೇಶ,
					       ಶೂನ್ಯ *priv),
			     ಪೂರ್ಣಾಂಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv,
			     u32 *ಹೊರಹೋಗು_ಮೊದಲ_hwirq)
{
	ರಚನೆ sbi_irqchip_handler *h;
	bool ಸಿಕ್ಕಿತು;
	u32 hwirq;

	ಆದರೆ (!chip || !chip->hwirq_ಗಣ_affinity || !ಅಂಕೆ_hwirq ||
	    !write_msi || !callback || !ಹೊರಹೋಗು_ಮೊದಲ_hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (chip->ಅಂಕೆ_hwirq < ಅಂಕೆ_hwirq)
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	hwirq = 0;
	ಸಿಕ್ಕಿತು = false;
	sbi_list_for_each_ನಮೂದು(h, &chip->handler_list, ಗಂಟು) {
		ಆದರೆ (h->ಮೊದಲ_hwirq <= hwirq && hwirq < (h->ಮೊದಲ_hwirq + h->ಅಂಕೆ_hwirq)) {
			hwirq = h->ಮೊದಲ_hwirq + h->ಅಂಕೆ_hwirq;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hwirq < h->ಮೊದಲ_hwirq) {
			ಆದರೆ (h->ಮೊದಲ_hwirq - hwirq >= ಅಂಕೆ_hwirq) {
				ಸಿಕ್ಕಿತು = true;
				ನಿಲ್ಲಿಸು;
			} ಇಲ್ಲದಿದ್ದರೆ {
				hwirq = h->ಮೊದಲ_hwirq + h->ಅಂಕೆ_hwirq;
			}
		}
	}
	ಆದರೆ (!ಸಿಕ್ಕಿತು && (chip->ಅಂಕೆ_hwirq - hwirq) >= ಅಂಕೆ_hwirq)
		ಸಿಕ್ಕಿತು = true;
	ಆದರೆ (!ಸಿಕ್ಕಿತು)
		ಹಿಂತಿರುಗಿ SBI_ENOSPC;
	*ಹೊರಹೋಗು_ಮೊದಲ_hwirq = hwirq;

	ಹಿಂತಿರುಗಿ __sbi_irqchip_register_handler(chip, *ಹೊರಹೋಗು_ಮೊದಲ_hwirq,
					      ಅಂಕೆ_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ರಿಜಿಸ್ಟರ್_handler(ರಚನೆ sbi_irqchip_device *chip,
				 u32 ಮೊದಲ_hwirq, u32 ಅಂಕೆ_hwirq, u32 hwirq_flags,
				 ಪೂರ್ಣಾಂಕ (*callback)(u32 hwirq, ಶೂನ್ಯ *priv), ಶೂನ್ಯ *priv)
{
	ಆದರೆ (!chip || !ಅಂಕೆ_hwirq || !callback)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (chip->ಅಂಕೆ_hwirq <= ಮೊದಲ_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (ಮೊದಲ_hwirq + ಅಂಕೆ_hwirq - 1))
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	ಹಿಂತಿರುಗಿ __sbi_irqchip_register_handler(chip, ಮೊದಲ_hwirq, ಅಂಕೆ_hwirq, hwirq_flags,
					      ಖಾಲಿ, callback, priv);
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ರಿಜಿಸ್ಟರ್_reserved(ರಚನೆ sbi_irqchip_device *chip,
				  u32 ಮೊದಲ_hwirq, u32 ಅಂಕೆ_hwirq)
{
	ಆದರೆ (!chip || !ಅಂಕೆ_hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (chip->ಅಂಕೆ_hwirq <= ಮೊದಲ_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (ಮೊದಲ_hwirq + ಅಂಕೆ_hwirq - 1))
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	ಹಿಂತಿರುಗಿ __sbi_irqchip_register_handler(chip, ಮೊದಲ_hwirq, ಅಂಕೆ_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_unregister_handler(ರಚನೆ sbi_irqchip_device *chip,
				   u32 ಮೊದಲ_hwirq, u32 ಅಂಕೆ_hwirq)
{
	ರಚನೆ sbi_irqchip_handler *fh, *lh;
	u32 i;

	ಆದರೆ (!chip || !ಅಂಕೆ_hwirq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (chip->ಅಂಕೆ_hwirq <= ಮೊದಲ_hwirq ||
	    chip->ಅಂಕೆ_hwirq <= (ಮೊದಲ_hwirq + ಅಂಕೆ_hwirq - 1))
		ಹಿಂತಿರುಗಿ SBI_EBAD_RANGE;

	fh = sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, ಮೊದಲ_hwirq);
	ಆದರೆ (!fh || fh->ಮೊದಲ_hwirq != ಮೊದಲ_hwirq || fh->ಅಂಕೆ_hwirq != ಅಂಕೆ_hwirq)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;
	lh = sbi_irqchip_ಕಂಡುಹಿಡಿ_handler(chip, ಮೊದಲ_hwirq + ಅಂಕೆ_hwirq - 1);
	ಆದರೆ (!lh || lh != fh)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	ಆದರೆ (chip->hwirq_ಮುಖವಾಡ) {
		ಗಾಗಿ (i = 0; i < fh->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_ಮುಖವಾಡ(chip, fh->ಮೊದಲ_hwirq + i);
	}

	ಆದರೆ (chip->hwirq_cleanup) {
		ಗಾಗಿ (i = 0; i < fh->ಅಂಕೆ_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->ಮೊದಲ_hwirq + i);
	}

	sbi_list_del(&fh->ಗಂಟು);
	ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ sbi_irqchip_device *sbi_irqchip_ಕಂಡುಹಿಡಿ_device_by_caps(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ caps,
							   ರಚನೆ sbi_irqchip_device *ಮೊದಲ)
{
	ರಚನೆ sbi_irqchip_device *chip;
	bool ಸಿಕ್ಕಿತು = (ಮೊದಲ == ಖಾಲಿ);

	sbi_list_for_each_ನಮೂದು(chip, &irqchip_list, ಗಂಟು) {
		ಆದರೆ (!ಸಿಕ್ಕಿತು) {
			ಆದರೆ (ಮೊದಲ == chip)
				ಸಿಕ್ಕಿತು = true;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ ((chip->caps & caps) == caps)
			ಹಿಂತಿರುಗಿ chip;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ರಚನೆ sbi_irqchip_device *sbi_irqchip_ಕಂಡುಹಿಡಿ_device(u32 id)
{
	ರಚನೆ sbi_irqchip_device *chip;

	sbi_list_for_each_ನಮೂದು(chip, &irqchip_list, ಗಂಟು) {
		ಆದರೆ (chip->id == id)
			ಹಿಂತಿರುಗಿ chip;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_ಕೂಡಿಸು_device(ರಚನೆ sbi_irqchip_device *chip)
{
	ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ *hd;
	ರಚನೆ sbi_scratch *scratch;
	u32 i, h;

	ಆದರೆ (!chip || !chip->ಅಂಕೆ_hwirq || !sbi_hartmask_ತೂಕ(&chip->ಗುರಿ_harts))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆದರೆ (sbi_irqchip_ಕಂಡುಹಿಡಿ_device(chip->id))
		ಹಿಂತಿರುಗಿ SBI_EALREADY;

	ಆದರೆ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->ಗುರಿ_harts) {
			scratch = sbi_hartindex_ಗಮ್ಯ_scratch(h);
			ಆದರೆ (!scratch)
				ಮುಂದುವರಿ;

			hd = sbi_scratch_ಸ್ಥಾನಾಂತರ_ptr(scratch, irqchip_hart_ದತ್ತಾಂಶ_off);
			ಆದರೆ (hd->chip && hd->chip != chip)
				ಹಿಂತಿರುಗಿ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(ಗಾತ್ರ(*chip->hwirqs) * chip->ಅಂಕೆ_hwirq);
	ಆದರೆ (!chip->hwirqs)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	ಗಾಗಿ (i = 0; i < chip->ಅಂಕೆ_hwirq; i++) {
		sbi_irqchip_ಗಣ_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_ಸೂಚ್ಯಂಕ = -1U;
	}

	SBI_INIT_LIST_ತಲೆ(&chip->handler_list);

	sbi_list_ಕೂಡಿಸು_ಬಾಲ(&chip->ಗಂಟು, &irqchip_list);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sbi_irqchip_init(ರಚನೆ sbi_scratch *scratch, bool cold_boot)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_platform *plat = sbi_platform_ptr(scratch);
	ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ *hd;
	ರಚನೆ sbi_irqchip_device *chip;
	ಪೂರ್ಣಾಂಕ rc;

	ಆದರೆ (cold_boot) {
		irqchip_hart_ದತ್ತಾಂಶ_off =
			sbi_scratch_alloc_ಸ್ಥಾನಾಂತರ(ಗಾತ್ರ(ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ));
		ಆದರೆ (!irqchip_hart_ದತ್ತಾಂಶ_off)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	sbi_list_for_each_ನಮೂದು(chip, &irqchip_list, ಗಂಟು) {
		ಆದರೆ (!chip->warm_init)
			ಮುಂದುವರಿ;
		ಆದರೆ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->ಗುರಿ_harts))
			ಮುಂದುವರಿ;
		rc = chip->warm_init(chip);
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	hd = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(irqchip_hart_ದತ್ತಾಂಶ_off);
	ಆದರೆ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_ಗಣ(CSR_MIE, MIP_MEIP);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sbi_irqchip_exit(ರಚನೆ sbi_scratch *scratch)
{
	ರಚನೆ sbi_irqchip_hart_ದತ್ತಾಂಶ *hd;
	ರಚನೆ sbi_irqchip_device *chip;
	ರಚನೆ sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ಪೂರ್ಣಾಂಕ rc;

	sbi_for_each_hartindex(i) {
		ಆದರೆ (i == current_hartindex())
			ಮುಂದುವರಿ;
		ಆದರೆ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ಮುಂದುವರಿ;
		migrate_hidx = i;
		migrate = true;
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (!migrate)
		ಹೋಗು skip_migrate;
	sbi_list_for_each_ನಮೂದು(chip, &irqchip_list, ಗಂಟು) {
		sbi_list_for_each_ನಮೂದು(h, &chip->handler_list, ಗಂಟು) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ಆದರೆ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->ಮೊದಲ_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_ಸ್ಥಾನಾಂತರ_ptr(irqchip_hart_ದತ್ತಾಂಶ_off);
	ಆದರೆ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
