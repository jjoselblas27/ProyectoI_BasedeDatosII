#include "../include/avl_file.hh"
#include "../utils/timer.hh"

void test_1() {
    auto extractor = [](AudioFeatures &af) { return af.isrc; };
    auto greater = [](char a[13], char b[13]) { return std::strcmp(a, b) > 0; };

    AVLFile<char [13], AudioFeatures, decltype(extractor), decltype(greater)> avl("database_isrc", extractor, greater);

    avl.load("../database/database_1k.csv");

    char _ATR981237524[13]{"ATR981237524"};
    auto search_ATR981237524 = avl.search(_ATR981237524);
    for (auto &i : search_ATR981237524) {
        std::cout << i.to_string() << std::endl;
    }

    AudioFeatures _AUXN22140335("AUXN22140335",0.000347,0.571,167000,0.816);
    avl.insert(_AUXN22140335);

    avl.print_inorder();
}

void test_2() {
    auto extractor = [](AudioFeatures &af) { return af.duration_ms; };
    auto greater = [](int a, int b) { return a > b; };

    AVLFile<int, AudioFeatures, decltype(extractor), decltype(greater)> avl("database_duration", extractor, greater);

    auto range_search_func = [&avl]() {
        avl.load("../database/database_1k.csv");
    };

    auto range_search_time = measure_time(range_search_func);

    /*auto range_search = avl.range_search(197904, 207490);

    for (auto &i : range_search) {
        std::cout << i.to_string() << std::endl;
    }

    avl.print_inorder();*/
}

void test_3() {
    auto extractor = [](AudioFeatures &af) { return af.energy; };
    auto greater = [](double a, double b) { return a > b; };

    AVLFile<double, AudioFeatures, decltype(extractor), decltype(greater)> avl("database_energy", extractor, greater);

    /*avl.load("../database/database_1k.csv");

    auto range_search = avl.range_search(0.0, 0.5);

    for (auto &i : range_search) {
        std::cout << i.to_string() << std::endl;
    }*/

    auto range_search_func = [&avl]() {
        auto range_search = avl.range_search(0.0, 0.5);
    };
}

int main(int argc, char const *argv[]) {

    test_2();

    return 0;
}
