#include <iostream>
#include <cpr/cpr.h>
#include "nlohmann/json.hpp"
#include "gumbo.h"

int main() {
    cpr::Response r = cpr::Get(cpr::Url("http://httpbin.org"));
    auto* parse = gumbo_parse(r.text.c_str());

    std::cout << r.text;

}