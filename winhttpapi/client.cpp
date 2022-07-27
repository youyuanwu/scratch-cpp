#include "windows.h"
#include "winhttp.h"


#include <vector>
#include <string>
#include <iostream>

class HttpRequest{
public:
    std::wstring Method;
    std::wstring URL; // www.something.com
    ULONG Port;
    std::wstring Path;
};

class HttpResponse{
public:
    DWORD StatusCode;
    std::vector<BYTE> Data;
};

// int test(){
// using unique_handle_t = std::unique_ptr<HINTERNET, decltype(&WinHttpCloseHandle)>;
//     unique_handle_t fp(WinHttpOpen(L"SFROSAgent", 
//                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
//                                     WINHTTP_NO_PROXY_NAME,
//                                     WINHTTP_NO_PROXY_BYPASS,
//                                     0)
//                     , &WinHttpCloseHandle);
// }

ULONG MakeRequest(HttpRequest const & req, HttpResponse & resp){
    HINTERNET hSession = WinHttpOpen(L"SFROSAgent", 
                                    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME,
                                    WINHTTP_NO_PROXY_BYPASS,
                                    0);
    if(!hSession){
        return GetLastError();
    }

    HINTERNET hConnect = WinHttpConnect( hSession, req.URL.c_str(),
                                   req.Port, 0);
    if(!hConnect){
        ULONG lastError = GetLastError();
        WinHttpCloseHandle(hSession);
        return lastError;
    }

    //HINTERNET hRequest = WinHttpOpenRequest( hConnect, req.Method.c_str(), 
    //                                   req.Path.c_str(), 
    //                                   L"HTTP/1.1",
    //                                    WINHTTP_NO_REFERER, 
    //                                   WINHTTP_DEFAULT_ACCEPT_TYPES,
    //                                   0);
    const wchar_t *att[] = { L"application/json", NULL };
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
                                       NULL,
                                       NULL,
                                       WINHTTP_NO_REFERER,
                                       att,
                                       WINHTTP_FLAG_SECURE);
    if (!hRequest){
        ULONG lastError = GetLastError();
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);
        return lastError;
    } 

    BOOL    bResults = WinHttpSendRequest( hRequest, 
                                       WINHTTP_NO_ADDITIONAL_HEADERS,
                                       0, WINHTTP_NO_REQUEST_DATA, 0, 
                                       0, 0);
    if( !bResults ){
        ULONG lastError = GetLastError();
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hRequest);
        return lastError;
    }
    bResults = WinHttpReceiveResponse( hRequest, NULL );
    if( !bResults )
    {
        ULONG lastError = GetLastError();
        WinHttpCloseHandle(hSession);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hRequest);
        return lastError;
    }

    {
        DWORD dwSize = sizeof(resp.StatusCode);

        bResults = WinHttpQueryHeaders(hRequest, 
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
            WINHTTP_HEADER_NAME_BY_INDEX, 
            &resp.StatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
        if(!bResults){
            ULONG lastError = GetLastError();
            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            return lastError;
        }
    }
    
    std::vector<BYTE> & response = resp.Data;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    do 
    {
        // Check for available data.
        if( !WinHttpQueryDataAvailable( hRequest, &dwSize ) )
        {
            ULONG lastError = GetLastError();
            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            return lastError;
        }

        // Allocate space for the buffer.
        std::vector<BYTE> buffer(dwSize+1, 0);


        if( !WinHttpReadData( hRequest, (LPVOID)buffer.data(), 
                            dwSize, &dwDownloaded ) )
        {
            ULONG lastError = GetLastError();
            WinHttpCloseHandle(hSession);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hRequest);
            return lastError;
        }
        else
        {
            // data is here:
            response.insert(response.end(), buffer.begin(), buffer.begin() + dwSize);
        }
    } while( dwSize > 0 );

    WinHttpCloseHandle(hSession);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hRequest);
    return 0;
}

int main(){
     HttpRequest req;
    req.Method = L"GET";
    req.URL = L"api.github.com";
    req.Port = INTERNET_DEFAULT_HTTPS_PORT;
    req.Path = L"/";
    HttpResponse resp;
    ULONG retCode = MakeRequest(req, resp);

    if(retCode != 0){
        std::cout << "MakeRequest Failed with" << retCode << std::endl;
        return retCode;
    }
    std::cout << "status: " << resp.StatusCode;
    return 0;
}