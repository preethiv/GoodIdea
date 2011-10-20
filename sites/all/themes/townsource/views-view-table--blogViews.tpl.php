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
<?php /*print_r($rows);*/ ?>

<?php foreach ($rows as $count => $row): ?>	
<div class="listing_box">
<div class="listing_images"><?php print $row['field_image_fid'];?></div>
<h3><a href="#"><?php print $row['title'];?></a></h3>
<div class="star">
</div>
<p><strong>Posted by  <?php print $row['last_comment_name'];?> <?php print $row['created'];?></strong></p>
<p><?php print $row['body'];?> </p>
</div>
<?php endforeach; ?>







