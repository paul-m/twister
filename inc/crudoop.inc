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
  
  // $joins is a list of all schema *names* that are joined
  // to this table.
  protected $joins;

  function __construct($table = '') {
    $this->set_schema($table);
  }

  function primary_key() {
    return $this->idkey;
  }

  public function table_name() {
    return $this->table;
  }

  function set_schema($table = '') {
    $this->table = NULL;
    $this->schema = NULL;
    $this->idkey = '';
    $this->clear_joins();
    $this->schema = get_schema($table);
    if ($this->schema) $this->table = $table;
    if (isset($this->schema['#UNIQUEID'])) {
      $this->idkey = $this->schema['#UNIQUEID'];
    }
  }
  
  function record_count() {
    // you fairly ask: How many records do we have?
    return count($this->records);
  }
  
  function record_id($id = NULL) {
    // do we have the record for this ID in memory?
    $result = FALSE;
    if (array_key_exists($id, $this->records))
      $result = $this->records[$id];
    return $result;
  }
  

  function crud_query($sql = '') {
    if ($sql == '') return NULL;
    $result = DB::connection()->query($sql);
    return $result;
  }

  function load_record($id = -1) {
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
    $primary_key = $this->primary_key();
    // if 'id' is given, then it's an update. otherwise, an insert.
    $affectedid = ArrayCheck::arr_get($record, $primary_key, -1);
    $keys = array_intersect_key($record, $this->schema);
    // check if the id key exists. if it's not there, insert.
    if (!array_key_exists($primary_key, $record)) {
      // prepare an insert.
      $into = array();
      $values = array();
      foreach ($keys as $key=>$value) { // discard $value
        $into[] = $key;
        $values[] = $this->sql_value_entick($record[$key], $key);
      }
      $queryString = 'INSERT INTO ' . $this->table_name() . ' (' .
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
      $queryString = 'UPDATE ' . $this->table_name() . ' SET ' .
        implode(", ", $set) .
        " WHERE " . $this->idkey . " = " . $record[$this->idkey];
    }
//echo '<pre>'; print_r($record); echo '</pre>';
//echo $queryString;
    $query = $this->crud_query($queryString);
    if ($affectedid == -1) $affectedid = DB::connection()->insert_id;
    return $affectedid;
    // TODO: exception for a bad outcome.
  }

  function delete_record($id) {
    // for schema, delete record where key = id.
    $query = $this->crud_query("DELETE FROM " . $this->table_name() .
      ' WHERE ' . $this->primary_key() . ' = ' . $id);
    if (!$query) {
      echo "Failed to delete id $id from MySQL.";
    }
  }
  
  
  /**
   * CRUDOOP::join_sql()
   *
   * Generates the text of a SQL command joining the 
   */

  function select($columns = NULL) {}

  /**
   * CRUDOOP::join()
   *
   * Join other crudoop objects here.
   * $anotherCrudOOP should contain a primary key that is
   * also present in our own schema.
   * This system will do a left join with all the join()ed objects
   * in the order they were join()ed.
   */
  function join($anotherCrudOOP = NULL) {
    if (!$anotherCrudOOP) return NULL;
    if (!is_a(CRUDOOP)) return NULL; // this really should raise an exception
    // do we have their key as a column?
    $foreign_key = $anotherCrudOOP->primary_key();
    if (!isset($this->table[$foreign_key])) return NULL;
    // it seems the object qualifies, so we add it.
    $this->joins[] = $anotherCrudOOP;
    return $this;
  }
  
  function clear_joins() {
    $this->joins = array();
    $this->join_schema = array();
  }

  function search($query, $columns = array(), $limit = 20) {
    // $query is the literal SQL string that goes after WHERE
    // eg %searchterm%
    // $columns is the set of columns to find within.
    // "SELECT DISTINCT key FROM table WHERE column like '%term%'"
    $primary_key = $this->primary_key();
    $table = $this->table();
    $sql = "SELECT DISTINCT $primary_key FROM $table WHERE ";
    $like = array();
    foreach ($columns as $column) {
      $like[] = $column . ' LIKE ' . $query;
    }
    $sql .= implode(' OR ', $like) . " LIMIT $limit";
    $result = DB::connection()->query($sql);
    if (!$result) echo "Failed to find stuff.";
    $hits = $result->fetch_all();
  }

  function join_sql($where = '') {
    $tables = array();
    $sql = 'SELECT * FROM ' . $this->table_name() . ' ';
    $last_table = $this->table_name();
    foreach($this->joins as $join) {
      $join_table = $join->table_name();
      $join_key = $join->primary_key();
      $sql .= "LEFT JOIN $join_table ON $last_table.$join_key = $join_table.$join_key ";
      $last_table = $join_table;
    }
    if ($where != '') $sql .= " WHERE $where";
    return $sql;
  }
}
