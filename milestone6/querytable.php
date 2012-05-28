<?php

/**
 * @file Times-table/Database HTML table maker
 *
 * Makes HTML tables out of two sets of data: 1-10 times tables
 * and arbitary database tables with zebra styling.
 */

// Here comes the style sheet...
?>
<html><head>
<style type="text/css">
.heading {
  font-weight: bold;
}
.odd {
  background: white;
  color: black;
}
.even {
  background: #DDDDFF;
  color: black;
}
</style>
<title>Project Milestone 6, ITA343</title>
</head>

<div>Project Milestone 6, ITA343<br>Paul Mitchum (<a href="mailto:paulm77@uw.ed">paulm77</a>)<br>May 24, 2012</div>

<?php

// assuming you've got a database here.....

$connection = FALSE;

$connection = mysql_connect("ovid.u.washington.edu:5611", "paulm77",
"");

//$connection = mysql_connect("localhost:8889", "root", "root");

if (!$connection) {
  // Better error handling here in actual app.
  die('This homework was unable to make a connection. Please try again later. Or something.');
  }

//mysql_select_db("paulm77", $connection);

//$db = 'TwisterDB';
$db = 'paulm77';

if (!mysql_select_db($db, $connection)) {
  // Better error handling here in actual app.
  die('This homework was unable to use a database. Please try again later. Or something.');
  }

/// do a query
$table = 'Twists';
$result = mysql_query("select * from $table LIMIT 300", $connection);

// important that $showme be associative
$showme = mysql_fetch_array($result, MYSQL_ASSOC);
if ($showme) {
  echo "<h2>$db : $table</h2>\n";
  $columns = array_keys($showme);
  // start table
  echo '<table cellpadding="3" cellspacing="0" border="1">';
  // show DB column names as table headers
  echo "<tr>\n";
  foreach ($columns as $column) {
    echo "<th>$column</th>";
  }
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
    echo '</tr>';
  } while ($showme = mysql_fetch_array($result, MYSQL_ASSOC));
}
else { // no results
  echo '<p>No results for this query.</p>';
}
