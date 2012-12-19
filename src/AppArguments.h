#ifndef __APP_ARGUMENTS_H__
#define __APP_ARGUMENTS_H__

#include <QString>

class AppArguments
{
	public:
		AppArguments();
		bool init(QStringList argList);
		
		QString configFile;
		bool	askConfirm;
};


#endif

