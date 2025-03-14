#include "wminet.h"


#include "tmsinfo.h"

extern bool	g_bDebug;

CWMINet::CWMINet()
{
    m_pEventReqMsg = NULL;
    m_ptcpSocket = NULL;
    m_nLastconnectmSec = 0;
    m_pCommMon = NULL;
    m_bEnding = false;
    m_nCurrmSec = 0;
}

CWMINet::~CWMINet()
{
    if( m_pEventReqMsg )
        delete m_pEventReqMsg;
    if(m_ptcpSocket)
        delete m_ptcpSocket;
    while (!m_listWrite.isEmpty())
    {
        m_pCurPollMsg = m_listWrite.takeFirst();
        delete m_pCurPollMsg;
    }
    while (!m_listInit.isEmpty())
        delete m_listInit.takeFirst();
}

void CWMINet::initial(const QString& ipAddr, int port)
{
    m_nPort = port;
    m_ipAddr = ipAddr;

    m_ptcpSocket = new QTcpSocket(this);

   // connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(m_ptcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_ptcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(m_ptcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
         this, SLOT(displayError(QAbstractSocket::SocketError)));

    m_strName = tr("LCD");
    MakeEventReq();

}

void CWMINet::initialComm(QWidget *pCommMon, const QString& ipAddr, int port, int type)
{
    m_nPort		= port;
    m_ipAddr	= ipAddr;
    m_pCommMon	= pCommMon;
    m_nType		= type;

    m_ptcpSocket = new QTcpSocket(this);

   // connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(m_ptcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_ptcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(m_ptcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
         this, SLOT(displayError(QAbstractSocket::SocketError)));

    m_strName = tr("LCD");
    if( m_pEventReqMsg )
        delete m_pEventReqMsg;
    if(type != 0)
        m_pEventReqMsg = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_VALREQ, 5000, false, false );
    else
        m_pEventReqMsg = new CPollMsg( this, 0, WMI_CMD_COMMMON, WMI_SCMD_IDLE, 5000, false, false );
    m_pEventReqMsg->End();

}

void CWMINet::commandWrite(quint32 seq, quint16 Cmd, quint16 SubCmd)
{
    CPollMsg*	pPollMsg;
    pPollMsg = new CPollMsg( this, seq, Cmd, SubCmd, 0, true, false );
    pPollMsg->End();
    AddWritePoll(pPollMsg);
}

void CWMINet::deviceSelect(QString& device, QString& topic)
{
    char msg[120];
    CPollMsg*	pPollMsg;
    pPollMsg = new CPollMsg( this, 0, WMI_CMD_COMMMON, WMI_SCMD_TOPIC_SELECT, 0, true, false );
    *( quint32* )&msg[0] = ( quint32 )device.length();
    memcpy(&msg[4], device.toLocal8Bit().data(), device.length());
    pPollMsg->Add(msg, device.length()+4);
    *( quint32* )&msg[0] = ( quint32 )topic.length();
    memcpy(&msg[4], topic.toLocal8Bit().data(), topic.length());
    pPollMsg->Add(msg, topic.length()+4);
    quint32 level = 2;
    pPollMsg->Add((char*)&level, 4);
    pPollMsg->End();
    AddWritePoll(pPollMsg);
}

void CWMINet::parameterChange(QString& tagname, QString& parameter, QString& value)
{
    char msg[120];
    CPollMsg*	pPollMsg;
    pPollMsg = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_PARAMETER, 0, true, false );

    *( quint32* )&msg[0] = ( quint32 )tagname.length();
    memcpy(&msg[4], tagname.toLocal8Bit().data(), tagname.length());
    pPollMsg->Add(msg, tagname.length()+4);

    quint32 noOfItem = 1;
    pPollMsg->Add((char*)&noOfItem, 4);

    *( quint32* )&msg[0] = ( quint32 )parameter.length();
    memcpy(&msg[4], parameter.toLocal8Bit().data(), parameter.length());
    pPollMsg->Add(msg, parameter.length()+4);

    *( quint32* )&msg[0] = ( quint32 )value.length();
    memcpy(&msg[4], value.toLocal8Bit().data(), value.length());
    pPollMsg->Add(msg, value.length()+4);
    pPollMsg->End();
    AddWritePoll(pPollMsg);
}

