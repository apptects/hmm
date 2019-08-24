#include "stl.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>

using namespace boost::interprocess;

void SaveBinarySTL(
    const std::string &path,
    const std::vector<glm::vec3> &points,
    const std::vector<glm::ivec3> &triangles)
{
    const uint64_t numBytes = uint64_t(triangles.size()) * 50 + 84;

    {
        file_mapping::remove(path.c_str());
        std::filebuf fbuf;
        fbuf.open(path.c_str(),
            std::ios_base::in | std::ios_base::out | std::ios_base::trunc |
            std::ios_base::binary);
        fbuf.pubseekoff(numBytes - 1, std::ios_base::beg);
        fbuf.sputc(0);
    }

    file_mapping fm(path.c_str(), read_write);
    mapped_region mr(fm, read_write);
    uint8_t *dst = (uint8_t *)mr.get_address();

    const uint32_t count = triangles.size();
    memcpy(dst + 80, &count, 4);

    for (uint32_t i = 0; i < triangles.size(); i++) {
        const glm::ivec3 &t = triangles[i];
        // const glm::vec3 normal = t.Normal();
        const glm::vec3 normal(0);
        const uint64_t idx = 84 + i * 50;
        memcpy(dst + idx + 0, &normal, 12);
        memcpy(dst + idx + 12, &points[t.x], 12);
        memcpy(dst + idx + 24, &points[t.y], 12);
        memcpy(dst + idx + 36, &points[t.z], 12);
    }
}
