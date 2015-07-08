#ifndef CONFIG_H
#define CONFIG_H

#include "webserver/webserver_config.h"

#define RECAPTCHA_PUBLIC_KEY "6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO"
#define RECAPTCHA_PRIVATE_KEY "6LfjsO4SAAAAAOlX0CmwHqWtaqUmftDha4PUmXQ2"

#ifndef REMOTE_TESTING
#define MYSQL_HOST "localhost"
#define MYSQL_DATABASE "novastory"
#define MYSQL_USER "novastory"
#define MYSQL_PASSWORD "novapassworddb"
#else
#define MYSQL_HOST "novastory.org"
#define MYSQL_DATABASE "novastory_test"
#define MYSQL_USER "novastory_tester"
#define MYSQL_PASSWORD "novatester8x9"
#endif

#define GOOGLE_WEB_CLIENT_ID "783882039569-u4nrpa2g7trtl9fo48uip8vs2kufljam.apps.googleusercontent.com"
#define GOOGLE_WEB_CLIENT_SECRET "Y-xXfG-mc9iBfS0ng10DluHw"

#endif // RAWFILEHANDLER_H
