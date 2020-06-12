#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

#if defined(_WIN32) || defined(__WIN32__)
	#define EXPORT_FUNC __declspec(dllexport)
#else
	#define EXPORT_FUNC __attribute__((visibility("default")))
#endif

#endif //GLOBALINTERFACES_H