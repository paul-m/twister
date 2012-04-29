<?php

/**
 * OOP on CRUD
 */

require_once 'db.inc';
require_once 'schema.inc';
require_once 'arraycheck.inc';

class CRUDOOP {
  protected $schema; // display/validation data.
  protected $table; // name of table.
  protected $idkey; // unique id for this table.

  function __construct($table = '') {
    $this->set_schema($table);
  }

  function set_schema($table = '') {
    $this->table = NULL;
    $this->schema = NULL;
    $this->idkey = '';
    $this->schema = get_schema($table);
    if ($this->schema) $this->table = $table;
    if (isset($this->schema['#UNIQUEID'])) {
      $this->idkey = $this->schema['#UNIQUEID'];
    } else {
      $this->idkey = '';
    }
  }

  function load_record($id) {
    if ($this->schema) {
      $db_connection = DB::connection();
      if ($db_connection) {
        $query = $db_connection->query("SELECT * FROM " . $this->table .
          " WHERE " . $this->idkey . " = " . $id);
        return $query->fetch_assoc();
      }
    }
    return array();
  }

  function load_records_for_keyvalue($keyvalue = array()) {
    if ($this->schema) {
      $db_connection = DB::connection();
      if ($db_connection) {
        // should check to see if key of array exists in schema.
        $keys = array_keys($keyvalue);
        $key = $keys[0];
        $querystring = "SELECT * FROM " . $this->table .
          " WHERE " . $key . " = " .
          $this->sql_value_entick($keyvalue[$key], $key);
//    echo $querystring;
        $query = $db_connection->query($querystring);
        if ($query) return $query->fetch_assoc();
      }
    }
    return array();
  }
  
  function load_all_records($orderby = NULL) { //array('id'=>'DESC')) {
    if ($this->schema) {
      $db_connection = DB::connection();
      if ($db_connection) {
        $orderbyquery = '';
        if ($orderby) {
          $ordercolumn = key($orderby);
          $ordertype = $orderby[$ordercolumn];
          $orderbyquery = ' ORDER BY ' . sanitize_VARCHAR($ordercolumn) .
            ' ' . sanitize_VARCHAR($ordertype);
        }
        $query = $db_connection->query('SELECT * FROM ' . $this->table . $orderbyquery);
        $rows = array();
        while($record = $query->fetch_assoc())
          $rows[$record[$this->idkey]] = $record;
//        echo '<pre>'; var_dump($rows); echo '</pre>';
        return $rows;//$query->fetch_all();
      }
    }
    return array();
  }

  /**
   * helper function to convert a unix timestamp to a proper
   * DATETIME, for a given key within the record.
   */
  function convert_unix_to_datetime_for_key(&$record, $key) {
    $column = ArrayCheck::arr_get($this->schema, $key, NULL);
    if (!$column) return;
    if (ArrayCheck::arr_get($column, 'type', 'NOTDATETIME') == 'DATETIME') {
      // convert the item in the record.
      $timestamp = (int)$record[$key];
      $record[$key] = date("Y-m-d H:i:s", $timestamp);
    }
  }

  /**
   * helper function to convert a DATETIME to a
   * timestamp, for a given key within the record.
   */
  function convert_datetime_to_unix_for_key(&$record, $key) {
    $column = ArrayCheck::arr_get($this->schema, $key, NULL);
    if (!$column) return;
    if (ArrayCheck::arr_get($column, 'type', 'NOTDATETIME') == 'DATETIME') {
      // convert the item in the record.
      $datetime = (int)$record[$key];
      $record[$key] = strtotime($datetime);
    }
  }
  
  /**
   * Given a value and a key, 'entick' string values
   * as determined by the schema. Helper for building
   * SQL queries.
   */
  function sql_value_entick($value, $key) {
    switch (ArrayCheck::arr_get($this->schema[$key], 'sanitize', 'VARCHAR')) {
//    switch ($this->schema[$key]['sanitize']) {
      // textual types get ticks
      // PASSWORD is expected to have been hashed.
      // DATETIME is expected to be formatted properly by caller.
      case 'VARCHAR':
      case 'PASSWORD':
      case 'DATETIME':
      case 'EMAIL':
        return "'" . $value . "'";
    }
    return $value;
  }

  function write_record($record) {
    // if 'id' is given, then it's an update. otherwise, an insert.
    $affectedid = ArrayCheck::arr_get($record, $this->idkey, -1);
    $keys = array_intersect_key($record, $this->schema);
    // check if the id key exists. if it's not there, insert.
    if (!array_key_exists($this->idkey, $record)) {
      // prepare an insert.
      $into = array();
      $values = array();
      foreach ($keys as $key=>$value) { // discard $value
        $into[] = $key;
        $values[] = $this->sql_value_entick($record[$key], $key);
      }
      $queryString = 'INSERT INTO ' . $this->table . ' (' .
        implode(", ", $into) .
        ") VALUES (" .
        implode(", ", $values) . ")";
    } else { // id key exists, so do an update.
      // prepare an update.
      $set = array();
      foreach ($keys as $key=>$value) {
        $set[$key] = $key . "=" . $this->sql_value_entick($record[$key], $key);
      }
      // remove the id from the SET part of the query
      unset($set[$this->idkey]);
      $queryString = 'UPDATE ' . $this->table . ' SET ' .
        implode(", ", $set) .
        " WHERE " . $this->idkey . " = " . $record[$this->idkey];
    }
    $query = DB::connection()->query($queryString);
    if ($affectedid == -1) $affectedid = DB::connection()->insert_id;
    return $affectedid;
    // TODO: exception for a bad outcome.
  }

  function delete_record($id) {
    // for schema, delete record where key = id.
    $query = DB::connection()->query("DELETE FROM " . $this->table .
      ' WHERE ' . $this->idkey . ' = ' . $id);
    if (!$query) {
      echo "Failed to delete id $id from MySQL.";
    }
  }
  
/*  function load_all_records() {
    if ($this->db_connection) {
      $query = $this->db_connection->query("SELECT * FROM " . $table);
      $i=0;
      $result = array();
      while($i++ < $query->num_rows) {
        $row = $query->fetch_assoc();
        $result[$row['id']] = $row;
      }
      return $result;
    }
    return array();
  }

*/
}

