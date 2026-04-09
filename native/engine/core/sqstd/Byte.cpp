#include "Byte.h"
#include "../base/config.h"

using namespace sqstd;

Byte::Byte(unsigned int nSize, unsigned int nAdjustSize)
{
    m_bNeedDelBuffer = true;
    m_pBuffer = NULL;
    m_nBufSize = 0;
    m_nDataSize = 0;
    m_nAdjustSize = nAdjustSize;
    m_nReadPos = 0;
    if (nSize > 0)
    {
        expand(nSize);
    }
}
Byte::Byte(char *buffer, int nsize)
{
    m_bNeedDelBuffer = false;
    m_pBuffer = buffer;
    m_nReadPos = 0;
    m_nDataSize = 0;
    m_nBufSize = nsize;
    m_nAdjustSize = 0;
}

Byte::Byte(char *buffer, int nsize, bool bNeedDelBuffer)
{
    m_bNeedDelBuffer = bNeedDelBuffer;
    m_pBuffer = buffer;
    m_nReadPos = 0;
    m_nDataSize = 0;
    m_nBufSize = nsize;
    m_nAdjustSize = 0;
}
Byte::Byte()
{
    m_bNeedDelBuffer = true;
    m_pBuffer = NULL;
    m_nReadPos = 0;
    m_nBufSize = 0;
    m_nDataSize = 0;
    m_nAdjustSize = 0;
    m_bAlign = false;
}
Byte::~Byte()
{
    if (m_bNeedDelBuffer && m_pBuffer)
    {
        // printf("destroy Byte %p \n",m_pBuffer);
        delete[] m_pBuffer;
        m_pBuffer = 0;
    }
}
void Byte::setBufferExpandStep(int nAdjustSize)
{
    m_nAdjustSize = nAdjustSize;
}
void Byte::setAlign(bool bAlign)
{
    m_bAlign = bAlign;
}

char *Byte::getBuffer()
{
    return m_pBuffer;
}

char *Byte::getReadPtr()
{
    if (m_nReadPos >= m_nDataSize)
        return 0;
    else
        return m_pBuffer + m_nReadPos;
}

unsigned int &Byte::getCurrentPos()
{
    return m_nReadPos;
}

unsigned int Byte::getDataSize()
{
    return m_nDataSize;
}

unsigned int Byte::getBuffSize()
{
    return m_nBufSize;
}

void Byte::setBufferSize(int nBufferSize)
{
    m_nBufSize = nBufferSize;
}

char *Byte::readBuffer(int nLength)
{
    char *pRet = m_pBuffer + m_nReadPos;
    m_nReadPos += nLength;
    return pRet;
}

char *Byte::readBufferAlign(int nLength)
{
    nLength = alignValue(nLength);
    char *pRet = m_pBuffer + m_nReadPos;
    m_nReadPos += nLength;
    return pRet;
}

int Byte::poppUTFStringLength()
{
    unsigned short &len = *popp<unsigned short>();
    return len + 1;
}

void Byte::popUTFString(std::vector<unsigned int> &strs)
{
    unsigned int length = m_nReadPos + *popp<unsigned short>();
    unsigned char *c, *c2, *c3;

    // printf("================pop %d \n",length);

    while (m_nReadPos < length)
    {
        c = popp<unsigned char>();

        // printf("unicode %u \n", *c);

        if ((*c) < 0x80) // 128
        {
            if (*c != 0)
            {
                strs.push_back(*c);
            }
        }
        else if ((*c) < 0xE0) // 224
        {
            int cc = (char)((*c & 0x3F) << 6) | (*popp<char>() & 0x7F);
            strs.push_back(cc);
        }
        else if ((*c) < 0xF0) // 240
        {

            c2 = popp<unsigned char>();
            // printf("=================== %d %d\n",*c,*c2);
            int cc = ((*c & 0x1F) << 12) | ((*c2 & 0x7F) << 6) | (*popp<unsigned char>() & 0x7F);
            strs.push_back(cc);
        }
        else
        {
            c2 = popp<unsigned char>();
            c3 = popp<unsigned char>();
            int _code = ((*c & 0x0F) << 18) | ((*c2 & 0x7F) << 12) | ((*c3 & 0x7F) << 6) | (*popp<unsigned char>() & 0x7F);
            if (_code >= 0x10000)
            {
                strs.push_back(_code);
            }
            else
            {
                strs.push_back(_code);
            }
        }
    }
}

