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

<div class="content-block container_9 clearfix clear-block">

  <span class="node-type-name">
  <h1><?php  print davissource_get_node_type_name($type); ?></h1>
  </span>

  <?php print $content; ?>
  
</div> <!-- /buildmode -->
