// To run test:
// pio test -vv -e pc

#include <gtest/gtest.h>
#include <StreamAverage.h>

#include <vector>
#include <numeric>

template<typename T> 
struct StreamAverageTest : public testing::Test
{
	StreamAverage<T> unitUnderTest;
	std::vector<T> values;
};

using testTypes = testing::Types<float, double, int, uint16_t>;

TYPED_TEST_SUITE(StreamAverageTest, testTypes);

TYPED_TEST(StreamAverageTest, shouldInitializeWithZero)
{
	EXPECT_EQ(0, this->unitUnderTest.get());
	EXPECT_EQ(0, this->unitUnderTest.getNumValues());
	EXPECT_EQ(0, this->unitUnderTest.getMax());
	EXPECT_EQ(0, this->unitUnderTest.getMin());
}

TYPED_TEST(StreamAverageTest, resetsAverageAndChecksZero)
{
	this->unitUnderTest << 12;
	this->unitUnderTest << 2;
	this->unitUnderTest.reset();
	EXPECT_EQ(0, this->unitUnderTest.get());
	EXPECT_EQ(0, this->unitUnderTest.getNumValues());
	EXPECT_EQ(0, this->unitUnderTest.getMax());
	EXPECT_EQ(0, this->unitUnderTest.getMin());
}

TYPED_TEST(StreamAverageTest, checksMaxMinNumValues)
{
	this->unitUnderTest.reset();
	this->unitUnderTest << 12;
	this->unitUnderTest << 2;
	this->unitUnderTest << 5;
	this->unitUnderTest << 58;
	EXPECT_EQ(58, this->unitUnderTest.getMax());
	EXPECT_EQ(2, this->unitUnderTest.getMin());
	EXPECT_EQ(4, this->unitUnderTest.getNumValues());
}

TYPED_TEST(StreamAverageTest, checksOperatorsAndImplicitCast)
{
	this->unitUnderTest.reset();
	this->unitUnderTest << 29;
	TypeParam average = this->unitUnderTest;
	EXPECT_EQ(29, average);
}

TYPED_TEST(StreamAverageTest, addsValuesAndChecksPrecision)
{
	this->unitUnderTest.reset();
	for(size_t i = 0; i < 1000; i++)
	{
		TypeParam newValue = static_cast<TypeParam>(std::rand() / 1.37);

		this->values.push_back(newValue);
		TypeParam expectedAvg = std::reduce(this->values.begin(), this->values.end()) / static_cast<TypeParam>(this->values.size());

		TypeParam actualAvg = this->unitUnderTest << newValue;

		EXPECT_NEAR(expectedAvg, actualAvg, 0.1);
	}
}

int main()
{
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}