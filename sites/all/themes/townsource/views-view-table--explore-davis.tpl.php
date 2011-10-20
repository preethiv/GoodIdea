<?php
/**
 * @file views-view-table.tpl.php
 * Template to display a view as a table.
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $header_classes: An array of header classes keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $class: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * - $field_classes: An array of classes to apply to each field, indexed by
 *   field id, then row number. This matches the index in $rows.
 * @ingroup views_templates
 */
/* print $base_url;?>/sites/all/themes/townsource/images/blk_arw.png*/
 global $base_url;
?>
<div  id="right-sidebar" class="popu_view_box3" >				
			<h4><?php print $title; ?> </h4>
<?php //echo "<pre>"; print_r($rows) ;echo "</pre>";?>	
<?php $ie=1; ?>	
<?php foreach ($rows as $count => $row): ?>		
<?php if($ie <= 4) { ?>			
			<div class="popu_view3">
				<ul>
				   <li><?php print $row['name_1'];?></li>					 		   				
				</ul>
			</div>
<?php } ?>					
<?php $ie++; ?>						
<?php endforeach; ?>

</div>

