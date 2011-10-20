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
 global $base_url;
?>
<div class="popu_view_box">
<h4><?php print $title; ?><span class="popular-listings-arrow"></span></h4>
<?php $ie=1; ?>
<?php foreach ($rows as $count => $row): ?>
<?php if($ie == 1) {	?>
<?php print $row['field_business_image_fid'];?>
<?php } ?>
<?php $ie++; ?>
<?php endforeach; ?>
<?php
//	echo "<pre>"; print_r($rows);echo "<pre>";
?>
<?php foreach ($rows as $count => $row): ?>
			<div class="popu_view">
				<ul>
				   <li><span><?php print $row['title'];?></span><br /><span class="field-number-connected"><?php print $row['count'];?></span></li>
				</ul>
			</div>
<?php endforeach; ?>
</div>