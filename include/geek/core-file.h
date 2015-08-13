#ifndef __GEEK_CORE_FILE_H_
#define __GEEK_CORE_FILE_H_

#include <string>
#include <vector>

namespace Geek {
namespace Core {

class File
{
 public:
    static bool mkdirs(std::string path);
};

}
}

#endif
