<?php

/**
 * @file
 * Dashboard module defaults file.
 * Stores default content options for dashboards.
 */

/** 
 * Create a default dashboard for a new user
 */
function dashboard_defaults($uid) {

  $dashboard = variable_get('dashboard_default_layout', '');

  if(is_array($dashboard)) {
    foreach($dashboard as $page) {
      $page_params = array(
        'uid' => $uid,
        'path' => $page['path'],
        'weight' => $page['weight'],
        'title' => $page['title'],
      );
      drupal_write_record('dashboard_page', $page_params);

      $page_id = $page_params['page_id'];

      if(is_array($page['widgets'])) {
        foreach($page['widgets'] as $widget) {
          $widget_params = array(
            'widget_id' => $widget['widget_id'],
            'page_id' => $page_id,
            'type' => $widget['type'],
            'subtype' => $widget['subtype'],
            'conf' => $widget['conf'],
            'col' => $widget['col'],
            'weight' => $widget['weight'],
            'visibility' => $widget['visibility'],
          );
          drupal_write_record('dashboard_widget', $widget_params);
        }
      }
    }
  } else {
    $page = array(
      'uid' => $uid,
      'path' => 'dashboard',
      'weight' => 0,
      'title' => 'Dashboard',
    );
    drupal_write_record('dashboard_page', $page);
  }

  drupal_set_message('A default dashboard has been generated.');

}

/** 
 * Set the default dashboard layout variable 
 */
function dashboard_set_default_layout($uid) {

  $dashboard = array();

  $pages = db_query("SELECT * FROM {dashboard_page} WHERE uid = '%d'", $uid);

  while($page = db_fetch_array($pages)) {
    $page_id = $page['page_id'];

    unset($page['page_id']);
    unset($page['uid']);

    $dashboard[$page_id] = $page;

  }

  if(count($dashboard) > 0) {
    $page_ids = implode(array_keys($dashboard), ',');
    $widgets = db_query("SELECT * FROM {dashboard_widget} WHERE page_id IN (%s)", $page_ids);

    while($widget = db_fetch_array($widgets)) {
      $page_id = $widget['page_id'];

      unset($widget['page_id']);

      $dashboard[$page_id]['widgets'][$widget['widget_id']] = $widget;
    }

    variable_set('dashboard_default_layout', $dashboard);

    drupal_set_message('You have set the default dashboard layout.');
  } else {
    drupal_set_message('Please setup a dashboard before you set the default.', 'error');
  }

}
