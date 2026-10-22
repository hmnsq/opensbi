/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

ಚಞ್ಚಲ unsigned long sbi_ubsan_report_ಎಣಿಕೆ;

ಸಂರಚನಾ CSourceLocation {
	ವರ್ಣ *mFilename;
	uint32_t m_ಸಾಲು;
	uint32_t mColumn;
};

ಸಂರಚನಾ CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ಸಂರಚನಾ COverflowData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

ಸಂರಚನಾ CUnreachableData {
	ಸಂರಚನಾ CSourceLocation mLocation;
};

ಸಂರಚನಾ CCFICheckFailData {
	uint8_t mCheckKind;
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

ಸಂರಚನಾ CDynamicTypeCacheMissData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
	ಶೂನ್ಯ *mTypeInfo;
	uint8_t mTypeCheckKind;
};

ಸಂರಚನಾ CFunctionTypeMismatchData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

ಸಂರಚನಾ CInvalidBuiltinData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	uint8_t mKind;
};

ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

ಸಂರಚನಾ CNonNullArgData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CSourceLocation mAttributeLocation;
	ಪೂರ್ಣಾಙ್ಕ mArgIndex;
};

ಸಂರಚನಾ CNonNullReturnData {
	ಸಂರಚನಾ CSourceLocation mAttributeLocation;
};

ಸಂರಚನಾ COutOfBoundsData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mArrayType;
	ಸಂರಚನಾ CTypeDescriptor *mIndexType;
};

ಸಂರಚನಾ CPointerOverflowData {
	ಸಂರಚನಾ CSourceLocation mLocation;
};

ಸಂರಚನಾ CShiftOutOfBoundsData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mLHSType;
	ಸಂರಚನಾ CTypeDescriptor *mRHSType;
};

ಸಂರಚನಾ CTypeMismatchData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

ಸಂರಚನಾ CTypeMismatchData_v1 {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

ಸಂರಚನಾ CVLABoundData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

ಸಂರಚನಾ CFloatCastOverflowData {
	ಸಂರಚನಾ CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ಸಂರಚನಾ CTypeDescriptor *mFromType;
	ಸಂರಚನಾ CTypeDescriptor *mToType;
};

ಸಂರಚನಾ CImplicitConversionData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CTypeDescriptor *mFromType;
	ಸಂರಚನಾ CTypeDescriptor *mToType;
	uint8_t mKind;
};