void Byte::popUTFString(std::vector<std::string> &strs)
{
    unsigned int length = m_nReadPos + *popp<unsigned short>();
    unsigned char *c, *c2, *c3;
    while (m_nReadPos < length)
    {
        c = popp<unsigned char>();
        if ((*c) < 0x80) // 128
        {
            // 小于127，表示为UTF8 1个字节的编码
            if (*c != 0)
            {
                strs.push_back((char *)c);
            }
        }
        else if ((*c) < 0xE0) // 224
        {
            // 2字节第一个字节最大值只能为：11011111 = 223
            // 所以小于224表示它是一个UTF8 2字节编码

            // 接下来取第二个字节
            //  strs[n++] = (   (*c & 0x3F) << 6) | (*popp<char>() & 0x7F);
            int cc = (unsigned char)((*c & 0x3F) << 6) | (*popp<unsigned char>() & 0x7F);
            strs.push_back((char *)(cc));
        }
        else if ((*c) < 0xF0) // 240
        {
            // 3字节第一个字节最大值只能为：11101111 = 239
            // 所以小于240表示为UTF8 3 字节编码

            c2 = popp<unsigned char>();
            int cc = ((*c & 0x1F) << 12) | ((*c2 & 0x7F) << 6) | (*popp<char>() & 0x7F);
            strs.push_back((char *)(cc));
        }
        else
        {
            // 剩下是4字节编码

            c2 = popp<unsigned char>();
            c3 = popp<unsigned char>();
            int _code = ((*c & 0x0F) << 18) | ((*c2 & 0x7F) << 12) | ((*c3 & 0x7F) << 6) | (*popp<unsigned char>() & 0x7F);
            if (_code >= 0x10000)
            {
                int _offset = _code - 0x10000;
                int _lead = 0xd800 | (_offset >> 10);
                int _trail = 0xdc00 | (_offset & 0x3ff);
                // strs[n++] = _lead;
                // strs[n++] = _trail;
                strs.push_back(std::string(1, _lead) + std::string(1, _trail));
            }
            else
            {
                // strs[n++] = _code;
                strs.push_back((char *)(_code));
            }
        }
    }
}

char *Byte::popUTFString(char *strs, int length)
{
    // unsigned short& len = *popp<unsigned short>();
    // int b = m_nReadPos;
    // char* strs = new char[len + 1];
    // length = len + 1;

    unsigned char *c, *c2, *c3;
    int n = 0;
    // int index = 0;
    // while (n < len) {
    while (n < length - 1)
    {
        c = popp<unsigned char>();
        if ((*c) < 0x80)
        {
            if (*c != 0)
                strs[n++] = *c;
        }
        else if ((*c) < 0xE0)
        {
            strs[n++] = ((*c & 0x3F) << 6) | (*popp<char>() & 0x7F);
        }
        else if ((*c) < 0xF0)
        {
            c2 = popp<unsigned char>();
            strs[n++] = ((*c & 0x1F) << 12) | ((*c2 & 0x7F) << 6) | (*popp<unsigned char>() & 0x7F);
        }
        else
        {
            c2 = popp<unsigned char>();
            c3 = popp<unsigned char>();
            int _code = ((*c & 0x0F) << 18) | ((*c2 & 0x7F) << 12) | ((*c3 & 0x7F) << 6) | (*popp<unsigned char>() & 0x7F);
            if (_code >= 0x10000)
            {
                int _offset = _code - 0x10000;
                int _lead = 0xd800 | (_offset >> 10);
                int _trail = 0xdc00 | (_offset & 0x3ff);
                strs[n++] = _lead;
                strs[n++] = _trail;
            }
            else
            {
                strs[n++] = _code;
            }
        }
    }
    strs[n] = '\0';
    return strs;
}

void Byte::poppUTFStringg(std::string &str)
{
    int strLength = poppUTFStringLength();
    char string[strLength];
    popUTFString(string, strLength);
    str = string;
}

