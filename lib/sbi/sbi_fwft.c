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
	จำนวนเต็ม (*supported)(sa_nracanaa fwft_config *conf);
	จำนวนเต็ม (*set)(sa_nracanaa fwft_config *conf, unsigned long value);
	จำนวนเต็ม (*get)(sa_nracanaa fwft_config *conf, unsigned long *value);
};

sa_nracanaa fwft_config {
	const sa_nracanaa fwft_feature *feature;
	unsigned long flags;
};

sa_nracanaa fwft_hart_state {
	unsigned จำนวนเต็ม config_count;
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
	จำนวนเต็ม i;

	สำหรับ (i = 0; i < array_size(fwft_defined_features); i++) {
		ถ้า (fwft_defined_features[i] == feature)
			คืน true;
	}

	คืน false;
}

static จำนวนเต็ม fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	ถ้า (value == 1) {
		ถ้า (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		มิฉะนั้น
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} มิฉะนั้น ถ้า (value == 0) {
		ถ้า (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		มิฉะนั้น
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} มิฉะนั้น {
		คืน SBI_EINVAL;
	}

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	ถ้า (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	มิฉะนั้น
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_misaligned_delegation_supported(sa_nracanaa fwft_config *conf)
{
	ถ้า (!misa_extension('S'))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_set_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long value)
{
	ถ้า (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	มิฉะนั้น ถ้า (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	มิฉะนั้น
		คืน SBI_EINVAL;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_get_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_double_trap_supported(sa_nracanaa fwft_config *conf)
{
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_set_double_trap(sa_nracanaa fwft_config *conf, unsigned long value)
{
	คืน fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static จำนวนเต็ม fwft_get_double_trap(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	คืน fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static จำนวนเต็ม fwft_adue_supported(sa_nracanaa fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_set_adue(sa_nracanaa fwft_config *conf, unsigned long value)
{
	จำนวนเต็ม res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	ถ้า (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ถ้า (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	คืน res;
}

static จำนวนเต็ม fwft_get_adue(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	คืน fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static จำนวนเต็ม fwft_lpad_supported(sa_nracanaa fwft_config *conf)
{
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_enable_lpad(sa_nracanaa fwft_config *conf, unsigned long value)
{
	คืน fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static จำนวนเต็ม fwft_get_lpad(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	คืน fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static จำนวนเต็ม fwft_sstack_supported(sa_nracanaa fwft_config *conf)
{
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_enable_sstack(sa_nracanaa fwft_config *conf, unsigned long value)
{
	คืน fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static จำนวนเต็ม fwft_get_sstack(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	คืน fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#ถ้า __riscv_xlen > 32
static จำนวนเต็ม fwft_pmlen_supported(sa_nracanaa fwft_config *conf)
{
	ถ้า (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		คืน SBI_ENOTSUPP;

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_set_pmlen(sa_nracanaa fwft_config *conf, unsigned long value)
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
		คืน SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ถ้า ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		คืน SBI_EINVAL;
	}

	คืน SBI_OK;
}

static จำนวนเต็ม fwft_get_pmlen(sa_nracanaa fwft_config *conf, unsigned long *value)
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
		คืน SBI_EFAIL;
	}

	คืน SBI_OK;
}
#endif

static sa_nracanaa fwft_config* get_feature_config(ga_nanaa sbi_fwft_feature_t feature)
{
	จำนวนเต็ม i;
	sa_nracanaa fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ถ้า (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		คืน NULL;

	สำหรับ (i = 0; i < fhs->config_count; i++){
		ถ้า (feature == fhs->configs[i].feature->id)
			คืน &fhs->configs[i];
	}

	คืน NULL;
}

static จำนวนเต็ม fwft_get_feature(ga_nanaa sbi_fwft_feature_t feature,
			    sa_nracanaa fwft_config **conf)
{
	จำนวนเต็ม ret;
	sa_nracanaa fwft_config *tconf;

	tconf = get_feature_config(feature);
	ถ้า (!tconf) {
		ถ้า (fwft_is_defined_feature(feature))
			คืน SBI_ENOTSUPP;

		คืน SBI_EDENIED;
	}

	ถ้า (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ถ้า (ret)
			คืน ret;
	}
	*conf = tconf;

	คืน SBI_SUCCESS;
}

static void fwft_clear_config_lock(ga_nanaa sbi_fwft_feature_t feature)
{
	จำนวนเต็ม ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ถ้า (ret)
		คืน;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

จำนวนเต็ม sbi_fwft_set(ga_nanaa sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	จำนวนเต็ม ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ถ้า (ret)
		คืน ret;

	ถ้า ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		คืน SBI_EINVAL;

	ถ้า (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		คืน SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	ถ้า (ret)
		คืน ret;

	conf->flags = flags;

	คืน SBI_OK;
}

จำนวนเต็ม sbi_fwft_get(ga_nanaa sbi_fwft_feature_t feature, unsigned long *out_val)
{
	จำนวนเต็ม ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ถ้า (ret)
		คืน ret;

	คืน conf->feature->get(conf, out_val);
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
#ถ้า __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

จำนวนเต็ม sbi_fwft_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	จำนวนเต็ม i;
	sa_nracanaa fwft_hart_state *fhs;

	ถ้า (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ถ้า (!fwft_ptr_offset)
			คืน SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ถ้า (!fhs) {
		fhs = sbi_zalloc(parimaa_na(*fhs) + array_size(features) * parimaa_na(sa_nracanaa fwft_config));
		ถ้า (!fhs)
			คืน SBI_ENOMEM;

		fhs->config_count = array_size(features);
		สำหรับ (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	สำหรับ (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	คืน 0;
}
