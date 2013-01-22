
#ifndef __POP3CLIENT_H__
#define __POP3CLIENT_H__

#include <QSslSocket>
#include <QTcpSocket>
#include <QStringList>
#include <QVector>
#include <QPair>

#define CONNECT_TIMEOUT 15*1000
#define DISCONNECT_TIMEOUT 5*1000
#define READ_TIMEOUT 15*1000
#define WRITE_TIMEOUT 15*1000

class Pop3Client
{
	public:
		typedef QPair<QString, QString> UniqueId;
		typedef QPair<QString, int> MessageId;

	private:
		enum Pop3ConnectionState
		{
			NotConnected,
			Authorization,
			Transaction,
			Update
		};
		
		QTcpSocket *m_sock;
		Pop3ConnectionState state;
		bool readOnly;
		bool useSsl;
		bool ignoreRFC1939;
		
		
		QString doCommand(QString command,bool isMultiline);
		bool ReadResponse(bool isMultiline,QString& response);
		bool SendUser(QString& user);
		bool SendPasswd(QString& pwd);
		static UniqueId parseUniqueIdListing(QString& line);
		static MessageId parseMsgIdListing(QString& line);
		
	protected:
	
	public:
		Pop3Client(bool readOnly = true, bool useSsl = false, bool ignoreRFC1939 = true);
		void SetReadOnly(bool readOnly);
		bool Connect(QString host="localhost",short unsigned  port=110);
		bool Login(QString user, QString pwd);
		bool LoginWithDigest(QString user, QString digest);
		bool Quit();
		bool GetMailboxStatus(int& nbMessages, int& totalSize);
		bool ResetDeleted();
		bool NoOperation();
		bool GetUniqueIdList(QVector< UniqueId >& uIdList);
		bool GetUniqueIdList(QString msgId, UniqueId& uIdList);
		bool GetMsgList(QVector< MessageId >& uIdList);
		bool GetMsgList(QString msgId, MessageId& uIdList);
		bool Delete(QString msgId);
		bool GetMessageTop(QString msgId, int nbLines, QString& msgTop);
		bool GetMessage(QString msgId, QString& msg);
};

#endif
