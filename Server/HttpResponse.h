//
// Created by hlhd on 2021/5/14.
//

#ifndef LINKAGING_HTTPRESPONSE_H
#define LINKAGING_HTTPRESPONSE_H

#include "../all.h"

#define CONNECT_TIMEOUT 500 // de-active connection will break over 500ms

namespace Linkaging
{

class Buffer;

class HttpResponse
{
public:
    HttpResponse(int statusCode, std::string path, bool keepAlive)
        : m_statusCode(statusCode), m_resourcePath(path), m_isKeepAlive(keepAlive)
    {}

    ~HttpResponse() {}

    Buffer makeResponse();

    void doErrorResponse(Buffer& output, std::string msg);
    void doStaticRequest(Buffer& output, long fileSize);

private:
    std::string getFileType();

public:
    static const std::map<int, std::string> statusCode2Msg;
    static const std::map<std::string, std::string> suffix2Type;

private:
    std::map<std::string, std::string> m_headers;
    std::string m_resourcePath;
    int m_statusCode;
    bool m_isKeepAlive;
};

}

#endif //LINKAGING_HTTPRESPONSE_H
