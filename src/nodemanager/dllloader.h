#ifndef CORE_DLL_LOADER_H
#define CORE_DLL_LOADER_H

#include <mutex>
#include <string>
#include <unordered_map>

class DllLoader {
public:
    ~DllLoader();

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
    std::unordered_map<std::string, void*> loaded_libraries_;
    std::unordered_map<std::string, void*> loaded_library_functions_;
};

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