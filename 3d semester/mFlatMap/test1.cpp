#include "gtest/gtest.h"

#include "mFlatMap.h"

TEST(FlatMapMethodsTests, AllTests) {
    mFlatMap<int, int> my_flatmap1(6);
    ASSERT_NE(nullptr, &my_flatmap1) << "my_flatmap1 was not created";
    mFlatMap<int, int> my_flatmap2(4);
    ASSERT_NE(nullptr, &my_flatmap2) << "my_flatmap2 was not created";

    EXPECT_TRUE(my_flatmap1.insert_by_key(8, 12));
    EXPECT_TRUE(my_flatmap1.insert_by_key(4, 17));
    EXPECT_TRUE(my_flatmap1.insert_by_key(11, 8));
    EXPECT_TRUE(my_flatmap1.insert_by_key(7, 222));
    EXPECT_TRUE(my_flatmap1.insert_by_key(10, 444));
    //
    EXPECT_FALSE(my_flatmap1.insert_by_key(10, 333));
    //

    EXPECT_TRUE(my_flatmap2.insert_by_key(2, 2));
    EXPECT_TRUE(my_flatmap2.insert_by_key(3, 3));

    my_flatmap1.swap_with(my_flatmap2);

    ASSERT_FALSE(my_flatmap1.empty());

    const mFlatMap<int, int> my_flatmap3(my_flatmap2);
    ASSERT_NE(nullptr, &my_flatmap3) << "my_flatmap3 was not created";
    EXPECT_TRUE(my_flatmap3 == my_flatmap2);

    int &temp1 = my_flatmap1.at(2);
    const int &temp2 = my_flatmap2.at(11);
    EXPECT_EQ(2, temp1);
    EXPECT_EQ(8, temp2);

    EXPECT_EQ(8, my_flatmap3.at(11));
    EXPECT_EQ(222, my_flatmap3.at(7));
    EXPECT_EQ(444, my_flatmap3.at(10));

    EXPECT_EQ(2, my_flatmap1.size());

    EXPECT_EQ(my_flatmap3, my_flatmap2);
    EXPECT_NE(my_flatmap1, my_flatmap2);

    EXPECT_NO_THROW(int& element1 = my_flatmap2[5];);

    EXPECT_NO_THROW(int& element2 = my_flatmap2[6];); //no space left, expanding -> more space
    EXPECT_NO_THROW(int& element3 = my_flatmap2.at(6);); //we've just put element with key 6
    EXPECT_ANY_THROW(const int& element4 = my_flatmap3.at(6);); //did not put element with key 6 to 3d map
    //

    EXPECT_TRUE(my_flatmap2.erase_by_key(4));
    EXPECT_FALSE(my_flatmap2.erase_by_key(111));
    my_flatmap1.clear_f_m();
    EXPECT_TRUE(my_flatmap1.empty());

    mFlatMap<int, int> my_flatmap111(5);
    ASSERT_NE(nullptr, &my_flatmap111) << "my_flatmap111 was not created";
    EXPECT_EQ(0, my_flatmap111.size());
    mFlatMap<int, int> my_flatmap222(8);
    ASSERT_NE(nullptr, &my_flatmap222) << "my_flatmap222 was not created";
    EXPECT_EQ(0, my_flatmap222.size());
    my_flatmap111.swap_with(my_flatmap222);
    EXPECT_EQ(0, my_flatmap111.size());
    EXPECT_EQ(0, my_flatmap222.size());

    mFlatMap<int, int> my_flatmap333(13);
    ASSERT_NE(nullptr, &my_flatmap333) << "my_flatmap333 was not created";
    EXPECT_EQ(0, my_flatmap333.size());
    mFlatMap<int, int> my_flatmap444;
    ASSERT_NE(nullptr, &my_flatmap444) << "my_flatmap444 was not created";
    EXPECT_EQ(0, my_flatmap444.size()); //10 is default ctor size
    my_flatmap444 = my_flatmap333;
    EXPECT_EQ(0, my_flatmap333.size());
    EXPECT_EQ(0, my_flatmap444.size());

    mFlatMap<int, int> my_flatmap555(3);
    ASSERT_NE(nullptr, &my_flatmap555);
    EXPECT_TRUE(my_flatmap555.insert_by_key(10, 11));
    mFlatMap<int, int> my_flatmap666(std::move(my_flatmap555));
    ASSERT_NE(nullptr, &my_flatmap666);
    EXPECT_EQ(11, my_flatmap666.at(10));
}

