#include <HttpTool.h>
#include "Exception.h"

using namespace std::regex_constants;       // для флага icase - игнорирование регистра в ссылке

HttpTool::HttpTool(std::string link){
    std::regex has_http("^https?://\\S+");
    if(std::regex_match(link, has_http)){       //убирает http://
        link.erase(link.begin(), link.begin() + link.find("//") + 2);
    }
    if(link.empty()) throw InvalidLinkException();

    std::regex has_www("^www\\.\\S+");
    if(std::regex_match(link, has_www)){       //убирает www.
        link.erase(link.begin(), link.begin() + 4);
    }
    if(link.empty()) throw InvalidLinkException();

    if(auto find = link.find('/'); find != std::string::npos){
        link.resize(find);       //убирает буквы после /
    }
    if(link.empty()) throw InvalidLinkException();

    domain = link;
    for(int i = 1; i < link.size() - 1; i++){
        if(link[i] == '.') link.insert(link.begin() + i++, '\\');       //экранирование '.' в link для применения в regex
    }
    own_link_regex = std::regex("(^https?://|^)(www\\.|)" + link + "(/\\S*$|$)", icase);
}

//проверка - ссылка принадлежит домену
bool HttpTool::is_ownLink(std::string& link) {
    if(std::regex_match(link, own_link_regex)){
        return true;
    } else if(link.front() == '/'){     //ссылка без домена, начинающиеся с /
        link = "http://" + domain + link;
        return true;
    } else{
        return false;
    }
}

std::string HttpTool::getDomain(){
    return domain;
}
