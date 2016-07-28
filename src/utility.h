#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <QString>
#include <string>

inline std::string stdStringFromQString(const QString & input)
{
  #ifdef _WIN32
    return input.toLocal8Bit().constData();
  #else
    return input.toUtf8().constData();
  #endif
}

#endif
