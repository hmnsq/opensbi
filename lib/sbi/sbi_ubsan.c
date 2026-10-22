/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

ଅସ୍ଥିର unsigned long sbi_ubsan_report_ଗଣନା;

ସଂରଚନା CSourceLocation {
	ଅକ୍ଷର *mFilename;
	uint32_t m_ଧାଡ଼ି;
	uint32_t mColumn;
};

ସଂରଚନା CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ସଂରଚନା COverflowData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

ସଂରଚନା CUnreachableData {
	ସଂରଚନା CSourceLocation mLocation;
};

ସଂରଚନା CCFICheckFailData {
	uint8_t mCheckKind;
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

ସଂରଚନା CDynamicTypeCacheMissData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
	ଶୂନ୍ୟ *mTypeInfo;
	uint8_t mTypeCheckKind;
};

ସଂରଚନା CFunctionTypeMismatchData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

ସଂରଚନା CInvalidBuiltinData {
	ସଂରଚନା CSourceLocation mLocation;
	uint8_t mKind;
};

ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

ସଂରଚନା CNonNullArgData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CSourceLocation mAttributeLocation;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mArgIndex;
};

ସଂରଚନା CNonNullReturnData {
	ସଂରଚନା CSourceLocation mAttributeLocation;
};

ସଂରଚନା COutOfBoundsData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mArrayType;
	ସଂରଚନା CTypeDescriptor *mIndexType;
};

ସଂରଚନା CPointerOverflowData {
	ସଂରଚନା CSourceLocation mLocation;
};

ସଂରଚନା CShiftOutOfBoundsData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mLHSType;
	ସଂରଚନା CTypeDescriptor *mRHSType;
};

ସଂରଚନା CTypeMismatchData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

ସଂରଚନା CTypeMismatchData_v1 {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

ସଂରଚନା CVLABoundData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

ସଂରଚନା CFloatCastOverflowData {
	ସଂରଚନା CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ସଂରଚନା CTypeDescriptor *mFromType;
	ସଂରଚନା CTypeDescriptor *mToType;
};

ସଂରଚନା CImplicitConversionData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CTypeDescriptor *mFromType;
	ସଂରଚନା CTypeDescriptor *mToType;
	uint8_t mKind;
};

