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
struct sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	정수 (*raw_handler)(struct sbi_irqchip_device *chip, u32 hwirq);

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
struct sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	struct sbi_dlist node;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 num_hwirq;

	/** Write MSI function of this handler */
	void (*write_msi)(u32 hwirq, const struct sbi_irqchip_msi_msg *msg, void *priv);

	/** Callback function of this handler */
	정수 (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

struct sbi_irqchip_hart_data {
	struct sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

정수 sbi_irqchip_process(void)
{
	struct sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	만약 (!hd || !hd->chip || !hd->chip->process_hwirqs)
		반환 SBI_ENODEV;

	반환 hd->chip->process_hwirqs(hd->chip);
}

정수 sbi_irqchip_process_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	만약 (!data->raw_handler)
		반환 SBI_ENOENT;

	반환 data->raw_handler(chip, hwirq);
}

static inline u32 sbi_irqchip_get_irq_state(struct sbi_irqchip_device *chip,
					     u32 hwirq)
{
	만약 (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		반환 0;

	반환 chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(struct sbi_irqchip_device *chip,
				   u32 hwirq)
{
	반환 !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

정수 sbi_irqchip_unmask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	만약 (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		반환 SBI_EALREADY;

	만약 (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	반환 0;
}

정수 sbi_irqchip_mask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	만약 (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		반환 SBI_EALREADY;

	만약 (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	반환 0;
}

static struct sbi_irqchip_handler *sbi_irqchip_find_handler(struct sbi_irqchip_device *chip,
							    u32 hwirq)
{
	struct sbi_irqchip_handler *h;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		만약 (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			반환 h;
	}

	반환 NULL;
}

정수 sbi_irqchip_set_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	struct sbi_irqchip_hwirq_data *data;

	만약 (!chip || chip->num_hwirq <= hwirq)
	     반환 SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	반환 0;
}

void *sbi_irqchip_get_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;
	만약 (!chip || chip->num_hwirq <= hwirq)
	    반환 NULL;

	data = &chip->hwirqs[hwirq];

	반환 data->priv;
}

정수 sbi_irqchip_raw_handler_default(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_handler *h;
	정수 rc = SBI_OK;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	만약 (!h)
		반환 SBI_EINVAL;

	만약 (h->callback)
		rc = h->callback(hwirq, h->priv);

	만약 (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	반환 rc;
}

정수 sbi_irqchip_set_raw_handler(struct sbi_irqchip_device *chip, u32 hwirq,
				정수 (*raw_hndl)(struct sbi_irqchip_device *, u32))
{
	struct sbi_irqchip_hwirq_data *data;

	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	반환 0;
}

정수 sbi_irqchip_write_msi(struct sbi_irqchip_device *chip, u32 hwirq,
			  const struct sbi_irqchip_msi_msg *msg)
{
	struct sbi_irqchip_handler *h;

	만약 (!chip || chip->num_hwirq <= hwirq || !msg)
		반환 SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	만약 (!h)
		반환 SBI_EFAIL;
	만약 (!h->write_msi)
		반환 SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	반환 0;
}

정수 sbi_irqchip_get_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	만약 (!chip || chip->num_hwirq <= hwirq)
		반환 SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	만약 (!sbi_irqchip_find_handler(chip, hwirq))
		반환 SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	반환 0;
}

정수 sbi_irqchip_set_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	struct sbi_irqchip_hwirq_data *data;
	정수 rc;

	만약 (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		반환 SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	만약 (!sbi_irqchip_find_handler(chip, hwirq))
		반환 SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	만약 (data->hart_index != hart_index) {
		만약 (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			만약 (rc)
				반환 rc;
		}
		data->hart_index = hart_index;
	}

	반환 0;
}

static 정수 __sbi_irqchip_handler_set_affinity(struct sbi_irqchip_device *chip,
					      struct sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	정수 rc;

	위해 (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		만약 (rc)
			반환 rc;

		만약 (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		만약 (rc)
			반환 rc;
	}

	반환 0;
}

static 정수 __sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const struct sbi_irqchip_msi_msg *msg,
							    void *priv),
					  정수 (*callback)(u32 hwirq, void *priv), void *priv)
{
	struct sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	정수 rc;

	위해 (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		만약 (h)
			반환 SBI_EALREADY;
	}

	h = sbi_zalloc(sizeof(*h));
	만약 (!h)
		반환 SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		만약 (h->first_hwirq < th->first_hwirq) {
			nh = th;
			break;
		}
	}
	만약 (nh)
		sbi_list_add(&h->node, &nh->node);
	아니면
		sbi_list_add_tail(&h->node, &chip->handler_list);

	만약 (chip->hwirq_setup) {
		위해 (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			만약 (rc) {
				만약 (chip->hwirq_cleanup) {
					위해 (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				반환 rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	만약 (rc) {
		만약 (chip->hwirq_cleanup) {
			위해 (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		반환 rc;
	}

	만약 (chip->hwirq_unmask) {
		위해 (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	반환 0;
}

정수 sbi_irqchip_register_msi(struct sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const struct sbi_irqchip_msi_msg *msg,
					       void *priv),
			     정수 (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	struct sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	만약 (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		반환 SBI_EINVAL;
	만약 (chip->num_hwirq < num_hwirq)
		반환 SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		만약 (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} 아니면 만약 (hwirq < h->first_hwirq) {
			만약 (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				break;
			} 아니면 {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	만약 (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	만약 (!found)
		반환 SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	반환 __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

정수 sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 정수 (*callback)(u32 hwirq, void *priv), void *priv)
{
	만약 (!chip || !num_hwirq || !callback)
		반환 SBI_EINVAL;
	만약 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		반환 SBI_EBAD_RANGE;

	반환 __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

정수 sbi_irqchip_register_reserved(struct sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	만약 (!chip || !num_hwirq)
		반환 SBI_EINVAL;
	만약 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		반환 SBI_EBAD_RANGE;

	반환 __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

정수 sbi_irqchip_unregister_handler(struct sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	struct sbi_irqchip_handler *fh, *lh;
	u32 i;

	만약 (!chip || !num_hwirq)
		반환 SBI_EINVAL;
	만약 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		반환 SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	만약 (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		반환 SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	만약 (!lh || lh != fh)
		반환 SBI_ENODEV;

	만약 (chip->hwirq_mask) {
		위해 (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	만약 (chip->hwirq_cleanup) {
		위해 (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	반환 0;
}

struct sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   struct sbi_irqchip_device *first)
{
	struct sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		만약 (!found) {
			만약 (first == chip)
				found = true;
			continue;
		}
		만약 ((chip->caps & caps) == caps)
			반환 chip;
	}

	반환 NULL;
}

struct sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	struct sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		만약 (chip->id == id)
			반환 chip;
	}

	반환 NULL;
}

정수 sbi_irqchip_add_device(struct sbi_irqchip_device *chip)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_scratch *scratch;
	u32 i, h;

	만약 (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		반환 SBI_EINVAL;

	만약 (sbi_irqchip_find_device(chip->id))
		반환 SBI_EALREADY;

	만약 (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			만약 (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			만약 (hd->chip && hd->chip != chip)
				반환 SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(sizeof(*chip->hwirqs) * chip->num_hwirq);
	만약 (!chip->hwirqs)
		반환 SBI_ENOMEM;
	위해 (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	반환 0;
}

정수 sbi_irqchip_init(struct sbi_scratch *scratch, bool cold_boot)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	정수 rc;

	만약 (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(sizeof(struct sbi_irqchip_hart_data));
		만약 (!irqchip_hart_data_off)
			반환 SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		만약 (rc)
			반환 rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		만약 (!chip->warm_init)
			continue;
		만약 (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		만약 (rc)
			반환 rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	만약 (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	반환 0;
}

void sbi_irqchip_exit(struct sbi_scratch *scratch)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	struct sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	정수 rc;

	sbi_for_each_hartindex(i) {
		만약 (i == current_hartindex())
			continue;
		만약 (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		break;
	}

	만약 (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			만약 (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	만약 (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
