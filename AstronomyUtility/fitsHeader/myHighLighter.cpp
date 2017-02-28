#include "myHighLighter.h"

myHighLighter::myHighLighter(QTextEdit *parent) :
    QSyntaxHighlighter(parent)
{
    m_hightlightRules.clear();
}

void myHighLighter::setMyHighLighterColor(const QString &str, \
                                          const QColor &color, \
                                          const QColor &bgcolor, \
                                          QTextDocument::FindFlags options)
{
    myHighLightRule rule;
    qDebug()<<"options = "<<options;
    if(options==0x0 || options==0x1)//forward
        rule.pattern = QRegExp(str,Qt::CaseInsensitive); //<TODO:CaseSensitive>
    else if(options==0x2 || options==0x3)//forward + FindCaseSensitively
        rule.pattern = QRegExp(str,Qt::CaseSensitive);
    else if(options==0x4 || options==0x5)//forward + FindWholeWords
        rule.pattern = QRegExp("\\b"+str+"\\b",Qt::CaseInsensitive);
    else if(options==0x6 || options==0x7)//forward + FindWholeWords + FindCaseSensitively
        rule.pattern = QRegExp("\\b"+str+"\\b",Qt::CaseSensitive); //this means "\bstr\b".
    QTextCharFormat format;
    format.setForeground(color);
    format.setBackground(bgcolor);

    rule.format = format;
    m_hightlightRules.append(rule);
}

void myHighLighter::highlightBlock(const QString &text)
{
    foreach(myHighLightRule rule, m_hightlightRules)
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while(index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index+length);
        }
    }
}

void myHighLighter::clearRules()
{
    m_hightlightRules.clear();
}
