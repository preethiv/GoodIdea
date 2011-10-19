<?php
// $Id$

/**
 * @file
 * A Dynamic Fields extension to allow referencing of existing files.
 */

/**
 * Implementation of hook_dynamic_fields_info().
 */
function dynamic_fields_nodereference_info() {
  $field = array();
  $field['nodereference'] = array(
    'name' => t('Node Reference'),
    'controller_widgets' => array(
      'nodereference_select'
    ),
    'controlled_widgets' => array(
      'nodereference_select',
      'nodereference_autocomplete',
      'nodereference_buttons'
    ),
  );
  return $field;
}

/**
 * Implementation of hook_menu().
 */
function dynamic_fields_nodereference_menu() {
  $items = array();

  return $items;
}


/**
 * Implementation of hook_dynamic_fields_field_form().
 */
function dynamic_fields_nodereference_field_form($field, $type) {
  $return = array();

  return $return;

}

/**
 * Implementation of hook_dynamic_fields_widget_form().
 */
function dynamic_fields_nodereference_widget_form($widget, $field, $type) {

  return $form;
}


/**
 * Implementation of hook_dynamic_fields_widget_settings().
 */
function dynamic_fields_nodereference_widget_settings($op, $field) {
  $return = array();
  return $return;

}


/**
 * An #after_build callback for the nodereference element
 */
function dynamic_fields_nodereference_element_after_build($element, $info, $form_state) {
  // Check if there is a controller field for this element.
  if (!empty($info['controller_field'])) {
    // Check if the widget is a select box or buttons and if so, apply the new options
    if (in_array($element['#type'], array('nodereference_select'))) {
      //$options = dynamic_fields_nodereference_allowed_values($info['field'], $info);
      //$element['nid']['nid']['#options'] = $options;

    }
  }

  return $element;
}

/**
 * An #process callback for the nodereference element
 */
function dynamic_fields_nodereference_element_process($element, $info, $form_state) {
  if (!empty($info['controller_field'])) {

  }

  return $element;
}

/**
 * An #validate callback for the nodereference element
 */
