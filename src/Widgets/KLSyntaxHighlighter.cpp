#include "KLSyntaxHighlighter.h"

using namespace FabricSplice;

KLSyntaxHighlighter::KLSyntaxHighlighter(QTextDocument * document)
: QSyntaxHighlighter(document)
{

  HighlightingRule rule;

  //keywordFormat.setFontWeight(QFont::Bold);
  keywordFormat.setForeground( QColor(255, 0, 127) );
  //classFormat.setFontWeight(QFont::Bold);
  classFormat.setForeground(Qt::cyan);

  // functionName 166, 226, 46

  singleLineCommentFormat.setForeground(QColor( 117, 113, 94));
  multiLineCommentFormat.setForeground(QColor( 117, 113, 94));

  quotationFormat.setForeground(Qt::yellow);

  numberFormat.setForeground(QColor(147, 112, 219));

  keywordPatterns 
    << "<<<"
    << ">>>"
    << "\\babs\\b"
    << "\\bacos\\b"
    << "\\basin\\b"
    << "\\batan\\b"
    << "\\bbreak\\b"
    << "\\bconst\\b"
    << "\\bcontinue\\b"
    << "\\bcos\\b"
    << "\\bcreateArrayGenerator\\b"
    << "\\bcreateConstValue\\b"
    << "\\bcreateReduce\\b"
    << "\\bcreateValueGenerator\\b"
    << "\\belse\\b"
    << "\\bfor\\b"
    << "\\bfunction\\b"
    << "\\bif\\b"
    << "\\bin\\b"
    << "\\bio\\b"
    << "\\boperator\\b"
    << "\\breport\\b"
    << "\\breturn\\b"
    << "\\bsetError\\b"
    << "\\bsin\\b"
    << "\\bsqrt\\b"
    << "\\bstruct\\b"
    << "\\btan\\b"
    << "\\btype\\b"
    << "\\buse\\b"
    << "\\brequire\\b"
    << "\\bvar\\b"
    << "\\btrue\\b"
    << "\\bfalse\\b"
    << "\\bValueProducer\\b"
    << "\\bwhile\\b";

  foreach (const QString &pattern, keywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }


  classPatterns 
    << "\\bBoolean\\b"
    << "\\bByte\\b"
    << "\\bInteger\\b"
    << "\\bSize\\b"
    << "\\bIndex\\b"
    << "\\bSInt32\\b"
    << "\\bSInt64\\b"
    << "\\bUInt64\\b"
    << "\\bScalar\\b"
    << "\\bFloat32\\b"
    << "\\bFloat64\\b"
    << "\\bString\\b"
    << "\\bVec2\\b"
    << "\\bVec3\\b"
    << "\\bVec4\\b"
    << "\\bQuat\\b"
    << "\\bEuler\\b"
    << "\\bRotationOrder\\b"
    << "\\bMat22\\b"
    << "\\bMat33\\b"
    << "\\bMat44\\b"
    << "\\bXfo\\b"
    << "\\bMath\\b"
    << "\\bColor\\b"
    << "\\bRGB\\b"
    << "\\bRGBA\\b"
    << "\\bBoundingBox3\\b"
    << "\\bRay\\b"
    << "\\bInlineDrawing\\b"
    << "\\bManipulationIntersection\\b"
    << "\\bKeyFrame\\b"
    << "\\bKeyFrameTrack\\b"
    << "\\bLSystem\\b"
    << "\\bLSystemRule\\b"
    << "\\bLSystemWord\\b"
    << "\\bBezierXfo\\b"
    << "\\bPolygonMesh\\b"
    << "\\bGeometryAttributes\\b"
    << "\\bScalarAttribute\\b"
    << "\\bVec2Attribute\\b"
    << "\\bVec3Attribute\\b"
    << "\\bVec4Attribute\\b"
    << "\\bRGBAttribute\\b"
    << "\\bRGBAAttribute\\b"
    << "\\bColorAttribute\\b"
    << "\\GeometryLocation\\b"
    << "\\bScalarSliderManipulator\\b"
    << "\\bVec2SliderManipulator\\b"
    << "\\bPositionManipulator\\b"
    << "\\bRotationManipulator\\b"
    << "\\bTransformManipulator\\b";

  foreach (const QString &pattern, classPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = classFormat;
    highlightingRules.append(rule);
  }

  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  rule.pattern = QRegExp("\\\"([^\\\"]|\\\\.)*\\\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  rule.pattern = QRegExp("\\\'([^\\\']|\\\\.)*\\\'");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  commentStartExpression = QRegExp("/\\*");
  commentEndExpression = QRegExp("\\*/");
}

bool KLSyntaxHighlighter::isKeyWord(const QString & word)
{
  if(word.length() == 0)
    return false;
  QString pattern = "\\b";
  pattern += word;
  pattern += "\\b";
  for(int i=0;i<keywordPatterns.length();i++)
  {
    if(keywordPatterns[i] == pattern)
      return true;
  }
  for(int i=0;i<classPatterns.length();i++)
  {
    if(classPatterns[i] == pattern)
      return true;
  }
  return false;
}

void KLSyntaxHighlighter::highlightBlock(const QString & text)
{
  foreach (const HighlightingRule &rule, highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn(text);

  while (startIndex >= 0)
  {
    int endIndex = commentEndExpression.indexIn(text, startIndex);
    int commentLength;
    if (endIndex == -1)
    {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    }
    else
    {
      commentLength = endIndex - startIndex
        + commentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
  }
}
