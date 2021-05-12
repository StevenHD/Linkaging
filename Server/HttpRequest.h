//
// Created by hlhd on 2021/4/27.
//

#ifndef MODERNCPP_HTTPREQUEST_H
#define MODERNCPP_HTTPREQUEST_H

#include "../all.h"
#include "Buffer.h"

namespace Linkaging {
#define STATIC_ROOT "../www"

    class Timer;

    class HttpRequest {
    public:
        //-----------------报文解析状态-----------------
        enum HttpRequestParseState {
            RequestLine,
            RequestHeader,
            RequestBody,
            GetAll
        };

        //-----------------Http方法-----------------
        enum HttpMethod {
            Invalid,
            Get,
            Post,
            Head,
            Put,
            Delete
        };

        //-----------------Http方法-----------------
        enum HttpVersion {
            Unknown,
            Http1_0,
            Http1_1
        };

    private:
        //-----------------Http方法-----------------
        int m_fd;            // 文件描述符
        Buffer m_inBuff;     // 读缓冲区
        Buffer m_outBuff;    // 写缓冲区
        bool m_isWorking;    // 若正在工作，则不能被超时事件断开连接

        // 定时器相关
        Timer *m_timer;

        // 报文解析相关
        HttpRequestParseState m_state;       // 报文解析状态
        HttpMethod m_method;                 // HTTP方法
        HttpVersion m_version;               // HTTP版本
        std::string m_path;                  // URL路径
        std::string m_queryPara;             // URL参数
        std::map<std::string, std::string> m_header; // 报文头部

    public:
        HttpRequest(int fd);

        ~HttpRequest();

        int getFd() {
            // 返回文件描述符
            return m_fd;
        }

        int readData(int *savedErrno); // 读数据
        int writeData(int *savedErrno); // 写数据

        void appendOutBuffer(const Buffer &buf) {
            m_outBuff.append(buf);
        }

        int writableBytes() {
            return m_outBuff.readableBytes();
        }

        void setTimer(Timer *timer) {
            m_timer = timer;
        }

        Timer *getTimer() {
            return m_timer;
        }

        void setWorking() {
            m_isWorking = true;
        }

        void setNoWorking() {
            m_isWorking = false;
        }

        bool isWorking() const {
            return m_isWorking;
        }

        bool parseRequest(); // 解析Http报文
        bool parseFinish() {
            // 是否解析完一个报文
            return m_state == GetAll;
        }

        void resetParse(); // 重置解析状态

        std::string getPath() const {
            return m_path;
        }

        std::string getQuery() const {
            return m_queryPara;
        }

        std::string getHeader(const std::string &field) const;

        std::string getMethod() const;

        bool keepAlive() const; // 是否长连接

    private:
        // 解析请求行
        bool parseRequestLine(const char *begin, const char *end);

        // 设置HTTP方法
        bool setHttpMethod(const char *start, const char *end);

        // 设置URL路径
        void setPath(const char *begin, const char *end) {
            std::string subPath;
            subPath.assign(begin, end);
            if (subPath == "/")
                subPath = "/index.html";
            m_path = STATIC_ROOT + subPath;
        }

        // 设置URL参数
        void setQueryParameter(const char *begin, const char *end) {
            m_queryPara.assign(begin, end);
        }

        // 设置HTTP版本
        void setHttpVersion(HttpVersion version) {
            m_version = version;
        }

        // 增加报文头
        void addHeader(const char *start, const char *colon, const char *end);
    };
}

#endif //MODERNCPP_HTTPREQUEST_H
