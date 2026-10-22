/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_irqchip.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
काष्ठा sbi_irqchip_hwirq_तथ्य {
	/** raw hardware interrupt handler */
	पूर्णांक (*raw_handler)(काष्ठा sbi_irqchip_device *chip, u32 hwirq);

#घोषणा IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_सूचकांक;

	/** chip's private data */
	व्योम *priv;
};

/** Internal irqchip interrupt handler */
काष्ठा sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	काष्ठा sbi_dlist गाँठ;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 अंक_hwirq;

	/** Write MSI function of this handler */
	व्योम (*write_msi)(u32 hwirq, स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश, व्योम *priv);

	/** Callback function of this handler */
	पूर्णांक (*callback)(u32 hwirq, व्योम *priv);

	/** Callback private data */
	व्योम *priv;
};

काष्ठा sbi_irqchip_hart_तथ्य {
	काष्ठा sbi_irqchip_device *chip;
};

अटल अचिन्हित दीर्घ irqchip_hart_तथ्य_off;
अटल SBI_LIST_शीर्ष(irqchip_list);

पूर्णांक sbi_irqchip_process(व्योम)
{
	काष्ठा sbi_irqchip_hart_तथ्य *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_तथ्य_off);
	अगर (!hd || !hd->chip || !hd->chip->process_hwirqs)
		वापस SBI_ENODEV;

	वापस hd->chip->process_hwirqs(hd->chip);
}

पूर्णांक sbi_irqchip_process_hwirq(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	तथ्य = &chip->hwirqs[hwirq];
	अगर (!तथ्य->raw_handler)
		वापस SBI_ENOENT;

	वापस तथ्य->raw_handler(chip, hwirq);
}