TEST(FlatMapMethodsTest, ManyInsertionsManyDeletions) {
    mFlatMap<int, int> my_flatmap1(180);
    ASSERT_NE(nullptr, &my_flatmap1) << "my_flatmap1 was not created";

    for (int i = 100; i < 200; i++) {
        int j = i;
        bool result = my_flatmap1.insert_by_key(j, j);
        EXPECT_TRUE(result) << "[happened on j = " << j << "]";
    }

    for (int i = 100; i < 200; i++) {
        int j = i;
        bool result = my_flatmap1.insert_by_key(j, j);
        EXPECT_FALSE(result) << "[happened on j = " << j << "]";
    }

    mFlatMap<int, int> my_flatmap2(my_flatmap1);
    ASSERT_NE(nullptr, &my_flatmap2) << "my_flatmap2 was not created";

    EXPECT_EQ(my_flatmap1, my_flatmap2);

    for (int i = 100; i < 200; i+=2) {
        int j = i;
        bool result = my_flatmap1.erase_by_key(j);
        EXPECT_TRUE(result) << "[happened on j = " << j << "]";
    }

    EXPECT_FALSE(my_flatmap1.empty());
    my_flatmap1.clear_f_m();
    EXPECT_TRUE(my_flatmap1.empty());

    for (int i = 100; i < 200; i++) {
        int j = i;
        bool result = my_flatmap2.insert_by_key(j, j);
        EXPECT_FALSE(result) << "[happened on j = " << j << "]";
    }

    mFlatMap<int, int> my_flatmap3(std::move(my_flatmap2));
    ASSERT_NE(nullptr, &my_flatmap3) << "my_flatmap3 was not created";

    my_flatmap3.clear_f_m();
    EXPECT_TRUE(my_flatmap3.empty());
    for (int i = 180; i > 0; i--) { //no real reason, just 94
        int j = i;
        bool result = my_flatmap3.insert_by_key(j, j);
        EXPECT_TRUE(result) << "[happened on j = " << j << "]";
    }

    for (int i = 181; i < 220; i++) {
        int j = i;
        bool result = my_flatmap3.insert_by_key(j, j);
        EXPECT_TRUE(result) << "[happened on j = " << j << "]";
    } //180 slots of flatmap are already occupied, expect expansion
}

TEST(FlatMapMethodsTest, AtAccessToElement) {
    mFlatMap<int, int> my_flatmap1;
    ASSERT_NE(nullptr, &my_flatmap1) << "my_flatmap1 was not created";

    for (int i = 90; i < 98; i++) {
        bool result = my_flatmap1.insert_by_key(i - 3, i);
        EXPECT_TRUE(result);
    }

    for (int i = 90; i < 98; i++) {
        int value = 0;
        EXPECT_NO_THROW(value = my_flatmap1.at(i - 3););
        EXPECT_EQ(i, value);
    }

    for (int i = 98; i < 110; i++) {
        EXPECT_ANY_THROW(int value = my_flatmap1.at(i - 3););
    }

    mFlatMap<int, int> my_flatmap2(my_flatmap1);
    ASSERT_NE(nullptr, &my_flatmap2);

    //this whole repeating segment is there just to test the same but with ->at for const objects
    for (int i = 90; i < 98; i++) {
        int value = 0;
        EXPECT_NO_THROW(value = my_flatmap2.at(i - 3););
        EXPECT_EQ(i, value);
    }

    for (int i = 98; i < 110; i++) {
        EXPECT_ANY_THROW(int value = my_flatmap2.at(i - 3););
    }
}

TEST(FlatMapMethodsTest, OtherTypesVariousTests) {
    mFlatMap<bool, double> my_flatmap1;
    mFlatMap<double, int> my_flatmap2{10};

    for (int i = 0; i < 512; i++) {
        my_flatmap1.insert_by_key(static_cast<double>(i * 1.0), true);
        my_flatmap2[i];
    }

    for (int i = 0; i < 512; i++) {
        EXPECT_TRUE(my_flatmap1.contains(static_cast<double>(i * 1.0)));
        EXPECT_TRUE(my_flatmap2.contains(i));
    }

    EXPECT_FALSE(my_flatmap1.empty());
    EXPECT_EQ(512, my_flatmap2.size());
    //------------------------------------------------------------------------------
    mFlatMap<bool, bool> small_map;
    EXPECT_TRUE(small_map.insert_by_key(true, true));
    EXPECT_FALSE(small_map.insert_by_key(true, false));
    EXPECT_TRUE(small_map.insert_by_key(false, false));
    EXPECT_FALSE(small_map.insert_by_key(false, true));

    EXPECT_EQ(true, small_map.at(true));
    EXPECT_EQ(false, small_map[false]);

    EXPECT_EQ(2, small_map.size());

    small_map.erase_by_key(false);
    EXPECT_ANY_THROW(small_map.at(false));

    small_map.clear_f_m();
    EXPECT_TRUE(small_map.empty());
    //------------------------------------------------------------------------------
    mFlatMap<int, double> my_flatmap3{5};
    for (int i = 0; i < 700; i++) {
        my_flatmap3.insert_by_key(static_cast<double>(i * 2.0), i * 4);
    }
    mFlatMap<int, double> my_flatmap4{my_flatmap3};

    EXPECT_EQ(my_flatmap3, my_flatmap4);

    my_flatmap3.clear_f_m();
    EXPECT_NE(my_flatmap3, my_flatmap4);

    my_flatmap4.swap_with(my_flatmap3);
    EXPECT_TRUE(my_flatmap4.empty());
    EXPECT_EQ(700, my_flatmap3.size());
    EXPECT_EQ(1000, my_flatmap3.at(static_cast<double>(500)));
}











