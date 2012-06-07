<?php
include 'inc/common_head.inc';

include 'site/common_header.html.inc';

require_once 'inc/arraycheck.inc';
require_once 'inc/superglobals.inc';
require_once 'inc/usercrud.inc';

$loginError = FALSE;

if (Server::request_method() == 'GET') {
  // request is either for a login form or to logout.
  $type = Get::get_get('action', '');
  if ($type == 'logout') {
    // User wants to logout.
    Session::set_current_user(-1);
//    Session::destroy();
    echo "<div>You're logged out.</div>";
    include 'site/loginform.html.inc';
  }
  if ($type == 'login' || $type == '') {
    if (Session::current_user() > 0) {
      //login_as_different_user_message()
      include 'site/logindifferent.html.inc';
    }
    // show the login form. POSTs back to login.php.
    include 'site/loginform.html.inc';
  }

// end of GET

} else if (Server::request_method() == 'POST') {
  // process login post from this page.
//  $username = sanitize_VARCHAR(Post::get('user_name', NULL));
  $username = Post::get('user_name', NULL);
  if ($username) {
    $password = Post::get('password', NULL);
    if ($password) {
      $usercrud = new UserCRUD;
      $user = $usercrud->load_user_username($username);
      if (User::hash_password($password) == $user['password']) {
        Session::set_current_user($user['USERID']);
        include 'site/logindifferent.html.inc';
        include 'site/loginform.html.inc';
      } else {
        $loginError = TRUE;
        // tell user they got the password wrong.
      }
    } else {
      $loginError = TRUE;
      // tell user they need a password to log in.
    }
  } else {
    $loginError = TRUE;
    // tell user they need a user name to log in.
  }
}

if ($loginError) {
  include 'site/loginerror.html.inc';
  include 'site/loginform.html.inc';
}

include 'inc/common_foot.inc';