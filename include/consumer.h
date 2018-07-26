#ifndef CONSUMER_H
#define CONSUMER_H

#include <gsl/gsl>
#include <producer.h>

class consumer
{
public:

    
    explicit consumer(gsl::not_null<producer *> p)
    { p->print_msg(); }

    ~consumer() = default;

public:

    consumer(consumer &&) noexcept = default;               
    consumer &operator=(consumer &&) noexcept = default;    

    consumer(const consumer &) = delete;                    
    consumer &operator=(const consumer &) = delete;         
};

#endif
