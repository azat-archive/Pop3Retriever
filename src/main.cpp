#include "Pop3Client.h"
#include "Pop3Retriever.h"
#include "store.h"
#include "AppArguments.h"

#include <iostream>
#include <QCoreApplication>
#include <QtGlobal>
#include <QtDebug>
using namespace std;

#define _ERROR(s) {cerr << (s) << endl; return false;}
bool HandleAccount(Pop3RetrieveAccount& account)
{
	Pop3Client client(false, account.useSsl, account.ignoreRFC1939);
	Store msgStore(account.targetFolder,account.storePattern);
	
	if (!client.Connect(account.host,account.port))
		_ERROR("could not connect")
	if (!client.Login(account.user,account.pwd))
		_ERROR("could not login")
	
	QVector< Pop3Client::UniqueId > uIds; //list with <msgId, uniqueId>
	if (!client.GetUniqueIdList(uIds))
		_ERROR("could not get uniqueId Listing")
	
	for (int i=0;i<uIds.count();i++)
	{
		if (msgStore.containsMessage(uIds[i].second))
		{//msg already in local store, so check date and try to delete on server
			QDateTime msgDate = msgStore.getMessageDate(uIds[i].second);
			int msgAge = msgDate.daysTo(QDateTime::currentDateTime());
			if (account.daysOnServer >=0 && msgAge >= account.daysOnServer)
			{//delete
				if (!client.Delete(uIds[i].first))
					_ERROR("could delete message on server")
				cout << "DELE: message ("<<uIds[i].first.toStdString() << ","<<uIds[i].second.toStdString() << ") removed from server" << endl;
				msgStore.removeMessage(uIds[i].second);
			}
			else
			{//skip
				cout << "SKIP: message (" << uIds[i].first.toStdString() << ","<<uIds[i].second.toStdString();
				cout << "), " << msgDate.toString("yyyy/MM/dd hh:mm:ss").toStdString() << endl;
			}
		}
		else
		{//retrieve and save message in local store
			QString msg;
//			qDebug() << "beforegetting " << uIds[i].first << "\n";
			if (!client.GetMessage(uIds[i].first,msg))
				_ERROR("could not retrieve msg " + uIds[i].first.toStdString())
//			qDebug() << "aftergetting " << uIds[i].first << "\n";
			msgStore.storeMessage(msg,uIds[i].second);
//			qDebug() << "stored " << uIds[i].first << "\n";
			QDateTime msgDate = msgStore.getMessageDate(uIds[i].second);
				cout << "RETR: message (" << uIds[i].first.toStdString() << ","<<uIds[i].second.toStdString();
				cout << "), " << msgDate.toString("yyyy/MM/dd hh:mm:ss").toStdString() << endl;
		}
	}
	
	if (!client.Quit())
		_ERROR("could not quit")
	
	return true;
}
#undef _ERROR

int main(int argc, char *argv[])
{
	QCoreApplication app(argc,argv);
	AppArguments appArgs;
	if (!appArgs.init(QCoreApplication::arguments()))
	{
		qFatal("Could not initialize ApplicationArguments, missing config file");
		QCoreApplication::exit(-1);
	}

	if (appArgs.askConfirm) {
		QString answer;
		cout << "start mailbox fetch / cleanup? y/n ";
		answer = QTextStream(stdin).readLine();
		if (answer.toLower() == "n") {
			cout << "cancelled by user" << endl;
			return -2;
		}
	}
	
	Pop3RetrieveConfig p3rc;
	
	if (!p3rc.LoadFromFile(appArgs.configFile))
	{
		qFatal(QString("ERROR: could not read configurationfile: %1").arg(appArgs.configFile).toLocal8Bit());
	}
	
	for (int i=0;i<p3rc.GetNbAccounts();i++)
		HandleAccount(p3rc.GetAccount(i));
	
		
	return 0;
}

