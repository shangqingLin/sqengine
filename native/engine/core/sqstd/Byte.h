#pragma once
#include <memory>
#include <cstdio>
#include <vector>
#include <string>

typedef void (*RESIZE_CALL)();

namespace sqstd
{
    class Byte
    {
    public:
        Byte(unsigned int nSize, unsigned int nAdjustSize);
        Byte(char *buffer, int nsize, bool m_bNeedDelBuffer);
        Byte(char *buffer, int nsize);
        Byte();
        ~Byte();

        void setBufferExpandStep(int nAdjustSize);
        void setAlign(bool bAlign);
        char *getBuffer();
        char *getReadPtr();
        unsigned int &getCurrentPos();
        unsigned int getDataSize();
        unsigned int getBuffSize();
        void setBufferSize(int nBufferSize);
        char *readBuffer(int nLength);
        char *readBufferAlign(int nLength);
        template <class T>
        T *popp()
        {
            T *ret = (T *)(m_pBuffer + m_nReadPos);
            m_nReadPos += sizeof(T);
            return ret;
        }
        template <class T>
        T *poppAlign()
        {
            T *ret = (T *)(m_pBuffer + m_nReadPos);
            m_nReadPos += alignValue(sizeof(T));
            return ret;
        }

        void popUTFString(std::vector<std::string> &strs);
        void popUTFString(std::vector<unsigned int> &strs);
        int poppUTFStringLength();
        char *popUTFString(char *strs, int length);
        void poppUTFStringg(std::string &);

        template <class T>
        T &get(unsigned int pos)
        {
            T &ret = *(T *)(m_pBuffer + pos);
            return ret;
        }

        template <class T>
        void getArray(unsigned int &startPos, int &arrayNum, T *result)
        {
            int size = sizeof(T);
            for (int i = 0; i < arrayNum; ++i)
            {
                result[i] = *(T *)(m_pBuffer + startPos);
                startPos += size;
            }
        }

        //===================数据填充函数======================
        template <class T>
        void append(T v)
        {
            int size = sizeof(T);
            expand(m_nDataSize + size - m_nBufSize);
            *(T *)(m_pBuffer + m_nDataSize) = v;
            m_nDataSize += size;
        }

        template <class T>
        void setValue(unsigned int pos, T v)
        {
            *(T *)(m_pBuffer + pos) = v;
        }
        template <class T>
        void appendAlign(T v)
        {
            int size = alignValue(sizeof(T));
            expand(m_nDataSize + size - m_nBufSize);
            *(T *)(m_pBuffer + m_nDataSize) = v;
            m_nDataSize += size;
        }

        void append(int v);
        void append(long v);

        void appendInt(int v);

        void appendFloat(float v);

        void appendUnsignedShort(unsigned short v);
        char *append(const void *pBufffer, int nSize);
        void *appendEmpty(int nSize);
        void *appendEmptyAlign(int nSize);

        //======================================
        void _setDataSize(int nSize);
        inline void clearData()
        {
            m_nDataSize = m_nReadPos = 0;
        }
        inline void clearData(char v)
        {
            m_nDataSize = m_nReadPos = 0;
            if (m_nBufSize > 0)
            {
                memset(m_pBuffer, v, m_nBufSize);
            }
        }
        inline bool isEmpty()
        {
            return m_nDataSize == 0;
        }

        inline void setReadPos(int nPos)
        {
            m_nReadPos = nPos;
        }
        unsigned int getReadPos();
        inline void setWritePos(int nSize)
        {
            m_nDataSize = nSize;
        }
        void setExternalBuffer(char *pBuffer, int nSize);

        char *getCurrentWriteBuffer();

        void resize(int size, bool copy = false);

    private:
        void expand(int nExpandSize);
        inline int alignValue(int nValue)
        {
            return m_bAlign ? (nValue + 3) & 0xfffffffc : nValue;
        }

        unsigned int m_nBufSize;    ///< buffer的总大小，内存开辟的空间大小
        char *m_pBuffer;            ///< buffer
        unsigned int m_nReadPos;    ///< 当前读取的位置
        unsigned int m_nAdjustSize; ///< 每次扩容的时候，增加的大小
        bool m_bNeedDelBuffer;      ///< 是否需要删除buffer
    public:
        RESIZE_CALL onResizeCall = nullptr;
        unsigned int m_nDataSize; ///< 里面数据的总大小，buffer中的真实数据大小
        bool m_bAlign = false;
    };
}