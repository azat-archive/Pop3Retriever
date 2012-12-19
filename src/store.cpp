#include "store.h"

#include <stdlib.h>

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

using namespace std;

bool checkOrCreateFileStore(QString filename)
{
	QFileInfo file(filename);
	if (file.exists())
		return true;
	//else check if dir exists
	QDir dir = file.dir();
	if (!dir.exists())
	{//create dir
		if (!dir.mkpath(file.absolutePath()))
			return false; // could not create the directory
	}
	//directory should exists, so create the file now
	QFile mfile(filename);
	if (!mfile.open(QIODevice::WriteOnly))
		return false;
	mfile.close();
	return true;
}

int getMonth(QString month)
{
	month = month.toLower();
	if (month == "jan")
		return 1;
	else if (month == "feb")
		return 2;
	else if (month == "mar")
		return 3;
	else if (month == "apr")
		return 4;
	else if (month == "may")
		return 5;
	else if (month == "jun")
		return 6;
	else if (month == "jul")
		return 7;
	else if (month == "aug")
		return 8;
	else if (month == "sep")
		return 9;
	else if (month == "oct")
		return 10;
	else if (month == "nov")
		return 11;
	else if (month == "dec")
		return 12;
	else return -1;
}
bool getTimeZoneOffset(QString& zone, int& hours, int& mins)
{
	bool ok;
	int tmp = zone.toInt(&ok);
	if (ok)
	{
		if (tmp < 0)
		{
			tmp = abs(tmp);
			hours = - (tmp / 100);
			mins = tmp % 100;
		}
		else
		{
			hours = tmp / 100;
			mins = tmp % 100;
		}
		return true;
	}
	return false;
}

QDateTime parseRFC2822DateTime(QString dateTime)
{
	QDateTime date;
	int posComma = dateTime.indexOf(',');
	if (posComma != -1)
		dateTime = dateTime.mid(posComma+1);
	QStringList dateParts = dateTime.split(' ',QString::SkipEmptyParts);
	
	bool ok;
	int day = dateParts[0].toInt(&ok);
	if (!ok) return date;
	int month = getMonth(dateParts[1]);
	ok = (month > 0) && (month <= 12);
	if (!ok) return date;
	if (dateParts[2].length() != 4) return date; //year should have 4 digits, otherwise it's probably RFC 822
	int year = dateParts[2].toInt(&ok);
	if (!ok) return date;
	QString time = dateParts[3];
	QStringList timeParts = time.split(':',QString::SkipEmptyParts);
	int hour = timeParts[0].toInt(&ok);
	if (!ok) return date;
	int min = timeParts[1].toInt(&ok);
	if (!ok) return date;
	int sec = 0;
	if (timeParts.count() > 2)
	{
		sec = timeParts[2].toInt(&ok);
		if (!ok) return date;
	}
	int offsetHours;
	int offsetMins;
	if (!getTimeZoneOffset(dateParts[4], offsetHours, offsetMins))
		return date;
	
	date.setTimeSpec(Qt::UTC);
	date.setDate(QDate(year,month,day));
	date.setTime(QTime(hour,min,sec));
	date = date.addSecs(-(offsetHours*60 + offsetMins)*60);
	return date;
}

bool Store::storeMessage(QString msg, QString uniqueId)
{
	QDateTime msgDate;
	QString msgEnveloppe;
	bool date = false;
	bool from = false;
	QStringList lines = msg.split("\r\n");
	int i=0;
	while ((!from || !date) && i < lines.count())
	{
		QString& line = lines[i];
		if (line.startsWith("From: "))
		{
			msgEnveloppe = line.section("From: ",1);
			from = true;
		}
		if (line.startsWith("Date: "))
		{
//rfc822_parsedt
//			std::cout << "date" << std::endl;
			msgDate = parseRFC2822DateTime(line.section("Date: ",1));
			if (msgDate.isValid()) // in case the date could not be parsed successfully
				date = true;
		}
		i++;
	}
	if (!from)
		msgEnveloppe = "Unknown_Sender";
	if (!date)
		msgDate = QDateTime::currentDateTime().toUTC();
	QString fullFilename = rootFolder +msgDate.toString(storePattern)  +".mbox";
	if (!checkOrCreateFileStore(fullFilename))
		return false;
	QFile mboxFile(fullFilename);
	if (!mboxFile.open(QIODevice::WriteOnly | QIODevice::Append))
		return false;
	QTextStream mboxStream(&mboxFile);
	
	mboxStream << "From " << msgEnveloppe << " " << msgDate.toString("ddd MMM dd hh:mm:ss yyyy") << endl;
	mboxStream << msg << endl;
	if (!msg.endsWith('\n'))
		mboxStream << endl; //in case of a partial line, add another newline
	
	mboxStream.flush();
	mboxFile.close();
	
	idList.insert(uniqueId,msgDate);
	saveIdList();
	return true;
}

bool Store::loadIdList()
{
	idList.clear();
	QFile file(rootFolder + "curidlist.xml");
	QDomDocument ddoc;
	ddoc.setContent(&file);
	QDomElement root = ddoc.documentElement();
	if (root.tagName() != "IdList")
			return false;
	QDomElement msg = root.firstChildElement("Message");
	while (!msg.isNull())
	{
		QDomElement el;
		el = msg.firstChildElement("UID");
		QString key = el.text();
		el = msg.firstChildElement("Date");
		QDateTime date = QDateTime::fromString(el.text(),"yyyy/MM/dd hh:mm:ss");
		idList.insert(key,date);
		msg = msg.nextSiblingElement("Message");
	}
	return true;
}

bool Store::saveIdList()
{
	QDomDocument ddoc;
	QDomElement root = ddoc.createElement("IdList");
	ddoc.appendChild(root);
	
	QHash < QString , QDateTime >::const_iterator i;
	for (i = idList.constBegin(); i != idList.constEnd(); ++i)
	{
		QString msgId = i.key();
		QDateTime msgDate = i.value();
		QDomElement msgEL = ddoc.createElement("Message");
    
		QDomElement tag = ddoc.createElement("UID");
    msgEL.appendChild(tag);
    QDomText t = ddoc.createTextNode(msgId);
    tag.appendChild(t);
		tag = ddoc.createElement("Date");
    msgEL.appendChild(tag);
    t = ddoc.createTextNode(msgDate.toString("yyyy/MM/dd hh:mm:ss"));
    tag.appendChild(t);
		
		root.appendChild(msgEL);
	}
	QString xml = ddoc.toString();
	QFile file(rootFolder + "curidlist.xml");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	out << xml;
	out.flush();
	file.close();
	return true;
}
