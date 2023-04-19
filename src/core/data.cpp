#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <zlib.h>
#include <cassert>
#include "utf8.h"

#include <geek/core-data.h>
#include <geek/core-string.h>

using namespace std;
using namespace Geek;

#define CHUNK 16384

Data::Data()
    : Logger("Data")
{
    reset();
}

Data::Data(char* data, unsigned int length)
    : Logger("Data")
{
    m_data = data;
    m_length = length;
    m_bufferSize = length;

    reset();
}

Data::~Data()
{
    clear();
}

void Data::setEndian(Endian endian)
{
    m_endian = endian;
}

bool Data::load(string filename)
{
    clear();
    log(DEBUG, "load: Loading: %s", filename.c_str());
    FILE* file = fopen(filename.c_str(), "r");
    if (file == nullptr)
    {
        log(ERROR, "load: Failed to load: %s", filename.c_str());
        return false;
    }

    fseek(file, 0, SEEK_END);
    m_length = ftell(file);
    m_bufferSize = m_length;
    fseek(file, 0, SEEK_SET);

    m_data = (char*) malloc(m_length);
    size_t res = fread(m_data, m_length, 1, file);
    fclose(file);

    reset();
    return (res == 1);
}

bool Data::loadCompressed(string filename, DataCompression dataCompression)
{
    clear();
    log(DEBUG, "loadCompressed: Loading: %s", filename.c_str());
    FILE* file = fopen(filename.c_str(), "r");
    if (file == nullptr)
    {
        log(ERROR, "loadCompressed: Failed to load: %s", filename.c_str());
        return false;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int window = 15;
    if (dataCompression == AUTO)
    {
        window += 32;
    }
    else if (dataCompression == GZIP)
    {
        window += 16;
    }

    int ret;
    // 32 = automatic gzip detection
    ret = inflateInit2(&strm, window);

    if (ret != Z_OK)
    {
        fclose(file);
        return false;
    }

    auto in = new uint8_t[CHUNK];
    auto out = new uint8_t[CHUNK];
    int flush;
    do
    {
        strm.avail_in = fread(in, 1, CHUNK, file);
        if (ferror(file))
        {
            (void) inflateEnd(&strm);
            fclose(file);
            delete[] in;
            delete[] out;
            return false;
        }
        flush = feof(file) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            int have = CHUNK - strm.avail_out;

#ifdef DEBUG_DATA
            printf("Data::loadCompressed: Read %d uncompressed bytes\n", have);
#endif
            append(out, have);
        }
        while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    }
    while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void) inflateEnd(&strm);

    fclose(file);

    reset();
    return true;
}

bool Data::loadCompressed(char* data, unsigned int length, DataCompression dataCompression)
{
    clear();

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int window = 15;
    if (dataCompression == AUTO)
    {
        window += 32;
    }
    else if (dataCompression == GZIP)
    {
        window += 16;
    }

    int ret;
    // 32 = automatic gzip detection
    ret = inflateInit2(&strm, window);

    if (ret != Z_OK)
    {
        return false;
    }

    auto ptr = data;
    auto in = new uint8_t[CHUNK];
    auto out = new uint8_t[CHUNK];
    int flush;
    while (length > 0)
    {
        strm.avail_in = CHUNK;
        flush = Z_NO_FLUSH;
        if (strm.avail_in > length)
        {
            strm.avail_in = length;
            flush = Z_FINISH;
        }
        memcpy(in, ptr, strm.avail_in);
        length -= strm.avail_in;
        strm.next_in = in;
        ptr += strm.avail_in;

        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            int have = CHUNK - strm.avail_out;

#ifdef DEBUG_DATA
            printf("Data::loadCompressed: Read %d uncompressed bytes\n", have);
#endif
            append(out, have);
        }
        while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
    }

    while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void) inflateEnd(&strm);

    reset();
    return true;
}


void Data::clear()
{
    if (m_data != nullptr && !m_isSub)
    {
        free(m_data);
        m_data = nullptr;
    }
    m_pos = nullptr;
    m_end = nullptr;
    m_length = 0;
    m_bufferSize = 0;
}

void Data::reset()
{
    m_pos = m_data;
    m_end = m_data + m_length;
}

bool Data::eof()
{
    return m_pos >= m_end;
}

uint32_t Data::pos()
{
    return (m_pos - m_data);
}

void Data::setPos(uint32_t pos)
{
    m_pos = m_data + pos;
}

void Data::skip(unsigned int amount)
{
    m_pos += amount;
}

uint8_t Data::read8()
{
    return *(m_pos++);
}

uint16_t Data::read16()
{
    return read16(m_endian);
}

uint16_t Data::read16(Endian endian)
{
    uint16_t res;
    if (!mustSwap(endian))
    {
        res = read8();
        res |= read8() << 8;
    }
    else
    {
        res = read8() << 8;
        res |= read8() << 0;
    }
    return res;
}

uint32_t Data::read32()
{
    return read32(m_endian);
}

uint32_t Data::read32(Endian endian)
{
    uint32_t res;
    if (!mustSwap(endian))
    {
        res = read8();
        res |= read8() << 8;
        res |= read8() << 16;
        res |= read8() << 24;
    }
    else
    {
        res = read8() << 24;
        res |= read8() << 16;
        res |= read8() << 8;
        res |= read8() << 0;
    }
    return res;
}

uint64_t Data::read64()
{
    return read64(m_endian);
}

uint64_t Data::read64(Endian endian)
{
    uint64_t res;
    if (!mustSwap(endian))
    {
        res = (uint64_t) read32(endian);
        res |= (uint64_t) read32(endian) << 32;
    }
    else
    {
        res = (uint64_t) read32(endian) << 32;
        res |= (uint64_t) read32(endian) << 0;
    }
    return res;
}

