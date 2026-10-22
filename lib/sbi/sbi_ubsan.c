/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

فرار unsigned long sbi_ubsan_report_شمار;

ساختار CSourceLocation {
	نویسه *mFilename;
	uint32_t m_سطر;
	uint32_t mColumn;
};

ساختار CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ساختار COverflowData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
};

ساختار CUnreachableData {
	ساختار CSourceLocation mLocation;
};

ساختار CCFICheckFailData {
	uint8_t mCheckKind;
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
};

ساختار CDynamicTypeCacheMissData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
	تهی *mTypeInfo;
	uint8_t mTypeCheckKind;
};

ساختار CFunctionTypeMismatchData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
};

ساختار CInvalidBuiltinData {
	ساختار CSourceLocation mLocation;
	uint8_t mKind;
};

ساختار CInvalid_مقدار_Data {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
};

ساختار CNonNullArgData {
	ساختار CSourceLocation mLocation;
	ساختار CSourceLocation mAttributeLocation;
	صحیح mArgIndex;
};

ساختار CNonNullReturnData {
	ساختار CSourceLocation mAttributeLocation;
};

ساختار COutOfBoundsData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mArrayType;
	ساختار CTypeDescriptor *mIndexType;
};

ساختار CPointerOverflowData {
	ساختار CSourceLocation mLocation;
};

ساختار CShiftOutOfBoundsData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mLHSType;
	ساختار CTypeDescriptor *mRHSType;
};

ساختار CTypeMismatchData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

ساختار CTypeMismatchData_v1 {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

ساختار CVLABoundData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mType;
};

ساختار CFloatCastOverflowData {
	ساختار CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ساختار CTypeDescriptor *mFromType;
	ساختار CTypeDescriptor *mToType;
};

ساختار CImplicitConversionData {
	ساختار CSourceLocation mLocation;
	ساختار CTypeDescriptor *mFromType;
	ساختار CTypeDescriptor *mToType;
	uint8_t mKind;
};

