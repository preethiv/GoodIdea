<?php 


/**
 * This adds a variable to views slideshow
 *  **NO LONGER USED, REMOVE THIS
 * @param unknown_type $vars
 */
function phptemplate_preprocess_views_slideshow_ddblock_cycle_block_content(&$vars) {
  $vars['fivestar_rating'] = "Fivestar Rating";
  $vars['node_type'] = "Node Type"; 
}

/**
* This function makes changes to the theme search box
*
*/
function townsource_preprocess_search_theme_form(&$vars, $hook) {
    // Modify elements of the search form
    //print_r($vars['form']['search_theme_form']);
    $vars['form']['search_theme_form']['#size'] = 50;
    unset($vars['form']['search_theme_form']['#title']);

    // Set a default value for the search box
    $vars['form']['search_theme_form']['#value'] = t('Search Davis Source!');
   
    // Add a custom class to the search box
    $vars['form']['search_theme_form']['#attributes'] = array(
       'onblur' => "if (this.value == '') {this.value = '".$vars['form']['search_theme_form']['#value']."';} this.style.color = '#888888';",
         'onfocus' => "if (this.value == '".$vars['form']['search_theme_form']['#value']."') {this.value = '';} this.style.color = '#000000';" );

    // Modify elements of the submit button
    unset($vars['form']['submit']);

    // Change text on the submit button
    //$vars['form']['submit']['#value'] = t('Go!');

    // Change submit button into image button - NOTE: '#src' leading '/' automatically added
    $vars['form']['submit']['image_button'] = array('#type' => 'image_button', '#src' => 'sites/all/themes/townsource/images/searchButton.png');

    // Rebuild the rendered version (search form only, rest remains unchanged)
    unset($vars['form']['search_theme_form']['#printed']);
    $vars['search']['search_theme_form'] = drupal_render($vars['form']['search_theme_form']);

    // Rebuild the rendered version (submit button, rest remains unchanged)
    unset($vars['form']['submit']['#printed']);
    $vars['search']['submit'] = drupal_render($vars['form']['submit']);
   
    // Collect all form elements to print entire form
    $vars['search_form'] = implode($vars['search']);
    //print_r($vars['form']['search_theme_form']);
}
