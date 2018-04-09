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
    $menu_items = menu_tree_all_data($menu_name, NULL, 5);;
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
	  if(isset($menu_item['link']['hidden'])) {
	    if($menu_item['link']['hidden'] == '1') {
		  continue;
		}
	  }
      $item_name = str_replace(' ', '-', strtolower($menu_item['link']['link_title']));
      $link_attributes = isset($menu_item['link']['options']['attributes']) ? 
        $menu_item['link']['options']['attributes'] : array();
      $item_attributes = isset($menu_item['link']['options']['item_attributes']) ? 
        $menu_item['link']['options']['item_attributes'] : array();
      
	  // Default variable declarations
	  $style = 'standard';
	  $nolink = 0;
	  $fid = 0;
	  $adv_content = '';
	  
	  if(isset($menu_settings[$menu_item['link']['mlid']])) {
		$mlid = $menu_item['link']['mlid'];
	    $style = $menu_settings[$mlid]['style'];
        $nolink = $menu_settings[$mlid]['style_settings']['nolink'];
        $fid = $menu_settings[$mlid]['style_settings']['wide_setting'];
        $adv_content = $menu_settings[$mlid]['style_settings']['advanced_setting']['value']; 
	  }
	  else if(isset($menu_settings[$item_name])){
		$style = $menu_settings[$item_name]['style'];
        $nolink = $menu_settings[$item_name]['style_settings']['nolink'];
        $fid = $menu_settings[$item_name]['style_settings']['wide_setting'];
        $adv_content = $menu_settings[$item_name]['style_settings']['advanced_setting']['value'];  
	  }
      $children = better_mm_get_children($menu_item);
      $l1_class = better_mm_get_li_class($key, $menu_items);
      $child_class = better_mm_get_child_class($children, $style);
      $child_class_mob = better_mm_get_child_class($children, $style, TRUE);
      $item_classes = $item_attributes['class'];
      $class_list = $item_classes . $l1_class . $child_class;
      $class_list_mob = $item_classes . $l1_class . $child_class_mob;
      $item_id = $item_attributes['id'];
            
      if ($menu_item['link']['link_path'] == '<front>') {
        $url = url('<front>');
      } elseif (preg_match('/^http/', $menu_item['link']['link_path']) === 1) {
        $url = $menu_item['link']['link_path'];
      } else {
        $url = '/' . drupal_get_path_alias($menu_item['link']['link_path']);
      }
      
      // If a file has been uploaded, set file status to permanent.
      if (is_numeric($fid) && $fid != 0) {
        $mlid = $menu_item['link']['mlid'];
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
        $html .= l($menu_item['link']['link_title'], $url, array('attributes' => $link_attributes));
        $mobile_html .= l($menu_item['link']['link_title'], $url, array('attributes' => $link_attributes));
      }
      
      if (!$nolink) {
        $html .= l($menu_item['link']['link_title'], $url, array('attributes' => $link_attributes));
        $mobile_html .= l($menu_item['link']['link_title'], $url, array('attributes' => $link_attributes));
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
  if($menu_item_needle['link']['mlid'] == 
  $menu_item_haystack['link']['mlid']) {
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

/*
 * Function that saves or deletes a menu item from the BMM form settings
 * database variable.
 *
 * @param $itemName
 *   The name of the menu item to be added or deleted (also called title).
 *
 * @param $mlid
 *   The menu link id (mlid) of the menu link to be added or deleted.
 *
 * @param $delete
 *   Optional variable that indicates where to add or delete the menu item.
 *   The default option is FALSE, which means the function will add the menu
 *   item. Set to TRUE if you wish for the item specified to be deleted.
 */
function saveToBMMItemSettings($itemName, $mlid, $delete = FALSE) {
  $currentSettings = variable_get('bmm_menu_items_settings', '');
  $itemName = strtolower($itemName);
  
  // If delete is false, add the menu item to the form settings with default
  // settings.
  if(!$delete) {
	if(!isset($currentSettings[$mlid])) {
      $currentSettings[$mlid] = array('name' => $itemName, 
        'style' => 'standard',
	    'style_settings' => array(
	      'nolink' => 0,
	      'wide_setting' => 0,
	      'advanced_setting' => array(
	        'value' => '',
	  	  'format' => 'full_html',
	      ),
	    ),
      );
	}
  }
  // Otherwise, remove the menu item based on the mlid, or title.
  else {
	if(isset($currentSettings[$mlid])) {
	  unset($currentSettings[$mlid]);
	}
	else {
	  unset($currentSettings[$itemName]);
	}
  }
  
  // Set the BMM menu items settings variable to the new form settings
  variable_set('bmm_menu_items_settings', $currentSettings);
}