void CWMINet::disconnect()
{
    m_ptcpSocket->disconnectFromHost();
}

void CWMINet::setInitEnd(bool end)
{
    m_bInitEnd = end;
}
bool CWMINet::isInitEnd()
{
    return m_bInitEnd;
}
void CWMINet::connectToHost()
{
    m_ptcpSocket->abort();
    m_ptcpSocket->connectToHost(m_ipAddr, m_nPort);
}

void CWMINet::onTimer()
{
    m_nCurrmSec += 100;

    if(m_ptcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        if(m_pCommMon)
            m_nState = PROT_IDLE;
        if(m_nState == PROT_IDLE)
            ProtPoll();
        else
        if(m_pCommMon == NULL && (m_nCurrmSec - m_nSendmSec) > 3000)
        {
            qWarning() << "disconnect";
            m_ptcpSocket->disconnectFromHost();
        }
        m_nLastconnectmSec = m_nCurrmSec;
    }
    else
    {
        //qWarning("m_nCurrmSec = %d, m_nLastconnectmSec=%d", m_nCurrmSec, m_nLastconnectmSec);
        if(m_ptcpSocket->state() == QAbstractSocket::UnconnectedState && (m_nCurrmSec > m_nLastconnectmSec+3000))
        {
//			qWarning() << "Connect....";
            connectToHost();
            m_nLastconnectmSec = m_nCurrmSec;
        }
    }
}

void CWMINet::connected()
{

    while (!m_listWrite.isEmpty())
    {
        m_pCurPollMsg = m_listWrite.takeFirst();
        if(m_pCurPollMsg->m_bDeleteAble)
            delete m_pCurPollMsg;
    }

    m_cSeq = 0;
    m_bInitEnd = false;
    m_nRspIndex = 0;
    m_pCurPollMsg = NULL;
    m_nState = PROT_IDLE;
    m_dwLastMsec = m_nCurrmSec;

    char str[120];
    CPollMsg *pPoll = new CPollMsg( this, 0, WMI_CMD_CONNECT, WMI_SCMD_INIT, 0, true, false );
    if(m_pCommMon && m_nType == 0)
        *( quint32* )&str[ 0 ] = WMI_CMD_COMMMON;	// Command group : Communication Monitor Data
    else
        *( quint32* )&str[ 0 ] = WMI_CMD_RTDATA;	// Command group : Realtime Data
    *( quint32* )&str[ 4 ] = ( quint32 )m_strName.length();
    strcpy((char*)&str[8], m_strName.toLocal8Bit().data());
    pPoll->Add( str, m_strName.length()+8 );
    pPoll->End();
    AddWritePoll(pPoll);
    for (int i = 0; i < m_listInit.size(); ++i) {
        pPoll = m_listInit.at(i);
        AddWritePoll(pPoll );
    }
    AllEnd();
}

void CWMINet::readyRead()
{
    qint32 len = m_ptcpSocket->bytesAvailable();
    len = m_ptcpSocket->read(m_TempBuffer, len);
    if(g_bDebug)
    {
        QString str;
        QTime localTime = QTime::currentTime(); // Qt::LocalTime
        str = localTime.toString(tr("hh:mm:ss"));
        str += " Recv(%d):";
        for(int i = 0; i < len; i++)
        {	QString tmp;
            if(m_TempBuffer[i] >= 0x30 && m_TempBuffer[i] < 'z')
                tmp = m_TempBuffer[i];
            else
                tmp.sprintf("%02X ", m_TempBuffer[i]);
            str += tmp;
        }
        qWarning() << str << len;
    }
    int index = 0;
    while(len > 0)
    {

        int pass = DoProtocol(&m_TempBuffer[index], len);
        if(pass < 0)
            return;
        len -= pass;
        index += pass;
    }
}

void CWMINet::disconnected()
{
    qWarning() << "Disonnected";
}

 void CWMINet::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
//         QMessageBox::information(tr("Fortune Client"),
  //                                tr("The host was not found. Please check the "
    //                                 "host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
 //        QMessageBox::information(this, tr("Fortune Client"),
   //                               tr("The connection was refused by the peer. "
     //                                "Make sure the fortune server is running, "
       //                              "and check that the host name and port "
         //                            "settings are correct."));
         break;
     default:
         ;
