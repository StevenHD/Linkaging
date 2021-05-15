//
// Created by hlhd on 2021/4/27.
//

#include "HttpRequest.h"

namespace Linkaging
{

HttpRequest::HttpRequest(int fd)
        : m_fd(fd), m_isWorking(false), m_timer(nullptr),
          m_state(RequestLine), m_method(Invalid), m_version(Unknown)
{
    assert(m_fd >= 0);
}

HttpRequest::~HttpRequest() {
    close(m_fd);
}

int HttpRequest::readData(int *savedErrno)
{
    int ret = m_inBuff.readFd(m_fd, savedErrno);
    return ret;
}

int HttpRequest::writeData(int *savedErrno)
{
    int ret = m_outBuff.writeFd(m_fd, savedErrno);
    return ret;
}

bool HttpRequest::parseRequest()
{
    bool ok = true;
    bool hasMore = true;

    while (hasMore)
    {
        if (m_state == RequestLine) {
            // 处理请求行
            const char *crlf = m_inBuff.findCRLF();
            if (crlf)
            {
                ok = parseRequestLine(m_inBuff.peek(), crlf);
                if (ok)
                {
                    m_inBuff.retrieveUntil(crlf + 2);
                    m_state = RequestHeader;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == RequestHeader) {
            // 处理报文头
            const char *crlf = m_inBuff.findCRLF();
            if (crlf)
            {
                const char *colon = std::find(m_inBuff.peek(), crlf, ':');
                if (colon != crlf)
                {
                    addHeader(m_inBuff.peek(), colon, crlf);
                }
                else
                {
                    m_state = GetAll;
                    hasMore = false;
                }

                m_inBuff.retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == RequestBody)
        {
            // TODO 处理报文体
        }
    }

    return ok;
}

bool HttpRequest::parseRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && setHttpMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char *question = std::find(start, space, '?');
            if (question != space)
            {
                setPath(start, question);
                setQueryParameter(question, space);
            }
            else
            {
                setPath(start, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");

            if (succeed)
            {
                if (*(end - 1) == '1')
                {
                    setHttpVersion(Http1_1);
                }
                else if (*(end - 1) == '0')
                {
                    setHttpVersion(Http1_0);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }

    return succeed;
}

bool HttpRequest::setHttpMethod(const char *start, const char *end)
{
    std::string m(start, end);

    if (m == "GET") m_method = Get;
    else if (m == "POST") m_method = Post;
    else if (m == "HEAD") m_method = Head;
    else if (m == "PUT") m_method = Put;
    else if (m == "DELETE") m_method = Delete;
    else m_method = Invalid;

    return m_method != Invalid;
}

void HttpRequest::addHeader(const char *start, const char *colon, const char *end) {
    std::string field(start, colon);
    ++colon;

    while (colon < end && *colon == ' ') ++colon;

    std::string value(colon, end);
    while (!value.empty() && value[value.size() - 1] == ' ') {
        value.resize(value.size() - 1);
    }

    m_header[field] = value;
}

std::string HttpRequest::getMethod() const {
    std::string res;

    if (m_method == Get) res = "GET";
    else if (m_method == Post) res = "POST";
    else if (m_method == Head) res = "HEAD";
    else if (m_method == Put) res = "Put";
    else if (m_method == Delete) res = "DELETE";

    return res;
}

std::string HttpRequest::getHeader(const std::string &field) const {
    std::string res;

    auto itr = m_header.find(field);
    if (itr != m_header.end()) {
        res = itr->second;
    }

    return res;
}

bool HttpRequest::keepAlive() const {
    std::string connection = getHeader("Connection");
    bool res = connection == "Keep-Alive" ||
               (m_version == Http1_1 && connection != "close");

    return res;
}

void HttpRequest::resetParse() {
    m_state = RequestLine; // 报文解析状态
    m_method = Invalid; // HTTP方法
    m_version = Unknown; // HTTP版本
    m_path = ""; // URL路径
    m_queryPara = ""; // URL参数
    m_header.clear(); // 报文头部
}

}