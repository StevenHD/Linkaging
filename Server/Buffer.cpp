//
// Created by hlhd on 2021/4/27.
//

#include "Buffer.h"

namespace Linkaging
{

ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    /* 保证一次读到足够多的数据 */
    char extraBuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();

    vec[0].iov_base = getBufferBegin() + m_writeIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    const ssize_t n = ::readv(fd, vec, 2);

    if (n < 0)
    {
        printf("[Buffer:readFd]fd = %d readv : %s\n", fd, strerror(errno));
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        m_writeIndex += n;
    }
    else
    {
        m_writeIndex = m_buffer.size();
        append(extraBuf, n - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int *savedErrno)
{
    size_t nLeft = readableBytes();
    char *bufPtr = getBufferBegin() + m_readIndex;

    ssize_t n;

    if ((n = ::write(fd, bufPtr, nLeft)) <= 0)
    {
        if (n < 0 && n == EINTR) return 0;
        else
        {
            printf("[Buffer:writeFd]fd = %d write : %s\n", fd, strerror(errno));
            *savedErrno = errno;
            return -1;
        }
    }
    else
    {
        m_readIndex += n;
        return n;
    }
}

}