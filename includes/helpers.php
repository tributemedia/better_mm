<?php

/*
 * Builds BMM block desktop and mobile HTML.
 * 
 * @param $menu_name
 *   The name of the menu to load the link structure from. If a menu name is
 *   not provided, then the default menu name set in the BMM settings will be
 *   used.
 *
 * @return array
 *   Returns a multi-dimensional array with two indices. The first is 'html'
 *   which is the HTML structure for the desktop menu. The second index,
 *   'mobile', is the HTML menu structure for a mobile menu.
 */
function bmm_gen_block_html($menu_name = NULL) {
  $menu_settings = variable_get('bmm_menu_items_settings');
  $mobile_settings = variable_get('bmm_mobile_menu_settings');
  $menu_items = '';
  
  if(isset($menu_name)) {
    $menu_items = menu_navigation_links($menu_name);
  }
  else {
	$menu_items = variable_get('bmm_menu_items');
  }
    
  // Build HTML for blocks
  $below_mobile_nav = $mobile_settings['below_mobile_nav']['value'];
  $mobile_footer = $mobile_settings['bmm_menu_mobile_footer']['value'];
  $key = 0;
  $html = '<ul class="mega-menu mega-level-1 first hide-on-med-and-down">';
  $mobile_html = '<ul id="nav-mobile" class="side-nav">';
  $mobile_html .= better_mm_mobile_secondary_html($mobile_settings);
  $mobile_html .= '<ul class="main-menu-navigation">';
  
  if (!empty($menu_items)) {
    foreach ($menu_items as $menu_item) {
      $item_name = str_replace(' ', '-', strtolower($menu_item['title']));
      $link_attributes = isset($menu_item['attributes']) ? 
        $menu_item['attributes'] : array();
      $item_attributes = isset($menu_item['item_attributes']) ? 
        $menu_item['item_attributes'] : array();
      $style = $menu_settings[$item_name]['style'];
      $nolink = $menu_settings[$item_name]['style_settings']['nolink'];
      $fid = $menu_settings[$item_name]['style_settings']['wide_setting'];
      $adv_content = $menu_settings[$item_name]['style_settings']['advanced_setting']['value'];
      $children = better_mm_get_children($menu_item);
      $l1_class = better_mm_get_li_class($key, $menu_items);
      $child_class = better_mm_get_child_class($children, $style);
      $child_class_mob = better_mm_get_child_class($children, $style, TRUE);
      $item_classes = $item_attributes['class'];
      $class_list = $item_classes . $l1_class . $child_class;
      $class_list_mob = $item_classes . $l1_class . $child_class_mob;
      $item_id = $item_attributes['id'];
            
      if ($menu_item['href'] == '<front>') {
        $url = url('<front>');
      } elseif (preg_match('/^http/', $menu_item['href']) === 1) {
        $url = $menu_item['href'];
      } else {
        $url = '/' . drupal_get_path_alias($menu_item['href']);
      }
      
      // If a file has been uploaded, set file status to permanent.
      if (is_numeric($fid) && $fid != 0) {
        $item = menu_get_item($menu_item['href']);
        $mlid = db_select('menu_links' , 'ml')
          ->condition('ml.link_path' , $item['href'])
          ->fields('ml' , array('mlid'))
          ->execute()
          ->fetchField();
        $file = file_load($fid);
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);
        file_usage_add($file, 'better_mm', 'menu-item', $mlid);
      }
      
      $html .= '<li ';
      if ($item_id) {
        $html .= 'id="' . $item_id . '" ';
      }
      $html .= 'class="' . $class_list . '">';
      
      $mobile_html .= '<li ';
      if ($item_id) {
        $mobile_html .= 'id="' . $item_id . '" ';
      }
      $mobile_html .= 'class="' . $class_list_mob . '">';
      
      if ($nolink) {
        $link_attributes['class'][] = 'no-link';
        $html .= l($menu_item['title'], $url, array('attributes' => $link_attributes));
        $mobile_html .= l($menu_item['title'], $url, array('attributes' => $link_attributes));
      }
      
      if (!$nolink) {
        $html .= l($menu_item['title'], $url, array('attributes' => $link_attributes));
        $mobile_html .= l($menu_item['title'], $url, array('attributes' => $link_attributes));
      }
      
      if ($style == 'advanced') {
        // Generate HTML for advanced item drop down.
        if (!empty($adv_content)) {
          $html .= '<ul class="mega-level-2 ' . $style . '">';
          $html .= '<div class="inner row">' . $adv_content . '</div> <!-- end inner row -->';
          $html .= '</ul>';
        }
        $html .= '</li>';
      }
      if (($style == 'wide') || ($style == 'standard')) {
        // Generate HTMl for the children of wide or standard items.
        $html .= better_mm_menu_item_tree($style, $children, $fid);
      }
      $mobile_html .= better_mm_menu_item_tree($style, $children, $fid, TRUE);
      $key++;
    } // foreach menu_item
    $html .= '</ul>';
    $mobile_html .= '</ul>' . $mobile_footer;
    $mobile_html .= '</ul>';
    // Add mobile nav button.
    $html .= '<span class="nav-button valign-wrapper hide-on-large-only">';
    $html .= '<div class="valign">';
    $html .= '<a href="#" data-activates="nav-mobile" class="button-collapse">';
    $html .= '<i class="material-icons">menu</i><span class="menu-text">menu</span></a>';
    // Add mobile code below mobile nav button.
    $html .= $below_mobile_nav . '</div></span>';
  }
  
  return array('html' => $html, 'mobile' => $mobile_html);
}

/*
 * Recursively drills down into the children of a parent menu item (the 
 * haystack) to find a menu item, and its children items (the needle).
 *
 * @param $menu_item_needle
 *   The menu item whose children need to be found, and returned.
 *
 * @param $menu_item_haystack
 *   The parent menu item to search through.
 *
 * @return array
 *   A multi-dimensional array consisting of the passed in menu item's 
 *   children.
 * 
 * @return NULL
 *   If no menu item is found matching the menu item passed in, NULL is
 *   returned.
 */
function find_menu_item_children($menu_item_needle, $menu_item_haystack) {
  // This is the base case of the recursive function. If the passed in
  // menu item matches what we're looking for (based on the link href and
  // title passed in) then return this menu item's children.
  if($menu_item_needle['link']['link_path'] == 
  $menu_item_haystack['link']['link_path'] &&
  $menu_item_needle['link']['title'] == 
  $menu_item_haystack['link']['title']) {
	return $menu_item_haystack['below'];
  }
  // Otherwise, drill down into this current menu item's children (if there
  // are any). If there are children, recursively call this function to 
  // explore the child menu item's children for what we're looking for.
  // Otherwise, return NULL.
  else {
	if(count($menu_item_haystack['below']) > 0) {
	  foreach($menu_item_haystack['below'] as $menu_child) {
		$val = find_menu_item_children($menu_item_needle, $menu_child);
		
		if(isset($val)) {
		  return $val;
		}
	  }
	  
	  return NULL;
	}
	else {
	  return NULL;
	}
  }
}