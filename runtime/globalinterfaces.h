#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

//Interface for a standard In Event Port
template <class message_type> class InEventPort{
    public:
    //protected:
        virtual void rx_(message_type*) = 0;
};

//Interface for a standard Out Event Port
template <class message_type> class OutEventPort{
    public:
    //protected:
        virtual void tx_(message_type*) = 0;
};

#endif //GLOBALINTERFACES_H
