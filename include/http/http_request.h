// http 请求, 使用正则表达式
#pragma once

#include <unordered_map> 
#include <unordered_set>
#include <string> 
#include <regex>            // 正则表达式库
#include <errno.h> 
#include <mysql/mysql.h>

#include "buffer.h"
#include "log.h"
#include "sql_conn_pool.h"
#include "sql_conn_RAII.h"


class HttpRequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,          // 请求行 
        HEADERS,               // 报文头
        BODY,                  // body
        FINISH                 // 状态完成
    };

    enum HTTP_CODE{
        NO_REQUEST,            // 没有请求
        GET_REQUEST,           // 得到请求
        BAD_REQUEST,           // 请求错误
        NO_RESOURCE,           // 没有找到请求的资源
        FORBIDDENT_REQUEST,    // 拒绝请求
        FILE_REQUEST,          // 文件请求
        INTERNAL_ERROR,        // 内部错误
        CLOSED_CONNECION       // 关闭连接
    };
    HttpRequest(){Init();}
    ~HttpRequest() = default;

    void Init();
    bool Parse(Buffer& buff);
    
    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;

    bool IsKeepAlive() const;                          // 是否是长连接 

private:
    bool ParseRequestLine_(const std::string& line);   // 解析请求行 
    void ParseHeader_(const std::string& line);        // 解析头部
    void ParseBody_(const std::string& line);          // 解析body

    void ParsePath_();                                 // 解析地址
    void ParsePost_();                                 // 解析post
    void ParseFromUrlencoded_();                       // 解析POST 提交的数据

    
    static bool UserVerify(                            // 验证用户
        const std::string& name,
        const std::string& pwd,
        bool is_login
    );
 
    PARSE_STATE state_;                                    // 当前解析状态               
    std::string method_, path_, version_, body_;           // 方法，路径，版本，body
    std::unordered_map<std::string,std::string> header_;   // 请求头键值对
    std::unordered_map<std::string,std::string> post_;     // 主机用户名与密码

    static const std::unordered_set<std::string> DEFAULT_HTML;          // 默认html
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG; // 上面的tag
    static int ConverHex(char ch);                                      // 转换16进制成一个int
};





