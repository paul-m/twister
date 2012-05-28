<?php

/**
 * ArrayCheck is a static class to deal with array checking.
 * Subclass this for various use cases, including superglobals.
 */
class ArrayCheck {

  /**
   * get() checks if the array exists, and has a given key.
   * Returns key for array, or $default otherwise.
   * ex: ArrayCheck::get($_POST, 'id', array('id' => -1));
   */
  public static function get($array, $key, $default = NULL) {
    if ($array && isset($array[$key])) {
      return $array[$key];
    }
    return $default;
  }

  /**
   * Set array for key, if they exist.
   * We do this so we can deal with superglobals that might
   * not exist, like $_SESSION.
   */
  public static function arr_set(&$array, $key = '', $value = 1) {
    if (isset($array) && ($key != '')) {
      $array[$key] = $value;
      return $value;
    }
    return FALSE;
  }

  public static function set_if_not_set(&$array, $key = '', $value = 1) {
    if (!self::get($array, $key, NULL)) {
      $array[$key] = $value;
      return $value;
    }
    return FALSE;
  }
  
  public static function arr_unset(&$array, $key = '') {
    if ($key && $array) {
      if (isset($array[$key])) {
        unset($array[$key]);
      }
    }
  }
  
  public static function arr_for_keys($array, $keys = array()) {
    if ($array) {
      $result = array_intersect_key($array, $keys);
      return $result;
    }
    return array();
  }
}