function dynamic_fields_nodereference_element_validate($element, $info, $form_state) {
  // TODO: See if this handler is actually needed
  $field = $info['field'];
  $nids = dynamic_fields_nodereference_retrieve_field_keys($info['field'], $info['field_value']);
  // Don't check if there are no nids
  if ($nids) {
    $field_key = $element['#columns'][0];
    $references = _dynamic_fields_nodereference_potential_references($info['field'], $info, '', NULL, $nids);
    foreach ($nids as $nid) {
      if (is_numeric($nid) && !isset($references[$nid])) {
        form_error($element, t("%name: this item can't be referenced .", array('%name' => t($field['widget']['label']))));
      }
    }
  }
  return $element;
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_value().
 */
function dynamic_fields_nodereference_retrieve_field_value($field, $form_value, $form_state) {
  $return = array();
  if (empty($form_value)) {
    return $return;
  }

  $multiple = isset($form_value[0]);

  if ($multiple) {
    foreach ($form_value as $delta => $item) {
      if (!is_numeric($delta) || empty($item)) {
        continue;
      }
      $return[] = dynamic_fields_nodereference_retrieve_field_value($field, $item, $form_state);
    }
  }
  else {
    // Make sure we get the actual form element value.
    if (is_array($form_value['nid'])) {
      $value = $form_value['nid']['nid'];
    }
    else {
      $value = $form_value['nid'];
    }

    if (is_numeric($value)) {
      $return = array('nid' => $value);
    }
    elseif (is_array($value)) {
      foreach ($value as $nid) {
        $return[] = $nid;
      }
    }
    elseif (is_string($value) && !empty($value)) {
      // Parse the nid from the autocomplete string
      preg_match('/^(?:\s*|(.*) )?\[\s*nid\s*:\s*(\d+)\s*\]$/', $value, $matches);
      if (!empty($matches)) {
        // Explicit [nid:n].
        list(, $title, $nid) = $matches;
        $return = array(
          'nid' => $nid,
          'title' => $title,
        );
      }
      else {
        // No [nid:n] value matched, so we fallback to searching for the title.
        $info = dynamic_fields_build_field_info($field, $form_state, NULL, TRUE);
        $reference = _dynamic_fields_nodereference_potential_references($field, $info, $value, 'equals', NULL, 1);
        if (!empty($reference)) {
          $nid = key($reference);
          $return = array(
            'nid' => $nid,
            'title' => $reference[$nid],
          );
        }
      }
    }
  }

  return $return;
}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_keys().
 */
function dynamic_fields_nodereference_retrieve_field_keys($field, $values) {
  $return = array();
  foreach ($values as $value) {
    // Locate the nid for each value.
    if (is_array($value) && is_numeric($value['nid'])) {
      $return[] = $value['nid'];
    }
    elseif (is_numeric($value)) {
      $return[] = $value;
    }
  }
  return $return;

}

/**
 * Implementation of hook_dynamic_fields_retrieve_field_titles().
 */
function dynamic_fields_nodereference_retrieve_field_titles($field, $values) {
  $return = array();
  foreach ($values as $value) {
    $nid = NULL;
    // Locate the title for each value.
    if (is_array($value) && !empty($value['title'])) {
      $return[] = $value['title'];
      continue;
    }
    elseif (is_array($value) && !is_numeric($value['nid'])) {
      $nid = $value['nid'];
    }
    elseif (is_numeric($value)) {
      $nid = $value;
    }
    if (!empty($nid)) {
      $sql = 'SELECT title FROM {node} WHERE nid = %d';
      $result = db_query($sql, $nid);
      $title = db_result($result);
      if (!empty($title)) {
        $return[] = $title;
      }
    }
  }
  return $return;

}

/**
 * Implementation of hook_dynamic_fields_build_field_info().
 */
function dynamic_fields_nodereference_build_field_info($info, $field, $widget, $type) {
  $return = array();
  $filter_field = $widget['dynamic_fields']['filter_field'];
  $filter_forced = isset($widget['dynamic_fields']['filter_forced']) ? $widget['dynamic_fields']['filter_forced'] : TRUE;
  $controller_view_args = isset($widget['dynamic_fields']['advanced']['controller_view_args']) ? $widget['dynamic_fields']['advanced']['controller_view_args'] : '';

  if (!empty($filter_field)) {
    $return['filter_field'] = content_fields($filter_field, $type_name);
  }

  if (empty($return['filter_field']) && !empty($info['controller_field'])) {
    $filter_field  = $info['controller_field'];
  }
  $return['filter_field'] = $filter_field;
  $return['filter_forced'] = $filter_forced;
  $return['controller_view_args'] = $controller_view_args;
  return $return;
}

/**
 * Implementation of hook_allowed_values().
 */
function dynamic_fields_nodereference_allowed_values($field, $info) {
  $references = _dynamic_fields_nodereference_potential_references($field, $info);
  return $references;
}

/**
 * Fetch an array of all candidate referenced nodes.
 */
function _dynamic_fields_nodereference_potential_references($field, $info, $string = '', $match = 'contains', $ids = array(), $limit = NULL) {
  static $results = array();
  // Create unique id for static cache.
  $cid = $field['field_name'] .':'. $match .':'. ($string !== '' ? $string : implode('-', $ids)) .':'. $limit;
  if (!isset($results[$cid])) {
    $references = FALSE;
    // Check if we need to use a view to get the potential references
    if (module_exists('views') && !empty($field['advanced_view']) && $field['advanced_view'] != '--') {
      $references = _dynamic_fields_nodereference_potential_references_views($field, $info, $string, $match, $ids, $limit);
    }
    // If the view doesn't exist, we got FALSE, and fallback to the regular 'standard mode'.

    if ($references === FALSE) {
      $references = _dynamic_fields_nodereference_potential_references_filtered($field, $info, $string, $match, $ids, $limit);
    }

    // Store the results.
    $results[$cid] = !empty($references) ? $references : array();
  }

  return $results[$cid];
}

function _dynamic_fields_nodereference_potential_references_views($field, $info, $string = '', $match = 'contains', $ids = array(), $limit = NULL) {
  $references = array();
  //$controller_view_args = isset($widget['dynamic_fields']['controller_view_args']) ? $widget['dynamic_fields']['controller_view_args'] : '';
  if (!empty($info['controller_view_args'])) {
    $default_args = $field['advanced_view_args'];
    $view_args = $info['controller_view_args'];

    // Replace our tokens with controller values
    $view_args = dynamic_fields_replace_string_tokens($info, $view_args);

    $field['advanced_view_args'] = $view_args;
    $references = _nodereference_potential_references_views($field, $string, $match, $ids, $limit);
    $field['advanced_view_args'] = $default_args;
  }
  else {
    $references = _nodereference_potential_references_views($field, $string, $match, $ids, $limit);
  }
  return $references;
}

/**
 * Helper function for _nodereference_potential_references():
 * referenceable nodes defined by content types.
 */
function _dynamic_fields_nodereference_potential_references_filtered($field, $info, $string = '', $match = 'contains', $ids = array(), $limit = NULL) {
  $related_types = array();
  $where = array();
  $join = array();
  $args = array();

  if (is_array($field['referenceable_types'])) {
    foreach (array_filter($field['referenceable_types']) as $related_type) {
      $related_types[] = "n.type = '%s'";
      $args[] = $related_type;
    }
  }

  if (!empty($related_types)) {
    $where[] = implode(' OR ', $related_types);
  }

  // Check if there is a controller field, and if so filter the results
  // TODO: more options on how the filter is applied.
  if (!empty($info['controller_field'])) {
    $empty_value = TRUE;
    foreach ($info['controller_value'] as $value) {
      if (is_numeric($value) || !empty($value)) {
        $empty_value = FALSE;
      }
    }
    // If the values are empty and force filter is set to true then return an
    // empty array.
    if ($empty_value && $info['filter_forced']) {
      return array();
    }
    elseif (!$empty_value) {
      // Use the filter field if it is set.
      $filter_field = !empty($info['filter_field']) ? $info['filter_field'] : $info['controller_field'];
      if (is_string($filter_field)) {
        $filter_field = content_fields($filter_field);
      }
  
      $controller_value = $info['controller_value'];
      // Retrieve Database info for the field.
      $db_info = content_database_info($filter_field);
      // Set the appropriate column
      $filter_column = array_shift($db_info['columns']);
      $filter_column = $filter_column['column'];

      $filter_keys = dynamic_fields_retrieve_field_keys($info['controller_field'], $info['controller_value']);

      if (!empty($filter_column) && !empty($filter_keys)) {
        $join[] = "INNER JOIN {". $db_info['table'] ."} cf ON n.vid = cf.vid";
        $where[] = "cf. $filter_column IN (" . db_placeholders($filter_keys) . ")";
        $args = array_merge($args, $filter_keys);
      }
    }

  }

  if ($string !== '') {
    $match_operators = array(
      'contains' => "LIKE '%%%s%%'",
      'equals' => "= '%s'",
      'starts_with' => "LIKE '%s%%'",
    );
    $where[] = 'n.title '. (isset($match_operators[$match]) ? $match_operators[$match] : $match_operators['contains']);
    $args[] = $string;
  }
  elseif ($ids) {
    $where[] = 'n.nid IN (' . db_placeholders($ids) . ')';
    $args = array_merge($args, $ids);
  }

  $join_clause = $join ? implode(' ', $join) : '';
  $where_clause = $where ? 'WHERE ('. implode(') AND (', $where) .')' : '';
  $sql = db_rewrite_sql("SELECT n.nid, n.title AS node_title, n.type AS node_type FROM {node} n $join_clause $where_clause ORDER BY n.title, n.type");
  $result = $limit ? db_query_range($sql, $args, 0, $limit) : db_query($sql, $args);
  $references = array();
  while ($node = db_fetch_object($result)) {
    $references[$node->nid] = array(
      'title' => $node->node_title,
      'rendered' => check_plain($node->node_title),
    );
  }

  return $references;
}


function _dynamic_fields_nodereference_potential_filter_fields($type, $include_controller = TRUE, $include_none = TRUE) {
  $options = array();
  $field_types = _content_field_types();
  // TODO: Filter only by those belonging to the referencable types.
  $potential_fields = content_fields();
  if (empty($potential_fields)) {
    return FALSE;
  }

  foreach ($potential_fields as $field) {
    $field_type = $field_types[$field['type']];
    $text = t('@type: @field (@label)', array('@type' => t($field_type['label']), '@label' => t($field['widget']['label']), '@field' => $field['field_name']));
    $options[$field['field_name']] = (drupal_strlen($text) > 80) ? truncate_utf8($text, 77) . '...' : $text;
  }

  asort($options);

  if ($include_controller) {
    array_unshift($options, t('Filter by Controller Field'));
  }
  if ($include_none) {
    $options['none'] = t('No Filter Field');
  }

  return $options;
}