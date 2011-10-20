<?php
  // $Id: ed_classified_notifications.inc,v 1.1.4.3.4.8 2009/07/11 05:24:49 milesgillham Exp $
  /**
   * @file
   * user notifications for imple text-based classified ads module
   * Michael Curry, Exodus Development, Inc.
   * exodusdev@gmail.com
   * for more information, please visit http://exodusdev.com
   * Copyright (c) 2006, 2007 Exodus Development, Inc.  All Rights Reserved. 
   * Licensed under the terms of the GNU Public License (GPL) version 2.  Please see LICENSE.txt for
   * license terms.  Posession and use of this code signifies acceptance of license
   * terms.
   */
  
/**
 * Process notification email handling on cron run.
 * @param $time The timestamp to use for email processing.
 * This allows us to use a consistent timestamp value, 
 * and push in fake values for testing and diagnostic
 * purposes.
 */

function _ed_classified_process_notification_emails($time) {
  _ed_classified_notify_advertisers_periodic($time);
}

/**
 * Process "periodic" notifications
 * Create a notification if a user has ads nearing expiration
 */
function _ed_classified_notify_advertisers_periodic($time) {
  /*
   * Process notifications periodically but not every cron run
   * But, don't slam server -- only do (n) messages per cron run? (admin-defined?) (later)
   * When done, record ending timestamp of last completed notification run
   * So: if the time since the last completed notification run is > admin-defined limit
   *   - get list of users needing reminder mails
   *   - process (format and send) a batch of emails
   *   - if done (no more users remaining) record completion time
   */
  if (_ed_classified_periodic_notification_time($time)) {
    // get list of users having published ads nearing expiration
    $target_time = $time + _ed_classified_days_to_seconds(_ed_classified_variable_get('ad_expiration_email_warning_days', EDI_CLASSIFIED_VAR_DEF_AD_EXPIRATION_EMAIL_WARNING_DAYS)); 
    _edi_wd(t('Processing notification emails for ads expiring soon (between now (!now) and !date)', array('!now' => format_date($time), '!date' => format_date($target_time))));
    $result = db_query('SELECT DISTINCT({node}.uid) FROM {node} INNER JOIN {edi_classified_nodes} ON {edi_classified_nodes}.vid = {node}.vid WHERE ({node}.status = 1) AND ({edi_classified_nodes}.expires_on < %d)', $target_time); 
    if ($result) {
      while ($uid = db_result($result)) {
        // todo: need bailout based on # of users emailed, total time spent
        $user = user_load(array('uid' => $uid));
        // todo: need to send mails only to those with user_access('reset classified ad expiration') && user_access('edit own classified ads') permissions
        if ($user) {
          if (!_ed_classified_send_user_notification_email($user, 'expiring')) {
            _edi_wd(t('Unable to send ad expiration reminder email to user #!uid', array('!uid' => $uid)), WATCHDOG_ERROR);
          }
        }
        else {
          _edi_wd(t('Unable to load user !uid', array('!uid' => $uid)), WATCHDOG_ERROR);
        }

      }
    } 
    //
    // now record the fact that we completed processing notifications, and when
    _ed_classified_record_periodic_notifications(time());
  } // time to notify
}

/**
* Notify a user that their ad has expired.
* @param $node The ad node.
* This function sends a note to the user regarding their expired ad.
*/
function _ed_classified_notify_user_of_ad_expiration($node) {
  $ad_author = user_load(array('uid' => $node->uid));
  _ed_classified_send_user_notification_email($ad_author, 'expired');
}

/**
* Send an email notification to the specified user
*/

function _ed_classified_send_user_notification_email(&$user, $key='') {
  $parms = _ed_classified_displayname_parms();
  $params['account'] = $user;
  $params['context'] = array('!sitename' => variable_get('site_name', ''),
                 '!user_ads_url' => url('user/'. $user->uid .'/'. EDI_CLASSIFIED_PATH_NAME, array('absolute' => TRUE)),
                 '!siteurl' => url('', array('absolute' => TRUE)));
  $params['context'] = array_merge($params['context'], $parms);
  $from = variable_get('site_mail', ini_get('sendmail_from')); // http://drupal.org/node/77689
  return drupal_mail(EDI_CLASSIFIED_MODULE_NAME, $key, $user->mail, user_preferred_language($user), $params, $from);
}

/**
 * Record last periodic notification processing time
 */
function _ed_classified_record_periodic_notifications($time) {
  _ed_classified_variable_set('email_reminders_last_sent', $time);
}
/**
 * Return TRUE if we need to send periodic notifications according to time and configuration options
 * Only send notifications if sufficient time has passed since last notification run completed.
 */
function _ed_classified_periodic_notification_time($time) {
  $last_notify_time = _ed_classified_variable_get('email_reminders_last_sent', 0);
  // only update notifications if haven't been notified in over 24 hours
  $next_notify_time = $last_notify_time + _ed_classified_variable_get('email_reminder_period_secs', 86400);
  return ($time > $next_notify_time);
}

