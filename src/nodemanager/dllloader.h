#ifndef CORE_DLL_LOADER_H
#define CORE_DLL_LOADER_H

#include <mutex>
#include <string>
#include <unordered_map>
// REVIEW (Mitch): Write tests for this class. It is trivially unit testable.
// REVIEW (Mitch): move this class to util namespace
// REVIEW (Mitch): DllLoader is an inaccurate name, prefer DylibLoader?
// REVIEW (Mitch): Make constructor private, supply public const static member factory func.
class DllLoader {
public:
    ~DllLoader();

// REVIEW (Mitch): use std::filesystem path
    template<class T>
    T* GetLibraryFunction(const std::string& lib_path,
                          const std::string& lib_name,
                          const std::string& function_name);

private:
    // DLL functions
    void* GetLibrary(const std::string& dll_path);
    void* GetLibraryFunction_(const std::string& dll_path, const std::string& function_name);
    bool CloseLibrary(void* lib);

    static std::string GetLibraryPrefix();
    static std::string GetLibrarySuffix();
    std::string GetLibraryError();

    // Library Path -> void *
    std::mutex mutex_;
    // REVIEW (Mitch): Create named type that either wraps or type erases loaded library and loaded function handles.
    // REVIEW (Mitch): Use std::filesystem paths as keys?
    std::unordered_map<std::string, void*> loaded_libraries_;
    std::unordered_map<std::string, void*> loaded_library_functions_;
};

// REVIEW (Mitch): Use std filesystem path as first two args?
template<class T>
T* DllLoader::GetLibraryFunction(const std::string& lib_path,
                                 const std::string& lib_name,
                                 const std::string& function_name)
{
    // Construct the library path
    const auto& dll_path = lib_path + "/" + GetLibraryPrefix() + lib_name + GetLibrarySuffix();
    return (T*)GetLibraryFunction_(dll_path, function_name);
}

#endif // CORE_DLL_LOADER_H