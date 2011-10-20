<?php
// $Id$

/**
 * A function to check whether the current element is managed by dynamic fields.
 */
function dynamic_fields_get_element_dynamic_field($element, $type = 'both') {
  // Check to see if this element is registered to one of our fields.
  if (!empty($element['#field_name']) && !empty($element['#type_name'])) {
    $field = content_fields($element['#field_name'], $element['#type_name']);
    if (!empty($field)) {
      // Make sure this element is an allowed widget
      $allowed_widgets = dynamic_fields_allowed_widgets($field['type'], $type);
      if (in_array($field['widget']['type'], $allowed_widgets)) {
        return $field;
      }
    }
  }

  return FALSE;
}

 /**
 * A #process callback that we make sure is the first to run.
 */
function dynamic_fields_element_preprocess($element, $edit, &$form_state, $form) {
  $field = dynamic_fields_get_element_dynamic_field($element, 'controlled');

  if (!$field) {
    return $element;
  }

  $info = dynamic_fields_build_field_info($field, $form_state);

  if (!empty($info)) {
    $params = array($element, $info, $form_state, $form);
    $element = array_merge($element, dynamic_fields_invoke_field($field['type'], 'element_preprocess', $params));
  }
  return $element;
}


/**
 * An #after_build function for an controlled field element
 *
 */
function dynamic_fields_element_after_build($element, $form_state) {
  // Check to see if this element is registered to one of our fields.
  $field = dynamic_fields_get_element_dynamic_field($element, 'controlled');
//dpr($element);
//ddebug_backtrace();
  if (!$field) {
    return $element;
  }

  $info = dynamic_fields_build_field_info($field, $form_state);

  if (!empty($info['controlled_fields'])) {
    dynamic_fields_set_field_elements($element, $field, 'controller');
  }

  if (!empty($info)) {
    $params = array($element, $info, $form_state);
    $element = array_merge($element, dynamic_fields_invoke_field($field['type'], 'element_after_build', $params));
  }
  return $element;
}

 /**
 * A #process callback to extend the dynamic field element type.
 */
function dynamic_fields_element_process($element, $edit, &$form_state, $form) {
  // Check to see if this element is registered to one of our fields.
  $field = dynamic_fields_get_element_dynamic_field($element, 'controlled');
  if (!$field) {
    return $element;
  }

  $info = dynamic_fields_build_field_info($field, $form_state);
  if (!empty($info)) {
    $params = array($element, $info, $form_state);
    $element = array_merge($element, dynamic_fields_invoke_field($field['type'], 'element_process', $params));
  }
  dynamic_fields_set_form_state($form_state);
  return $element;
}


 /**
 * An #element_validate callback to extend the dynamic field element type.
 */
function dynamic_fields_element_validate($element, &$form_state, $form) {
  // Check to see if this element is registered to one of our fields.
  $field = dynamic_fields_get_element_dynamic_field($element,'controlled');
  if (!$field) {
    return $element;
  }

  $info = dynamic_fields_build_field_info($field, $form_state);
  if (!empty($info['controller_field'])) {
    $params = array($element, $info, $form_state);
    $element = array_merge($element, dynamic_fields_invoke_field($field['type'], 'element_validate', $params));
  }
  return $element;
}

/**
 * A #process callback to extend the optionwidgets type.
 */
function dynamic_fields_optionwidgets_process($element, $edit, &$form_state, $form) {
  // Make sure we have the form state set
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $info = dynamic_fields_build_field_info($field, $form_state);
  if (!empty($info)) {
    $column = $element['#columns'][0];
    $options = &$element[$column]['#options'];
    $option_count = count($element[$column]['#options']);
    $value = &$element['#value'][$column];

    if ($info['hide_if_empty']) {
      if ($option_count <= 0 || ($option_count == 1 && !empty($options[''])) ) {
        $element['#type'] = 'hidden';
        unset($element['#value']);
        form_set_value($element, $element['#value'], $form_state);
        dynamic_fields_set_form_state($form_state);
        return $element;
      }
    }
    if ($option_count > 0 && !empty($value)) {
      if (!isset($options[$value])) {
        $value = NULL;
        form_set_value($element, $value, $form_state);
      }
    }
    if ($info['hide_none']) {
      unset($element[$column]['#options']['']);
      if ($option_count > 0 && empty($value)) {
        reset($options);
        $element['#value'][$column] = key($options);
        form_set_value($element, $element['#value'][$column], $form_state);
      }
    }

    dynamic_fields_set_form_state($form_state);
  }

  return $element;
}