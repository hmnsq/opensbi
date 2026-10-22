/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_irqchip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_list.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ {
	/** raw hardware interrupt handler */
	ਪੂਰਨ (*raw_handler)(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IRQ_ENABLED	ਦੋਆਧਾਰੀ_ਅੰਕ(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_ਸੂਚਕਾਂਕ;

	/** chip's private data */
	ਖਾਲੀ *priv;
};

/** Internal irqchip interrupt handler */
ਬਣਤਰ sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ਬਣਤਰ sbi_dlist ਗੰਢ;

	/** First hardware IRQ handled by this handler */
	u32 ਪਹਿਲਾ_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 ਅੰਕ_hwirq;

	/** Write MSI function of this handler */
	ਖਾਲੀ (*write_msi)(u32 hwirq, ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ, ਖਾਲੀ *priv);

	/** Callback function of this handler */
	ਪੂਰਨ (*callback)(u32 hwirq, ਖਾਲੀ *priv);

	/** Callback private data */
	ਖਾਲੀ *priv;
};

ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ {
	ਬਣਤਰ sbi_irqchip_device *chip;
};

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ irqchip_hart_ਅੰਕੜੇ_off;
ਸਥਿਰ SBI_LIST_ਸਿਰ(irqchip_list);

ਪੂਰਨ sbi_irqchip_process(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ *hd;

	hd = sbi_scratch_thishart_ਵਿੱਥ_ptr(irqchip_hart_ਅੰਕੜੇ_off);
	ਜੇ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ਵਾਪਸ SBI_ENODEV;

	ਵਾਪਸ hd->chip->process_hwirqs(hd->chip);
}

ਪੂਰਨ sbi_irqchip_process_hwirq(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਜੇ (!ਅੰਕੜੇ->raw_handler)
		ਵਾਪਸ SBI_ENOENT;

	ਵਾਪਸ ਅੰਕੜੇ->raw_handler(chip, hwirq);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ u32 sbi_irqchip_get_irq_state(ਬਣਤਰ sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ਜੇ (!chip || !chip->hwirqs || hwirq >= chip->ਅੰਕ_hwirq)
		ਵਾਪਸ 0;

	ਵਾਪਸ chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ਬਣਤਰ sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ਵਾਪਸ !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ਪੂਰਨ sbi_irqchip_unmask_hwirq(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਜੇ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ਵਾਪਸ SBI_EALREADY;

	ਜੇ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	ਅੰਕੜੇ->irq_state |= IRQ_ENABLED;
	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_ਨਕਾਬ_hwirq(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ਵਾਪਸ SBI_EALREADY;

	ਜੇ (chip->hwirq_ਨਕਾਬ)
		chip->hwirq_ਨਕਾਬ(chip, hwirq);

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਅੰਕੜੇ->irq_state &= ~IRQ_ENABLED;
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ sbi_irqchip_handler *sbi_irqchip_ਲੱਭੋ_handler(ਬਣਤਰ sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_handler *h;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ ਨਿਰਾ;

	sbi_list_for_each_ਇੰਦਰਾਜ(h, &chip->handler_list, ਗੰਢ) {
		ਜੇ (h->ਪਹਿਲਾ_hwirq <= hwirq && hwirq < (h->ਪਹਿਲਾ_hwirq + h->ਅੰਕ_hwirq))
			ਵਾਪਸ h;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ sbi_irqchip_ਸੈੱਟ_hwirq_priv(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq, ਖਾਲੀ *priv)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
	     ਵਾਪਸ SBI_EINVAL;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਅੰਕੜੇ->priv = priv;

	ਵਾਪਸ 0;
}

ਖਾਲੀ *sbi_irqchip_get_hwirq_priv(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;
	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
	    ਵਾਪਸ ਨਿਰਾ;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];

	ਵਾਪਸ ਅੰਕੜੇ->priv;
}

ਪੂਰਨ sbi_irqchip_raw_handler_default(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq)
{
	ਬਣਤਰ sbi_irqchip_handler *h;
	ਪੂਰਨ rc = SBI_OK;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	h = sbi_irqchip_ਲੱਭੋ_handler(chip, hwirq);
	ਜੇ (!h)
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ਜੇ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ਵਾਪਸ rc;
}

ਪੂਰਨ sbi_irqchip_ਸੈੱਟ_raw_handler(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq,
				ਪੂਰਨ (*raw_hndl)(ਬਣਤਰ sbi_irqchip_device *, u32))
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਅੰਕੜੇ->raw_handler = raw_hndl;
	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_write_msi(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq,
			  ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ)
{
	ਬਣਤਰ sbi_irqchip_handler *h;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq || !ਸੁਨੇਹਾ)
		ਵਾਪਸ SBI_EINVAL;

	h = sbi_irqchip_ਲੱਭੋ_handler(chip, hwirq);
	ਜੇ (!h)
		ਵਾਪਸ SBI_EFAIL;
	ਜੇ (!h->write_msi)
		ਵਾਪਸ SBI_ENOTSUPP;

	h->write_msi(hwirq, ਸੁਨੇਹਾ, h->priv);
	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_get_affinity(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq,
			     u32 *ਬਾਹਰਲਾ_hart_ਸੂਚਕਾਂਕ)
{
	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ਜੇ (!sbi_irqchip_ਲੱਭੋ_handler(chip, hwirq))
		ਵਾਪਸ SBI_ENOTSUPP;

	*ਬਾਹਰਲਾ_hart_ਸੂਚਕਾਂਕ = chip->hwirqs[hwirq].hart_ਸੂਚਕਾਂਕ;
	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_ਸੈੱਟ_affinity(ਬਣਤਰ sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_ਸੂਚਕਾਂਕ)
{
	ਬਣਤਰ sbi_irqchip_hwirq_ਅੰਕੜੇ *ਅੰਕੜੇ;
	ਪੂਰਨ rc;

	ਜੇ (!chip || chip->ਅੰਕ_hwirq <= hwirq || sbi_hart_ਗਿਣਤੀ() <= hart_ਸੂਚਕਾਂਕ)
		ਵਾਪਸ SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ਜੇ (!sbi_irqchip_ਲੱਭੋ_handler(chip, hwirq))
		ਵਾਪਸ SBI_ENOTSUPP;

	ਅੰਕੜੇ = &chip->hwirqs[hwirq];
	ਜੇ (ਅੰਕੜੇ->hart_ਸੂਚਕਾਂਕ != hart_ਸੂਚਕਾਂਕ) {
		ਜੇ (chip->hwirq_ਸੈੱਟ_affinity) {
			rc = chip->hwirq_ਸੈੱਟ_affinity(chip, hwirq, hart_ਸੂਚਕਾਂਕ);
			ਜੇ (rc)
				ਵਾਪਸ rc;
		}
		ਅੰਕੜੇ->hart_ਸੂਚਕਾਂਕ = hart_ਸੂਚਕਾਂਕ;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ __sbi_irqchip_handler_set_affinity(ਬਣਤਰ sbi_irqchip_device *chip,
					      ਬਣਤਰ sbi_irqchip_handler *h,
					      u32 compare_hart_ਸੂਚਕਾਂਕ,
					      u32 hart_ਸੂਚਕਾਂਕ)
{
	u32 i, current_hart_ਸੂਚਕਾਂਕ;
	ਪੂਰਨ rc;

	ਲਈ (i = 0; i < h->ਅੰਕ_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->ਪਹਿਲਾ_hwirq + i,
					      &current_hart_ਸੂਚਕਾਂਕ);
		ਜੇ (rc)
			ਵਾਪਸ rc;

		ਜੇ (compare_hart_ਸੂਚਕਾਂਕ != -1U &&
		    current_hart_ਸੂਚਕਾਂਕ != compare_hart_ਸੂਚਕਾਂਕ)
			ਜਾਰੀ;

		rc = sbi_irqchip_ਸੈੱਟ_affinity(chip, h->ਪਹਿਲਾ_hwirq + i, hart_ਸੂਚਕਾਂਕ);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ __sbi_irqchip_register_handler(ਬਣਤਰ sbi_irqchip_device *chip,
					  u32 ਪਹਿਲਾ_hwirq, u32 ਅੰਕ_hwirq, u32 hwirq_flags,
					  ਖਾਲੀ (*write_msi)(u32 hwirq,
							    ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ,
							    ਖਾਲੀ *priv),
					  ਪੂਰਨ (*callback)(u32 hwirq, ਖਾਲੀ *priv), ਖਾਲੀ *priv)
{
	ਬਣਤਰ sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ਪੂਰਨ rc;

	ਲਈ (i = ਪਹਿਲਾ_hwirq; i < (ਪਹਿਲਾ_hwirq + ਅੰਕ_hwirq); i++) {
		h = sbi_irqchip_ਲੱਭੋ_handler(chip, i);
		ਜੇ (h)
			ਵਾਪਸ SBI_EALREADY;
	}

	h = sbi_zalloc(ਆਕਾਰ(*h));
	ਜੇ (!h)
		ਵਾਪਸ SBI_ENOMEM;
	h->ਪਹਿਲਾ_hwirq = ਪਹਿਲਾ_hwirq;
	h->ਅੰਕ_hwirq = ਅੰਕ_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ਨਿਰਾ;
	sbi_list_for_each_ਇੰਦਰਾਜ(th, &chip->handler_list, ਗੰਢ) {
		ਜੇ (h->ਪਹਿਲਾ_hwirq < th->ਪਹਿਲਾ_hwirq) {
			nh = th;
			ਰੋਕੋ;
		}
	}
	ਜੇ (nh)
		sbi_list_ਜੋੜੋ(&h->ਗੰਢ, &nh->ਗੰਢ);
	ਨਹੀਂਤਾਂ
		sbi_list_ਜੋੜੋ_ਪੂਛ(&h->ਗੰਢ, &chip->handler_list);

	ਜੇ (chip->hwirq_ਤਿਆਰੀ) {
		ਲਈ (i = 0; i < h->ਅੰਕ_hwirq; i++) {
			rc = chip->hwirq_ਤਿਆਰੀ(chip, h->ਪਹਿਲਾ_hwirq + i, hwirq_flags);
			ਜੇ (rc) {
				ਜੇ (chip->hwirq_cleanup) {
					ਲਈ (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->ਪਹਿਲਾ_hwirq + j);
				}
				sbi_list_del(&h->ਗੰਢ);
				sbi_free(h);
				ਵਾਪਸ rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ਜੇ (rc) {
		ਜੇ (chip->hwirq_cleanup) {
			ਲਈ (i = 0; i < h->ਅੰਕ_hwirq; i++)
				chip->hwirq_cleanup(chip, h->ਪਹਿਲਾ_hwirq + i);
		}
		sbi_list_del(&h->ਗੰਢ);
		sbi_free(h);
		ਵਾਪਸ rc;
	}

	ਜੇ (chip->hwirq_unmask) {
		ਲਈ (i = 0; i < h->ਅੰਕ_hwirq; i++)
			chip->hwirq_unmask(chip, h->ਪਹਿਲਾ_hwirq + i);
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_ਰਜਿਸਟਰ_msi(ਬਣਤਰ sbi_irqchip_device *chip, u32 ਅੰਕ_hwirq,
			     ਖਾਲੀ (*write_msi)(u32 hwirq,
					       ਅਚਲ ਬਣਤਰ sbi_irqchip_msi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ,
					       ਖਾਲੀ *priv),
			     ਪੂਰਨ (*callback)(u32 hwirq, ਖਾਲੀ *priv), ਖਾਲੀ *priv,
			     u32 *ਬਾਹਰਲਾ_ਪਹਿਲਾ_hwirq)
{
	ਬਣਤਰ sbi_irqchip_handler *h;
	bool ਮਿਲਿਆ;
	u32 hwirq;

	ਜੇ (!chip || !chip->hwirq_ਸੈੱਟ_affinity || !ਅੰਕ_hwirq ||
	    !write_msi || !callback || !ਬਾਹਰਲਾ_ਪਹਿਲਾ_hwirq)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (chip->ਅੰਕ_hwirq < ਅੰਕ_hwirq)
		ਵਾਪਸ SBI_EBAD_RANGE;

	hwirq = 0;
	ਮਿਲਿਆ = false;
	sbi_list_for_each_ਇੰਦਰਾਜ(h, &chip->handler_list, ਗੰਢ) {
		ਜੇ (h->ਪਹਿਲਾ_hwirq <= hwirq && hwirq < (h->ਪਹਿਲਾ_hwirq + h->ਅੰਕ_hwirq)) {
			hwirq = h->ਪਹਿਲਾ_hwirq + h->ਅੰਕ_hwirq;
		} ਨਹੀਂਤਾਂ ਜੇ (hwirq < h->ਪਹਿਲਾ_hwirq) {
			ਜੇ (h->ਪਹਿਲਾ_hwirq - hwirq >= ਅੰਕ_hwirq) {
				ਮਿਲਿਆ = true;
				ਰੋਕੋ;
			} ਨਹੀਂਤਾਂ {
				hwirq = h->ਪਹਿਲਾ_hwirq + h->ਅੰਕ_hwirq;
			}
		}
	}
	ਜੇ (!ਮਿਲਿਆ && (chip->ਅੰਕ_hwirq - hwirq) >= ਅੰਕ_hwirq)
		ਮਿਲਿਆ = true;
	ਜੇ (!ਮਿਲਿਆ)
		ਵਾਪਸ SBI_ENOSPC;
	*ਬਾਹਰਲਾ_ਪਹਿਲਾ_hwirq = hwirq;

	ਵਾਪਸ __sbi_irqchip_register_handler(chip, *ਬਾਹਰਲਾ_ਪਹਿਲਾ_hwirq,
					      ਅੰਕ_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ਪੂਰਨ sbi_irqchip_ਰਜਿਸਟਰ_handler(ਬਣਤਰ sbi_irqchip_device *chip,
				 u32 ਪਹਿਲਾ_hwirq, u32 ਅੰਕ_hwirq, u32 hwirq_flags,
				 ਪੂਰਨ (*callback)(u32 hwirq, ਖਾਲੀ *priv), ਖਾਲੀ *priv)
{
	ਜੇ (!chip || !ਅੰਕ_hwirq || !callback)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (chip->ਅੰਕ_hwirq <= ਪਹਿਲਾ_hwirq ||
	    chip->ਅੰਕ_hwirq <= (ਪਹਿਲਾ_hwirq + ਅੰਕ_hwirq - 1))
		ਵਾਪਸ SBI_EBAD_RANGE;

	ਵਾਪਸ __sbi_irqchip_register_handler(chip, ਪਹਿਲਾ_hwirq, ਅੰਕ_hwirq, hwirq_flags,
					      ਨਿਰਾ, callback, priv);
}

ਪੂਰਨ sbi_irqchip_ਰਜਿਸਟਰ_reserved(ਬਣਤਰ sbi_irqchip_device *chip,
				  u32 ਪਹਿਲਾ_hwirq, u32 ਅੰਕ_hwirq)
{
	ਜੇ (!chip || !ਅੰਕ_hwirq)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (chip->ਅੰਕ_hwirq <= ਪਹਿਲਾ_hwirq ||
	    chip->ਅੰਕ_hwirq <= (ਪਹਿਲਾ_hwirq + ਅੰਕ_hwirq - 1))
		ਵਾਪਸ SBI_EBAD_RANGE;

	ਵਾਪਸ __sbi_irqchip_register_handler(chip, ਪਹਿਲਾ_hwirq, ਅੰਕ_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

ਪੂਰਨ sbi_irqchip_unregister_handler(ਬਣਤਰ sbi_irqchip_device *chip,
				   u32 ਪਹਿਲਾ_hwirq, u32 ਅੰਕ_hwirq)
{
	ਬਣਤਰ sbi_irqchip_handler *fh, *lh;
	u32 i;

	ਜੇ (!chip || !ਅੰਕ_hwirq)
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (chip->ਅੰਕ_hwirq <= ਪਹਿਲਾ_hwirq ||
	    chip->ਅੰਕ_hwirq <= (ਪਹਿਲਾ_hwirq + ਅੰਕ_hwirq - 1))
		ਵਾਪਸ SBI_EBAD_RANGE;

	fh = sbi_irqchip_ਲੱਭੋ_handler(chip, ਪਹਿਲਾ_hwirq);
	ਜੇ (!fh || fh->ਪਹਿਲਾ_hwirq != ਪਹਿਲਾ_hwirq || fh->ਅੰਕ_hwirq != ਅੰਕ_hwirq)
		ਵਾਪਸ SBI_ENODEV;
	lh = sbi_irqchip_ਲੱਭੋ_handler(chip, ਪਹਿਲਾ_hwirq + ਅੰਕ_hwirq - 1);
	ਜੇ (!lh || lh != fh)
		ਵਾਪਸ SBI_ENODEV;

	ਜੇ (chip->hwirq_ਨਕਾਬ) {
		ਲਈ (i = 0; i < fh->ਅੰਕ_hwirq; i++)
			chip->hwirq_ਨਕਾਬ(chip, fh->ਪਹਿਲਾ_hwirq + i);
	}

	ਜੇ (chip->hwirq_cleanup) {
		ਲਈ (i = 0; i < fh->ਅੰਕ_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->ਪਹਿਲਾ_hwirq + i);
	}

	sbi_list_del(&fh->ਗੰਢ);
	ਵਾਪਸ 0;
}

ਬਣਤਰ sbi_irqchip_device *sbi_irqchip_ਲੱਭੋ_device_by_caps(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ caps,
							   ਬਣਤਰ sbi_irqchip_device *ਪਹਿਲਾ)
{
	ਬਣਤਰ sbi_irqchip_device *chip;
	bool ਮਿਲਿਆ = (ਪਹਿਲਾ == ਨਿਰਾ);

	sbi_list_for_each_ਇੰਦਰਾਜ(chip, &irqchip_list, ਗੰਢ) {
		ਜੇ (!ਮਿਲਿਆ) {
			ਜੇ (ਪਹਿਲਾ == chip)
				ਮਿਲਿਆ = true;
			ਜਾਰੀ;
		}
		ਜੇ ((chip->caps & caps) == caps)
			ਵਾਪਸ chip;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਬਣਤਰ sbi_irqchip_device *sbi_irqchip_ਲੱਭੋ_device(u32 id)
{
	ਬਣਤਰ sbi_irqchip_device *chip;

	sbi_list_for_each_ਇੰਦਰਾਜ(chip, &irqchip_list, ਗੰਢ) {
		ਜੇ (chip->id == id)
			ਵਾਪਸ chip;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ sbi_irqchip_ਜੋੜੋ_device(ਬਣਤਰ sbi_irqchip_device *chip)
{
	ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ *hd;
	ਬਣਤਰ sbi_scratch *scratch;
	u32 i, h;

	ਜੇ (!chip || !chip->ਅੰਕ_hwirq || !sbi_hartmask_ਭਾਰ(&chip->ਨਿਸ਼ਾਨਾ_harts))
		ਵਾਪਸ SBI_EINVAL;

	ਜੇ (sbi_irqchip_ਲੱਭੋ_device(chip->id))
		ਵਾਪਸ SBI_EALREADY;

	ਜੇ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->ਨਿਸ਼ਾਨਾ_harts) {
			scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(h);
			ਜੇ (!scratch)
				ਜਾਰੀ;

			hd = sbi_scratch_ਵਿੱਥ_ptr(scratch, irqchip_hart_ਅੰਕੜੇ_off);
			ਜੇ (hd->chip && hd->chip != chip)
				ਵਾਪਸ SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(ਆਕਾਰ(*chip->hwirqs) * chip->ਅੰਕ_hwirq);
	ਜੇ (!chip->hwirqs)
		ਵਾਪਸ SBI_ENOMEM;
	ਲਈ (i = 0; i < chip->ਅੰਕ_hwirq; i++) {
		sbi_irqchip_ਸੈੱਟ_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_ਸੂਚਕਾਂਕ = -1U;
	}

	SBI_INIT_LIST_ਸਿਰ(&chip->handler_list);

	sbi_list_ਜੋੜੋ_ਪੂਛ(&chip->ਗੰਢ, &irqchip_list);
	ਵਾਪਸ 0;
}

ਪੂਰਨ sbi_irqchip_init(ਬਣਤਰ sbi_scratch *scratch, bool cold_boot)
{
	ਅਚਲ ਬਣਤਰ sbi_platform *plat = sbi_platform_ptr(scratch);
	ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ *hd;
	ਬਣਤਰ sbi_irqchip_device *chip;
	ਪੂਰਨ rc;

	ਜੇ (cold_boot) {
		irqchip_hart_ਅੰਕੜੇ_off =
			sbi_scratch_alloc_ਵਿੱਥ(ਆਕਾਰ(ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ));
		ਜੇ (!irqchip_hart_ਅੰਕੜੇ_off)
			ਵਾਪਸ SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	sbi_list_for_each_ਇੰਦਰਾਜ(chip, &irqchip_list, ਗੰਢ) {
		ਜੇ (!chip->warm_init)
			ਜਾਰੀ;
		ਜੇ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->ਨਿਸ਼ਾਨਾ_harts))
			ਜਾਰੀ;
		rc = chip->warm_init(chip);
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	hd = sbi_scratch_thishart_ਵਿੱਥ_ptr(irqchip_hart_ਅੰਕੜੇ_off);
	ਜੇ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_ਸੈੱਟ(CSR_MIE, MIP_MEIP);

	ਵਾਪਸ 0;
}

ਖਾਲੀ sbi_irqchip_exit(ਬਣਤਰ sbi_scratch *scratch)
{
	ਬਣਤਰ sbi_irqchip_hart_ਅੰਕੜੇ *hd;
	ਬਣਤਰ sbi_irqchip_device *chip;
	ਬਣਤਰ sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ਪੂਰਨ rc;

	sbi_for_each_hartindex(i) {
		ਜੇ (i == current_hartindex())
			ਜਾਰੀ;
		ਜੇ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ਜਾਰੀ;
		migrate_hidx = i;
		migrate = true;
		ਰੋਕੋ;
	}

	ਜੇ (!migrate)
		ਜਾਓ skip_migrate;
	sbi_list_for_each_ਇੰਦਰਾਜ(chip, &irqchip_list, ਗੰਢ) {
		sbi_list_for_each_ਇੰਦਰਾਜ(h, &chip->handler_list, ਗੰਢ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ਜੇ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->ਪਹਿਲਾ_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_ਵਿੱਥ_ptr(irqchip_hart_ਅੰਕੜੇ_off);
	ਜੇ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
