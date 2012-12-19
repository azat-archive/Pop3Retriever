#include "AppArguments.h"
#include <QtDebug>
#include <QStringList>

AppArguments::AppArguments()
{
	askConfirm = false;
}

bool AppArguments::init(QStringList argList)
{
	qDebug() << argList;
	argList.removeFirst();
	QStringListIterator strIt(argList);
	while (strIt.hasNext())
	{
		const QString& arg = strIt.next();
		if (arg.startsWith("-c")) {
			qDebug() << "adding option ask confirmation";
			askConfirm = true;
		} else {
			configFile = arg;
		}
	}
	qDebug() << "AppArguments::init Done";
	if (configFile.isNull()) return false;
	return true;
}
