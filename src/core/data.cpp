#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>
#include <assert.h>

#include <geek/core-data.h>

using namespace std;
using namespace Geek;

#define CHUNK 16384

#if 0
static void hexdump(const char* pos, int len)
{
    int i;
    for (i = 0; i < len; i += 16)
    {
        int j;
        printf("%08llx: ", (uint64_t)(pos + i));
        for (j = 0; j < 16 && (i + j) < len; j++)
        {
            printf("%02x ", (uint8_t)pos[i + j]);
        }
        for (j = 0; j < 16 && (i + j) < len; j++)
        {
            char c = pos[i + j];
            if (!isprint(c))
            {
                c = '.';
            }
            printf("%c", c);
        }
        printf("\n");
    }
}
#endif

Data::Data()
    : Logger("Data")
{
    m_data = NULL;
    m_length = 0;
    m_bufferSize = 0;
    m_swap = false;

    reset();
}

Data::Data(char* data, unsigned int length)
    : Logger("Data")
{
    m_data = data;
    m_length = length;
    m_bufferSize = length;
    m_swap = false;

    reset();
}

Data::~Data()
{
    clear();
}

bool Data::load(string filename)
{
    clear();
    log(DEBUG, "Data::load: Loading: %s", filename.c_str());
    FILE* file = fopen(filename.c_str(), "r");
    fseek(file, 0, SEEK_END);
    m_length = ftell(file);
    m_bufferSize = m_length;
    fseek(file, 0, SEEK_SET);

    m_data = (char*)malloc(m_length);
    fread(m_data, m_length, 1, file);
    fclose(file);

    reset();
    return true;
}

bool Data::loadCompressed(string filename, DataCompression dataCompression)
{
    clear();
    log(DEBUG, "Data::loadCompressed: Loading: %s", filename.c_str());
    FILE* file = fopen(filename.c_str(), "r");

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

    uint8_t* in = new uint8_t[CHUNK];
    uint8_t* out = new uint8_t[CHUNK];
    int flush;
    do
    {
        strm.avail_in = fread(in, 1, CHUNK, file);
        if (ferror(file))
        {
            (void)inflateEnd(&strm);
            delete[] in;
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
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)inflateEnd(&strm);

    fclose(file);

    reset();
    return true;
}


void Data::clear()
{
    if (m_data != NULL)
    {
         free(m_data);
         m_data = NULL;
    }
    m_pos = NULL;
    m_end = NULL;
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
    uint16_t res;
if (!m_swap)
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
    uint32_t res;
if (!m_swap)
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
    uint64_t res;
    if (!m_swap)
    {
        res = (uint64_t)read32();
        res |= (uint64_t)read32() << 32;
    }
    else
    {
        res = (uint64_t)read32() << 32;
        res |= (uint64_t)read32() << 0;
    }
    return res;
}


uint8_t Data::peek8()
{
    return *(m_pos);
}

float Data::readFloat()
{
    float res;
    res = *((float*)m_pos);
    m_pos += sizeof(float);
    return res;
}

double Data::readDouble()
{
    double res;
    res = *((double*)m_pos);
    m_pos += sizeof(double);
    return res;
}


/*
Vector Data::readVector()
{
    Vector v;
    v.x = readFloat();
    v.y = readFloat();
    v.z = readFloat();
    return v;
}
*/

string Data::cstr()
{
    string str = "";
    while (!eof())
    {
        char c = (char)read8();
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
    string str = "";
    bool eol = false;
    int i;
    for (i = 0; i < len; i++)
    {
        char c = (char)read8();
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
    string line = "";
    while (!eof())
    {
        char c = (char)read8();
        if (c == '\r')
        {
            c = (char)read8();
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
#ifdef DEBUG_DATA
    printf("Data::append8: data=%d\n", data);
#endif
    return append(&data, 1);
}

bool Data::append16(uint16_t data)
{
    if (!m_swap)
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
    if (!m_swap)
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
    if (!m_swap)
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
    return append((uint8_t*)&data, sizeof(float));
}

bool Data::appendDouble(double data)
{
    return append((uint8_t*)&data, sizeof(double));
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
#ifdef DEBUG_DATA
        printf("Data::append: Growing by %d bytes\n", grow);
#endif

        m_bufferSize += grow;

        m_data = (char*)realloc(m_data, m_bufferSize);

        m_pos = m_data;
        m_end = m_data + m_length;
    }

#ifdef DEBUG_DATA
    printf("Data::append: Copying %d bytes...\n", length);
#endif
    memcpy(m_end, data, length);
    m_end += length;
    m_length += length;
#ifdef DEBUG_DATA
    printf("Data::append: m_length=%d\n", m_length);
#endif

    return true;
}

bool Data::write(std::string file)
{
    return write(file, 0, m_length);
}

bool Data::write(std::string file, uint32_t pos, uint32_t length)
{
    FILE* fp = fopen(file.c_str(), "w");
    if (fp == NULL)
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
    z_stream stream = {
        .zalloc   = Z_NULL,
        .zfree    = Z_NULL,
        .opaque   = Z_NULL,
        .next_in  = (uint8_t*)m_data,
        .avail_in = m_length
    };

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

    uint8_t* outBuffer = new uint8_t[CHUNK];

    do
    {
        stream.next_out  = outBuffer;
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
    while(stream.avail_out == 0);

    (void)deflateEnd(&stream);

    delete[] outBuffer;

    fclose(fd);

    return true;
}

Data* Data::getSubData(uint32_t pos, uint32_t length)
{
    return new Data(m_data + pos, length);
}




