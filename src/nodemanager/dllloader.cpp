#include "dllloader.h"

#include <iostream>
#include <chrono>

#ifdef _WIN32
    #include <windows.h>
    #include <stdio.h>
#else
    #include <dlfcn.h>
#endif

DllLoader::~DllLoader(){
    std::lock_guard<std::mutex> lock(mutex_);

    for(auto& l : loaded_libraries_){
        if(!CloseLibrary(l.second)){
            std::cerr << "DLL Error: Cannot Close " << l.first + " : " << GetLibraryError() << std::endl;
        }
    }
    loaded_libraries_.clear();
}

std::string DllLoader::GetLibrarySuffix() const{
     #ifdef _WIN32
        return ".dll";
    #else
        return ".so";
    #endif
}

std::string DllLoader::GetLibraryPrefix() const{
     #ifdef _WIN32
        return "";
    #else
        return "lib";
    #endif
}

std::string DllLoader::GetLibraryError(){
    std::string message;
    #ifdef _WIN32
        DWORD error_id = ::GetLastError();
        if(!error_id){
            return message;
        }
        LPSTR buffer = nullptr;
        //Fill the buffer and get the size.
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
        //Copy into our return parameter
        message = std::string(buffer, size);

        //Free the buffer.
        LocalFree(buffer);
    #else
        char* error = dlerror();
        if(error){
            message = std::string(error);
        }
    #endif
    return message;
}

void* DllLoader::GetLibrary(const std::string& dll_path){
    void* library = 0;
    
    std::lock_guard<std::mutex> lock(mutex_);
    if(!loaded_libraries_.count(dll_path)){
        auto start = std::chrono::steady_clock::now();

        #ifdef _WIN32
            library = LoadLibrary(dll_path.c_str());
        #else
            //Get a handle to the dynamically linked library
            library = dlopen(dll_path.c_str(), RTLD_LAZY);
        #endif

        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        //Check for errors
        const auto& error = GetLibraryError();

        if(library && error.empty()){
            std::cout << "* Loaded DLL: '" << dll_path <<  "' In: " << ms.count() << " us" << std::endl;
            loaded_libraries_[dll_path] = library;
        }else{
            throw std::runtime_error( "DLL Error: Cannot load '" + dll_path + "' : " + error);
            library = 0;
        }
    }else{
        library = loaded_libraries_[dll_path];
    }

    return library;
}

void* DllLoader::GetLibraryFunction_(const std::string& dll_path, const std::string& function_name){
    //Check in map
    auto lookup_str = dll_path + "_" + function_name;
    void* library_func = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        library_func = loaded_library_functions_.count(lookup_str) ? loaded_library_functions_[lookup_str] : 0;
    }
    
    
    if(!library_func){
        auto lib_handle = GetLibrary(dll_path);
        if(lib_handle){
            //Gain the mutex
            std::lock_guard<std::mutex> lock(mutex_);

            auto start = std::chrono::steady_clock::now();
            void* function = nullptr;
            #ifdef _WIN32
                function = (void*) GetProcAddress((HMODULE)lib_handle, function_name.c_str());
            #else
                function = dlsym(lib_handle, function_name.c_str());
            #endif
        
            auto end = std::chrono::steady_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
            auto error = GetLibraryError();

            if(function && error.empty()){
                loaded_library_functions_[lookup_str] = function;
                library_func = function;
            }else{
                throw std::runtime_error("DLL Error: Linking '" + function_name + "' " + error);
            }
        }
    }
    return library_func;
}

bool DllLoader::CloseLibrary(void* library){
    bool closed = false;
    if(library){
        #ifdef _WIN32
            closed = FreeLibrary((HMODULE)library);
        #else
            closed = dlclose(library) == 0;
        #endif
    }
    return closed;
}