<?php

/**
 * @file
 * A bunch of static classes based on ArrayCheck.
 * They wrap superglobal use-cases around needing to check for
 * the existence of a key in an array.
 */

require_once 'arraycheck.inc';
require_once 'usercrud.inc';

/**
 * Session is a static class to deal with the current session.
 */
class Post extends ArrayCheck {
  /**
   * helper method to get a value from $_POST.
   * Pass the name of the key to gather
   * Pass default value you want returned if the variable doesn't exist.
   */
  public static function get($key, $default = NULL) {
    if (isset($_POST)) return parent::arr_get($_POST, $key, $default);
    return $default;
  }

  /**
   * Set post variables.
   */
  public static function set($key = '', $value = 1) {
    if (isset($_POST)) return parent::set($_POST, $key, $value);
    return FALSE;
  }
  
  public static function post_unset($key = '') {
    if (isset($_POST)) parent::arr_unset($_POST, $key);
  }
  
  public static function for_keys($keys = array()) {
    if (isset($_POST)) return parent::arr_for_keys($_POST, $keys);
  }
}

/**
 * Session is a static class to deal with the current session.
 */
class Get extends ArrayCheck {
  /**
   * helper method to get a value from $_GET.
   * Pass the name of the key to gather
   * Pass default value you want returned if the variable doesn't exist.
   * Called 'get_get' because 'get' would just be a constructor.
   */
  public static function get_get($key, $default = NULL) {
      if (isset($_GET)) return parent::arr_get($_GET, $key, $default);
    return $default;
  }

  /**
   * Set session variables.
   */
  public static function set($key = '', $value = 1) {
    if (isset($_GET)) return parent::set($_GET, $key, $value);
    return FALSE;
  }
  
  public static function post_unset($key = '') {
    if (isset($_GET)) parent::arr_unset($_GET, $key);
  }
  
  public static function for_keys($keys = array()) {
    if (isset($_GET)) return parent::arr_for_keys($_GET, $keys);
  }
}


/**
 * Session is a static class to deal with the current session.
 */
class Session extends ArrayCheck {

  protected static $started = FALSE;

  public static function start() {
    if (!self::$started) {
      self::$started = session_start();
    }
    return self::$started;
  }
  // get or set a session ID.
  public static function id($id = NULL) {
    if ($id) return session_id($id);
    return session_id();
  }
  public static function destroy() {
    if (isset($_SESSION)) $_SESSION = array();
    if (self::$started) session_destroy();
    setcookie('PHPSESSID', '', time()-3600, '/', '', 0);
    self::$started == FALSE;
    return FALSE;
  }

  /**
   * helper method to get a value from $_SESSION.
   * Pass the name of the key to gather
   * Pass default value you want returned if the variable doesn't exist.
   */
  public static function get($key, $default = NULL) {
    self::start();
    if (isset($_SESSION)) return parent::arr_get($_SESSION, $key, $default);
    return $default;
  }

  /**
   * Set session variables.
   */
  public static function set($key = '', $value = 1) {
    self::start();
    if (isset($_SESSION)) {
      return parent::set($_SESSION, $key, $value);
    }
    return FALSE;
  }

  /**
   * Unset a session variable
   */
  public static function sess_unset($key = '') {
    self::start();
    if (isset($_SESSION)) {
      parent::arr_unset($_SESSION, $key);
    }
  }


  /**
   * if a user is logged in, return their id.
   * $default otherwise.
   */
  public static function current_user($default = -1) {
    self::start();
    // we use the name of the db table, so 'users,'
    // rather than 'user.'
    $user = self::get('users', NULL);
    if ($user && isset($user['id'])) return $user['id'];
    return $default;
  }

  /** 
   * return true if current user is an admin
   */
  public static function current_user_is_admin() {
    $currid = self::current_user();
    if ($currid > 0) {
      $userdb = new UserCRUD;
      $current_user = $userdb->load_user($currid);
      return ArrayCheck::arr_get($current_user, 'type', 'notadmin') == 'admin';
    }
    return FALSE;
  }
  
  public static function current_user_can_edit($content_user = 0) {
    $currid = self::current_user();
    if ($currid > 0) {
      $userdb = new UserCRUD;
      $current_user = $userdb->load_user($currid);
      $can = ArrayCheck::arr_get($current_user, 'type', 'notadmin') == 'admin';
      if (!$can)
        $can = ArrayCheck::arr_get($current_user, 'id', NULL) === 
          Session::current_user(-1);
      return $can;
    }
    return FALSE;
  }
  
  public static function set_current_user($id = -1) {
    self::start();
    if ($id < 1) {
      self::sess_unset('users');
      return;
    }
    $foo= self::set('users', array('id'=>$id));
    return $foo;
  }
  
  public static function set_message($message = '') {
    self::start();
    if ($message) {
      $messages = self::get('messages', array());
      $messages[Server::php_self()][] = $message;
      self::set('messages', $messages);
    }
  }

  /**
   * Display messages sent from previous forms.
   */
  public static function get_page_messages() {
    self::start();
    $messages = self::get('messages', array());
    return self::get($messages, Server::php_self(), NULL);
  }
}

/**
 * Session is a static class to deal with the current session.
 */
class Server extends ArrayCheck {
  /**
   * helper method to get a value from $_COOKIE.
   * Pass the name of the key to gather
   * Pass default value you want returned if the variable doesn't exist.
   */
  public static function get($key, $default = NULL) {
    if (isset($_SERVER)) return parent::arr_get($_SERVER, $key, $default);
    return $default;
  }

  /**
   * Set session variables.
   */
  public static function set($key = '', $value = 1) {
    if (isset($_SERVER)) return parent::set($_SERVER, $key, $value);
    return FALSE;
  }

  /**
   * Request type
   */
  public static function request_method() {
    return self::get('REQUEST_METHOD', '');
  }
  
  public static function php_self($default = '') {
    return self::get('PHP_SELF', $default);
  }
}

/**
 * Session is a static class to deal with the current session.
 */
class Cookie extends ArrayCheck {
  /**
   * helper method to get a value from $_COOKIE.
   * Pass the name of the key to gather
   * Pass default value you want returned if the variable doesn't exist.
   */
  public static function get($key, $default = NULL) {
    if (isset($_COOKIE)) return parent::arr_get($_COOKIE, $key, $default);
    return $default;
  }

  /**
   * Set session variables.
   */
  public static function set($key = '', $value = 1) {
    if (isset($_COOKIE)) return parent::set($_COOKIE, $key, $value);
    return FALSE;
  }
  
  public static function sess_unset($key = '') {
    if (isset($_COOKIE)) parent::arr_unset($_COOKIE, $key);
  }

  /**
   * if a user is logged in, return their id.
   * -1 otherwise.
   */
  public static function current_user() {
    self::start();
    // we use the name of the db table, so 'users,'
    // rather than 'user.'
    $user = self::get('users', NULL);
    if ($user && exists($user['id'])) return $user['id'];
    return -1;
  }
  
  /**
   * Display messages sent from previous forms.
   */
  public static function show_message($function = '') {
    self::start();
    if ($function) return $function(self::get('messages', array()));
    return self::get('messages', array());
  }
}

class User {
  // Only the User class can get the hash salt.
  // Call me paranoid if you must.
  private static $hash_salt = 'mm.salty.';

  /**
   * Need to hash a password? Here's your function.
   */
  public static function hash_password($password) {
    return md5(self::$hash_salt . $password);
  }

}
