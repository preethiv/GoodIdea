<?php
// $Id$

/**
 * @file
 * A Dynamic Fields extension for the number field
 */

/**
 * Implementation of hook_dynamic_fields_info().
 */
function dynamic_fields_number_info() {
  $field = array();
  $field['number_integer'] = array(
    'name' => t('Integer Field'),
    'controller_widgets' => array(
      'optionwidgets_select',
    ),
    'controlled_widgets' => array(
      'number',
      'optionwidgets_select',
      'optionwidgets_buttons',
      'optionwidgets_onoff'
    ),
  );
  $field['number_float'] = array(
    'name' => t('Float Field'),
    'controller_widgets' => array(
      'optionwidgets_select',
    ),
    'controlled_widgets' => array(
      'number',
      'optionwidgets_select',
      'optionwidgets_buttons',
      'optionwidgets_onoff'
    ),
  );
  $field['number_decimal'] = array(
    'name' => t('Decimal Field'),
    'controller_widgets' => array(
      'optionwidgets_select',
    ),
    'controlled_widgets' => array(
      'number',
      'optionwidgets_select',
      'optionwidgets_buttons',
      'optionwidgets_onoff'
    ),
  );
  return $field;
}

/* Integer Field */

/**
 * Implementation of hook_dynamic_fields_widget_form().
 */
function dynamic_fields_number_integer_widget_form($widget, $field, $type) {
  return dynamic_fields_number_widget_form($widget, $field, $type);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_number_integer_retrieve_field_value($field, $form_value, $form_state) {
  return dynamic_fields_number_retrieve_field_value($field, $form_value, $form_state);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_keys().
 */
function dynamic_fields_number_integer_retrieve_field_keys($field, $values) {
  return dynamic_fields_number_retrieve_field_keys($field, $values);
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_titles().
 */
function dynamic_fields_number_integer_retrieve_field_titles($field, $values) {
  return dynamic_fields_number_retrieve_field_titles($field, $values);
}

/**
 * Implementation of hook_allowed_values().
 */
function dynamic_fields_number_integer_allowed_values($field, $info) {
  return dynamic_fields_number_allowed_values($field, $info);
}


/* Float Field */

/**
 * Implementation of hook_dynamic_fields_widget_form().
 */
function dynamic_fields_number_float_widget_form($widget, $field, $type) {
  return dynamic_fields_number_widget_form($widget, $field, $type);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_number_float_retrieve_field_value($field, $form_value, $form_state) {
  return dynamic_fields_number_retrieve_field_value($field, $form_value, $form_state);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_keys().
 */
function dynamic_fields_number_float_retrieve_field_keys($field, $values) {
  return dynamic_fields_number_retrieve_field_keys($field, $values);
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_titles().
 */
function dynamic_fields_number_float_retrieve_field_titles($field, $values) {
  return dynamic_fields_number_retrieve_field_titles($field, $values);
}

/**
 * Implementation of hook_allowed_values().
 */
function dynamic_fields_number_float_allowed_values($field, $info) {
  return dynamic_fields_number_allowed_values($field, $info);
}

/* Decimal Field */

/**
 * Implementation of hook_dynamic_fields_widget_form().
 */
function dynamic_fields_number_decimal_widget_form($widget, $field, $type) {
  return dynamic_fields_number_widget_form($widget, $field, $type);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_number_decimal_retrieve_field_value($field, $form_value, $form_state) {
  return dynamic_fields_number_retrieve_field_value($field, $form_value, $form_state);
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_keys().
 */
function dynamic_fields_number_decimal_retrieve_field_keys($field, $values) {
  return dynamic_fields_number_retrieve_field_keys($field, $values);
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_titles().
 */
function dynamic_fields_number_decimal_retrieve_field_titles($field, $values) {
  return dynamic_fields_number_retrieve_field_titles($field, $values);
}

/**
 * Implementation of hook_allowed_values().
 */
function dynamic_fields_number_decimal_allowed_values($field, $info) {
  return dynamic_fields_number_allowed_values($field, $info);
}


/* Generic Number Functions called above */

/**
 * Implementation of hook_dynamic_fields_widget_form().
 */
function dynamic_fields_number_widget_form($widget, $field, $type) {

  return $form;
}


/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_number_retrieve_field_value($field, $form_value, $form_state) {
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
      $return[] = dynamic_fields_number_retrieve_field_value($field, $item, $form_state);
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
function dynamic_fields_number_retrieve_field_keys($field, $values) {
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
function dynamic_fields_number_retrieve_field_titles($field, $values) {
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
function dynamic_fields_number_allowed_values($field, $info) {
  $keys = dynamic_fields_retrieve_field_keys($info['controller_field'], $info['controller_value']);
  $all_values = content_allowed_values($field);
  $mapping = dynamic_fields_allowed_values_mapping($field);
  $allowed_values = dynamic_fields_filter_allowed_values($all_values, $mapping, $keys);
  return $allowed_values;
}
