#include "avatar.h"

novastory::Avatar::Avatar()
{
	setObjectName("avatars");
}

const novastory::WebDataContainer& novastory::Avatar::avatar() const
{
	return data();
}

void novastory::Avatar::setAvatar(const novastory::WebDataContainer& image)
{
	setData(image);
}
