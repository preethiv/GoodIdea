<?php
// $Id: freelinking_oneinchframe.inc,v 1.1.2.3 2009/11/19 07:38:54 grayside Exp $
/**
 * one inch frame plugin for freelinking
 *
 * @file
 * Allows for a link like [[oif:Mac OS X]] to be expanded to
 * oif.eafarris.com/wiki/mac-os-x
 */

$freelinking['one inch frame'] = array(
  'indicator' => '/oif/',
  'translate' => array(' ' => '-'),
  'replacement' => 'http://oif.eafarris.com/wiki/%1',
  'tip' => t('Link to a wiki page at oif.eaffaris.com'),
);