ساختار CAlignmentAssumptionData {
	ساختار CSourceLocation mLocation;
	ساختار CSourceLocation mAssumptionLocation;
	ساختار CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
تهی __ubsan_handle_add_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
تهی __ubsan_handle_add_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
تهی __ubsan_handle_alignment_assumption(ساختار CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
تهی __ubsan_handle_alignment_assumption_abort(
	ساختار CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
تهی __ubsan_handle_builtin_unreachable(ساختار CUnreachableData *pData);
تهی __ubsan_handle_divrem_overflow(ساختار COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
تهی __ubsan_handle_divrem_overflow_abort(ساختار COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
تهی __ubsan_handle_function_type_mismatch(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction);
تهی __ubsan_handle_function_type_mismatch_abort(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction);
تهی __ubsan_handle_function_type_mismatch_v1(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
تهی __ubsan_handle_function_type_mismatch_v1_abort(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
تهی __ubsan_handle_invalid_builtin(ساختار CInvalidBuiltinData *pData);
تهی __ubsan_handle_invalid_builtin_abort(ساختار CInvalidBuiltinData *pData);
تهی __ubsan_handle_load_invalid_value(ساختار CInvalid_مقدار_Data *pData,
				       unsigned long ulVal);
تهی __ubsan_handle_load_invalid_value_abort(ساختار CInvalid_مقدار_Data *pData,
					     unsigned long ulVal);
تهی __ubsan_handle_missing_return(ساختار CUnreachableData *pData);
تهی __ubsan_handle_mul_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
تهی __ubsan_handle_mul_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
تهی __ubsan_handle_negate_overflow(ساختار COverflowData *pData,
				    unsigned long ulOldVal);
تهی __ubsan_handle_negate_overflow_abort(ساختار COverflowData *pData,
					  unsigned long ulOldVal);
تهی __ubsan_handle_nullability_arg(ساختار CNonNullArgData *pData);
تهی __ubsan_handle_nullability_arg_abort(ساختار CNonNullArgData *pData);
تهی __ubsan_handle_nullability_return_v1(
	ساختار CNonNullReturnData *pData,
	ساختار CSourceLocation *pLocationPointer);
تهی __ubsan_handle_nullability_return_v1_abort(
	ساختار CNonNullReturnData *pData,
	ساختار CSourceLocation *pLocationPointer);
تهی __ubsan_handle_out_of_bounds(ساختار COutOfBoundsData *pData,
				  unsigned long ulIndex);
تهی __ubsan_handle_out_of_bounds_abort(ساختار COutOfBoundsData *pData,
					unsigned long ulIndex);
تهی __ubsan_handle_pointer_overflow(ساختار CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
تهی __ubsan_handle_pointer_overflow_abort(ساختار CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
تهی __ubsan_handle_shift_out_of_bounds(ساختار CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
تهی __ubsan_handle_shift_out_of_bounds_abort(
	ساختار CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
تهی __ubsan_handle_sub_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
تهی __ubsan_handle_sub_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
تهی __ubsan_handle_type_mismatch(ساختار CTypeMismatchData *pData,
				  unsigned long ulPointer);
تهی __ubsan_handle_type_mismatch_abort(ساختار CTypeMismatchData *pData,
					unsigned long ulPointer);
تهی __ubsan_handle_type_mismatch_v1(ساختار CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
تهی __ubsan_handle_type_mismatch_v1_abort(ساختار CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
تهی __ubsan_handle_vla_bound_not_positive(ساختار CVLABoundData *pData,
					   unsigned long ulBound);
تهی __ubsan_handle_vla_bound_not_positive_abort(ساختار CVLABoundData *pData,
						 unsigned long ulBound);
تهی __ubsan_get_current_report_data(ثابت نویسه **ppOutIssueKind,
				     ثابت نویسه **ppOutMessage,
				     ثابت نویسه **ppOutFilename,
				     uint32_t *p_Out_سطر, uint32_t *pOutCol,
				     نویسه **ppOutMemoryAddr);
ایستا تهی HandleOverflow(bool isFatal, ساختار COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ثابت نویسه *szOperation);
ایستا تهی HandleNegateOverflow(bool isFatal, ساختار COverflowData *pData,
				 unsigned long ul_Old_مقدار);
ایستا تهی HandleBuiltinUnreachable(bool isFatal,
				     ساختار CUnreachableData *pData);
ایستا تهی HandleTypeMismatch(bool isFatal, ساختار CSourceLocation *mLocation,
			       ساختار CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
ایستا تهی HandleVlaBoundNotPositive(bool isFatal, ساختار CVLABoundData *pData,
				      unsigned long ulBound);
ایستا تهی HandleOutOfBounds(bool isFatal, ساختار COutOfBoundsData *pData,
			      unsigned long ulIndex);
ایستا تهی HandleShiftOutOfBounds(bool isFatal,
				   ساختار CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
ایستا تهی Handle_Load_Invalid_مقدار(bool isFatal,
				   ساختار CInvalid_مقدار_Data *pData,
				   unsigned long ul_مقدار);
ایستا تهی HandleInvalidBuiltin(bool isFatal,
				 ساختار CInvalidBuiltinData *pData);
ایستا تهی HandleFunctionTypeMismatch(bool isFatal,
				       ساختار CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
ایستا تهی HandleMissingReturn(bool isFatal, ساختار CUnreachableData *pData);
ایستا تهی HandlePointerOverflow(bool isFatal,
				  ساختار CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
ایستا تهی HandleAlignmentAssumption(bool isFatal,
				      ساختار CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define عدد_SIGNED_BIT 1
#define عدد_MAXLEN 128
#define __arraycount(__a) (اندازه(__a) / اندازه(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

ایستا bool isAlreadyReported(ساختار CSourceLocation *pLocation)
{
	uint32_t si_Old_مقدار;
	فرار uint32_t *p_سطر;

	اگر (!pLocation)
		بازگشت false;

	p_سطر = &pLocation->m_سطر;

	انجامبده {
		si_Old_مقدار = *p_سطر;
	} وقتی (__sync_val_compare_and_swap(p_سطر, si_Old_مقدار,
					     si_Old_مقدار | ACK_REPORTED) !=
		 si_Old_مقدار);

	اگر (!(si_Old_مقدار & ACK_REPORTED)) {

		sbi_ubsan_report_شمار++;

		بازگشت false;
	}

	بازگشت true;
}

ایستا تهی HandleOverflow(bool isFatal, ساختار COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ثابت نویسه *szOperation)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & عدد_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	اگر (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} وگرنه {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ایستا تهی HandleNegateOverflow(bool isFatal, ساختار COverflowData *pData,
				 unsigned long ul_Old_مقدار)
{
	اگر (!pData) {
		بازگشت;
	}
	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & عدد_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	اگر (is_signed) {
		sbi_printf("%ld", (long)ul_Old_مقدار);
	} وگرنه {
		sbi_printf("%lu", ul_Old_مقدار);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ایستا تهی HandleBuiltinUnreachable(bool isFatal,
				     ساختار CUnreachableData *pData)
{
	اگر (!pData) {
		بازگشت;
	}
	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ثابت نویسه *rgczTypeCheckKinds[] = { "load of",
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

ایستا تهی HandleTypeMismatch(bool isFatal, ساختار CSourceLocation *mLocation,
			       ساختار CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	اگر ((!mLocation) || (!mType)) {
		بازگشت;
	}

	اگر (isAlreadyReported(mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_سطر & ~ACK_REPORTED,
		   mLocation->mColumn);

	ثابت نویسه *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	اگر (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} وگرنه اگر (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (تهی *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} وگرنه {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (تهی *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

ایستا تهی HandleVlaBoundNotPositive(bool isFatal, ساختار CVLABoundData *pData,
				      unsigned long ulBound)
{
	اگر (!pData) {
		بازگشت;
	}
	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & عدد_SIGNED_BIT;

	اگر (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} وگرنه {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ایستا تهی HandleOutOfBounds(bool isFatal, ساختار COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	اگر (!pData) {
		بازگشت;
	}
	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & عدد_SIGNED_BIT;

	اگر (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} وگرنه {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

ایستا bool is_Negative_عدد(ساختار CTypeDescriptor *pType, unsigned long ulVal)
{
	اگر (!(pType->mTypeInfo & عدد_SIGNED_BIT)) {
		بازگشت false;
	}

	بازگشت (long)ulVal < 0;
}

ایستا size_t type_width(ساختار CTypeDescriptor *pType)
{
	بازگشت 1UL << (pType->mTypeInfo >> 1);
}

ایستا تهی HandleShiftOutOfBounds(bool isFatal,
				   ساختار CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	اگر (is_Negative_عدد(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} وگرنه اگر (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} وگرنه اگر (is_Negative_عدد(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} وگرنه {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ایستا تهی Handle_Load_Invalid_مقدار(bool isFatal,
				   ساختار CInvalid_مقدار_Data *pData,
				   unsigned long ul_مقدار)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & عدد_SIGNED_BIT;

	sbi_printf("load of value ");

	اگر (is_signed) {
		sbi_printf("%ld ", (long)ul_مقدار);
	} وگرنه {
		sbi_printf("%lu ", ul_مقدار);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ثابت نویسه *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ایستا تهی HandleInvalidBuiltin(bool isFatal,
				 ساختار CInvalidBuiltinData *pData)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ثابت نویسه *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ایستا تهی HandleFunctionTypeMismatch(bool isFatal,
				       ساختار CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

ایستا تهی HandleMissingReturn(bool isFatal, ساختار CUnreachableData *pData)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ایستا تهی HandlePointerOverflow(bool isFatal,
				  ساختار CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

ایستا تهی HandleAlignmentAssumption(bool isFatal,
				      ساختار CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	اگر (!pData) {
		بازگشت;
	}

	اگر (isAlreadyReported(&pData->mLocation)) {
		بازگشت;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_سطر & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (تهی *)ulRealPointer, (تهی *)ulOffset);

	اگر (pData->mAssumptionLocation.mFilename != پوچ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_سطر,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
تهی __ubsan_handle_add_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

تهی __ubsan_handle_add_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

تهی __ubsan_handle_sub_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

تهی __ubsan_handle_sub_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

تهی __ubsan_handle_mul_overflow(ساختار COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

تهی __ubsan_handle_mul_overflow_abort(ساختار COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

تهی __ubsan_handle_negate_overflow(ساختار COverflowData *pData,
				    unsigned long ul_Old_مقدار)
{
	HandleNegateOverflow(false, pData, ul_Old_مقدار);
}

تهی __ubsan_handle_negate_overflow_abort(ساختار COverflowData *pData,
					  unsigned long ul_Old_مقدار)
{
	HandleNegateOverflow(true, pData, ul_Old_مقدار);
}

تهی __ubsan_handle_divrem_overflow(ساختار COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

تهی __ubsan_handle_divrem_overflow_abort(ساختار COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

تهی __ubsan_handle_type_mismatch_v1(ساختار CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

تهی __ubsan_handle_type_mismatch_v1_abort(ساختار CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

تهی __ubsan_handle_out_of_bounds(ساختار COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

تهی __ubsan_handle_out_of_bounds_abort(ساختار COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

تهی __ubsan_handle_shift_out_of_bounds(ساختار CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

تهی __ubsan_handle_shift_out_of_bounds_abort(
	ساختار CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

تهی __ubsan_handle_pointer_overflow(ساختار CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

تهی __ubsan_handle_pointer_overflow_abort(ساختار CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

تهی __ubsan_handle_alignment_assumption(ساختار CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

تهی __ubsan_handle_alignment_assumption_abort(
	ساختار CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

تهی __ubsan_handle_builtin_unreachable(ساختار CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

تهی __ubsan_handle_invalid_builtin(ساختار CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

تهی __ubsan_handle_invalid_builtin_abort(ساختار CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

تهی __ubsan_handle_load_invalid_value(ساختار CInvalid_مقدار_Data *pData,
				       unsigned long ul_مقدار)
{
	Handle_Load_Invalid_مقدار(false, pData, ul_مقدار);
}

تهی __ubsan_handle_load_invalid_value_abort(ساختار CInvalid_مقدار_Data *pData,
					     unsigned long ul_مقدار)
{
	Handle_Load_Invalid_مقدار(true, pData, ul_مقدار);
}

تهی __ubsan_handle_missing_return(ساختار CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

تهی __ubsan_handle_vla_bound_not_positive(ساختار CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

تهی __ubsan_handle_vla_bound_not_positive_abort(ساختار CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

تهی __ubsan_handle_function_type_mismatch(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

تهی __ubsan_handle_function_type_mismatch_abort(
	ساختار CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
