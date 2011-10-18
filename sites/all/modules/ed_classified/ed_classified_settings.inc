<?php
  // $Id: ed_classified_settings.inc,v 1.1.2.1.4.7 2009/08/06 10:47:37 milesgillham Exp $

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
 * Implementation of hook_settings().
 * TODO - this is hardcoded to the Classified Ads vid - needs to be able to handle
 * any taxonomy assigned
 */
function ed_classified_admin_settings() {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');

  // lifted from image.module - neat trick
  $form[_ed_classified_cfg_varname('settings_updated')] = array('#type' => 'hidden', '#value' => time());

  // Miles - I have disabled this temporarily to keep the form neat and in line with
  // Drupal guidelines.
  // _ed_classified_settings_get_banner($form);
  if (!_ed_classified_check_settings($form)) {
    // bail, system not configured correctly.
    return $form;
  }
    
  $vid = _ed_classified_get_vid(); 
  $taxonomy = taxonomy_get_tree($vid);

  $form['expiration'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Ad Duration and Expiration'),
  );
  $form['expiration'][ _ed_classified_cfg_varname('default_ad_duration')] = array('#type' => 'textfield', '#title' => t('New ad default duration (days)'), '#default_value' => _ed_classified_variable_get('default_ad_duration', EDI_CLASSIFIED_VAR_DEF_EXPIRATION_DAYS), '#size' => '3', '#maxlength' => 3, '#description' => t('Ads will auto-expire this many days after the ad is created, unless you override this on a per-category basis (see below.).  Requires cron.'), '#required' => TRUE);

  $form['expiration'][ _ed_classified_cfg_varname('ad_expired_purge_age')] = array('#type' => 'textfield', '#title' => t('Expired ad deletion grace period'), '#default_value' => _ed_classified_variable_get('ad_expired_purge_age', EDI_CLASSIFIED_VAR_DEF_PURGE_AGE), '#size' => '3', '#maxlength' => 3, '#description' => t('Expired ads are subject to purging (deletion) this many days after expiration.'), '#required' => TRUE);


$form['expiration']['notifications'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Ad Expiration Notifications'),
  );
  $form['expiration']['notifications'][ _ed_classified_cfg_varname('send_email_reminders')] = array('#type' => 'checkbox', '#title' => t('Send email reminders'), '#default_value' => _ed_classified_variable_get('send_email_reminders', EDI_CLASSIFIED_VAR_DEF_SEND_EMAIL_REMINDERS), '#description' => t('Send reminder emails to advertisers (using the specified templates) periodically if they have ads expiring within the warning period defined below.'), '#required' => FALSE);
  $form['expiration']['notifications'][ _ed_classified_cfg_varname('ad_expiration_email_warning_days')] = array('#type' => 'textfield', '#title' => t('Days before expiration to send reminder emails'), '#default_value' => _ed_classified_variable_get('ad_expiration_email_warning_days', EDI_CLASSIFIED_VAR_DEF_AD_EXPIRATION_EMAIL_WARNING_DAYS), '#size' => '3', '#maxlength' => 3, '#required' => TRUE);
  $form['expiration']['notifications']['email']
    = array('#type' => 'fieldset',
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#title' => t('Notification messages'),
            '#description' => t('Substitution variables: !sitename = your website name, !siteurl = your site\'s base URL, !user_ads_url = link to user\'s classified ads list.')
  );
  
  $form['expiration']['notifications']['email'][ _ed_classified_cfg_varname('email_subj') ] 
    = array('#type' => 'textarea', '#default_value' => t(_ed_classified_variable_get('email_subj', EDI_CLASSIFIED_VAR_DEF_EMAIL_SUBJ)),
            '#title' => t('Expiration reminder email subject'));
  $form['expiration']['notifications']['email'][ _ed_classified_cfg_varname('email_body') ] 
    = array('#type' => 'textarea', '#default_value' => t(_ed_classified_variable_get('email_body', EDI_CLASSIFIED_VAR_DEF_EMAIL_BODY)),
            '#title' => t('Expiration reminder email body'));

$form['content_policy'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Ad Content Policy'),
  );
  $form['content_policy'][ _ed_classified_cfg_varname('ad_standard_body_length')] = array('#type' => 'textfield', '#title' => t('Body length limit for standard ads'), '#default_value' => _ed_classified_variable_get('ad_standard_body_length', EDI_CLASSIFIED_VAR_DEF_BODYLEN_LIMIT), '#size' => '6', '#maxlength' => 5, '#description' => t('Maximum characters to allow in classified ad body.'), '#required' => TRUE);

  $form['content_policy'][ _ed_classified_cfg_varname('show_contact_form_link_on_posts')] = array('#type' => 'checkbox', '#title' => t('Show contact link on classified ad posts'), '#default_value' => _ed_classified_variable_get('show_contact_form_link_on_posts', EDI_CLASSIFIED_VAR_DEF_SHOW_CONTACT_LINK_ON_POSTS), '#description' => t('Provide contact link in classified ad links.  Not necessary if you provide an alternative method of contacting advertisers, such as the privatemsg module).'), '#required' => FALSE);

$form['display'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Ad Display'),
  );
  $form['display'][ _ed_classified_cfg_varname('show_body_in_ad_list')] = array('#type' => 'checkbox', '#title' => t('Show ad body in taxonomy lists.'), '#default_value' => _ed_classified_variable_get('show_body_in_ad_list', EDI_CLASSIFIED_VAR_DEF_SHOW_BODY_IN_AD_LIST), '#description' => t('Show full ad body in taxonomy lists (otherwise will just show teaser - curently the title).'), '#required' => FALSE);

$form['file_attachment_enhancements'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('File Attachment Enhancements'),
  );

  $form['file_attachment_enhancements'][ _ed_classified_cfg_varname('alter_attachment_text')] = array('#type' => 'checkbox', '#title' => t('Alter classified ad create/edit form file attachment text'), '#default_value' => _ed_classified_variable_get('alter_attachment_text', EDI_CLASSIFIED_VAR_DEF_ALTER_ATTACHMENT_TEXT), '#description' => t('If selected, this option will enhance classified node edit forms where attachments are enabled - altering form behavior to help users with attaching an image.'), '#required' => FALSE);

  $form['file_attachment_enhancements'][ _ed_classified_cfg_varname('alter_attachment_text_description')] = array('#type' => 'textarea', '#title' => t('File attachment description text'), '#default_value' => _ed_classified_variable_get('alter_attachment_text_description', t(EDI_CLASSIFIED_VAR_DEF_ALTER_ATTACHMENT_TEXT_DESCRIPTION)), '#description' => t('If you choose to alter the file attachment form behavior, this text will be shown.'), '#required' => TRUE);

  $form['taxonomy_settings'] = 
    array('#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          '#title' => t('Taxonomy-dependent settings'),
            '#description' => t('Ad duration, in days, for the ads created in each category.  Ads expire this many days after creation in this category - note that changing the category after creation will not change the expiration date unless the author resets the expiration.  Ads assigned more than one category at the time of creation or editing receive the maximum duration found in all assigned categories. '),
          );

  // get terms
  //  _dbg($taxonomy);
  foreach ($taxonomy as $t) {
    $name = _ed_classified_cfg_varname('vid_'. $vid .'_tid_'. $t->tid .'_duration');
    $parms =  array('@name' => $t->name, '!tid' => $t->tid);
    $form['taxonomy_settings'][$name] = 
      array('#type' => 'textfield',
            '#size' => 10,
            '#maxlength' => 4, 
            '#title' => t('Ad duration for Category \'@name\'', $parms),
            '#default_value' => _ed_classified_get_duration(array($vid => $t->tid)),
            '#prefix' => '<div style="margin: '. $t->depth * 2 .'em; margin-top: 0; margin-bottom: 0;">',
            '#suffix' => '</div>',
            );
  }

  return system_settings_form($form);
}
