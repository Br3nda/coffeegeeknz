<?php
// $Id: timelinemap.theme.inc,v 1.2 2010/07/14 14:55:16 vesnaradivojevic Exp $

/**
 * @file
 * Theme functions for timelinemap.
 */

/**
 * Implementation of template_process for views-view-timelinemap.
 */
function template_preprocess_views_view_timelinemap(&$vars) {
  $view = $vars['view'];
  if (_timelinemap_include($vars['options']['timelinemap']['theme'])) {
    // preprocess css information for the template
    //$vars['id'] = $vars['view']->name;
    $vars['class'] = $vars['view']->name; 
    $vars['div_id'] = $view->name .'-'. $view->current_display;
  }
}

/**
 * Inlude all js and css files that needs the timelinemap.
 *
 */
function _timelinemap_include($timeline_theme) {
  static $timelinemap_initialized = FALSE;
  if ($timelinemap_initialized) {
    return TRUE;
  } 
  drupal_add_js(array('timelinemap_path' => drupal_get_path('module', 'timelinemap')), 'setting');
  
  $timelinemap_mod_path = drupal_get_path('module', 'timelinemap');

  $gmap_apikey = variable_get('googlemap_api_key', '');
  $timelinemap_apikey = $gmap_apikey ? $gmap_apikey : variable_get('timelinemap_api_key', '');
  $timelinemap_gapi = 'http://maps.google.com/maps?file=api&v=2&key='. $timelinemap_apikey;
  $timelinemap_sapi = 'http://static.simile.mit.edu/timeline/api/timeline-api.js';
  drupal_set_html_head('<script type="text/javascript" src="'. $timelinemap_gapi .'"></script>');
  drupal_set_html_head('<script type="text/javascript" src="'. $timelinemap_sapi .'"></script>');
  
  drupal_set_html_head('<script src="'. check_url(url('http://js.mapbox.com/g/2/mapbox.js')) .'" type="text/javascript"></script>');

  drupal_add_css($timelinemap_mod_path . '/css/timelinemap.css');

  drupal_add_js($timelinemap_mod_path . '/js/timemap.js');
  drupal_add_js($timelinemap_mod_path . '/js/timelinemap.js');
  
  $timelinemap_initialized = TRUE;
  return TRUE;
}