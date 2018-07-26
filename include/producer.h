#ifndef PRODUCER_H
#define PRODUCER_H

#include <iostream>

class producer
{
public:

    producer() = default;

    VIRTUAL ~producer() = default;

    VIRTUAL void print_msg()
    { std::cout << "hello world\n"; }

public:


    producer(producer &&) noexcept = default;               
    producer &operator=(producer &&) noexcept = default;    

    producer(const producer &) = delete;                    
    producer &operator=(const producer &) = delete;         
};

#endif
