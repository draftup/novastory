-- MySQL dump 10.14  Distrib 5.5.32-MariaDB, for Win64 (x86)
--
-- Host: localhost    Database: novastory
-- ------------------------------------------------------
-- Server version	5.5.32-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `avatars`
--

DROP TABLE IF EXISTS `avatars`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `avatars` (
  `userid` int(10) unsigned NOT NULL,
  `avatar` mediumblob,
  `contenttype` varchar(64) DEFAULT NULL,
  `contentsize` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `avatars`
--

LOCK TABLES `avatars` WRITE;
/*!40000 ALTER TABLE `avatars` DISABLE KEYS */;
/*!40000 ALTER TABLE `avatars` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `texteditor`
--

DROP TABLE IF EXISTS `texteditor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `texteditor` (
  `userid` int(10) unsigned NOT NULL,
  `text` mediumtext,
  UNIQUE KEY `userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `texteditor`
--

LOCK TABLES `texteditor` WRITE;
/*!40000 ALTER TABLE `texteditor` DISABLE KEYS */;
/*!40000 ALTER TABLE `texteditor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `userid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `password` char(32) NOT NULL DEFAULT '',
  `salt` char(32) NOT NULL DEFAULT '',
  `email` char(100) NOT NULL DEFAULT '',
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `firstname` varchar(64) DEFAULT NULL,
  `lastname` varchar(64) DEFAULT NULL,
  `nickname` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`userid`),
  KEY `email` (`email`)
) ENGINE=InnoDB AUTO_INCREMENT=174 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'9e018658fec3fc6e7fd4a5e7317aec70','1f8b9be4b10381dc1f86e0d98f539490','degitx@gmail.com','2014-04-20 17:29:13','Alexey','Kasyanchuk','DEgITx'),(173,'995ddff75421a80c6b932292422ab336','281f38d79f9703a2b3e23772e71721b2','testmail@test.com','2014-04-20 16:47:47',NULL,NULL,NULL);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `userspassforgot`
--

DROP TABLE IF EXISTS `userspassforgot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `userspassforgot` (
  `token` char(32) NOT NULL DEFAULT '',
  `userid` int(10) unsigned NOT NULL,
  `newpassword` char(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`token`),
  KEY `userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `userspassforgot`
--

LOCK TABLES `userspassforgot` WRITE;
/*!40000 ALTER TABLE `userspassforgot` DISABLE KEYS */;
/*!40000 ALTER TABLE `userspassforgot` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `usersverify`
--

DROP TABLE IF EXISTS `usersverify`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `usersverify` (
  `token` char(32) NOT NULL DEFAULT '',
  `password` char(32) NOT NULL DEFAULT '',
  `salt` char(32) NOT NULL DEFAULT '',
  `email` char(100) NOT NULL DEFAULT '',
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`token`),
  KEY `email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `usersverify`
--

LOCK TABLES `usersverify` WRITE;
/*!40000 ALTER TABLE `usersverify` DISABLE KEYS */;
INSERT INTO `usersverify` VALUES ('1823c11c5d83bb1462a6629a09f710b9','facf1e5de373b98e8570b3e13b064729','3e0735b1da2e918559c9eb945c4aa680','dsdasd@gmail.com','2014-04-20 16:47:48'),('364e8efc1ebe7bf756cea1b1bfe1ae59','4aa85736960326891271cd424cdcd665','eb5f2d96a4fe7f4975c7918026491bd3','testp@dar.us','2014-04-05 08:41:34');
/*!40000 ALTER TABLE `usersverify` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-04-21  3:23:11
