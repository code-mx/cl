
#include "curl_get.h"

#include <string>

int main(int argc, char const *argv[])
{
    /* code */
    std::string out;
    //http_get("http://www.baidu.com", out);
    http_get2("http://www.baidu.com", out);
    return 0;
}
