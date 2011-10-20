<?php
// $Id$

/**
 * Call all Dynamic Fields hooks stored in the available include files.
 */
function dynamic_fields_invoke_all($method, &$params) {
  $return = array();
  foreach (dynamic_fields_includes() as $source) {
    $function = 'dynamic_fields_' . $source . '_' . $method;
    if (function_exists($function)) {
      $result = call_user_func_array($function, $params);
      if (isset($result) && !empty($result) && is_array($result)) {
        $return = array_merge_recursive($return, $result);
      }
      elseif (isset($result)) {
        $return[] = $result;
      }
    }
  }
  return $return;
}

/**
 * Call Dynamic Fields hooks for specific fields / widgets
 */
function dynamic_fields_invoke_field($field, $method, &$params) {
  $field_name = $field;
  $return = array();
  $info = dynamic_fields_info();

  // Make sure we can handle this field.
  if (empty($info[$field_name])) {
    return $return;
  }
  $field_info = $info[$field_name];

  // Create and test if the function name exists.
  $function = 'dynamic_fields_' . $field_name . '_' . $method;
  if (function_exists($function)) {
    $result = call_user_func_array($function, $params);
    if (isset($result) && !empty($result)) {
      $return = $result;
    }
  }

  return $return;
}

/**
 * Load hook_dynamic_fields_info() data from all modules.
 */
function dynamic_fields_info() {
  static $info = array();
  if (!empty($info)) {
    return $info;
  }
  $info = module_invoke_all('dynamic_fields_info');
  drupal_alter('dynamic_fields_info', $info);
  return $info;
}

/**
 * Create a list of Dynamic Fields by name, suitable for a select list.
 */
function dynamic_fields_list($include_default = TRUE) {
  $info = dynamic_fields_info();
  $list = array();

  foreach ($info as $key => $source) {
    $list[$key] = $source['name'];
  }

  return $list;
}

/**
 * Implementation of hook_dynamic_fields_info().
 */
function dynamic_fields_dynamic_fields_info() {
  $params = array();
  return dynamic_fields_invoke_all('info', $params);
}

/**
 * Load all the potential sources.
 */
function dynamic_fields_includes($include = TRUE) {
  static $includes = array();
  if (!empty($includes)) {
    return $includes;
  }

  $directory = drupal_get_path('module', 'dynamic_fields') . '/fields';
  foreach (file_scan_directory($directory, '.inc$') as $file) {
    $includes[] = $file->name;
    if ($include) {
      include_once($file->filename);
    }
  }
  return $includes;
}

/**
 * This returns a list of widgets that are compatible with Dynamic Fields.
 */
function dynamic_fields_allowed_widgets($field = '', $type = 'both') {
  $widgets = array();
  foreach (dynamic_fields_info() as $source => $info) {
    if (empty($field) || $field == $source) {
      if ($type == 'both' || $type == 'controller') {
        if (!empty($info['controller_widgets'])) {
          $widgets = array_merge_recursive($widgets, $info['controller_widgets']);
        }
      }
      if ($type == 'both' || $type == 'controlled') {
        if (!empty($info['controlled_widgets'])) {
          $widgets = array_merge_recursive($widgets, $info['controlled_widgets']);
        }
      }
    }
  }
  return array_unique($widgets);
}

function dynamic_fields_build_field_info($field, $form_state = NULL, $controller_value = NULL, $omit_field_value = FALSE) {
  $info = array();
  if (!empty($field)) {
    $type = (array)content_types($field['type_name']);
    $controller_field = dynamic_fields_controller_field($field, $field['type_name']);
    $controlled_fields = dynamic_fields_controlled_fields($field, $type);
    $hide_if_empty = $field['widget']['dynamic_fields']['hide_if_empty'];
    $hide_none = $field['widget']['dynamic_fields']['hide_none'];
    if (empty($controller_field) && empty($controlled_fields)) {
      return $info;
    }
    $info = array(
      'field' => $field,
      'widget' => $field['widget'],
      'hide_if_empty' => $hide_if_empty,
      'hide_none' => $hide_none,

    );
    if (!$omit_field_value) {
      $info['field_value'] = dynamic_fields_retrieve_field_value($field, $form_state);
    }

    if (!empty($controller_field)) {
      $info['controller_field'] = $controller_field;
      // Check to see if a controller value has been passed in.
      if (!empty($controller_value)) {
        $info['controller_value'] = $controller_value;
      }
      else {
        $info['controller_value'] = dynamic_fields_retrieve_field_value($controller_field, $form_state);
      }
    }
    if (!empty($controlled_fields)) {
      $info['controlled_fields'] = $controlled_fields;
    }
  }
  $params = array($info, $field, $field['widget'], $type);
  $info = array_merge($info, dynamic_fields_invoke_field($field['type'], 'build_field_info', $params));

  return $info;
}