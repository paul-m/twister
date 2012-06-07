<?php

require_once 'crudoop.inc';
require_once 'arraycheck.inc';

class CRUDDisplay extends CRUDOOP {

  // return HTML as a nice display of the data.
  // we'll just make a table for now. easy to process.
  function html_display($id = -1) {
    $record = $this->load_record($id);

    if ($record) {
      $table = array();
      $schema = $this->schema;
      foreach($schema as $db_col=>$info) {
        if ($db_col != '#UNIQUEID') {
          $display_info = ArrayCheck::arr_get($info, 'form', NULL);
          if ($display_info) {
            $display = ArrayCheck::arr_get($display_info, 'display', '');
            if ($display != 'hidden') {
              $label = ArrayCheck::arr_get($display_info, 'label', $db_col);
              $text = $record[$db_col];
              if ($display == 'password')
                $text = '*****';
              $table[$db_col] = array ($label => $text);
            }
          }
        }
      }
              //echo '<pre>'; var_dump($table); echo '</pre>';
      if (count($table) < 1)
        return "<div><h3>Unable to display record id: $id.</h3></div>";

      // let's make a chart.
      $html = '<table border="0">';
      foreach($table as $foo=>$row) {
        $html .= '<tr>';
        foreach($row as $key=>$value) {
          $html .= '<td>' . $key . ':</td><td>' . $value . '</td>';
        }
        $html .= '</tr>';
      }
      return $html;

//      print_r($table);
    } else {
    return "<div><h3>Unable to load record for display. id: $id.</h3></div>";
    }
  }


  // return HTML as a form for entering the data.
  // caller must supply their own <form> tag and submit button.
  function html_form($id = -1) {
  //echo "<br>id = $id<br>";
    if ($id > 0) {
      $record = $this->load_record($id);
    } else {
      $record = array($this->primary_key() => -1);
    }
    $schema = $this->schema;
    $primary_key = $this->primary_key();
    $html = '';
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

        $html .= '<input type="' . $display_info['type'] .
          '" name="' . $form_name . 
          '" value="' . ArrayCheck::arr_get($record, $db_col, '') . '" /><br>' . "\n";
      }
    }
    return $html;
  }

  // dump our table to an XML file.
  function xml($filepointer = NULL) {
    $tablexmlname = $this->table . '-db-table';
    if (!$filepointer) $file = fopen($tablexmlname . '.xml', 'w');

    $xml = '';
    $data = $this->load_all_records();
//    echo '<pre>'; print_r($data); echo '</pre>';
    $xml .= '<?xml version="1.0" ?>' . "\n" .
      '<' . $tablexmlname . ">\n";
    fwrite($file, $xml);
    foreach($data as $recordID=>$record) {
      
      $xml = '<recordtype ID="' . htmlentities($recordID) . '">' . "\n";
      foreach($record as $key=>$value) {
        $xml .= "<$key>" . htmlentities($value) . "</$key>\n";
      }
      $xml .= '</recordtype>' . "\n";
      fwrite($file, $xml);
    }
    $xml = '</' . $tablexmlname . ">\n";
    fwrite($file, $xml);
    fclose($file);
    return $tablexmlname . '.xml';
  }

  // dump our table to an XML file.
  function rss($filepointer = NULL) {
    $tablexmlname = $this->table . '-db-table';
    if (!$filepointer) $file = fopen($tablexmlname . '.rss', 'w');

    $xml = '';
    $data = $this->load_all_records();
//    echo '<pre>'; print_r($data); echo '</pre>';
    $xml .= '<?xml version="1.0" ?>' . "\n" .
      '<' . $tablexmlname . ">\n";
    fwrite($file, $xml);
    foreach($data as $recordID=>$record) {
      
      $xml = '<recordtype ID="' . htmlentities($recordID) . '">' . "\n";
      foreach($record as $key=>$value) {
        $xml .= "<$key>" . htmlentities($value) . "</$key>\n";
      }
      $xml .= '</recordtype>' . "\n";
      fwrite($file, $xml);
    }
    $xml = '</' . $tablexmlname . ">\n";
    fwrite($file, $xml);
    fclose($file);
    return $tablexmlname . '.xml';
  }



