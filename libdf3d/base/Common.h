#pragma once

// Common includes.
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <queue>
#include <list>
#include <set>
#include <stdexcept>
#include <memory>
#include <chrono>

#ifndef MSVC
#include <gui/impl/RocketIntrusivePtr.h>
#endif

#include <boost/intrusive_ptr.hpp>

#define GLM_FORCE_RADIANS
//#define GLM_MESSAGES
//#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <json/json.h>

namespace Rocket { namespace Core { class Element; class ElementDocument; } }

using RocketDocument = boost::intrusive_ptr<Rocket::Core::ElementDocument>;
using RocketElement = boost::intrusive_ptr<Rocket::Core::Element>;

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

// Common macros.

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define SAFE_DELETE(x) { delete x; x = nullptr; }
#define SAFE_ARRAY_DELETE(x) { delete [] x; x = nullptr; }

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

namespace df3d {

class Node;

enum class ResourceLoadingMode
{
    IMMEDIATE,
    ASYNC
};

enum class PixelFormat
{
    INVALID,
    RGB,
    BGR,
    RGBA,
    GRAYSCALE
};

using ResourceGUID = std::string;
using SceneNode = shared_ptr<df3d::Node>;
using WeakSceneNode = weak_ptr<df3d::Node>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

inline float IntervalBetween(const TimePoint &t1, const TimePoint &t2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t2).count() / 1000.f;
}

// seconds
inline float IntervalBetweenNowAnd(const TimePoint &timepoint)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timepoint).count() / 1000.0f;
}

static const int DEFAULT_WINDOW_WIDTH = 640;
static const int DEFAULT_WINDOW_HEIGHT = 480;

}

// Include some useful df3d stuff.

#include <base/Log.h>
#include <utils/NonCopyable.h>
