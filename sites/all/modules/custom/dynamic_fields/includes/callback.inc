<?php
// $Id$

/**
 * The AHAH callbacks.
 */
function dynamic_fields_callback($type_name, $field_name, $delta) {
  module_load_include('inc', 'dynamic_fields', 'includes/form');
  module_load_include('inc', 'node', 'node.pages');

  $field = content_fields($field_name, $type_name);
  if ($field['widget']['dynamic_fields']['make_controller_field'] != 1) {
    print drupal_json(array('status' => FALSE, 'data' => 'Not'));
    exit();
  }

  $type = (array)content_types($type_name);

  $form_state = array('storage' => NULL, 'submitted' => TRUE);

  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);

  if (!$form) {
    // Invalid form_build_id.
    drupal_set_message(t('An unrecoverable error occurred. This form was missing from the server cache. Try reloading the page and submitting again.'), 'error');
    print drupal_to_js(array('data' => theme('status_messages')));
    exit;
  }

  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;

  // Enable the submit/validate handlers to determine whether AHAH-submittted.
  $form_state['ahah_submission'] = TRUE;
  $form_state['ahah_controller'] = $field_name;
  $form_state['ahah_controlled'] = array();

  $form['#programmed'] = $form['#redirect'] = FALSE;

  $form_state['values'] = array();
  // Step #5.
  drupal_process_form($form_id, $form, $form_state);
  // Step #6 and #7 and #8.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  // Step #9.
  $form_element = $form[$field_name . '_ahah_wrapper'];
  // Since the after build functions don't seem to run in some cck fields form, lets
  // run it on our form element.

  dynamic_fields_callback_after_build($form_element, $form_state);

  unset($form_element['#prefix'], $form_element['#suffix']);

  if (isset($form_element['_weight'])) {
    unset($form_element['_weight']);
  }

  // Render our form element
  $output = drupal_render($form_element);

  // Loop through the JS settings and find the settings needed for our fields.
  $javascript = drupal_add_js(NULL, NULL);
  $dynamic_field_ahah_settings = array();
  if (isset($javascript['setting'])) {
    foreach ($javascript['setting'] as $settings) {
      if (isset($settings['ahah'])) {
        foreach ($settings['ahah'] as $id => $ahah_settings) {
          $url = $ahah_settings['url'];
          if (strpos($url, 'dynamic_fields/callbacklevel2')) {
            $dynamic_field_ahah_settings[$id] = $ahah_settings;
          }
        }
      }
    }
  }

  // Add the AHAH settings needed for our new buttons.
  if (!empty($dynamic_field_ahah_settings)) {
    //$output .= '<script type="text/javascript">jQuery.extend(Drupal.settings.ahah, '. drupal_to_js($dynamic_field_ahah_settings) .');</script>';
  }
  
  // Include any status messages (unlikely as we have disabled validation)
  $output_clear = theme('status_messages');

  print drupal_json(array(
    'status'   => TRUE,
    'data'     => $output,
  ));

  exit();
}
