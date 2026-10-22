/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_bitmap.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hfence.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_tlb.h>
#include <sbi/sbi_types.h>

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
static unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), void *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

struct fwft_config;

struct fwft_feature {
	enum sbi_fwft_feature_t id;
	số_nguyên (*supported)(struct fwft_config *conf);
	số_nguyên (*set)(struct fwft_config *conf, unsigned long value);
	số_nguyên (*get)(struct fwft_config *conf, unsigned long *value);
};

struct fwft_config {
	const struct fwft_feature *feature;
	unsigned long flags;
};

struct fwft_hart_state {
	unsigned số_nguyên config_count;
	struct fwft_config configs[];
};

static const unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_STACK,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

static bool fwft_is_defined_feature(enum sbi_fwft_feature_t feature)
{
	số_nguyên i;

	cho (i = 0; i < array_size(fwft_defined_features); i++) {
		nếu (fwft_defined_features[i] == feature)
			trả_về true;
	}

	trả_về false;
}

static số_nguyên fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	nếu (value == 1) {
		nếu (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		không_thì
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} không_thì nếu (value == 0) {
		nếu (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		không_thì
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} không_thì {
		trả_về SBI_EINVAL;
	}

	trả_về SBI_OK;
}

static số_nguyên fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	nếu (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	không_thì
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	trả_về SBI_OK;
}

static số_nguyên fwft_misaligned_delegation_supported(struct fwft_config *conf)
{
	nếu (!misa_extension('S'))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_set_misaligned_delegation(struct fwft_config *conf,
					 unsigned long value)
{
	nếu (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	không_thì nếu (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	không_thì
		trả_về SBI_EINVAL;

	trả_về SBI_OK;
}

static số_nguyên fwft_get_misaligned_delegation(struct fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	trả_về SBI_OK;
}

static số_nguyên fwft_double_trap_supported(struct fwft_config *conf)
{
	nếu (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_set_double_trap(struct fwft_config *conf, unsigned long value)
{
	trả_về fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static số_nguyên fwft_get_double_trap(struct fwft_config *conf, unsigned long *value)
{
	trả_về fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static số_nguyên fwft_adue_supported(struct fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	nếu (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_set_adue(struct fwft_config *conf, unsigned long value)
{
	số_nguyên res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	nếu (res == SBI_OK) {
		__sbi_sfence_vma_all();

		nếu (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	trả_về res;
}

static số_nguyên fwft_get_adue(struct fwft_config *conf, unsigned long *value)
{
	trả_về fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static số_nguyên fwft_lpad_supported(struct fwft_config *conf)
{
	nếu (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_enable_lpad(struct fwft_config *conf, unsigned long value)
{
	trả_về fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static số_nguyên fwft_get_lpad(struct fwft_config *conf, unsigned long *value)
{
	trả_về fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static số_nguyên fwft_sstack_supported(struct fwft_config *conf)
{
	nếu (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_enable_sstack(struct fwft_config *conf, unsigned long value)
{
	trả_về fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static số_nguyên fwft_get_sstack(struct fwft_config *conf, unsigned long *value)
{
	trả_về fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#nếu __riscv_xlen > 32
static số_nguyên fwft_pmlen_supported(struct fwft_config *conf)
{
	nếu (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		trả_về SBI_ENOTSUPP;

	trả_về SBI_OK;
}

static số_nguyên fwft_set_pmlen(struct fwft_config *conf, unsigned long value)
{
	unsigned long pmm, prev;

	switch (value) {
	case 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		break;
	case 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		break;
	case 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		break;
	default:
		trả_về SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	nếu ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		trả_về SBI_EINVAL;
	}

	trả_về SBI_OK;
}

static số_nguyên fwft_get_pmlen(struct fwft_config *conf, unsigned long *value)
{
	switch (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	case ENVCFG_PMM_PMLEN_0:
		*value = 0;
		break;
	case ENVCFG_PMM_PMLEN_7:
		*value = 7;
		break;
	case ENVCFG_PMM_PMLEN_16:
		*value = 16;
		break;
	default:
		trả_về SBI_EFAIL;
	}

	trả_về SBI_OK;
}
#endif

static struct fwft_config* get_feature_config(enum sbi_fwft_feature_t feature)
{
	số_nguyên i;
	struct fwft_hart_state *fhs = fwft_thishart_state_ptr();

	nếu (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		trả_về NULL;

	cho (i = 0; i < fhs->config_count; i++){
		nếu (feature == fhs->configs[i].feature->id)
			trả_về &fhs->configs[i];
	}

	trả_về NULL;
}

static số_nguyên fwft_get_feature(enum sbi_fwft_feature_t feature,
			    struct fwft_config **conf)
{
	số_nguyên ret;
	struct fwft_config *tconf;

	tconf = get_feature_config(feature);
	nếu (!tconf) {
		nếu (fwft_is_defined_feature(feature))
			trả_về SBI_ENOTSUPP;

		trả_về SBI_EDENIED;
	}

	nếu (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		nếu (ret)
			trả_về ret;
	}
	*conf = tconf;

	trả_về SBI_SUCCESS;
}

static void fwft_clear_config_lock(enum sbi_fwft_feature_t feature)
{
	số_nguyên ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	nếu (ret)
		trả_về;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

số_nguyên sbi_fwft_set(enum sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	số_nguyên ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	nếu (ret)
		trả_về ret;

	nếu ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		trả_về SBI_EINVAL;

	nếu (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		trả_về SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	nếu (ret)
		trả_về ret;

	conf->flags = flags;

	trả_về SBI_OK;
}

số_nguyên sbi_fwft_get(enum sbi_fwft_feature_t feature, unsigned long *out_val)
{
	số_nguyên ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	nếu (ret)
		trả_về ret;

	trả_về conf->feature->get(conf, out_val);
}

static const struct fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.set = fwft_set_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.set = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_STACK,
		.supported = fwft_sstack_supported,
		.set = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.set = fwft_set_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.set = fwft_set_adue,
		.get = fwft_get_adue,
	},
#nếu __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

số_nguyên sbi_fwft_init(struct sbi_scratch *scratch, bool cold_boot)
{
	số_nguyên i;
	struct fwft_hart_state *fhs;

	nếu (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		nếu (!fwft_ptr_offset)
			trả_về SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	nếu (!fhs) {
		fhs = sbi_zalloc(sizeof(*fhs) + array_size(features) * sizeof(struct fwft_config));
		nếu (!fhs)
			trả_về SBI_ENOMEM;

		fhs->config_count = array_size(features);
		cho (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	cho (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	trả_về 0;
}