ସଂରଚନା CAlignmentAssumptionData {
	ସଂରଚନା CSourceLocation mLocation;
	ସଂରଚନା CSourceLocation mAssumptionLocation;
	ସଂରଚନା CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
ଶୂନ୍ୟ __ubsan_handle_add_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_add_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_alignment_assumption(ସଂରଚନା CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
ଶୂନ୍ୟ __ubsan_handle_alignment_assumption_abort(
	ସଂରଚନା CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
ଶୂନ୍ୟ __ubsan_handle_builtin_unreachable(ସଂରଚନା CUnreachableData *pData);
ଶୂନ୍ୟ __ubsan_handle_divrem_overflow(ସଂରଚନା COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_divrem_overflow_abort(ସଂରଚନା COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch_abort(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch_v1(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch_v1_abort(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ଶୂନ୍ୟ __ubsan_handle_invalid_builtin(ସଂରଚନା CInvalidBuiltinData *pData);
ଶୂନ୍ୟ __ubsan_handle_invalid_builtin_abort(ସଂରଚନା CInvalidBuiltinData *pData);
ଶୂନ୍ୟ __ubsan_handle_load_invalid_value(ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
				       unsigned long ulVal);
ଶୂନ୍ୟ __ubsan_handle_load_invalid_value_abort(ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
					     unsigned long ulVal);
ଶୂନ୍ୟ __ubsan_handle_missing_return(ସଂରଚନା CUnreachableData *pData);
ଶୂନ୍ୟ __ubsan_handle_mul_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_mul_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_negate_overflow(ସଂରଚନା COverflowData *pData,
				    unsigned long ulOldVal);
ଶୂନ୍ୟ __ubsan_handle_negate_overflow_abort(ସଂରଚନା COverflowData *pData,
					  unsigned long ulOldVal);
ଶୂନ୍ୟ __ubsan_handle_nullability_arg(ସଂରଚନା CNonNullArgData *pData);
ଶୂନ୍ୟ __ubsan_handle_nullability_arg_abort(ସଂରଚନା CNonNullArgData *pData);
ଶୂନ୍ୟ __ubsan_handle_nullability_return_v1(
	ସଂରଚନା CNonNullReturnData *pData,
	ସଂରଚନା CSourceLocation *pLocationPointer);
ଶୂନ୍ୟ __ubsan_handle_nullability_return_v1_abort(
	ସଂରଚନା CNonNullReturnData *pData,
	ସଂରଚନା CSourceLocation *pLocationPointer);
ଶୂନ୍ୟ __ubsan_handle_out_of_bounds(ସଂରଚନା COutOfBoundsData *pData,
				  unsigned long ulIndex);
ଶୂନ୍ୟ __ubsan_handle_out_of_bounds_abort(ସଂରଚନା COutOfBoundsData *pData,
					unsigned long ulIndex);
ଶୂନ୍ୟ __ubsan_handle_pointer_overflow(ସଂରଚନା CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
ଶୂନ୍ୟ __ubsan_handle_pointer_overflow_abort(ସଂରଚନା CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
ଶୂନ୍ୟ __ubsan_handle_shift_out_of_bounds(ସଂରଚନା CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_shift_out_of_bounds_abort(
	ସଂରଚନା CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_sub_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_sub_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ଶୂନ୍ୟ __ubsan_handle_type_mismatch(ସଂରଚନା CTypeMismatchData *pData,
				  unsigned long ulPointer);
ଶୂନ୍ୟ __ubsan_handle_type_mismatch_abort(ସଂରଚନା CTypeMismatchData *pData,
					unsigned long ulPointer);
ଶୂନ୍ୟ __ubsan_handle_type_mismatch_v1(ସଂରଚନା CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
ଶୂନ୍ୟ __ubsan_handle_type_mismatch_v1_abort(ସଂରଚନା CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
ଶୂନ୍ୟ __ubsan_handle_vla_bound_not_positive(ସଂରଚନା CVLABoundData *pData,
					   unsigned long ulBound);
ଶୂନ୍ୟ __ubsan_handle_vla_bound_not_positive_abort(ସଂରଚନା CVLABoundData *pData,
						 unsigned long ulBound);
ଶୂନ୍ୟ __ubsan_get_current_report_data(ଅଚଳ ଅକ୍ଷର **ppOutIssueKind,
				     ଅଚଳ ଅକ୍ଷର **ppOutMessage,
				     ଅଚଳ ଅକ୍ଷର **ppOutFilename,
				     uint32_t *p_Out_ଧାଡ଼ି, uint32_t *pOutCol,
				     ଅକ୍ଷର **ppOutMemoryAddr);
ସ୍ଥିର ଶୂନ୍ୟ HandleOverflow(bool isFatal, ସଂରଚନା COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ଅଚଳ ଅକ୍ଷର *szOperation);
ସ୍ଥିର ଶୂନ୍ୟ HandleNegateOverflow(bool isFatal, ସଂରଚନା COverflowData *pData,
				 unsigned long ul_Old_ମୂଲ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ HandleBuiltinUnreachable(bool isFatal,
				     ସଂରଚନା CUnreachableData *pData);
ସ୍ଥିର ଶୂନ୍ୟ HandleTypeMismatch(bool isFatal, ସଂରଚନା CSourceLocation *mLocation,
			       ସଂରଚନା CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
ସ୍ଥିର ଶୂନ୍ୟ HandleVlaBoundNotPositive(bool isFatal, ସଂରଚନା CVLABoundData *pData,
				      unsigned long ulBound);
ସ୍ଥିର ଶୂନ୍ୟ HandleOutOfBounds(bool isFatal, ସଂରଚନା COutOfBoundsData *pData,
			      unsigned long ulIndex);
ସ୍ଥିର ଶୂନ୍ୟ HandleShiftOutOfBounds(bool isFatal,
				   ସଂରଚନା CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
ସ୍ଥିର ଶୂନ୍ୟ Handle_Load_Invalid_ମୂଲ୍ୟ(bool isFatal,
				   ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
				   unsigned long ul_ମୂଲ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ HandleInvalidBuiltin(bool isFatal,
				 ସଂରଚନା CInvalidBuiltinData *pData);
ସ୍ଥିର ଶୂନ୍ୟ HandleFunctionTypeMismatch(bool isFatal,
				       ସଂରଚନା CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
ସ୍ଥିର ଶୂନ୍ୟ HandleMissingReturn(bool isFatal, ସଂରଚନା CUnreachableData *pData);
ସ୍ଥିର ଶୂନ୍ୟ HandlePointerOverflow(bool isFatal,
				  ସଂରଚନା CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
ସ୍ଥିର ଶୂନ୍ୟ HandleAlignmentAssumption(bool isFatal,
				      ସଂରଚନା CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define ସଂଖ୍ୟା_SIGNED_BIT 1
#define ସଂଖ୍ୟା_MAXLEN 128
#define __arraycount(__a) (ଆକାର(__a) / ଆକାର(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

ସ୍ଥିର bool isAlreadyReported(ସଂରଚନା CSourceLocation *pLocation)
{
	uint32_t si_Old_ମୂଲ୍ୟ;
	ଅସ୍ଥିର uint32_t *p_ଧାଡ଼ି;

	ଯଦି (!pLocation)
		ଫେରନ୍ତୁ false;

	p_ଧାଡ଼ି = &pLocation->m_ଧାଡ଼ି;

	କର {
		si_Old_ମୂଲ୍ୟ = *p_ଧାଡ଼ି;
	} ଯେତେବେଳେ (__sync_val_compare_and_swap(p_ଧାଡ଼ି, si_Old_ମୂଲ୍ୟ,
					     si_Old_ମୂଲ୍ୟ | ACK_REPORTED) !=
		 si_Old_ମୂଲ୍ୟ);

	ଯଦି (!(si_Old_ମୂଲ୍ୟ & ACK_REPORTED)) {

		sbi_ubsan_report_ଗଣନା++;

		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleOverflow(bool isFatal, ସଂରଚନା COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ଅଚଳ ଅକ୍ଷର *szOperation)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ଯଦି (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} ଅନ୍ୟଥା {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleNegateOverflow(bool isFatal, ସଂରଚନା COverflowData *pData,
				 unsigned long ul_Old_ମୂଲ୍ୟ)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}
	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	ଯଦି (is_signed) {
		sbi_printf("%ld", (long)ul_Old_ମୂଲ୍ୟ);
	} ଅନ୍ୟଥା {
		sbi_printf("%lu", ul_Old_ମୂଲ୍ୟ);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleBuiltinUnreachable(bool isFatal,
				     ସଂରଚନା CUnreachableData *pData)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}
	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ଅଚଳ ଅକ୍ଷର *rgczTypeCheckKinds[] = { "load of",
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

ସ୍ଥିର ଶୂନ୍ୟ HandleTypeMismatch(bool isFatal, ସଂରଚନା CSourceLocation *mLocation,
			       ସଂରଚନା CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	ଯଦି ((!mLocation) || (!mType)) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_ଧାଡ଼ି & ~ACK_REPORTED,
		   mLocation->mColumn);

	ଅଚଳ ଅକ୍ଷର *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	ଯଦି (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} ଅନ୍ୟଥା ଯଦି (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (ଶୂନ୍ୟ *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} ଅନ୍ୟଥା {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (ଶୂନ୍ୟ *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleVlaBoundNotPositive(bool isFatal, ସଂରଚନା CVLABoundData *pData,
				      unsigned long ulBound)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}
	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT;

	ଯଦି (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} ଅନ୍ୟଥା {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleOutOfBounds(bool isFatal, ସଂରଚନା COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}
	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT;

	ଯଦି (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} ଅନ୍ୟଥା {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

ସ୍ଥିର bool is_Negative_ସଂଖ୍ୟା(ସଂରଚନା CTypeDescriptor *pType, unsigned long ulVal)
{
	ଯଦି (!(pType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT)) {
		ଫେରନ୍ତୁ false;
	}

	ଫେରନ୍ତୁ (long)ulVal < 0;
}

ସ୍ଥିର size_t type_width(ସଂରଚନା CTypeDescriptor *pType)
{
	ଫେରନ୍ତୁ 1UL << (pType->mTypeInfo >> 1);
}

ସ୍ଥିର ଶୂନ୍ୟ HandleShiftOutOfBounds(bool isFatal,
				   ସଂରଚନା CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ଯଦି (is_Negative_ସଂଖ୍ୟା(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} ଅନ୍ୟଥା ଯଦି (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} ଅନ୍ୟଥା ଯଦି (is_Negative_ସଂଖ୍ୟା(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} ଅନ୍ୟଥା {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ Handle_Load_Invalid_ମୂଲ୍ୟ(bool isFatal,
				   ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
				   unsigned long ul_ମୂଲ୍ୟ)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & ସଂଖ୍ୟା_SIGNED_BIT;

	sbi_printf("load of value ");

	ଯଦି (is_signed) {
		sbi_printf("%ld ", (long)ul_ମୂଲ୍ୟ);
	} ଅନ୍ୟଥା {
		sbi_printf("%lu ", ul_ମୂଲ୍ୟ);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ଅଚଳ ଅକ୍ଷର *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ସ୍ଥିର ଶୂନ୍ୟ HandleInvalidBuiltin(bool isFatal,
				 ସଂରଚନା CInvalidBuiltinData *pData)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ଅଚଳ ଅକ୍ଷର *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleFunctionTypeMismatch(bool isFatal,
				       ସଂରଚନା CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleMissingReturn(bool isFatal, ସଂରଚନା CUnreachableData *pData)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandlePointerOverflow(bool isFatal,
				  ସଂରଚନା CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

ସ୍ଥିର ଶୂନ୍ୟ HandleAlignmentAssumption(bool isFatal,
				      ସଂରଚନା CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	ଯଦି (!pData) {
		ଫେରନ୍ତୁ;
	}

	ଯଦି (isAlreadyReported(&pData->mLocation)) {
		ଫେରନ୍ତୁ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_ଧାଡ଼ି & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (ଶୂନ୍ୟ *)ulRealPointer, (ଶୂନ୍ୟ *)ulOffset);

	ଯଦି (pData->mAssumptionLocation.mFilename != ଖାଲି) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_ଧାଡ଼ି,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
ଶୂନ୍ୟ __ubsan_handle_add_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

ଶୂନ୍ୟ __ubsan_handle_add_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

ଶୂନ୍ୟ __ubsan_handle_sub_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

ଶୂନ୍ୟ __ubsan_handle_sub_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

ଶୂନ୍ୟ __ubsan_handle_mul_overflow(ସଂରଚନା COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

ଶୂନ୍ୟ __ubsan_handle_mul_overflow_abort(ସଂରଚନା COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

ଶୂନ୍ୟ __ubsan_handle_negate_overflow(ସଂରଚନା COverflowData *pData,
				    unsigned long ul_Old_ମୂଲ୍ୟ)
{
	HandleNegateOverflow(false, pData, ul_Old_ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ __ubsan_handle_negate_overflow_abort(ସଂରଚନା COverflowData *pData,
					  unsigned long ul_Old_ମୂଲ୍ୟ)
{
	HandleNegateOverflow(true, pData, ul_Old_ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ __ubsan_handle_divrem_overflow(ସଂରଚନା COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

ଶୂନ୍ୟ __ubsan_handle_divrem_overflow_abort(ସଂରଚନା COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

ଶୂନ୍ୟ __ubsan_handle_type_mismatch_v1(ସଂରଚନା CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ଶୂନ୍ୟ __ubsan_handle_type_mismatch_v1_abort(ସଂରଚନା CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ଶୂନ୍ୟ __ubsan_handle_out_of_bounds(ସଂରଚନା COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

ଶୂନ୍ୟ __ubsan_handle_out_of_bounds_abort(ସଂରଚନା COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

ଶୂନ୍ୟ __ubsan_handle_shift_out_of_bounds(ସଂରଚନା CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

ଶୂନ୍ୟ __ubsan_handle_shift_out_of_bounds_abort(
	ସଂରଚନା CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

ଶୂନ୍ୟ __ubsan_handle_pointer_overflow(ସଂରଚନା CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

ଶୂନ୍ୟ __ubsan_handle_pointer_overflow_abort(ସଂରଚନା CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

ଶୂନ୍ୟ __ubsan_handle_alignment_assumption(ସଂରଚନା CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ଶୂନ୍ୟ __ubsan_handle_alignment_assumption_abort(
	ସଂରଚନା CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ଶୂନ୍ୟ __ubsan_handle_builtin_unreachable(ସଂରଚନା CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

ଶୂନ୍ୟ __ubsan_handle_invalid_builtin(ସଂରଚନା CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ଶୂନ୍ୟ __ubsan_handle_invalid_builtin_abort(ସଂରଚନା CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ଶୂନ୍ୟ __ubsan_handle_load_invalid_value(ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
				       unsigned long ul_ମୂଲ୍ୟ)
{
	Handle_Load_Invalid_ମୂଲ୍ୟ(false, pData, ul_ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ __ubsan_handle_load_invalid_value_abort(ସଂରଚନା CInvalid_ମୂଲ୍ୟ_Data *pData,
					     unsigned long ul_ମୂଲ୍ୟ)
{
	Handle_Load_Invalid_ମୂଲ୍ୟ(true, pData, ul_ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ __ubsan_handle_missing_return(ସଂରଚନା CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

ଶୂନ୍ୟ __ubsan_handle_vla_bound_not_positive(ସଂରଚନା CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

ଶୂନ୍ୟ __ubsan_handle_vla_bound_not_positive_abort(ସଂରଚନା CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

ଶୂନ୍ୟ __ubsan_handle_function_type_mismatch_abort(
	ସଂରଚନା CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
