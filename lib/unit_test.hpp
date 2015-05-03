
#define utAssertTrue(expr) utAssertTrueImpl( (expr), #expr, __FILE__, __LINE__ )

void utAssertTrueImpl( bool ok, char const * t, char const * fil, int ln );
