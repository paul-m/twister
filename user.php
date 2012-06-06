<?php
include 'inc/common_head.inc';

include 'site/common_header.html.inc';

require_once 'inc/db.inc';
require_once 'inc/schema.inc';
require_once 'inc/usercrud.inc';
require_once 'inc/superglobals.inc';
require_once 'inc/sanitize.inc';

// break the DB oop model because that's just how it goes. :-)
// this code is designed to work when I show off my project,
// not in a production environment.

$request_method = Server::request_method();
if ($request_method == 'GET') {
  // GET means:
  // listing users
  // showing single user
  // editing single user
  // deleing single user (are you sure?)
  $userdb = new UserCRUD;
  $action = Get::get_get('action', '');
  $userid = Get::get_get('id', '-1');
  if ($action == 'edit') {
    echo '<h2>Edit this user</h2>';
    echo '<form name="input" action="' . Server::php_self('user.php') .
      '" method="post">';
    echo '<input type="hidden" id="input" name="action" value="edit" />';
    echo $userdb->html_form($userid);
    echo '<input type="submit" value="Submit" />';
  }
  /*else if ($action == 'add') {
   this should be registration.
    $current_user = Session::current_user();
    if ($current_user > 0) {
      echo '<h2>Add a user</h2>'."\n";
      echo '<form name="twist_add" action="' . Server::php_self('index.php') . '" method="post">'."\n";
      echo '<input type="hidden" name="action" value="add" />'."\n";
      echo $userdb->html_form($userid);
      echo '<input type="submit" value="Submit" />';
    } else {
      echo "<h2>You can't add a twist yet.</h2>";
      echo '<div>Why not <a href="register.php">register</a> or <a href="login.php">log in</a>?</div>';
    }
  }*/
  else if ($action == 'delete') {
    echo '<h2>Are you sure you want to delete this user?</h2>';
    echo $userdb->html_display($userid);
    echo '<form name="input" action="' . Server::php_self('index.php') . '" method="post">';
    echo '<input type="hidden" id="twist_edit" name="action" value="delete" />';
    echo '<input type="hidden" id="twist_edit" name="id" value="' . $userid . '" />';
    echo '<input type="submit" value="Delete" />';
  }
  
  else {
    if ($userid > 0) {
      // there's a valid twist ID
      echo '<h2>Gaze upon this user!</h2>';
      echo $userdb->html_display($userid);
      if (Session::current_user_is_admin() || (Session::current_user() == $userid)) {
        echo '<div><a href="' . Server::php_self('user.php') .
          '?action=edit&id=' . $userid . '">edit</a> ';
        echo '<a href="' . Server::php_self('user.php') .
          '?action=delete&id=' . $userid . '">delete</a></div>';
      }
    } else {
      // no user ID so show the big list of users.
      $userdb = new UserCRUD;
      $userdb->table_show_all(TRUE, TRUE);
    }
  }
//// end of GET
} else if ($request_method == 'POST') {
  // POST means to add or update this twist record.
  $action = Post::get('action', '');
  $userdb = new UserCRUD;
  switch ($action) {
    case 'delete':
      // delete item
      $userdb->delete_record(sanitize_INT($_POST['id']));
      echo '<h2>Deleted.</h2><div>Why not look at <a href="' . Server::php_self('user.php') . '">the big list of users</a>?</div>';
      break;
    case 'edit':
      $userdb = new UserCRUD();
      $userSchema = get_schema($userdb->table_name());
      $input = Post::for_keys($userSchema);
      $input = sanitize_input_schema($input, $userSchema);
      $changed = $userdb->write_user($input);
      echo 'Edited user.';
      echo $userdb->html_display($changed);
      break;
    default:
      echo "wtf? $action";
  }
}

include 'site/common_footer.html.inc';

include 'inc/common_foot.inc';
