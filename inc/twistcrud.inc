<?php

/**
 * UserCRUD consumes user arrays and turns them into database records.
 * It can also load database user records and give you arrays.
 */

require_once 'arraycheck.inc';
require_once 'superglobals.inc';
require_once 'cruddisplay.inc';
require_once 'usercrud.inc';

class TwistCRUD extends CRUDDisplay {

  /**
   * Constructor method.
   */
  function __construct() {
    // TwistCRUD only ever deals with the 'questions' table.
    parent::__construct('questions');
  }

  function write_twist($record = array()) {
    // check for insert condition... id doesn't exist or is -1
    if (ArrayCheck::arr_get($record, 'id', -1) < 1) {
      $record['created'] = time();
      unset($record['id']);
    }
    ArrayCheck::set_if_not_set($record, 'survey_id', -1);
    $record['modified'] = time();
    $this->convert_unix_to_datetime_for_key($record, 'modified');
    $this->convert_unix_to_datetime_for_key($record, 'created');
    return parent::write_record($record);
  }

  function load_twist($id = -1) {
    if ($id < 0) return array('id' => $id);
    $record = parent::load_record($id);
    if ($record) {
      $this->convert_datetime_to_unix_for_key($record, 'modified');
      $this->convert_datetime_to_unix_for_key($record, 'created');
    }
    return $record;
  }
  
  function delete_twist($id = -1) {
    if ($id > 0) {
      parent::delete_record($id, $this->table, $this->idkey);
    }
  }

  // return HTML as a nice display of the data.
  function html_display($id = -1) {
    if ($id > 0) {
      $record = $this->load_twist($id);
      if ($record) {
        $userdb = new UserCRUD;
        $user = $userdb->load_user($record['survey_id']);
        
        $html = '<div id="twist">' . $record['question'] . '</div>';
        $html .= '<div>Written by: ' .
          ArrayCheck::arr_get($user,'user_name','&lt;unknown&gt;') .
          ' on ' . date(DATE_RFC850, $record['created']);
        
        $current_user = Session::current_user();
        $admin = Session::current_user_is_admin();
        
        if ($admin || ($current_user == $user['id'])) {
          $html .= ' <a href="' . Server::php_self() .
          '?type=edit&id=' . $id . '">edit</a>';
          $html .= ' <a href="' . Server::php_self() .
          '?type=delete&id=' . $id . '">delete</a>';
        }
        $html .= ' <a href="' . Server::php_self() .
        '?id=' . $id . '">permalink</a>';
        $html .= '</div>';
        return $html;
      }
    }
  return "<div><h3>Unable to load twist. id: $id.</h3></div>";
  }

  // return HTML as a form for entering the data.
  // caller must supply their own <form> tag and submit button.
  function html_form($id = -1) {
    if ($id > 0) {
      $record = $this->load_twist($id);
    } else {
      $record = array($this->idkey => $id);
    }
    $html = '<label for="question">Twist:</label>';
    $html .= '<input type="text" name="question" value="' .
      ArrayCheck::arr_get($record, 'question', '') . '" /><br>'."\n";
    $html .= '<input type="hidden" name="id" value="' .
      ArrayCheck::arr_get($record, 'id', -1) . '" /><br>'."\n";
    $html .= '<input type="hidden" name="survey_id" value="' .
      Session::current_user() . '" /><br>'."\n";
    return $html;
  }

}
