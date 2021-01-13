#pragma once

#include <string>

bool http_get(const std::string& url
    , std::string& response);

bool http_get2(const std::string& strUrl
    , std::string& strTmpStr);