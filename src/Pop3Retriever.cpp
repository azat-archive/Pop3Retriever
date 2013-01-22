#include "Pop3Retriever.h"

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>


bool Pop3RetrieveConfig::LoadFromFile(QString filename)
{
	accounts.clear();
	QFile file(filename);
	QDomDocument ddoc;
	ddoc.setContent(&file);
	QDomElement root = ddoc.documentElement();
	if (root.tagName() != "Pop3Retriever")
			return false;
	QDomElement account = root.firstChildElement("Account");
	while (!account.isNull())
	{
		accounts.append(parseAccountElement(account));
		account = account.nextSiblingElement("Account");
	}
	return true;
}

Pop3RetrieveAccount Pop3RetrieveConfig::parseAccountElement(QDomElement& account)
{
	Pop3RetrieveAccount acc;
	//set the default value
	acc.storePattern = "yyyyMMdd.mbox";
	QDomElement el;
	el = account.firstChildElement("Host");
	acc.host = el.text();
	el = account.firstChildElement("Port");
	acc.port = el.text().toInt();
	el = account.firstChildElement("User");
	acc.user = el.text();
	el = account.firstChildElement("Password");
	acc.pwd = el.text();
	el = account.firstChildElement("TargetFolder");
	acc.targetFolder = el.text();
	el = account.firstChildElement("StorePattern");
	acc.storePattern = el.text();
	el = account.firstChildElement("DaysOnServer");
	acc.daysOnServer = el.text().toInt();

	el = account.firstChildElement("UseSsl");
	QString canonicalValue = getCanonicalValue(el);
	acc.useSsl = (canonicalValue == "true" || canonicalValue == "y" || canonicalValue == "yes" || canonicalValue == "1");

	// TODO: refactoring
	{
		el = account.firstChildElement("IgnoreRFC1939");
		QString canonicalValue = getCanonicalValue(el);
		acc.ignoreRFC1939 = (canonicalValue == "true" || canonicalValue == "y" || canonicalValue == "yes" || canonicalValue == "1");
	}

	return acc;
}

void Pop3RetrieveConfig::addAccountProperty(QDomDocument& ddoc, QDomElement& account, QString propertyName, QString propertyValue)
{
		QDomElement tag = ddoc.createElement(propertyName);
    account.appendChild(tag);
    QDomText t = ddoc.createTextNode(propertyValue);
    tag.appendChild(t);
}

bool Pop3RetrieveConfig::SaveToFile(QString filename)
{
	QDomDocument ddoc;
	QDomElement root = ddoc.createElement("Pop3Retriever");
	ddoc.appendChild(root);
	
	int i;
	for(i=0;i<accounts.count();i++)
	{
		Pop3RetrieveAccount& acc = accounts[i];
		QDomElement accountEl = ddoc.createElement("Account");
    
		addAccountProperty(ddoc, accountEl,"Host",acc.host);
		addAccountProperty(ddoc, accountEl,"Port",QString::number(acc.port));
		addAccountProperty(ddoc, accountEl,"User",acc.user);
		addAccountProperty(ddoc, accountEl,"Password",acc.pwd);
		addAccountProperty(ddoc, accountEl,"TargetFolder",acc.targetFolder);
		addAccountProperty(ddoc, accountEl,"StorePattern",acc.storePattern);
		addAccountProperty(ddoc, accountEl,"DaysOnServer",QString::number(acc.daysOnServer));
		
		root.appendChild(accountEl);
	}
	QString xml = ddoc.toString();
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	out << xml;
	out.flush();
	file.close();
	return true;
}
Pop3RetrieveAccount& Pop3RetrieveConfig::GetAccount(int i)
{
	return accounts[i];
}
