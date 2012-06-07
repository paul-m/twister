<?php

/**
 * @file Some DB abstraction so we can have a singleton connection.
 */

/**
 * Static DB class so we can have a singleton connection to the db.
 * This class only manages the connection.
 * Consumers should generally use DB::connection() to get the mysqli object
 * and let the class deal with the mechanics.
 */
class DB {
  public static $mysqli = NULL; // mysqli object

  /**
   * Set up the connection by creating a mysqli object.
   */
  // should actually be private
  public static function start_connect($db = NULL) {
    if (!$db) $db = get_db(); // get_db() should be a static property of this class.
    self::$mysqli = new mysqli($db['host'], $db['user'], $db['password'], $db['database'], $db['port']);
    if (self::$mysqli->connect_errno) {
      self::$mysqli = NULL;
      // this should be an exception.
//      throw new Exception('Failed to connect to MySQL: ' . self::$mysqli->connect_error);
      echo "Failed to connect to MySQL: " . self::$mysqli->connect_error;
    }
  }
  
  public static function end_connect() {
    if (self::$mysqli) {
      self::$mysqli->close();
    }
    self::$mysqli = NULL;
  }

  public static function real_escape_string($suspect) {
    return self::connection()->real_escape_string($suspect);
  }

  /**
   * The designated function to use in this class.
   */
  public static function connection() {
    if (!self::$mysqli) self::start_connect();
    return self::$mysqli;
  }
}

function db_shutdown() {
  DB::end_connect();
}

register_shutdown_function('db_shutdown');

/**
 * get_db function.
 * 
 * @access public
 * @return Array with database credentials.
 */
function get_db() {
  return array(
    'database'=> 'paulm77',
    'user' => 'paulm77',
    'password' => 'hYN5ZYlp',
    'host' => "ovid.u.washington.edu",
    'port' => "5611",
    'table_prefix' => '',
  );
}

/*function get_db() {
  return array(
    'database'=> 'paulm77',
    'user' => 'root',
    'password' => 'root',
    'host' => 'localhost',
    'port' => '8889',
  );
}*/

/*
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL auto_increment,
  `type` varchar(255) NOT NULL default 'user',
  `first_name` varchar(256) default NULL,
  `last_name` varchar(256) default NULL,
  `user_name` varchar(256) default NULL,
  `password` varchar(40) default NULL,
  `hint` varchar(40) default NULL,
  `created` datetime default NULL,
  `modified` datetime default NULL,
  `email_address` varchar(256) default NULL,
  PRIMARY KEY  (`id`),
  KEY `user_name` (`user_name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
*/
