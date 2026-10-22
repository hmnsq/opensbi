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
संरचना sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	पूर्णांक (*raw_handler)(संरचना sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	शून्य *priv;
};

/** Internal irqchip interrupt handler */
संरचना sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	संरचना sbi_dlist गाठ;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 अंक_hwirq;

	/** Write MSI function of this handler */
	शून्य (*write_msi)(u32 hwirq, स्थिरांक संरचना sbi_irqchip_msi_संदेश *संदेश, शून्य *priv);

	/** Callback function of this handler */
	पूर्णांक (*callback)(u32 hwirq, शून्य *priv);

	/** Callback private data */
	शून्य *priv;
};

संरचना sbi_irqchip_hart_data {
	संरचना sbi_irqchip_device *chip;
};

स्थिर अचिन्हित दीर्घ irqchip_hart_data_off;
स्थिर SBI_LIST_HEAD(irqchip_list);

पूर्णांक sbi_irqchip_process(शून्य)
{
	संरचना sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	जर (!hd || !hd->chip || !hd->chip->process_hwirqs)
		परत SBI_ENODEV;

	परत hd->chip->process_hwirqs(hd->chip);
}

पूर्णांक sbi_irqchip_process_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	जर (!data->raw_handler)
		परत SBI_ENOENT;

	परत data->raw_handler(chip, hwirq);
}

