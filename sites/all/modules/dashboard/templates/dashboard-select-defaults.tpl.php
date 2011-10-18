<?php

/**
 * @file dashboard-select-defaults.tpl.php
 * Default theme implementation to select default widgets from all available blocks.
 *
 */
?>
<?php
  // Add table javascript
  drupal_add_js('misc/tableheader.js');
?>
<p>Select blocks from this list to be added as default available widgets list that can be added to user dashboards. Widgets that are removed from this list are <em>not</em> removed from a user's dashboard.</p>
<table id="select-defaults" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Block'); ?></th>
      <th><?php print t('Operations'); ?></th>
    </tr>
  </thead>
  <tbody>
  <?php $row = 0; ?>
  <?php foreach($block_options as $key => $block): ?>
    <tr class="<?php print $row % 2 == 0 ? 'odd' : 'even'; ?>">
      <td><?php print $block['title']; ?></td>
      <td><?php print $block['operation']; ?></td>
    </tr>
    <?php $row++; ?>
  <? endforeach; ?>
  </tbody>
</table>
