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
संरचना sbi_irqchip_hwirq_दत्त {
	/** raw hardware interrupt handler */
	पूर्णाङ्क (*raw_handler)(संरचना sbi_irqchip_device *chip, u32 hwirq);

#परिभाषय IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_सूचकांक;

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

संरचना sbi_irqchip_hart_दत्त {
	संरचना sbi_irqchip_device *chip;
};

स्थिर अचिह्नित दीर्घ irqchip_hart_दत्त_off;
स्थिर SBI_LIST_शीर्ष(irqchip_list);

पूर्णाङ्क sbi_irqchip_process(शून्य)
{
	संरचना sbi_irqchip_hart_दत्त *hd;

	hd = sbi_scratch_thishart_विस्थापनम्_ptr(irqchip_hart_दत्त_off);
	यदि (!hd || !hd->chip || !hd->chip->process_hwirqs)
		प्रत्यागम SBI_ENODEV;

	प्रत्यागम hd->chip->process_hwirqs(hd->chip);
}

पूर्णाङ्क sbi_irqchip_process_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	दत्त = &chip->hwirqs[hwirq];
	यदि (!दत्त->raw_handler)
		प्रत्यागम SBI_ENOENT;

	प्रत्यागम दत्त->raw_handler(chip, hwirq);
}

