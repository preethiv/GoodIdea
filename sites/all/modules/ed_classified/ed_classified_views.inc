<?php
  // $Id: ed_classified_views.inc,v 1.1.2.1.4.4 2009/08/06 10:47:37 milesgillham Exp $

  /**
   * @file
   * Simple text-based classified ads module.
   * Autho: Michael Curry, Exodus Development, Inc.  * exodusdev@gmail.com
   * for more information, please visit http://exodusdev.com/drupal/modules/classified.module
   */


/* ================= views.module integration ===================== */

function ed_classified_views_handlers() {
  $handlers = array(
    'handlers' => array(
      'ed_classified_views_time_remaining_handler_field' => array(
        'parent' => 'views_handler_field_date'
      )
    )
  );
  return $handlers;
}

/*
 * Implements hook_views_data()
 * No need to make it a base, doesn't have sufficient first class data
 * as yet, really just dependent upon Node.
 */

function ed_classified_views_data() {
  module_load_include('inc', 'ed_classified', 'ed_classified_utils');

  $parms = _ed_classified_displayname_parms();
  $parms['!modulename'] = EDI_CLASSIFIED_MODULE_NAME;
  $data = array();
  // All classified ads are in a single table group
  $data['edi_classified_nodes']['table']['group'] = t('Classified Ads');
  // Everything else is in a Node
  $data['edi_classified_nodes']['table']['join'] = array(
    // Tell Views that we can join with node (on vid just to get current
    // revision.)
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid'
    )
  );
  // 'expires_on'
  $data['edi_classified_nodes']['expires_on'] = array(
    'title' => t('Expires On'),
    // The help that appears on the UI
    'help' => t('Classified ad expiry date.'),
    // Information for displaying
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE
    ),
    // Information for accepting as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_date',
      'name field' => 'title',
      'numeric' => TRUE,
    ),
    // Information for accepting as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_date'
    ),
    // Information for sorting
    'sort' => array(
      'handler' => 'views_handler_sort_date'
    )
  );
  // 'expiry_remaining' time remaining is an alias of 'expires_on'
  // but displayed differently
  $data['edi_classified_nodes']['expiry_remaining'] = array(
    'real field' => 'expires_on',
    'title' => t('Expiry time remaining'),
    // The help that appears on the UI
    'help' => t('Time remaining until expiry.'),
    // Information for displaying
    'field' => array(
      'handler' => 'ed_classified_views_time_remaining_handler_field',
      'click sortable' => TRUE
    ),
    // Information for accepting as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_date',
      'name field' => 'title',
      'numeric' => TRUE,
    ),
    // Information for accepting as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_date'
    ),
    // Information for sorting
    'sort' => array(
      'handler' => 'views_handler_sort_date'
    )
  );
  return $data;
}


/* ================= views.module integration END  ===================== */
