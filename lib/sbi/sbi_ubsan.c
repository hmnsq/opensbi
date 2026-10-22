/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#அந்தர்பாவ <sbi/sbi_types.h>
#அந்தர்பாவ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

நிலையற்ற unsigned long sbi_ubsan_report_மொத்தம்;

கட்டமைப்பு CSourceLocation {
	எழுத்து *mFilename;
	uint32_t m_வரி;
	uint32_t mColumn;
};

கட்டமைப்பு CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

கட்டமைப்பு COverflowData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

கட்டமைப்பு CUnreachableData {
	கட்டமைப்பு CSourceLocation mLocation;
};

கட்டமைப்பு CCFICheckFailData {
	uint8_t mCheckKind;
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

கட்டமைப்பு CDynamicTypeCacheMissData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
	வெறுமை *mTypeInfo;
	uint8_t mTypeCheckKind;
};

கட்டமைப்பு CFunctionTypeMismatchData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

கட்டமைப்பு CInvalidBuiltinData {
	கட்டமைப்பு CSourceLocation mLocation;
	uint8_t mKind;
};

கட்டமைப்பு CInvalid_மதிப்பு_Data {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

கட்டமைப்பு CNonNullArgData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CSourceLocation mAttributeLocation;
	முழு mArgIndex;
};

கட்டமைப்பு CNonNullReturnData {
	கட்டமைப்பு CSourceLocation mAttributeLocation;
};

கட்டமைப்பு COutOfBoundsData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mArrayType;
	கட்டமைப்பு CTypeDescriptor *mIndexType;
};

கட்டமைப்பு CPointerOverflowData {
	கட்டமைப்பு CSourceLocation mLocation;
};

கட்டமைப்பு CShiftOutOfBoundsData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mLHSType;
	கட்டமைப்பு CTypeDescriptor *mRHSType;
};

கட்டமைப்பு CTypeMismatchData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

கட்டமைப்பு CTypeMismatchData_v1 {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

கட்டமைப்பு CVLABoundData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

கட்டமைப்பு CFloatCastOverflowData {
	கட்டமைப்பு CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	கட்டமைப்பு CTypeDescriptor *mFromType;
	கட்டமைப்பு CTypeDescriptor *mToType;
};

கட்டமைப்பு CImplicitConversionData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CTypeDescriptor *mFromType;
	கட்டமைப்பு CTypeDescriptor *mToType;
	uint8_t mKind;
};

