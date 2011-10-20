<?php
  // $Id: ed_classified_utils.inc,v 1.1.2.2.4.19 2009/08/06 10:47:37 milesgillham Exp $

  /**
   * @file
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
 * config variable helpers
 */

function _ed_classified_cfg_varname($name) {
  // ensure unique varnames for this module
  return EDI_CLASSIFIED_MODULE_NAME .'_'. $name;
}

function _ed_classified_variable_get($name, $defval, $log=FALSE) {
  $val = variable_get(_ed_classified_cfg_varname($name), $defval);
  if ($log) {
    _edi_wd(sprintf('variable_get \'%s\'= \'%s\'',
                    _ed_classified_cfg_varname($name),
                    $val));
  }
  return $val;
}

function _ed_classified_variable_set($name, $newval, $log=FALSE) {
  if ($log) {
    _edi_wd(sprintf('variable_set \'%s\' changed from \'%s\' to \'%s\'',
                    _ed_classified_cfg_varname($name),
                    _ed_classified_variable_get($name, ''),
                    $newval));
  }
  variable_set(_ed_classified_cfg_varname($name), $newval);
}

/**
 * Convert db_query results to an array
 */ 
function _ed_classified_query_results_to_array($qr) {
  $arr = array();
  while ($info = db_fetch_object($qr)) {
    $arr[] = $info;
  }
  return $arr;
}

/**
 * Helper function to determine the single ad category name to which this node belongs.
 */
function _ed_classified_get_primary_category($node) {
  // lifted from _nodeapi implementation - crude, need to find out if there's a better way to get the single term for this node.
  $terms = taxonomy_node_get_terms_by_vocabulary($node, _ed_classified_get_vid());
  $term = array_pop($terms);
  if ($term) {
    $vocabulary = taxonomy_vocabulary_load(_ed_classified_get_vid());
    if ($parents = taxonomy_get_parents_all($term->tid)) {
      return $parents[0];
    }
  }
  return 0;
}

/** 
 * convert days to # seconds
 */ 

function _ed_classified_days_to_seconds($days) {
  return $days * 86400; // 60 * 60 * 24 = 86400
}


/**
 * Is a node really a classified ad?
 */

function _ed_classified_node_is_classified($node) {
  return EDI_CLASSIFIED_MODULE_NAME == $node->type;
}

/**
 * Is a tid a 'classified' term?  Is a tid a child of our taxonomy?
 * @param $tid The term id to check.
 * @return true if the term is a child of our vocabulary.
 */

function _ed_tid_is_classified_term($tid) { 
  $vid = _ed_classified_get_vid();
  $vocab = taxonomy_vocabulary_load($vid);
  $term = taxonomy_get_term($tid);
  if ($vocab->hierarchy && $vocab->module == EDI_CLASSIFIED_MODULE_NAME) {
    return $term->vid == $vid;
  }
  else {
    //
    // TODO: warn user about problems in a non-log-filling way.
    // _edi_wd(t('Internal error: Vocabulary !vid (@vocab_name) is not our dedicated classified ads vocabulary - someone has tampered with the vocabulary, or our stored variables.', array('!vid' => $vid, '@vocab_name' => $vocab->name)), WATCHDOG_ERROR);
    return FALSE; // sanity check: not possible since the vocab is not hierarchical or is not our vocabulary
  }
}

/**
 * Get default ad duration in seconds
 */

function _ed_classified_get_default_ad_duration_in_seconds() {
  $duration =  _ed_classified_days_to_seconds(_ed_classified_variable_get('default_ad_duration', EDI_CLASSIFIED_VAR_DEF_EXPIRATION_DAYS));
  return $duration;
}

/**
 * Find the longest duration, in days, for a given set of terms
 * @param $terms, an array of vid/tid pairs with vid as the key
 * sub-arrays are allowed, for example:
 * Array = (
 *   [63]=>5,
 *   [10]=> Array ([0]=>234, [1]=>984)
 * )
 * equivalent to:
 * Array = (
 *   [63]=>5,
 *   [10]=>234,
 *   [10]=>984
 * )
 * @return the duration, in days, of the largest duration found.
 */
