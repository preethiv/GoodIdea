<?php
// $Id$

/**
 * Implementation of hook_content_edit_form().
 */
function dynamic_fields_content_edit_form(&$form) {
  // Only support modules that implement hook_dynamic_fields_widgets().
  $field = $form['#field'];
  $widget = $field['widget'];
//dpr($field);
  $widget_type = isset($widget['widget_type']) ? $widget['widget_type'] : $widget['type'];
  if (!in_array($widget_type, dynamic_fields_allowed_widgets($field['type'], 'controlled'))) {
    return;
  }

  $type = (array)content_types($form['type_name']['#value']);

  dynamic_fields_field_settings_form($form['field'], $field, $type);

  dynamic_fields_widget_settings_form($form['widget'], $widget, $field, $type);

  return;
}

/**
 * Implementation of hook_widget_settings_alter().
 */

function _dynamic_fields_widget_settings_alter(&$settings, $op, $widget) {
  // Only support modules that implement hook_dynamic_fields_widgets().
  $widget_type = isset($widget['widget_type']) ? $widget['widget_type'] : $widget['type'];
  $allowed_widgets = dynamic_fields_allowed_widgets('', 'controlled');

  if (!in_array($widget_type, $allowed_widgets)) {
    return;
  }

  if ($op == 'save') {
    $settings = array_merge($settings, dynamic_fields_widget_settings($widget));
  }
}

/**
 * A list of settings needed by Dynamic Fields module on widgets.
 */
function dynamic_fields_widget_settings($widget) {
  $settings = array(
    'dynamic_fields',
  );
  return $settings;
}

/**
 * Configuration form for editing the field settings for a Dynamic Field.
 */
function dynamic_fields_field_settings_form(&$form, $field, $type) {
  $params = array($field, $type);
  $field_settings = dynamic_fields_invoke_field($field['type'], 'field_form', $params);
  if (!empty($field_settings)) {
    $form['dynamic_fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Dynamic Fields'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 15,
    );
    $form['dynamic_fields'] = array_merge($form['dynamic_fields'], $field_settings);
  }
}

/**
 * Configuration form for editing the field settings for a Dynamic Field.
 */
function dynamic_fields_widget_settings_form(&$form, $widget, $field, $type) {
  $controller_field = isset($widget['dynamic_fields']['controller_field']) ? $widget['dynamic_fields']['controller_field'] : '';

  $controlled_fields = dynamic_fields_controlled_fields($field, $type);
//dpr($widget);
  // Add the fieldset to the form
  $form['dynamic_fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dynamic Fields Setting'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($controller_field) && empty($controlled_fields),
    '#weight' => 15,
    '#attributes' => array('id' => 'dynamic-field-settings'),
    '#tree' => TRUE,
  );

  $fields = dynamic_fields_list(FALSE);

  $potential_fields = dynamic_fields_potential_controller_options($field, $type);
  
  $controller_already_exists = dynamic_fields_controller_already_exists($field, $type);

  if (!empty($potential_fields)) {
  	if (!$controller_already_exists) {
	    $form['dynamic_fields']['make_controller_field'] = array(
	      '#type' => 'radios',
	      '#options' => array(
	        t('Not a controller'),
	        t('Controller field'),
	      ),
	      '#title' => t('Controller Field'),
	      '#description' => t('Make this field a controller field.'),
	      '#default_value' => isset($field['widget']['dynamic_fields']['make_controller_field']) ? $field['widget']['dynamic_fields']['make_controller_field'] : 0,
	    );
	    $fft = array();
	    if ($field['widget']['type'] == 'content_taxonomy_select') {
	    	if (isset($field['vid']) && !empty($field['vid'])) {
		    	$vocab = taxonomy_vocabulary_load($field['vid']);
		    	$terms = $form['settings']['group_parent']['#options'][$vocab->name];
		    	$fft = array_values($terms);
	    	}
	    	else {
	        drupal_set_message(t('To use this field as a Dynamic Field controller remember to choose the appropriate Vocabulary below.'), 'info');
	    	}
	    	//dpr($terms);
	    } 
	    else {
	    	$fft = explode("\n", $field['allowed_values']);
	    }
	    
	    $form['dynamic_fields']['allowed_values'] = array(
			    '#type' => 'fieldset',
			    '#title' => t('Controller Field Options-Controlled Fields Mapping'),
			    '#collapsible' => TRUE,
			    '#collapsed' => TRUE,
			    '#weight' => 15,
			    '#attributes' => array('id' => 'dynamic-field-settings'),
	        '#prefix' => '<div class="dynamic-fields-controlled-fields">',
	        '#suffix' => '</div>',
			);
			$potential_controlled_fields = dynamic_fields_controlled_fields($field, $type);
			$pcf = array();
			//dpr($potential_controlled_fields);
			foreach ($potential_controlled_fields as $key => $value) {
				//dpr($key);
				if (isset($pcf[$key])) {
					continue;
				}
				$pcf[$key] = t($value['widget']['label']);
			}
			$potential_controlled_fields = array_unique($pcf);
	
			if (!empty($fft)) {
				foreach ($fft as $key => $value) {
		    	$form['dynamic_fields']['allowed_values'][trim($value)] = array(
				    '#type' => 'fieldset',
				    '#title' => t(trim($value).'-Controlled Fields'),
				    '#collapsible' => TRUE,
				    '#collapsed' => TRUE,
				    '#weight' => 15,
				    '#attributes' => array('id' => 'dynamic-field-settings'),
				  );
		    	$pcf_current_values = $field['widget']['dynamic_fields']['allowed_values'][trim($value)]['choice_controlled_field'];
				  if (!empty($pcf)) {
				    $form['dynamic_fields']['allowed_values'][trim($value)]['choice_controlled_field'] = array(
				      '#type' => 'checkboxes',
				      '#title' => t('Controlled Fields'),
				      '#description' => t('Select controlled fields for this controller option.'),
				      '#options' => $pcf,
	      			'#default_value' => isset($field['widget']['dynamic_fields']['allowed_values'][trim($value)]['choice_controlled_field']) ? $field['widget']['dynamic_fields']['allowed_values'][trim($value)]['choice_controlled_field'] : array(),
				    );
				  }
		    }
			}
			$controlled_fields_check = TRUE;
  	}
  	else {
	    $form['dynamic_fields']['make_controller_field'] = array(
	      '#type' => 'markup',
	      '#value' => t('A controller already exists for this content type.'),
	      '#prefix' => '<p class="dynamic-field-empty-text">',
	      '#suffix' => '</p>',
	    );
  	}
  }
  else {
    $form['dynamic_fields']['controller_field'] = array(
      '#type' => 'value',
      '#default_value' => '',
    );
    $form['dynamic_fields']['controller_field_empty'] = array(
      '#type' => 'markup',
      '#value' => t('This field cannot be a controller.'),
      '#prefix' => '<p class="dynamic-field-empty-text">',
      '#suffix' => '</p>',
    );
    $controlled_fields_check = FALSE;
  }
  
  if ($controlled_fields_check) {
	  $controlled_fields = dynamic_fields_controlled_fields_list($field, $type);
	
	  if (!empty($controlled_fields)) {
	    $form['dynamic_fields']['controlled_fields'] = array(
	      '#type' => 'markup',
	      '#value' => t('This field can acts as a controller to the following fields:') . theme_item_list($controlled_fields),
	      '#prefix' => '<div class="dynamic-fields-controlled-fields">',
	      '#suffix' => '</div>',
	    );
	  }
  }
}