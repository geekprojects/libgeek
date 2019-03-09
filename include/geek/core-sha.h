#ifndef __LIBGEEK_CORE_SHA_H_
#define __LIBGEEK_CORE_SHA_H_

#include <string>

namespace Geek
{
namespace Core
{

class SHA
{
 public:
    static std::string sha256(std::string text);
    static std::string sha256File(std::string path);
};

};
};

#endif