function _ed_classified_get_longest_duration($terms) {
  $duration = 0;
  if (!empty($terms)) {
    // Normally we expect an array of taxonomy terms
    if (is_array($terms)) {
      reset($terms);
      while (list($key, $val) = each($terms)) {
        if (is_array($val)) {
          reset($val);
          while (list($subkey, $subval) = each($val)) {
            $d = _ed_classified_get_duration(array($key => $subval));
            if ($d > $duration) $duration = $d;
          }
        }
        else {
          $d = _ed_classified_get_duration(array($key => $val));
          if ($d > $duration) $duration = $d;
        }
      }
    }
  }
  // If we couldn't find a duration then set to default rather than zero
  if (0 == $duration) {
    $duration = _ed_classified_get_default_ad_duration_in_seconds();
  }
  return $duration; 
}

/**
 * Get stats 
 */
function ed_classified_get_ad_stats() {
  return theme('ed_classified_ads_stats');
}

function ed_classified_get_adcount($only_published) {
  $status = '';
  if ($only_published) $status = 'status=1 AND';
  $qr = db_query("SELECT COUNT(nid) as c FROM {node} WHERE ". $status ." type='ed_classified';");
  $o = db_fetch_object($qr);
  return $o->c;
}
/**
 * Get ad count for time range
 */
function ed_classified_get_adcount_for_time_range($start_time, $end_time, $only_published) {
  $status = '';
  if ($only_published) $status = 'status=1 AND';
  $qr = db_query("SELECT COUNT(nid) as c FROM {node} WHERE ". $status ." type='ed_classified' AND created >= %d AND created <=%d;", $start_time, $end_time);
  $o= db_fetch_object($qr);
  return $o->c;
}

/**
 * Get a list of popular ads for block use 
 */
function ed_classified_get_popular_ads_list() {
  $output = '';
  $have_statistics = _ed_classified_module_exists('statistics');
  if ($have_statistics) {
    $q = db_query("SELECT n.nid, n.title, n.created, n.changed, s.totalcount as counter FROM {node_counter} s INNER JOIN {node} n ON s.nid = n.nid WHERE n.status = 1 AND n.type = '%s' ORDER BY s.totalcount DESC LIMIT %d;", EDI_CLASSIFIED_MODULE_NAME, _ed_classified_variable_get('block_popular_limit', 4));
    $output = theme('ed_classified_ads_block', _ed_classified_query_results_to_array($q), FALSE, TRUE, TRUE);
  }
  return $output;
}


/**
 * Get a list of latest ads for block use
 */
function ed_classified_get_latest_ads_list() {
  // Show recent nodes, but only if they haven't expired
  $q = db_query('SELECT n.title, n.nid, n.created AS timestamp, n.changed'
   .' FROM {node} AS n, {edi_classified_nodes} AS ec'
   .' WHERE n.type = \'ed_classified\' AND n.status = 1 AND n.nid = ec.nid AND n.vid = ec.vid AND ec.expires_on > '. time()
   .' ORDER BY created DESC LIMIT %d;', _ed_classified_variable_get('block_latest_limit', 4));

  return theme('ed_classified_ads_block', _ed_classified_query_results_to_array($q), TRUE, FALSE, TRUE );
}

/**
 * expire old ads 
 */

function _ed_classified_expire_ads($time) {
  // mark ads matching criteria.  Set  $node->status=>unpublished, 
  $query = _ed_classified_get_aged_ads(0, $time, 1); // get published but old ads 
  while ($node = db_fetch_object($query)) {
    _ed_classified_unpublish_ad($node->nid);
    _ed_classified_notify_user_of_ad_expiration($node);
  }
}



/** 
 * Get a query result with any ads that have expired prior to a given date, and having a specific publish status
 */
function _ed_classified_get_aged_ads($expire_time_start, $expire_time_end, $status=1) {
  return db_query("SELECT * FROM {node} n, {edi_classified_nodes} ec WHERE n.type='ed_classified' AND n.vid = ec.vid  AND n.status = %d AND (ec.expires_on >= %d AND ec.expires_on <= %d)", $status, $expire_time_start, $expire_time_end );
}


/** 
 * Get the expiration date of a classified ad node
 */
function ed_classified_get_ad_expiration($node) {
  $expires = 0;// TODO: need windows-compatible old date - use time()?
  $q = db_query("SELECT expires_on FROM {edi_classified_nodes} ecn WHERE ecn.nid = '%d' AND ecn.vid = '%d'", $node->nid, $node->vid);
  if ($q) {
    $expires = db_result($q);
  }
  return $expires;
}

