
#ifndef __JCBuffer_H__
#define __JCBuffer_H__
#include <memory>
#include <cstdio>
#include <vector>
#include <string>

typedef void (*RESIZE_CALL)();

/**
 * @brief 内存快速的buffer操作
 */
class ArrayBuffer
{
public:
    /** @brief 构造函数
     *  @param[in] 初始化size的大小
     *  @param[in] 当大小不够的时候，每次增加的大小
     */
    ArrayBuffer(unsigned int nSize, unsigned int nAdjustSize);
    ArrayBuffer(char *buffer, int nsize, bool m_bNeedDelBuffer);
    ArrayBuffer(char *buffer, int nsize);
    ArrayBuffer();

    /** @brief 析构函数
     */
    ~ArrayBuffer();

    /** @brief 设置每次扩充buffer的size
     *  @param[in] 扩充的size
     */
    void setBufferExpandStep(int nAdjustSize);

    /** @brief 是否需要对其
     *  @param[in] 是否需要对其
     */
    void setAlign(bool bAlign);


    /** @brief 获得buffer
     *  @return 返回buffer
     */
    char *getBuffer();

    /** @brief 获得读取到当前的位置
     *  @return 返回buffer的指针
     */
    char *getReadPtr();

    unsigned int &getCurrentPos();

    /** @brief 获得数据长度
     *  @return 返回数据长度大小
     */
    unsigned int getDataSize();

    /** @brief 返回bufferSize
     *  @return 返回buffer数据大小
     */
    unsigned int getBuffSize();

    /** @brief 设置buffer的大小。只分配空间，但是getDataSize依然是原来的值
     *  @param[in] 要设置的大小
     */
    void setBufferSize(int nBufferSize);

    /** @brief 读取buffer，注意：不是4字节对其
     *  @param[in] 需要传入读取的长度
     *  @return 返回buffer指针
     */
    char *readBuffer(int nLength);

    /** @brief 读取buffer，4字节对其
     *  @param[in] 需要传入读取的长度
     *  @return 返回buffer指针
     */
    char *readBufferAlign(int nLength);

    /** @brief 从当前读指针返回一个对象指针。并且前进，注意：不是4字节对其
     */
    template <class T>
    T *popp()
    {
        T *ret = (T *)(m_pBuffer + m_nReadPos);
        m_nReadPos += sizeof(T);
        return ret;
    }

    /** @brief 从当前读指针返回一个对象指针。并且前进。（4对齐）
     */
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
    void poppUTFStringg(std::string&);

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

    /** @brief append 数据，比如一个结构体，注意不是4字节对其
     *  @param[in] 数据
     */
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

    /** @brief append 数据，比如一个结构体
     *  @param[in] 数据
     */
    template <class T>
    void appendAlign(T v)
    {
        int size = alignValue(sizeof(T));
        expand(m_nDataSize + size - m_nBufSize);
        *(T *)(m_pBuffer + m_nDataSize) = v;
        m_nDataSize += size;
    }

    /** @brief append一个int值
     *  @param[in] int值
     */
    void append(int v);

    /** @brief append一个long值
     *  @param[in] long值
     */
    void append(long v);

    void appendInt(int v);

    void appendFloat(float v);

    void appendUnsignedShort(unsigned short v);

    /** @brief append buffer函数
     *  @param[in] buffer指针
     *  @param[in] buffer大小
     */
    char *append(const void *pBufffer, int nSize);

    /** @brief 假装添加数据，其实就是修改数据大小。如果调用者希望直接针对指针修改数据，通过这个来通知buffer 数据已经改变了。
     *  @param[in] sz 大小，字节。
     *  @return 扩展前的位置。
     */
    void *appendEmpty(int nSize);

    /** @brief 假装添加数据，其实就是修改数据大小。如果调用者希望直接针对指针修改数据，通过这个来通知buffer 数据已经改变了。
     *  @param[in] sz 大小，字节。
     *  @return 扩展前的位置。
     */
    void *appendEmptyAlign(int nSize);

    //======================================
    /*
     * 慎用，只有非常明白的人才可以使用
     */
    void _setDataSize(int nSize);

    /** @brief 清空数据，但是不删除buffer
     *  @return
     */
    inline void clearData()
    {
        m_nDataSize = m_nReadPos = 0;
    }

    /** @brief 是否为Emplty,如果没有数据，就认为是null
     *  @return
     */
    inline bool isEmpty()
    {
        return m_nDataSize == 0;
    }

    /** @brief 设置读取read的pos
     *  @param[in] pos
     */
    inline void setReadPos(int nPos)
    {
        m_nReadPos = nPos;
    }
    unsigned int getReadPos();

    /** @brief 设置写入的数据的位置，也就是相当于data的size
     *  @param[in] 输入size
     */
    inline void setWritePos(int nSize)
    {
        m_nDataSize = nSize;
    }

    /**
     * 设置一个外部的Buffer
     */
    void setExternalBuffer(char *pBuffer, int nSize);
    
    char *getCurrentWriteBuffer();

    void resize(int size, bool copy = false);

private:
    /** @brief 扩充buffer
     *  @param[in] 需要扩充的size
     */
    void expand(int nExpandSize);

    /** @brief 对其数据函数
     *  @param[in] 输入的值
     *  @return 获得对其后的值
     */
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
    unsigned int m_nDataSize;   ///< 里面数据的总大小，buffer中的真实数据大小
    bool m_bAlign = false;
};
#endif