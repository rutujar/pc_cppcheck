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

    consumer(consumer &&) noexcept = default;               ///< Default move construction
    consumer &operator=(consumer &&) noexcept = default;    ///< Default move operator

    consumer(const consumer &) = delete;                    ///< Deleted copy construction
    consumer &operator=(const consumer &) = delete;         ///< Deleted copy operator
};

#endif
