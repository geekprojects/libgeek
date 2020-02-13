#ifndef __LIBGEEK_CORE_DATA_H_
#define __LIBGEEK_CORE_DATA_H_

#include <string>
#include <stdint.h>

#include <geek/core-logger.h>

namespace Geek {

enum DataCompression
{
    AUTO = 1,
    GZIP = 2,
    DEFLATE = 3
};

class Data : public Geek::Logger
{
 protected:
    char* m_data;
    char* m_end;
    char* m_pos;
    unsigned int m_length;
    unsigned int m_bufferSize;
    bool m_isSub;

    bool m_swap;

 public:
    Data();
    Data(char* data, unsigned int length);
    virtual ~Data();

    bool load(std::string filename);
    bool loadCompressed(std::string filename, DataCompression compression);

    void setSwap(bool swap) { m_swap = swap; }
    bool getSwap() { return m_swap; }

    void clear();
    void reset();

    bool eof();
    uint32_t pos();
    void setPos(uint32_t pos);
    void skip(unsigned int amount);
    uint32_t getLength() { return m_length; }
    char* posPointer() { return m_pos; }

    uint8_t peek8();
    uint32_t peek32();

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    uint64_t read64();
    float readFloat();
    double readDouble();
    //Vector readVector();
    uint64_t readULEB128();

    char* readStruct(size_t len);

    std::string cstr();
    std::string readString(int max);
    std::string readLine();

    bool append8(uint8_t data);
    bool append16(uint16_t data);
    bool append32(uint32_t data);
    bool append64(uint64_t data);
    bool appendFloat(float data);
    bool appendDouble(double data);
    bool append(uint8_t* data, int length);

    bool write(std::string file);
    bool write(std::string file, uint32_t pos, uint32_t length);
    bool write(FILE* fp, uint32_t pos, uint32_t length);
    bool writeCompressed(std::string file, DataCompression dataCompression);

    Data* getSubData(uint32_t pos, uint32_t length);
};

}

#endif
