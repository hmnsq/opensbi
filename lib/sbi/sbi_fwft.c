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
	ຈຳນວນເຕັມ (*supported)(sa_nracanaa fwft_config *conf);
	ຈຳນວນເຕັມ (*set)(sa_nracanaa fwft_config *conf, unsigned long value);
	ຈຳນວນເຕັມ (*get)(sa_nracanaa fwft_config *conf, unsigned long *value);
};

sa_nracanaa fwft_config {
	const sa_nracanaa fwft_feature *feature;
	unsigned long flags;
};

sa_nracanaa fwft_hart_state {
	unsigned ຈຳນວນເຕັມ config_count;
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
	ຈຳນວນເຕັມ i;

	ສຳລັບ (i = 0; i < array_size(fwft_defined_features); i++) {
		ຖ້າ (fwft_defined_features[i] == feature)
			ສົ່ງຄືນ true;
	}

	ສົ່ງຄືນ false;
}

static ຈຳນວນເຕັມ fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	ຖ້າ (value == 1) {
		ຖ້າ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ບໍ່ດັ່ງນັ້ນ
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (value == 0) {
		ຖ້າ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ບໍ່ດັ່ງນັ້ນ
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ SBI_EINVAL;
	}

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	ຖ້າ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	ບໍ່ດັ່ງນັ້ນ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_misaligned_delegation_supported(sa_nracanaa fwft_config *conf)
{
	ຖ້າ (!misa_extension('S'))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_set_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long value)
{
	ຖ້າ (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ SBI_EINVAL;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_get_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_double_trap_supported(sa_nracanaa fwft_config *conf)
{
	ຖ້າ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_set_double_trap(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ສົ່ງຄືນ fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_get_double_trap(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ສົ່ງຄືນ fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_adue_supported(sa_nracanaa fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ຖ້າ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_set_adue(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ຈຳນວນເຕັມ res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	ຖ້າ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ຖ້າ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ສົ່ງຄືນ res;
}

static ຈຳນວນເຕັມ fwft_get_adue(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ສົ່ງຄືນ fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_lpad_supported(sa_nracanaa fwft_config *conf)
{
	ຖ້າ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_enable_lpad(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ສົ່ງຄືນ fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_get_lpad(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ສົ່ງຄືນ fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_sstack_supported(sa_nracanaa fwft_config *conf)
{
	ຖ້າ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_enable_sstack(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ສົ່ງຄືນ fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static ຈຳນວນເຕັມ fwft_get_sstack(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ສົ່ງຄືນ fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#ຖ້າ __riscv_xlen > 32
static ຈຳນວນເຕັມ fwft_pmlen_supported(sa_nracanaa fwft_config *conf)
{
	ຖ້າ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ສົ່ງຄືນ SBI_ENOTSUPP;

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_set_pmlen(sa_nracanaa fwft_config *conf, unsigned long value)
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
		ສົ່ງຄືນ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ຖ້າ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ສົ່ງຄືນ SBI_EINVAL;
	}

	ສົ່ງຄືນ SBI_OK;
}

static ຈຳນວນເຕັມ fwft_get_pmlen(sa_nracanaa fwft_config *conf, unsigned long *value)
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
		ສົ່ງຄືນ SBI_EFAIL;
	}

	ສົ່ງຄືນ SBI_OK;
}
#endif

static sa_nracanaa fwft_config* get_feature_config(ga_nanaa sbi_fwft_feature_t feature)
{
	ຈຳນວນເຕັມ i;
	sa_nracanaa fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ຖ້າ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ສົ່ງຄືນ NULL;

	ສຳລັບ (i = 0; i < fhs->config_count; i++){
		ຖ້າ (feature == fhs->configs[i].feature->id)
			ສົ່ງຄືນ &fhs->configs[i];
	}

	ສົ່ງຄືນ NULL;
}

static ຈຳນວນເຕັມ fwft_get_feature(ga_nanaa sbi_fwft_feature_t feature,
			    sa_nracanaa fwft_config **conf)
{
	ຈຳນວນເຕັມ ret;
	sa_nracanaa fwft_config *tconf;

	tconf = get_feature_config(feature);
	ຖ້າ (!tconf) {
		ຖ້າ (fwft_is_defined_feature(feature))
			ສົ່ງຄືນ SBI_ENOTSUPP;

		ສົ່ງຄືນ SBI_EDENIED;
	}

	ຖ້າ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ຖ້າ (ret)
			ສົ່ງຄືນ ret;
	}
	*conf = tconf;

	ສົ່ງຄືນ SBI_SUCCESS;
}

static void fwft_clear_config_lock(ga_nanaa sbi_fwft_feature_t feature)
{
	ຈຳນວນເຕັມ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ຖ້າ (ret)
		ສົ່ງຄືນ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ຈຳນວນເຕັມ sbi_fwft_set(ga_nanaa sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	ຈຳນວນເຕັມ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ຖ້າ (ret)
		ສົ່ງຄືນ ret;

	ຖ້າ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ສົ່ງຄືນ SBI_EINVAL;

	ຖ້າ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ສົ່ງຄືນ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	ຖ້າ (ret)
		ສົ່ງຄືນ ret;

	conf->flags = flags;

	ສົ່ງຄືນ SBI_OK;
}

ຈຳນວນເຕັມ sbi_fwft_get(ga_nanaa sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ຈຳນວນເຕັມ ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ຖ້າ (ret)
		ສົ່ງຄືນ ret;

	ສົ່ງຄືນ conf->feature->get(conf, out_val);
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
#ຖ້າ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ຈຳນວນເຕັມ sbi_fwft_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	ຈຳນວນເຕັມ i;
	sa_nracanaa fwft_hart_state *fhs;

	ຖ້າ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ຖ້າ (!fwft_ptr_offset)
			ສົ່ງຄືນ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ຖ້າ (!fhs) {
		fhs = sbi_zalloc(parimaa_na(*fhs) + array_size(features) * parimaa_na(sa_nracanaa fwft_config));
		ຖ້າ (!fhs)
			ສົ່ງຄືນ SBI_ENOMEM;

		fhs->config_count = array_size(features);
		ສຳລັບ (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	ສຳລັບ (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ສົ່ງຄືນ 0;
}