//         QMessageBox::information(this, tr("Fortune Client"),
  //                                tr("The following error occurred: %1.")
    //                              .arg(m_ptcpSocket->errorString()));
     }

}

void CWMINet::MakeInitMsg( QString& TagName)
{
    CPollMsg *pPollMsg = NULL;

    if( m_listInit.isEmpty())
        m_listInit.append( new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_ADDTAG, 0, false, true ) );

    int size = m_listInit.size();
    pPollMsg = m_listInit[size-1];
    if( !pPollMsg->AddTag(TagName))
    {
        pPollMsg = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_ADDTAG, 0, false, true );
        pPollMsg->AddTag(TagName);
        m_listInit.append(pPollMsg );
    }
}

void CWMINet::AllEnd( void )
{
    for (int i = 0; i < m_listInit.size(); ++i) {
        CPollMsg *pPollMsg = m_listInit.at(i);
        pPollMsg->End();
    }
}

void CWMINet::InitDelete( void )
{
    while (!m_listInit.isEmpty())
         delete m_listInit.takeFirst();
}

void CWMINet::ReMake( void )
{
    CPollMsg *pPoll = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_OPENPAGE, 0, true, false );
    pPoll->End();
    AddWritePoll(pPoll);

    for (int i = 0; i < m_listInit.size(); ++i) {
        pPoll = m_listInit.at(i);
        AddWritePoll(pPoll );
    }
    AllEnd();
    if( m_pEventReqMsg )
        m_pEventReqMsg->Due = true;
}

void CWMINet::MakeEventReq( void )
{
    if( m_pEventReqMsg )
        delete m_pEventReqMsg;
    m_pEventReqMsg = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_VALREQ, 1000, false, false );
    m_pEventReqMsg->End();
}

void CWMINet::AddWritePoll( QString strTagName, bool type, double value, QString& str, bool ack )
{
    char msg[ 256 ];
    CPollMsg*	pPollMsg;
    pPollMsg = new CPollMsg( this, 0, WMI_CMD_RTDATA, WMI_SCMD_CONTROLTAG, 0, true, false );
//	*( quint32* )&msg[0] = ( quint16 )strTagName.length();
//	strcpy( ( char* )&msg[4], strTagName.toLocal8Bit().data());
//	pPollMsg->Add( msg, strTagName.length()+4 );

    QByteArray ba = strTagName.toUtf8();
    quint16 len = ba.length();
    *( quint32* )&msg[0] = len;
    memcpy( ( char* )&msg[4], ba.data(), len);
    pPollMsg->Add( msg, len+4 );
    pPollMsg->AddWrite( 0, type, value, str, ack );
    pPollMsg->End();
    AddWritePoll(pPollMsg);
}

bool CWMINet::SendQuery( void )
{
    if( !m_pCurPollMsg )
    {
        return false;
    }

    m_nSendmSec = m_nCurrmSec;
    if(m_pCommMon == NULL)
        m_pCurPollMsg->SetSeq(m_cSeq++);
    if(g_bDebug)
    {
        QString str;
        QTime localTime = QTime::currentTime();
        str = localTime.toString(tr("hh:mm:ss"));
        str += " Send : (%d)";
        for(int i = 0; i < m_pCurPollMsg->m_nIndex; i++)
        {	QString tmp;
            tmp.sprintf("%02X ", m_pCurPollMsg->Message[i]);
            str += tmp;
        }
        qWarning() << str << m_pCurPollMsg->m_nIndex;
    }
    m_pEventReqMsg->ScanReset();
//	m_dwLastMsec   = m_nCurrmSec;
    int ret;
    ret = m_ptcpSocket->write(m_pCurPollMsg->Message, m_pCurPollMsg->m_nIndex);

    if(m_pEventReqMsg )
        m_pEventReqMsg->ScanTimer = m_pEventReqMsg->ScanReload;
    if( ret != -1)
    {
        m_nState = PROT_WAIT_RES;

        return true;
    }
    if( m_pCurPollMsg->m_bDeleteAble )
    {
        delete m_pCurPollMsg;
        m_pCurPollMsg = NULL;
    }
    return false;
}

