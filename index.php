<?php
include 'inc/common_head.inc';

include 'site/common_header.html.inc';

require_once 'inc/db.inc';
require_once 'inc/schema.inc';
require_once 'inc/usercrud.inc';
require_once 'inc/twistcrud.inc';
require_once 'inc/superglobals.inc';
require_once 'inc/sanitize.inc';

// this page exists so you can post 'twists,' which is what
// I call tweets in the context of this web app.

// this code is designed to work when I show off my project,
// not in a production environment.

/*$crud = new TwistCRUD;
$crud->join(new CRUDOOP('Ratings'));
$crud->join(new UserCRUD);

echo '<p class="header_title">' . $crud->join_sql() . '</p>';*/

$request_method = Server::request_method();
if ($request_method == 'GET') {
  // GET means:
  // viewing twist
  // adding twist type=add
  // editing twist type=edit
  // xml output type=xml
  // searching type=search
  //
  $twistdb = new TwistCRUD;
  $action = Get::get_get('action', '');
  $twistid = Get::get_get('id', '-1');
  if ($action == 'edit') {
    if (TRUE) {//Session::current_user_can_edit()) {
      echo '<h2>Edit this twist</h2>';
      echo '<form name="input" action="' . Server::php_self('index.php') . '" method="post">';
      echo '<input type="hidden" id="twist_edit" name="action" value="edit" />';
      echo $twistdb->html_form($twistid);
      echo '<input type="submit" value="Submit" />';
    } else {
      echo "<h2>Sorry. You can't edit anything.</h2>";
    }
  }
  else if ($action == 'add') {
    //$current_user = Session::current_user();
    if (TRUE) { //$current_user > 0) {
      echo '<h2>Add a twist</h2>'."\n";
      echo '<form name="twist_add" action="' . Server::php_self('index.php') . '" method="post">'."\n";
      echo '<input type="hidden" name="action" value="add" />'."\n";
      echo $twistdb->html_form($twistid);
      echo '<input type="submit" value="Submit" />';
    } else {
      echo "<h2>You can't add a twist yet.</h2>";
      echo '<div>Why not <a href="register.php">register</a> or <a href="login.php">log in</a>?</div>';
    }
  }
  else if ($action == 'delete') {
    if (TRUE) { //Session::current_user_can_edit()) {
      echo '<h2>Are you sure you want to delete this twist?</h2>';
      echo '<form name="input" action="' . Server::php_self('index.php') . '" method="post">';
      echo '<input type="hidden" id="twist_edit" name="action" value="delete" />';
      echo '<input type="hidden" id="twist_edit" name="id" value="' . $twistid . '" />';
      echo '<input type="submit" value="Delete" />';
      echo $twistdb->html_display($twistid);
    } else {
      echo "<h2>Sorry. You can't delete anything.</h2>";
    }
  }
  else if ($action == 'xml') {
    $twistdb = new TwistCRUD;
    $filename = $twistdb->xml();
    echo '<h3>Your XML file here: <a href="' . $filename . '">' . $filename . '</a></h3>';
  }
  else if ($action == 'view' && $twistid > 0) {
    // there's a valid twist ID
    echo '<h2>See this nifty twist?</h2>';
    echo $twistdb->html_display($twistid);
  } else {
    // by default we show all the twists.
    $twistdb = new TwistCRUD;
    $twistdb->table_show_all(TRUE, TRUE);
  }
//// end of GET
} else if ($request_method == 'POST') {
  // POST means to add or update this twist record.
  $action = Post::get('action', '');
  $twisid = Post::get('id', '-1');
  $twistdb = new TwistCRUD;
  switch ($action) {
    case 'delete':
      // delete item
      if (TRUE) { //Session::current_user_can_edit()) {
        $twistdb->delete_record(sanitize_INT($twisid));
        echo '<h2>Deleted.</h2>';
        $twistdb = new TwistCRUD;
        $twistdb->table_show_all(TRUE, TRUE);
      } else {
        echo "<h2>Sorry. You can't delete anything.</h2>";
      }
      break;
    case 'add':
    case 'edit':
      if (TRUE) {//Session::current_user_can_edit()) {
        $twistdb = new TwistCRUD();
        $userSchema = get_schema($twistdb->table_name());
        $input = Post::for_keys($userSchema);
        $input = sanitize_input_schema($input, $userSchema);
        echo '<pre>'; var_dump($input); echo '</pre>';
        //$input['created'] = time();
        $added = $twistdb->write_twist($input);
        echo 'Updated twist.';
        echo $twistdb->html_display($added);
      } else {
        echo "<h2>Sorry. You can't edit anything.</h2>";
      }
      break;
    default:
      $twistdb = new TwistCRUD;
      $twistdb->table_show_all(TRUE, TRUE);
  }
}

include 'site/common_footer.html.inc';

include 'inc/common_foot.inc';
