#ifndef __POP3RETRIEVECONFIG_H__
#define __POP3RETRIEVECONFIG_H__

#include <QString>
#include <QVector>
#include <QDomElement>

typedef struct Pop3RetrieveAccount_t
{
	QString host;
	int port;
	QString user;
	QString pwd;
	QString targetFolder;
	QString storePattern;
	int daysOnServer;
} Pop3RetrieveAccount;

class Pop3RetrieveConfig
{
	private:
		QVector<Pop3RetrieveAccount> accounts;
	
	protected:
		Pop3RetrieveAccount parseAccountElement(QDomElement& account);
		void addAccountProperty(QDomDocument& ddoc, QDomElement& account, QString propertyName, QString propertyValue);
	public:
		bool LoadFromFile(QString filename);
		bool SaveToFile(QString filename);
		Pop3RetrieveAccount& GetAccount(int i);
		int GetNbAccounts(){return accounts.count();};
};


#endif

