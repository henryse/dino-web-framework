//
//  main.c
//  sinatra
//
//  Created by Henry Seurer on 4/19/14.
//  Copyright (c) 2014 Henry Seurer. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "dino.h"

GET(amor)
{
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

    char *lyrics = "When the moon hits you eye like a big pizza pie\n That's amore!\n\r";
    
    RESPONSE_SEND(lyrics, strlen(lyrics));


    RSP_HEADER_SET("Content-Type", "music");

    RSP_HEADER_SET("Content-Type", "text/html");

    RSP_HEADER_SET("Dino-Type", "Wine");

    return HTTP_ERROR_CODE_OK;
}

GET(sway)
{
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

    char *data = "Hello From GET 2!\n";
    
    RESPONSE_SEND(data, strlen(data));
    
    RESPONSE_SEND(data, strlen(data));
    
    RSP_HEADER_SET("Content-Type", "music");
    
    RSP_HEADER_SET("Content-Type", "text/html");
    
    RSP_HEADER_SET("Dino-Type", "Wine");
    
    return HTTP_ERROR_CODE_OK;
}

POST(main)
{
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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
    for (int i = 0; i < PARAMS_COUNT; i++)
    {
        RESPONSE_SEND(PARAM_KEY(i), strlen(PARAM_KEY(i)));
        RESPONSE_SEND(":", 1);
        RESPONSE_SEND(PARAM_VALUE(i), strlen(PARAM_VALUE(i)));
        RESPONSE_SEND("\n\r", 2);
    }

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

