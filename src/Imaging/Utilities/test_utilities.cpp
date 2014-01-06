#include <iostream>
#include <array>

#include "containers.h"
//#include "algorithms.h"
//#include "safe_operations.h"

void TestSafeOperations(void)
{
	int i1 = 1, i2 = 2, i3, i_max = std::numeric_limits<int>::max(), i_neg = -2;
	unsigned int ui1 = 1, ui2 = 2, ui3;
	long long l1 = 1, l2 = 2, l3;
	unsigned long long ul1 = 1, ul2 = 2, ul3;
	float f1 = 1.0f, f2 = 2.0f, f3, f4 = 1.5f;
	double d1 = 1.0, d2 = 2.0, d3;

	////////////////////////////////////////////////////////////////////////////////////////
	// Unsafe operations.
	i3 = i_max + i1;	// positive overflow without warning!
	ul3 = ul1 + i_neg;	// addition with unsigned type; negative overflow without warning!
	f3 = f1 + (i_max - 1);		// same width floating; data loss risk without warning!?
	i3 = i1 + l2;		// narrowing; data loss risk with warning
	f3 = f1 + d2;		// narrowing; data loss risk with warning
	i3 = f4 + i2;		// floating to integral; data loss risk with warning

	ul3 = ul1 - i2;		// any subtraction has negative overflow risk.

	using namespace Utilities;
	////////////////////////////////////////////////////////////////////////////////////////
	// Addition safe operations.
	// Function arguments can be either (T, U, U&) or (T, U, T&).

	// integral and integral
	Add(i1, i2, i3);	// same type; Okay because safeint checks overflow.
	Add(i1, ui2, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	Add(ui1, i2, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	Add(i1, l2, l3);	// widening
	Add(l1, i2, l3);	// widening
	Add(l1, i2, i3);	// narrowing; Okay because safeint checks overflow.
	Add(i1, l2, i3);	// narrowing; Okay because safeint checks overflow.

	// floating and floating
	Add(f1, f2, f3);	// same type
	Add(f1, d2, d3);	// widening
	Add(d1, f2, d3);	// widening
	//Add(d1, f2, f3);	// narrowing; Disabled for data loss risk.
	//Add(f1, d2, f3);	// narrowing; Disabled for data loss risk.

	// integral and floating
	Add(i1, d2, d3);	// widening floating
	Add(d1, i2, d3);	// widening floating
	Add(ui1, d2, d3);	// widening floating
	Add(d1, ui2, d3);	// widening floating
	//Add(i1, f2, f3);	// same width floating; Disabled for data loss risk.
	//Add(f1, i2, f3);	// same width floating; Disabled for data loss risk.
	//Add(l1, f2, l3);	// floating to integral; Disabled for data loss risk.
	//Add(f1, l2, l3);	// floating to integral; Disabled for data loss risk.

	////////////////////////////////////////////////////////////////////////////////////////
	// Multiplication safe operations.
	// Restriction is identical to addition.
	Multiply(i1, i2, i3);	// same type; Okay because safeint checks overflow.
	Multiply(i1, ui2, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	Multiply(ui1, i2, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	Multiply(i1, l2, l3);	// widening
	Multiply(l1, i2, l3);	// widening
	Multiply(l1, i2, i3);	// narrowing; Okay because safeint checks overflow.
	Multiply(i1, l2, i3);	// narrowing; Okay because safeint checks overflow.
	Multiply(f1, f2, f3);	// same type
	Multiply(f1, d2, d3);	// widening
	Multiply(d1, f2, d3);	// widening
	Multiply(i1, d2, d3);	// widening floating
	Multiply(d1, i2, d3);	// widening floating
	Multiply(ui1, d2, d3);	// widening floating
	Multiply(d1, ui2, d3);	// widening floating

	////////////////////////////////////////////////////////////////////////////////////////
	// Subtraction safe operations.
	// NOTE: Function arguments must be (T, U, T&).

	Subtract(i2, i1, i3);	// same type; Okay because safeint checks overflow.
	//Subtract(i2, ui1, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	Subtract(ui2, i1, ui3);	// same width with cross sign; Okay because safeint checks overflow.
	//Subtract(i2, l1, l3);	// widening
	Subtract(l2, i1, l3);	// widening
	//Subtract(l2, i1, i3);	// narrowing; Okay because safeint checks overflow.
	Subtract(i2, l1, i3);	// narrowing; Okay because safeint checks overflow.
	Subtract(f2, f1, f3);	// same type
	//Subtract(f2, d1, d3);	// widening
	Subtract(d2, f1, d3);	// widening
	//Subtract(i2, d1, d3);	// widening floating
	Subtract(d2, i1, d3);	// widening floating
	//Subtract(ui2, d1, d3);	// widening floating
	Subtract(d2, ui1, d3);	// widening floating

	// Casting
	Cast(d1, i1);
	i3 = Cast<int>(d1);

	// Overflow detections.
	try
	{	// positive overflow
		Add(i_max, i2, i3);
	}
	catch (const std::overflow_error &e)
	{
		std::cout << e.what() << std::endl;
	}
	try
	{	// negative overflow
		Add(ul1, i_neg, ul3);
	}
	catch (const std::overflow_error &e)
	{
		std::cout << e.what() << std::endl;
	}
	try
	{	// negative overflow
		Subtract(ul1, i2, ul3);
	}
	catch (const std::overflow_error &e)
	{
		std::cout << e.what() << std::endl;
	}
	try
	{	// positive overflow
		int a = 256;
		unsigned char b;
		Cast(a, b);
	}
	catch (const std::overflow_error &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void TestContainers(void)
{
	std::array<int, 2> arrayI1 = { 1, 2 }, arrayI2{ 2, 3 }, arrayI3;
	std::array<unsigned int, 2> arrayUI1 = { 1, 2 }, arrayUI2{ 2, 3 }, arrayUI3;

	// Arithmetic operators for arrays with the same data type.
	arrayI3 = arrayI1 + arrayI2;
	arrayI3 = arrayI1 + std::array<int, 2>{2, 3};
	arrayI3 = arrayI1 + std::array<int, 2>{{ 2, 3 }};
	arrayI3 = arrayI1 * arrayI2;
	arrayI3 = arrayI1 * std::array<int, 2>{2, 3};
	arrayI3 = arrayI1 * std::array<int, 2>{{ 2, 3 }};
	arrayI3 = arrayI2 - arrayI1;
	arrayI3 = arrayI2 - std::array<int, 2>{1, 2};
	arrayI3 = arrayI2 - std::array<int, 2>{{ 1, 2 }};
	arrayI3 += arrayI1;
	arrayI3 *= arrayI1;
	arrayI3 -= arrayI1;

	// Functions for arrays with the same data type.
	// (T, U, T&) or (T, U, U&)
	Utilities::AddRange(arrayI1.cbegin(), arrayI1.cend(), arrayUI1.cbegin(), arrayI3.begin());
	Utilities::AddRange(arrayI1.cbegin(), arrayI1.cend(), arrayUI1.cbegin(), arrayUI3.begin());
	Utilities::MultiplyRange(arrayI1.cbegin(), arrayI1.cend(), arrayUI1.cbegin(), arrayI3.begin());
	Utilities::MultiplyRange(arrayI1.cbegin(), arrayI1.cend(), arrayUI1.cbegin(), arrayUI3.begin());
	// only (T, U, T&)
	Utilities::SubtractRange(arrayI2.cbegin(), arrayI2.cend(), arrayUI1.cbegin(), arrayI3.begin());
	
	// NOTE: A += B requires different function from A + B -> C.
	Utilities::AddRange(arrayUI1.cbegin(), arrayUI1.cend(), arrayI3.begin());
	Utilities::MultiplyRange(arrayUI1.cbegin(), arrayUI1.cend(), arrayI3.begin());
	Utilities::SubtractRange(arrayUI1.cbegin(), arrayUI1.cend(), arrayI3.begin());

	arrayI3 = arrayI1 + 2;
	arrayI3 = arrayI1 - 2;
	arrayI3 = arrayI1 * 2;
	++arrayI3;
	arrayI3++;
	--arrayI3;
	arrayI3--;

	// Functions.
	std::array<double, 2> arrayD1 = { 1.5, 2.5 };
	//arrayI3 = Utilities::RoundAs<int>(arrayD1);
	Utilities::Round(arrayD1, arrayI3);
	//arrayI3 = Utilities::Cast<int>(arrayD1);
	Utilities::Cast(arrayD1, arrayI3);
	std::vector<int> v1 = Utilities::GetRangeVector<int>(10);
}

int main(void)
{
	TestSafeOperations();
	TestContainers();
}