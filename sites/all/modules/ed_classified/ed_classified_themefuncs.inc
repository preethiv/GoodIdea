<?php
  // $Id: ed_classified_themefuncs.inc,v 1.1.2.1.4.9 2009/08/06 10:47:37 milesgillham Exp $

  /**
   $ @file
   * Simple text-based classified ads module.
   * Michael Curry, Exodus Development, Inc.
   * exodusdev@gmail.com
   * for more information, please visit http://exodusdev.com/drupal/modules/classified.module
   * Copyright (c) 2006, 2007 Exodus Development, Inc.  All Rights Reserved. 
   * Licensed under the terms of the GNU Public License (GPL) version 2.  Please see LICENSE.txt for
   * license terms.  Possession and use of this code signifies acceptance of license
   * terms.
   */


/**
 * Drupal 6 hook_theme() registration function
 */

function ed_classified_theme() {
  return array(
       'ed_classified_body' => array('arguments' => array('node')),
       'ed_classified_teaser' => array('arguments' => array('node')),
       'ed_classified_ending_date' => array('arguments' => array('ad_expiration_date')),
       'ed_classified_ads_block' => array('arguments' => array('ads', 'display_timestamp' => TRUE, 'display_counter' => FALSE, 'display_ad_category' => FALSE)),
       'ed_classified_taxonomy' => array('arguments' => array('cats' => NULL, 'ads' => NULL)),
       'ed_classified_adcount' => array('arguments' => array('count' => NULL)),
       'ed_classified_taxonomy_catlist' => array('arguments' => array('cats' => NULL)),
       'ed_classified_category_list_row' => array('arguments' => array('cat' => NULL, 'row_count' => NULL)),
       'ed_classified_taxonomy_ads' => array('arguments' => array('ads' => NULL)),
       'ed_classified_category_list_ad_row' => array('arguments' => array('ad' => NULL, 'rowcount' => NULL)),
       'ed_classified_category_name' => array('arguments' => array('cat' => NULL)),
       'ed_classified_category_description' => array('arguments' => array('cat' => NULL)),
       'ed_classified_ads_stats' => array(),             
  );
}

/** 
 * Theme an ads block
 * 
 */
function theme_ed_classified_ads_block($ads, $display_timestamp=TRUE, $display_counter=FALSE, $display_ad_category=FALSE) {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');
  //    var_dump($ads); 
  $content = '';
  foreach ($ads as $x) {
    $content .= '<li class="classified-ad-item">'. l($x->title, drupal_get_path_alias('node/'. $x->nid), array('attributes' => array('title' => check_markup($x->teaser)))); 
    if ($display_ad_category) {
      $term = _ed_classified_get_primary_category($x);
      if ($term) {
        $content .= ' <span class=\'classified-block-ad-term\'>'. l("($term->name)", _ed_classified_make_category_path($term->tid)) .'</span>'; // TODO: themeable function
      }
    }
    if ($display_timestamp) {
      $content .= ' <span class=\'classified-block-ad-age\'>('. format_interval(time() - $x->timestamp, 2) . t(' ago') .') </span>';
    }
    if ($display_counter) {
      $content .= ' <span class=\'classified-block-ad-count\'>('. $x->counter .') </span>';
    }
    if (node_last_viewed($x->nid) < $x->changed) 
      $content .= theme_mark();
    $content .= '</li>';
  }
  if (!empty($content)) {
    $content = '<div class="classified-ad-block item-list"><ul>'. $content;
    $content .= '</ul></div>';
  }
  return $content;
}

/**
 * Theme a classified ads taxonomy browser page
 */

function theme_ed_classified_taxonomy($cats, $ads) {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');

  $content = '';
  if (user_access('create classified ads')) {
    $content .= '<div class="classified-category-link-add">'. l(t('Create a new ad'), drupal_get_path_alias("node/add/". EDI_CLASSIFIED_PATH_NAME)) ."</div>\n";
  }

  if (count($cats)) {
    $content .= theme('ed_classified_taxonomy_catlist', $cats);
  }

  if (count($ads)) {
    $content .= theme('ed_classified_taxonomy_ads', $ads);
  }

  if ($pager = theme('pager', NULL, _ed_classified_variable_get('ads_per_page', 10), 0)) {
    $content .= $pager;
  }

  if (count($ads) + count($cats) == 0) {
    $content .= theme('ed_classified_category_adcount', 0);
  }
  
  return $content;
}

function theme_ed_classified_category_adcount($count) {
  return '<p class="classified-count">'. format_plural($count, 'There is @count ad in this category', 'There are @count ads in this category') ."</p>\n";
}

function theme_ed_classified_taxonomy_catlist($cats) {
    $row_count = 0;
    $content .= '<ul class="classified-category-list">';
    foreach ($cats as $cat) {
      $content .= theme('ed_classified_category_list_row', $cat, $row_count);
      $row_count ++;
    }
    $content .= "</ul>\n";
    return $content;
}

