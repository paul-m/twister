<?php

/**
 * UserCRUD consumes user arrays and turns them into database records.
 * It can also load database user records and give you arrays.
 */

require_once 'arraycheck.inc';
require_once 'cruddisplay.inc';
require_once 'superglobals.inc';

class UserCRUD extends CRUDDisplay {

  /**
   * Constructor method.
   */
  function __construct() {
    // UserCRUD only ever deals with the 'users' table.
    parent::__construct('users');
  }

  /**
   * Some 'users' use-cases for write_record().
   */
  function write_user($record = array()) {
    $isinsert = (ArrayCheck::arr_get($record, 'id', -1) < 1);
    // unset the password to update a record without changing it.
    if ($isinsert) {
      $record['created'] = time();
      if (isset($record['password'])) {
        // hash the password on the way out... if it's set.
        $record['password'] = User::hash_password($record['password']);
      }
      unset($record['id']);
    } else {
      // SORRY, YOU CAN'T CHANGE YOUR PASSWORD. WOOPS.
      unset($record['password']);
    }
    // add modified time, created time if we're doing an insert.
    $record['modified'] = time();
    $this->convert_unix_to_datetime_for_key($record, 'modified');
    $this->convert_unix_to_datetime_for_key($record, 'created');
    // add user type
    ArrayCheck::set_if_not_set($record, 'type', 'user');
    return parent::write_record($record);
  }

  function load_user($id = -1) {
    if ($id < 0) return array('id' => $id);
    $record = parent::load_record($id);
    if ($record) {
      $this->convert_datetime_to_unix_for_key($record, 'modified');
      $this->convert_datetime_to_unix_for_key($record, 'created');
    }
    return $record;
  }
  
  function load_user_username($username = '') {
    if ($username) {
      return parent::load_records_for_keyvalue(array('user_name' => $username));
    }
    return NULL;
  }
  
  function delete_user($id = -1) {
    if ($id > 0) {
      parent::delete_record($id, $this->table, $this->idkey);
    }
    // TODO: return a success/fail value.
  }

}
