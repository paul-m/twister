<html><head>
<title>Project Milestone 6 Gibberish Generator, ITA343</title>
</head>

<div>Project Milestone 6 Gibberish Generator, ITA343<br>Paul Mitchum (<a href="mailto:paulm77@uw.ed">paulm77</a>)<br>May 24, 2012</div>

<h2>Adding a bunch of stuff to the table</h2>
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

function random_twist() {
  global $connection;
  // return SQL for a random Twist entry
  $userid = rand(1, 20);
  $points = rand(0, 666);
  $reply_to = rand(0, 666);
  $sql = "insert into Twists (USERID, reply_to_TWISTID, twist, points_total) VALUES ($userid, $reply_to, '$userid randomly got $points points', $points)";
  return $sql;
}

for ($i = 30; $i >= 0; --$i) {
  mysql_query(random_twist(), $connection);
}

