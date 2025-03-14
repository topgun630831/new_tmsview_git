#include "pollmsg.h"

extern bool	g_bDebug;
extern QMap<QString, CTag*> m_TagMap;
extern QMap<quint16, CTag*> m_AtomTagMap;

CPollMsg::CPollMsg( CWMINet *pPro, quint32 Seq, quint16 Cmd, quint16 SubCmd, int Time, bool DeleteAble, bool bNoRetry )
{
    m_bAdd = false;
    m_pWMINet = pPro;
    Due        = true;
    ScanTimer  = ScanReload = ( int )Time;
    m_bDeleteAble = DeleteAble;
    Retries = 3;
    m_nMaxLen = MAXMSGLEN;
    if( bNoRetry )
        Retries = 0;
    m_nIndex = 0;
    m_pHeader = (WMINET_HEADER *) Message;
    strcpy(m_pHeader->ID, "WMI0");
    m_pHeader->Seq = Seq;
    m_pHeader->Len = 0;
    m_pHeader->CmdGroup = Cmd;
    m_pHeader->SubCmd = SubCmd;
    m_nIndex = sizeof(WMINET_HEADER);
    if(m_pHeader->CmdGroup == WMI_CMD_RTDATA && m_pHeader->SubCmd == WMI_SCMD_ADDTAG)
        m_nIndex += 4;
    m_bEnd = false;
}

CPollMsg::~CPollMsg()
{
}

void CPollMsg::SetSeq(quint32 Seq)
{
    m_pHeader->Seq = Seq;
}

void CPollMsg::ReMake( void )
{
    m_nIndex = 3;
    char str[ 256 ];
    for (int i = 0; i < m_TagList.size(); ++i) {
        QString Tag = m_TagList.at(i);
/*		int len = Tag.length();
        *( quint32 * )&str[ 0 ] = ( quint32 )len;
        strcpy( ( char* )&str[ 4 ], ( char* )Tag.toLocal8Bit().data());
*/
        QByteArray ba = Tag.toUtf8();
        int len = ba.length();
        *( quint32 * )&str[ 0 ] = ( quint32 )len;
        memcpy( ( char* )&str[ 4 ], ( char* )ba.data(), len);
        Add( str, len+4 );
    }
}

void CPollMsg::End( void )
{
    m_nLast = m_nIndex;
    m_pHeader->Len = m_nIndex - sizeof(WMINET_HEADER);
    if(m_pHeader->CmdGroup == WMI_CMD_RTDATA && m_pHeader->SubCmd == WMI_SCMD_ADDTAG)
    {
        quint32 *cnt = (quint32*)&Message[sizeof(WMINET_HEADER)];
        *cnt =   (quint32)m_TagList.size();
    }
    m_bEnd = true;
    m_bAdd = false;
}

bool CPollMsg::AddTag( QString& TagName)
{
//	int len = TagName.length();
    char str[ 256 ];

//	*( quint32 * )&str[ 0 ] = ( quint32 )len;
//	strcpy( ( char* )&str[ 4 ], ( char* )TagName.toLocal8Bit().data());
    QByteArray ba = TagName.toUtf8();
    int len = ba.length();
    *( quint32 * )&str[ 0 ] = ( quint32 )len;
    memcpy( ( char* )&str[ 4 ], ( char* )ba.data(), len);
    str[4+len] = 0;

    if(g_bDebug)
        qWarning() << "AddTag tag=%s len =%d m_nIndex=%d" << &str[4] << len << m_nIndex;

    if(m_bEnd)
    {
        m_nIndex = m_nLast;
        m_bEnd = false;
    }
    if(Add( str, len+4 ))
    {
        m_TagList.append(TagName);
        return true;
    }
    return false;
}

bool CPollMsg::Add( char *str, int len )
{
    int RealLen = (len + 3) & 0xfffffffc;

    if((m_nIndex + RealLen) > m_nMaxLen)
        return false;

    memcpy(&Message[m_nIndex], str, len);
    memset(&Message[m_nIndex+len], 0, RealLen-len);

    m_nIndex += RealLen;
    return true;
}

bool CPollMsg::AddWrite( quint32 atom, bool type, double value, QString &str, bool ack )
{
    atom = atom;
    ack = ack;
    char msg[ 512 ];

    if( type )
    {
//		*( quint32 * )&msg[ 0 ] = ( quint32 ) str.length();
//		strcpy( ( char* )&msg[ 4 ], ( char* )str.toLocal8Bit().data());

        QByteArray ba = str.toUtf8();
        int len = ba.length();
        *( quint32 * )&msg[ 0 ] = ( quint32 ) len;
        memcpy( ( char* )&msg[ 4 ], ba.data(), len);

        return Add( msg, len+4 );
    }

    *( float* )&msg[ 0 ] = ( float )value;

    return Add( msg, 4 );
}

