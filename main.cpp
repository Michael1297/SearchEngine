#include <iostream>
#include <cpr/cpr.h>
#include "nlohmann/json.hpp"
#include "gumbo.h"


int main() {
    cpr::Response r = cpr::Get(cpr::Url("http://httpbin.org"));
    
}