TODO List
====

() Convert into a lib

() Make Autoconf work.

() Create \lib directory for lib files.

() Create \sample directory for sample programs

() Fill out documentation.

() How to do unit tests?

() Get RESPONSE_PRINTF working

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

Include the dino.lib in your project.

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

Then define a function that will be called when the route is invoked on a given path:

~~~C
GET(amor)
{
    char *lyrics = "When the moon hits you eye like a big pizza pie\n That's amore!\n";
    
    RESPONSE_SEND(lyrics, strlen(lyrics));

    return HTTP_ERROR_CODE_OK;
}
~~~

So your simple program should look something like:

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

Parameters (Params)
====

Routes
====

In Dino, a route is an HTTP method paired with a URL-matching pattern. Each route is associated a call back function defined in a CONFIG block:

~~~C
int main(int argc, const char * argv[])
{
    int port = 3031;
    char *host = "localhost";
    
    DINO_CONFIG_START(port, host);
        ROUTE_GET(amor, "/")
        ROUTE_GET(sway, "/:wine/:bottle")
        ROUTE_POST(main, "/")
        ROUTE_DELETE(main, "/")
        ROUTE_PUT(main, "/")
        ROUTE_OPTIONS(main, "/")
        ROUTE_HEAD(main, "/")
        ROUTE_TRACE(main, "/")
        ROUTE_CONNECT(volare, "/")
    DINO_CONFIG_END;
    
    DINO_START;
    
    return 0;
}
~~~

The config block starts off with the port and the host that the server will listen on:

~~~C
DINO_CONFIG_START(port, host);
~~~

Then each route is bound to a name, for example amor is bound to "http://host:port/" and sway is bound to "http://host:port/sway"

~~~C
	ROUTE_GET(amor, "/")
	ROUTE_GET(sway, "/smile")
~~~

Dino also support parameter binding. so if the route is defined as:

~~~C
	ROUTE_GET(sway, "/:wine/:bottle")
~~~

The parameter list will bind what ever appears at :wine and :bottle to values at those location.  For example:

~~~
http://host:port/red/glass
~~~

Will cause :wine == red and :bottle == glass in the parameter list. 

API
====

Configuration
====

DINO_CONFIG_START(port, host)

ROUTE_GET(route_name, path)

ROUTE_POST(route_name, path)

ROUTE_DELETE(route_name, path)

ROUTE_PUT(route_name, path)

ROUTE_OPTIONS(route_name, path)

ROUTE_HEAD(route_name, path)

ROUTE_TRACE(route_name, path)

ROUTE_CONNECT(route_name, path)

DINO_CONFIG_END

Start/Stop
====

DINO_START

DINO_STOP


HTTP Methods
====

GET(route_name)

POST(route_name)

PUT(route_name)

DELETE(route_name)

OPTIONS(route_name)

HEAD(route_name)

TRACE(route_name)

CONNECT(route_name)

Operations
====

RESPONSE_SEND(data, size)

RESPONSE_PRINTF(restrict, ...)

RSP_HEADER_SET(key, value)

PARAMS(key)

PARAMS_COUNT

PARAM_KEY(index)

PARAM_VALUE(index)

Helpers
====

Params list to pass dino parameters

DINO_VARS

DINO_DEF_VARS