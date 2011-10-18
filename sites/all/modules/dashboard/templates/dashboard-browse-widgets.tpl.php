<?php if($links): ?>
  <div class="nav-content-dashboard"><?php print $links; ?></div>
<?php endif; ?>
<?php print $widget_filter; ?>
<?php if($widgets): ?>
  <?php foreach ($widgets as $widget): ?>
    <?php print theme('dashboard_display_widget', $widget); ?> 
  <?php endforeach; ?>
<?php endif; ?>

