// Twister JavaScript
// 
// Basically, we want:
// To count the number of characters in a text field.
// Show the count in a div.
// Enable a submit button if the count is right.

var twist_typing = function () {
  var counter = $("#id_twist_count");
  var submit = $("#id_Twists_submit");
  var length = $("#id_Twists_twist").val().length;
  submit.attr("disabled", ((length < 1) || (length > 140)));
  if (length > 0) {
    counter.html(length);
  }
  else {
    counter.html("");
  }
}

var twist_validate = function () {
  var length = $("#id_Twists_twist").val().length;
  if ((length < 1) || (length > 140)) {
    alert("Your twist must be at least one character, and no more than 140.");
    return false;
  }
}

window.onload = function() {
  $("#id_Twists_twist").keyup(twist_typing);
  $("#id_Twists_twist").keyup();
  $("#twist_add").submit(twist_validate);
}
