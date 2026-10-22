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

volatile unsigned long sbi_ubsan_report_बिलांग;

struct CSourceLocation {
	char *mFilename;
	uint32_t m_लिन्या;
	uint32_t mColumn;
};

struct CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

struct COverflowData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
};

struct CUnreachableData {
	struct CSourceLocation mLocation;
};

struct CCFICheckFailData {
	uint8_t mCheckKind;
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
};

struct CDynamicTypeCacheMissData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
	void *mTypeInfo;
	uint8_t mTypeCheckKind;
};

struct CFunctionTypeMismatchData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
};

struct CInvalidBuiltinData {
	struct CSourceLocation mLocation;
	uint8_t mKind;
};

struct CInvalid_हालागा_Data {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
};

struct CNonNullArgData {
	struct CSourceLocation mLocation;
	struct CSourceLocation mAttributeLocation;
	int mArgIndex;
};

struct CNonNullReturnData {
	struct CSourceLocation mAttributeLocation;
};

struct COutOfBoundsData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mArrayType;
	struct CTypeDescriptor *mIndexType;
};

struct CPointerOverflowData {
	struct CSourceLocation mLocation;
};

struct CShiftOutOfBoundsData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mLHSType;
	struct CTypeDescriptor *mRHSType;
};

struct CTypeMismatchData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

struct CTypeMismatchData_v1 {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

struct CVLABoundData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mType;
};

struct CFloatCastOverflowData {
	struct CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	struct CTypeDescriptor *mFromType;
	struct CTypeDescriptor *mToType;
};

struct CImplicitConversionData {
	struct CSourceLocation mLocation;
	struct CTypeDescriptor *mFromType;
	struct CTypeDescriptor *mToType;
	uint8_t mKind;
};

