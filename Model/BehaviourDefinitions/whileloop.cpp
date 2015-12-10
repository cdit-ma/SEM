#include "whileloop.h"

WhileLoop::WhileLoop():Branch(){

}

WhileLoop::~WhileLoop(){

}

bool WhileLoop::canAdoptChild(Node *node)
{
    if(hasChildren()){
        return false;
    }
    return Branch::canAdoptChild(node);
}


