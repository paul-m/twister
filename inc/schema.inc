<?php

/**
 * @file schema.inc defines some functions and data structures
 * for the data we're representing.
 * DB abstraction, validation, and display are all keyed off
 * this array structure.
 */


/*

  The data structure is like this:
  
  array(
  
    [tablename]
      [columnname]
        ['type'] // eg VARCHAR, INT, DATETIME, PASSWORD
        ['sanitize'] // similar to db_type. appended to sanitize_ for function name.
        ['form'] // for input form
          ['type'] // type of element: textarea, checkbox, etc.
                   // radiobuttons not allowed at the moment.
                   // 'password' gets special treatment: two form elements to compare,
                   // one named columnname and one named columname_compare.
                   // Will need special way to work out whether we're on a login
                   // form or a register form.
          ['label'] // human-readable display label for element
          ['required'] // if not present, FALSE. set TRUE to require this element.
        ['display']
          // might not actually need this. we'll see. similar to ['form']

    )
  
  */


function get_schema($table) {
  $schema = array(

/** USERS **/
    'Users' => array (
      '#UNIQUEID' => 'USERID',
      'USERID' => array(
        'type' => 'INT',
        'sanitize' => 'INT',
        'form' => array (
          'type' => 'hidden',
          'display' => 'hidden',
        ),
      ),
      'role' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'hidden',
          'display' => 'hidden',
        ),
      ),
      'first_name' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'text',
          'label' => 'First name',
        ),
      ),
      'last_name' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'text',
          'label' => 'Last name',
        ),
      ),
      'user_name' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'text',
          'label' => 'Username',
        ),
      ),
      'password' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'PASSWORD',
        'form' => array (
          'type' => 'password',
          'display' => 'password',
          'label' => 'Password',
        ),
      ),
      'hint' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'text',
          'label' => 'Hint',
        ),
      ),
      'created' => array(
        'type' => 'DATETIME',
        'sanitize' => 'DATETIME',
        'form' => array (
          'type'=>'text',
          'label'=>'Created',
        ),
      ),
      'modified' => array(
        'type' => 'DATETIME',
        'sanitize' => 'DATETIME',
        'form' => array (
          'type'=>'text',
          'label'=>'Modified',
        ),
      ),
      'email_address' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'EMAIL',
        'form' => array (
          'type' => 'text',
          'label' => 'Email',
        ),
      ),
    ),

/** TWISTS **/
    'Twists' => array (
      '#UNIQUEID' => 'TWISTID',
      'TWISTID' => array(
        'type' => 'INT',
        'sanitize' => 'INT',
        'form' => array (
          'type' => 'hidden',
          'display' => 'hidden',
        ),
      ),
      'USERID' => array(
        'type' => 'INT',
        'sanitize' => 'INT',
        'form' => array (
          'type' => 'hidden',
          'display' => 'hidden',
        ),
      ),
/*      'sequence' => array(
        'type' => 'INT',
        'sanitize' => 'INT',
        'form' => array (
          'type' => 'hidden',
          'display' => 'hidden',
        ),
      ),*/
      'twist' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'text',
          'label' => 'Twist',
        ),
      ),
 /*     'desc' => array(
        'type' => 'VARCHAR',
        'sanitize' => 'VARCHAR',
        'form' => array (
          'type' => 'hidden',
        ),
      ),*/
      'created' => array(
        'type' => 'DATETIME',
        'sanitize' => 'DATETIME',
        'form' => array (
          'type'=>'hidden',
          'label'=>'Created',
        ),
      ),
      'modified' => array(
        'type' => 'DATETIME',
        'sanitize' => 'DATETIME',
        'form' => array (
          'type'=>'hidden',
//          'display' => 'hidden',
          'label'=>'Modified',
        ),
      ),
    ),
  );
  if (isset($schema[$table])) return $schema[$table];
  return array();
}
