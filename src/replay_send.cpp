// replay_send.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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

#include <zip.h>

#include <stdio.h>
#include <curl/curl.h>

#define _CRT_SECURE_NO_WARNINGS

static bool is_dir(const std::string& dir)
{
    struct stat st;
    ::stat(dir.c_str(), &st);
    return S_ISDIR(st.st_mode);
}

static void walk_directory(const std::string& startdir, const std::string& inputdir, zip_t* zipper)
{
    DIR* dp = ::opendir(inputdir.c_str());
    if (dp == nullptr) {
        throw std::runtime_error("Failed to open input directory: " + std::string(::strerror(errno)));
    }

    struct dirent* dirp;
    while ((dirp = readdir(dp)) != NULL) {
        if (dirp->d_name != std::string(".") && dirp->d_name != std::string("..")) {
            std::string fullname = inputdir + "/" + dirp->d_name;
            if (is_dir(fullname)) {
                if (zip_dir_add(zipper, fullname.substr(startdir.length() + 1).c_str(), ZIP_FL_ENC_UTF_8) < 0) {
                    throw std::runtime_error("Failed to add directory to zip: " + std::string(zip_strerror(zipper)));
                }
                walk_directory(startdir, fullname, zipper);
            }
            else {
                zip_source_t* source = zip_source_file(zipper, fullname.c_str(), 0, 0);
                if (source == nullptr) {
                    throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
                }
                if (zip_file_add(zipper, fullname.substr(startdir.length() + 1).c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
                    zip_source_free(source);
                    throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
                }
            }
        }
    }
    ::closedir(dp);
}

static void zip_directory(const std::string& inputdir, const std::string& output_filename)
{
    int errorp;
    zip_t* zipper = zip_open(output_filename.c_str(), ZIP_CREATE | ZIP_EXCL, &errorp);
    if (zipper == nullptr) {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, errorp);
        throw std::runtime_error("Failed to open output file " + output_filename + ": " + zip_error_strerror(&ziperror));
    }

    try {
		if (!is_dir(inputdir))
		{
			zip_source_t* source = zip_source_file(zipper, inputdir.c_str(), 0, 0);
			if (source == nullptr) {
				throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
			}
			if (zip_file_add(zipper, boost::filesystem::path("inputdir").filename().string().c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
				zip_source_free(source);
				throw std::runtime_error("Failed to add file to zip: " + std::string(zip_strerror(zipper)));
			}
		}
		else
		{
			walk_directory(inputdir, inputdir, zipper);
		}
    }
    catch (...) {
        zip_close(zipper);
        throw;
    }

    zip_close(zipper);
}

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


int main(int argc, char const* argv[])
{
    //zip_directory("D:\\_test\\C#CEF", "D:\\_test\\C.zip");
    //std::cout << "Hello World!\n";

    //boost::shared_ptr<boost::program_options::options_description> opt = boost::make_shared<boost::program_options::options_description>("");

    //opt->add_options()
    //    ("codepage,c", boost::program_options::value<std::string>()->default_value("GBK"), "设置客户端代码页");

    //步骤一: 构造选项描述器和选项存储器
    //选项描述器,其参数为该描述器的名字
    boost::program_options::options_description opts("all options");
    //选项存储器,继承自map容器
    boost::program_options::variables_map vm;

    //步骤二: 为选项描述器增加选项
    //其参数依次为: key, value的类型，该选项的描述
    opts.add_options()
        ("file,f", boost::program_options::value<std::string>(), "复盘文件（目录）完整路径")
        ("url,u", boost::program_options::value<std::string>()->default_value("http://10.171.136.185:8080/currentbus-dataaccess/dataaccess/user/processfile/fileUpload"), "推送URL")
        ("system,s", boost::program_options::value<std::string>()->default_value("sosim"), "系统")
        ("scenario,i", boost::program_options::value<std::string>(), "想定")
        ("process,p", boost::program_options::value<std::string>()->default_value("default"), "进程")
        ("compress,c", boost::program_options::value<bool>(), "是否压缩（对于文件是否启用压缩，对于目录则总是压缩）")
        ("help,h", "显示本帮助信息");

    //  位置猜测器
    boost::program_options::positional_options_description pod;
    pod.add("file", 1);
    pod.add("url", 2);

    //步骤三: 先对命令行输入的参数做解析,而后将其存入选项存储器
    //如果输入了未定义的选项，程序会抛出异常，所以对解析代码要用try-catch块包围
    try {
        //parse_command_line()对输入的选项做解析
        //store()将解析后的结果存入选项存储器
        //boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts
        //, boost::program_options::command_line_style::unix_style 
        //    | boost::program_options::command_line_style::case_insensitive
        //    , [](const std::string& s)->std::pair<std::string, std::string> {
        //        std::cout << "ext str = " << s << std::endl;

        //        return { "", "" };
        //    }), vm);
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
            .options(opts).positional(pod).run(), vm);
        boost::program_options::notify(vm);
    }
    catch (const boost::system::error_code& ec)
    {
        std::cout << "参数不正确:" << ec.message() << std::endl;
        return -1;
    }
    catch (...) {
        std::cout << "输入的参数中存在未定义的选项！\n";
        return -1;
    }

    //步骤四: 参数解析完毕，处理实际信息
    //count()检测该选项是否被输入
    if (vm.count("help"))
    {
        std::cout << opts << std::endl;

        return 0;
    }

    if (vm.empty())
    {
        std::cout << opts << std::endl;

        return 0;
    }

    if (!vm.count("file"))
    {
        std::cout << "请输出 file 参数" << std::endl;
        return -1;
    }

    //  源文件
    boost::filesystem::path replay_dir = vm["file"].as<std::string>();

    //  临时文件，这里利用智能指针，析构的时候会自动删除临时文件
    //boost::filesystem::path tmp_path(boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.tmp"));
    boost::shared_ptr<boost::filesystem::path> tmp_path(new boost::filesystem::path(
        boost::filesystem::path("tmp")
        / boost::filesystem::unique_path("%%%%_%%%%_%%%%_%%%%.tmp")),
        [](boost::filesystem::path* p) {

            //  删除临时文件
            boost::system::error_code ec;
            boost::filesystem::remove(*p, ec);
            std::cout << "删除临时文件失败:" << ec.message() << std::endl;

            delete p;
        });

	//	临时目录
	boost::system::error_code ec;
	boost::filesystem::create_directory("tmp", ec);

    //  压缩
    if ((vm.count("compress") == false
		|| vm["compress"].as<bool>() == false)
        && boost::filesystem::is_directory(replay_dir) == false)
    {
        //  文件，且没有启用压缩
        boost::system::error_code ec;
        boost::filesystem::copy_file(replay_dir, *tmp_path, ec);
        if (ec)
        {
            std::cout << "拷贝文件失败:" << ec.message() << std::endl;
            return -1;
        }
    }
    else
    {
        try
        {
            zip_directory(replay_dir.string().c_str(), tmp_path->string().c_str());
        }
        catch (const std::exception& ec)   //  std::runtime_error
        {
            std::cout << "压缩文件失败:" << ec.what() << std::endl;
        }
    }
        

    //  上传
    std::string url = vm["url"].as<std::string>();
    std::string systemName = vm["system"].as<std::string>();
    std::string scenarioName = replay_dir.filename().string();
    if (vm.count("scenario"))
    {
        scenarioName = vm["scenario"].as<std::string>();
    }
    std::string processName = vm["process"].as<std::string>();

    if (http_post(url, systemName, scenarioName, processName, tmp_path->string().c_str()) == true)
    {
        std::cout << "上传成功" << std::endl;
    }
    else
    {
        std::cout << "上传失败" << std::endl;
    }

    //getch();
#ifdef _DEBUG
	system("pause");
#endif

    return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
