#ifndef	AVATAR_H
#define AVATAR_H

#include "logic/webdata.h"

class Test_Avatar;

namespace novastory
{

class Avatar : public WebData
{
	friend class ::Test_Avatar;
public:
	Avatar();

	const WebDataContainer& avatar() const;
	void setAvatar(const WebDataContainer& image);
};

}

#endif
