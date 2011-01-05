Apture

INTRO
=====

This module integrates the Apture into your Drupal installation. 

Apture allows for easily searching and inserting rich media embeds or links on your site.
With Apture enabled links, you will be able to keep more visitors on your site because the
links do not take users to an another page.

To link text or insert embeds using apture, just click the Apture button on your drupal
editing page.

Apture works with a large number of Rich Text Editors, including (but not limited to):
TinyMCE, FCKEditor, YUI Editor


INSTALLATION
============

1) Place this module directory into your Drupal modules directory.

2) Enable the Apture module in Drupal at:   
   administration -> site configuration -> modules (admin/build/modules)

3) To enable Rich Media Embeds with apture, you'll need to configure your drupal filters to allow full HTML tags:
   Administer > input formats > configure > configure filters (admin/settings/filters) 
   
   You may also need to update the profile(s) you've previously setup for your editors 
   to prevent source formatting 
   so that it does not strip out apture html attributes:   
   
4) Create a new content or edit an old content using Drupal. 

5) Click the Apture Setup Button on your editing page to complete the remaining installation steps.

6) After installation, click the Apture Link or Embed Button to bring up the Apture Media Hub
   where you can search for media to link to or embed in your posts.  
   
   To create a Link, first highlight a text to link to, then click the Apture Link Button
   To create an Embed, place the cursor where the embed should go, then click the Apture Embed Button
   

ADDITIONAL NOTES
================

During Installation, Apture will generate a unique site token associated with your site. 
You should've been able to click a button to automatically save that token in your drupal
database. If you ever need to change your Apture site token (for e.g. because you've created 
a new site directly on apture.com), you may can do so at:
 
Administer -> Site configuration -> Apture (admin/settings/apture)


For users of the TinyMCE module, you may also need to do the following:

a) The Advanced Link plugin for TinyMCE does not allow 'id' attribute within 
   anchor tags or links, unless you add it to plugin_req.php on the following line. 
   If you have enabled the 'advlink' plugin/button in your tinymce profile,
   then you'll need to either disable it or:

   -Edit [your_drupal_install_directory]/modules/tinymce/plugin_reg.php
   -Find the line containing "$plugins['advlink']['extended_valid_elements']"
   -Add the 'id' tag to  to "a[id|...]"
        
b) Also, if you have previously modified the tinymce plugin's registeration component 
   (modules/tinymce/plugin_reg.php) to filter out certain tags or attributes from tags, 
   you'll need to make sure that anchor or link ('a') tags are allowed to have 
   the 'id' and 'style' attributes.


CREDITS
========
Written by
  - apture <contact at apture dot com>
  - Irakli Nadareshvili <irakli at phase2technology dot com>
  - Frank Febbraro <frank at phase2technology dot com>