स्थिर अन्तःस्थ u32 sbi_irqchip_get_irq_state(संरचना sbi_irqchip_device *chip,
					     u32 hwirq)
{
	यदि (!chip || !chip->hwirqs || hwirq >= chip->अङ्कः_hwirq)
		प्रत्यागम 0;

	प्रत्यागम chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(संरचना sbi_irqchip_device *chip,
				   u32 hwirq)
{
	प्रत्यागम !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

पूर्णाङ्क sbi_irqchip_unmask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	दत्त = &chip->hwirqs[hwirq];
	यदि (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		प्रत्यागम SBI_EALREADY;

	यदि (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	दत्त->irq_state |= IRQ_ENABLED;
	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_आवरणम्_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	यदि (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		प्रत्यागम SBI_EALREADY;

	यदि (chip->hwirq_आवरणम्)
		chip->hwirq_आवरणम्(chip, hwirq);

	दत्त = &chip->hwirqs[hwirq];
	दत्त->irq_state &= ~IRQ_ENABLED;
	प्रत्यागम 0;
}

स्थिर संरचना sbi_irqchip_handler *sbi_irqchip_find_handler(संरचना sbi_irqchip_device *chip,
							    u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम रिक्त;

	sbi_list_for_each_प्रविष्टिः(h, &chip->handler_list, ग्रन्थिः) {
		यदि (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्कः_hwirq))
			प्रत्यागम h;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क sbi_irqchip_समुच्चयः_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq, शून्य *priv)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
	     प्रत्यागम SBI_EINVAL;

	दत्त = &chip->hwirqs[hwirq];
	दत्त->priv = priv;

	प्रत्यागम 0;
}

शून्य *sbi_irqchip_get_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;
	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
	    प्रत्यागम रिक्त;

	दत्त = &chip->hwirqs[hwirq];

	प्रत्यागम दत्त->priv;
}

पूर्णाङ्क sbi_irqchip_raw_handler_default(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;
	पूर्णाङ्क rc = SBI_OK;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	यदि (!h)
		प्रत्यागम SBI_EINVAL;

	यदि (h->callback)
		rc = h->callback(hwirq, h->priv);

	यदि (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	प्रत्यागम rc;
}

पूर्णाङ्क sbi_irqchip_समुच्चयः_raw_handler(संरचना sbi_irqchip_device *chip, u32 hwirq,
				पूर्णाङ्क (*raw_hndl)(संरचना sbi_irqchip_device *, u32))
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	दत्त = &chip->hwirqs[hwirq];
	दत्त->raw_handler = raw_hndl;
	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_write_msi(संरचना sbi_irqchip_device *chip, u32 hwirq,
			  ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः)
{
	संरचना sbi_irqchip_handler *h;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq || !सन्देशः)
		प्रत्यागम SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	यदि (!h)
		प्रत्यागम SBI_EFAIL;
	यदि (!h->write_msi)
		प्रत्यागम SBI_ENOTSUPP;

	h->write_msi(hwirq, सन्देशः, h->priv);
	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_get_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 *बहिर्गमनम्_hart_सूचकांक)
{
	यदि (!chip || chip->अङ्कः_hwirq <= hwirq)
		प्रत्यागम SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	यदि (!sbi_irqchip_find_handler(chip, hwirq))
		प्रत्यागम SBI_ENOTSUPP;

	*बहिर्गमनम्_hart_सूचकांक = chip->hwirqs[hwirq].hart_सूचकांक;
	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_समुच्चयः_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_सूचकांक)
{
	संरचना sbi_irqchip_hwirq_दत्त *दत्त;
	पूर्णाङ्क rc;

	यदि (!chip || chip->अङ्कः_hwirq <= hwirq || sbi_hart_गणनम्() <= hart_सूचकांक)
		प्रत्यागम SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	यदि (!sbi_irqchip_find_handler(chip, hwirq))
		प्रत्यागम SBI_ENOTSUPP;

	दत्त = &chip->hwirqs[hwirq];
	यदि (दत्त->hart_सूचकांक != hart_सूचकांक) {
		यदि (chip->hwirq_समुच्चयः_affinity) {
			rc = chip->hwirq_समुच्चयः_affinity(chip, hwirq, hart_सूचकांक);
			यदि (rc)
				प्रत्यागम rc;
		}
		दत्त->hart_सूचकांक = hart_सूचकांक;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क __sbi_irqchip_handler_set_affinity(संरचना sbi_irqchip_device *chip,
					      संरचना sbi_irqchip_handler *h,
					      u32 compare_hart_सूचकांक,
					      u32 hart_सूचकांक)
{
	u32 i, current_hart_सूचकांक;
	पूर्णाङ्क rc;

	प्रति (i = 0; i < h->अङ्कः_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_सूचकांक);
		यदि (rc)
			प्रत्यागम rc;

		यदि (compare_hart_सूचकांक != -1U &&
		    current_hart_सूचकांक != compare_hart_सूचकांक)
			अनुवर्त;

		rc = sbi_irqchip_समुच्चयः_affinity(chip, h->first_hwirq + i, hart_सूचकांक);
		यदि (rc)
			प्रत्यागम rc;
	}

	प्रत्यागम 0;
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

	प्रति (i = first_hwirq; i < (first_hwirq + अङ्कः_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		यदि (h)
			प्रत्यागम SBI_EALREADY;
	}

	h = sbi_zalloc(परिमाण(*h));
	यदि (!h)
		प्रत्यागम SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->अङ्कः_hwirq = अङ्कः_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = रिक्त;
	sbi_list_for_each_प्रविष्टिः(th, &chip->handler_list, ग्रन्थिः) {
		यदि (h->first_hwirq < th->first_hwirq) {
			nh = th;
			भङ्ग;
		}
	}
	यदि (nh)
		sbi_list_सङ्कलय(&h->ग्रन्थिः, &nh->ग्रन्थिः);
	अन्यथा
		sbi_list_सङ्कलय_पुच्छ(&h->ग्रन्थिः, &chip->handler_list);

	यदि (chip->hwirq_सज्जता) {
		प्रति (i = 0; i < h->अङ्कः_hwirq; i++) {
			rc = chip->hwirq_सज्जता(chip, h->first_hwirq + i, hwirq_flags);
			यदि (rc) {
				यदि (chip->hwirq_cleanup) {
					प्रति (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->ग्रन्थिः);
				sbi_free(h);
				प्रत्यागम rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	यदि (rc) {
		यदि (chip->hwirq_cleanup) {
			प्रति (i = 0; i < h->अङ्कः_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->ग्रन्थिः);
		sbi_free(h);
		प्रत्यागम rc;
	}

	यदि (chip->hwirq_unmask) {
		प्रति (i = 0; i < h->अङ्कः_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_पञ्जिकाकोशः_msi(संरचना sbi_irqchip_device *chip, u32 अङ्कः_hwirq,
			     शून्य (*write_msi)(u32 hwirq,
					       ध्रुव संरचना sbi_irqchip_msi_सन्देशः *सन्देशः,
					       शून्य *priv),
			     पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv,
			     u32 *बहिर्गमनम्_first_hwirq)
{
	संरचना sbi_irqchip_handler *h;
	bool प्राप्तम्;
	u32 hwirq;

	यदि (!chip || !chip->hwirq_समुच्चयः_affinity || !अङ्कः_hwirq ||
	    !write_msi || !callback || !बहिर्गमनम्_first_hwirq)
		प्रत्यागम SBI_EINVAL;
	यदि (chip->अङ्कः_hwirq < अङ्कः_hwirq)
		प्रत्यागम SBI_EBAD_RANGE;

	hwirq = 0;
	प्राप्तम् = false;
	sbi_list_for_each_प्रविष्टिः(h, &chip->handler_list, ग्रन्थिः) {
		यदि (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अङ्कः_hwirq)) {
			hwirq = h->first_hwirq + h->अङ्कः_hwirq;
		} अन्यथा यदि (hwirq < h->first_hwirq) {
			यदि (h->first_hwirq - hwirq >= अङ्कः_hwirq) {
				प्राप्तम् = true;
				भङ्ग;
			} अन्यथा {
				hwirq = h->first_hwirq + h->अङ्कः_hwirq;
			}
		}
	}
	यदि (!प्राप्तम् && (chip->अङ्कः_hwirq - hwirq) >= अङ्कः_hwirq)
		प्राप्तम् = true;
	यदि (!प्राप्तम्)
		प्रत्यागम SBI_ENOSPC;
	*बहिर्गमनम्_first_hwirq = hwirq;

	प्रत्यागम __sbi_irqchip_register_handler(chip, *बहिर्गमनम्_first_hwirq,
					      अङ्कः_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

पूर्णाङ्क sbi_irqchip_पञ्जिकाकोशः_handler(संरचना sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 अङ्कः_hwirq, u32 hwirq_flags,
				 पूर्णाङ्क (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	यदि (!chip || !अङ्कः_hwirq || !callback)
		प्रत्यागम SBI_EINVAL;
	यदि (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		प्रत्यागम SBI_EBAD_RANGE;

	प्रत्यागम __sbi_irqchip_register_handler(chip, first_hwirq, अङ्कः_hwirq, hwirq_flags,
					      रिक्त, callback, priv);
}

पूर्णाङ्क sbi_irqchip_पञ्जिकाकोशः_reserved(संरचना sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 अङ्कः_hwirq)
{
	यदि (!chip || !अङ्कः_hwirq)
		प्रत्यागम SBI_EINVAL;
	यदि (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		प्रत्यागम SBI_EBAD_RANGE;

	प्रत्यागम __sbi_irqchip_register_handler(chip, first_hwirq, अङ्कः_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, रिक्त, रिक्त, रिक्त);
}

पूर्णाङ्क sbi_irqchip_unregister_handler(संरचना sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 अङ्कः_hwirq)
{
	संरचना sbi_irqchip_handler *fh, *lh;
	u32 i;

	यदि (!chip || !अङ्कः_hwirq)
		प्रत्यागम SBI_EINVAL;
	यदि (chip->अङ्कः_hwirq <= first_hwirq ||
	    chip->अङ्कः_hwirq <= (first_hwirq + अङ्कः_hwirq - 1))
		प्रत्यागम SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	यदि (!fh || fh->first_hwirq != first_hwirq || fh->अङ्कः_hwirq != अङ्कः_hwirq)
		प्रत्यागम SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + अङ्कः_hwirq - 1);
	यदि (!lh || lh != fh)
		प्रत्यागम SBI_ENODEV;

	यदि (chip->hwirq_आवरणम्) {
		प्रति (i = 0; i < fh->अङ्कः_hwirq; i++)
			chip->hwirq_आवरणम्(chip, fh->first_hwirq + i);
	}

	यदि (chip->hwirq_cleanup) {
		प्रति (i = 0; i < fh->अङ्कः_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->ग्रन्थिः);
	प्रत्यागम 0;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device_by_caps(अचिह्नित दीर्घ caps,
							   संरचना sbi_irqchip_device *first)
{
	संरचना sbi_irqchip_device *chip;
	bool प्राप्तम् = (first == रिक्त);

	sbi_list_for_each_प्रविष्टिः(chip, &irqchip_list, ग्रन्थिः) {
		यदि (!प्राप्तम्) {
			यदि (first == chip)
				प्राप्तम् = true;
			अनुवर्त;
		}
		यदि ((chip->caps & caps) == caps)
			प्रत्यागम chip;
	}

	प्रत्यागम रिक्त;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	संरचना sbi_irqchip_device *chip;

	sbi_list_for_each_प्रविष्टिः(chip, &irqchip_list, ग्रन्थिः) {
		यदि (chip->id == id)
			प्रत्यागम chip;
	}

	प्रत्यागम रिक्त;
}

पूर्णाङ्क sbi_irqchip_सङ्कलय_device(संरचना sbi_irqchip_device *chip)
{
	संरचना sbi_irqchip_hart_दत्त *hd;
	संरचना sbi_scratch *scratch;
	u32 i, h;

	यदि (!chip || !chip->अङ्कः_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		प्रत्यागम SBI_EINVAL;

	यदि (sbi_irqchip_find_device(chip->id))
		प्रत्यागम SBI_EALREADY;

	यदि (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_गन्तव्यम्_scratch(h);
			यदि (!scratch)
				अनुवर्त;

			hd = sbi_scratch_विस्थापनम्_ptr(scratch, irqchip_hart_दत्त_off);
			यदि (hd->chip && hd->chip != chip)
				प्रत्यागम SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(परिमाण(*chip->hwirqs) * chip->अङ्कः_hwirq);
	यदि (!chip->hwirqs)
		प्रत्यागम SBI_ENOMEM;
	प्रति (i = 0; i < chip->अङ्कः_hwirq; i++) {
		sbi_irqchip_समुच्चयः_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_सूचकांक = -1U;
	}

	SBI_INIT_LIST_शीर्ष(&chip->handler_list);

	sbi_list_सङ्कलय_पुच्छ(&chip->ग्रन्थिः, &irqchip_list);
	प्रत्यागम 0;
}

पूर्णाङ्क sbi_irqchip_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	ध्रुव संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना sbi_irqchip_hart_दत्त *hd;
	संरचना sbi_irqchip_device *chip;
	पूर्णाङ्क rc;

	यदि (cold_boot) {
		irqchip_hart_दत्त_off =
			sbi_scratch_alloc_विस्थापनम्(परिमाण(संरचना sbi_irqchip_hart_दत्त));
		यदि (!irqchip_hart_दत्त_off)
			प्रत्यागम SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		यदि (rc)
			प्रत्यागम rc;
	}

	sbi_list_for_each_प्रविष्टिः(chip, &irqchip_list, ग्रन्थिः) {
		यदि (!chip->warm_init)
			अनुवर्त;
		यदि (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			अनुवर्त;
		rc = chip->warm_init(chip);
		यदि (rc)
			प्रत्यागम rc;
	}

	hd = sbi_scratch_thishart_विस्थापनम्_ptr(irqchip_hart_दत्त_off);
	यदि (hd && hd->chip && hd->chip->process_hwirqs)
		csr_समुच्चयः(CSR_MIE, MIP_MEIP);

	प्रत्यागम 0;
}

शून्य sbi_irqchip_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_irqchip_hart_दत्त *hd;
	संरचना sbi_irqchip_device *chip;
	संरचना sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	पूर्णाङ्क rc;

	sbi_for_each_hartindex(i) {
		यदि (i == current_hartindex())
			अनुवर्त;
		यदि (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			अनुवर्त;
		migrate_hidx = i;
		migrate = true;
		भङ्ग;
	}

	यदि (!migrate)
		गच्छ skip_migrate;
	sbi_list_for_each_प्रविष्टिः(chip, &irqchip_list, ग्रन्थिः) {
		sbi_list_for_each_प्रविष्टिः(h, &chip->handler_list, ग्रन्थिः) {
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

	hd = sbi_scratch_thishart_विस्थापनम्_ptr(irqchip_hart_दत्त_off);
	यदि (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
