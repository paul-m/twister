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
    parent::__construct('Users');
  }

  /**
   * Some 'users' use-cases for write_record().
   */
  function write_user($record = array()) {
    $primary_key = $this->primary_key();
    $isinsert = (ArrayCheck::arr_get($record, $primary_key, -1) < 1);
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
    ArrayCheck::set_if_not_set($record, 'role', 'user');
    return parent::write_record($record);
  }

  function load_user($id = -1) {
    if ($id < 0) return array('USERID' => $id);
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

  /**
   * $result is a mysqli result object
   */
  function list_userszzz($result = NULL, $allow_view = TRUE, $allow_edit = FALSE) {
    if ($result) {
      // set up the column display names
      $columns = $this->table_column_display_info();
  
      // start showing data
      $showme = $result->fetch_assoc();
      if ($showme) {
        // if the query doesn't give us the primary key then
        // we have no way of knowing which record we'd be editing.
        $has_primary_key = ArrayCheck::arr_get($showme, $this->primary_key(), FALSE);
        $table_name = $this->table_name();
        echo "<h2>$table_name</h2>\n";
        // start table
        echo '<table cellpadding="3" cellspacing="0" border="1">';
        // show DB column names as table headers
        echo "<tr>\n";
        foreach ($columns as $column=>$display) {
          echo "<th>$display</th>";
        }
        if (($allow_edit || $allow_view) && $has_primary_key) echo '<th></th>';
        echo "</tr>\n";
        // now start showing row data
        $rownum = 0;
        do {
          if (++$rownum & 1) echo '<tr class="odd">';
          else echo '<tr class="even">';
          foreach ($columns as $column=>$display) {
            $data = $showme[$column];
            if ($data == 'NULL') $data = 'N/A';
            echo "<td>$data</td>\n";
          }
          if ($has_primary_key && ($allow_edit || $allow_view)) {
            $operations = '<div>';
            if ($allow_view)
              $operations .= $this->view_link($showme[$this->primary_key()], 'view');
            if ($allow_edit)
              $operations .= ' ' . $this->edit_link($showme[$this->primary_key()], 'edit');
            echo "<td>$operations</td>";
          }
          echo '</tr>';
        } while ($showme = $result->fetch_assoc());
      }
      return;
    }
    echo '<div style="error">No results for this query.</div>';
  }

  /**
   * Returns the text of a link to edit a given user, by id.
   */
  function edit_link($id, $text) {
    return '<div><a href="' . Server::php_self('user.php') .
      '?action=edit&id=' . $id . '">' . $text . '</a></div>';
  }

  /**
   * Returns the text of a link to view a user, by id.
   */
  function view_link($id, $text) {
    return '<div><a href="' . Server::php_self('user.php') .
      '?id=' . $id . '">' . $text . '</a></div>';
  }
  
  function search($searchterm) {
    // find $searchterm in name and/or email
    return parent::search($searchterm, array('name', 'email'));
  }

}