கட்டமைப்பு CAlignmentAssumptionData {
	கட்டமைப்பு CSourceLocation mLocation;
	கட்டமைப்பு CSourceLocation mAssumptionLocation;
	கட்டமைப்பு CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
வெறுமை __ubsan_handle_add_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
வெறுமை __ubsan_handle_add_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
வெறுமை __ubsan_handle_alignment_assumption(கட்டமைப்பு CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
வெறுமை __ubsan_handle_alignment_assumption_abort(
	கட்டமைப்பு CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
வெறுமை __ubsan_handle_builtin_unreachable(கட்டமைப்பு CUnreachableData *pData);
வெறுமை __ubsan_handle_divrem_overflow(கட்டமைப்பு COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
வெறுமை __ubsan_handle_divrem_overflow_abort(கட்டமைப்பு COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
வெறுமை __ubsan_handle_function_type_mismatch(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction);
வெறுமை __ubsan_handle_function_type_mismatch_abort(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction);
வெறுமை __ubsan_handle_function_type_mismatch_v1(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
வெறுமை __ubsan_handle_function_type_mismatch_v1_abort(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
வெறுமை __ubsan_handle_invalid_builtin(கட்டமைப்பு CInvalidBuiltinData *pData);
வெறுமை __ubsan_handle_invalid_builtin_abort(கட்டமைப்பு CInvalidBuiltinData *pData);
வெறுமை __ubsan_handle_load_invalid_value(கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
				       unsigned long ulVal);
வெறுமை __ubsan_handle_load_invalid_value_abort(கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
					     unsigned long ulVal);
வெறுமை __ubsan_handle_missing_return(கட்டமைப்பு CUnreachableData *pData);
வெறுமை __ubsan_handle_mul_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
வெறுமை __ubsan_handle_mul_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
வெறுமை __ubsan_handle_negate_overflow(கட்டமைப்பு COverflowData *pData,
				    unsigned long ulOldVal);
வெறுமை __ubsan_handle_negate_overflow_abort(கட்டமைப்பு COverflowData *pData,
					  unsigned long ulOldVal);
வெறுமை __ubsan_handle_nullability_arg(கட்டமைப்பு CNonNullArgData *pData);
வெறுமை __ubsan_handle_nullability_arg_abort(கட்டமைப்பு CNonNullArgData *pData);
வெறுமை __ubsan_handle_nullability_return_v1(
	கட்டமைப்பு CNonNullReturnData *pData,
	கட்டமைப்பு CSourceLocation *pLocationPointer);
வெறுமை __ubsan_handle_nullability_return_v1_abort(
	கட்டமைப்பு CNonNullReturnData *pData,
	கட்டமைப்பு CSourceLocation *pLocationPointer);
வெறுமை __ubsan_handle_out_of_bounds(கட்டமைப்பு COutOfBoundsData *pData,
				  unsigned long ulIndex);
வெறுமை __ubsan_handle_out_of_bounds_abort(கட்டமைப்பு COutOfBoundsData *pData,
					unsigned long ulIndex);
வெறுமை __ubsan_handle_pointer_overflow(கட்டமைப்பு CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
வெறுமை __ubsan_handle_pointer_overflow_abort(கட்டமைப்பு CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
வெறுமை __ubsan_handle_shift_out_of_bounds(கட்டமைப்பு CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
வெறுமை __ubsan_handle_shift_out_of_bounds_abort(
	கட்டமைப்பு CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
வெறுமை __ubsan_handle_sub_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
வெறுமை __ubsan_handle_sub_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
வெறுமை __ubsan_handle_type_mismatch(கட்டமைப்பு CTypeMismatchData *pData,
				  unsigned long ulPointer);
வெறுமை __ubsan_handle_type_mismatch_abort(கட்டமைப்பு CTypeMismatchData *pData,
					unsigned long ulPointer);
வெறுமை __ubsan_handle_type_mismatch_v1(கட்டமைப்பு CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
வெறுமை __ubsan_handle_type_mismatch_v1_abort(கட்டமைப்பு CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
வெறுமை __ubsan_handle_vla_bound_not_positive(கட்டமைப்பு CVLABoundData *pData,
					   unsigned long ulBound);
வெறுமை __ubsan_handle_vla_bound_not_positive_abort(கட்டமைப்பு CVLABoundData *pData,
						 unsigned long ulBound);
வெறுமை __ubsan_get_current_report_data(மாறிலி எழுத்து **ppOutIssueKind,
				     மாறிலி எழுத்து **ppOutMessage,
				     மாறிலி எழுத்து **ppOutFilename,
				     uint32_t *p_Out_வரி, uint32_t *pOutCol,
				     எழுத்து **ppOutMemoryAddr);
நிலையான வெறுமை HandleOverflow(bool isFatal, கட்டமைப்பு COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   மாறிலி எழுத்து *szOperation);
நிலையான வெறுமை HandleNegateOverflow(bool isFatal, கட்டமைப்பு COverflowData *pData,
				 unsigned long ul_Old_மதிப்பு);
நிலையான வெறுமை HandleBuiltinUnreachable(bool isFatal,
				     கட்டமைப்பு CUnreachableData *pData);
நிலையான வெறுமை HandleTypeMismatch(bool isFatal, கட்டமைப்பு CSourceLocation *mLocation,
			       கட்டமைப்பு CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
நிலையான வெறுமை HandleVlaBoundNotPositive(bool isFatal, கட்டமைப்பு CVLABoundData *pData,
				      unsigned long ulBound);
நிலையான வெறுமை HandleOutOfBounds(bool isFatal, கட்டமைப்பு COutOfBoundsData *pData,
			      unsigned long ulIndex);
நிலையான வெறுமை HandleShiftOutOfBounds(bool isFatal,
				   கட்டமைப்பு CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
நிலையான வெறுமை Handle_Load_Invalid_மதிப்பு(bool isFatal,
				   கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
				   unsigned long ul_மதிப்பு);
நிலையான வெறுமை HandleInvalidBuiltin(bool isFatal,
				 கட்டமைப்பு CInvalidBuiltinData *pData);
நிலையான வெறுமை HandleFunctionTypeMismatch(bool isFatal,
				       கட்டமைப்பு CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
நிலையான வெறுமை HandleMissingReturn(bool isFatal, கட்டமைப்பு CUnreachableData *pData);
நிலையான வெறுமை HandlePointerOverflow(bool isFatal,
				  கட்டமைப்பு CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
நிலையான வெறுமை HandleAlignmentAssumption(bool isFatal,
				      கட்டமைப்பு CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define எண்_SIGNED_BIT 1
#define எண்_MAXLEN 128
#define __arraycount(__a) (அளவு(__a) / அளவு(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

நிலையான bool isAlreadyReported(கட்டமைப்பு CSourceLocation *pLocation)
{
	uint32_t si_Old_மதிப்பு;
	நிலையற்ற uint32_t *p_வரி;

	எனில் (!pLocation)
		திரும்பு false;

	p_வரி = &pLocation->m_வரி;

	செய் {
		si_Old_மதிப்பு = *p_வரி;
	} போது (__sync_val_compare_and_swap(p_வரி, si_Old_மதிப்பு,
					     si_Old_மதிப்பு | ACK_REPORTED) !=
		 si_Old_மதிப்பு);

	எனில் (!(si_Old_மதிப்பு & ACK_REPORTED)) {

		sbi_ubsan_report_மொத்தம்++;

		திரும்பு false;
	}

	திரும்பு true;
}

நிலையான வெறுமை HandleOverflow(bool isFatal, கட்டமைப்பு COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   மாறிலி எழுத்து *szOperation)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & எண்_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	எனில் (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} இல்லையெனில் {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

நிலையான வெறுமை HandleNegateOverflow(bool isFatal, கட்டமைப்பு COverflowData *pData,
				 unsigned long ul_Old_மதிப்பு)
{
	எனில் (!pData) {
		திரும்பு;
	}
	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & எண்_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	எனில் (is_signed) {
		sbi_printf("%ld", (long)ul_Old_மதிப்பு);
	} இல்லையெனில் {
		sbi_printf("%lu", ul_Old_மதிப்பு);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

நிலையான வெறுமை HandleBuiltinUnreachable(bool isFatal,
				     கட்டமைப்பு CUnreachableData *pData)
{
	எனில் (!pData) {
		திரும்பு;
	}
	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

மாறிலி எழுத்து *rgczTypeCheckKinds[] = { "load of",
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

நிலையான வெறுமை HandleTypeMismatch(bool isFatal, கட்டமைப்பு CSourceLocation *mLocation,
			       கட்டமைப்பு CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	எனில் ((!mLocation) || (!mType)) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_வரி & ~ACK_REPORTED,
		   mLocation->mColumn);

	மாறிலி எழுத்து *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	எனில் (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} இல்லையெனில் எனில் (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (வெறுமை *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} இல்லையெனில் {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (வெறுமை *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

நிலையான வெறுமை HandleVlaBoundNotPositive(bool isFatal, கட்டமைப்பு CVLABoundData *pData,
				      unsigned long ulBound)
{
	எனில் (!pData) {
		திரும்பு;
	}
	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & எண்_SIGNED_BIT;

	எனில் (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} இல்லையெனில் {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

நிலையான வெறுமை HandleOutOfBounds(bool isFatal, கட்டமைப்பு COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	எனில் (!pData) {
		திரும்பு;
	}
	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & எண்_SIGNED_BIT;

	எனில் (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} இல்லையெனில் {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

நிலையான bool is_Negative_எண்(கட்டமைப்பு CTypeDescriptor *pType, unsigned long ulVal)
{
	எனில் (!(pType->mTypeInfo & எண்_SIGNED_BIT)) {
		திரும்பு false;
	}

	திரும்பு (long)ulVal < 0;
}

நிலையான size_t type_width(கட்டமைப்பு CTypeDescriptor *pType)
{
	திரும்பு 1UL << (pType->mTypeInfo >> 1);
}

நிலையான வெறுமை HandleShiftOutOfBounds(bool isFatal,
				   கட்டமைப்பு CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	எனில் (is_Negative_எண்(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} இல்லையெனில் எனில் (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} இல்லையெனில் எனில் (is_Negative_எண்(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} இல்லையெனில் {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

நிலையான வெறுமை Handle_Load_Invalid_மதிப்பு(bool isFatal,
				   கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
				   unsigned long ul_மதிப்பு)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & எண்_SIGNED_BIT;

	sbi_printf("load of value ");

	எனில் (is_signed) {
		sbi_printf("%ld ", (long)ul_மதிப்பு);
	} இல்லையெனில் {
		sbi_printf("%lu ", ul_மதிப்பு);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

மாறிலி எழுத்து *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

நிலையான வெறுமை HandleInvalidBuiltin(bool isFatal,
				 கட்டமைப்பு CInvalidBuiltinData *pData)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	மாறிலி எழுத்து *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

நிலையான வெறுமை HandleFunctionTypeMismatch(bool isFatal,
				       கட்டமைப்பு CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

நிலையான வெறுமை HandleMissingReturn(bool isFatal, கட்டமைப்பு CUnreachableData *pData)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

நிலையான வெறுமை HandlePointerOverflow(bool isFatal,
				  கட்டமைப்பு CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

நிலையான வெறுமை HandleAlignmentAssumption(bool isFatal,
				      கட்டமைப்பு CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	எனில் (!pData) {
		திரும்பு;
	}

	எனில் (isAlreadyReported(&pData->mLocation)) {
		திரும்பு;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_வரி & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (வெறுமை *)ulRealPointer, (வெறுமை *)ulOffset);

	எனில் (pData->mAssumptionLocation.mFilename != வெற்று) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_வரி,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
வெறுமை __ubsan_handle_add_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

வெறுமை __ubsan_handle_add_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

வெறுமை __ubsan_handle_sub_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

வெறுமை __ubsan_handle_sub_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

வெறுமை __ubsan_handle_mul_overflow(கட்டமைப்பு COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

வெறுமை __ubsan_handle_mul_overflow_abort(கட்டமைப்பு COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

வெறுமை __ubsan_handle_negate_overflow(கட்டமைப்பு COverflowData *pData,
				    unsigned long ul_Old_மதிப்பு)
{
	HandleNegateOverflow(false, pData, ul_Old_மதிப்பு);
}

வெறுமை __ubsan_handle_negate_overflow_abort(கட்டமைப்பு COverflowData *pData,
					  unsigned long ul_Old_மதிப்பு)
{
	HandleNegateOverflow(true, pData, ul_Old_மதிப்பு);
}

வெறுமை __ubsan_handle_divrem_overflow(கட்டமைப்பு COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

வெறுமை __ubsan_handle_divrem_overflow_abort(கட்டமைப்பு COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

வெறுமை __ubsan_handle_type_mismatch_v1(கட்டமைப்பு CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

வெறுமை __ubsan_handle_type_mismatch_v1_abort(கட்டமைப்பு CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

வெறுமை __ubsan_handle_out_of_bounds(கட்டமைப்பு COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

வெறுமை __ubsan_handle_out_of_bounds_abort(கட்டமைப்பு COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

வெறுமை __ubsan_handle_shift_out_of_bounds(கட்டமைப்பு CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

வெறுமை __ubsan_handle_shift_out_of_bounds_abort(
	கட்டமைப்பு CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

வெறுமை __ubsan_handle_pointer_overflow(கட்டமைப்பு CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

வெறுமை __ubsan_handle_pointer_overflow_abort(கட்டமைப்பு CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

வெறுமை __ubsan_handle_alignment_assumption(கட்டமைப்பு CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

வெறுமை __ubsan_handle_alignment_assumption_abort(
	கட்டமைப்பு CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

வெறுமை __ubsan_handle_builtin_unreachable(கட்டமைப்பு CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

வெறுமை __ubsan_handle_invalid_builtin(கட்டமைப்பு CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

வெறுமை __ubsan_handle_invalid_builtin_abort(கட்டமைப்பு CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

வெறுமை __ubsan_handle_load_invalid_value(கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
				       unsigned long ul_மதிப்பு)
{
	Handle_Load_Invalid_மதிப்பு(false, pData, ul_மதிப்பு);
}

வெறுமை __ubsan_handle_load_invalid_value_abort(கட்டமைப்பு CInvalid_மதிப்பு_Data *pData,
					     unsigned long ul_மதிப்பு)
{
	Handle_Load_Invalid_மதிப்பு(true, pData, ul_மதிப்பு);
}

வெறுமை __ubsan_handle_missing_return(கட்டமைப்பு CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

வெறுமை __ubsan_handle_vla_bound_not_positive(கட்டமைப்பு CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

வெறுமை __ubsan_handle_vla_bound_not_positive_abort(கட்டமைப்பு CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

வெறுமை __ubsan_handle_function_type_mismatch(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

வெறுமை __ubsan_handle_function_type_mismatch_abort(
	கட்டமைப்பு CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
