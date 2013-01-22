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
	bool useSsl;
	bool ignoreRFC1939;
} Pop3RetrieveAccount;

class Pop3RetrieveConfig
{
	private:
		QVector<Pop3RetrieveAccount> accounts;
	
	protected:
		Pop3RetrieveAccount parseAccountElement(QDomElement& account);
		void addAccountProperty(QDomDocument& ddoc, QDomElement& account, QString propertyName, QString propertyValue);
	protected:
		static QString getCanonicalValue(const QString& string)
		{
			return string.toLower().trimmed();
		}
		static QString getCanonicalValue(const QDomElement& el)
		{
			return getCanonicalValue(el.text());
		}
		static bool toBool(const QString& val)
		{
			return (val == "true" || val == "y" || val == "yes" || val == "1");
		}
	public:
		bool LoadFromFile(QString filename);
		bool SaveToFile(QString filename);
		Pop3RetrieveAccount& GetAccount(int i);
		int GetNbAccounts(){return accounts.count();};
};


#endif

