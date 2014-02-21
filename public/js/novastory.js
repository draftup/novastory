// Author: DEgITx <degitx@gmail.com>

// Some globals
jQuery.fn.exists = function ()
{
	return jQuery(this).length > 0;
}

includeJS("js/jquery.sha1.js");
var STOKEN;

$(document).ready(function ()
{

	function doLogin()
	{
		var login = $('#login-email-field').val();
		var pass = $('#login-password-field').val();
		$.post(
			'/api/login',
		{
			email : login,
			password : $.sha1(pass)
		},
			function (data)
		{
			if (data.logined != null && data.logined)
			{
				alert("login successful");
				STOKEN = data.token;
			}
			else
			{
				alert("login failed");
			}
		},
			'json');
	}

	function doRegistration()
	{
		var email_adress = $('#registration-email-field').val();
		var password_original = $('#registration-password-field').val();
		var password_verify = $('#registration-password-verify-field').val();
		
		if(email_adress == null || password_original == null || password_verify == null)
		{
			alert("input error");
			return;
		}
		
		if(password_original != password_verify)
		{
			alert("input error");
			return;
		}

			$.post(
				'/api/register',
			{
				email : email_adress,
				password : $.sha1(password_original),
				challenge : Recaptcha.get_challenge(),
				response : Recaptcha.get_response()
			},
				function (data)
			{

				if (data.error == null || data.error)
				{
					alert("error");
				}
				else
				{
					Recaptcha.destroy();
					alert("okay");
				}
			},
				'json');

	}

	$('#login-controls').click(function ()
	{
		if (!$('#login-panel').exists())
		{
			$('#page-content').after("\
																															<div id=\"login-panel\">\
																																<div class=\"content\">\
																																	<input type=\"text\" id=\"login-email-field\" placeholder=\"Логин\" />\
																																	<input type=\"password\" id=\"login-password-field\" placeholder=\"Пароль\" />\
																																	<div class=\"registration\">Регистрация</div>\
																																	<div class=\"password-forgotten\">Я забыл(а) пароль</div>\
																																</div>\
																																<div class=\"hidder\"></div>\
																															</div>\
																															");

			$('#login-email-field').keyup(function (e)
			{
				if (e.keyCode == 13)
				{
					doLogin();
				}
			}
			);

			$('#login-password-field').keyup(function (e)
			{
				if (e.keyCode == 13)
				{
					doLogin();
				}
			}
			);

			$('#login-panel .registration').click(function ()
			{
				$("#login-panel").hide();

				$('#page-content').after("\
																																				<div id=\"registration-panel\">\
																																					<div class=\"content\">\
																																						<input type=\"text\" id=\"registration-email-field\" placeholder=\"Почтовый адресс\" />\
																																						<input type=\"password\" id=\"registration-password-field\" placeholder=\"Пароль\" />\
																																						<input type=\"password\" id=\"registration-password-verify-field\" placeholder=\"Подтвердите пароль\" />\
																																						<div id=\"captcha-place\"></div>\
																																						<input type=\"checkbox\" name=\"registration-agreement\" value=\"agree\">я согласен(на) с условиями лицензионного соглашения\
																																					</div>\
																																					<div class=\"hidder\"></div>\
																																				</div>\
																																				");
				// init captcha
				Recaptcha.create(
					"6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO",
					"captcha-place",
				{
					theme : "red",
					callback : Recaptcha.focus_response_field
				}
				);

				$('#registration-email-field').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRegistration();
					}
				}
				);

				$('#registration-password-field').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRegistration();
					}
				}
				);

				$('#registration-password-verify-field').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRegistration();
					}
				}
				);
			}
			);
		}
		else
		{
			if ($('#login-panel').is(":hidden"))
			{
				$("#login-panel").show();
			}
			else
			{
				$("#login-panel").hide();
			}
		}
	}
	);

}
);
