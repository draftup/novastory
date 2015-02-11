#include <QCoreApplication>
#include "downloader.h"
#include <QSslSocket>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QThread>
#include <QObject>
#include <iostream>
#include <windows.h>

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	HANDLE consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << "-------------------------------------" << std::endl;
	std::cout << "   Build tookit " GIT_DESCRIBE  " "   << std::endl;
	std::cout << "-------------------------------------" << std::endl;

	// Base checks
	if (!QSslSocket::supportsSsl())
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
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
	const QString source_directory = QDir(exe_dir + "/../").absolutePath();
	const QString build_directory = exe_dir + "/build";
	const QString installation_dir = source_directory + "/build";

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	qDebug() << "Source directory: " << source_directory;

	if (!QFileInfo::exists(source_directory + "/CMakeLists.txt"))
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("unknown source directory");
	}

	// First of all download archive util
	if (!QFileInfo::exists(ar7za_exe))
	{
		Downloader::download(ar7za_file, exe_dir);
	}

	if (!QFileInfo::exists(ar7za_exe))
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("No 7za founded");
	}

	Downloader md5checker(package_md5_file, exe_dir);
	md5checker.wait();
	QString md5package_real = md5checker.data();
	md5checker.close();
	QStringList md5fList = md5package_real.split(" ");
	if (md5fList.size() != 2)
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("Unknown md5 file format");
	}
	md5package_real = md5fList[0];

	if (md5package_real.size() <= 0)
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("Md5 not founded in downloaded file");
	}

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	qDebug() << "package.7z must be md5:" << md5package_real;

	QString md5package_test;
	// check md5 of old pakage
	if (QFileInfo::exists(package))
	{
		QFile checkPackage(package);
		if (!checkPackage.open(QIODevice::ReadOnly))
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			qFatal("Can not read pakage.7z");
		}

		QByteArray package_data = checkPackage.readAll();
		md5package_test = QString(QCryptographicHash::hash(package_data, QCryptographicHash::Md5).toHex());

		qDebug() << "Current package md5:" << md5package_test;
	}

	if (md5package_test != md5package_real)
	{
		if (QFileInfo::exists(package_dir) && !QDir(package_dir).removeRecursively())
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			qFatal("Remove old package directory failed");
		}

		// also removing build dir: who knows, may be changed compiller or cmake
		if (QFileInfo::exists(build_directory) && !QDir(build_directory).removeRecursively())
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			qFatal("Remove old build directory failed");
		}

		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		qDebug() << "Update old package";
		Downloader package_downloader(package_file, exe_dir);
		package_downloader.wait();
		md5package_test = QString(QCryptographicHash::hash(package_downloader.data(), QCryptographicHash::Md5).toHex());
		package_downloader.close();
		if (md5package_test != md5package_real)
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			qFatal("New pakage is deprecated, update md5");
		}
	}

	if (!QFileInfo::exists(package_dir + "/bin/qmake.exe"))
	{
		QProcess un7zip;
		un7zip.setWorkingDirectory(exe_dir);
		un7zip.start("7za.exe", QStringList() << "x" << package);
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		qDebug() << "Unpacking pakage file";

		QObject::connect(&un7zip, &QProcess::readyReadStandardOutput, [&un7zip, consolehwnd]()
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
			std::cout << un7zip.readAllStandardOutput().constData();
		});
		QObject::connect(&un7zip, &QProcess::readyReadStandardError, [&un7zip, consolehwnd]()
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			std::cout << un7zip.readAllStandardError().constData();
		});

		un7zip.waitForFinished(-1);
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		if (un7zip.exitStatus() != QProcess::NormalExit)
		{
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
			qFatal("Something wrong on unpacking");
		}
	}
	else
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		qDebug() << "Package already unpacked";
	}

	if (!QFileInfo::exists(build_directory))
	{
		QDir().mkpath(build_directory);
	}

	QProcess cmake;
	QProcessEnvironment env;
	env.insert("PATH", package_dir + "/bin");
	env.insert("OPENSSL_ROOT_DIR", package_dir);
	cmake.setProcessEnvironment(env);
	cmake.setWorkingDirectory(build_directory);
	cmake.start(package_dir + "/bin/cmake.exe", QStringList()
				<< "-G"
				<< "MinGW Makefiles"
				<< "-DWITH_TESTS=ON"
				<< "-DREMOTE_TESTING=ON"
				<< "-DCMAKE_BUILD_TYPE=Release"
				<< "-DCMAKE_INSTALL_PREFIX=" + installation_dir + ""
				<< source_directory
			   );

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_INTENSITY);
	QObject::connect(&cmake, &QProcess::readyReadStandardOutput, [&cmake, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		std::cout << cmake.readAllStandardOutput().constData();
	});
	QObject::connect(&cmake, &QProcess::readyReadStandardError, [&cmake, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		std::cout << cmake.readAllStandardError().constData();
	});

	cmake.waitForFinished(-1);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	if (cmake.exitStatus() != QProcess::NormalExit)
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("Something wrong on cmake");
	}

	int threads_count = QThread::idealThreadCount();
	if (threads_count < 1)
	{
		threads_count = 1;
	}

	qDebug() << "Using" << threads_count << "threads for build";

	QProcess build;
	build.setProcessEnvironment(env);
	build.setWorkingDirectory(build_directory);
	build.start(package_dir + "/bin/mingw32-make", QStringList() << "-j" + QString::number(threads_count));

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_INTENSITY);
	QObject::connect(&build, &QProcess::readyReadStandardOutput, [&build, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		std::cout << build.readAllStandardOutput().constData();
	});
	QObject::connect(&build, &QProcess::readyReadStandardError, [&build, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		std::cout << build.readAllStandardError().constData();
	});

	build.waitForFinished(-1);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	if (build.exitStatus() != QProcess::NormalExit)
	{
		qFatal("Something wrong on buiding");
	}

	QProcess testing;
	testing.setProcessEnvironment(env);
	testing.setWorkingDirectory(build_directory);
	testing.start(package_dir + "/bin/mingw32-make test");

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_INTENSITY);
	QObject::connect(&testing, &QProcess::readyReadStandardOutput, [&testing, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		std::cout << testing.readAllStandardOutput().constData();
	});
	QObject::connect(&testing, &QProcess::readyReadStandardError, [&testing, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		std::cout << testing.readAllStandardError().constData();
	});

	testing.waitForFinished(-1);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	if (testing.exitStatus() != QProcess::NormalExit)
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("Something wrong on testing");
	}

	QProcess installation;
	installation.setProcessEnvironment(env);
	installation.setWorkingDirectory(build_directory);
	installation.start(package_dir + "/bin/mingw32-make install");

	SetConsoleTextAttribute(consolehwnd, FOREGROUND_INTENSITY);
	QObject::connect(&installation, &QProcess::readyReadStandardOutput, [&installation, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN);
		std::cout << installation.readAllStandardOutput().constData();
	});
	QObject::connect(&installation, &QProcess::readyReadStandardError, [&installation, consolehwnd]()
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		std::cout << installation.readAllStandardError().constData();
	});

	installation.waitForFinished(-1);
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	qDebug() << "Installation status: " << installation.readAll();

	if (installation.exitStatus() != QProcess::NormalExit)
	{
		SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED);
		qFatal("Something wrong on installation");
	}

	// Copy dlls
	QFile::copy(package_dir + "/bin/libgcc_s_dw2-1.dll", installation_dir + "/libgcc_s_dw2-1.dll");
	QFile::copy(package_dir + "/bin/libwinpthread-1.dll", installation_dir + "/libwinpthread-1.dll");
	QFile::copy(package_dir + "/bin/libstdc++-6.dll", installation_dir + "/libstdc++-6.dll");

	QFile::copy(package_dir + "/bin/Qt5Core.dll", installation_dir + "/Qt5Core.dll");
	QFile::copy(package_dir + "/bin/Qt5Network.dll", installation_dir + "/Qt5Network.dll");
	QFile::copy(package_dir + "/bin/Qt5Sql.dll", installation_dir + "/Qt5Sql.dll");

	QFile::copy(package_dir + "/bin/libeay32.dll", installation_dir + "/libeay32.dll");
	QFile::copy(package_dir + "/bin/ssleay32.dll", installation_dir + "/ssleay32.dll");

	QFile::copy(package_dir + "/bin/icudt53.dll", installation_dir + "/icudt53.dll");
	QFile::copy(package_dir + "/bin/icuin53.dll", installation_dir + "/icuin53.dll");
	QFile::copy(package_dir + "/bin/icuuc53.dll", installation_dir + "/icuuc53.dll");

	QFile::copy(package_dir + "/bin/libmysql.dll", installation_dir + "/libmysql.dll");

	QDir().mkpath(installation_dir + "/sqldrivers");
	QFile::copy(package_dir + "/plugins/sqldrivers/qsqlmysql.dll", installation_dir + "/sqldrivers/qsqlmysql.dll");

	exit(0);

	return a.exec();
}
