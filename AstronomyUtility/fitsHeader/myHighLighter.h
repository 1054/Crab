#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QRegExp>
#include <QTextEdit>
#include <QtDebug>
#define   m_noneColor QColor(0,0,0,0)

class QTextDocument;
class myHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    myHighLighter(QTextEdit *parent = 0);
    void setMyHighLighterColor(const QString &str, \
                               const QColor &color, \
                               const QColor &bgcolor=m_noneColor,\
                               QTextDocument::FindFlags options=0);
    void highlightBlock(const QString &text);
    void clearRules();
private:
    struct myHighLightRule{
        QRegExp pattern;
        QTextCharFormat format;};
    QVector<myHighLightRule> m_hightlightRules;

};

#endif // MYHIGHLIGHTER_H
