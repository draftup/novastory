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

#endif // RAWFILEHANDLER_H