bool CWMINet::ProtPoll( void )
{
    bool	fRet=false;
    while (!m_listWrite.isEmpty())
    {
            m_pCurPollMsg = m_listWrite.takeFirst();
            m_pCurPollMsg->Due = false;
            SendQuery();
        //	if(m_pCommMon == NULL)
                return true;
    }

    if(m_bInitEnd == false)
    {
        return false;
    }
    if( m_pEventReqMsg )
    {
//		m_pEventReqMsg->TimeCheck( ( int )( m_nCurrmSec-m_dwLastMsec ) );
        m_pEventReqMsg->TimeCheck( 100 );
        if( m_pEventReqMsg->Due )
        {
            m_pEventReqMsg->Due = false;
            m_pCurPollMsg = m_pEventReqMsg;
            SendQuery();
            fRet = true;
        }
    }
    return fRet;
}

void CWMINet::StartConv()
{
    connectToHost();
//	m_Timer.start(100);
//	m_dwLastMsec = m_dwLastRecvMsec = m_dwLastTimeSendMsec = m_dwLastTimeSendMsec = GetTickCount();

}

void CWMINet::ConnectProc( void )
{
    m_cSeq = 0;
    char str[120];
    CPollMsg *pPoll = new CPollMsg( this, 0, WMI_CMD_CONNECT, WMI_SCMD_INIT, 0, true, false );
    *( quint32* )&str[ 0 ] = WMI_CMD_RTDATA;	// Command group : Realtime Data
    *( quint32* )&str[ 4 ] = ( quint16 )m_strName.length();
    strcpy( (char*)&str[8], m_strName.toLocal8Bit().data());
    pPoll->Add( str, m_strName.length()+8 );
    pPoll->End();
    AddWritePoll(pPoll);

    for (int i = 0; i < m_listInit.size(); ++i) {
        pPoll = ( CPollMsg* )m_listInit.at(i);
        AddWritePoll(pPoll );
    }
//	m_dwLastMsec = m_dwLastRecvMsec = m_dwLastTimeSendMsec = m_dwLastTimeSendMsec = GetTickCount();*/
}
int CWMINet::DoProtocol(char* lpszBlock, quint32 nLength )
{
    int	   len = qMin(nLength, (quint32)INBUFSIZE-m_nRspIndex-1 );

//	if(!m_pCurPollMsg)
//		return;

    memcpy(&m_RspBuffer[m_nRspIndex], lpszBlock, len);
    m_nRspIndex += len;

    if(m_nRspIndex < (int)sizeof(WMINET_HEADER))
        return -1;

    WMINET_HEADER *pRcvHeader = (WMINET_HEADER *)m_RspBuffer;
    if(m_nRspIndex < (int)(pRcvHeader->Len + sizeof(WMINET_HEADER)))
        return -1;
    switch( m_nState )
    {
        case PROT_IDLE :
        case PROT_WAIT_RES:
            if(m_pCommMon)
            {
//                if(m_nType == 0)
//                    ((CCommMonDialog*)m_pCommMon)->ExtractData(m_RspBuffer);
//                else
                if(m_nType == 1)
                    ((DecimalDisplayTab*)m_pCommMon)->ExtractData(m_RspBuffer);
                else
                if(m_nType == 2)
                    ((CalibrationTab*)m_pCommMon)->ExtractData(m_RspBuffer);
            }
            else
                ProcessResponse( m_pCurPollMsg );
            m_nRspIndex = 0;
            break;
        default:
            break;
    }

    m_nState = PROT_IDLE;
//	m_nRetries = 0;

    if( m_pCurPollMsg)
    {
        if( m_pCurPollMsg->m_bDeleteAble )
            delete m_pCurPollMsg;
        m_pCurPollMsg = NULL;
    }
//	if( m_nCurErrCnt == 5 )
    {
//		Close();
    }
    return (pRcvHeader->Len + sizeof(WMINET_HEADER));
}

void CWMINet::ProcessResponse( CPollMsg *poll )
{
    if(poll->ExtractData( m_RspBuffer) == false)
    {
        return;
    }
}

void CWMINet::AddWritePoll(CPollMsg   *pPollMsg)
{
    m_listWrite.append( pPollMsg );
}
