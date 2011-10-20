<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>

<body class="<?php print $classes; ?>">                

<div id="header" class="container_12 clearfix">

<?php print $header; ?>

<div id="logo" class="grid_6"><img src="<?php print base_path() . path_to_theme(); ?>/images/logo.png" /></div>

<div id="top-corner-bar">
<?php if($user->uid): ?>
<span id="user-top-text-left">
Welcome <a href="<?php print url("user/".$user->uid);?>"><?php print $user->name;?></a>
</span><span id="user-top-text-right">
<a href="<?php print url("user/".$user->uid);?>">My Account</a> | <a href="<?php print url("logout");?>">Logout</a></span>
<?php else: ?>
<span id="anon-top-text">
<a href="<?php print url('user/login');?>">Log In</a> | 
<?php  print theme_menu_item_link(menu_link_load(1302));?>
</span>
<?php endif; ?>
</div>

<div id="top-menu" class="container_12 clearfix">
<?php if ($primary_links): ?>
<?php print theme(array('links__system_main_menu', 'links'), $primary_links,
            array(
              'id' => 'top-menu',
              'class' => 'clearfix',
            ));
?>
<?php endif; ?>


</div>

<div id="under-menu" class="container_12 clearfix">
<div id="tagline" class="grid_12">
<p>Davis source is the fun and easy way to find reviews and talk about the great - and not so great - in your area.
</p>
</div>
</div>

<div id="search-area" class="container_12 clearfix">
<div id="inside-search" class="grid_12">
<div id="search-form">
<form><label>Search:</label><input type="text" size=30></input>&nbsp;&nbsp;&nbsp;&nbsp;<input type="submit" Value="Search"></input> </form>
</div>
</div>

</div>
</div>

                

<div id="content" class="container_12">

<!--  <div class="contentTopRound" class="container_12">&nbsp;</div> --><!--Top Round: see comments on this in dev log-->

<div class="inner-content clearfix" class="container_12">
hello
<div id="main" class="grid_9 clearfix">
<?php print $content; ?>
</div>

<div id="right-sidebar" class="grid_3 clearfix">

<?php print $sidebar_right; ?>

</div>

</div> <!-- End #inner-content -->

<!--  <div class="contentBottomRound" class="container_12">&nbsp;</div>-->
</div> <!-- End #content -->
                

<div id="footer" class="container_12">
<?php print $footer; ?>
</div>

 <?php print $closure; ?>
 
</body>
</html>