struct CAlignmentAssumptionData {
	struct CSourceLocation mLocation;
	struct CSourceLocation mAssumptionLocation;
	struct CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
void __ubsan_handle_add_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_add_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_alignment_assumption(struct CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
void __ubsan_handle_alignment_assumption_abort(
	struct CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
void __ubsan_handle_builtin_unreachable(struct CUnreachableData *pData);
void __ubsan_handle_divrem_overflow(struct COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_divrem_overflow_abort(struct COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
void __ubsan_handle_function_type_mismatch(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction);
void __ubsan_handle_function_type_mismatch_abort(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction);
void __ubsan_handle_function_type_mismatch_v1(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
void __ubsan_handle_function_type_mismatch_v1_abort(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
void __ubsan_handle_invalid_builtin(struct CInvalidBuiltinData *pData);
void __ubsan_handle_invalid_builtin_abort(struct CInvalidBuiltinData *pData);
void __ubsan_handle_load_invalid_value(struct CInvalid_हालागा_Data *pData,
				       unsigned long ulVal);
void __ubsan_handle_load_invalid_value_abort(struct CInvalid_हालागा_Data *pData,
					     unsigned long ulVal);
void __ubsan_handle_missing_return(struct CUnreachableData *pData);
void __ubsan_handle_mul_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_mul_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_negate_overflow(struct COverflowData *pData,
				    unsigned long ulOldVal);
void __ubsan_handle_negate_overflow_abort(struct COverflowData *pData,
					  unsigned long ulOldVal);
void __ubsan_handle_nullability_arg(struct CNonNullArgData *pData);
void __ubsan_handle_nullability_arg_abort(struct CNonNullArgData *pData);
void __ubsan_handle_nullability_return_v1(
	struct CNonNullReturnData *pData,
	struct CSourceLocation *pLocationPointer);
void __ubsan_handle_nullability_return_v1_abort(
	struct CNonNullReturnData *pData,
	struct CSourceLocation *pLocationPointer);
void __ubsan_handle_out_of_bounds(struct COutOfBoundsData *pData,
				  unsigned long ulIndex);
void __ubsan_handle_out_of_bounds_abort(struct COutOfBoundsData *pData,
					unsigned long ulIndex);
void __ubsan_handle_pointer_overflow(struct CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
void __ubsan_handle_pointer_overflow_abort(struct CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
void __ubsan_handle_shift_out_of_bounds(struct CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
void __ubsan_handle_shift_out_of_bounds_abort(
	struct CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
void __ubsan_handle_sub_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
void __ubsan_handle_sub_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
void __ubsan_handle_type_mismatch(struct CTypeMismatchData *pData,
				  unsigned long ulPointer);
void __ubsan_handle_type_mismatch_abort(struct CTypeMismatchData *pData,
					unsigned long ulPointer);
void __ubsan_handle_type_mismatch_v1(struct CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
void __ubsan_handle_type_mismatch_v1_abort(struct CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
void __ubsan_handle_vla_bound_not_positive(struct CVLABoundData *pData,
					   unsigned long ulBound);
void __ubsan_handle_vla_bound_not_positive_abort(struct CVLABoundData *pData,
						 unsigned long ulBound);
void __ubsan_get_current_report_data(const char **ppOutIssueKind,
				     const char **ppOutMessage,
				     const char **ppOutFilename,
				     uint32_t *p_Out_लिन्या, uint32_t *pOutCol,
				     char **ppOutMemoryAddr);
static void HandleOverflow(bool isFatal, struct COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   const char *szOperation);
static void HandleNegateOverflow(bool isFatal, struct COverflowData *pData,
				 unsigned long ul_Old_हालागा);
static void HandleBuiltinUnreachable(bool isFatal,
				     struct CUnreachableData *pData);
static void HandleTypeMismatch(bool isFatal, struct CSourceLocation *mLocation,
			       struct CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
static void HandleVlaBoundNotPositive(bool isFatal, struct CVLABoundData *pData,
				      unsigned long ulBound);
static void HandleOutOfBounds(bool isFatal, struct COutOfBoundsData *pData,
			      unsigned long ulIndex);
static void HandleShiftOutOfBounds(bool isFatal,
				   struct CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
static void Handle_Load_Invalid_हालागा(bool isFatal,
				   struct CInvalid_हालागा_Data *pData,
				   unsigned long ul_हालागा);
static void HandleInvalidBuiltin(bool isFatal,
				 struct CInvalidBuiltinData *pData);
static void HandleFunctionTypeMismatch(bool isFatal,
				       struct CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
static void HandleMissingReturn(bool isFatal, struct CUnreachableData *pData);
static void HandlePointerOverflow(bool isFatal,
				  struct CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
static void HandleAlignmentAssumption(bool isFatal,
				      struct CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define नुमेरो_SIGNED_BIT 1
#define नुमेरो_MAXLEN 128
#define __arraycount(__a) (sizeof(__a) / sizeof(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

static bool isAlreadyReported(struct CSourceLocation *pLocation)
{
	uint32_t si_Old_हालागा;
	volatile uint32_t *p_लिन्या;

	if (!pLocation)
		return false;

	p_लिन्या = &pLocation->m_लिन्या;

	do {
		si_Old_हालागा = *p_लिन्या;
	} while (__sync_val_compare_and_swap(p_लिन्या, si_Old_हालागा,
					     si_Old_हालागा | ACK_REPORTED) !=
		 si_Old_हालागा);

	if (!(si_Old_हालागा & ACK_REPORTED)) {

		sbi_ubsan_report_बिलांग++;

		return false;
	}

	return true;
}

static void HandleOverflow(bool isFatal, struct COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   const char *szOperation)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & नुमेरो_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	if (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} else {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

static void HandleNegateOverflow(bool isFatal, struct COverflowData *pData,
				 unsigned long ul_Old_हालागा)
{
	if (!pData) {
		return;
	}
	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & नुमेरो_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	if (is_signed) {
		sbi_printf("%ld", (long)ul_Old_हालागा);
	} else {
		sbi_printf("%lu", ul_Old_हालागा);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

static void HandleBuiltinUnreachable(bool isFatal,
				     struct CUnreachableData *pData)
{
	if (!pData) {
		return;
	}
	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

const char *rgczTypeCheckKinds[] = { "load of",
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

static void HandleTypeMismatch(bool isFatal, struct CSourceLocation *mLocation,
			       struct CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	if ((!mLocation) || (!mType)) {
		return;
	}

	if (isAlreadyReported(mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_लिन्या & ~ACK_REPORTED,
		   mLocation->mColumn);

	const char *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	if (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} else if (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (void *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} else {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (void *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

static void HandleVlaBoundNotPositive(bool isFatal, struct CVLABoundData *pData,
				      unsigned long ulBound)
{
	if (!pData) {
		return;
	}
	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & नुमेरो_SIGNED_BIT;

	if (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} else {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

static void HandleOutOfBounds(bool isFatal, struct COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	if (!pData) {
		return;
	}
	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & नुमेरो_SIGNED_BIT;

	if (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} else {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

static bool is_Negative_नुमेरो(struct CTypeDescriptor *pType, unsigned long ulVal)
{
	if (!(pType->mTypeInfo & नुमेरो_SIGNED_BIT)) {
		return false;
	}

	return (long)ulVal < 0;
}

static size_t type_width(struct CTypeDescriptor *pType)
{
	return 1UL << (pType->mTypeInfo >> 1);
}

static void HandleShiftOutOfBounds(bool isFatal,
				   struct CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	if (is_Negative_नुमेरो(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} else if (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} else if (is_Negative_नुमेरो(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} else {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

static void Handle_Load_Invalid_हालागा(bool isFatal,
				   struct CInvalid_हालागा_Data *pData,
				   unsigned long ul_हालागा)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & नुमेरो_SIGNED_BIT;

	sbi_printf("load of value ");

	if (is_signed) {
		sbi_printf("%ld ", (long)ul_हालागा);
	} else {
		sbi_printf("%lu ", ul_हालागा);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

const char *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

static void HandleInvalidBuiltin(bool isFatal,
				 struct CInvalidBuiltinData *pData)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	const char *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

static void HandleFunctionTypeMismatch(bool isFatal,
				       struct CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

static void HandleMissingReturn(bool isFatal, struct CUnreachableData *pData)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

static void HandlePointerOverflow(bool isFatal,
				  struct CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

static void HandleAlignmentAssumption(bool isFatal,
				      struct CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	if (!pData) {
		return;
	}

	if (isAlreadyReported(&pData->mLocation)) {
		return;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_लिन्या & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (void *)ulRealPointer, (void *)ulOffset);

	if (pData->mAssumptionLocation.mFilename != NULL) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_लिन्या,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
void __ubsan_handle_add_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

void __ubsan_handle_add_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

void __ubsan_handle_sub_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

void __ubsan_handle_sub_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

void __ubsan_handle_mul_overflow(struct COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

void __ubsan_handle_mul_overflow_abort(struct COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

void __ubsan_handle_negate_overflow(struct COverflowData *pData,
				    unsigned long ul_Old_हालागा)
{
	HandleNegateOverflow(false, pData, ul_Old_हालागा);
}

void __ubsan_handle_negate_overflow_abort(struct COverflowData *pData,
					  unsigned long ul_Old_हालागा)
{
	HandleNegateOverflow(true, pData, ul_Old_हालागा);
}

void __ubsan_handle_divrem_overflow(struct COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

void __ubsan_handle_divrem_overflow_abort(struct COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

void __ubsan_handle_type_mismatch_v1(struct CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

void __ubsan_handle_type_mismatch_v1_abort(struct CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

void __ubsan_handle_out_of_bounds(struct COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

void __ubsan_handle_out_of_bounds_abort(struct COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

void __ubsan_handle_shift_out_of_bounds(struct CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

void __ubsan_handle_shift_out_of_bounds_abort(
	struct CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

void __ubsan_handle_pointer_overflow(struct CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

void __ubsan_handle_pointer_overflow_abort(struct CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

void __ubsan_handle_alignment_assumption(struct CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

void __ubsan_handle_alignment_assumption_abort(
	struct CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

void __ubsan_handle_builtin_unreachable(struct CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

void __ubsan_handle_invalid_builtin(struct CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

void __ubsan_handle_invalid_builtin_abort(struct CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

void __ubsan_handle_load_invalid_value(struct CInvalid_हालागा_Data *pData,
				       unsigned long ul_हालागा)
{
	Handle_Load_Invalid_हालागा(false, pData, ul_हालागा);
}

void __ubsan_handle_load_invalid_value_abort(struct CInvalid_हालागा_Data *pData,
					     unsigned long ul_हालागा)
{
	Handle_Load_Invalid_हालागा(true, pData, ul_हालागा);
}

void __ubsan_handle_missing_return(struct CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

void __ubsan_handle_vla_bound_not_positive(struct CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

void __ubsan_handle_vla_bound_not_positive_abort(struct CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

void __ubsan_handle_function_type_mismatch(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

void __ubsan_handle_function_type_mismatch_abort(
	struct CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
