NovastoryApi = {};

NovastoryApi.login = function (email, rawpassword, callback)
{
	$.post(
		'/api/login',
	{
		email : email,
		password : $.sha1(rawpassword)
	},
		callback,
		'json');
};

NovastoryApi.loginSync = function (email, rawpassword)
{
	var data;
	$.ajax(
	{
		type : "POST",
		url : '/api/login',
		data :
		{
			email : email,
			password : $.sha1(rawpassword)
		},
		async : false,
		success : function (apidata)
		{
			data = apidata;
		}
	}
	);
	return data;
};

NovastoryApi.register = function (email, rawpassword, challenge, response, callback)
{
	$.post(
		'/api/register',
	{
		email : email,
		password : $.sha1(rawpassword),
		challenge : challenge,
		response : response
	},
		callback,
		'json');
};

NovastoryApi.restorepassword = function (email, callback)
{
	$.post(
		'/api/forgotpassword',
	{
		email : email
	},
		callback,
		'json');
};

NovastoryApi.updateAvatar = function (avatar, callback)
{
	$.post(
		'/api/updateavatar',
	{
		avatar : avatar
	},
		callback,
		'json');
};

NovastoryApi.updateUserPic = function (userpic, callback)
{
	$.post(
		'/api/updateuserpic',
	{
		userpic : userpic
	},
		callback,
		'json');
};

NovastoryApi.updateProfile = function (firstname, lastname, nickname, profileid, callback)
{
	$.post(
		'/api/updateprofile',
	{
		firstname : firstname,
		lastname : lastname,
		nickname : nickname,
		profileid : profileid
	},
		callback,
		'json');
};

NovastoryApi.myProfile = function (callback)
{
	$.post(
		'/api/myprofile',
	{},
		callback,
		'json');
};

NovastoryApi.subscribe = function (targetuser, callback)
{
	$.post(
		'/api/subscribe',
	{
		targetid : targetuser,
	},
		callback,
		'json');
};

NovastoryApi.unsubscribe = function (targetuser, callback)
{
	$.post(
		'/api/unsubscribe',
	{
		targetid : targetuser,
	},
		callback,
		'json');
};

NovastoryApi.isSubscribed = function (targetuser, callback)
{
	$.post(
		'/api/issubscribed',
	{
		targetid : targetuser,
	},
		callback,
		'json');
};

NovastoryApi.editorUpdate = function (text, callback)
{
	$.post(
		'/api/editorupdate',
	{
		text : text,
	},
		callback,
		'json');
};

NovastoryApi.editorUpdateSync = function (text)
{
	$.ajax(
	{
		type : "POST",
		url : '/api/editorupdate',
		data :
		{
			text : text,
		},
		async : false
	}
	);
};

NovastoryApi.editorText = function (callback)
{
	$.post(
		'/api/editortext',
	{},
		callback,
		'json');
};
