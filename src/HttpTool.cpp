#include <HttpTool.h>
#include "Exception.h"

using namespace std::regex_constants;       // для флага icase - игнорирование регистра в ссылке

HttpTool::HttpTool(std::string link){
    std::regex has_http("^https?://\\S+");
    if(std::regex_match(link, has_http)){       //убирает http://
        link.erase(link.begin(), link.begin() + link.find("//") + 2);
    }
    if(link.empty()) throw Exception("Invalid link");

    std::regex has_www("^www\\.\\S+");
    if(std::regex_match(link, has_www)){       //убирает www.
        link.erase(link.begin(), link.begin() + 4);
    }
    if(link.empty()) throw Exception("Invalid link");

    if(auto find = link.find('/'); find != std::string::npos){
        link.resize(find);       //убирает буквы после /
    }
    if(link.empty()) throw Exception("Invalid link");

    domain = link;
    HttpTool::escape(link, '.');    //экранирование '.' в link для применения в regex
    own_link_regex = std::regex("(^https?://|^)(www\\.|)" + link + "(/\\S*$|$)", icase);
}

std::string HttpTool::getPath(std::string link) {
    if(link.front() != '/') link.erase(link.begin(), link.begin() + link.find(domain) + domain.size());
    if(link.empty()) link = "/";
    return link;
}

void HttpTool::escape(std::string& text, char symbol) { //экранирование
    for(int i = 0; i < text.size(); i++){
        if(text[i] == symbol) text.insert(text.begin() + i++, '\\');       //экранирование symbol в text
    }
}

//проверка - ссылка принадлежит домену
bool HttpTool::is_ownLink(std::string& link) {
    if(std::regex_match(link, own_link_regex)){
        return true;
    } else if(link.front() == '/'){     //ссылка без домена, начинающиеся с /
        link = "https://" + domain + link;
        return true;
    } else if(!std::regex_match(link, has_domain)){     //ссылка без домена, не начинающиеся с /
        link = "https://" + domain + "/" + link;
        return true;
    } else{
        return false;
    }
}

std::string HttpTool::getDomain(){
    return domain;
}
