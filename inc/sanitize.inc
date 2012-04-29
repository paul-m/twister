<?php

require_once 'db.inc';

function sanitize_input_schema($input = array(), $schema = array()) {
  // return an array made up of key/value pairs from $input,
  // for keys in $schema, with sanitized values.
  $result = array();
  $keys = array_intersect_key($input, $schema);
  foreach($keys as $key=>$value) {
    $type = ArrayCheck::arr_get($schema, 'sanitize', 'VARCHAR');
    $funcName = 'sanitize_' . $type;
    if (function_exists($funcName)) {
      $result[$key] = $funcName($input[$key]);
    }
  }
  return $result;
}

function sanitize_INT($int) {
  // input something and output an int
  if (!is_numeric($int)) return -1;
  return (int) $int;
}
  
function sanitize_VARCHAR($varchar) {
  // Standard sanitize voodoo for varchar input text
  // heading for a MySQL database.
  if (get_magic_quotes_gpc()) $varchar = stripslashes($varchar);
  return DB::real_escape_string($varchar);
}

function sanitize_EMAIL($email) {
  return filter_var($email, FILTER_SANITIZE_EMAIL);
}

function sanitize_DATETIME($datetime) {
  return sanitize_VARCHAR($datetime);
}
