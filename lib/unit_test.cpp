
#include <iostream>
#include <cstdlib>

void utAssertTrueImpl( bool ok, char const * t, char const * fil, int ln )
{
    if( ok ) return;
    
    std::cout << "failure: " << t << " " << fil << "(" << ln << ")\n";
    exit(1);
}
