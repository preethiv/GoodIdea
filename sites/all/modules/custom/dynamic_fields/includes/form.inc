<?php
// $Id$

/**
 * Implementation of node_form_alter - invoked in hook_form_alter
 */

function dynamic_fields_node_form_alter(&$form, &$form_state, $form_id) {
  $type = (array)content_types($form['type']['#value']);
  $controllers = dynamic_fields_type_controllers_controlled($type);
  if (!empty($controllers)) {
    dynamic_fields_set_form_state($form_state);
    dynamic_fields_reorganize_form($form, $form_state, $controllers, $type['fields'], $type);
    $form['#after_build'][] = 'dynamic_fields_node_form_after_build';
    $form['#pre_render'][] = 'dynamic_fields_node_form_pre_render';
    $form['#validate'][] = 'dynamic_fields_node_form_validate';
  }
}

/**
 * Additional #after_build handler for the node form.
 *
 */

function dynamic_fields_node_form_after_build($form, &$form_state) {
  
  $form['#cache'] = TRUE;
  $type = (array)content_types($form['type']['#value']);

  $controllers = dynamic_fields_type_controllers_controlled($type);

  // Check to see if this type has any controller fields.
  if (!empty($controllers)) {
    // Check to see if this is an ahah submission.
    if (!empty($form_state['ahah_submission']) && $form_state['ahah_submission']) {
      // Disable validation on the entire form, to prevent errors on fields.
      dynamic_fields_disable_validation($form);
    }
    // To be on the safe site, reset the field overrides
    dynamic_fields_set_field_overrides($form['#field_info'], NULL, TRUE);
  }

  return $form;
}

/**
 * Additional #validate handler for the node form.
 */
function dynamic_fields_node_form_validate($form, &$form_state) {
  $continue_button = $form_state['clicked_button']['#submit'][0] == 'dynamic_fields_fallback_submit' ? TRUE : FALSE;
    if ($continue_button) {
      /// Since we can't seem to disable all validation, lets clear error messages
      drupal_get_messages();
    }
}

/**
 * Additional #pre_render handler for the node form.
 */

function dynamic_fields_node_form_pre_render($form) {
  drupal_add_css(drupal_get_path('module', 'dynamic_fields') . '/dynamic_fields.css');
  return $form;
}

/**
 * Run the after build functions on the field element.
 */
function dynamic_fields_callback_after_build(&$form, &$form_state) {
  if (isset($form['#after_build'])) {
    foreach ($form['#after_build'] as $function) {
      $form = $function($form, $form_state);
      $form['#after_build']['#after_build_done'] = TRUE;
    }
  }
  if (is_array($form)) {
	  foreach ($form as $id => $element) {
	    if ($id != '#after_build' && is_array($element)) {
	      dynamic_fields_callback_after_build($form[$id], $form_state);
	    }
	  }
  }
}

/**
 * Disable validation for all of the form.
 */
function dynamic_fields_disable_validation(&$form) {
  foreach (element_children($form) as $child) {
    $form[$child]['#validated'] = TRUE;
    $form[$child]['#element_validate'] = NULL;
    $form[$child]['#needs_validation'] = FALSE;
    dynamic_fields_disable_validation($form[$child]);
  }
}

/**
 * Function to prepare form to work with the ahah callback
 */
function dynamic_fields_reorganize_form(&$form, &$form_state, $tree, $fields, $type, $level = 0) {
	$controllers_level2_count = 0;
	$top_controller_field_name = '';
	$level2_controller_field_name = '';
	foreach ($tree as $field_name => $field_array) {
    $controller_field = $field_name;
    $child_count = count($field_array);
    if ($child_count ==  0) {
      continue;
    }
        
    if ($form['#field_info'][$field_name]['widget']['dynamic_fields']['make_controller_field'] == 1) {
	    $wrapper = array();
	    $wrapper['#prefix'] = '<div id="checkboxes">';
	    $wrapper['#value'] = ' ';
	    $wrapper['#suffix'] = '</div>';
	    $wrapper['#weight'] = $form[$field_name]['#weight'] + 1;
	    $top_controller_field_name = $field_name;
    }

    if ($form_state['ahah_submission']) {
    	$controller_option =  $form_state['post'][$field_name]['value'];
    	if (isset($type['fields'][$field_name]) && $type['fields'][$field_name]['module'] == 'content_taxonomy') {
    		$term = taxonomy_get_term($controller_option);
    		$controller_option = $term->name;
    	}
    	if (empty($controller_option)) {
    		$form[$field_name . '_ahah_wrapper'] = $wrapper;
    		continue;
    	}
    	$controlled_fields = $form['#field_info'][$field_name]['widget']['dynamic_fields']['allowed_values'][$controller_option]['choice_controlled_field'];
			foreach ($controlled_fields as $controlled_field) {
				$wrapper[$controlled_field] = $form[$controlled_field];
				unset($form[$controlled_field]);
			}
    }
    else if ($form[$field_name]['#default_value'][0]['value'] != '') {
    	$controller_option = $form[$field_name]['#default_value'][0]['value'];
    	if (isset($type['fields'][$field_name]) && $type['fields'][$field_name]['module'] == 'content_taxonomy') {
    		$term = taxonomy_get_term($controller_option);
    		$controller_option = $term->name;
    	}
    	$controlled_fields = $form['#field_info'][$field_name]['widget']['dynamic_fields']['allowed_values'][$controller_option]['choice_controlled_field'];
			foreach ($controlled_fields as $controlled_field) {
				if ($form['#field_info'][$field_name]['widget']['dynamic_fields']['make_controller_field'] == 1) {
					$wrapper[$controlled_field] = $form[$controlled_field];
				}
				unset($form[$controlled_field]);
			}
    }
    
    if ($form['#field_info'][$field_name]['widget']['dynamic_fields']['make_controller_field'] == 1) {
    	$form[$field_name . '_ahah_wrapper'] = $wrapper;
    }
    
    foreach ($field_array as $controlled_field) {
    	if (isset($form[$controlled_field])) {
    		unset($form[$controlled_field]);
    	}
    }
  }
  
}

/**
 * Function to check if a field is located within a fieldgroup.
 */
function dynamic_fields_get_field_group_name($type, $field_name) {
  if (module_exists('fieldgroup') && ($group_name = _fieldgroup_field_get_group($type['type'], $field_name))) {
    return $group_name;
  }

  return FALSE;
}

/**
 * Function to retrieve a field's parent element from a form.
 */
function &dynamic_fields_retrieve_field_parent_element(&$form, $type, $field_name) {
  $group_name = &dynamic_fields_get_field_group_name($type, $field_name);

  if (!empty($group_name)) {
    $parent = &$form[$group_name];
  }
  else {
    $parent = &$form;
  }

  return $parent;
}

/**
 * Function to retrieve a field element from a form.
 */
function &dynamic_fields_retrieve_field_element(&$form, $type, $field_name) {
  $parent = &dynamic_fields_retrieve_field_parent_element($form, $type, $field_name);
  return $parent[$field_name];
}
