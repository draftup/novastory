// Author: DEgITx <degitx@gmail.com>

// Some globals
jQuery.fn.exists = function ()
{
	return jQuery(this).length > 0;
}

includeJS("js/jquery.sha1.js");
includeJS("js/jquery.cookie.js");
var USERID;
var STOKEN;

$(document).ready(function ()
{

	function doLogin()
	{
		var login = $('#loginmail').val();
		var pass = $('#loginpass').val();
		NovastoryApi.login(login, pass, function (data)
		{
			if (data.logined != null && data.logined)
			{
				USERID = data.userid;
				STOKEN = data.token;
				alert(data.userid);
				alert(data.token);
				$.cookie("userid", data.userid, {path: '/'});
				$.cookie("stoken", data.token, {path: '/'});
				window.location.href=window.location.href;
			}
			else
			{
				alert("login failed");
			}
		}
		);
	}

	function proceedRegistration(email_adress, password_original, password_verify, challenge, response)
	{
		NovastoryApi.register(email_adress, password_original, challenge, response, function (data)
		{

			if (data.error == null || data.error)
			{
				alert("error on registration");
			}
			else
			{
				$('#registration-panel #registration-body').text("Congratulations with registration");
				alert("okay");
			}
		}
		);

	}

	function doRecaptcha()
	{
		var email_adress = $('#registration-panel #regmail').val();
		var password_original = $('#registration-panel #regpass').val();
		var password_verify = $('#registration-panel #confregpass').val();

		if (email_adress == null || password_original == null || password_verify == null)
		{
			alert("input error");
			return;
		}

		if (password_original != password_verify)
		{
			alert("input error");
			return;
		}

		if (!$('#registration-panel #checkterms').prop('checked'))
		{
			alert("you must agree with terms");
			return;
		}

		var trueRegistrationBody = $('#registration-panel #registration-body').clone();

		// init captcha
		Recaptcha.create(
			"6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO",
			"registration-body",
		{
			theme : "red",
			callback : function ()
			{
				$('#registration-body').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						var challenge = Recaptcha.get_challenge();
						var response = Recaptcha.get_response();
						Recaptcha.destroy();
						$(this).unbind("keyup");
						$('#registration-panel #registration-body').replaceWith(trueRegistrationBody);
						proceedRegistration(email_adress, password_original, password_verify, challenge, response);
					}
				}
				);

				Recaptcha.focus_response_field();
			}
		}
		);
	}

	$('#login').click(function ()
	{
		if (!$('#login-panel').exists())
		{
			$('#login-space').load('/modal-login.html', null, function ()
			{
				$('#loginmail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doLogin();
					}
				}
				);

				$('#loginpass').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doLogin();
					}
				}
				);

				$('#loginbutt').click(function ()
				{
					doLogin();
				}
				);

				$('#login-panel #newacc').click(function ()
				{
					$("#login-panel").hide();
					$('#registration-panel').show();
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

			if (!$('#registration-panel').is(":hidden"))
			{
				$("#registration-panel").hide();
			}
		}

		if (!$('#registration-panel').exists())
		{
			$('#register-space').load('/modal-register.html', null, function ()
			{
				$('#registration-panel').hide();

				$('#registration-panel #myacc').click(function ()
				{
					$('#registration-panel').hide();
					$('#login-panel').show();
				}
				);

				$('#registration-panel #regmail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #regpass').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #confregpass').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #tocapcha').click(function ()
				{
					doRecaptcha();
				}
				);
			}
			);
		}
	}
	);

}
);
