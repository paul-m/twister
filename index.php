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

// break the DB oop model because that's just how it goes. :-)
// this code is designed to work when I show off my project,
// not in a production environment.

$request_method = Server::request_method();
if ($request_method == 'GET') {
  // GET means:
  // viewing twist
  // adding twist type=add
  // editing twist type=edit
  $twistdb = new TwistCRUD;
  $type = Get::get_get('type', '');
  $twistid = Get::get_get('id', '-1');
  if ($type == 'edit') {
    if (Session::current_user_can_edit()) {
      echo '<h2>Edit this twist</h2>';
      echo '<form name="input" action="' . Server::php_self('index.php') . '" method="post">';
      echo '<input type="hidden" id="twist_edit" name="type" value="edit" />';
      echo $twistdb->html_form($twistid);
      echo '<input type="submit" value="Submit" />';
    } else {
      echo "<h2>Sorry. You can't edit anything.</h2>";
    }
  }
  else if ($type == 'add') {
    $current_user = Session::current_user();
    if ($current_user > 0) {
      echo '<h2>Add a twist</h2>'."\n";
      echo '<form name="twist_add" action="' . Server::php_self('index.php') . '" method="post">'."\n";
      echo '<input type="hidden" name="type" value="add" />'."\n";
      echo $twistdb->html_form($twistid);
      echo '<input type="submit" value="Submit" />';
    } else {
      echo "<h2>You can't add a twist yet.</h2>";
      echo '<div>Why not <a href="register.php">register</a> or <a href="login.php">log in</a>?</div>';
    }
  }
  else if ($type == 'delete') {
    if (Session::current_user_can_edit()) {
      echo '<h2>Are you sure you want to delete this twist?</h2>';
      echo $twistdb->html_display($twistid);
      echo '<form name="input" action="' . Server::php_self('index.php') . '" method="post">';
      echo '<input type="hidden" id="twist_edit" name="type" value="delete" />';
      echo '<input type="hidden" id="twist_edit" name="id" value="' . $twistid . '" />';
      echo '<input type="submit" value="Delete" />';
    } else {
      echo "<h2>Sorry. You can't delete anything.</h2>";
    }
  }
  else if ($type == 'xml') {
    $twistdb = new TwistCRUD;
    $filename = $twistdb->xml();
    echo '<h3>Your XML file here: <a href="' . $filename . '">' . $filename . '</a></h3>';
  }
  
  else {
    if ($twistid > 0) {
      // there's a valid twist ID
      echo '<h2>See this nifty twist?</h2>';
      echo $twistdb->html_display($twistid);
    } else {
      // no valid twist ID so show the big list of twists.
      $twistdb = new TwistCRUD;
      $twists = $twistdb->load_all_records(array('created'=>'DESC'));

      if (count($twists) > 0) {
        $db = DB::connection();
        
        // 'survey_id' is our stand-in for user id.
        $result = $db->query("SELECT DISTINCT survey_id FROM questions");
        $uids = array();
        while ($uid = $result->fetch_assoc()) $uids[$uid['survey_id']] = $uid['survey_id'];
        
        // $uids now contains all the users we'll need.
        // let's load all the users.
        
        $users = array();
        $userdb = new UserCRUD;
        foreach ($uids as $uid=>$foo) {
          $user = $userdb->load_user($uid);
          if ($user) $users[$user['id']] = $user;
        }
        
        // gather admin and current info
        $admin = Session::current_user_is_admin();
        $current_user = $userdb->load_user(Session::current_user());

        echo '<h2>The Big List Of Twists</h2>';
        echo '<table border="1"><tr>';
        // header row
        echo '<th>Twist</th><th>By</th>';
        // ...and now all the twist rows.
        foreach($twists as $twist) {
          $user = ArrayCheck::arr_get($users, $twist['survey_id'], -1);
          echo '<tr>';
          echo '<td><a href="'. Server::php_self('index.php') . '?id=' . $twist['id'] . '">' .
            $twist['question'] . '</a></td><td>' .
              ArrayCheck::arr_get($user,'user_name','&lt;unknown&gt;') . '</td>';
          echo '</tr>';
        }
        echo '</table>';
      } else {
        // no twists to display.
        echo "<h2>Sorry, there aren't any twists to lists.</h2>";
        echo '<div>Why not <a href="' . Server::php_self('index.php') . '?type=add">add one</a>?</div>';
      }
    }
  }
//// end of GET
} else if ($request_method == 'POST') {
  // POST means to add or update this twist record.
  $type = Post::get('type', '');
  $twistdb = new TwistCRUD;
  switch ($type) {
    case 'delete':
      // delete item
      if (Session::current_user_can_edit()) {
        $twistdb->delete_record(sanitize_INT($_POST['id']));
        echo '<h2>Deleted.</h2><div>Why not look at <a href="' . Server::php_self('index.php') . '">the big list of twists</a>?</div>';
      } else {
        echo "<h2>Sorry. You can't delete anything.</h2>";
      }
      break;
    case 'add':
    case 'edit':
      if (Session::current_user_can_edit()) {
        $userSchema = get_schema('questions');
        $input = Post::for_keys($userSchema);
      //  echo '<pre>'; var_dump($input); echo '</pre>';
        $input = sanitize_input_schema($input, $userSchema);
        $input['created'] = time();
        $twistdb = new TwistCRUD();
        $added = $twistdb->write_twist($input);
        echo 'Added twist.';
        echo $twistdb->html_display($added);
      } else {
        echo "<h2>Sorry. You can't edit anything.</h2>";
      }
  }
}

include 'site/common_footer.html.inc';

include 'inc/common_foot.inc';
