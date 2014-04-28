Getting Started
====

The inspiration for the framework came from Sinatra, hence the name Dino or Dean Martin. 

Dino is a simple web framework for quickly creating web applications in C with minimal effort.  

Simply include the Dino header:

~~~C
#include <stdio.h>
#include <string.h>
#include "dino.h"  /* <--- Here is the header you need */
~~~

Inlcude the dino.lib in your project.

Define a route by binding it to a path:

~~~C
int main(int argc, const char * argv[])
{
    int port = 3030;
    char *host = "localhost";
    
    DINO_CONFIG_START(port, host);
        ROUTE_GET(amor, "/")
    DINO_CONFIG_END;
    
    DINO_START;
    
    return 0;
}
~~~

Then define a fuction that will be called when the route is invoked on a given path:

~~~C
GET(amor)
{
    char *lyrics = "When the moon hits you eye like a big pizza pie\n That's amore!\n";
    
    RESPONSE_SEND(lyrics, strlen(lyrics));

    return HTTP_ERROR_CODE_OK;
}
~~~

So your simple program should look someting like:

~~~C
#include <stdio.h>
#include <string.h>
#include "dino.h"  /* <--- Here is the header you need */

GET(amor)
{
    char *lyrics = "When the moon hits you eye like a big pizza pie\n That's amore!\n";
    
    RESPONSE_SEND(lyrics, strlen(lyrics));

    return HTTP_ERROR_CODE_OK;
}

int main(int argc, const char * argv[])
{
    int port = 3030;
    char *host = "localhost";
    
    DINO_CONFIG_START(port, host);
        ROUTE_GET(amor, "/")
    DINO_CONFIG_END;
    
    DINO_START;
    
    return 0;
}
~~~

That's it! Fire up a browser and point it at http://localhost:3030