void CPollMsg::TimeCheck( int MsecChange )
{
    if( !Due  )
    {
        if( ( ScanTimer -= MsecChange ) <= 0 )
        {
            Due = true;
            ScanTimer = ScanReload;
        }
    }
}

void CPollMsg::ScanReset()
{
    ScanTimer = ScanReload;
}

// STX(0) SEQ(1) CMD(2) DATA(3) ------------DATA(Len-2) ETX(Len-1)
bool CPollMsg::ExtractData( char *Buffer)
{

    quint16	   atom, status;
    float	   val;
    QString    str;
    char	   *buf;
    quint32	   len, cnt;
    quint32	   i;
    quint16	   idx = sizeof(WMINET_HEADER);
    WMINET_HEADER *pRcvHeader = (WMINET_HEADER *)Buffer;
    if(pRcvHeader->SubCmd & 0x8000)
    {
        len = *( quint32* )&Buffer[ idx ];
        idx+=sizeof( quint32 );
        buf = new char[ len+1 ];
        memcpy( buf, &Buffer[ idx ], len );
        idx+=len;
        buf[ len ] = 0;
        qWarning() << "WMINet Err(%s)" << buf;
        delete [] buf;
        return false;
    }

    switch(pRcvHeader->CmdGroup)
    {
        case WMI_CMD_CONNECT:
            if(g_bDebug)
                qWarning() << "WMI_CMD_CONNECT";
            len = *( quint32* )&Buffer[ idx ];
            idx+=sizeof( quint32 );
            buf = new char[ len+1 ];
            memcpy( buf, &Buffer[ idx ], len );
            idx+=len;
            buf[ len ] = 0;
            m_pWMINet->setInitEnd();
            delete [] buf;
            break;
        case WMI_CMD_RTDATA:
            if(m_pWMINet->isInitEnd() != true)
                break;
            {
                switch(pRcvHeader->SubCmd)
                {
                    case WMI_SCMD_OPENPAGE:
                        break;
                    case WMI_SCMD_ADDTAG:
                        cnt = *( quint32* )&Buffer[ idx ];
                        idx += sizeof( quint32 );
                        if(m_TagList.size() < (int)cnt)
                            cnt = (quint32)m_TagList.size();
                        for(i = 0; i < cnt; i++)
                        {
                            atom = (quint16)(*( quint32* )&Buffer[ idx ] & 0xffff);
                            status = (quint16)((*( quint32* )&Buffer[ idx ] & 0xffff) >> 16) ;
                            idx+=sizeof( quint32 );

                            QString Tag = m_TagList.at(i);

                            if(g_bDebug)
                            {
                                QString str;
                                str = "Tag = ";
                                str += Tag;
                                str += "  Atom = ";
                                str += QString::number(atom);
                                qWarning() << str;
                            }

                            SetTagAtom(Tag, atom, status);
                        }

                        break;
                    case WMI_SCMD_VALREQ:

                        cnt = *( quint32* )&Buffer[ idx ];
                        idx += sizeof( quint32 );
                        for(i = 0; i < cnt; i++)
                        {
                            atom = (quint16)(*( quint32* )&Buffer[ idx ] & 0xffff);
                            status = (quint16)((*( quint32* )&Buffer[ idx ] & 0xffff) >> 16) ;
                            idx+=sizeof( quint32 );
                            val = *( float* )&Buffer[ idx ];
                            if(g_bDebug)
                            {
                                QString str;
                                str = "ExtractData Atom = ";
                                str += QString::number(atom);
                                str += "  Value = ";
                                /*for(int i = 0; i < 4; i++)
                                {
                                    str += QString::number((int)Buffer[ idx+i ], 16);
                                    str += " ";
                                }*/
                                str += QString::number(val);
                                qWarning() << str;
                            }
                            idx+=sizeof( float );
                            while(m_pWMINet->m_bEnding);
                            SetTagValue(atom, status, val);
                        }
                        break;
                    case WMI_SCMD_CONTROLTAG:
                        break;
                }
            }
    }

    return true;
}

void CPollMsg::SetTagAtom(QString& TagName,quint16 atom, quint16 status)
{
    CTag* pTag;
    if(m_TagMap.contains(TagName))
    {
        pTag = m_TagMap[TagName];
        if(pTag == NULL)
            return;
        pTag->setAtom(atom);
        pTag->setStatus(status);
        m_AtomTagMap[atom] = pTag;
    }
}

void CPollMsg::SetTagValue(quint16 atom, quint16 status, float value)
{
    CTag* pTag;
    if(m_AtomTagMap.contains(atom))
    {
        pTag = m_AtomTagMap[atom];
        pTag->setStatus(status);
        pTag->setValue(value);
    }
}
