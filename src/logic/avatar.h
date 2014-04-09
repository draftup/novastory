#ifndef	AVATAR_H
#define AVATAR_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"

class Test_Avatar;

namespace novastory
{

class Avatar : protected Sqlizable, public JsonThrower
{
	friend class ::Test_Avatar;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserid)
	Q_PROPERTY(QByteArray avatar READ avatar WRITE setAvatar)
public:
	Avatar();

	int userid() const;
	void setUserid(int userid);

	const QByteArray& avatar() const;
	void setAvatar(const QByteArray& avatar);

	const QString& email() const;
	void setEmail(const QString& email);

	bool sync();
	bool update();
	bool remove();
private:
	int m_userid;
	QByteArray m_avatar;
	QString m_email;
};

}

#endif
