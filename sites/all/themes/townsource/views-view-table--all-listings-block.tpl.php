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
<div class="popu_view_box2">
<div style="clear:both"></div>
			<div style="border-bottom:1px solid #b9e4ed;margin-bottom:5px;min-height:22px;">
				<div style="float:left">
					<h4><?php print $title; ?><span class="popular-listings-arrow"></span></h4>
				</div>
			</div>
<div style="clear:both"></div>
<?php foreach ($rows as $count => $row): ?>
			<div class="popu_view2">
				<ul>
				   <li><?php print $row['title'];?></li>
				</ul>
			</div>
<?php endforeach; ?>
</div>