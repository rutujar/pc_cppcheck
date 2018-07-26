#include <producer.h>
#include <consumer.h>

int
MAIN(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    producer p{};
    consumer c{&p};

    return 0;
}
