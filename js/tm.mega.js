;(function($){
$(document).ready(function() {

  $('.button-collapse').sideNav({
    menuWidth: 300, // Default is 240
    closeOnClick: true
  });
  
    $(".plus-button").click(function(){
        $(this).next("ul").slideToggle("slow");
    });
    $(".plus-button").click(function(){
        $(this).toggleClass("opened");
    });
    

         
         
});
<<<<<<< Updated upstream
})(jQuery);
=======
<<<<<<< HEAD
<<<<<<< HEAD
})(jQuery);
=======
})(jQuery);
>>>>>>> origin/7.x-1.x-dev
=======
})(jQuery);
>>>>>>> origin/7.x-1.x-dev
>>>>>>> Stashed changes
