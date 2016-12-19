#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

#define EXPORT_FUNC __attribute__((visibility("default")))

//Interface for a standard In Event Port
template <class T> class InEventPort{
    public:
        virtual void rx_(T*) = 0;
};

//Interface for a standard Out Event Port
template <class T> class OutEventPort{
    public:
        virtual void tx_(T*) = 0;
};

#endif //GLOBALINTERFACES_H
