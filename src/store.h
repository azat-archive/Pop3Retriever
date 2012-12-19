#ifndef __STORE_H__
#define __STORE_H__


#include <QString>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QDate>
#include <QHash>

class Store
{
	private:
		QString rootFolder;
		QString storePattern;
		QHash < QString , QDateTime > idList;
	
	public:
		Store(QString root,QString storePattern){rootFolder = root; this->storePattern=storePattern; loadIdList();};
		bool storeMessage(QString msg, QString uniqueId);
		bool containsMessage(QString uniqueId) {return idList.contains(uniqueId);};
		QDateTime getMessageDate(QString uniqueId) {return idList[uniqueId];};
		void removeMessage(QString uniqueId){idList.remove(uniqueId);};
		
		virtual ~Store(){saveIdList();};
		
	protected:
		bool loadIdList();
		bool saveIdList();
};

#endif