//--------------------------------
void Byte::expand(int nExpandSize)
{

    if (nExpandSize < 1)
        return;

    if (!m_bNeedDelBuffer)
    {
        // 表示由外部通过setExternalBuffer设置的外部内存
        // 外部的内存都不够，那么由你外部处理，不要在这里自动处理,很危险的
        SQ_ASSERT(false);
        return;
    }
    
    unsigned long nSize = nExpandSize + m_nBufSize + m_nAdjustSize;

    char *pBuf = new char[nSize];

    // printf("expand %p %d %d\n",pBuf,m_nBufSize,nExpandSize);

    if (!pBuf)
    {
        return;
    }
    if (m_pBuffer)
    {
        memcpy(pBuf, m_pBuffer, m_nBufSize);
        delete[] m_pBuffer;
    }
    m_nBufSize = nSize;
    m_pBuffer = pBuf;
    if (onResizeCall)
        (*onResizeCall)();
}

void Byte::resize(int size, bool copy)
{

    if (m_nBufSize == size)
    {
        return;
    }

    char *pBuf = new char[size];

    // printf(" array buffer resize %p %d %p %p \n",this,size,pBuf,m_pBuffer);

    if (!pBuf)
    {
        return;
    }

    if (m_pBuffer)
    {
        if (copy)
        {
            int oldSize = size > m_nBufSize ? m_nBufSize : size;
            memcpy(pBuf, m_pBuffer, oldSize);
        }
        delete[] m_pBuffer;
    }

    m_bNeedDelBuffer = true;
    m_nBufSize = size;
    m_pBuffer = pBuf;
    if (onResizeCall)
        (*onResizeCall)();
}

//-----------------添加数据------------------------------
void Byte::append(int v)
{
    expand(m_nDataSize + sizeof(int) - m_nBufSize);
    *(int *)(m_pBuffer + m_nDataSize) = v;
    m_nDataSize += sizeof(int);
}

void Byte::append(long v)
{
    expand(m_nDataSize + sizeof(long) - m_nBufSize);
    *(long *)(m_pBuffer + m_nDataSize) = v;
    m_nDataSize += sizeof(long);
}

void Byte::appendInt(int v)
{
    expand(m_nDataSize + sizeof(int) - m_nBufSize);
    *(int *)(m_pBuffer + m_nDataSize) = v;
    m_nDataSize += sizeof(int);
}

void Byte::appendFloat(float v)
{
    expand(m_nDataSize + sizeof(float) - m_nBufSize);
    *(float *)(m_pBuffer + m_nDataSize) = v;
    m_nDataSize += sizeof(float);
}

void Byte::appendUnsignedShort(unsigned short v)
{
    expand(m_nDataSize + sizeof(unsigned short) - m_nBufSize);
    *(unsigned short *)(m_pBuffer + m_nDataSize) = v;
    m_nDataSize += sizeof(unsigned short);
}

char *Byte::append(const void *pBufffer, int nSize)
{
    if (!pBufffer)
        return NULL;
    expand(m_nDataSize + nSize - m_nBufSize);
    char *pLastBuffer = m_pBuffer + m_nDataSize;
    memcpy(pLastBuffer, pBufffer, nSize);
    m_nDataSize += nSize;
    return pLastBuffer;
}

void *Byte::appendEmpty(int nSize)
{
    expand(m_nDataSize + nSize - m_nBufSize);
    void *oldpos = m_pBuffer + m_nDataSize;
    m_nDataSize += nSize;
    return oldpos;
}

void *Byte::appendEmptyAlign(int nSize)
{
    int alignedsz = alignValue(nSize);
    expand(m_nDataSize + alignedsz - m_nBufSize);
    void *oldpos = m_pBuffer + m_nDataSize;
    m_nDataSize += nSize;
    return oldpos;
}

void Byte::_setDataSize(int nSize)
{
    m_nDataSize = nSize;
}

void Byte::setExternalBuffer(char *pBuffer, int nSize)
{
    if (m_bNeedDelBuffer && m_pBuffer)
    {
        delete[] m_pBuffer;
    }
    m_bNeedDelBuffer = false;
    m_pBuffer = pBuffer;
    m_nDataSize = m_nBufSize = nSize;
    m_nReadPos = 0;
}

unsigned int Byte::getReadPos()
{
    return m_nReadPos;
}

char *Byte::getCurrentWriteBuffer()
{
    return m_pBuffer + m_nDataSize;
}