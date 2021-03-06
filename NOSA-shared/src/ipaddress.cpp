#include "ipaddress.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include "helper.h"

IpAddress::IpAddress(const char* ip_string)
{
    try
    {
        int counter = 0;
        StringVector vector = Helper::split(ip_string,  '.');
        for (StringVector::iterator itr = vector.begin(); itr != vector.end(); ++itr)
            part[counter++] = atoi((*itr).c_str());
    }
    catch (int /*e*/)
    {
        setNull();
    }
}

bool IpAddress::operator==(const IpAddress& other)
{
    for (int i = 0; i < 4; ++i)
        if (part[i] != other.part[i])
            return false;

    return true;
}

bool IpAddress::isSameDomain(const IpAddress& other)
{
    if (part[0] != other.part[0] || part[1] != other.part[1])
        return false;

    if (abs((int)part[2] - (int)other.part[2]) > 1)
        return false;

    return true;
}

bool IpAddress::isNull()
{
    for (int i = 0; i < 4; ++i)
        if (part[i] != 0)
            return false;

    return true;
}

void IpAddress::setNull()
{
    for (int i = 0; i < 4; ++i)
        part[i] = 0;
}

std::string IpAddress::getString()
{
    std::ostringstream ss;
    for (int i = 0; i < 4; ++i)
    {
        if (i)
            ss << ".";
        ss << part[i];
    }

    return ss.str();
}
