<?php
class ed_classified_views_time_remaining_handler_field extends views_handler_field_date {
  function render($values) {
    $value = $values->edi_classified_nodes_expires_on;
    $time_now = time();
    $delta = $value ? $value - $time_now : 0;
    return format_interval($delta);
  }
}
