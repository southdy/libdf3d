#pragma once

#include <random>
#include <chrono>
#include <sstream>
#include <cctype>

namespace df3d {

// Distribution should be consistent across all platforms.
class RandomUtils
{
public:
    static std::mt19937 gen;

    static void srand();
    static void srand(std::mt19937::result_type seed);

    // Random value in [0, 32767]
    static int rand();
    // Random value in [a, b)
    static float randRange(float a, float b);
    // Random value in [a.x, b.x)
    static float randRange(const glm::vec2 &a) { return randRange(a.x, a.y); }
    // Random value in [a, b]
    static int randRange(int a, int b);
};

class TimeUtils
{
public:
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

    // Interval in seconds.
    static float IntervalBetween(const TimePoint &t1, const TimePoint &t2);
    // Interval in seconds.
    static float IntervalBetweenNowAnd(const TimePoint &timepoint);
    static TimePoint now();
};

namespace utils {

inline void skip_line(std::istream &is)
{
    std::string temp;
    std::getline(is, temp);
}

template<typename T>
struct shared_ptr_less
{
    bool operator()(const shared_ptr<T> &a, const shared_ptr<T> &b) const
    {
        return a.get() < b.get();
    }
};

template <typename T>
uintptr_t getTypeId()
{
    static char dummy;
    return reinterpret_cast<uintptr_t>(&dummy);
}

template<typename C, typename T>
inline bool contains(const C &container, const T &val)
{
    return std::find(std::begin(container), std::end(container), val) != std::end(container);
}

template<typename C, typename T>
inline bool contains_key(const C &container, const T &key)
{
    return container.find(key) != container.end();
}

template<typename T>
inline const T& random_vector_element(const std::vector<T> &vect)
{
    return vect[RandomUtils::rand() % vect.size()];
}

template<typename T>
inline std::string to_string(const T &v)
{
    std::ostringstream os;
    os << v;
    return os.str();
}

template<typename T>
inline T from_string(const std::string &s)
{
    T res{};
    std::istringstream is(s);
    is >> res;
    return res;
}

inline std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> result;

    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);

    return result;
}

inline bool starts_with(const std::string &s, const std::string &with)
{
    return s.compare(0, with.size(), with) == 0;
}

inline bool ends_with(const std::string &s, const std::string &with)
{
    if (with.size() > s.size())
        return false;

    return s.compare(s.size() - with.size(), s.size(), with) == 0;
}

inline void replace_all(std::string &s, const std::string &what, const std::string &with)
{
    size_t found = 0;
    while ((found = s.find(what)) != std::string::npos) // TODO: more efficient implementation.
    {
        s.replace(found, what.size(), with);
    }
}

template<typename T>
inline T clamp(const T &val, const T &min, const T &max)
{
    return std::min(std::max(val, min), max);
}

inline void trim_left(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); }));
}

inline void trim_right(std::string &str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); }).base(), str.end());
}

inline void trim(std::string &str)
{
    trim_right(str);
    trim_left(str);
}

inline void to_lower(std::string &str)
{
    for (auto &c : str)
        c = std::tolower(c);
}

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen);

std::vector<uint8_t> zlibCompress(const std::vector<uint8_t> &input);
std::vector<uint8_t> zlibDecompress(const std::vector<uint8_t> &input);

inline size_t sizeKB(size_t val)
{
    return (val + 1023) / 1024;
}

inline size_t sizeMB(size_t val)
{
    return (sizeKB(val) + 1023) / 1024;
}

} }
