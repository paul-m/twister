<?php
include 'inc/common_head.inc';

include 'site/common_header.html.inc';

require_once 'inc/schema.inc';
require_once 'inc/usercrud.inc';
require_once 'inc/superglobals.inc';
require_once 'inc/sanitize.inc';


$request_method = Server::request_method();
if ($request_method == 'GET') {
  // GET means:
  // 1) user is logged in, offer to log out.
  // 2) user is not logged in, show registration form.
  // 3) profit
  $userID = Session::current_user();
  if ($userID > 0) {
    // user is logged in.
    $userdb = new UserCRUD;
    $user = $userdb->load_user($userID);
    $first = $user['first_name'];
    echo "Greetings, $first! You shouldn't need to register.";
    // redirect_to_front();
  } else {
    // user not logged in.
    // show a registration form.
    $user = new UserCRUD;
    echo '<form name="input" action="' . Server::php_self('register.php') . '" method="post">';
    echo '<fieldset><legend>Register:</legend>';
    echo $user->html_form(-1);
    echo '<input type="submit" value="Submit" />';
    echo '</fieldset>';
  }

// end of GET.

} else if ($request_method == 'POST') {
  // POST means user tried to add a user record.
  $userSchema = get_schema('users');
  $input = Post::for_keys($userSchema);
  
/*  if (Post::get('password_reenter') != $input['password']) {
    Session::set_message('Passwords do not match.');
  }*/
  $input = sanitize_input_schema($input, $userSchema);
  unset($input['id']);
  $cruddy = new UserCRUD();
  $cruddy->write_user($input);
  echo '<div>Welcome! Why not <a href="login.php">log in</a>?</div>';
  //send_email();
  // redirect to register yay! login.
}

include 'inc/common_foot.inc';
