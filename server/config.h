#ifndef CONFIG_H
#define CONFIG_H

#define RECAPTCHA_PUBLIC_KEY "6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO"
#define RECAPTCHA_PRIVATE_KEY "6LfjsO4SAAAAAOlX0CmwHqWtaqUmftDha4PUmXQ2"

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define SMTP_USER "uruchie.org@gmail.com"
#define SMTP_PASSWORD "uruchiesrver"

#define MYSQL_HOST "localhost"
#define MYSQL_DATABASE "novastory"
#define MYSQL_USER "novastory"
#define MYSQL_PASSWORD "novapassworddb"

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

#endif // RAWFILEHANDLER_H
