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
  `data` mediumblob,
  `contenttype` varchar(64) DEFAULT NULL,
  `contentsize` int(10) unsigned DEFAULT NULL,
  `modifydate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
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
-- Table structure for table `subscriptions`
--

DROP TABLE IF EXISTS `subscriptions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `subscriptions` (
  `userid` int(10) unsigned DEFAULT NULL,
  `targetid` int(10) unsigned DEFAULT NULL,
  `options` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `userid_2` (`userid`,`targetid`),
  KEY `userid` (`userid`),
  KEY `targetid` (`targetid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `subscriptions`
--

LOCK TABLES `subscriptions` WRITE;
/*!40000 ALTER TABLE `subscriptions` DISABLE KEYS */;
/*!40000 ALTER TABLE `subscriptions` ENABLE KEYS */;
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
  `profileid` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`userid`),
  KEY `email` (`email`),
  KEY `profileid` (`profileid`)
) ENGINE=InnoDB AUTO_INCREMENT=365 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (2,'995ddff75421a80c6b932292422ab336','281f38d79f9703a2b3e23772e71721b2','someuser@gmail.com','2014-05-12 14:58:56','lol','notlol','tester','tester'),(226,'aa379344a1a68db67749f86e298495c9','9fb61927a7cbf0b7eb5fc85183ba2bfa','degitx@gmail.com','2014-05-05 20:02:22','Alexey','Kasyanchuk','degitx','degitx'),(364,'995ddff75421a80c6b932292422ab336','281f38d79f9703a2b3e23772e71721b2','testmail@test.com','2014-05-10 19:24:56',NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = cp866 */ ;
/*!50003 SET character_set_results = cp866 */ ;
/*!50003 SET collation_connection  = cp866_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 trigger delete_user after delete on users for each row begin delete from subscriptions where userid = OLD.userid OR targetid = OLD.userid; delete from avatars where userid = OLD.userid; delete from userspics where userid = OLD.userid; end */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

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
-- Table structure for table `userspics`
--

DROP TABLE IF EXISTS `userspics`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `userspics` (
  `userid` int(10) unsigned NOT NULL,
  `data` mediumblob,
  `contenttype` varchar(64) DEFAULT NULL,
  `contentsize` int(10) unsigned DEFAULT NULL,
  `modifydate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `userspics`
--

LOCK TABLES `userspics` WRITE;
/*!40000 ALTER TABLE `userspics` DISABLE KEYS */;
/*!40000 ALTER TABLE `userspics` ENABLE KEYS */;
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
INSERT INTO `usersverify` VALUES ('2b2522b312e01988ad48b990676b4418','8580268937bfb4bc26c7c925f55ac88c','552d31d4e523bd0d49e5c0c0093a0e6b','dsdasd@gmail.com','2014-05-09 23:51:28'),('364e8efc1ebe7bf756cea1b1bfe1ae59','4aa85736960326891271cd424cdcd665','eb5f2d96a4fe7f4975c7918026491bd3','testp@dar.us','2014-04-05 08:41:34');
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

-- Dump completed on 2014-05-15 20:01:19
