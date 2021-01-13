
#include "curl_get.h"
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

//#include <zip.h>

#include <stdio.h>
#include <curl/curl.h>

#define _CRT_SECURE_NO_WARNINGS

// static bool is_dir(const std::string& dir)
// {
//     struct stat st;
//     ::stat(dir.c_str(), &st);
//     return S_ISDIR(st.st_mode);
// }

// static void walk_directory(const std::string& startdir, const std::string& inputdir, zip_t* zipper)
// {
//     DIR* dp = ::opendir(inputdir.c_str());
//     if (dp == nullptr) {
//         throw std::runtime_error("Failed to open input directory: " + std::string(::strerror(errno)));
//     }

//     struct dirent* dirp;
//     while ((dirp = readdir(dp)) != NULL) {
//         if (dirp->d_name != std::string(".") && dirp->d_name != std::string("..")) {
//             std::string fullname = inputdir + "/" + dirp->d_name;
//             if (is_dir(fullname)) {
//                 if (zip_dir_add(zipper, fullname.substr(startdir.length() + 1).c_str(), ZIP_FL_ENC_UTF_8) < 0) {
//                     throw std::runtime_error("Failed to add directory to zip: " + std::string(zip_strerror(zipper)));
//                 }
//                 walk_directory(startdir, fullname, zipper);
//             }
//             else {
//                 zip_source_t* source = zip_source_file(zipper, fullname.c_str(), 0, 0);
//                 if (source == nullptr) {
//                     throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
//                 }
//                 if (zip_file_add(zipper, fullname.substr(startdir.length() + 1).c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
//                     zip_source_free(source);
//                     throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
//                 }
//             }
//         }
//     }
//     ::closedir(dp);
// }

// static void zip_directory(const std::string& inputdir, const std::string& output_filename)
// {
//     int errorp;
//     zip_t* zipper = zip_open(output_filename.c_str(), ZIP_CREATE | ZIP_EXCL, &errorp);
//     if (zipper == nullptr) {
//         zip_error_t ziperror;
//         zip_error_init_with_code(&ziperror, errorp);
//         throw std::runtime_error("Failed to open output file " + output_filename + ": " + zip_error_strerror(&ziperror));
//     }

//     try {
// 		if (!is_dir(inputdir))
// 		{
// 			zip_source_t* source = zip_source_file(zipper, inputdir.c_str(), 0, 0);
// 			if (source == nullptr) {
// 				throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
// 			}
// 			if (zip_file_add(zipper, boost::filesystem::path("inputdir").filename().string().c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
// 				zip_source_free(source);
// 				throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
// 			}
// 		}
// 		else
// 		{
// 			walk_directory(inputdir, inputdir, zipper);
// 		}
//     }
//     catch (...) {
//         zip_close(zipper);
//         throw;
//     }

//     zip_close(zipper);
// }

