#ifndef GLOBALINTERFACES_H
#define GLOBALINTERFACES_H

class OutEventPort{
    public:
        virtual void receive() = 0;
}

#endif //GLOBALINTERFACES_H