#ifndef __DOUBLEVALIDATOR_H__
#define __DOUBLEVALIDATOR_H__

#include <QDoubleValidator>

namespace FabricSplice
{
  
  class DoubleValidator : public QDoubleValidator
  {
  public:
      DoubleValidator(QObject * parent)
        : QDoubleValidator(parent)
      {
      }

      DoubleValidator(double bottom, double top, int decimals, QObject * parent)
        : QDoubleValidator(bottom, top, decimals, parent)
      {
      }

      QValidator::State validate(QString &s, int &i) const
      {
          if (s.isEmpty() || s == "-") {
              return QValidator::Intermediate;
          }

          QLocale locale;

          QChar decimalPoint = locale.decimalPoint();
          int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

          if (charsAfterPoint > decimals()) {
              return QValidator::Invalid;
          }

          bool ok;
          double d = locale.toDouble(s, &ok);

          if (ok && d >= bottom() && d <= top()) {
              return QValidator::Acceptable;
          } else {
              return QValidator::Invalid;
          }
      }
  };
	
}


#endif
