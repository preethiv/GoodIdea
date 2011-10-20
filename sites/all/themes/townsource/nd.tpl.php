<?php
// $Id: nd.tpl.php,v 1.1.2.3 2010/03/15 19:19:48 swentel Exp $

/**
 * @file
 * This is an implementation of the template provided by nd 
 * which is optimized for use with the Node displays module.
 *
 *
 */
?>

<?php if($build_mode == 'full') $fullpage = true; ?>

<div class="content-block clearfix buildmode-<?php print $build_mode; ?>
            <?php if($fullpage) : ?> container_12 <?php endif;?>
            node node-type-<?php print $node->type; ?> <?php if (isset($node_classes)): print $node_classes; endif; ?><?php if ($sticky && $node->build_mode == 'sticky'): print ' sticky'; endif; ?><?php if (!$status): print ' node-unpublished'; endif; ?>
            ">
  <?php if($fullpage): ?>
  	<span class="node-type-name">
  		<h1><?php  print davissource_get_node_type_name($type); ?></h1>
  	</span>
  <?php endif; ?>
  <?php print $content; ?> 
</div> <!-- /buildmode -->
