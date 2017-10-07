#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include "Shlwapi.h"
#else
#include <experimental/filesystem>
#endif

std::string get_working_path() {
    char temp[1000];
    return ( getcwd(temp, 1000) ? std::string( temp ) : std::string("") );
}

std::string escape_string(const std::string &input) {
    std::ostringstream ss;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        switch (*iter) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

std::string remove_carrige_return(const std::string &input) {
    std::ostringstream ss;

    for (auto iter = input.cbegin(); iter != input.cend(); iter++)
        if(*iter != '\r') ss << *iter;
    
    return ss.str();
}

#include "../src/lib.cpp"
#include "lexer_test.cpp"
#include "ast_test.cpp"
#include "parser_test.cpp"
#include "irgen_test.cpp"