/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_irqchip.h>
#అన్తర్భావ <sbi/sbi_list.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం {
	/** raw hardware interrupt handler */
	పూర్ణాంకం (*raw_handler)(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq);

#నిర్వచించు IRQ_ENABLED	బిట్టు(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_సూచీ;

	/** chip's private data */
	శూన్య *priv;
};

/** Internal irqchip interrupt handler */
నిర్మాణం sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	నిర్మాణం sbi_dlist కణుపు;

	/** First hardware IRQ handled by this handler */
	u32 మొదటి_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 అంకె_hwirq;

	/** Write MSI function of this handler */
	శూన్య (*write_msi)(u32 hwirq, స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం, శూన్య *priv);

	/** Callback function of this handler */
	పూర్ణాంకం (*callback)(u32 hwirq, శూన్య *priv);

	/** Callback private data */
	శూన్య *priv;
};

నిర్మాణం sbi_irqchip_hart_దత్తాంశం {
	నిర్మాణం sbi_irqchip_device *chip;
};

స్థిర గుర్తులేని దీర్ఘ irqchip_hart_దత్తాంశం_off;
స్థిర SBI_LIST_తల(irqchip_list);

పూర్ణాంకం sbi_irqchip_process(శూన్య)
{
	నిర్మాణం sbi_irqchip_hart_దత్తాంశం *hd;

	hd = sbi_scratch_thishart_స్థానభ్రంశం_ptr(irqchip_hart_దత్తాంశం_off);
	ఒకవేళ (!hd || !hd->chip || !hd->chip->process_hwirqs)
		తిరిగి SBI_ENODEV;

	తిరిగి hd->chip->process_hwirqs(hd->chip);
}

పూర్ణాంకం sbi_irqchip_process_hwirq(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	దత్తాంశం = &chip->hwirqs[hwirq];
	ఒకవేళ (!దత్తాంశం->raw_handler)
		తిరిగి SBI_ENOENT;

	తిరిగి దత్తాంశం->raw_handler(chip, hwirq);
}

