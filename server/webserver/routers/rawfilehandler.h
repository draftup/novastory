#ifndef RAWFILEHANDLER_H
#define RAWFILEHANDLER_H

#include "../datahandler.h"
class QTcpSocket;

namespace novastory
{

class RawFileHandler : public DataHandler
{
public:
	bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QHash<QString, QString>& get = QHash<QString, QString>(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
	static bool checkUserLanguage(const int quserid, const QString& token);
#endif
};

}

#endif // RAWFILEHANDLER_H
