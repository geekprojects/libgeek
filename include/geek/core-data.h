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

enum Endian
{
    NONE,
    BIG,
    LITTLE
};

class Data : public Geek::Logger
{
 protected:
    char* m_data = nullptr;
    char* m_end = nullptr;
    char* m_pos = nullptr;
    unsigned int m_length = 0;
    unsigned int m_bufferSize = 0;
    bool m_isSub = false;
    Endian m_endian = Endian::NONE;

 public:
    Data();
    Data(char* data, unsigned int length);
    virtual ~Data();

    bool load(std::string filename);
    bool loadCompressed(std::string filename, DataCompression compression);
    bool loadCompressed(char* data, unsigned int length, DataCompression compression);

    void setEndian(Endian endian);
    void setSwap(bool swap)
    {
        Endian endian = getMachineEndian();
        if (swap)
        {
            if (endian == LITTLE)
            {
                m_endian = BIG;
            }
            else
            {
                m_endian = LITTLE;
            }
        }
        else
        {
            m_endian = endian;
        }
    }
    bool getSwap() const { return mustSwap(m_endian); }

    void clear();
    void reset();

    char* getData() { return m_data; }
    bool eof();
    uint32_t pos();
    void setPos(uint32_t pos);
    void skip(unsigned int amount);
    uint32_t getLength() const { return m_length; }
    uint32_t getRemaining() const { return m_end - m_pos; }
    char* posPointer() { return m_pos; }

    uint8_t peek8();
    uint32_t peek32();

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    uint64_t read64();
    float readFloat();
    double readDouble();
    uint64_t readULEB128();

    uint16_t read16(Endian endian);
    uint32_t read32(Endian endian);
    uint64_t read64(Endian endian);

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
    bool appendString(std::string str);
    bool appendString(std::wstring str);

    bool write(std::string file);
    bool write(std::string file, uint32_t pos, uint32_t length);
    bool write(FILE* fp, uint32_t pos, uint32_t length);
    bool writeCompressed(std::string file, DataCompression dataCompression);

    Data* getSubData(uint32_t pos, uint32_t length);

    static inline Endian getMachineEndian()
    {
        union
        {
            uint32_t i;
            char c[4];
        } b = {0x01020304};

        if (b.c[0] == 1)
        {
            return BIG;
        }
        else
        {
            return LITTLE;
        }
    }

    static bool mustSwap(Endian endian)
    {
        return endian != NONE && endian != getMachineEndian();
    }
};

}

#endif