uint8_t Data::peek8()
{
    return *(m_pos);
}

uint32_t Data::peek32()
{
    return *((uint32_t*) m_pos);
}

float Data::readFloat()
{
    float res;
    res = *((float*) m_pos);
    m_pos += sizeof(float);
    return res;
}

double Data::readDouble()
{
    double res;
    res = *((double*) m_pos);
    m_pos += sizeof(double);
    return res;
}

uint64_t Data::readULEB128()
{
    uint64_t result = 0;
    int bit = 0;

    while (!eof())
    {
        uint8_t b = read8();
        result |= (((uint64_t) (b & 0x7f)) << bit);
        bit += 7;

        if (!(b & 0x80))
        {
            break;
        }
    }

    return result;
}

char* Data::readStruct(size_t len)
{
    char* pos = m_pos;
    m_pos += len;
    return pos;
}

string Data::cstr()
{
    string str;
    while (!eof())
    {
        char c = (char) read8();
        if (c == 0)
        {
            break;
        }
        str += c;
    }
    return str;
}

string Data::readString(int len)
{
    string str;
    bool eol = false;
    int i;
    for (i = 0; i < len; i++)
    {
        char c = (char) read8();
        if (!eol)
        {
            if (c == 0)
            {
                eol = true;
            }
            else
            {
                str += c;
            }
        }
    }
    return str;
}

string Data::readLine()
{
    string line;
    while (!eof())
    {
        char c = (char) read8();
        if (c == '\r')
        {
            c = (char) read8();
        }
        if (c == '\n')
        {
            break;
        }
        line += c;
    }
    return line;
}

bool Data::append8(uint8_t data)
{
    return append(&data, 1);
}

bool Data::append16(uint16_t data)
{
    if (!mustSwap(m_endian))
    {
        append8(data >> 0);
        append8(data >> 8);
    }
    else
    {
        append8(data >> 8);
        append8(data >> 0);
    }
    return true;
}

bool Data::append32(uint32_t data)
{
    if (!mustSwap(m_endian))
    {
        append8(data >> 0);
        append8(data >> 8);
        append8(data >> 16);
        append8(data >> 24);
    }
    else
    {
        append8(data >> 24);
        append8(data >> 16);
        append8(data >> 8);
        append8(data >> 0);
    }

    return true;
}

bool Data::append64(uint64_t data)
{
    if (!mustSwap(m_endian))
    {
        append32(data << 0);
        append32(data << 32);
    }
    else
    {
        append32(data << 32);
        append32(data << 0);
    }
    return true;
}

bool Data::appendFloat(float data)
{
    return append((uint8_t*) &data, sizeof(float));
}

bool Data::appendDouble(double data)
{
    return append((uint8_t*) &data, sizeof(double));
}

bool Data::append(uint8_t* data, int length)
{
    int remaining = m_bufferSize - m_length;
    if (remaining < length)
    {
        int grow = length * 2;
        if (length < 32)
        {
            grow = 64;
        }
        else if (length < 8 * 1024)
        {
            grow = length * 2;
        }
        else
        {
            grow = length;
        }

        m_bufferSize += grow;

        m_data = (char*) realloc(m_data, m_bufferSize);

        m_pos = m_data;
        m_end = m_data + m_length;
    }

    memcpy(m_end, data, length);
    m_end += length;
    m_length += length;

    return true;
}

bool Data::appendString(string str)
{
    return append((uint8_t*) str.c_str(), str.length());
}

bool Data::appendString(wstring str)
{
    string strUtf8 = Geek::Core::wstring2utf8(str);
    return appendString(strUtf8);
}

bool Data::write(std::string file)
{
    return write(file, 0, m_length);
}

bool Data::write(std::string file, uint32_t pos, uint32_t length)
{
    FILE* fp = fopen(file.c_str(), "w");
    if (fp == nullptr)
    {
        return false;
    }

    bool res;
    res = Data::write(fp, pos, length);

    fclose(fp);
    return res;
}

bool Data::write(FILE* fp, uint32_t pos, uint32_t length)
{
    unsigned int res;
    res = fwrite(m_data + pos, length, 1, fp);
    return res == length;
}

bool Data::writeCompressed(string file, DataCompression dataCompression)
{
    FILE* fd = fopen(file.c_str(), "w");

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.next_in = (uint8_t*) m_data;
    stream.avail_in = m_length;

    // Default bits
    int windowbits = 15;

    if (dataCompression == GZIP)
    {
        // Add a gzip header
        windowbits += 16;
    }

    int res;
    res = deflateInit2(
        &stream,
        Z_DEFAULT_COMPRESSION,
        Z_DEFLATED,
        windowbits,
        8,
        Z_DEFAULT_STRATEGY);
    if (res != Z_OK)
    {
        log(ERROR, "writeCompressed: Failed to initialse deflate");
        fclose(fd);
        return false;
    }

    auto outBuffer = new uint8_t[CHUNK];

    do
    {
        stream.next_out = outBuffer;
        stream.avail_out = CHUNK;

        res = deflate(&stream, Z_FINISH);
        if (res == Z_STREAM_ERROR)
        {
            log(ERROR, "writeCompressed: Failed to deflate buffer");
            delete[] outBuffer;
            fclose(fd);
            return false;
        }

        fwrite(outBuffer, CHUNK - stream.avail_out, 1, fd);

    }
    while (stream.avail_out == 0);

    (void) deflateEnd(&stream);

    delete[] outBuffer;

    fclose(fd);

    return true;
}

Data* Data::getSubData(uint32_t pos, uint32_t length)
{
    Data* data = new Data(m_data + pos, length);
    data->m_isSub = true;
    return data;
}




