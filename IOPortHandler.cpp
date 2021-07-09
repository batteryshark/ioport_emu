#include "IOPortHandler.h"

void IOPortHandler::register_handler(unsigned short port, io_function handler){
    PortHandler* ph = new PortHandler();
    ph->port = port;
    ph->func_handler = handler;
    this->handlers.push_back(ph);
}

void IOPortHandler::deregister_handler(unsigned short port){
    for(int i = 0; i < this->handlers.size();i++){
        if(port == this->handlers[i]->port){
            this->handlers.erase(this->handlers.begin()+i);
        }
    } 
}

void IOPortHandler::process(unsigned short port, unsigned int* value){
       for(int i = 0; i < this->handlers.size();i++){
        if(port == this->handlers[i]->port){
            this->handlers[i]->func_handler(value);
            return;            
        }
    } 
}




