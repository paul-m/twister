<html><head>
<title>Project Milestone 7 User Generator, ITA343</title>
</head>

<div>Project Milestone 7 User Generator, ITA343<br>Paul Mitchum (<a href="mailto:paulm77@uw.ed">paulm77</a>)<br>May 30, 2012</div>

<h2>Adding some users to the database</h2>
<div><a href="querytable.php">Go look at it.</a></div>
<?php

// assuming you've got a database here.....

$connection = FALSE;

$connection = mysql_connect("ovid.u.washington.edu:5611", "paulm77", "hYN5ZYlp");

//$connection = mysql_connect("localhost:8889", "root", "root");

if (!$connection) {
  // Better error handling here in actual app.
  die('This homework was unable to make a connection. Please try again later. Or something.');
  }

if (!mysql_select_db("paulm77", $connection)) {
  // Better error handling here in actual app.
  die('This homework was unable to use a database. Please try again later. Or something.');
  }

require_once('names.inc');

$names = get_names();

foreach($names as $name) {
  mysql_query("insert into Users (user_name) VALUES ('$name')", $connection);
}