ಸಂರಚನಾ CAlignmentAssumptionData {
	ಸಂರಚನಾ CSourceLocation mLocation;
	ಸಂರಚನಾ CSourceLocation mAssumptionLocation;
	ಸಂರಚನಾ CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
ಶೂನ್ಯ __ubsan_handle_add_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_add_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_alignment_assumption(ಸಂರಚನಾ CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
ಶೂನ್ಯ __ubsan_handle_alignment_assumption_abort(
	ಸಂರಚನಾ CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
ಶೂನ್ಯ __ubsan_handle_builtin_unreachable(ಸಂರಚನಾ CUnreachableData *pData);
ಶೂನ್ಯ __ubsan_handle_divrem_overflow(ಸಂರಚನಾ COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_divrem_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_abort(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_v1(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_v1_abort(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ಶೂನ್ಯ __ubsan_handle_invalid_builtin(ಸಂರಚನಾ CInvalidBuiltinData *pData);
ಶೂನ್ಯ __ubsan_handle_invalid_builtin_abort(ಸಂರಚನಾ CInvalidBuiltinData *pData);
ಶೂನ್ಯ __ubsan_handle_load_invalid_value(ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
				       unsigned long ulVal);
ಶೂನ್ಯ __ubsan_handle_load_invalid_value_abort(ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
					     unsigned long ulVal);
ಶೂನ್ಯ __ubsan_handle_missing_return(ಸಂರಚನಾ CUnreachableData *pData);
ಶೂನ್ಯ __ubsan_handle_mul_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_mul_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_negate_overflow(ಸಂರಚನಾ COverflowData *pData,
				    unsigned long ulOldVal);
ಶೂನ್ಯ __ubsan_handle_negate_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
					  unsigned long ulOldVal);
ಶೂನ್ಯ __ubsan_handle_nullability_arg(ಸಂರಚನಾ CNonNullArgData *pData);
ಶೂನ್ಯ __ubsan_handle_nullability_arg_abort(ಸಂರಚನಾ CNonNullArgData *pData);
ಶೂನ್ಯ __ubsan_handle_nullability_return_v1(
	ಸಂರಚನಾ CNonNullReturnData *pData,
	ಸಂರಚನಾ CSourceLocation *pLocationPointer);
ಶೂನ್ಯ __ubsan_handle_nullability_return_v1_abort(
	ಸಂರಚನಾ CNonNullReturnData *pData,
	ಸಂರಚನಾ CSourceLocation *pLocationPointer);
ಶೂನ್ಯ __ubsan_handle_out_of_bounds(ಸಂರಚನಾ COutOfBoundsData *pData,
				  unsigned long ulIndex);
ಶೂನ್ಯ __ubsan_handle_out_of_bounds_abort(ಸಂರಚನಾ COutOfBoundsData *pData,
					unsigned long ulIndex);
ಶೂನ್ಯ __ubsan_handle_pointer_overflow(ಸಂರಚನಾ CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
ಶೂನ್ಯ __ubsan_handle_pointer_overflow_abort(ಸಂರಚನಾ CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds(ಸಂರಚನಾ CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds_abort(
	ಸಂರಚನಾ CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_sub_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_sub_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ಶೂನ್ಯ __ubsan_handle_type_mismatch(ಸಂರಚನಾ CTypeMismatchData *pData,
				  unsigned long ulPointer);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_abort(ಸಂರಚನಾ CTypeMismatchData *pData,
					unsigned long ulPointer);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1(ಸಂರಚನಾ CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1_abort(ಸಂರಚನಾ CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive(ಸಂರಚನಾ CVLABoundData *pData,
					   unsigned long ulBound);
ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive_abort(ಸಂರಚನಾ CVLABoundData *pData,
						 unsigned long ulBound);
ಶೂನ್ಯ __ubsan_get_current_report_data(ಧ್ರುವ ವರ್ಣ **ppOutIssueKind,
				     ಧ್ರುವ ವರ್ಣ **ppOutMessage,
				     ಧ್ರುವ ವರ್ಣ **ppOutFilename,
				     uint32_t *p_Out_ಸಾಲು, uint32_t *pOutCol,
				     ವರ್ಣ **ppOutMemoryAddr);
ಸ್ಥಿರ ಶೂನ್ಯ HandleOverflow(bool isFatal, ಸಂರಚನಾ COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ಧ್ರುವ ವರ್ಣ *szOperation);
ಸ್ಥಿರ ಶೂನ್ಯ HandleNegateOverflow(bool isFatal, ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ul_Old_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleBuiltinUnreachable(bool isFatal,
				     ಸಂರಚನಾ CUnreachableData *pData);
ಸ್ಥಿರ ಶೂನ್ಯ HandleTypeMismatch(bool isFatal, ಸಂರಚನಾ CSourceLocation *mLocation,
			       ಸಂರಚನಾ CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
ಸ್ಥಿರ ಶೂನ್ಯ HandleVlaBoundNotPositive(bool isFatal, ಸಂರಚನಾ CVLABoundData *pData,
				      unsigned long ulBound);
ಸ್ಥಿರ ಶೂನ್ಯ HandleOutOfBounds(bool isFatal, ಸಂರಚನಾ COutOfBoundsData *pData,
			      unsigned long ulIndex);
ಸ್ಥಿರ ಶೂನ್ಯ HandleShiftOutOfBounds(bool isFatal,
				   ಸಂರಚನಾ CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
ಸ್ಥಿರ ಶೂನ್ಯ Handle_Load_Invalid_ಮೌಲ್ಯ(bool isFatal,
				   ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
				   unsigned long ul_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ HandleInvalidBuiltin(bool isFatal,
				 ಸಂರಚನಾ CInvalidBuiltinData *pData);
ಸ್ಥಿರ ಶೂನ್ಯ HandleFunctionTypeMismatch(bool isFatal,
				       ಸಂರಚನಾ CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
ಸ್ಥಿರ ಶೂನ್ಯ HandleMissingReturn(bool isFatal, ಸಂರಚನಾ CUnreachableData *pData);
ಸ್ಥಿರ ಶೂನ್ಯ HandlePointerOverflow(bool isFatal,
				  ಸಂರಚನಾ CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
ಸ್ಥಿರ ಶೂನ್ಯ HandleAlignmentAssumption(bool isFatal,
				      ಸಂರಚನಾ CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define ಸಂಖ್ಯೆ_SIGNED_BIT 1
#define ಸಂಖ್ಯೆ_MAXLEN 128
#define __arraycount(__a) (ಪರಿಮಾಣ(__a) / ಪರಿಮಾಣ(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

ಸ್ಥಿರ bool isAlreadyReported(ಸಂರಚನಾ CSourceLocation *pLocation)
{
	uint32_t si_Old_ಮೌಲ್ಯ;
	ಚಞ್ಚಲ uint32_t *p_ಸಾಲು;

	ಯದಿ (!pLocation)
		ಪ್ರತ್ಯಾಗಮ false;

	p_ಸಾಲು = &pLocation->m_ಸಾಲು;

	ಕುರು {
		si_Old_ಮೌಲ್ಯ = *p_ಸಾಲು;
	} ಯಾವತ್ (__sync_val_compare_and_swap(p_ಸಾಲು, si_Old_ಮೌಲ್ಯ,
					     si_Old_ಮೌಲ್ಯ | ACK_REPORTED) !=
		 si_Old_ಮೌಲ್ಯ);

	ಯದಿ (!(si_Old_ಮೌಲ್ಯ & ACK_REPORTED)) {

		sbi_ubsan_report_ಎಣಿಕೆ++;

		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleOverflow(bool isFatal, ಸಂರಚನಾ COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ಧ್ರುವ ವರ್ಣ *szOperation)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ಯದಿ (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} ಅನ್ಯಥಾ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleNegateOverflow(bool isFatal, ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ul_Old_ಮೌಲ್ಯ)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	ಯದಿ (is_signed) {
		sbi_printf("%ld", (long)ul_Old_ಮೌಲ್ಯ);
	} ಅನ್ಯಥಾ {
		sbi_printf("%lu", ul_Old_ಮೌಲ್ಯ);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleBuiltinUnreachable(bool isFatal,
				     ಸಂರಚನಾ CUnreachableData *pData)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ಧ್ರುವ ವರ್ಣ *rgczTypeCheckKinds[] = { "load of",
				     "store to",
				     "reference binding to",
				     "member access within",
				     "member call on",
				     "constructor call on",
				     "downcast of",
				     "downcast of",
				     "upcast of",
				     "cast to virtual base of",
				     "_Nonnull binding to",
				     "dynamic operation on" };

ಸ್ಥಿರ ಶೂನ್ಯ HandleTypeMismatch(bool isFatal, ಸಂರಚನಾ CSourceLocation *mLocation,
			       ಸಂರಚನಾ CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	ಯದಿ ((!mLocation) || (!mType)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_ಸಾಲು & ~ACK_REPORTED,
		   mLocation->mColumn);

	ಧ್ರುವ ವರ್ಣ *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	ಯದಿ (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} ಅನ್ಯಥಾ ಯದಿ (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (ಶೂನ್ಯ *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} ಅನ್ಯಥಾ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (ಶೂನ್ಯ *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleVlaBoundNotPositive(bool isFatal, ಸಂರಚನಾ CVLABoundData *pData,
				      unsigned long ulBound)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT;

	ಯದಿ (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} ಅನ್ಯಥಾ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleOutOfBounds(bool isFatal, ಸಂರಚನಾ COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT;

	ಯದಿ (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} ಅನ್ಯಥಾ {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ bool is_Negative_ಸಂಖ್ಯೆ(ಸಂರಚನಾ CTypeDescriptor *pType, unsigned long ulVal)
{
	ಯದಿ (!(pType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT)) {
		ಪ್ರತ್ಯಾಗಮ false;
	}

	ಪ್ರತ್ಯಾಗಮ (long)ulVal < 0;
}

ಸ್ಥಿರ size_t type_width(ಸಂರಚನಾ CTypeDescriptor *pType)
{
	ಪ್ರತ್ಯಾಗಮ 1UL << (pType->mTypeInfo >> 1);
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleShiftOutOfBounds(bool isFatal,
				   ಸಂರಚನಾ CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ಯದಿ (is_Negative_ಸಂಖ್ಯೆ(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} ಅನ್ಯಥಾ ಯದಿ (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} ಅನ್ಯಥಾ ಯದಿ (is_Negative_ಸಂಖ್ಯೆ(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} ಅನ್ಯಥಾ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ Handle_Load_Invalid_ಮೌಲ್ಯ(bool isFatal,
				   ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
				   unsigned long ul_ಮೌಲ್ಯ)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ಸಂಖ್ಯೆ_SIGNED_BIT;

	sbi_printf("load of value ");

	ಯದಿ (is_signed) {
		sbi_printf("%ld ", (long)ul_ಮೌಲ್ಯ);
	} ಅನ್ಯಥಾ {
		sbi_printf("%lu ", ul_ಮೌಲ್ಯ);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ಧ್ರುವ ವರ್ಣ *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ಸ್ಥಿರ ಶೂನ್ಯ HandleInvalidBuiltin(bool isFatal,
				 ಸಂರಚನಾ CInvalidBuiltinData *pData)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ಧ್ರುವ ವರ್ಣ *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleFunctionTypeMismatch(bool isFatal,
				       ಸಂರಚನಾ CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleMissingReturn(bool isFatal, ಸಂರಚನಾ CUnreachableData *pData)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandlePointerOverflow(bool isFatal,
				  ಸಂರಚನಾ CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

ಸ್ಥಿರ ಶೂನ್ಯ HandleAlignmentAssumption(bool isFatal,
				      ಸಂರಚನಾ CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	ಯದಿ (!pData) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (isAlreadyReported(&pData->mLocation)) {
		ಪ್ರತ್ಯಾಗಮ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ಸಾಲು & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (ಶೂನ್ಯ *)ulRealPointer, (ಶೂನ್ಯ *)ulOffset);

	ಯದಿ (pData->mAssumptionLocation.mFilename != ರಿಕ್ತ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_ಸಾಲು,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
ಶೂನ್ಯ __ubsan_handle_add_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

ಶೂನ್ಯ __ubsan_handle_add_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

ಶೂನ್ಯ __ubsan_handle_sub_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

ಶೂನ್ಯ __ubsan_handle_sub_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

ಶೂನ್ಯ __ubsan_handle_mul_overflow(ಸಂರಚನಾ COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

ಶೂನ್ಯ __ubsan_handle_mul_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

ಶೂನ್ಯ __ubsan_handle_negate_overflow(ಸಂರಚನಾ COverflowData *pData,
				    unsigned long ul_Old_ಮೌಲ್ಯ)
{
	HandleNegateOverflow(false, pData, ul_Old_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_negate_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
					  unsigned long ul_Old_ಮೌಲ್ಯ)
{
	HandleNegateOverflow(true, pData, ul_Old_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_divrem_overflow(ಸಂರಚನಾ COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

ಶೂನ್ಯ __ubsan_handle_divrem_overflow_abort(ಸಂರಚನಾ COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1(ಸಂರಚನಾ CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ಶೂನ್ಯ __ubsan_handle_type_mismatch_v1_abort(ಸಂರಚನಾ CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ಶೂನ್ಯ __ubsan_handle_out_of_bounds(ಸಂರಚನಾ COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

ಶೂನ್ಯ __ubsan_handle_out_of_bounds_abort(ಸಂರಚನಾ COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds(ಸಂರಚನಾ CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

ಶೂನ್ಯ __ubsan_handle_shift_out_of_bounds_abort(
	ಸಂರಚನಾ CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

ಶೂನ್ಯ __ubsan_handle_pointer_overflow(ಸಂರಚನಾ CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

ಶೂನ್ಯ __ubsan_handle_pointer_overflow_abort(ಸಂರಚನಾ CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

ಶೂನ್ಯ __ubsan_handle_alignment_assumption(ಸಂರಚನಾ CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ಶೂನ್ಯ __ubsan_handle_alignment_assumption_abort(
	ಸಂರಚನಾ CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ಶೂನ್ಯ __ubsan_handle_builtin_unreachable(ಸಂರಚನಾ CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

ಶೂನ್ಯ __ubsan_handle_invalid_builtin(ಸಂರಚನಾ CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ಶೂನ್ಯ __ubsan_handle_invalid_builtin_abort(ಸಂರಚನಾ CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ಶೂನ್ಯ __ubsan_handle_load_invalid_value(ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
				       unsigned long ul_ಮೌಲ್ಯ)
{
	Handle_Load_Invalid_ಮೌಲ್ಯ(false, pData, ul_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_load_invalid_value_abort(ಸಂರಚನಾ CInvalid_ಮೌಲ್ಯ_Data *pData,
					     unsigned long ul_ಮೌಲ್ಯ)
{
	Handle_Load_Invalid_ಮೌಲ್ಯ(true, pData, ul_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ __ubsan_handle_missing_return(ಸಂರಚನಾ CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive(ಸಂರಚನಾ CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

ಶೂನ್ಯ __ubsan_handle_vla_bound_not_positive_abort(ಸಂರಚನಾ CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

ಶೂನ್ಯ __ubsan_handle_function_type_mismatch(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

ಶೂನ್ಯ __ubsan_handle_function_type_mismatch_abort(
	ಸಂರಚನಾ CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
