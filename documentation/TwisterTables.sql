CREATE TABLE IF NOT EXISTS `Twists` (
  `TWISTID` int(11) UNSIGNED NOT NULL auto_increment,
  `USERID` int(11) UNSIGNED NOT NULL,
  `reply_to_TWISTID` int(11) UNSIGNED default NULL,
  `created` datetime default NULL,
  `modified` datetime default NULL,
  `twist` varchar(140) default NULL,
  `points_total` int(11) default 0,
  PRIMARY KEY (`TWISTID`)
);

CREATE TABLE IF NOT EXISTS `Ratings` (
  `RATINGID` int(11) UNSIGNED NOT NULL auto_increment,
  `TWISTID` int(11) UNSIGNED NOT NULL,
  `voter_USERID` int(11) UNSIGNED NOT NULL,
  `points` int(11),
  `need_update` BOOLEAN default FALSE,
  PRIMARY KEY (`RATINGID`)
);

CREATE TABLE IF NOT EXISTS `Users` (
  `USERID` int(11) UNSIGNED NOT NULL auto_increment,
  `role` varchar(255) NOT NULL default 'user',
  `first_name` varchar(256) default NULL,
  `last_name` varchar(256) default NULL,
  `user_name` varchar(256) NOT NULL,
  `password` varchar(40) default NULL,
  `hint` varchar(40) default NULL,
  `created` datetime default NULL,
  `modified` datetime default NULL,
  `email_address` varchar(256) NOT NULL,
  PRIMARY KEY  (`USERID`),
  KEY `user_name` (`user_name`)
);
