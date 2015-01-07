#ifndef	WEBDATA_H
#define WEBDATA_H

#include "webserver/sql/sqlizable.h"
#include "jsonthrower.h"
#include "webserver/webdatacontainer.h"
#include "user.h"

class Test_WebData;

namespace novastory
{

class WebData : protected Sqlizable, public JsonThrower
{
	friend class ::Test_WebData;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserid)
	Q_PROPERTY(WebDataContainer data READ data WRITE setData)
	Q_PROPERTY(QString contenttype READ contentType WRITE setContentType)
	Q_PROPERTY(unsigned int contentsize READ contentSize WRITE setContentSize)
public:
	WebData();

	int userid() const;
	void setUserid(int userid);

	const WebDataContainer& data() const;
	void setData(const WebDataContainer& data);

	const QString& email() const;
	void setEmail(const QString& email);

	const User& user() const;
	void setUser(const User& user);

	bool sync();
	bool update();
	bool remove();

	unsigned int contentSize() const {return m_data.size();};
	void setContentSize(unsigned int contsize) { };

	const QString& contentType() const { return m_data.mimeType(); };
	void setContentType(const QString& ct) { m_data.setMimeType(ct); };

	const QDateTime& modifyDate() const { return m_data.modificatedDate(); };
	void setModifyDate(const QDateTime& md) { m_data.setModificatedDate(md); };
private:
	User m_user;
	WebDataContainer m_data;
};

}

#endif
