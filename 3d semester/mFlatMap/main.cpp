#include <iostream>

#include "mFlatMap.h"

#define GOOGLE_TEST_ON //add "//" in front of line if necessary

#ifdef GOOGLE_TEST_ON
#include "gtest/gtest.h"
#endif


int main(int argc, char **argv) {
#ifndef GOOGLE_TEST_ON
    auto *my_flatmap1 = new mFlatMap<int, int>(6);

    auto *my_flatmap2 = new mFlatMap<int, int>(4);

    my_flatmap1->insert_by_key(8, 12);
    my_flatmap1->insert_by_key(4, 17);
    my_flatmap1->insert_by_key(11, 8);
    my_flatmap1->insert_by_key(7, 222);
    my_flatmap1->insert_by_key(10, 444);
    my_flatmap1->insert_by_key(10, 333);
    if (my_flatmap1->empty()) {
        std::cout << "It is empty(" << std::endl;
    } else {
        std::cout << "It is not empty)" << std::endl;
    }

    my_flatmap2->insert_by_key(2, 2);
    my_flatmap2->insert_by_key(3, 3);

    my_flatmap1->swap_with(*my_flatmap2);
    if (my_flatmap1->empty()) {
        std::cout << "It is empty(" << std::endl;
    } else {
        std::cout << "It is not empty)" << std::endl;
    }

    //!!!
    const mFlatMap<int, int> *my_flatmap3 = new mFlatMap<int, int>(*my_flatmap2);

    int &temp1 = my_flatmap1->at(2);
    const int &temp2 = my_flatmap2->at(11);
    std::cout << "temp1 is [" << temp1 << "] = 2" << std::endl; //should be 2
    std::cout << "temp2 is [" << temp2 << "] = 8" << std::endl; //should be 8

    //!!!
    const int &temp3 = my_flatmap3->at(4);
    std::cout << "temp3 is [" << temp3 << "] = 17" << std::endl; //should be 17
    const int &temp4 = my_flatmap3->at(8);
    std::cout << "temp4 is [" << temp4 << "] = 12" << std::endl; //should be 12
    const int &temp5 = my_flatmap3->at(11);
    std::cout << "temp5 is [" << temp5 << "] = 8" << std::endl; //should be 8
    const int &temp6 = my_flatmap3->at(7);
    std::cout << "temp6 is [" << temp6 << "] = 222" << std::endl; //should be 222
    const int &temp7 = my_flatmap3->at(10);
    std::cout << "temp7 is [" << temp7 << "] = 444" << std::endl; //should be 444

    const size_t size1 = my_flatmap1->size();
    std::cout << "size1 is [" << size1 << "] = 4" << std::endl; //should be 4

    if ((*my_flatmap3) == (*my_flatmap2)) {
        std::cout << "mf3 and mf2 are equal" << std::endl;
    } else {
        std::cout << "mf3 and mf2 are NOT equal" << std::endl;
    }

    if ((*my_flatmap1) != (*my_flatmap2)) {
        std::cout << "mf1 and mf2 are NOT equal" << std::endl;
    } else {
        std::cout << "mf1 and mf2 are equal" << std::endl;
    }

    try {
        int& element1 = (*my_flatmap2)[5];
    }
    catch (std::length_error &exception) {
        std::cout << &exception << std::endl;
    }

    try {
        int& element2 = (*my_flatmap2)[6];
    }
    catch (std::length_error &exception) {
        std::cout << &exception << std::endl;
    }

    try {
        int& element3 = my_flatmap2->at(6);
    }
    catch (std::invalid_argument &exception) {
        std::cout << &exception << std::endl;
    }

    try {
        const int& element4 = my_flatmap3->at(6);
    }
    catch (std::invalid_argument &exception) {
        std::cout << &exception << std::endl;
    }

    my_flatmap2->erase_by_key(4); //should return true
    my_flatmap2->erase_by_key(111); //should return false
    my_flatmap1->clear_f_m();
    if (my_flatmap1->empty()) {
        std::cout << "It is now empty(" << std::endl;
    } else {
        std::cout << "It is still NOT empty)" << std::endl;
    }
    delete my_flatmap1;
    delete my_flatmap2;
    delete my_flatmap3;

    auto *my_flatmap111 = new mFlatMap<int, int>(5);
    std::cout << "111 had [" << my_flatmap111->size() << "] for size" << std::endl;
    auto *my_flatmap222 = new mFlatMap<int, int>(8);
    std::cout << "222 had [" << my_flatmap222->size() << "] for size" << std::endl;
    (*my_flatmap111).swap_with2(*my_flatmap222);
    std::cout << "111 now has [" << my_flatmap111->size() << "] for size" << std::endl;
    std::cout << "222 now has [" << my_flatmap222->size() << "] for size" << std::endl;
    delete my_flatmap111;
    delete my_flatmap222;

    auto *my_flatmap333 = new mFlatMap<int, int>(13);
    std::cout << "333 had [" << my_flatmap333->size() << "] for size" << std::endl;
    auto *my_flatmap444 = new mFlatMap<int, int>();
    std::cout << "444 had [" << my_flatmap444->size() << "] for size" << std::endl;
    *my_flatmap444 = (*my_flatmap333);
    std::cout << "333 now has [" << my_flatmap333->size() << "] for size" << std::endl; //should be 13
    std::cout << "444 now has [" << my_flatmap444->size() << "] for size" << std::endl; //should be 13
    delete my_flatmap333;
    delete my_flatmap444;

    auto *my_flatmap555 = new mFlatMap<int, int>(3);
    my_flatmap555->insert_by_key(10, 11);
    auto *my_flatmap666 = new mFlatMap<int, int>(std::move(*my_flatmap555));
    int &temp666 = my_flatmap666->at(10);
    std::cout << "666 has [" << temp666 << "] with key 10" << std::endl; //should be 11
    delete my_flatmap555;
    delete my_flatmap666;

    return 0;
#else
    ::testing::InitGoogleTest(&argc, argv); //googletest insisted on adding this line
    return RUN_ALL_TESTS();
#endif
}
