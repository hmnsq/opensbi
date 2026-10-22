/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#include <sbi/sbi_types.h>
#include <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

ca_ncala unsigned long sbi_ubsan_report_count;

sa_nracanaa CSourceLocation {
	var_na *mFilename;
	uint32_t mLine;
	uint32_t mColumn;
};

sa_nracanaa CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

sa_nracanaa COverflowData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

sa_nracanaa CUnreachableData {
	sa_nracanaa CSourceLocation mLocation;
};

sa_nracanaa CCFICheckFailData {
	uint8_t mCheckKind;
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

sa_nracanaa CDynamicTypeCacheMissData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
	void *mTypeInfo;
	uint8_t mTypeCheckKind;
};

sa_nracanaa CFunctionTypeMismatchData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

sa_nracanaa CInvalidBuiltinData {
	sa_nracanaa CSourceLocation mLocation;
	uint8_t mKind;
};

sa_nracanaa CInvalidValueData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

sa_nracanaa CNonNullArgData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CSourceLocation mAttributeLocation;
	จำนวนเต็ม mArgIndex;
};

sa_nracanaa CNonNullReturnData {
	sa_nracanaa CSourceLocation mAttributeLocation;
};

sa_nracanaa COutOfBoundsData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mArrayType;
	sa_nracanaa CTypeDescriptor *mIndexType;
};

sa_nracanaa CPointerOverflowData {
	sa_nracanaa CSourceLocation mLocation;
};

sa_nracanaa CShiftOutOfBoundsData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mLHSType;
	sa_nracanaa CTypeDescriptor *mRHSType;
};

sa_nracanaa CTypeMismatchData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

sa_nracanaa CTypeMismatchData_v1 {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

sa_nracanaa CVLABoundData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

sa_nracanaa CFloatCastOverflowData {
	sa_nracanaa CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	sa_nracanaa CTypeDescriptor *mFromType;
	sa_nracanaa CTypeDescriptor *mToType;
};

sa_nracanaa CImplicitConversionData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CTypeDescriptor *mFromType;
	sa_nracanaa CTypeDescriptor *mToType;
	uint8_t mKind;
};