స్థిర అంతఃవరుస u32 sbi_irqchip_get_irq_state(నిర్మాణం sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ఒకవేళ (!chip || !chip->hwirqs || hwirq >= chip->అంకె_hwirq)
		తిరిగి 0;

	తిరిగి chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(నిర్మాణం sbi_irqchip_device *chip,
				   u32 hwirq)
{
	తిరిగి !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

పూర్ణాంకం sbi_irqchip_unmask_hwirq(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	దత్తాంశం = &chip->hwirqs[hwirq];
	ఒకవేళ (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		తిరిగి SBI_EALREADY;

	ఒకవేళ (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	దత్తాంశం->irq_state |= IRQ_ENABLED;
	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_ముసుగు_hwirq(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		తిరిగి SBI_EALREADY;

	ఒకవేళ (chip->hwirq_ముసుగు)
		chip->hwirq_ముసుగు(chip, hwirq);

	దత్తాంశం = &chip->hwirqs[hwirq];
	దత్తాంశం->irq_state &= ~IRQ_ENABLED;
	తిరిగి 0;
}

స్థిర నిర్మాణం sbi_irqchip_handler *sbi_irqchip_కనుగొను_handler(నిర్మాణం sbi_irqchip_device *chip,
							    u32 hwirq)
{
	నిర్మాణం sbi_irqchip_handler *h;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి ఖాళీ;

	sbi_list_for_each_నమోదు(h, &chip->handler_list, కణుపు) {
		ఒకవేళ (h->మొదటి_hwirq <= hwirq && hwirq < (h->మొదటి_hwirq + h->అంకె_hwirq))
			తిరిగి h;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం sbi_irqchip_సమితి_hwirq_priv(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq, శూన్య *priv)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
	     తిరిగి SBI_EINVAL;

	దత్తాంశం = &chip->hwirqs[hwirq];
	దత్తాంశం->priv = priv;

	తిరిగి 0;
}

శూన్య *sbi_irqchip_get_hwirq_priv(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;
	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
	    తిరిగి ఖాళీ;

	దత్తాంశం = &chip->hwirqs[hwirq];

	తిరిగి దత్తాంశం->priv;
}

పూర్ణాంకం sbi_irqchip_raw_handler_default(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq)
{
	నిర్మాణం sbi_irqchip_handler *h;
	పూర్ణాంకం rc = SBI_OK;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	h = sbi_irqchip_కనుగొను_handler(chip, hwirq);
	ఒకవేళ (!h)
		తిరిగి SBI_EINVAL;

	ఒకవేళ (h->callback)
		rc = h->callback(hwirq, h->priv);

	ఒకవేళ (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	తిరిగి rc;
}

పూర్ణాంకం sbi_irqchip_సమితి_raw_handler(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq,
				పూర్ణాంకం (*raw_hndl)(నిర్మాణం sbi_irqchip_device *, u32))
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	దత్తాంశం = &chip->hwirqs[hwirq];
	దత్తాంశం->raw_handler = raw_hndl;
	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_write_msi(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq,
			  స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం)
{
	నిర్మాణం sbi_irqchip_handler *h;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq || !సందేశం)
		తిరిగి SBI_EINVAL;

	h = sbi_irqchip_కనుగొను_handler(chip, hwirq);
	ఒకవేళ (!h)
		తిరిగి SBI_EFAIL;
	ఒకవేళ (!h->write_msi)
		తిరిగి SBI_ENOTSUPP;

	h->write_msi(hwirq, సందేశం, h->priv);
	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_get_affinity(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq,
			     u32 *బహిర్గతం_hart_సూచీ)
{
	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq)
		తిరిగి SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ఒకవేళ (!sbi_irqchip_కనుగొను_handler(chip, hwirq))
		తిరిగి SBI_ENOTSUPP;

	*బహిర్గతం_hart_సూచీ = chip->hwirqs[hwirq].hart_సూచీ;
	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_సమితి_affinity(నిర్మాణం sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_సూచీ)
{
	నిర్మాణం sbi_irqchip_hwirq_దత్తాంశం *దత్తాంశం;
	పూర్ణాంకం rc;

	ఒకవేళ (!chip || chip->అంకె_hwirq <= hwirq || sbi_hart_లెక్క() <= hart_సూచీ)
		తిరిగి SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ఒకవేళ (!sbi_irqchip_కనుగొను_handler(chip, hwirq))
		తిరిగి SBI_ENOTSUPP;

	దత్తాంశం = &chip->hwirqs[hwirq];
	ఒకవేళ (దత్తాంశం->hart_సూచీ != hart_సూచీ) {
		ఒకవేళ (chip->hwirq_సమితి_affinity) {
			rc = chip->hwirq_సమితి_affinity(chip, hwirq, hart_సూచీ);
			ఒకవేళ (rc)
				తిరిగి rc;
		}
		దత్తాంశం->hart_సూచీ = hart_సూచీ;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం __sbi_irqchip_handler_set_affinity(నిర్మాణం sbi_irqchip_device *chip,
					      నిర్మాణం sbi_irqchip_handler *h,
					      u32 compare_hart_సూచీ,
					      u32 hart_సూచీ)
{
	u32 i, current_hart_సూచీ;
	పూర్ణాంకం rc;

	కొరకు (i = 0; i < h->అంకె_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->మొదటి_hwirq + i,
					      &current_hart_సూచీ);
		ఒకవేళ (rc)
			తిరిగి rc;

		ఒకవేళ (compare_hart_సూచీ != -1U &&
		    current_hart_సూచీ != compare_hart_సూచీ)
			కొనసాగు;

		rc = sbi_irqchip_సమితి_affinity(chip, h->మొదటి_hwirq + i, hart_సూచీ);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం __sbi_irqchip_register_handler(నిర్మాణం sbi_irqchip_device *chip,
					  u32 మొదటి_hwirq, u32 అంకె_hwirq, u32 hwirq_flags,
					  శూన్య (*write_msi)(u32 hwirq,
							    స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం,
							    శూన్య *priv),
					  పూర్ణాంకం (*callback)(u32 hwirq, శూన్య *priv), శూన్య *priv)
{
	నిర్మాణం sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	పూర్ణాంకం rc;

	కొరకు (i = మొదటి_hwirq; i < (మొదటి_hwirq + అంకె_hwirq); i++) {
		h = sbi_irqchip_కనుగొను_handler(chip, i);
		ఒకవేళ (h)
			తిరిగి SBI_EALREADY;
	}

	h = sbi_zalloc(పరిమాణం(*h));
	ఒకవేళ (!h)
		తిరిగి SBI_ENOMEM;
	h->మొదటి_hwirq = మొదటి_hwirq;
	h->అంకె_hwirq = అంకె_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ఖాళీ;
	sbi_list_for_each_నమోదు(th, &chip->handler_list, కణుపు) {
		ఒకవేళ (h->మొదటి_hwirq < th->మొదటి_hwirq) {
			nh = th;
			ఆపు;
		}
	}
	ఒకవేళ (nh)
		sbi_list_కూడు(&h->కణుపు, &nh->కణుపు);
	లేకపోతే
		sbi_list_కూడు_తోక(&h->కణుపు, &chip->handler_list);

	ఒకవేళ (chip->hwirq_అమరిక) {
		కొరకు (i = 0; i < h->అంకె_hwirq; i++) {
			rc = chip->hwirq_అమరిక(chip, h->మొదటి_hwirq + i, hwirq_flags);
			ఒకవేళ (rc) {
				ఒకవేళ (chip->hwirq_cleanup) {
					కొరకు (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->మొదటి_hwirq + j);
				}
				sbi_list_del(&h->కణుపు);
				sbi_free(h);
				తిరిగి rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ఒకవేళ (rc) {
		ఒకవేళ (chip->hwirq_cleanup) {
			కొరకు (i = 0; i < h->అంకె_hwirq; i++)
				chip->hwirq_cleanup(chip, h->మొదటి_hwirq + i);
		}
		sbi_list_del(&h->కణుపు);
		sbi_free(h);
		తిరిగి rc;
	}

	ఒకవేళ (chip->hwirq_unmask) {
		కొరకు (i = 0; i < h->అంకె_hwirq; i++)
			chip->hwirq_unmask(chip, h->మొదటి_hwirq + i);
	}

	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_రిజిస్టరు_msi(నిర్మాణం sbi_irqchip_device *chip, u32 అంకె_hwirq,
			     శూన్య (*write_msi)(u32 hwirq,
					       స్థిరరాశి నిర్మాణం sbi_irqchip_msi_సందేశం *సందేశం,
					       శూన్య *priv),
			     పూర్ణాంకం (*callback)(u32 hwirq, శూన్య *priv), శూన్య *priv,
			     u32 *బహిర్గతం_మొదటి_hwirq)
{
	నిర్మాణం sbi_irqchip_handler *h;
	bool దొరికింది;
	u32 hwirq;

	ఒకవేళ (!chip || !chip->hwirq_సమితి_affinity || !అంకె_hwirq ||
	    !write_msi || !callback || !బహిర్గతం_మొదటి_hwirq)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (chip->అంకె_hwirq < అంకె_hwirq)
		తిరిగి SBI_EBAD_RANGE;

	hwirq = 0;
	దొరికింది = false;
	sbi_list_for_each_నమోదు(h, &chip->handler_list, కణుపు) {
		ఒకవేళ (h->మొదటి_hwirq <= hwirq && hwirq < (h->మొదటి_hwirq + h->అంకె_hwirq)) {
			hwirq = h->మొదటి_hwirq + h->అంకె_hwirq;
		} లేకపోతే ఒకవేళ (hwirq < h->మొదటి_hwirq) {
			ఒకవేళ (h->మొదటి_hwirq - hwirq >= అంకె_hwirq) {
				దొరికింది = true;
				ఆపు;
			} లేకపోతే {
				hwirq = h->మొదటి_hwirq + h->అంకె_hwirq;
			}
		}
	}
	ఒకవేళ (!దొరికింది && (chip->అంకె_hwirq - hwirq) >= అంకె_hwirq)
		దొరికింది = true;
	ఒకవేళ (!దొరికింది)
		తిరిగి SBI_ENOSPC;
	*బహిర్గతం_మొదటి_hwirq = hwirq;

	తిరిగి __sbi_irqchip_register_handler(chip, *బహిర్గతం_మొదటి_hwirq,
					      అంకె_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

పూర్ణాంకం sbi_irqchip_రిజిస్టరు_handler(నిర్మాణం sbi_irqchip_device *chip,
				 u32 మొదటి_hwirq, u32 అంకె_hwirq, u32 hwirq_flags,
				 పూర్ణాంకం (*callback)(u32 hwirq, శూన్య *priv), శూన్య *priv)
{
	ఒకవేళ (!chip || !అంకె_hwirq || !callback)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (chip->అంకె_hwirq <= మొదటి_hwirq ||
	    chip->అంకె_hwirq <= (మొదటి_hwirq + అంకె_hwirq - 1))
		తిరిగి SBI_EBAD_RANGE;

	తిరిగి __sbi_irqchip_register_handler(chip, మొదటి_hwirq, అంకె_hwirq, hwirq_flags,
					      ఖాళీ, callback, priv);
}

పూర్ణాంకం sbi_irqchip_రిజిస్టరు_reserved(నిర్మాణం sbi_irqchip_device *chip,
				  u32 మొదటి_hwirq, u32 అంకె_hwirq)
{
	ఒకవేళ (!chip || !అంకె_hwirq)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (chip->అంకె_hwirq <= మొదటి_hwirq ||
	    chip->అంకె_hwirq <= (మొదటి_hwirq + అంకె_hwirq - 1))
		తిరిగి SBI_EBAD_RANGE;

	తిరిగి __sbi_irqchip_register_handler(chip, మొదటి_hwirq, అంకె_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ఖాళీ, ఖాళీ, ఖాళీ);
}

పూర్ణాంకం sbi_irqchip_unregister_handler(నిర్మాణం sbi_irqchip_device *chip,
				   u32 మొదటి_hwirq, u32 అంకె_hwirq)
{
	నిర్మాణం sbi_irqchip_handler *fh, *lh;
	u32 i;

	ఒకవేళ (!chip || !అంకె_hwirq)
		తిరిగి SBI_EINVAL;
	ఒకవేళ (chip->అంకె_hwirq <= మొదటి_hwirq ||
	    chip->అంకె_hwirq <= (మొదటి_hwirq + అంకె_hwirq - 1))
		తిరిగి SBI_EBAD_RANGE;

	fh = sbi_irqchip_కనుగొను_handler(chip, మొదటి_hwirq);
	ఒకవేళ (!fh || fh->మొదటి_hwirq != మొదటి_hwirq || fh->అంకె_hwirq != అంకె_hwirq)
		తిరిగి SBI_ENODEV;
	lh = sbi_irqchip_కనుగొను_handler(chip, మొదటి_hwirq + అంకె_hwirq - 1);
	ఒకవేళ (!lh || lh != fh)
		తిరిగి SBI_ENODEV;

	ఒకవేళ (chip->hwirq_ముసుగు) {
		కొరకు (i = 0; i < fh->అంకె_hwirq; i++)
			chip->hwirq_ముసుగు(chip, fh->మొదటి_hwirq + i);
	}

	ఒకవేళ (chip->hwirq_cleanup) {
		కొరకు (i = 0; i < fh->అంకె_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->మొదటి_hwirq + i);
	}

	sbi_list_del(&fh->కణుపు);
	తిరిగి 0;
}

నిర్మాణం sbi_irqchip_device *sbi_irqchip_కనుగొను_device_by_caps(గుర్తులేని దీర్ఘ caps,
							   నిర్మాణం sbi_irqchip_device *మొదటి)
{
	నిర్మాణం sbi_irqchip_device *chip;
	bool దొరికింది = (మొదటి == ఖాళీ);

	sbi_list_for_each_నమోదు(chip, &irqchip_list, కణుపు) {
		ఒకవేళ (!దొరికింది) {
			ఒకవేళ (మొదటి == chip)
				దొరికింది = true;
			కొనసాగు;
		}
		ఒకవేళ ((chip->caps & caps) == caps)
			తిరిగి chip;
	}

	తిరిగి ఖాళీ;
}

నిర్మాణం sbi_irqchip_device *sbi_irqchip_కనుగొను_device(u32 id)
{
	నిర్మాణం sbi_irqchip_device *chip;

	sbi_list_for_each_నమోదు(chip, &irqchip_list, కణుపు) {
		ఒకవేళ (chip->id == id)
			తిరిగి chip;
	}

	తిరిగి ఖాళీ;
}

పూర్ణాంకం sbi_irqchip_కూడు_device(నిర్మాణం sbi_irqchip_device *chip)
{
	నిర్మాణం sbi_irqchip_hart_దత్తాంశం *hd;
	నిర్మాణం sbi_scratch *scratch;
	u32 i, h;

	ఒకవేళ (!chip || !chip->అంకె_hwirq || !sbi_hartmask_బరువు(&chip->లక్ష్యం_harts))
		తిరిగి SBI_EINVAL;

	ఒకవేళ (sbi_irqchip_కనుగొను_device(chip->id))
		తిరిగి SBI_EALREADY;

	ఒకవేళ (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->లక్ష్యం_harts) {
			scratch = sbi_hartindex_గమ్యం_scratch(h);
			ఒకవేళ (!scratch)
				కొనసాగు;

			hd = sbi_scratch_స్థానభ్రంశం_ptr(scratch, irqchip_hart_దత్తాంశం_off);
			ఒకవేళ (hd->chip && hd->chip != chip)
				తిరిగి SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(పరిమాణం(*chip->hwirqs) * chip->అంకె_hwirq);
	ఒకవేళ (!chip->hwirqs)
		తిరిగి SBI_ENOMEM;
	కొరకు (i = 0; i < chip->అంకె_hwirq; i++) {
		sbi_irqchip_సమితి_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_సూచీ = -1U;
	}

	SBI_INIT_LIST_తల(&chip->handler_list);

	sbi_list_కూడు_తోక(&chip->కణుపు, &irqchip_list);
	తిరిగి 0;
}

పూర్ణాంకం sbi_irqchip_init(నిర్మాణం sbi_scratch *scratch, bool cold_boot)
{
	స్థిరరాశి నిర్మాణం sbi_platform *plat = sbi_platform_ptr(scratch);
	నిర్మాణం sbi_irqchip_hart_దత్తాంశం *hd;
	నిర్మాణం sbi_irqchip_device *chip;
	పూర్ణాంకం rc;

	ఒకవేళ (cold_boot) {
		irqchip_hart_దత్తాంశం_off =
			sbi_scratch_alloc_స్థానభ్రంశం(పరిమాణం(నిర్మాణం sbi_irqchip_hart_దత్తాంశం));
		ఒకవేళ (!irqchip_hart_దత్తాంశం_off)
			తిరిగి SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	sbi_list_for_each_నమోదు(chip, &irqchip_list, కణుపు) {
		ఒకవేళ (!chip->warm_init)
			కొనసాగు;
		ఒకవేళ (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->లక్ష్యం_harts))
			కొనసాగు;
		rc = chip->warm_init(chip);
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	hd = sbi_scratch_thishart_స్థానభ్రంశం_ptr(irqchip_hart_దత్తాంశం_off);
	ఒకవేళ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_సమితి(CSR_MIE, MIP_MEIP);

	తిరిగి 0;
}

శూన్య sbi_irqchip_exit(నిర్మాణం sbi_scratch *scratch)
{
	నిర్మాణం sbi_irqchip_hart_దత్తాంశం *hd;
	నిర్మాణం sbi_irqchip_device *chip;
	నిర్మాణం sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	పూర్ణాంకం rc;

	sbi_for_each_hartindex(i) {
		ఒకవేళ (i == current_hartindex())
			కొనసాగు;
		ఒకవేళ (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			కొనసాగు;
		migrate_hidx = i;
		migrate = true;
		ఆపు;
	}

	ఒకవేళ (!migrate)
		వెళ్ళు skip_migrate;
	sbi_list_for_each_నమోదు(chip, &irqchip_list, కణుపు) {
		sbi_list_for_each_నమోదు(h, &chip->handler_list, కణుపు) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ఒకవేళ (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->మొదటి_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_స్థానభ్రంశం_ptr(irqchip_hart_దత్తాంశం_off);
	ఒకవేళ (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
