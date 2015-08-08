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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#ifndef dino_http_error_h
#define dino_http_error_h

#define HTTP_ERROR_CODE_DESCRIPTION                     0        /* Reference */
#define HTTP_ERROR_CODE_CONTINUE                        100        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.2.1] */
#define HTTP_ERROR_CODE_SWITCHING_PROTOCOLS             101        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.2.2] */
#define HTTP_ERROR_CODE_PROCESSING                      102        /* [RFC2518] */

#define HTTP_ERROR_CODE_OK                              200        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.1] */
#define HTTP_ERROR_CODE_CREATED                         201        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.2] */
#define HTTP_ERROR_CODE_ACCEPTED                        202        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.3] */
#define HTTP_ERROR_CODE_NON_AUTHORITATIVE_INFORMATION    203        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.4] */
#define HTTP_ERROR_CODE_NO_CONTENT                      204        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.5] */
#define HTTP_ERROR_CODE_RESET_CONTENT                   205        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.3.6] */
#define HTTP_ERROR_CODE_PARTIAL_CONTENT                 206        /* [RFC-ietf-httpbis-p5-range-26, Section 4.1] */
#define HTTP_ERROR_CODE_MULTI_STATUS                    207        /* [RFC4918] */
#define HTTP_ERROR_CODE_ALREADY REPORTED                208        /* [RFC5842] */
#define HTTP_ERROR_CODE_IM_USED                         226        /* [RFC3229] */

#define HTTP_ERROR_CODE_MULTIPLE CHOICES                300        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.1] */
#define HTTP_ERROR_CODE_MOVED PERMANENTLY               301        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.2] */
#define HTTP_ERROR_CODE_FOUND                           302        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.3] */
#define HTTP_ERROR_CODE_SEE_OTHER                       303        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.4] */
#define HTTP_ERROR_CODE_NOT_MODIFIED                    304        /* [RFC-ietf-httpbis-p4-conditional-26, Section 4.1] */
#define HTTP_ERROR_CODE_USE_PROXY                       305        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.5] */
#define HTTP_ERROR_CODE_TEMPORARY_REDIRECT              307        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.4.7] */
#define HTTP_ERROR_CODE_PERMANENT REDIRECT              308        /* [RFC-reschke-http-status-308-07] */

#define HTTP_ERROR_CODE_BAD_REQUEST                     400        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.1] */
#define HTTP_ERROR_CODE_UNAUTHORIZED                    401        /* [RFC-ietf-httpbis-p7-auth-26, Section 3.1] */
#define HTTP_ERROR_CODE_PAYMENT_REQUIRED                402        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.2] */
#define HTTP_ERROR_CODE_FORBIDDEN                       403        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.3] */
#define HTTP_ERROR_CODE_NOT_FOUND                       404        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.4] */
#define HTTP_ERROR_CODE_METHOD_NOT_ALLOWED              405        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.5] */
#define HTTP_ERROR_CODE_NOT_ACCEPTABLE                  406        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.6] */
#define HTTP_ERROR_CODE_PROXY_AUTHENTICATION_REQUIRED    407        /* [RFC-ietf-httpbis-p7-auth-26, Section 3.2] */
#define HTTP_ERROR_CODE_REQUEST_TIMEOUT                 408        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.7] */
#define HTTP_ERROR_CODE_CONFLICT                        409        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.8] */
#define HTTP_ERROR_CODE_GONE                            410        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.9] */
#define HTTP_ERROR_CODE_LENGTH_REQUIRED                 411        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.10] */
#define HTTP_ERROR_CODE_PRECONDITION_FAILED             412        /* [RFC-ietf-httpbis-p4-conditional-26, Section 4.2] */
#define HTTP_ERROR_CODE_PAYLOAD_TOO_LARGE               413        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.11] */
#define HTTP_ERROR_CODE_URI_TOO_LONG                    414        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.12] */
#define HTTP_ERROR_CODE_UNSUPPORTED MEDIA TYPE            415        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.13] */
#define HTTP_ERROR_CODE_REQUESTED_RANGE_NOT_SATISFIABLE    416        /* [RFC-ietf-httpbis-p5-range-26, Section 4.4] */
#define HTTP_ERROR_CODE_EXPECTATION_FAILED              417        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.14] */
#define HTTP_ERROR_CODE_UNPROCESSABLE ENTITY            422        /* [RFC4918] */
#define HTTP_ERROR_CODE_LOCKED                          423        /* [RFC4918] */
#define HTTP_ERROR_CODE_FAILED_DEPENDENCY               424        /* [RFC4918] */
#define HTTP_ERROR_CODE_UPGRADE_REQUIRED                426        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.5.15] */
#define HTTP_ERROR_CODE_PRECONDITION REQUIRED            428        /* [RFC6585] */
#define HTTP_ERROR_CODE_TOO MANY REQUESTS               429        /* [RFC6585] */
#define HTTP_ERROR_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE    431        /* [RFC6585] */

#define HTTP_ERROR_CODE_INTERNAL_SERVER_ERROR            500        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.1] */
#define HTTP_ERROR_CODE_NOT_IMPLEMENTED                 501        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.2] */
#define HTTP_ERROR_CODE_BAD_GATEWAY                     502        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.3] */
#define HTTP_ERROR_CODE_SERVICE_UNAVAILABLE             503        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.4] */
#define HTTP_ERROR_CODE_GATEWAY_TIMEOUT                 504        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.5] */
#define HTTP_ERROR_CODE_HTTP_VERSION_NOT_SUPPORTED        505        /* [RFC-ietf-httpbis-p2-semantics-26, Section 6.6.6] */
#define HTTP_ERROR_CODE_VARIANT_ALSO_NEGOTIATES            506        /* [RFC2295] */
#define HTTP_ERROR_CODE_INSUFFICIENT_STORAGE            507        /* [RFC4918] */
#define HTTP_ERROR_CODE_LOOP_DETECTED                   508        /* [RFC5842] */
#define HTTP_ERROR_CODE_NOT_EXTENDED                    510        /* [RFC2774] */
#define HTTP_ERROR_CODE_NETWORK_AUTHENTICATION_REQUIRED    511        /* [RFC6585] */

#endif

#pragma clang diagnostic pop