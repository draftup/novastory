#include <QCoreApplication>
#include "downloader.h"
#include <QSslSocket>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	// Base checks
	if (!QSslSocket::supportsSsl())
	{
		qFatal("Ssl support failed");
	}

	// Directories
	const QString exe_dir = QCoreApplication::applicationDirPath();
	const QString package_dir = exe_dir + "/package";
	const QString package = exe_dir + "/package.7z";
	const QString ar7za_exe = exe_dir + "/7za.exe";
	const QString ar7za_file = "https://dl.dropboxusercontent.com/u/66826430/Work/buildenv/7za.exe";
	const QString package_md5_file = "https://dl.dropboxusercontent.com/u/66826430/Work/buildenv/package.md5";
	const QString package_file = "https://dl.dropboxusercontent.com/u/66826430/Work/buildenv/package.7z";

	// First of all download archive util
	if (!QFileInfo::exists(ar7za_exe))
		Downloader::download(ar7za_file, exe_dir);

	if (!QFileInfo::exists(ar7za_exe))
		qFatal("No 7za founded");

	Downloader md5checker(package_md5_file, exe_dir);
	md5checker.wait();
	QString md5package_real = md5checker.data();
	QStringList md5fList = md5package_real.split(" ");
	if (md5fList.size() != 2)
	{
		qFatal("Unknown md5 file format");
	}
	md5package_real = md5fList[0];

	if (md5package_real.size() <= 0)
		qFatal("Md5 not founded in downloaded file");

	qDebug() << "package.7z must be md5:" << md5package_real;

	QString md5package_test;
	// check md5 of old pakage
	if (QFileInfo::exists(package))
	{
		QFile checkPackage(package);
		if (!checkPackage.open(QIODevice::ReadOnly))
			qFatal("Can not read pakage.7z");

		QByteArray package_data = checkPackage.readAll();
		md5package_test = QString(QCryptographicHash::hash(package_data, QCryptographicHash::Md5).toHex());

		qDebug() << "Current package md5:" << md5package_test;
	}

	if (md5package_test != md5package_real)
	{
		qDebug() << "Update old package";
		Downloader package_downloader(package_file, exe_dir);
		package_downloader.wait();
		md5package_test = QString(QCryptographicHash::hash(package_downloader.data(), QCryptographicHash::Md5).toHex());
		if (md5package_test != md5package_real)
			qFatal("New pakage is deprecated, update md5");

		if (QFileInfo::exists(package_dir) && !QDir(package_dir).removeRecursively())
			qFatal("Remove old package directory failed");
	}

	QProcess un7zip;
	un7zip.start("7za.exe", QStringList() << "x" << "-o" + exe_dir << package);
	qDebug() << "Unpacking pakage file";
	un7zip.waitForFinished();
	qDebug() << "Unpacked Log:";
	qDebug() << un7zip.readAll();

	if (un7zip.exitStatus() != QProcess::NormalExit)
	{
		qFatal("Something wrong on unpacking");
	}

	return a.exec();
}