/*
    $table = array();
    $schema = $this->schema;
    $html = '';
    foreach($schema as $db_col=>$info) {
      $display_info = NULL;
      if (isset($info['form'])) $display_info = $info['form'];
//      echo '<pre>'; print_r($display_info); echo '</pre>';
      // we don't want to show #uniqueid.
      // there's probably a better way to structure this.
      if ($display_info && (!($db_col == '#UNIQUEID'))) {
        if ($display_info['type'] != 'hidden') {
          $html .= '<label for="' . $db_col . '">' . $display_info['label'] . "</label>\n";
        }
        $html .= '<input type="' . $display_info['type'] .
          '" name="' . $db_col . 
          '" value="' . $record[$db_col] . '" /><br>' . "\n";
      }
    }
    return $html;
  }
*/


  /**
   * $result is a mysqli result object
   */
  function html_generic_table($result = NULL, $allow_view = FALSE, $allow_edit = FALSE, $allow_delete = TRUE) {
    if ($result) {
      // set up the column display names
      $columns = $this->table_column_display_info();
      // start showing data
      $showme = $result->fetch_assoc();
      $has_primary_key = ArrayCheck::arr_get($showme, $this->primary_key(), FALSE);
      if ($showme) {
        $name = $this->table_name();
        echo "<h2>$name</h2>\n";
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
            if ($allow_delete)
              $operations .= ' ' . $this->delete_link($showme[$this->primary_key()], 'delete');
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
   * $result is a mysqli result object
   * this function differes from html_generic_table by showing the raw
   * column names.
   */
  function html_result_table($result = NULL, $allow_view = FALSE, $allow_edit = FALSE, $allow_delete = FALSE) {
    if ($result) {
      // start showing data
      $showme = $result->fetch_assoc();
      if ($showme) {
        $has_primary_key = ArrayCheck::arr_get($showme, $this->primary_key(), FALSE);
        $columns = array_keys($showme);
        $name = $this->table_name();
        echo "<h2>$name</h2>\n";
        // start table
        echo '<table cellpadding="3" cellspacing="0" border="1">';
        // show DB column names as table headers
        echo "<tr>\n";
        foreach ($columns as $column) {
          echo "<th>$column</th>";
        }
        if (($allow_edit || $allow_view) && $has_primary_key) echo '<th></th>';
        echo "</tr>\n";
        // now start showing row data
        $rownum = 0;
        do {
          if (++$rownum & 1) echo '<tr class="odd">';
          else echo '<tr class="even">';
          foreach ($columns as $column) {
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
            if ($allow_delete)
              $operations .= $this->delete_link($showme[$this->primary_key()], 'view');
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
   * Hands us an array with the display info for
   * all the columns. Useful for building tables.
   */
  function table_column_display_info() {
    // set up the column display names
    $columns = array();
    $schema = $this->schema;
    foreach($schema as $db_col=>$info) {
      if ($db_col != '#UNIQUEID') {
        $display_info = ArrayCheck::arr_get($info, 'form', NULL);
        if ($display_info) {
          $display = ArrayCheck::arr_get($display_info, 'display', '');
          if ($display != 'hidden') {
            $label = ArrayCheck::arr_get($display_info, 'label', $db_col);
            $columns[$db_col] = $label;
          }
        }
      }
    }
    return $columns;
  }

  /**
   * Returns the text of a link to edit a given user, by id.
   */
  function edit_link($id, $text) {
//    if ($id < 1) return '<div class="disabled">' . $text . '</div>';
    return '<div><a href="' . Server::php_self('index.php') .
      '?action=edit&id=' . $id . '">' . $text . '</a></div>';
  }

  /**
   * Returns the text of a link to view a user, by id.
   */
  function view_link($id, $text) {
//    if ($id < 1) return '<div class="disabled">' . $text . '</div>';
    return '<div><a href="' . Server::php_self('index.php') .
      '?action=view&id=' . $id . '">' . $text . '</a></div>';
  }
  
  function delete_link($id, $text) {
//    if ($id < 1) return '<div class="disabled">' . $text . '</div>';
    return '<div><a href="' . Server::php_self('index.php') .
      '?action=delete&id=' . $id . '">' . $text . '</a></div>';
  }

  function table_show_all($allow_view = FALSE, $allow_edit = TRUE) {
    $table = $this->table_name();
    $sql = "SELECT * FROM $table";
    $results = $this->crud_query($sql);
    if ($results->num_rows > 0)
      $this->html_generic_table($results, $allow_view, $allow_edit);
    else
      echo '<div>No results.</div>';
  }

}



