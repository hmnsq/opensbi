/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

متطاير unsigned long sbi_ubsan_report_count;

بنية CSourceLocation {
	حرف *mFilename;
	uint32_t mLine;
	uint32_t mColumn;
};

بنية CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

بنية COverflowData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CUnreachableData {
	بنية CSourceLocation mLocation;
};

بنية CCFICheckFailData {
	uint8_t mCheckKind;
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CDynamicTypeCacheMissData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	فراغ *mTypeInfo;
	uint8_t mTypeCheckKind;
};

بنية CFunctionTypeMismatchData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CInvalidBuiltinData {
	بنية CSourceLocation mLocation;
	uint8_t mKind;
};

بنية CInvalidValueData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CNonNullArgData {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAttributeLocation;
	صحيح mArgIndex;
};

بنية CNonNullReturnData {
	بنية CSourceLocation mAttributeLocation;
};

بنية COutOfBoundsData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mArrayType;
	بنية CTypeDescriptor *mIndexType;
};

بنية CPointerOverflowData {
	بنية CSourceLocation mLocation;
};

بنية CShiftOutOfBoundsData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mLHSType;
	بنية CTypeDescriptor *mRHSType;
};

بنية CTypeMismatchData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

بنية CTypeMismatchData_v1 {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

بنية CVLABoundData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CFloatCastOverflowData {
	بنية CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *mToType;
};

بنية CImplicitConversionData {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *mToType;
	uint8_t mKind;
};

بنية CAlignmentAssumptionData {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAssumptionLocation;
	بنية CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
فراغ __ubsan_handle_add_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
فراغ __ubsan_handle_add_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
فراغ __ubsan_handle_alignment_assumption(بنية CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachableData *pData);
فراغ __ubsan_handle_divrem_overflow(بنية COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction);
فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction);
فراغ __ubsan_handle_function_type_mismatch_v1(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
فراغ __ubsan_handle_function_type_mismatch_v1_abort(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
فراغ __ubsan_handle_invalid_builtin(بنية CInvalidBuiltinData *pData);
فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalidBuiltinData *pData);
فراغ __ubsan_handle_load_invalid_value(بنية CInvalidValueData *pData,
				       unsigned long ulVal);
فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalidValueData *pData,
					     unsigned long ulVal);
فراغ __ubsan_handle_missing_return(بنية CUnreachableData *pData);
فراغ __ubsan_handle_mul_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
فراغ __ubsan_handle_mul_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
فراغ __ubsan_handle_negate_overflow(بنية COverflowData *pData,
				    unsigned long ulOldVal);
فراغ __ubsan_handle_negate_overflow_abort(بنية COverflowData *pData,
					  unsigned long ulOldVal);
فراغ __ubsan_handle_nullability_arg(بنية CNonNullArgData *pData);
فراغ __ubsan_handle_nullability_arg_abort(بنية CNonNullArgData *pData);
فراغ __ubsan_handle_nullability_return_v1(
	بنية CNonNullReturnData *pData,
	بنية CSourceLocation *pLocationPointer);
فراغ __ubsan_handle_nullability_return_v1_abort(
	بنية CNonNullReturnData *pData,
	بنية CSourceLocation *pLocationPointer);
فراغ __ubsan_handle_out_of_bounds(بنية COutOfBoundsData *pData,
				  unsigned long ulIndex);
فراغ __ubsan_handle_out_of_bounds_abort(بنية COutOfBoundsData *pData,
					unsigned long ulIndex);
فراغ __ubsan_handle_pointer_overflow(بنية CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
فراغ __ubsan_handle_shift_out_of_bounds(بنية CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
فراغ __ubsan_handle_sub_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
فراغ __ubsan_handle_sub_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
فراغ __ubsan_handle_type_mismatch(بنية CTypeMismatchData *pData,
				  unsigned long ulPointer);
فراغ __ubsan_handle_type_mismatch_abort(بنية CTypeMismatchData *pData,
					unsigned long ulPointer);
فراغ __ubsan_handle_type_mismatch_v1(بنية CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABoundData *pData,
					   unsigned long ulBound);
فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABoundData *pData,
						 unsigned long ulBound);
فراغ __ubsan_get_current_report_data(ثابت حرف **ppOutIssueKind,
				     ثابت حرف **ppOutMessage,
				     ثابت حرف **ppOutFilename,
				     uint32_t *pOutLine, uint32_t *pOutCol,
				     حرف **ppOutMemoryAddr);
ساكن فراغ HandleOverflow(bool isFatal, بنية COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ثابت حرف *szOperation);
ساكن فراغ HandleNegateOverflow(bool isFatal, بنية COverflowData *pData,
				 unsigned long ulOldValue);
ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachableData *pData);
ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
ساكن فراغ HandleVlaBoundNotPositive(bool isFatal, بنية CVLABoundData *pData,
				      unsigned long ulBound);
ساكن فراغ HandleOutOfBounds(bool isFatal, بنية COutOfBoundsData *pData,
			      unsigned long ulIndex);
ساكن فراغ HandleShiftOutOfBounds(bool isFatal,
				   بنية CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
ساكن فراغ HandleLoadInvalidValue(bool isFatal,
				   بنية CInvalidValueData *pData,
				   unsigned long ulValue);
ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalidBuiltinData *pData);
ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachableData *pData);
ساكن فراغ HandlePointerOverflow(bool isFatal,
				  بنية CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define NUMBER_SIGNED_BIT 1
#define NUMBER_MAXLEN 128
#define __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

ساكن bool isAlreadyReported(بنية CSourceLocation *pLocation)
{
	uint32_t siOldValue;
	متطاير uint32_t *pLine;

	ܐܢ (!pLocation)
		ܦܢܐ false;

	pLine = &pLocation->mLine;

	ܥܒܕ {
		siOldValue = *pLine;
	} ܟܡܐ (__sync_val_compare_and_swap(pLine, siOldValue,
					     siOldValue | ACK_REPORTED) !=
		 siOldValue);

	ܐܢ (!(siOldValue & ACK_REPORTED)) {

		sbi_ubsan_report_count++;

		ܦܢܐ false;
	}

	ܦܢܐ true;
}

ساكن فراغ HandleOverflow(bool isFatal, بنية COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ثابت حرف *szOperation)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ܐܢ (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} ܐܚܪܢܐ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleNegateOverflow(bool isFatal, بنية COverflowData *pData,
				 unsigned long ulOldValue)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}
	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	ܐܢ (is_signed) {
		sbi_printf("%ld", (long)ulOldValue);
	} ܐܚܪܢܐ {
		sbi_printf("%lu", ulOldValue);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachableData *pData)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}
	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ثابت حرف *rgczTypeCheckKinds[] = { "load of",
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

ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	ܐܢ ((!mLocation) || (!mType)) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->mLine & ~ACK_REPORTED,
		   mLocation->mColumn);

	ثابت حرف *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	ܐܢ (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} ܐܚܪܢܐ ܐܢ (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (فراغ *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} ܐܚܪܢܐ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (فراغ *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

ساكن فراغ HandleVlaBoundNotPositive(bool isFatal, بنية CVLABoundData *pData,
				      unsigned long ulBound)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}
	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	ܐܢ (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} ܐܚܪܢܐ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساكن فراغ HandleOutOfBounds(bool isFatal, بنية COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}
	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & NUMBER_SIGNED_BIT;

	ܐܢ (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} ܐܚܪܢܐ {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

ساكن bool isNegativeNumber(بنية CTypeDescriptor *pType, unsigned long ulVal)
{
	ܐܢ (!(pType->mTypeInfo & NUMBER_SIGNED_BIT)) {
		ܦܢܐ false;
	}

	ܦܢܐ (long)ulVal < 0;
}

ساكن size_t type_width(بنية CTypeDescriptor *pType)
{
	ܦܢܐ 1UL << (pType->mTypeInfo >> 1);
}

ساكن فراغ HandleShiftOutOfBounds(bool isFatal,
				   بنية CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ܐܢ (isNegativeNumber(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} ܐܚܪܢܐ ܐܢ (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} ܐܚܪܢܐ ܐܢ (isNegativeNumber(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} ܐܚܪܢܐ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساكن فراغ HandleLoadInvalidValue(bool isFatal,
				   بنية CInvalidValueData *pData,
				   unsigned long ulValue)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("load of value ");

	ܐܢ (is_signed) {
		sbi_printf("%ld ", (long)ulValue);
	} ܐܚܪܢܐ {
		sbi_printf("%lu ", ulValue);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalidBuiltinData *pData)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ثابت حرف *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachableData *pData)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساكن فراغ HandlePointerOverflow(bool isFatal,
				  بنية CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	ܐܢ (!pData) {
		ܦܢܐ;
	}

	ܐܢ (isAlreadyReported(&pData->mLocation)) {
		ܦܢܐ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (فراغ *)ulRealPointer, (فراغ *)ulOffset);

	ܐܢ (pData->mAssumptionLocation.mFilename != عدم) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.mLine,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
فراغ __ubsan_handle_add_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_add_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_sub_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_sub_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_mul_overflow(بنية COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_mul_overflow_abort(بنية COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_negate_overflow(بنية COverflowData *pData,
				    unsigned long ulOldValue)
{
	HandleNegateOverflow(false, pData, ulOldValue);
}

فراغ __ubsan_handle_negate_overflow_abort(بنية COverflowData *pData,
					  unsigned long ulOldValue)
{
	HandleNegateOverflow(true, pData, ulOldValue);
}

فراغ __ubsan_handle_divrem_overflow(بنية COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_type_mismatch_v1(بنية CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

فراغ __ubsan_handle_out_of_bounds(بنية COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

فراغ __ubsan_handle_out_of_bounds_abort(بنية COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

فراغ __ubsan_handle_shift_out_of_bounds(بنية CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

فراغ __ubsan_handle_pointer_overflow(بنية CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

فراغ __ubsan_handle_alignment_assumption(بنية CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

فراغ __ubsan_handle_invalid_builtin(بنية CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

فراغ __ubsan_handle_load_invalid_value(بنية CInvalidValueData *pData,
				       unsigned long ulValue)
{
	HandleLoadInvalidValue(false, pData, ulValue);
}

فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalidValueData *pData,
					     unsigned long ulValue)
{
	HandleLoadInvalidValue(true, pData, ulValue);
}

فراغ __ubsan_handle_missing_return(بنية CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