//  entity
//  系统（systemName) 想定(scenarioName) 进程(processName) 文件 （file）
bool http_post(const std::string& url
    , const std::string& systemName
    , const std::string& scenarioName
    , const std::string& processName
    , const std::string& file)
{
    CURL* pCurl = NULL;
    CURLcode res;

    struct curl_slist* headerlist = NULL;

    struct curl_httppost* post = NULL;
    struct curl_httppost* last = NULL;

    //系统
    curl_formadd(&post, &last
        , CURLFORM_COPYNAME, "entity.systemName"
        , CURLFORM_COPYCONTENTS, systemName.c_str()
        , CURLFORM_END);

    //想定
    curl_formadd(&post, &last
        , CURLFORM_COPYNAME, "entity.scenarioName"
        , CURLFORM_COPYCONTENTS, scenarioName.c_str()
        , CURLFORM_END);

    //进程
    curl_formadd(&post, &last
        , CURLFORM_COPYNAME, "entity.processName"
        , CURLFORM_COPYCONTENTS, processName.c_str()
        , CURLFORM_END);

    //文件
    curl_formadd(&post, &last
        , CURLFORM_COPYNAME, "file"
        , CURLFORM_FILE, file.c_str()
        , CURLFORM_END);

    ////-----------------------------------------------------------------------//picture1
    //curl_formadd(&post, &last, CURLFORM_COPYNAME, "picture1", //此处表示要传的参数名
    //    CURLFORM_FILE, "1.jpg",                               //此处表示图片文件的路径
    //    //CURLFORM_CONTENTTYPE, "image/jpeg",
    //    CURLFORM_END);
    ////------------------------------------------------------------------------//picture2
    std::string response;
     /* send all data to this function  */
    curl_easy_setopt(&post, CURLOPT_WRITEFUNCTION
        , [](void* ptr, size_t size, size_t nmemb, void* stream)
        ->size_t {
            if (stream == NULL || ptr == NULL || size == 0)
            {
                return 0;
            }

           size_t realsize = size * nmemb;
            std::string * buffer = (std::string*)stream;
            if (buffer != NULL)
            {
                buffer->append((const char*)ptr, realsize);
            }
            return realsize;
            /*
            std::string *str = (std::string*)stream;
            (*str).append((char*)ptr, size*nmemb);
            return size * nmemb;
            */
        });

    curl_easy_setopt(&post, CURLOPT_WRITEDATA, (void*)&response);

    pCurl = curl_easy_init();                           //初始化句柄

    if (NULL != pCurl)
    {
        //curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);
        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, post);

        res = curl_easy_perform(pCurl);
        if (res != CURLE_OK)
        {
            printf("\nres is not ok!\n");                           
            printf("curl_easy_perform() failed，error code is:%s\n", curl_easy_strerror(res));

            return false;
        }

        //  输出返回信息
        std::cout << response << std::endl;

        long httpcode;
        
        if (curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &httpcode) != CURLE_OK)
        {
            std::cout << "curl_easy_getinfo 失败 : " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        if (httpcode > 399)
        {
            std::cout << "curl_easy_getinfo 失败 : " << httpcode << std::endl;
            return false;
        }

        /*
            CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... ); 
            info参数就是我们需要获取的内容，下面是一些参数值:
            1.CURLINFO_RESPONSE_CODE
            获取应答码
            2.CURLINFO_HEADER_SIZE
            头大小
            3.CURLINFO_COOKIELIST
            cookies列表
        */
        printf("\n");

        curl_easy_cleanup(pCurl);

    }

    return true;
}

bool http_get(const std::string& url
    , std::string& response)
{
    CURL* pCurl = NULL;
    CURLcode res;

    pCurl = curl_easy_init();                           //初始化句柄

    if (NULL != pCurl)
    {
        //curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);
        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
        //curl_easy_setopt(pCurl, CURLOPT_HTTPGET, post);
        curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 2);

        /* send all data to this function  */
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION
            , [](void* ptr, size_t size, size_t nmemb, void* stream)
            ->size_t {
            if (stream == NULL || ptr == NULL || size == 0)
            {
                return 0;
            }

            size_t realsize = size * nmemb;
            std::string* buffer = (std::string*)stream;
            if (buffer != NULL)
            {
                buffer->append((const char*)ptr, realsize);
            }
            return realsize;
            /*
            std::string *str = (std::string*)stream;
            (*str).append((char*)ptr, size*nmemb);
            return size * nmemb;
            */
        });

        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&response);

        res = curl_easy_perform(pCurl);
        if (res != CURLE_OK)
        {
            printf("\nres is not ok!\n");                           
            printf("curl_easy_perform() failed，error code is:%s\n", curl_easy_strerror(res));

            return false;
        }

        //  输出返回信息
        std::cout << response << std::endl;

        long httpcode;
        
        if (curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &httpcode) != CURLE_OK)
        {
            std::cout << "curl_easy_getinfo 失败 : " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        if (httpcode > 399)
        {
            std::cout << "curl_easy_getinfo 失败 : " << httpcode << std::endl;
            return false;
        }

        /*
            CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ... ); 
            info参数就是我们需要获取的内容，下面是一些参数值:
            1.CURLINFO_RESPONSE_CODE
            获取应答码
            2.CURLINFO_HEADER_SIZE
            头大小
            3.CURLINFO_COOKIELIST
            cookies列表
        */
        printf("\n");

        curl_easy_cleanup(pCurl);

    }

    return true;
}


static size_t downloadCallback(void* buffer, size_t sz, size_t nmemb, void* writer)
{
    std::string* psResponse = (std::string*)writer;
    size_t size = sz * nmemb;
    psResponse->append((char*)buffer, size);

    return sz * nmemb;
}

bool http_get2(const std::string& strUrl
    , std::string& strTmpStr)
{
    //     string strUrl = "http://www.baidu.com";
    //     string strTmpStr;
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strTmpStr);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    std::string strRsp;
    if (res != CURLE_OK)
    {
        strRsp = "error";
    }
    else
    {
        strRsp = strTmpStr;
    }

    printf("strRsp is |%s|\n", strRsp.c_str());

    return res == CURLE_OK;

}