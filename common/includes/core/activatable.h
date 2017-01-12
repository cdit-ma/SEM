#ifndef ACTIVATABLE_H
#define ACTIVATABLE_H

class Activatable{  
    public:
        virtual bool activate();
        virtual bool passivate();
        const bool is_active();
   private:
        bool active_ = false;
};

#endif //ACTIVATABLE_H