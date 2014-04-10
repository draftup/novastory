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

	function login()
	{
		var login = $('#loginmail').val();
		var pass = $('#loginpass').val();
		NovastoryApi.login(login, pass, function (data)
		{
			if (data.logined != null && data.logined)
			{
				USERID = data.userid;
				STOKEN = data.token;
				$.cookie("userid", data.userid,
				{
					path : '/',
					expires: 7
				}
				);
				$.cookie("stoken", data.token,
				{
					path : '/',
					expires: 7
				}
				);
				window.location.href = window.location.href;
			}
			else
			{
				alert("login failed");
			}
		}
		);
	}
	
	function logout()
	{
		$.removeCookie("userid");
		$.removeCookie("stoken");
		
		// reload page after logout
		window.location.href = window.location.href;
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
		if($("#registration-panel #tocapcha").attr('class') != 'enabled')
		{
			alert($("#registration-panel #tocapcha").attr('class'));
			return;
		}
	
		var email_adress = $('#registration-panel #regmail').val();
		var password_original = $('#registration-panel #regpass').val();
		var password_verify = $('#registration-panel #confregpass').val();

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

	function doRestorePassword()
	{
		var email = $('#restore-panel #restoremail').val();

		NovastoryApi.restorepassword(email, function (data)
		{
			if (data.error == null || data.error)
			{
				alert("error on registration");
			}
			else
			{
				alert("sended");
				$("#login-panel").show();
				$("#restore-panel").hide();
			}
		}
		);
	}

	$('#login').click(function ()
	{
		if (!$('#login-panel').exists())
		{
			$('#login-space').load('/modal-login.html #login-panel', null, function ()
			{
				$('#loginmail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						login();
					}
				}
				);

				$('#loginpass').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						login();
					}
				}
				);

				$('#loginbutt').click(function ()
				{
					login();
				}
				);

				$('#login-panel #newacc').click(function ()
				{
					$("#login-panel").hide();
					$('#registration-panel').show();
				}
				);

				$('#login-panel #restorepass').click(function ()
				{
					$("#login-panel").hide();
					$('#restore-panel').show();
				}
				);
			}
			);
		}
		else
		{
			if (!$('#login-panel').is(":hidden") || !$('#registration-panel').is(":hidden") || !$('#restore-panel').is(":hidden"))
			{
				$("#login-panel").hide();
				$("#registration-panel").hide();
				$("#restore-panel").hide();
			}
			else
			{
				$("#login-panel").show();
			}
		}

		if (!$('#restore-panel').exists())
		{
			$('#restore-space').load('/modal-restore.html #restore-panel', null, function ()
			{
				$('#restore-panel').hide();
			
				$('#restore-panel #restorepassword').click(function ()
				{
					doRestorePassword();
				}
				);
				

				$('#restore-panel #backtologin').click(function(){
					$('#restore-panel').hide();
					$("#login-panel").show();
				});
				
				$('#restore-panel #restoremail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRestorePassword();
					}
				}
				);
			}
			);
		}

		if (!$('#registration-panel').exists())
		{
			$('#register-space').load('/modal-register.html #registration-panel', null, function ()
			{
				$('#registration-panel').hide();

				$('#registration-panel #myacc').click(function ()
				{
					$('#registration-panel').hide();
					$('#login-panel').show();
				}
				);

				function checkReady()
				{
					var email_adress = $('#registration-panel #regmail').val();
					var password_original = $('#registration-panel #regpass').val();
					var password_verify = $('#registration-panel #confregpass').val();
					
					if(email_adress.length == 0 || password_original.length == 0 || password_verify.length == 0)
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}
					
					if(password_original != password_verify)
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}
					
					if(!$('#registration-panel #checkterms').prop('checked'))
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}
					
					$("#registration-panel #tocapcha").attr('class', 'enabled');
				}
				
				$('#registration-panel #regmail').keyup(function (e)
				{
					checkReady();
				
					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #regpass').keyup(function (e)
				{
					checkReady();
				
					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #confregpass').keyup(function (e)
				{
					checkReady();
				
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
				
				$('#registration-panel #checkterms').click(function()
				{
					checkReady();
				});
			}
			);
		}
	}
	);

	$('#outico').click(function(){
		logout();
	})
}
);
