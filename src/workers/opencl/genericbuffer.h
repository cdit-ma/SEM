#ifndef GENERICBUFFER_H
#define GENERICBUFFER_H



class GenericBuffer {
public:
    GenericBuffer(int id);
    
    bool is_valid();
    
    int GetID() const;

protected:
    int id_ = -1;
    bool valid_ = false;
};

#endif