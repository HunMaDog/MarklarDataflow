#include "gtest/gtest.h"

#include <data/properties.hpp>

#include <typeindex>
#include <typeinfo>

using marklar_dataflow::data::Properties;
using marklar_dataflow::error::Exception;

namespace
{

TEST(gtest_data_properties, properties_set_and_get)
{
    const std::string propertieName = "testName";
    const std::string propertieData = "testData";

    Properties testProperties;
    EXPECT_FALSE(testProperties.contains(propertieName));

    testProperties.set(propertieName, propertieData);
    EXPECT_TRUE(testProperties.contains(propertieName));
    EXPECT_EQ(testProperties.getAs<std::string>(propertieName), propertieData);
}

TEST(gtest_data_properties, properties_set_already_exists)
{
    const std::string metadataName = "testName";
    const std::string metadataData = "testData";

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    EXPECT_NO_THROW(testMetadata.set(metadataName, metadataData));
    EXPECT_TRUE(testMetadata.contains(metadataName));
    EXPECT_THROW(testMetadata.set(metadataName, metadataData), Exception);
}

TEST(gtest_data_properties, properties_get_not_contained)
{
    const std::string metadataName = "testName";

    Properties testMetadata;

    EXPECT_THROW(testMetadata.getAs<std::string>(metadataName), Exception);
}

TEST(gtest_data_properties, properties_get_failed_conversion)
{
    const std::string metadataName = "testName";
    const std::string metadataData = "testData";

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    testMetadata.set(metadataName, metadataData);
    EXPECT_THROW(testMetadata.getAs<double>(metadataName), Exception);
}

TEST(gtest_data_properties, properties_get_int_conversion)
{
    const std::string metadataName = "testName";
    const int metadataData = 1234567890;

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    testMetadata.set(metadataName, metadataData);
    EXPECT_EQ(testMetadata.getAs<int>(metadataName), metadataData);
}

TEST(gtest_data_properties, properties_get_double_conversion)
{
    const std::string metadataName = "testName";
    const double metadataData = 3.14159265359;

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    testMetadata.set(metadataName, metadataData);
    EXPECT_EQ(testMetadata.getAs<double>(metadataName), metadataData);
}

TEST(gtest_data_properties, properties_get_int_type)
{
    const std::string metadataName = "testName";
    const int metadataData = 1234567890;

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    testMetadata.set(metadataName, metadataData);
    EXPECT_EQ(testMetadata.type(metadataName), std::type_index(typeid(metadataData)));
}

TEST(gtest_data_properties, properties_get_double_type)
{
    const std::string metadataName = "testName";
    const double metadataData = 3.14159265359;

    Properties testMetadata;
    EXPECT_FALSE(testMetadata.contains(metadataName));

    testMetadata.set(metadataName, metadataData);
    EXPECT_EQ(testMetadata.type(metadataName), std::type_index(typeid(metadataData)));
}

} // namespace