/**
 * Get settings 'banner'
 */
function _ed_classified_settings_get_banner(&$form) {
  $name = EDI_CLASSIFIED_MODULE_NAME;
  $d = '<a target="_blank" href="https://www.paypal.com/cgi-bin/webscr?cmd=_xclick&business=exodus.paypal%40gmail.com&item_name='. $name .'&item_number= '. $name .'-donation&page_style=PayPal&no_shipping=1&return=http%3A%2F%2Fexodusdev.com%2Fproducts&cancel_return=http%3A%2F%2Fexodusdev.com%2Fproducts&no_note=1&tax=0&currency_code=USD&lc=US&bn=PP%2dDonationsBF&charset=UTF%2d8"><img src="http://www.paypal.com/en_US/i/btn/x-click-but7.gif" alt="Support ongoing development!" title="Support ongoing development!"></a>';
  $form['module_banner'] = array('#type' => 'markup', 
  '#value' => '<div style="border: solid 1px #eee; margin: .5em; padding: .5em;"><div style="float:right;">'. $d .'</div><strong>Module development by <a href="http://exodusdev.com">Exodus Development</a></strong>.<br/>');
  $form['module_id'] = array('#type' => 'markup', '#value' =>  EDI_CLASSIFIED_MODULE_VERSION .'<br/></div>');
}

/**
 * do the sanity-check dance.
 */
function _ed_classified_check_settings(&$form) {
  $vid = _ed_classified_get_vid(); 
  $taxonomy = taxonomy_get_tree($vid);
  $warning ="";
  if (empty($taxonomy)) {
    if (!empty($vid)) {
      $url = "admin/content/taxonomy/$vid/add/term";
      $url = l('taxonomy administration', $url);
      $warning =  t('<div class="error">It appears that you have not configured any vocabulary terms to be used with the Classified Ads module.  This module will not function correctly until you define vocabulary terms for vocabulary id !vid.  Please visit !url to add terms such as "For Sale" or "Wanted" (or other categories as appropriate for your needs).</div>', array('!url' => $url, '!vid' => $vid));
    }
    else {
      $url='http://exodusdev.com/drupal/modules/ed_classified.module';
      $url=l('the project homepage', $url);
      $warning = t('<div class="error">The classified ads vocabulary settings are not configured correctly.  This is mostly likely due to a failed installation or an administrator deleting the automatically configured taxonomy or other problems.  Please visit the online handbook page(s) at !url.</div>', array('!url' => $url));
    }
    $form['warning'] = array('#type' => 'markup', '#value' => $warning);
    return FALSE;
  }
  return TRUE;
}

/**
 * Get just the raw text describing expiration date
 */
function _ed_classified_get_ending_date_string($ad_expiration_date) {
  // TODO: get time string for how long ago expired
  $interval = $ad_expiration_date - time();
  $expired = FALSE;
  if ($interval < 0) {
    $interval = -$interval; $expired =TRUE;
  }
  $str = '';
  if ($ad_expiration_date != 0) {
    $str = sprintf(t('%s on %s (%s%s)'), 
                   $expired ? t('expired') : t('expires'), 
                   format_date($ad_expiration_date), format_interval($interval, 2),
                   $expired ? t(' ago') : '');
  }
  return $str;
}


/**
 * Helper function: does $expiration date expire "soon" (soon depends on who you ask)
 */
function _ed_classified_ad_expires_soon($expiration_date) {
  return ($expiration_date - time() > 0 && ($expiration_date - time()) < _ed_classified_days_to_seconds(3));  // TODO: use config var
}

/**
 * return true if ad has expired
 */
function _ed_classified_ad_expired_already($expiration_date) {
  return $expiration_date <= time();
}
/**
 * Returns (and possibly creates) a new vocabulary for classified
 * Lifted from image.module.
 * TODO: Create some built-in terms for the created taxonomy: Miscellaneous, Cars, Pets, etc. (look at craigslist for inspiration)
 */
