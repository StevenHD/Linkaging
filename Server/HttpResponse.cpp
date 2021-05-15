//
// Created by hlhd on 2021/5/14.
//

#include "HttpResponse.h"
#include "Buffer.h"

namespace Linkaging
{

const std::map<int, std::string> HttpResponse::statusCode2Msg =
{
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"}
};

const std::map<std::string, std::string> HttpResponse::suffix2Type =
{
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"}
};

Buffer HttpResponse::makeResponse()
{
    Buffer output;
    if (m_statusCode == 400)
    {
        doErrorResponse(output, "Linkaging cannot parse the msg");
        return output;
    }

    struct stat stBuf;
    if (::stat(m_resourcePath.data(), &stBuf) < 0) // "Not Found"
    {
        m_statusCode = 404;
        doErrorResponse(output, "Linkaging cannot find the file");
        return output;
    }

    if (!(S_ISREG(stBuf.st_mode) || !(S_IRUSR & stBuf.st_mode)))
    {
        m_statusCode = 403;
        doErrorResponse(output, "Linkaging cannot read the file");
        return output;
    }

    doStaticRequest(output, stBuf.st_size);
    return output;
}

void HttpResponse::doStaticRequest(Buffer &output, long fileSize)
{
    assert(fileSize >= 0);
    auto itr = statusCode2Msg.find(m_statusCode);
    if (itr == statusCode2Msg.end())
    {
        m_statusCode = 400; // ?
        doErrorResponse(output, "Bad Request");
        return;
    }

    output.append("HTTP/1.1 "+ std::to_string(m_statusCode) + " " + itr->second + "\r\n");
    if (m_isKeepAlive)
    {
        output.append("Connection: Keep-Alive\r\n");
        output.append("Keep-Alive: timeout = " + std::to_string(CONNECT_TIMEOUT) + "\r\n");
    }
    else
    {
        output.append("Connection: close\r\n");
    }

    output.append("Content-type: " + getFileType() + "\r\n");
    output.append("Content-length: " + std::to_string(fileSize) + "\r\n");
    output.append("Server: Linkaging\r\n");
    output.append("\r\n");

    int srcFd = ::open(m_resourcePath.data(), O_RDONLY, 0);
    void* mmapRet = ::mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, srcFd, 0);
    ::close(srcFd);
    if (mmapRet == (void*)-1)
    {
        munmap(mmapRet, fileSize);
        output.retrieveAll();
        m_statusCode = 404;
        doErrorResponse(output, "Linkaging cannot find the file");
        return;
    }

    char* srcAddr = static_cast<char*>(mmapRet);
    output.append(srcAddr, fileSize);
    munmap(srcAddr, fileSize);
}

std::string HttpResponse::getFileType()
{
    int idx = m_resourcePath.find_last_of('.');

    std::string suffix;
    if (idx == std::string::npos) return "text/plain";

    suffix = m_resourcePath.substr(idx);
    auto itr = suffix2Type.find(suffix);
    if (itr == suffix2Type.end()) return "text/plain";

    return itr->second;
}

void HttpResponse::doErrorResponse(Buffer &output, std::string msg)
{
    std::string pgBody;
    auto itr = statusCode2Msg.find(m_statusCode);
    if (itr == statusCode2Msg.end()) return;

    pgBody += "<html><title>Linkaging Error</title>";
    pgBody += "<body bgcolor=\"ffffff\">";
    pgBody += std::to_string(m_statusCode) + " : " + itr->second + "\n";
    pgBody += "<p>" + msg + "</p>";
    pgBody += "<hr><em>Linkaging Web Server</em></body><html>";

    output.append("HTTP/1.1 "+ std::to_string(m_statusCode) + " " + itr->second + "\r\n");
    output.append("Server: Linkaging\r\n");
    output.append("Content-type: text/html\r\n");
    output.append("Connection: close\r\n");
    output.append("Content-length: " + std::to_string(pgBody.size()) + "\r\n\r\n");
    output.append(pgBody);
}

}