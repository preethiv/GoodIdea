<?php

/**
 * @file block.tpl.php
 *
 * Theme implementation to display a block.
 *
 * Available variables:
 * - $block->subject: Block title.
 * - $block->content: Block content.
 * - $block->module: Module that generated the block.
 * - $block->delta: This is a numeric id connected to each module.
 * - $block->region: The block region embedding the current block.
 *
 * Helper variables:
 * - $block_zebra: Outputs 'odd' and 'even' dependent on each block region.
 * - $zebra: Same output as $block_zebra but independent of any block region.
 * - $block_id: Counter dependent on each block region.
 * - $id: Same output as $block_id but independent of any block region.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_block()
 */
?>
<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="clearfix block block-<?php print $block->module ?>">
<?php if ($block->subject): ?>

<?php 
// Here I get the nid of the current page and send it to the custom function
// which generates a "See All" link for this block, if necessary.

if(arg(0) == 'node' && is_numeric(arg(1))) {
  $nid = arg(1); 
} else $nid = NULL;

$seealllink = davissource_block_seeall($block->module .'-'. $block->delta, $nid);
?>
<?php if($seealllink): ?>

<div id="templink"><?php print $seealllink; ?></div>

<?php endif; ?>

  <span class="block-title"><h2><?php print _davissource_override_block_title($block->subject) ?></h2></span>
<?php endif;?>

  <div class="content">
    <?php print $block->content ?>
  </div>
</div>