<?php
  // $Id: ed_classified_delete.inc,v 1.1.4.3.4.2 2009/07/02 05:21:29 milesgillham Exp $
  /**
   * @file
   * Node deletion facilities.
   * Michael Curry, Exodus Development, Inc.
   * exodusdev@gmail.com
   * for more information, please visit http://exodusdev.com
   * Copyright (c) 2006, 2007 Exodus Development, Inc.  All Rights Reserved. 
   * Licensed under the terms of the GNU Public License (GPL) version 2.  Please see LICENSE.txt for
   * license terms.  Posession and use of this code signifies acceptance of license
   * terms.
   */


/**
 * Unpublish a node
 * (Partially) lifted from spam module (jeremy [at] kerneltrap.org)
 */

function _ed_classified_unpublish_ad($nid) {
  $result = db_query('UPDATE {node} SET status = 0 WHERE nid = %d', $nid);
  if (_ed_classified_variable_get('log_unpublish', FALSE)) {
    _edi_wd(t('Unpublished ad nid=%nid', array('%nid' => $nid)), WATCHDOG_NOTICE, l(t('View ad'), "node/$nid"));
  }
}

/**
 * User entry point to purge old, expired classified ads
 */

function _ed_classified_user_purge() {
  global $user;
  $purge_endtime = _ed_classified_purge();
  drupal_set_message('Purged ads that expired before '. format_date($purge_endtime));
  return ed_classified_admin_overview($user->uid);
}

/**
 * Purge old, expired classified ads
 */

function _ed_classified_purge() {
  /* 
   * Purge ads that have expired anytime between the start of time, and nn days ago
   * TODO: make this a manual process available under the admin/ed_classified menu, with confirmation
   */
  $purge_starttime=0; // is there a better way to do this?
  $purge_endtime=time() -  _ed_classified_days_to_seconds(_ed_classified_variable_get('ad_expired_purge_age', EDI_CLASSIFIED_VAR_DEF_PURGE_AGE));
  _ed_classified_purge_ads($purge_starttime, $purge_endtime);
  // report # of ads purged?
  // echo 'Purged ads that expired before '.format_date($purge_endtime);
  return $purge_endtime;
}


/**
 * purge old ads (must have been expired - IOW, unpublished - first.)
 */

function _ed_classified_purge_ads($time_start, $time_end) {
  $query = _ed_classified_get_aged_ads($time_start, $time_end, 0); // get unpublished ads whose time has come
  $count = 0;
  while ($node = db_fetch_object($query)) {
    _ed_classified_purge_ad($node);
    $count++;
  }
  if ($count>0) {
    _edi_wd(sprintf(t('Purged %d ads expired between %s and %s'), $count, _edi_safe_date_fmt($time_start), _edi_safe_date_fmt($time_end)));
  }
}

/**
 * "purge" an expired ad 
 * (This will delete a classified ad that has been 'expired' and is older than the threshold)
 */
function _ed_classified_purge_ad($node) {
  if (_ed_classified_variable_get('log_deletions', FALSE)) {
    $expired = _ed_classified_get_ending_date_string($node->expires_on);
    _edi_wd(t('Purging ad @title (nid=!nid) which @expired', array('@title' => $node->title, '!nid' => $node->nid, '@expired' => $expired)));
  }
  _ed_classified_delete($node->nid);
}


/**
 * Reimplement the node_delete code, because the code in node_delete
 * performs an access check for node deletion rights
 * This can be made more efficient, if needed.
 */

function _ed_classified_delete($nid) {
  $node = node_load($nid);
  // sanity check: only delete if:
  // Node found, nid != 0, node is unpublished (stats == 0) and it is truly a classified ad.
  if ($node && $node->nid != 0 && 0 == $node->status && _ed_classified_node_is_classified($node)) {
    db_query('DELETE FROM {node} WHERE nid = %d', $node->nid);
    db_query('DELETE FROM {node_revisions} WHERE nid = %d', $node->nid);

    // Call the node-specific callback (if any):
    node_invoke($node, 'delete');
    node_invoke_nodeapi($node, 'delete');

    // Remove this node from the search index if needed.
    if (function_exists('search_wipe')) {
      search_wipe($node->nid, 'node');
    }
  }
}
