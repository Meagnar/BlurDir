#include "library.cpp"
#include <gtest/gtest.h>

TEST(LibraryTest, NoFile)
{
    ASSERT_EQ(findFaces("").size(), 0) << "Function call with non-existant file must return empty vector.";
}

TEST(LibraryTest, NotImage)
{
    ASSERT_EQ(findFaces("Test_files/1.txt").size(), 0) << "Function call with non-image file must return empty vector.";
}

TEST(LibraryTest, NicolasCage)
{
    EXPECT_EQ(findFaces("Test_files/Cage.png").size(), 1) << "Image with Nicolas Cage.";
}

TEST(LibraryTest, Penguins)
{
    EXPECT_EQ(findFaces("Test_files/peng.jpg").size(), 0) << "Image with several penguins.";
}
