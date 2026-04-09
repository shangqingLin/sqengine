
bool littleEndian = false;
bool checkLittleEndian = false;
bool isLittleEndian() {
    if(!checkLittleEndian){
        unsigned short num = 0x1;  // 16-bit整数
        unsigned char *ptr = reinterpret_cast<unsigned char*>(&num);
        littleEndian = ptr[0] == 0x1;  // 低地址是否存储低字节
        checkLittleEndian = true;
    }
    return littleEndian;
}
