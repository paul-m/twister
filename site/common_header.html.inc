<html>
<head>
<META HTTP-EQUIV="Expires" CONTENT="-1">
<LINK href="site/common.css" rel="stylesheet" type="text/css">
<!-- LINK href="site/more.css" rel="stylesheet" type="text/css" -->
<script type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js"></script> 
<script type="text/javascript" src="site/twister.js"></script>
<title>Twister</title></head>
<body>
<nav id="centeredmenu">
   <ul>
      <li><a href="index.php">Twists</a></li>
      <li><a href="index.php?action=add">Add a Twist</a></li>
      <li><a href="user.php">Users</a></li>
      <li><a href="register.php">Register</a></li>

<?php // let's find out if we're logged in.
require_once('inc/superglobals.inc');
if (Session::current_user() < 1) echo '<li><a href="login.php">Login</a></li>';
else echo '<li><a href="login.php?action=logout">Logout</a></li>';
?>
      <li>
  <form class="searchform" action="index.php" method="post">
  <input type="hidden" name="action" value="search">
	<input class="searchfield" type="text" name="searchterm" value="Search..." onfocus="if (this.value == 'Search...') {this.value = '';}" onblur="if (this.value == '') {this.value = 'Search...';}" />
	<input class="searchbutton" type="submit" value="Go" />
  </form>
      </li>
   </ul>
</nav>
<div id="wrapper">
<div id="content">
<div id="item">
