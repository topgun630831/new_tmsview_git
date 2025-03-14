#ifndef TAG_H
#define TAG_H
#include <QString>
#include "runani.h"
#include "srview.h"
//#include "parser.h"

class CTag
{
public:
    CTag(QString& TagName);
    ~CTag();
    QString&	GetTagName();
    quint16		GetAtom();
    quint16		GetAlmStatus() {return m_nAlmStatus;};
    quint16		GetTagStatus() {return m_nTagStatus;};
    void		setAtom(quint16 atom);
    void		setAlarmStatus(quint16 status);
    void		setTagStatus(quint16 status);
    void		setValue(double value);
    void		setValue(QString value);
    void		Update();
    quint16		isHLAlarm() {return m_nAlmStatus;}
    quint16		isDevAlarm() {return m_nAlmStatus;}
    quint16		isRocAlarm() {return m_nAlmStatus;}
    void		setRunList(CRunObject *pRun);
    void	removeRunList(CSrView* pView);
    void		setParserList(CParser *pParser);
    void		writeValue(double value, int deley=0);
    void		writeValue(QString value, int deley=0);
    void        setTagType(int type) { m_TagType = type;};
    void        setDesc(QString desc) { m_sDesc = desc;};
    void        setUnit(QString unit) { m_sUnit = unit;};
    void        setRw(QString Rw) { m_sRw = Rw;};
    void        setRoundUp(int roundup) { m_nRoundUp = roundup;};
    QString     getDesc() { return m_sDesc;};
    QString     getUnit() { return m_sUnit;};
    QString     getRw() { return m_sRw;};
    int         getTagType() { return m_TagType;};
    double      getValue() { return m_Value;};
    QString     getValueS() { return m_sValue;};
    QString     getAlmStatusS() { return m_sAlmStatus;};
    QString     getTagStatusS() { return m_sTagStatus;};
    int         getRoundUp() { return m_nRoundUp;};

    double		m_Value;
    QString		m_sValue;
    quint16		m_nAlmStatus;
    quint16		m_nTagStatus;
    QString     m_sAlmStatus;
    QString 	m_sTagStatus;
    bool	m_bMajorDev;
    bool	m_bMinorDev;
    bool	m_bROC;
    bool	m_bHiLo;
    int     m_nInput;
    QString m_Addr;
    QString m_OnMsg;
    QString m_OffMsg;
private:
    quint16		m_nAtom;
    quint16		m_TagStatus;
    QString		m_sTagName;
    QString		m_sDesc;
    QString		m_sUnit;
    QString		m_sRw;
    RUNLIST		m_RunList;
    PARSERLIST	m_ParserList;
    bool		m_bChanged;
    int         m_TagType;
    int         m_nRoundUp;
};

#endif // TAG_H
