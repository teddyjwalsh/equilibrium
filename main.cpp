#include "noise.h"
#include "map.h"

int main()
{
    equilibrium::Map em(300,300);

    for (int i = 0; i < 1000; ++i)
    {
        em.iterate();
    }
}