स्थिर ओळीत u32 sbi_irqchip_get_irq_state(संरचना sbi_irqchip_device *chip,
					     u32 hwirq)
{
	जर (!chip || !chip->hwirqs || hwirq >= chip->अंक_hwirq)
		परत 0;

	परत chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(संरचना sbi_irqchip_device *chip,
				   u32 hwirq)
{
	परत !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

पूर्णांक sbi_irqchip_unmask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	जर (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		परत SBI_EALREADY;

	जर (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	परत 0;
}

पूर्णांक sbi_irqchip_mask_hwirq(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	जर (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		परत SBI_EALREADY;

	जर (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	परत 0;
}

स्थिर संरचना sbi_irqchip_handler *sbi_irqchip_find_handler(संरचना sbi_irqchip_device *chip,
							    u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत रिक्त;

	sbi_list_for_each_entry(h, &chip->handler_list, गाठ) {
		जर (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अंक_hwirq))
			परत h;
	}

	परत रिक्त;
}

पूर्णांक sbi_irqchip_set_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq, शून्य *priv)
{
	संरचना sbi_irqchip_hwirq_data *data;

	जर (!chip || chip->अंक_hwirq <= hwirq)
	     परत SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	परत 0;
}

शून्य *sbi_irqchip_get_hwirq_priv(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_hwirq_data *data;
	जर (!chip || chip->अंक_hwirq <= hwirq)
	    परत रिक्त;

	data = &chip->hwirqs[hwirq];

	परत data->priv;
}

पूर्णांक sbi_irqchip_raw_handler_default(संरचना sbi_irqchip_device *chip, u32 hwirq)
{
	संरचना sbi_irqchip_handler *h;
	पूर्णांक rc = SBI_OK;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	जर (!h)
		परत SBI_EINVAL;

	जर (h->callback)
		rc = h->callback(hwirq, h->priv);

	जर (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	परत rc;
}

पूर्णांक sbi_irqchip_set_raw_handler(संरचना sbi_irqchip_device *chip, u32 hwirq,
				पूर्णांक (*raw_hndl)(संरचना sbi_irqchip_device *, u32))
{
	संरचना sbi_irqchip_hwirq_data *data;

	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	परत 0;
}

पूर्णांक sbi_irqchip_write_msi(संरचना sbi_irqchip_device *chip, u32 hwirq,
			  स्थिरांक संरचना sbi_irqchip_msi_संदेश *संदेश)
{
	संरचना sbi_irqchip_handler *h;

	जर (!chip || chip->अंक_hwirq <= hwirq || !संदेश)
		परत SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	जर (!h)
		परत SBI_EFAIL;
	जर (!h->write_msi)
		परत SBI_ENOTSUPP;

	h->write_msi(hwirq, संदेश, h->priv);
	परत 0;
}

पूर्णांक sbi_irqchip_get_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	जर (!chip || chip->अंक_hwirq <= hwirq)
		परत SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	जर (!sbi_irqchip_find_handler(chip, hwirq))
		परत SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	परत 0;
}

पूर्णांक sbi_irqchip_set_affinity(संरचना sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	संरचना sbi_irqchip_hwirq_data *data;
	पूर्णांक rc;

	जर (!chip || chip->अंक_hwirq <= hwirq || sbi_hart_मोजणी() <= hart_index)
		परत SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	जर (!sbi_irqchip_find_handler(chip, hwirq))
		परत SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	जर (data->hart_index != hart_index) {
		जर (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			जर (rc)
				परत rc;
		}
		data->hart_index = hart_index;
	}

	परत 0;
}

स्थिर पूर्णांक __sbi_irqchip_handler_set_affinity(संरचना sbi_irqchip_device *chip,
					      संरचना sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	पूर्णांक rc;

	साठी (i = 0; i < h->अंक_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		जर (rc)
			परत rc;

		जर (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			सुरूठेवा;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		जर (rc)
			परत rc;
	}

	परत 0;
}

स्थिर पूर्णांक __sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 अंक_hwirq, u32 hwirq_flags,
					  शून्य (*write_msi)(u32 hwirq,
							    स्थिरांक संरचना sbi_irqchip_msi_संदेश *संदेश,
							    शून्य *priv),
					  पूर्णांक (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	संरचना sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	पूर्णांक rc;

	साठी (i = first_hwirq; i < (first_hwirq + अंक_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		जर (h)
			परत SBI_EALREADY;
	}

	h = sbi_zalloc(आकार(*h));
	जर (!h)
		परत SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->अंक_hwirq = अंक_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = रिक्त;
	sbi_list_for_each_entry(th, &chip->handler_list, गाठ) {
		जर (h->first_hwirq < th->first_hwirq) {
			nh = th;
			थांबा;
		}
	}
	जर (nh)
		sbi_list_add(&h->गाठ, &nh->गाठ);
	अन्यथा
		sbi_list_add_tail(&h->गाठ, &chip->handler_list);

	जर (chip->hwirq_setup) {
		साठी (i = 0; i < h->अंक_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			जर (rc) {
				जर (chip->hwirq_cleanup) {
					साठी (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->गाठ);
				sbi_free(h);
				परत rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	जर (rc) {
		जर (chip->hwirq_cleanup) {
			साठी (i = 0; i < h->अंक_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->गाठ);
		sbi_free(h);
		परत rc;
	}

	जर (chip->hwirq_unmask) {
		साठी (i = 0; i < h->अंक_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	परत 0;
}

पूर्णांक sbi_irqchip_register_msi(संरचना sbi_irqchip_device *chip, u32 अंक_hwirq,
			     शून्य (*write_msi)(u32 hwirq,
					       स्थिरांक संरचना sbi_irqchip_msi_संदेश *संदेश,
					       शून्य *priv),
			     पूर्णांक (*callback)(u32 hwirq, शून्य *priv), शून्य *priv,
			     u32 *out_first_hwirq)
{
	संरचना sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	जर (!chip || !chip->hwirq_set_affinity || !अंक_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		परत SBI_EINVAL;
	जर (chip->अंक_hwirq < अंक_hwirq)
		परत SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, गाठ) {
		जर (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->अंक_hwirq)) {
			hwirq = h->first_hwirq + h->अंक_hwirq;
		} अन्यथा जर (hwirq < h->first_hwirq) {
			जर (h->first_hwirq - hwirq >= अंक_hwirq) {
				found = true;
				थांबा;
			} अन्यथा {
				hwirq = h->first_hwirq + h->अंक_hwirq;
			}
		}
	}
	जर (!found && (chip->अंक_hwirq - hwirq) >= अंक_hwirq)
		found = true;
	जर (!found)
		परत SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	परत __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      अंक_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

पूर्णांक sbi_irqchip_register_handler(संरचना sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 अंक_hwirq, u32 hwirq_flags,
				 पूर्णांक (*callback)(u32 hwirq, शून्य *priv), शून्य *priv)
{
	जर (!chip || !अंक_hwirq || !callback)
		परत SBI_EINVAL;
	जर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		परत SBI_EBAD_RANGE;

	परत __sbi_irqchip_register_handler(chip, first_hwirq, अंक_hwirq, hwirq_flags,
					      रिक्त, callback, priv);
}

पूर्णांक sbi_irqchip_register_reserved(संरचना sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 अंक_hwirq)
{
	जर (!chip || !अंक_hwirq)
		परत SBI_EINVAL;
	जर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		परत SBI_EBAD_RANGE;

	परत __sbi_irqchip_register_handler(chip, first_hwirq, अंक_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, रिक्त, रिक्त, रिक्त);
}

पूर्णांक sbi_irqchip_unregister_handler(संरचना sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 अंक_hwirq)
{
	संरचना sbi_irqchip_handler *fh, *lh;
	u32 i;

	जर (!chip || !अंक_hwirq)
		परत SBI_EINVAL;
	जर (chip->अंक_hwirq <= first_hwirq ||
	    chip->अंक_hwirq <= (first_hwirq + अंक_hwirq - 1))
		परत SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	जर (!fh || fh->first_hwirq != first_hwirq || fh->अंक_hwirq != अंक_hwirq)
		परत SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + अंक_hwirq - 1);
	जर (!lh || lh != fh)
		परत SBI_ENODEV;

	जर (chip->hwirq_mask) {
		साठी (i = 0; i < fh->अंक_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	जर (chip->hwirq_cleanup) {
		साठी (i = 0; i < fh->अंक_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->गाठ);
	परत 0;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device_by_caps(अचिन्हित दीर्घ caps,
							   संरचना sbi_irqchip_device *first)
{
	संरचना sbi_irqchip_device *chip;
	bool found = (first == रिक्त);

	sbi_list_for_each_entry(chip, &irqchip_list, गाठ) {
		जर (!found) {
			जर (first == chip)
				found = true;
			सुरूठेवा;
		}
		जर ((chip->caps & caps) == caps)
			परत chip;
	}

	परत रिक्त;
}

संरचना sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	संरचना sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, गाठ) {
		जर (chip->id == id)
			परत chip;
	}

	परत रिक्त;
}

पूर्णांक sbi_irqchip_add_device(संरचना sbi_irqchip_device *chip)
{
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_scratch *scratch;
	u32 i, h;

	जर (!chip || !chip->अंक_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		परत SBI_EINVAL;

	जर (sbi_irqchip_find_device(chip->id))
		परत SBI_EALREADY;

	जर (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			जर (!scratch)
				सुरूठेवा;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			जर (hd->chip && hd->chip != chip)
				परत SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(आकार(*chip->hwirqs) * chip->अंक_hwirq);
	जर (!chip->hwirqs)
		परत SBI_ENOMEM;
	साठी (i = 0; i < chip->अंक_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->गाठ, &irqchip_list);
	परत 0;
}

पूर्णांक sbi_irqchip_init(संरचना sbi_scratch *scratch, bool cold_boot)
{
	स्थिरांक संरचना sbi_platform *plat = sbi_platform_ptr(scratch);
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_irqchip_device *chip;
	पूर्णांक rc;

	जर (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(आकार(संरचना sbi_irqchip_hart_data));
		जर (!irqchip_hart_data_off)
			परत SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		जर (rc)
			परत rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, गाठ) {
		जर (!chip->warm_init)
			सुरूठेवा;
		जर (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			सुरूठेवा;
		rc = chip->warm_init(chip);
		जर (rc)
			परत rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	जर (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	परत 0;
}

शून्य sbi_irqchip_exit(संरचना sbi_scratch *scratch)
{
	संरचना sbi_irqchip_hart_data *hd;
	संरचना sbi_irqchip_device *chip;
	संरचना sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	पूर्णांक rc;

	sbi_for_each_hartindex(i) {
		जर (i == current_hartindex())
			सुरूठेवा;
		जर (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			सुरूठेवा;
		migrate_hidx = i;
		migrate = true;
		थांबा;
	}

	जर (!migrate)
		जा skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, गाठ) {
		sbi_list_for_each_entry(h, &chip->handler_list, गाठ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			जर (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	जर (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
