
# Welcome!

In my professional career I use Java and C++ but my favorite two languages are C and Ruby.   I wanted to create a web server framework for my Raspberry PI that would be simple and memory efficient.  This is not intended as High Performance Framework like NGINX or a Reactor Pattern like JavaRX.  It is simply an easy to use framework to create simple web services for small systems in C.

This framework's targeted platforms are:
* Debian Raspberry PI
* Debian Intel
* Mac OS X

The inspiration for this framework came from folks at [Sinatra](http://www.sinatrarb.com), hence it is named of after that other famous member of the rat pack: Dino or Dean Martin. 

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
    unsigned short port = 9080;
    char *host = "localhost";
    const char *application_name="dino_example";
    
    DINO_CONFIG_START_HTTP(application_name, host, port, true);  
        ROUTE_GET(amor, "/amor")
        ROUTE_GET(base, "/")
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

GET(base)
{
    char *response = "Welcome Home!";
    
    RESPONSE_PRINTF("Response %s\n", response);

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

GET(base)
{
    char *response = "Welcome Home!";
    
    RESPONSE_PRINTF("Response %s\n", response);

    return HTTP_ERROR_CODE_OK;
}

int main(int argc, const char * argv[])
{
    int port = 3030;
    char *host = "localhost";
    const char *application_name="dino_example";
    bool logging = true;
    
    DINO_CONFIG_START_HTTP(application_name, host, port, logging);  
        ROUTE_GET(amor, "/amor")
        ROUTE_GET(base, "/")
    DINO_CONFIG_END;
    
    DINO_START;
    
    return 0;
}
~~~

That's it! Fire up a browser and point it at http://localhost:3030

# Parameters (Params)

Dino considers HTTP headers and parameters on the command line to all be "parameters".  They are stored as key:value pairs.  The key:value pairs are stored in a string hash map.  The parameters can be accessed via API Macros:

~~~C
const char *PARAMS(key)
size_t PARAMS_COUNT                    
void PARAMS_ENUMERATE(callback, obj) 
~~~

## PARAMS

If you want the "first" given value for the "key".  Please note that if there is more than one, you will only get the first on that was found. 

In order to iterate through the list you can use PARAMS_ENUMERATE.


## PARAMS_COUNT

Then total number of key:value pairs in this request.

## PARAMS_ENUMERATE

#Routes

In Dino, a route is an HTTP method paired with a URL-matching pattern. Each route is associated a call back function defined in a CONFIG block:

~~~C
int main(int argc, const char * argv[])
{
    int port = 3031;
    char *host = "localhost";
    const char *application_name="dino_example";
    
    DINO_CONFIG_START_HTTP(application_name, host, port, true);  
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
DINO_CONFIG_START_HTTP(application_name, host, port, true);  
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

Installing
====

To install is simple, pull this project.   Use "cmake" to generate the necessary make files, and then run make:

~~~
cmake .
make
make install
~~~


API
====

Configuration
----

DINO_CONFIG_START_HTTP(application_name, host, port, logging);  
This line is required to declare the beginning of the route configuration.  
* application_name - application name to be used for logging
* host - host or ip address t listen on
* port - port to listen on
* logging - true/false to enable sys logging.

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
----

DINO_START

DINO_STOP


HTTP Methods
----

GET(route_name)

POST(route_name)

PUT(route_name)

DELETE(route_name)

OPTIONS(route_name)

HEAD(route_name)

TRACE(route_name)

CONNECT(route_name)

Sending Responses
----

RESPONSE_SEND(data, size)

RESPONSE_PRINTF(restrict, ...)

RSP_HEADER_SET(key, value)

Parameter Enumeration
----

PARAMS(key)

PARAMS_COUNT

PARAMS_ENUMERATE(callback, obj) 


Helpers
----

Params list to pass dino parameters

DINO_VARS

DINO_DECLARE_VARS


TODO List
====

() Fill out documentation.

() Use curl or wget to create unit tests.

() Need to find a way to parse results from curl or wget

