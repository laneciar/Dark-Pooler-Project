#ifndef H_WEBCURL_H
#define H_WEBCURL_H
#include <curl/curl.h>
#include <iostream>

using namespace std;
//CurlObj is used to get the html from the given webpage
class CurlObj {
public:
    CurlObj (std::string url) {
        CURLcode res;

        curl = curl_easy_init();
        if (!curl)
        {
            throw std::string ("Curl did not initialize.");
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlObj::curlWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
              fprintf(stderr, "curl_easy_perform() failed: %s\n",
                      curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    };

    static int curlWriter(char *data, int size, int nmemb, std::string *buffer) {
        int result = 0;
        if (buffer != NULL) {
            buffer->append(data, size * nmemb);
            result = size * nmemb;
        }

        return result;
    }

    std::string getData() const noexcept{
        return curlBuffer;
    }

protected:
    CURL * curl{nullptr};
    std::string curlBuffer;
};
#endif // H_WEBCURL_H

