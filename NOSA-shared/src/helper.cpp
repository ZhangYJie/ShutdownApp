#include "helper.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <Iphlpapi.h>
#endif

#define MAC_SIZE 17
#define IPV6_SIZE 40

StringVector &Helper::split(const std::string &s, char delim, StringVector &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);

    return elems;
}


StringVector Helper::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string Helper::trim(std::string s)
{
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    return s;
}

std::string Helper::toLowerCase(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string Helper::replace(std::string s, const char *from, const char *to)
{
    std::size_t start_pos = 0;
    while((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, strlen(from), to);
        start_pos += strlen(to);
    }

    return s;
}

#ifdef _WIN32
std::string Helper::GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName )
{
   HKEY hKey = 0;
   BYTE data[512] ;
   DWORD szsize = 512 ;
   std::string value ;

   LONG retValue = RegOpenKeyEx( openKey, regkey, 0, KEY_READ, &hKey ) ;

   if ( retValue == ERROR_SUCCESS )
   {
        LONG retV = RegQueryValueEx( hKey, keyName, 0, 0, data, &szsize ) ;
        if ( retV == ERROR_SUCCESS )
        {
           char* _value = reinterpret_cast<char*>(data) ;
           value = _value ;

           RegCloseKey (hKey) ;
           return value ;
        }
        else
        {
        }
    }
    else
    {

    }

    RegCloseKey (hKey) ;
    return "" ;
}

const char* Helper::getMAC(IpAddress* clientIp = NULL, IpAddress* serverIp = NULL)
{
    IP_ADAPTER_INFO* adapterInfo;
    DWORD dwBufLen = sizeof(adapterInfo);
    char *mac_addr;

    adapterInfo = new IP_ADAPTER_INFO[sizeof(IP_ADAPTER_INFO)];
    if (adapterInfo == NULL)
        std::cout << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << std::endl;

    if (GetAdaptersInfo(adapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        delete[] adapterInfo;
        adapterInfo = new IP_ADAPTER_INFO[dwBufLen];
        if (adapterInfo == NULL)
            std::cout << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << std::endl;
    }

    if (GetAdaptersInfo(adapterInfo, &dwBufLen) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        PIP_ADAPTER_INFO candidate = NULL;

        do {
            IpAddress ipAddress = pAdapterInfo->IpAddressList.IpAddress.String;
            if (ipAddress.isNull())
                continue;

            if (serverIp)
            {
                if (ipAddress == *serverIp)
                {
                    candidate = pAdapterInfo;
                    break;
                }
            }
            else
            {
                if (candidate)
                {
                    if (clientIp)
                    {
                        if (clientIp->isSameDomain(ipAddress))
                        {
                            candidate = pAdapterInfo;
                            break;
                        }
                    }
                }
                else
                {
                    candidate = pAdapterInfo;
                    if (!clientIp)
                        break;
                }
            }

        } while (pAdapterInfo = pAdapterInfo->Next);

        if (!candidate)
            return NULL;

        mac_addr = new char[MAC_SIZE];
        _snprintf_s(mac_addr, _TRUNCATE, MAC_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X",
            candidate->Address[0], candidate->Address[1],
            candidate->Address[2], candidate->Address[3],
            candidate->Address[4], candidate->Address[5]);

    }

    delete adapterInfo;
    return mac_addr;
}
#endif

bool Helper::iequals(const std::string& a, const std::string& b)
{
    return a.size() == b.size() && toLowerCase(a) == toLowerCase(b);
}

const char* Helper::to_string(int val)
{
    char str[64];
    sprintf(str, "%d", val);
    return strdup(str);
}

const char* Helper::to_string(float val)
{
    char str[64];
    sprintf(str, "%f", val);
    return strdup(str);
}

const char* Helper::stripNewLine(const char* ori)
{
    if (ori[strlen(ori)-1] == '\n')
    {
        return strndup(ori, strlen(ori)-1);
    }

    return ori;
}

char* Helper::strndup(const char *s, size_t n) {
#ifdef _WIN32
    char* res = new char[n+1];
    strncpy(res, s, n);
    res[n] = '\0';
    return res;
#else
    return ::strndup(s, n);
#endif
}

void Helper::printBytes(char **bytes, unsigned printMax)
{
    if (!printMax)
        printMax = strlen(*bytes);

    std::cout << "Len: " << strlen(*bytes) << ", bytes: ";
    for (int i = 0; i < printMax; ++i)
        std::cout << (int) (*bytes)[i] << " ";

    std::cout << std::endl;
}

bool Helper::file_exists(const std::string &name)
{
    struct stat fileInfo;
    return stat(name.c_str(), &fileInfo) == 0;
}

std::string Helper::fromDecimal(ullint n, ullint b)
{
    char buffer[17];
    buffer[16] = '\0';
    int c;
    for(int i = 15; i >= 0; --i)
    {
        c = n % b;
        buffer[i] = c < 10 ? 48+c : 87+c;
        n /= b;
    }

    return strdup(buffer);
}

size_t Helper::position_of_char(const char *text, char ch) {
    char* end = (char*)memchr(text, ch, strlen(text));
    if (!end)
        return std::string::npos;

    return end - text;
}

bool Helper::request_privileges(LPCSTR value)
{
    LPCSTR privileges[] = { value };
    return request_privileges(privileges);
}

bool Helper::request_privileges(LPCSTR values[])
{
    HANDLE hToken = NULL;

    size_t count = sizeof(LPCSTR)/sizeof(values);

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
    int offset = FIELD_OFFSET(TOKEN_PRIVILEGES, Privileges[count]);
    PTOKEN_PRIVILEGES tkp = (PTOKEN_PRIVILEGES) malloc(offset);

    tkp->PrivilegeCount = count;
    for(size_t i = 0; i < count; ++i) {
        LookupPrivilegeValue(NULL, values[i], &tkp->Privileges[i].Luid);
        tkp->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
    }

    AdjustTokenPrivileges(hToken, FALSE, tkp, 0, NULL, 0);
    CloseHandle(hToken);

    return true;
}

std::string Helper::getPathToFile(const char *fileName, bool extraBackslash)
{
    char szHelpFileName[_MAX_FNAME];

    GetModuleFileName(NULL, szHelpFileName, sizeof(szHelpFileName));
    char *cp = strrchr(szHelpFileName, '\\');
    if(cp)
       ++cp;
    else
    {
       cp = strrchr(szHelpFileName, ':');
       if( cp )
          ++cp;
       else
          cp = szHelpFileName;
    }
    strcpy( cp, fileName );

    if (extraBackslash)
        return Helper::replace(szHelpFileName, "\\", "\\\\");
    else
        return szHelpFileName;
}

char* Helper::ipv6_string(in6_addr& addr)
{
    u_short* part = (u_short*)&addr.u.Word;
    char buffer[IPV6_SIZE];
    _snprintf_s(buffer, _TRUNCATE, IPV6_SIZE, "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
        part[0], part[1],
        part[2], part[3],
        part[4], part[5],
        part[6], part[7]);

    buffer[IPV6_SIZE-1] = '\0';

    return strdup(buffer);
}
