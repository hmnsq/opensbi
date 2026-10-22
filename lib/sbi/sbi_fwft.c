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

sa_nracanaa fwft_config;

sa_nracanaa fwft_feature {
	ga_nanaa sbi_fwft_feature_t id;
	ចំនួនគត់ (*supported)(sa_nracanaa fwft_config *conf);
	ចំនួនគត់ (*set)(sa_nracanaa fwft_config *conf, unsigned long value);
	ចំនួនគត់ (*get)(sa_nracanaa fwft_config *conf, unsigned long *value);
};

sa_nracanaa fwft_config {
	const sa_nracanaa fwft_feature *feature;
	unsigned long flags;
};

sa_nracanaa fwft_hart_state {
	unsigned ចំនួនគត់ config_count;
	sa_nracanaa fwft_config configs[];
};

static const unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_STACK,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

static bool fwft_is_defined_feature(ga_nanaa sbi_fwft_feature_t feature)
{
	ចំនួនគត់ i;

	សម្រាប់ (i = 0; i < array_size(fwft_defined_features); i++) {
		ប្រសិនបើ (fwft_defined_features[i] == feature)
			ត្រឡប់ true;
	}

	ត្រឡប់ false;
}

static ចំនួនគត់ fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	ប្រសិនបើ (value == 1) {
		ប្រសិនបើ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		បើមិនដូច្នេះ
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} បើមិនដូច្នេះ ប្រសិនបើ (value == 0) {
		ប្រសិនបើ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		បើមិនដូច្នេះ
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} បើមិនដូច្នេះ {
		ត្រឡប់ SBI_EINVAL;
	}

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	ប្រសិនបើ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	បើមិនដូច្នេះ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_misaligned_delegation_supported(sa_nracanaa fwft_config *conf)
{
	ប្រសិនបើ (!misa_extension('S'))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_set_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long value)
{
	ប្រសិនបើ (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	បើមិនដូច្នេះ ប្រសិនបើ (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	បើមិនដូច្នេះ
		ត្រឡប់ SBI_EINVAL;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_get_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_double_trap_supported(sa_nracanaa fwft_config *conf)
{
	ប្រសិនបើ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_set_double_trap(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ត្រឡប់ fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static ចំនួនគត់ fwft_get_double_trap(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ត្រឡប់ fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static ចំនួនគត់ fwft_adue_supported(sa_nracanaa fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ប្រសិនបើ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_set_adue(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ចំនួនគត់ res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	ប្រសិនបើ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ប្រសិនបើ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ត្រឡប់ res;
}

static ចំនួនគត់ fwft_get_adue(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ត្រឡប់ fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static ចំនួនគត់ fwft_lpad_supported(sa_nracanaa fwft_config *conf)
{
	ប្រសិនបើ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_enable_lpad(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ត្រឡប់ fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static ចំនួនគត់ fwft_get_lpad(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ត្រឡប់ fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static ចំនួនគត់ fwft_sstack_supported(sa_nracanaa fwft_config *conf)
{
	ប្រសិនបើ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_enable_sstack(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ត្រឡប់ fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static ចំនួនគត់ fwft_get_sstack(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ត្រឡប់ fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#ប្រសិនបើ __riscv_xlen > 32
static ចំនួនគត់ fwft_pmlen_supported(sa_nracanaa fwft_config *conf)
{
	ប្រសិនបើ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ត្រឡប់ SBI_ENOTSUPP;

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_set_pmlen(sa_nracanaa fwft_config *conf, unsigned long value)
{
	unsigned long pmm, prev;

	switch (value) {
	pak_sa 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		bha_nga;
	pak_sa 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		bha_nga;
	pak_sa 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		bha_nga;
	default:
		ត្រឡប់ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ប្រសិនបើ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ត្រឡប់ SBI_EINVAL;
	}

	ត្រឡប់ SBI_OK;
}

static ចំនួនគត់ fwft_get_pmlen(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	switch (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	pak_sa ENVCFG_PMM_PMLEN_0:
		*value = 0;
		bha_nga;
	pak_sa ENVCFG_PMM_PMLEN_7:
		*value = 7;
		bha_nga;
	pak_sa ENVCFG_PMM_PMLEN_16:
		*value = 16;
		bha_nga;
	default:
		ត្រឡប់ SBI_EFAIL;
	}

	ត្រឡប់ SBI_OK;
}
#endif

static sa_nracanaa fwft_config* get_feature_config(ga_nanaa sbi_fwft_feature_t feature)
{
	ចំនួនគត់ i;
	sa_nracanaa fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ប្រសិនបើ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ត្រឡប់ NULL;

	សម្រាប់ (i = 0; i < fhs->config_count; i++){
		ប្រសិនបើ (feature == fhs->configs[i].feature->id)
			ត្រឡប់ &fhs->configs[i];
	}

	ត្រឡប់ NULL;
}

static ចំនួនគត់ fwft_get_feature(ga_nanaa sbi_fwft_feature_t feature,
			    sa_nracanaa fwft_config **conf)
{
	ចំនួនគត់ ret;
	sa_nracanaa fwft_config *tconf;

	tconf = get_feature_config(feature);
	ប្រសិនបើ (!tconf) {
		ប្រសិនបើ (fwft_is_defined_feature(feature))
			ត្រឡប់ SBI_ENOTSUPP;

		ត្រឡប់ SBI_EDENIED;
	}

	ប្រសិនបើ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ប្រសិនបើ (ret)
			ត្រឡប់ ret;
	}
	*conf = tconf;

	ត្រឡប់ SBI_SUCCESS;
}

static void fwft_clear_config_lock(ga_nanaa sbi_fwft_feature_t feature)
{
	ចំនួនគត់ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ប្រសិនបើ (ret)
		ត្រឡប់;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ចំនួនគត់ sbi_fwft_set(ga_nanaa sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	ចំនួនគត់ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;

	ប្រសិនបើ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ត្រឡប់ SBI_EINVAL;

	ប្រសិនបើ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ត្រឡប់ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;

	conf->flags = flags;

	ត្រឡប់ SBI_OK;
}

ចំនួនគត់ sbi_fwft_get(ga_nanaa sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ចំនួនគត់ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ប្រសិនបើ (ret)
		ត្រឡប់ ret;

	ត្រឡប់ conf->feature->get(conf, out_val);
}

static const sa_nracanaa fwft_feature features[] =
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
#ប្រសិនបើ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ចំនួនគត់ sbi_fwft_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	ចំនួនគត់ i;
	sa_nracanaa fwft_hart_state *fhs;

	ប្រសិនបើ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ប្រសិនបើ (!fwft_ptr_offset)
			ត្រឡប់ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ប្រសិនបើ (!fhs) {
		fhs = sbi_zalloc(parimaa_na(*fhs) + array_size(features) * parimaa_na(sa_nracanaa fwft_config));
		ប្រសិនបើ (!fhs)
			ត្រឡប់ SBI_ENOMEM;

		fhs->config_count = array_size(features);
		សម្រាប់ (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	សម្រាប់ (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ត្រឡប់ 0;
}
