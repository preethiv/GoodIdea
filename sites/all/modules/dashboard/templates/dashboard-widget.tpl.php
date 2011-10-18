<?php

/**
 * @file
 * Widget template file for Dashboard module.
 */

  // TODO: move this into a preprocess function
  if ($widget->subject) {
    $widget_title = $widget->subject;
  } else if ($widget->title) {
    $widget_title = $widget->title;
  } else {
    $widget_title = 'Widget '. $widget->widget_id;
  }

?>
<div id="widget-<?php print $widget->widget_id ?>" class="widget grid-4 alpha omega">
  <div class="tools">
    <a href="#" class="minimize-widget" title="<?php print t('Hide This Widget') ?>"></a>
    <?php if ($widget->pages): ?>
      <a href="#" class="change-widget-page" title="<?php print t('Change Widget Page') ?>"></a>
    <?php endif; ?>
    <a href="#" class="remove-widget" title="<?php print t('Remove from Dashboard') ?>"></a>
    <?php if ($widget->pages): ?>
      <ul class="page-list">
        <li>Move widget to...</li>
        <?php print $widget->pages; ?>
      </ul>
    <?php endif; ?>
  </div>
  <h2><?php print $widget_title; ?></h2>
  <div class="content"<?php print $widget->visibility;?>>
    <?php print $widget->content; ?>
  </div>
</div>
