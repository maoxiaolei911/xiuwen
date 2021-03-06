/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZNODE_H
#define XWTIKZNODE_H

#include <QStringList>
#include <QList>

#include "XWTikzOperation.h"

class QUndoCommand;
class XWTikzCommand;
class XWTIKZOptions;
class XWTeXBox;
class XWTikzMatrix;
class XWTikzAnnotationArrow;

class XWTikzCoordinate : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCoordinate(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTikzCoordinate(XWTikzGraphic * graphicA, int idA,QObject * parent = 0);

  virtual bool back(XWTikzState * state);

  virtual bool cut(XWTikzState * state);

  virtual bool del(XWTikzState * state);
  virtual void doCompute(XWTikzState * state);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  virtual void dragTo(XWTikzState * state);
  virtual bool dropTo(XWTikzState * state);

  QString getName() {return name;}

  virtual QPointF getAnchor(int a, XWTikzState * state);
  virtual QPointF getAnchor(XWTikzState * state);
  virtual int     getAnchorPosition();
  virtual QPointF getAngle(double a, XWTikzState * state);
  virtual QPointF getBorder(const QPointF & p,XWTikzState * state);
  XWTeXBox * getBox() {return box;}
  virtual int     getCursorPosition();
  virtual QString getCurrentText();
  virtual QPointF getPoint(XWTikzState * state);
  virtual QVector3D getPoint3D(XWTikzState * state);
  virtual double getRadius(XWTikzState * state);
  virtual QString getSelectedText();
  virtual QString getText();
  virtual QString getTextForPath();
  virtual void goToEnd();
  virtual bool goToNext();
  virtual void goToOperationEnd();  
  virtual void goToOperationStart();
  virtual void goToPathEnd();
  virtual void goToPathStart();
  virtual bool goToPrevious();
  virtual void goToStart();

  virtual bool hitTest(XWTikzState * state);

          void insert(int i, XWTikzOperation * opA);
  virtual bool insertText(XWTikzState * state);
  virtual bool isMe(const QString & nameA,XWTikzState * state);
  

  virtual bool keyInput(XWTikzState * state);

  virtual bool newPar(XWTikzState * state);

  virtual bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);
  virtual void scanOption(const QString & str, int & len, int & pos);
  virtual void scanText(const QString & str, int & len, int & pos);
  virtual void setText(const QString & str);
  virtual void setName(const QString & str);

  XWTikzOperation * takeAt(int i);

public slots:
  void addChild();
  void addNode();

protected:
  void scanChildren(const QString & str, int & len, int & pos);

protected:
  XWTIKZOptions * options;
  XWTeXBox * box;
  int cur;
  QString name;
  QList<XWTikzOperation*> children;
};

class XWTikzNode : public XWTikzCoordinate
{
  Q_OBJECT

public:
  XWTikzNode(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu, XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  QString getContent();
  int     getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  QString getTextForPath();
  bool    goToNext();
  bool    goToPrevious();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void scanText(const QString & str, int & len, int & pos);
  void setText(const QString & str);
};

class XWTikzEdge : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzEdge(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu, XWTikzState * state);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  bool dropTo(XWTikzState * state);

  int     getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(XWTikzState * state);

  void insert(int i, XWTikzCoordinate * node);
  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

  XWTikzCoordinate * takeAt(int i);

private slots:
  void addNode();
  void removeNode();

private:
  XWTIKZOptions * options;
  XWTikzCoord * coord;
  int cur;
  QList<XWTikzCoordinate*> nodes;
};

#define XW_TIKZ_LABEL_NOANGLE -1

class XWTikzLabel : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzLabel(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);
  XWTikzLabel(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  QString getAngle();
  QString getContent();
  int     getCursorPosition();
  QString getCurrentText();
  QString getOptions();
  QString getSelectedText();
  virtual QString getText();
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);
  void setAngle(const QString & str);
  void setText(const QString & str);

protected:
  XWTIKZOptions * options;
  double angle;
  XWTeXBox * box;
};

class XWTikzChild : public XWTikzCoordinate
{
  Q_OBJECT

public:
  XWTikzChild(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu, XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  bool dropTo(XWTikzState * state);

  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  QStringList variables;
  QStringList list;
};

class XWTikzEdgeFromParent : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzEdgeFromParent(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTIKZOptions * options;
};

class XWTikzOperationText : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzOperationText(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  void scan(const QString & str);

private:
  QString text;
  QList<XWTikzOperation *> ops;
};

class XWTikzPathText : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzPathText(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  void scan(const QString & str);

private:
  QString text;
  QList<XWTikzCommand *> cmds;
};

class XWTikzNodePart : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzNodePart(XWTikzGraphic * graphicA, QObject * parent = 0);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  QString text;
};

class XWTikzUnit : public XWTikzLabel
{
  Q_OBJECT

public:
  XWTikzUnit(XWTikzGraphic * graphicA, 
             const QString & nameA,
             QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
  void    getUnit(QString & nameA, QString & valueA);

  void scan(const QString & str, int & len, int & pos);
  void setUnit(const QString & nameA,const QString & valueA);
  void setValue(const QString & valueA);

private:
  QString name;
  QString value;
};

#endif //XWTIKZNODE_H
