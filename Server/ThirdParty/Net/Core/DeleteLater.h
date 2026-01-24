#pragma once

#ifdef _WIN32
#define EXPORT_FUNC __declspec(dllexport)
#elif __linux__
#define EXPORT_FUNC
#endif

class DeleteLaterImpl {
public:
    virtual ~DeleteLaterImpl() = default;
};
EXPORT_FUNC void DeleteLater(DeleteLaterImpl* ptr);