#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>

#include "helpers.h"


#define TEST(SUITE_NAME, TEST_NAME, ...) \
    class Test_ ## SUITE_NAME ## _ ## TEST_NAME : public ::Tests, ## __VA_ARGS__ { \
    private: \
        Test_ ## SUITE_NAME ## _ ## TEST_NAME (char const * suiteName, char const * testName): \
            ::Tests(__FILE__, __LINE__, suiteName, testName) { \
        } \
        void run_() override; \
        static Test_ ## SUITE_NAME ## _ ## TEST_NAME singleton_; \
    } \
    Test_ ## SUITE_NAME ## _ ## TEST_NAME ::singleton_{# SUITE_NAME, # TEST_NAME }; \
    inline void Test_ ## SUITE_NAME ## _ ## TEST_NAME ::run_() 


#define EXPECT(...) if (expect(__FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)) {} else {}
#define EXPECT_EQ(...) if (expectEq(__FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)) {} else {}

class Tests {
public:
    
    static int run(int argc, char * argv[]);

protected:
    Tests(char const * filename, size_t line, char const * suiteName, char const * testName):
        suiteName_{suiteName},
        testName_{testName},
        filename_{filename},
        line_{line} {
            if (addTest_(suiteName_, testName_, this) == false)
                throw std::invalid_argument{"Test with same name and suite already exists"};
    }

    std::string const & suiteName() const { return suiteName_; }
    std::string const & testName() const { return testName_; }

    template<typename T>
    bool expect(char const * filename, size_t line, char const * exprStr, T const & expr) {
        return expect(filename, line, exprStr, expr, "");
    }

    template<typename T>
    bool expect(char const * filename, size_t line, char const * exprStr, T const & expr, char const * msg) {
        UNUSED(msg);
        ++checks_;
        if (expr) 
            return true;
        // if this is the first failed check, print the test info first
        if (++failed_ == 1)
            std::cout << "Test " << testName_ << " (" << filename_ << ":" << line_ << "):" << std::endl;
        std::cout << "    " << exprStr << " not true at " << filename << ":" << line << std::endl;
        return false;
    }

    template<typename T, typename W> 
    bool expectEq(char const * filename, size_t line, char const * expr, T const & x, W const & y) {
        return expectEq(filename, line, expr, x, y, "");
    }

    template<typename T, typename W> 
    bool expectEq(char const * filename, size_t line, char const * expr, T const & x, W const & y, char const * msg) {
        UNUSED(msg);
        ++checks_;
        if (x == y)
            return true;
        // if this is the first failed check, print the test info first
        if (++failed_ == 1)
            std::cout << "Test " << testName_ << " (" << filename_ << ":" << line_ << "):" << std::endl;
        std::cout << "    " << expr << " not equal, found " << x << ", expected " << y << " at " << filename << ":" << line << std::endl;
        return false;
    }

private:

    std::string suiteName_;
    std::string testName_;
    char const * filename_;
    size_t line_;

    size_t checks_ = 0;
    size_t failed_ = 0;

    virtual void run_() = 0;

    static std::unordered_map<std::string, std::unordered_map<std::string, Tests *>> & tests_() {
        static std::unordered_map<std::string, std::unordered_map<std::string, Tests *>> tests;
        return tests;
    } 

    static bool addTest_(std::string const & suiteName, std::string const & testName, Tests * test) {
        Tests * & t = tests_()[suiteName][testName];
        if (t != nullptr)
            return false;
        t = test;
        return true;
    }

}; // Test

inline int Tests::run(int argc, char * argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    #if (! defined TESTS)
    std::cout << "Target not compiled with -DTESTS. Tests might not be visible." << std::endl;
    #endif

    for (auto const & suite : tests_()) {
        for (auto const & test : suite.second) {
            test.second->run_();
        }
    }

    std::cout << "All done." << std::endl;
    return EXIT_SUCCESS;
}
