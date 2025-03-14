#include "tag.h"
#include "srview.h"

extern CSrView	*g_pMainView;
extern bool g_DebugDisplay;

CTag::CTag(QString& TagName) : m_sTagName(TagName)
{
    m_bChanged = false;
    m_Value = 0;

}

CTag::~CTag()
{
//	while (!m_RunList.isEmpty())
//		delete m_RunList.takeFirst();
//	while (!m_ParserList.isEmpty())
//		delete m_ParserList.takeFirst();
}

QString& CTag::GetTagName()
{
    return m_sTagName;
}

quint16 CTag::GetAtom()
{
    return m_nAtom;
}

void CTag::setRunList(CRunObject *pRun)
{

    if(m_RunList.contains(pRun) == false)
        m_RunList.append(pRun);
    if(m_bChanged)
        pRun->Exec(m_Value);
}

void CTag::removeRunList(CSrView* pView)
{

    for(int i = 0; i < m_RunList.size(); i++)
    {
        CRunObject *pRun = m_RunList[i];
        if(((CSrView*)pRun->m_pParent) == pView)
        {
            m_RunList.removeAt(i);

            // KKK Close할때 에러 (CParser::LookupVariable의 	if(m_nMode == AddMode) 삭제시)

            delete pRun;
        }
    }

    for(int i = 0; i < m_ParserList.size(); i++)
    {
        CParser *pRun = m_ParserList[i];
        if(pRun->m_pView == pView)
        {
            m_ParserList.removeAt(i);
            delete pRun;
        }
    }
}

void CTag::setParserList(CParser *pParser)
{
    m_ParserList.append(pParser);
//	if(m_bChanged)
//		pParser->Exec(this);
}

void CTag::setAtom(quint16 atom)
{
    m_nAtom = atom;
}

const char* ALM_STATUS_MSG[6] = {
    "Normal",
    "Digital",
    "LoLo",
    "Low",
    "High",
    "HiHi"
};
void CTag::setAlarmStatus(quint16 status)
{
    m_nAlmStatus = status;
    if(status < 6)
        m_sAlmStatus = ALM_STATUS_MSG[status];
}

void CTag::setTagStatus(quint16 status)
{
    m_nTagStatus = status;
    if(status == 0)
        m_sTagStatus = "Normal";
    else
    if(status & 0x01)
        m_sTagStatus = "OffLine";
    else
    if(status & 0x02)
        m_sTagStatus = "Under";
    else
    if(status & 0x04)
        m_sTagStatus = "Over";
}

void CTag::setValue(double value)
{
    if(m_bChanged && m_Value == value)
        return;
    m_bChanged = true;
    m_Value = value;
    if(g_DebugDisplay)
        qDebug() << "Tag, Val : " << m_sTagName << "," << value;
    for (int i = 0; i < m_RunList.size(); ++i) {
        CRunObject* pRunObj = m_RunList.at(i);
        pRunObj->sendSignal();
        pRunObj->Exec(value);
    }
    for (int i = 0; i < m_ParserList.size(); ++i) {
        CParser *pParser = m_ParserList.at(i);
        pParser->Exec();
    }
}

void CTag::setValue(QString value)
{
    if(m_bChanged && m_sValue == value)
        return;
    m_bChanged = true;
    m_sValue = value;
    if(g_DebugDisplay)
        qDebug() << "Tag, Val : " << m_sTagName << "," << value << m_RunList.size();
    for (int i = 0; i < m_RunList.size(); ++i) {
        CRunObject* pRunObj = m_RunList.at(i);
        pRunObj->sendSignal();
        pRunObj->Exec(value);
    }
}

void CTag::Update()
{
    m_bChanged = true;
    if(g_DebugDisplay)
        qDebug() << "Update Tag, Val, size" << m_sTagName << "," << m_Value << "," << m_RunList.size();
    for (int i = 0; i < m_RunList.size(); ++i) {
        CRunObject* pRunObj = m_RunList.at(i);
        pRunObj->sendSignal();
        pRunObj->Exec(m_Value);
    }
    for (int i = 0; i < m_ParserList.size(); ++i) {
        CParser *pParser = m_ParserList.at(i);
        pParser->Exec();
    }
}
void CTag::writeValue(double value, int delay)
{
    QString val = QString().number(value, 'g', 15);
    g_pMainView->writeTag(m_sTagName, val, delay);
//   setValue(value);
}

void CTag::writeValue(QString value, int delay)
{
    g_pMainView->writeTag(m_sTagName, value, delay);
//   setValue(value);
}
