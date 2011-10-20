<?php
// $Id$

/*!
 * Views Slideshow Dynamic display block module template: vsd-townsource-10p - content template
 * Copyright (c) 2008 - 2010 P. Blaauw All rights reserved.
 * Version 1.2 (13-APR-2010)
 * Licenced under GPL license
 * http://www.gnu.org/licenses/gpl.html
 */

/**
 * @file
 * Dynamic display block module template: vsd-townsource-10p - content template
 *
 * Available variables:
 * - $settings['origin']: From which module comes the block.
 * - $settings['delta']: Block number of the block.
 *
 * - $settings['template']: template name
 * - $settings['output_type']: type of content
 *
 * - $views_slideshow_ddblock_slider_items: array with slidecontent
 * - $settings['slide_text_position']: of the text in the slider (top | right | bottom | left)
 * - $settings['slide_direction']: direction of the text in the slider (horizontal | vertical )
 * - 
 * - $views_slideshow_ddblock_pager_content: Themed pager content
 * - $settings['pager_position']: position of the pager (top | bottom)
 *
 * notes: don't change the ID names, they are used by the jQuery script.
 */
 
$settings = $views_slideshow_ddblock_slider_settings;
// add Cascading style sheet
drupal_add_css($directory . '/custom/modules/views_slideshow_ddblock/' . $settings['template'] . '/views-slideshow-ddblock-cycle-' . $settings['template'] . '.css', 'template', 'all', FALSE);
?>
<!-- dynamic display block slideshow -->
<div id="views-slideshow-ddblock-<?php print $settings['delta'] ?>" class="views-slideshow-ddblock-cycle-<?php print $settings['template'] ?> clear-block">
 <div class="container clear-block border">
  <div class="container-inner clear-block border">
   <?php if ($settings['pager_position'] == "top") : ?>
    <!-- number pager --> 
    <?php print $views_slideshow_ddblock_pager_content ?>
   <?php endif; ?>
   <!-- slider content -->
   <div class="slider clear-block border">
    <div class="slider-inner clear-block border">
     <?php if ($settings['output_type'] == 'view_fields') : ?>
      <?php foreach ($views_slideshow_ddblock_slider_items as $slider_item): ?>
       <div class="slide clear-block border">
        <div class="slide-inner clear-block border">
       
       		<?php 
       		$nid = $slider_item['node_id'];
          $node = node_load($nid);
          $node->build_mode = 'views_slideshow';
          $teaser = ($node->build_mode != 'full') ? TRUE : FALSE;
          $show_links = ds_show_field('nd', $node->type, $node->build_mode, 'links');
          $teaser = node_view($node, $teaser, FALSE, $links);
          print $teaser;
          
          ?>
        </div> <!-- slide-inner-->
       </div>  <!-- slide-->
      <?php endforeach; ?>
     <?php endif; ?>
      <div class="controls"> 
       <span><a href="" class="prev">Prev</a></span> 
       <span><a href="" class="next">Next</a></span> 
       <span><a href="" class="pause">Pause</a></span> 
       <span><a href="" class="play">Play</a></span> 
      </div> 
    </div> <!-- slider-inner-->
   </div>  <!-- slider-->
   <?php if ($settings['pager_position'] == "bottom") : ?>
    <!-- number pager --> 
    <?php print $views_slideshow_ddblock_pager_content ?>
   <?php endif; ?>
  </div> <!-- container-inner-->
 </div> <!--container-->
</div> <!--  template -->
