<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
  <script type="text/javascript" src="<?php print base_path(); ?>sites/all/themes/townsource/js/jquery.corner.js"></script>
  <script type="text/javascript">
    $(function(){
      if ($('#content')) {
        $('#content').corner();
      }
      if ($('#search-area')) {
        $('#search-area').corner("bottom");
      }
    });
  </script>
</head>

<body class="<?php print $classes; ?>">

  <div id="header" class="container_16 clearfix">
    <?php print $header; ?>

    <div id="logo" class="grid_8 push_1 alpha">
    	<img src="<?php print base_path() . path_to_theme(); ?>/images/logo.png" />
    </div>

    <div id="top-corner-bar">
      <?php if($user->uid): ?>
      <?php $block = module_invoke('custom_forms', 'block', 'view', my_account_block);
            print $block['content'];
      ?>
      <?php //print $myaccountblock; ?>
      <?php else: ?>
        <span id="anon-top-text">
          <a href="<?php print url('user/login');?>">Log In</a> |
          <?php  print theme_menu_item_link(menu_link_load(1302));?>
        </span>
      <?php endif; ?>
    </div>
    <?php if($user->uid): ?>
	<div class="top-add"><?php print $bannerad; ?></div>
    <?php endif; ?>

    <div id="top-menu" class="container_16 clearfix">
      <?php if ($primary_links): ?>
        <?php print theme(array('links__system_main_menu', 'links'), $primary_links,
                    array(
                      'id' => 'top-menu',
                      'class' => 'clearfix',
                    ));
        ?>
      <?php endif; ?>
    </div>

    <div id="under-menu" class="container_16 clearfix">
      <div id="tagline" class="grid_16 alpha omega">
        <p>Davis source is the fun and easy way to find reviews and talk about the great - and not so great - in your area.</p>
      </div>
    </div>

    <div id="search-area" class="container_16 clearfix">
      <div id="inside-search" class="grid_16 alpha omega">
        <div id="search-form">
          <?php print $search_box; ?>
      </div>
    </div>

  </div> <!-- End #header -->



  <div id="content" class="container_16 clearfix">

    <div class="inner-content container_16 clearfix">

      <div id="main" class="grid_12 clearfix">


      	<?php print $tabs; ?>
	<?php print $topcontent; ?>
        <?php print $content; ?>
      </div> <!-- End #main -->

      <div id="right-sidebar" class="grid_4 clearfix">
        <?php print $sidebar_right; ?>
      </div> <!-- End #sidebar-right -->

    </div> <!-- End #inner-content -->

  </div> <!-- End #content -->


  <div id="footer" class="container_16 clearfix">
    <?php print $footer; ?>
  </div>

  <?php  print '<pre>';
    //var_dump(get_defined_vars());
    print '</pre>';
  ?>
  <?php print $closure; ?>

</body>
</html>
