$Id: README.txt,v 1.1.4.1.2.2 2009/06/26 12:07:37 milesgillham Exp $

Simple Text-based classified ads module for Drupal.

Original author: Michael Curry, Exodus Development, Inc.
  exodusdev@gmail.com http://exodusdev.com

Maintainer: Miles Gillham (milesgillham)

This module creates a simple textual classified ad node type
(ed_classified), and the following features:

 - Automatic expiration (on expiration, node is moved to unpublished state.)
 - Automatic purge of expired ads (on purge, unpublished ads are deleted.)
 - Simple taxonomy-based browsing similar to that provided by image.module galleries.
 - Several useful blocks (latest ads, most popular, and stats).  
 - Per-user classified ads lists (under the user's profile, at /user/n/ed_classified).
 - Admin: list of all classified ads, sortable by expiration date and other columns,
   at admin/content/node/ed_classified.

NOTE: This version supports MySQL database - postgres may work but your mileage may
vary.
