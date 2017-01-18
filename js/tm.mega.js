;(function($){
$(document).ready(function() {

  $('.button-collapse').sideNav({
    menuWidth: 300, // Default is 240
    closeOnClick: true
  });
  
  $(".plus-button").click(function(){
    var height = $(this).closest("ul").height();
    $(this).closest("ul").css("min-height", height);
    
    if ($(this).hasClass("opened")) {
      $(this).next("ul").slideToggle("slow");
      $(this).removeClass("opened");
    } else {
      $(".opened").next("ul").slideToggle("slow");
      $(".plus-button").removeClass("opened");
      $(this).addClass("opened");
      $(this).next("ul").slideToggle("slow");
    }
  });

});

})(jQuery);
