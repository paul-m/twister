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
    parent::__construct('Twists');
  }

  function write_twist($record = array()) {
    // check for insert condition... id doesn't exist or is -1
    $primary_key = $this->primary_key();
    $id = ArrayCheck::arr_get($record, 'id', -1);
    $id = ArrayCheck::arr_get($record, $primary_key, $id);
    if ($id < 1) {
      $record['created'] = time();
      unset($record[$primary_key]);
      unset($record['id']);
    }
    ArrayCheck::set_if_not_set($record, 'USERID', 0);
    ArrayCheck::set_if_not_set($record, 'reply_to_TWISTID', 0);
    $time = time();
    if (!isset($record['created']) ||
      $record['created'] == '')
        $record['created'] = $time;
    $record['modified'] = $time;
    $this->convert_unix_to_datetime_for_key($record, 'modified');
    $this->convert_unix_to_datetime_for_key($record, 'created');
    //echo '<pre>'; var_dump($record); echo '</pre>';
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

  // return HTML as a form for entering the data.
  // caller must supply their own <form> tag and submit button.
  function html_form($id = -1) {
    $primary_key = $this->primary_key();
    if ($id > 0) {
      $record = $this->load_record($id);
    } else {
      $record = array($primary_key => -1);
    }
    $current_user = Session::current_user();
    if ($current_user > 0) $record['USERID'] = $current_user;
    $html = '';
    $schema = $this->schema;
    foreach($schema as $db_col=>$info) {
      $display_info = ArrayCheck::arr_get($info, 'form', NULL);
      // we don't want to show #uniqueid.
      // there's probably a better way to structure this.
      if ($display_info && (!($db_col == '#UNIQUEID'))) {
        if ($display_info['type'] != 'hidden') {
          $html .= '<label for="' . $db_col . '">' .
          ArrayCheck::arr_get($display_info, 'label', $db_col) . "</label>\n";
        }
        $form_name = $db_col;
        if ($db_col == $primary_key) $form_name = 'id';
        $namey_stuff = $this->table_name() . '_' . $db_col;

        $html .= '<input type="' . $display_info['type'] .
          '" name="' . $form_name . 
          '" value="' . ArrayCheck::arr_get($record, $db_col, '') . 
          '" class="class_' . $namey_stuff . 
          '" id="id_' . $namey_stuff .
          '" />' . "\n";
      }
    }
    return $html;
  }

  // return HTML as a nice display of the data.
  function html_display($id = -1) {
    if ($id > 0) {
      $sql = "select t.`TWISTID`, t.`twist`, t.`modified`, u.`user_name` from Twists as t left join Users as u on t.`USERID` = u.`USERID` where t.TWISTID = $id";
      $result = $this->crud_query($sql);
      $this->html_result_table($result, FALSE, TRUE);
      return;
    }
    return "<div><h3>Unable to load twist. id: $id.</h3></div>";
  }

  // return HTML as a nice display of the data.
  function show_twist_html($id = -1, $allow_edit = FALSE, $allow_delete = FALSE) {
    if ($id > 0) {
      $sql = "select t.`USERID`, t.`twist`, t.`modified`, u.`user_name` from Twists as t left join Users as u on t.`USERID` = u.`USERID` where t.TWISTID = $id LIMIT 1";
      $twist_result = $this->crud_query($sql);
      if ($twist_result) {
        $twist = $twist_result->fetch_assoc();
        
        echo '<div class="twist_frame"><div class="twist">' . $twist['twist'] . '</div>';
        echo '<div class="twist_info">Posted on: ' . $twist['modified'] . ' by ';
        echo '<a href="user.php?action=view&id=' . $twist['USERID'] . '">';
        echo $twist['user_name'] . '</a>';
        if ($allow_edit) echo ' <a href="index.php?action=edit&id=' . $id . '">edit</a>';
        if ($allow_delete) echo ' <a href="index.php?action=delete&id=' . $id . '">delete</a>';
  
        echo '</div></div>';
  /*      
        $twist_result->data_seek(0);
  
        $ratingdb = new CRUDDisplay('Ratings');
        $tid = $twist['TWISTID'];
        echo "tid: $tid ";
        $sql = "SELECT * FROM Ratings WHERE TWISTID = $tid";
        echo $sql;
        $rating_result = $ratingdb->crud_query($sql);
        $ratingdb->html_result_table($rating_result, TRUE, TRUE);
  
        $this->html_result_table($twist_result, FALSE, TRUE);*/
        return;
      }
    }
    return "<div><h3>Unable to load twist. id: $id.</h3></div>";
  }
  
  function show_twists_html($ids = array(), $allow_edit = FALSE, $allow_delete = FALSE) {
    if (count($ids) < 1) {
      $ids = array();
      $sql = "SELECT TWISTID from Twists ORDER BY modified DESC LIMIT 20";
      $result = $this->crud_query($sql);
      while($record = $result->fetch_assoc()) $ids[] = $record['TWISTID'];
    }
    if (count($ids) < 1) {
      echo '<div class="twist_outer_frame"><div class="twist">There are no twists to see.</div></div>';
      return;
    }
    else {
      echo '<div class="twist_outer_frame">';
      foreach ($ids as $id) {
        $this->show_twist_html($id, TRUE, TRUE);
      }
      echo '</div>';
    }
  }



  // return HTML as a form for entering the data.
  // caller must supply their own <form> tag and submit button.
/*  function html_form($id = -1) {
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
    $html .= '<input type="hidden" name="USERID" value="' .
      Session::current_user() . '" /><br>'."\n";
    return $html;
  }*/

}
