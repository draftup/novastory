#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define SMTP_USER "uruchie.org@gmail.com"
#define SMTP_PASSWORD "uruchiesrver"

#define UPLOAD_LIMIT_BYTES 1 * 1024 * 1024
#define CONTENT_LIMIT_BYTES 5 * 1024 * 1024

#define CACHE_SIZE 35 * 1024 * 1024

#ifdef QT_DEBUG
#define WORKERS_NUMBER 5
#else
#define WORKERS_NUMBER 10
#endif
#define WORKERS_MAX_TIME 180

#define REPORTS_MAIL "degitx@gmail.com"




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

#endif // WEBSERVER_CONFIG_H
