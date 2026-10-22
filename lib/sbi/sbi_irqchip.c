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
	ціле (*raw_handler)(struct sbi_irqchip_device *chip, u32 hwirq);

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
	ціле (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

struct sbi_irqchip_hart_data {
	struct sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

ціле sbi_irqchip_process(void)
{
	struct sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	якщо (!hd || !hd->chip || !hd->chip->process_hwirqs)
		повернути SBI_ENODEV;

	повернути hd->chip->process_hwirqs(hd->chip);
}

ціле sbi_irqchip_process_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	якщо (!data->raw_handler)
		повернути SBI_ENOENT;

	повернути data->raw_handler(chip, hwirq);
}

static inline u32 sbi_irqchip_get_irq_state(struct sbi_irqchip_device *chip,
					     u32 hwirq)
{
	якщо (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		повернути 0;

	повернути chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(struct sbi_irqchip_device *chip,
				   u32 hwirq)
{
	повернути !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

ціле sbi_irqchip_unmask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	якщо (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		повернути SBI_EALREADY;

	якщо (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	повернути 0;
}

ціле sbi_irqchip_mask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	якщо (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		повернути SBI_EALREADY;

	якщо (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	повернути 0;
}

static struct sbi_irqchip_handler *sbi_irqchip_find_handler(struct sbi_irqchip_device *chip,
							    u32 hwirq)
{
	struct sbi_irqchip_handler *h;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		якщо (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			повернути h;
	}

	повернути NULL;
}

ціле sbi_irqchip_set_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	struct sbi_irqchip_hwirq_data *data;

	якщо (!chip || chip->num_hwirq <= hwirq)
	     повернути SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	повернути 0;
}

void *sbi_irqchip_get_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;
	якщо (!chip || chip->num_hwirq <= hwirq)
	    повернути NULL;

	data = &chip->hwirqs[hwirq];

	повернути data->priv;
}

ціле sbi_irqchip_raw_handler_default(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_handler *h;
	ціле rc = SBI_OK;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	якщо (!h)
		повернути SBI_EINVAL;

	якщо (h->callback)
		rc = h->callback(hwirq, h->priv);

	якщо (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	повернути rc;
}

ціле sbi_irqchip_set_raw_handler(struct sbi_irqchip_device *chip, u32 hwirq,
				ціле (*raw_hndl)(struct sbi_irqchip_device *, u32))
{
	struct sbi_irqchip_hwirq_data *data;

	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	повернути 0;
}

ціле sbi_irqchip_write_msi(struct sbi_irqchip_device *chip, u32 hwirq,
			  const struct sbi_irqchip_msi_msg *msg)
{
	struct sbi_irqchip_handler *h;

	якщо (!chip || chip->num_hwirq <= hwirq || !msg)
		повернути SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	якщо (!h)
		повернути SBI_EFAIL;
	якщо (!h->write_msi)
		повернути SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	повернути 0;
}

ціле sbi_irqchip_get_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	якщо (!chip || chip->num_hwirq <= hwirq)
		повернути SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	якщо (!sbi_irqchip_find_handler(chip, hwirq))
		повернути SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	повернути 0;
}

ціле sbi_irqchip_set_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	struct sbi_irqchip_hwirq_data *data;
	ціле rc;

	якщо (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		повернути SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	якщо (!sbi_irqchip_find_handler(chip, hwirq))
		повернути SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	якщо (data->hart_index != hart_index) {
		якщо (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			якщо (rc)
				повернути rc;
		}
		data->hart_index = hart_index;
	}

	повернути 0;
}

static ціле __sbi_irqchip_handler_set_affinity(struct sbi_irqchip_device *chip,
					      struct sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	ціле rc;

	для (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		якщо (rc)
			повернути rc;

		якщо (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		якщо (rc)
			повернути rc;
	}

	повернути 0;
}

static ціле __sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const struct sbi_irqchip_msi_msg *msg,
							    void *priv),
					  ціле (*callback)(u32 hwirq, void *priv), void *priv)
{
	struct sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	ціле rc;

	для (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		якщо (h)
			повернути SBI_EALREADY;
	}

	h = sbi_zalloc(sizeof(*h));
	якщо (!h)
		повернути SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		якщо (h->first_hwirq < th->first_hwirq) {
			nh = th;
			break;
		}
	}
	якщо (nh)
		sbi_list_add(&h->node, &nh->node);
	інакше
		sbi_list_add_tail(&h->node, &chip->handler_list);

	якщо (chip->hwirq_setup) {
		для (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			якщо (rc) {
				якщо (chip->hwirq_cleanup) {
					для (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				повернути rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	якщо (rc) {
		якщо (chip->hwirq_cleanup) {
			для (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		повернути rc;
	}

	якщо (chip->hwirq_unmask) {
		для (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	повернути 0;
}

ціле sbi_irqchip_register_msi(struct sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const struct sbi_irqchip_msi_msg *msg,
					       void *priv),
			     ціле (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	struct sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	якщо (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		повернути SBI_EINVAL;
	якщо (chip->num_hwirq < num_hwirq)
		повернути SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		якщо (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} інакше якщо (hwirq < h->first_hwirq) {
			якщо (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				break;
			} інакше {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	якщо (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	якщо (!found)
		повернути SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	повернути __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

ціле sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 ціле (*callback)(u32 hwirq, void *priv), void *priv)
{
	якщо (!chip || !num_hwirq || !callback)
		повернути SBI_EINVAL;
	якщо (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		повернути SBI_EBAD_RANGE;

	повернути __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

ціле sbi_irqchip_register_reserved(struct sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	якщо (!chip || !num_hwirq)
		повернути SBI_EINVAL;
	якщо (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		повернути SBI_EBAD_RANGE;

	повернути __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

ціле sbi_irqchip_unregister_handler(struct sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	struct sbi_irqchip_handler *fh, *lh;
	u32 i;

	якщо (!chip || !num_hwirq)
		повернути SBI_EINVAL;
	якщо (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		повернути SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	якщо (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		повернути SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	якщо (!lh || lh != fh)
		повернути SBI_ENODEV;

	якщо (chip->hwirq_mask) {
		для (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	якщо (chip->hwirq_cleanup) {
		для (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	повернути 0;
}

struct sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   struct sbi_irqchip_device *first)
{
	struct sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		якщо (!found) {
			якщо (first == chip)
				found = true;
			continue;
		}
		якщо ((chip->caps & caps) == caps)
			повернути chip;
	}

	повернути NULL;
}

struct sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	struct sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		якщо (chip->id == id)
			повернути chip;
	}

	повернути NULL;
}

ціле sbi_irqchip_add_device(struct sbi_irqchip_device *chip)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_scratch *scratch;
	u32 i, h;

	якщо (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		повернути SBI_EINVAL;

	якщо (sbi_irqchip_find_device(chip->id))
		повернути SBI_EALREADY;

	якщо (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			якщо (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			якщо (hd->chip && hd->chip != chip)
				повернути SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(sizeof(*chip->hwirqs) * chip->num_hwirq);
	якщо (!chip->hwirqs)
		повернути SBI_ENOMEM;
	для (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	повернути 0;
}

ціле sbi_irqchip_init(struct sbi_scratch *scratch, bool cold_boot)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	ціле rc;

	якщо (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(sizeof(struct sbi_irqchip_hart_data));
		якщо (!irqchip_hart_data_off)
			повернути SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		якщо (rc)
			повернути rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		якщо (!chip->warm_init)
			continue;
		якщо (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		якщо (rc)
			повернути rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	якщо (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	повернути 0;
}

void sbi_irqchip_exit(struct sbi_scratch *scratch)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	struct sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	ціле rc;

	sbi_for_each_hartindex(i) {
		якщо (i == current_hartindex())
			continue;
		якщо (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		break;
	}

	якщо (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			якщо (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	якщо (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