function theme_ed_classified_category_list_row($cat, $row_count) {
  $content = '';
  $row_style = ($row_count % 2 == 0) ? 'even' : 'odd';
  $content .= "<li class ='$row_style' >";
  // 'preview' (was thumbnail of latest image)
  //      if ($cat->count)
  //        $content.= l('foo', _ed_classified_make_category_path($cat->tid), array('html' => TRUE));
  $content .= theme('ed_classified_category_name', $cat); 
  // Removed the description from below the name, instead it is integrated with the name
  // (i.e. it appears as a title in there)
  //if(trim($cat->description) != '') {
  //  $content .= theme('ed_classified_category_description', $cat);
  //}
  // TODO: get preview list 
  if ($cat->latest) {
    $content .= '<div class=\'title\'>'. t('Latest ad: ') . l($cat->latest->title, drupal_get_path_alias('node/'. $cat->latest->nid)) .'</div>';
    $content .= '<div class=\'created\'>'. t('Created on ') . format_date($cat->latest->created) .' ('. format_interval(time() - $cat->latest->created) . t(' ago') .')</div>';
  }
  $content .= '<div class="status">';

  $content .= theme('ed_classified_category_adcount', $cat->count); 
  if ($cat->latest->changed) {
    $content .= t('  Last updated: !date', array('!date' => format_date($cat->latest->changed)));
  }

  $content .="</div>\n"; // class="status"
  $content .= "</li>\n";
  return $content;
}

function theme_ed_classified_taxonomy_ads($ads) {
    $content .= '<ul class="classified-ad-list">';
    $rowcount = 0;
    foreach ($ads as $ad) {
      $content .= theme('ed_classified_category_list_ad_row', $ad, $rowcount);
      $rowcount++;
    }
    $content .= "</ul>\n";
    return $content;
}

function theme_ed_classified_category_list_ad_row($ad, $rowcount) {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');
  $content .= '<li';
  if ($ad->sticky) {
    $content .= ' class="sticky"';
  }
  $content .= ">\n";
  $content .= '<h3>'. l($ad->title, drupal_get_path_alias('node/'. $ad->nid),
    array('attributes' => array('title' => 'Description here!'))) ."</h3>";
  if (_ed_classified_variable_get('show_body_in_ad_list', EDI_CLASSIFIED_VAR_DEF_SHOW_BODY_IN_AD_LIST)) {
    $content .= '<div class=\'classified-description\'>'. check_markup($ad->body, $ad->format, FALSE) .'</div>';
    if (theme_get_setting('toggle_node_info_'. $ad->type)) {
      $content .= '<div class="classified-author">'. t('Posted by: !name', array('!name' => theme('username', $ad))) ."</div>\n";
      if ($ad->created > 0) {
        $content .= '<div class="classified-date">'. t('Created on ') . format_date($ad->created) ."</div>\n";
      }
      $expires = ed_classified_get_ad_expiration($ad);
      if ($expires > 0) {
        $content .= '<div class="classified-date">'. theme('ed_classified_ending_date', $expires) ."</div>\n";
      }
    }
  }
  $content .= "</li>\n";
  return $content;
}

function theme_ed_classified_category_name($cat) {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');
  return '<div class=\'classified-cat-name\'>'. l($cat->name, _ed_classified_make_category_path($cat->tid),
    array('attributes' => array('title' => $cat->description))) ."</div>\n";
}

function theme_ed_classified_category_description($cat) {
  return '<div class="classified-description">'. check_plain($cat->description) ."</div>\n";
}

function theme_ed_classified_ads_stats() {
  $content = '';
  $today = ed_classified_get_adcount_for_time_range(time()-86400, time(), TRUE); // 86400 = # seconds in a day
  $total = ed_classified_get_adcount(TRUE);
  if ($today >0 || $total > 0) {
    $stats =  array('!adcount' => $total, '!ads_today' => $today);
    $content = '<div class="classified-ad-block item-list"><ul>';
    $content .= '<li>'. t('!adcount ads total.', $stats) .'</li>'; // todo: format_plural() http://api.drupal.org/api/HEAD/function/format_plural
    $content .= '<li>'. t('!ads_today ads in last 24 hours.', $stats) .'</li>'; // todo: format_plural()
    $content .= '</ul></div>';
  }
  return $content;
}

function theme_ed_classified_teaser($node) {
  return '<div class=\'classified-teaser\'>'. check_markup($node->teaser, $node->format, FALSE) .'</div><br/>'. theme('ed_classified_ending_date', $node->expires_on);
}

function theme_ed_classified_body($node) {
  return '<div class=\'classified-body\'>'. check_markup($node->body, $node->format, FALSE) .'</div><br/>'. theme('ed_classified_ending_date', $node->expires_on);
}

/**
 * Get a formatted div with a readable, friendly ad expiration date.
 */
function theme_ed_classified_ending_date($ad_expiration_date) {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');
  $class = 'classified-expiration-info'; // standard expiration formatting
  $str = _ed_classified_get_ending_date_string($ad_expiration_date);
  // set style for ending soon
  if (_ed_classified_ad_expires_soon($ad_expiration_date)) {
    $class = 'classified-expiration-expires_soon'; // TODO: crappy name, fix this
  }
  if (_ed_classified_ad_expired_already($ad_expiration_date)) {
    $class = 'classified-expiration-expired'; // TODO: crappy name, fix this
  }
  return "<span class='$class'>". $str .'</span>';
}