अटल अंतरभूत u32 sbi_irqchip_get_irq_state(काष्ठा sbi_irqchip_device *chip,
					     u32 hwirq)
{
	अगर (!chip || !chip->hwirqs || hwirq >= chip->अंक_hwirq)
		वापस 0;

	वापस chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(काष्ठा sbi_irqchip_device *chip,
				   u32 hwirq)
{
	वापस !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

पूर्णांक sbi_irqchip_unmask_hwirq(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	तथ्य = &chip->hwirqs[hwirq];
	अगर (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		वापस SBI_EALREADY;

	अगर (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	तथ्य->irq_state |= IRQ_ENABLED;
	वापस 0;
}

पूर्णांक sbi_irqchip_mask_hwirq(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	अगर (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		वापस SBI_EALREADY;

	अगर (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	तथ्य = &chip->hwirqs[hwirq];
	तथ्य->irq_state &= ~IRQ_ENABLED;
	वापस 0;
}

अटल काष्ठा sbi_irqchip_handler *sbi_irqchip_find_handler(काष्ठा sbi_irqchip_device *chip,
							    u32 hwirq)
{
	काष्ठा sbi_irqchip_handler *h;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस शून्य;

	sbi_list_for_each_entry(h, &chip->handler_list, गाँठ) {
		अगर (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अंक_hwirq))
			वापस h;
	}

	वापस शून्य;
}

पूर्णांक sbi_irqchip_set_hwirq_priv(काष्ठा sbi_irqchip_device *chip, u32 hwirq, व्योम *priv)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
	     वापस SBI_EINVAL;

	तथ्य = &chip->hwirqs[hwirq];
	तथ्य->priv = priv;

	वापस 0;
}

व्योम *sbi_irqchip_get_hwirq_priv(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;
	अगर (!chip || chip->अंक_hwirq <= hwirq)
	    वापस शून्य;

	तथ्य = &chip->hwirqs[hwirq];

	वापस तथ्य->priv;
}

पूर्णांक sbi_irqchip_raw_handler_default(काष्ठा sbi_irqchip_device *chip, u32 hwirq)
{
	काष्ठा sbi_irqchip_handler *h;
	पूर्णांक rc = SBI_OK;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	अगर (!h)
		वापस SBI_EINVAL;

	अगर (h->callback)
		rc = h->callback(hwirq, h->priv);

	अगर (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	वापस rc;
}

पूर्णांक sbi_irqchip_set_raw_handler(काष्ठा sbi_irqchip_device *chip, u32 hwirq,
				पूर्णांक (*raw_hndl)(काष्ठा sbi_irqchip_device *, u32))
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;

	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	तथ्य = &chip->hwirqs[hwirq];
	तथ्य->raw_handler = raw_hndl;
	वापस 0;
}

पूर्णांक sbi_irqchip_write_msi(काष्ठा sbi_irqchip_device *chip, u32 hwirq,
			  स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश)
{
	काष्ठा sbi_irqchip_handler *h;

	अगर (!chip || chip->अंक_hwirq <= hwirq || !संदेश)
		वापस SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	अगर (!h)
		वापस SBI_EFAIL;
	अगर (!h->write_msi)
		वापस SBI_ENOTSUPP;

	h->write_msi(hwirq, संदेश, h->priv);
	वापस 0;
}

पूर्णांक sbi_irqchip_get_affinity(काष्ठा sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_सूचकांक)
{
	अगर (!chip || chip->अंक_hwirq <= hwirq)
		वापस SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	अगर (!sbi_irqchip_find_handler(chip, hwirq))
		वापस SBI_ENOTSUPP;

	*out_hart_सूचकांक = chip->hwirqs[hwirq].hart_सूचकांक;
	वापस 0;
}

पूर्णांक sbi_irqchip_set_affinity(काष्ठा sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_सूचकांक)
{
	काष्ठा sbi_irqchip_hwirq_तथ्य *तथ्य;
	पूर्णांक rc;

	अगर (!chip || chip->अंक_hwirq <= hwirq || sbi_hart_गिनती() <= hart_सूचकांक)
		वापस SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	अगर (!sbi_irqchip_find_handler(chip, hwirq))
		वापस SBI_ENOTSUPP;

	तथ्य = &chip->hwirqs[hwirq];
	अगर (तथ्य->hart_सूचकांक != hart_सूचकांक) {
		अगर (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_सूचकांक);
			अगर (rc)
				वापस rc;
		}
		तथ्य->hart_सूचकांक = hart_सूचकांक;
	}

	वापस 0;
}

अटल पूर्णांक __sbi_irqchip_handler_set_affinity(काष्ठा sbi_irqchip_device *chip,
					      काष्ठा sbi_irqchip_handler *h,
					      u32 compare_hart_सूचकांक,
					      u32 hart_सूचकांक)
{
	u32 i, current_hart_सूचकांक;
	पूर्णांक rc;

	क्रम (i = 0; i < h->अंक_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_सूचकांक);
		अगर (rc)
			वापस rc;

		अगर (compare_hart_सूचकांक != -1U &&
		    current_hart_सूचकांक != compare_hart_सूचकांक)
			जारी;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_सूचकांक);
		अगर (rc)
			वापस rc;
	}

	वापस 0;
}

अटल पूर्णांक __sbi_irqchip_register_handler(काष्ठा sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 अंक_hwirq, u32 hwirq_flags,
					  व्योम (*write_msi)(u32 hwirq,
							    स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश,
							    व्योम *priv),
					  पूर्णांक (*callback)(u32 hwirq, व्योम *priv), व्योम *priv)
{
	काष्ठा sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	पूर्णांक rc;

	क्रम (i = first_hwirq; i < (first_hwirq + अंक_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		अगर (h)
			वापस SBI_EALREADY;
	}

	h = sbi_zalloc(माप(*h));
	अगर (!h)
		वापस SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->अंक_hwirq = अंक_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = शून्य;
	sbi_list_for_each_entry(th, &chip->handler_list, गाँठ) {
		अगर (h->first_hwirq < th->first_hwirq) {
			nh = th;
			अवरोध;
		}
	}
	अगर (nh)
		sbi_list_add(&h->गाँठ, &nh->गाँठ);
	अन्यथा
		sbi_list_add_पुच्छ(&h->गाँठ, &chip->handler_list);

	अगर (chip->hwirq_setup) {
		क्रम (i = 0; i < h->अंक_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			अगर (rc) {
				अगर (chip->hwirq_cleanup) {
					क्रम (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->गाँठ);
				sbi_free(h);
				वापस rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	अगर (rc) {
		अगर (chip->hwirq_cleanup) {
			क्रम (i = 0; i < h->अंक_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->गाँठ);
		sbi_free(h);
		वापस rc;
	}

	अगर (chip->hwirq_unmask) {
		क्रम (i = 0; i < h->अंक_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	वापस 0;
}

पूर्णांक sbi_irqchip_register_msi(काष्ठा sbi_irqchip_device *chip, u32 अंक_hwirq,
			     व्योम (*write_msi)(u32 hwirq,
					       स्थिर काष्ठा sbi_irqchip_msi_संदेश *संदेश,
					       व्योम *priv),
			     पूर्णांक (*callback)(u32 hwirq, व्योम *priv), व्योम *priv,
			     u32 *out_first_hwirq)
{
	काष्ठा sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	अगर (!chip || !chip->hwirq_set_affinity || !अंक_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		वापस SBI_EINVAL;
	अगर (chip->अंक_hwirq < अंक_hwirq)
		वापस SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, गाँठ) {
		अगर (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अंक_hwirq)) {
			hwirq = h->first_hwirq + h->अंक_hwirq;
		} अन्यथा अगर (hwirq < h->first_hwirq) {
			अगर (h->first_hwirq - hwirq >= अंक_hwirq) {
				found = true;
				अवरोध;
			} अन्यथा {
				hwirq = h->first_hwirq + h->अंक_hwirq;
			}
		}
	}
	अगर (!found && (chip->अंक_hwirq - hwirq) >= अंक_hwirq)
		found = true;
	अगर (!found)
		वापस SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	वापस __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      अंक_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

पूर्णांक sbi_irqchip_register_handler(काष्ठा sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 अंक_hwirq, u32 hwirq_flags,
				 पूर्णांक (*callback)(u32 hwirq, व्योम *priv), व्योम *priv)
{
	अगर (!chip || !अंक_hwirq || !callback)
		वापस SBI_EINVAL;
	अगर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		वापस SBI_EBAD_RANGE;

	वापस __sbi_irqchip_register_handler(chip, first_hwirq, अंक_hwirq, hwirq_flags,
					      शून्य, callback, priv);
}

पूर्णांक sbi_irqchip_register_reserved(काष्ठा sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 अंक_hwirq)
{
	अगर (!chip || !अंक_hwirq)
		वापस SBI_EINVAL;
	अगर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		वापस SBI_EBAD_RANGE;

	वापस __sbi_irqchip_register_handler(chip, first_hwirq, अंक_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, शून्य, शून्य, शून्य);
}

पूर्णांक sbi_irqchip_unregister_handler(काष्ठा sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 अंक_hwirq)
{
	काष्ठा sbi_irqchip_handler *fh, *lh;
	u32 i;

	अगर (!chip || !अंक_hwirq)
		वापस SBI_EINVAL;
	अगर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		वापस SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	अगर (!fh || fh->first_hwirq != first_hwirq || fh->अंक_hwirq != अंक_hwirq)
		वापस SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + अंक_hwirq - 1);
	अगर (!lh || lh != fh)
		वापस SBI_ENODEV;

	अगर (chip->hwirq_mask) {
		क्रम (i = 0; i < fh->अंक_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	अगर (chip->hwirq_cleanup) {
		क्रम (i = 0; i < fh->अंक_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->गाँठ);
	वापस 0;
}

काष्ठा sbi_irqchip_device *sbi_irqchip_find_device_by_caps(अचिन्हित दीर्घ caps,
							   काष्ठा sbi_irqchip_device *first)
{
	काष्ठा sbi_irqchip_device *chip;
	bool found = (first == शून्य);

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठ) {
		अगर (!found) {
			अगर (first == chip)
				found = true;
			जारी;
		}
		अगर ((chip->caps & caps) == caps)
			वापस chip;
	}

	वापस शून्य;
}

काष्ठा sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	काष्ठा sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठ) {
		अगर (chip->id == id)
			वापस chip;
	}

	वापस शून्य;
}

पूर्णांक sbi_irqchip_add_device(काष्ठा sbi_irqchip_device *chip)
{
	काष्ठा sbi_irqchip_hart_तथ्य *hd;
	काष्ठा sbi_scratch *scratch;
	u32 i, h;

	अगर (!chip || !chip->अंक_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		वापस SBI_EINVAL;

	अगर (sbi_irqchip_find_device(chip->id))
		वापस SBI_EALREADY;

	अगर (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			अगर (!scratch)
				जारी;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_तथ्य_off);
			अगर (hd->chip && hd->chip != chip)
				वापस SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(माप(*chip->hwirqs) * chip->अंक_hwirq);
	अगर (!chip->hwirqs)
		वापस SBI_ENOMEM;
	क्रम (i = 0; i < chip->अंक_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_सूचकांक = -1U;
	}

	SBI_INIT_LIST_शीर्ष(&chip->handler_list);

	sbi_list_add_पुच्छ(&chip->गाँठ, &irqchip_list);
	वापस 0;
}

पूर्णांक sbi_irqchip_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	स्थिर काष्ठा sbi_platform *plat = sbi_platform_ptr(scratch);
	काष्ठा sbi_irqchip_hart_तथ्य *hd;
	काष्ठा sbi_irqchip_device *chip;
	पूर्णांक rc;

	अगर (cold_boot) {
		irqchip_hart_तथ्य_off =
			sbi_scratch_alloc_offset(माप(काष्ठा sbi_irqchip_hart_तथ्य));
		अगर (!irqchip_hart_तथ्य_off)
			वापस SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		अगर (rc)
			वापस rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, गाँठ) {
		अगर (!chip->warm_init)
			जारी;
		अगर (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			जारी;
		rc = chip->warm_init(chip);
		अगर (rc)
			वापस rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_तथ्य_off);
	अगर (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	वापस 0;
}

व्योम sbi_irqchip_exit(काष्ठा sbi_scratch *scratch)
{
	काष्ठा sbi_irqchip_hart_तथ्य *hd;
	काष्ठा sbi_irqchip_device *chip;
	काष्ठा sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	पूर्णांक rc;

	sbi_for_each_hartindex(i) {
		अगर (i == current_hartindex())
			जारी;
		अगर (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			जारी;
		migrate_hidx = i;
		migrate = true;
		अवरोध;
	}

	अगर (!migrate)
		जाओ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, गाँठ) {
		sbi_list_for_each_entry(h, &chip->handler_list, गाँठ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			अगर (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_तथ्य_off);
	अगर (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