sa_nracanaa CAlignmentAssumptionData {
	sa_nracanaa CSourceLocation mLocation;
	sa_nracanaa CSourceLocation mAssumptionLocation;
	sa_nracanaa CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
void __ubsan_handle_add_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_add_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_alignment_assumption(sa_nracanaa CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
void __ubsan_handle_alignment_assumption_abort(
	sa_nracanaa CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
void __ubsan_handle_builtin_unreachable(sa_nracanaa CUnreachableData *pData);
void __ubsan_handle_divrem_overflow(sa_nracanaa COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_divrem_overflow_abort(sa_nracanaa COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
void __ubsan_handle_function_type_mismatch(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction);
void __ubsan_handle_function_type_mismatch_abort(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction);
void __ubsan_handle_function_type_mismatch_v1(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
void __ubsan_handle_function_type_mismatch_v1_abort(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
void __ubsan_handle_invalid_builtin(sa_nracanaa CInvalidBuiltinData *pData);
void __ubsan_handle_invalid_builtin_abort(sa_nracanaa CInvalidBuiltinData *pData);
void __ubsan_handle_load_invalid_value(sa_nracanaa CInvalidValueData *pData,
				       unsigned long ulVal);
void __ubsan_handle_load_invalid_value_abort(sa_nracanaa CInvalidValueData *pData,
					     unsigned long ulVal);
void __ubsan_handle_missing_return(sa_nracanaa CUnreachableData *pData);
void __ubsan_handle_mul_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_mul_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_negate_overflow(sa_nracanaa COverflowData *pData,
				    unsigned long ulOldVal);
void __ubsan_handle_negate_overflow_abort(sa_nracanaa COverflowData *pData,
					  unsigned long ulOldVal);
void __ubsan_handle_nullability_arg(sa_nracanaa CNonNullArgData *pData);
void __ubsan_handle_nullability_arg_abort(sa_nracanaa CNonNullArgData *pData);
void __ubsan_handle_nullability_return_v1(
	sa_nracanaa CNonNullReturnData *pData,
	sa_nracanaa CSourceLocation *pLocationPointer);
void __ubsan_handle_nullability_return_v1_abort(
	sa_nracanaa CNonNullReturnData *pData,
	sa_nracanaa CSourceLocation *pLocationPointer);
void __ubsan_handle_out_of_bounds(sa_nracanaa COutOfBoundsData *pData,
				  unsigned long ulIndex);
void __ubsan_handle_out_of_bounds_abort(sa_nracanaa COutOfBoundsData *pData,
					unsigned long ulIndex);
void __ubsan_handle_pointer_overflow(sa_nracanaa CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
void __ubsan_handle_pointer_overflow_abort(sa_nracanaa CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
void __ubsan_handle_shift_out_of_bounds(sa_nracanaa CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
void __ubsan_handle_shift_out_of_bounds_abort(
	sa_nracanaa CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
void __ubsan_handle_sub_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_sub_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_type_mismatch(sa_nracanaa CTypeMismatchData *pData,
				  unsigned long ulPointer);
void __ubsan_handle_type_mismatch_abort(sa_nracanaa CTypeMismatchData *pData,
					unsigned long ulPointer);
void __ubsan_handle_type_mismatch_v1(sa_nracanaa CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
void __ubsan_handle_type_mismatch_v1_abort(sa_nracanaa CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
void __ubsan_handle_vla_bound_not_positive(sa_nracanaa CVLABoundData *pData,
					   unsigned long ulBound);
void __ubsan_handle_vla_bound_not_positive_abort(sa_nracanaa CVLABoundData *pData,
						 unsigned long ulBound);
void __ubsan_get_current_report_data(const var_na **ppOutIssueKind,
				     const var_na **ppOutMessage,
				     const var_na **ppOutFilename,
				     uint32_t *pOutLine, uint32_t *pOutCol,
				     var_na **ppOutMemoryAddr);
static void HandleOverflow(bool isFatal, sa_nracanaa COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   const var_na *szOperation);
static void HandleNegateOverflow(bool isFatal, sa_nracanaa COverflowData *pData,
				 unsigned long ulOldValue);
static void HandleBuiltinUnreachable(bool isFatal,
				     sa_nracanaa CUnreachableData *pData);
static void HandleTypeMismatch(bool isFatal, sa_nracanaa CSourceLocation *mLocation,
			       sa_nracanaa CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
static void HandleVlaBoundNotPositive(bool isFatal, sa_nracanaa CVLABoundData *pData,
				      unsigned long ulBound);
static void HandleOutOfBounds(bool isFatal, sa_nracanaa COutOfBoundsData *pData,
			      unsigned long ulIndex);
static void HandleShiftOutOfBounds(bool isFatal,
				   sa_nracanaa CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
static void HandleLoadInvalidValue(bool isFatal,
				   sa_nracanaa CInvalidValueData *pData,
				   unsigned long ulValue);
static void HandleInvalidBuiltin(bool isFatal,
				 sa_nracanaa CInvalidBuiltinData *pData);
static void HandleFunctionTypeMismatch(bool isFatal,
				       sa_nracanaa CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
static void HandleMissingReturn(bool isFatal, sa_nracanaa CUnreachableData *pData);
static void HandlePointerOverflow(bool isFatal,
				  sa_nracanaa CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
static void HandleAlignmentAssumption(bool isFatal,
				      sa_nracanaa CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define NUMBER_SIGNED_BIT 1
#define NUMBER_MAXLEN 128
#define __arraycount(__a) (parimaa_na(__a) / parimaa_na(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

static bool isAlreadyReported(sa_nracanaa CSourceLocation *pLocation)
{
	uint32_t siOldValue;
	ca_ncala uint32_t *pLine;

	ถ้า (!pLocation)
		คืน false;

	pLine = &pLocation->mLine;

	do {
		siOldValue = *pLine;
	} ขณะ (__sync_val_compare_and_swap(pLine, siOldValue,
					     siOldValue | ACK_REPORTED) !=
		 siOldValue);

	ถ้า (!(siOldValue & ACK_REPORTED)) {

		sbi_ubsan_report_count++;

		คืน false;
	}

	คืน true;
}

static void HandleOverflow(bool isFatal, sa_nracanaa COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   const var_na *szOperation)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	ถ้า (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} มิฉะนั้น {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

static void HandleNegateOverflow(bool isFatal, sa_nracanaa COverflowData *pData,
				 unsigned long ulOldValue)
{
	ถ้า (!pData) {
		คืน;
	}
	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	ถ้า (is_signed) {
		sbi_printf("%ld", (long)ulOldValue);
	} มิฉะนั้น {
		sbi_printf("%lu", ulOldValue);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

static void HandleBuiltinUnreachable(bool isFatal,
				     sa_nracanaa CUnreachableData *pData)
{
	ถ้า (!pData) {
		คืน;
	}
	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

const var_na *rgczTypeCheckKinds[] = { "load of",
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

static void HandleTypeMismatch(bool isFatal, sa_nracanaa CSourceLocation *mLocation,
			       sa_nracanaa CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	ถ้า ((!mLocation) || (!mType)) {
		คืน;
	}

	ถ้า (isAlreadyReported(mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->mLine & ~ACK_REPORTED,
		   mLocation->mColumn);

	const var_na *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	ถ้า (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} มิฉะนั้น ถ้า (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (void *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} มิฉะนั้น {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (void *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

static void HandleVlaBoundNotPositive(bool isFatal, sa_nracanaa CVLABoundData *pData,
				      unsigned long ulBound)
{
	ถ้า (!pData) {
		คืน;
	}
	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	ถ้า (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} มิฉะนั้น {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

static void HandleOutOfBounds(bool isFatal, sa_nracanaa COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	ถ้า (!pData) {
		คืน;
	}
	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & NUMBER_SIGNED_BIT;

	ถ้า (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} มิฉะนั้น {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

static bool isNegativeNumber(sa_nracanaa CTypeDescriptor *pType, unsigned long ulVal)
{
	ถ้า (!(pType->mTypeInfo & NUMBER_SIGNED_BIT)) {
		คืน false;
	}

	คืน (long)ulVal < 0;
}

static size_t type_width(sa_nracanaa CTypeDescriptor *pType)
{
	คืน 1UL << (pType->mTypeInfo >> 1);
}

static void HandleShiftOutOfBounds(bool isFatal,
				   sa_nracanaa CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ถ้า (isNegativeNumber(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} มิฉะนั้น ถ้า (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} มิฉะนั้น ถ้า (isNegativeNumber(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} มิฉะนั้น {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

static void HandleLoadInvalidValue(bool isFatal,
				   sa_nracanaa CInvalidValueData *pData,
				   unsigned long ulValue)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & NUMBER_SIGNED_BIT;

	sbi_printf("load of value ");

	ถ้า (is_signed) {
		sbi_printf("%ld ", (long)ulValue);
	} มิฉะนั้น {
		sbi_printf("%lu ", ulValue);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

const var_na *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

static void HandleInvalidBuiltin(bool isFatal,
				 sa_nracanaa CInvalidBuiltinData *pData)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	const var_na *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

static void HandleFunctionTypeMismatch(bool isFatal,
				       sa_nracanaa CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
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

static void HandleMissingReturn(bool isFatal, sa_nracanaa CUnreachableData *pData)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
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

static void HandlePointerOverflow(bool isFatal,
				  sa_nracanaa CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
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

static void HandleAlignmentAssumption(bool isFatal,
				      sa_nracanaa CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	ถ้า (!pData) {
		คืน;
	}

	ถ้า (isAlreadyReported(&pData->mLocation)) {
		คืน;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.mLine & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (void *)ulRealPointer, (void *)ulOffset);

	ถ้า (pData->mAssumptionLocation.mFilename != NULL) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.mLine,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
void __ubsan_handle_add_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

void __ubsan_handle_add_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

void __ubsan_handle_sub_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

void __ubsan_handle_sub_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

void __ubsan_handle_mul_overflow(sa_nracanaa COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

void __ubsan_handle_mul_overflow_abort(sa_nracanaa COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

void __ubsan_handle_negate_overflow(sa_nracanaa COverflowData *pData,
				    unsigned long ulOldValue)
{
	HandleNegateOverflow(false, pData, ulOldValue);
}

void __ubsan_handle_negate_overflow_abort(sa_nracanaa COverflowData *pData,
					  unsigned long ulOldValue)
{
	HandleNegateOverflow(true, pData, ulOldValue);
}

void __ubsan_handle_divrem_overflow(sa_nracanaa COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

void __ubsan_handle_divrem_overflow_abort(sa_nracanaa COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

void __ubsan_handle_type_mismatch_v1(sa_nracanaa CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

void __ubsan_handle_type_mismatch_v1_abort(sa_nracanaa CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

void __ubsan_handle_out_of_bounds(sa_nracanaa COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

void __ubsan_handle_out_of_bounds_abort(sa_nracanaa COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

void __ubsan_handle_shift_out_of_bounds(sa_nracanaa CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

void __ubsan_handle_shift_out_of_bounds_abort(
	sa_nracanaa CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

void __ubsan_handle_pointer_overflow(sa_nracanaa CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

void __ubsan_handle_pointer_overflow_abort(sa_nracanaa CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

void __ubsan_handle_alignment_assumption(sa_nracanaa CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

void __ubsan_handle_alignment_assumption_abort(
	sa_nracanaa CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

void __ubsan_handle_builtin_unreachable(sa_nracanaa CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

void __ubsan_handle_invalid_builtin(sa_nracanaa CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

void __ubsan_handle_invalid_builtin_abort(sa_nracanaa CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

void __ubsan_handle_load_invalid_value(sa_nracanaa CInvalidValueData *pData,
				       unsigned long ulValue)
{
	HandleLoadInvalidValue(false, pData, ulValue);
}

void __ubsan_handle_load_invalid_value_abort(sa_nracanaa CInvalidValueData *pData,
					     unsigned long ulValue)
{
	HandleLoadInvalidValue(true, pData, ulValue);
}

void __ubsan_handle_missing_return(sa_nracanaa CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

void __ubsan_handle_vla_bound_not_positive(sa_nracanaa CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

void __ubsan_handle_vla_bound_not_positive_abort(sa_nracanaa CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

void __ubsan_handle_function_type_mismatch(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

void __ubsan_handle_function_type_mismatch_abort(
	sa_nracanaa CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
