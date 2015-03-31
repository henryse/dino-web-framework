//    Copyright (c) 2014 Henry Seurer
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE.
//

#include <stdio.h>
#include <string.h>
#include "dino.h"

bool enum_params(DINO_DECLARE_VARS, const char *key, const char *value, const void *obj)
{
    RESPONSE_SEND(key, strlen(key));
    RESPONSE_SEND(":", 1);
    RESPONSE_SEND(value, strlen(value));
    RESPONSE_SEND("\n\r", 2);
    
    if ( obj )
    {
        RESPONSE_PRINTF(obj);
    }
    
    return true;
}

GET(amor)
{
    PARAMS_ENUMERATE(enum_params, "Check Pass");

    char *lyrics = "When the moon hits you eye like a big %s %s pizza pie\n That's amore!\n\r";
    char *ingredientOne = "pineapple pepperoni";
    char *magicIngredient = "anchovies";
    RESPONSE_PRINTF(lyrics, ingredientOne, magicIngredient);


    RSP_HEADER_SET("Content-Type", "music");

    RSP_HEADER_SET("Content-Type", "text/html");

    RSP_HEADER_SET("Dino-Type", "Wine");

    return HTTP_ERROR_CODE_OK;
}

void sway_test(DINO_DECLARE_VARS)
{
    char *data = "Hello From GET sway_test!\n";
    
    RESPONSE_SEND(data, strlen(data));
}

GET(sway)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    sway_test(DINO_VARS);
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

POST(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From POST!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

DELETE(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From DELETE!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

PUT(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From PUT!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

OPTIONS(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From OPTIONS!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

HEAD(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From HEAD!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

TRACE(main)
{
    PARAMS_ENUMERATE(enum_params, NULL);


    char *data = "Hello From TRACE!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

CONNECT(volare)
{
    PARAMS_ENUMERATE(enum_params, NULL);

    char *data = "Hello From CONNECT!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

int main(int argc, const char * argv[])
{
    int port = 3030;
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

