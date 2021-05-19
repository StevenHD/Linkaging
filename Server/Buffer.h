//
// Created by hlhd on 2021/4/27.
//


#ifndef MODERNCPP_BUFFER_H
#define MODERNCPP_BUFFER_H

#include "../all.h"

#define INIT_SIZE 1024

namespace Linkaging
{

class Buffer
{
private:
    std::vector<char> m_buffer;
    size_t m_readIndex;
    size_t m_writeIndex;

public:
    Buffer() : m_buffer(INIT_SIZE), m_readIndex(0), m_writeIndex(0)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == INIT_SIZE);
    }

    ~Buffer() {}

    size_t readableBytes() const // 可读字节数
    {
        return m_writeIndex - m_readIndex;      // ??不懂为啥溯writeIdx - readIdx
    }

    size_t writableBytes() const // 可写字节数
    {
        return m_buffer.size() - m_writeIndex;
    }

    size_t prependableBytes() const // m_readerIdx前面的空闲缓冲区大小
    {
        return m_readIndex - 0;
    }

    const char *peek() const // 第一个可读位置
    {
        return getBufferBegin() + m_readIndex;
    }

    void retrieve(size_t len) // 取出len个字节
    {
        assert(len <= readableBytes());
        m_readIndex += len;
    }

    void retrieveUntil(const char *end) // 取出数据直到end
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll() // 取出buffer内全部数据
    {
        // ?
        m_readIndex = 0;
        m_writeIndex = 0;
    }

    std::string retrieveAsString() // 以string形式取出全部数据
    {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string &str) // 插入数据
    {
        append(str.data(), str.length());
    }

    void append(const char *data, size_t len) // 插入数据
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len) // 插入数据
    {
        append(static_cast<const char *>(data), len);
    }

    void append(const Buffer &otherBuff) // 把其它缓冲区的数据添加到本缓冲区
    {
        append(otherBuff.peek(), otherBuff.readableBytes());
    }

    void ensureWritableBytes(size_t len) // 确保缓冲区有足够空间
    {
        if (writableBytes() < len)
        {
            makeBufferSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char *beginWrite() // 可写char指针
    {
        return getBufferBegin() + m_writeIndex;
    }

    const char *beginWrite() const
    {
        return getBufferBegin() + m_writeIndex;
    }

    void hasWritten(size_t len) // 写入数据后移动writerIndex_
    {
        m_writeIndex += len;
    }

    ssize_t readFd(int fd, int *savedErrno); // 从套接字读到缓冲区
    ssize_t writeFd(int fd, int *savedErrno); // 缓冲区写到套接字

    const char *findCRLF() const
    {
        const char CRLF[] = "\r\n";
        const char *crlf = std::search(peek(), beginWrite(), CRLF, CRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    const char *findCRLF(const char *start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char CRLF[] = "\r\n";
        const char *crlf = std::search(start, beginWrite(), CRLF, CRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

private:
    char *getBufferBegin()              // 返回缓冲区头指针
    {
        return &*m_buffer.begin();
    }

    const char *getBufferBegin() const  // 返回缓冲区头指针
    {
        return &*m_buffer.begin();
    }

    void makeBufferSpace(size_t len) // 确保缓冲区有足够空间
    {
        if (writableBytes() + prependableBytes() < len)
        {
            m_buffer.resize(m_writeIndex + len);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(getBufferBegin() + m_readIndex,
                      getBufferBegin() + m_writeIndex,
                      getBufferBegin());

            m_readIndex = 0;
            m_writeIndex = m_readIndex + readable;
            assert(readable == readableBytes());
        }
    }
};

}

#endif //MODERNCPP_BUFFER_H
