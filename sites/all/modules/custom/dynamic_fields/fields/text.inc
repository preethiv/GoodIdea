<?php
// $Id$

/**
 * @file
 * A Dynamic Fields extension for the text field
 *
 */

/**
 * Implementation of hook_dynamic_fields_info().
 */
function dynamic_fields_text_info() {
  $field = array();
  $field['text'] = array(
    'name' => t('Text Field'),
    'controller_widgets' => array(
      'optionwidgets_select',
    ),
    'controlled_widgets' => array(
      'text_textfield',
      'text_textarea',
      'optionwidgets_select',
      'optionwidgets_buttons',
      'optionwidgets_onoff'
    ),
  );
  return $field;
}

/**
 * An #after_build callback for the field element
 */
function dynamic_fields_text_element_after_build($element, $info, $form_state) {
  return $element;
}

/**
 * An #after_build callback for the field element
 */
function dynamic_fields_text_element_process($element, $info, $form_state) {
  return $element;
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_text_retrieve_field_value($field, $form_value, $form_state) {
  $return = array();

  if (empty($form_value)) {
    return $return;
  }

  $multiple = isset($form_value[0]);

  if ($multiple) {
    foreach ($form_value as $delta => $item) {
      if (!is_numeric($delta) || isset($item)) {
        continue;
      }
      $return[] = dynamic_fields_text_retrieve_field_value($field, $item, $form_state);
    }
  }
  else {
    // Make sure we get the actual form element value.
    if (isset($form_value['value'])) {
      $return = array('value' => $form_value['value']);
    }
  }
  return $return;
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_keys().
 */
function dynamic_fields_text_retrieve_field_keys($field, $values) {
  $return = array();

  foreach ($values as $value) {
    // Locate each value.
    if (is_array($value) && isset($value['value'])) {
      $return[] = $value['value'];
    }
    elseif (isset($value)) {
      $return[] = $value;
    }
  }
  return $return;
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_titles().
 */
function dynamic_fields_text_retrieve_field_titles($field, $values) {
  $return = array();
  $all_values = content_allowed_values($field);
  foreach ($values as $value) {
    $key = '';
    if (is_array($value) && !empty($value['value'])) {
      $key = $value['value'];
    }
    elseif (!empty($value)) {
      $key = $value;
    }
    if (!empty($key) && !empty($all_values[$key])) {
      $return[] = $all_values[$key];
    }
  }
  return $return;

}

/**
 * Implementation of hook_allowed_values().
 */
function dynamic_fields_text_allowed_values($field, $info) {
  $keys = dynamic_fields_retrieve_field_keys($info['controller_field'], $info['controller_value']);
  $all_values = content_allowed_values($field);
  $mapping = dynamic_fields_allowed_values_mapping($field);
  $allowed_values = dynamic_fields_filter_allowed_values($all_values, $mapping, $keys);
  return $allowed_values;
}