function _ed_classified_get_vid() {
  $vid = _ed_classified_variable_get('vocabulary', '');
  if (empty($vid)) {
    // Check to see if classified ads vocabulary exists
    $vid = db_result(db_query("SELECT vid FROM {vocabulary} WHERE module='%s'", EDI_CLASSIFIED_MODULE_NAME)); 
    if (!$vid) {
      _edi_wd("Classified Ads vocabulary not found - creating taxonomy.");
      $vocabulary 
        = array('name' =>  _ed_classified_displayname() . t(' Category'),
                'description' => t('Vocabulary required by Classified Ads (ed_classified) module.  <strong>Warning:  You should not delete this vocabulary unless you intend to uninstall (or have already uninstalled) the Classified Ads module.</strong>'), 
                'multiple' => '0', 
                'required' => '1', 
                'hierarchy' => '1', 
                'relations' => '0', 
                'module' => EDI_CLASSIFIED_MODULE_NAME, 
                'nodes' => array(EDI_CLASSIFIED_MODULE_NAME => 1)); 
      taxonomy_save_vocabulary($vocabulary);
      $vid = $vocabulary['vid'];
      $msg = "Classified Ads taxonomy". $vocabulary->name ." (id=$vid) created.";
      _edi_wd($msg);
      drupal_set_message($msg);
      // TODO: create standard terms and subterms specified in external file (or, at a minimum, create a few standard categories)
      // taxonomy_save_term(...);
    }
    _ed_classified_variable_set('vocabulary', $vid);
  }

  return $vid;
}

/**
 * Make a 'path' to a category view for a given tid.
 */

function _ed_classified_make_category_path($tid) {
  return drupal_get_path_alias(EDI_CLASSIFIED_PATH_NAME .'/tid/'. $tid);
}

/**
 * Determine, in a version-friendly way, if a module exists.
 * some folks have decided to change module_exist() to module_exists() for 4.8/5.0.  see: http://drupal.org/node/79601
 */
function _ed_classified_module_exists($module) {
  $module_exists = FALSE;
  if (function_exists('module_exist'))
    $module_exists = module_exist($module); // 4.7
  if (function_exists('module_exists'))
    $module_exists = module_exists($module); // 4.8/5.0?
  return $module_exists;
}

/**
 * shortcut - log something to the watchdog log
 */
function _edi_wd($message, $severity = WATCHDOG_NOTICE, $link = NULL) {
  watchdog(EDI_CLASSIFIED_MODULE_NAME, $message, NULL, $severity, $link);
}

function _edi_safe_date_fmt($date) {
  return ($date ==0) ? "_the beginning of time_" : format_date($date);
}
/**
 * Determine whether a node is expired given a target timestamp
 * An ad is considered to have 'expired' if the $time parameter
 * is larger than the node's expires_on field.
 */

function ed_classified_ad_expired($node, $time) {
  if ($node->type != EDI_CLASSIFIED_MODULE_NAME)
    return FALSE;
  return ($time >= $node->expires_on);
}

/**
 * Helper gets content display name
 */

function _ed_classified_displayname() {
  $name = node_get_types('name', 'ed_classified');
  if (isset($name)) {
    return $name;
  }
  else {
    return 'undefined';
  }
}

function _ed_classified_displayname_parms() {
  $name = _ed_classified_displayname();
  $parms=array('@name' => $name, '!name' => $name, '%name' => $name );
  return $parms;
}

/** 
 * Make a node edit link
 */
function _ed_classified_make_edit_link($node, $text, $attributes=array()) {
  return l($text, "node/$node->nid/edit", array('attributes' => $attributes));
}

/**
 * Get duration in days for taxonomy term (array of tids, indexed by vid);
 * Expecting array of size 1, so any more is ignored
 * if nothing set, use the default value
 * TODO - if a single value, not an array, then assume the vid to be the
 * Classified Ads vid
 */
function _ed_classified_get_duration($term) {
  $duration=0;
  reset($term);
  list($vid, $tid) = each($term);
  $def = _ed_classified_variable_get('default_ad_duration', EDI_CLASSIFIED_VAR_DEF_EXPIRATION_DAYS);
  $name = 'vid_'. $vid .'_tid_'. $tid .'_duration';
  $duration = _ed_classified_variable_get($name, $def);
  return $duration;
}


// generic debug function
/**
function _dbg($thing, $heading=NULL) {
  drupal_set_message("<strong>$heading</strong><pre>".htmlspecialchars(print_r($thing, true)).'</pre>');
}

*/
